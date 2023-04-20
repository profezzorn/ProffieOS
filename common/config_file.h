#ifndef COMMON_CONFIG_FILE_H
#define COMMON_CONFIG_FILE_H

#include "file_reader.h"
#include "strfun.h"

// Reads an config file, looking for variable assignments.
// TODO(hubbe): Read config files from serialflash.
struct ConfigFile {

  struct VariableBase {
    virtual void set(float v) = 0;
    virtual float get() = 0;
    virtual void setDefault() = 0;
  };

  template<class T>
  struct Variable : public VariableBase {
    Variable(T& var, T def) : var_(var), def_(def) {}
    void set(float value) override { var_ = value; }
    float get() override { return var_; }
    void setDefault() override { var_ = def_; }
  private:
    T& var_;
    T def_;
  };

  struct VariableOP {
    virtual void run(const char* name, VariableBase* var) = 0;
  };

  struct SetDefaultOP : public VariableOP {
    void run(const char* name, VariableBase* var) override {
      var->setDefault();
    }
  };
  struct SetVariableOP : public VariableOP {
    SetVariableOP(const char* variable, float v) : variable_(variable), v_(v) { }
    void run(const char* name, VariableBase* var) override {
      if (!strcasecmp(name, variable_)) {
	var->set(v_);
      }
    }
  private:
    const char* variable_;
    float v_;
  };
  struct PrintVariableOP : public VariableOP {
    explicit PrintVariableOP(const char* variable) : variable_(variable) { }
    void run(const char* name, VariableBase* var) override {
      if (!strcasecmp(name, variable_)) {
	STDOUT.println(var->get());
      }
    }
  private:
    const char* variable_;
  };

  struct SaveVariableOP : public VariableOP {
    SaveVariableOP(FileReader& f) : f_(f) {}
    void run(const char* name, VariableBase* var) override {
      f_.write_key_value_float(name, var->get());
    }
  private:
    FileReader& f_;
  };

  struct BufferedSaveVariableOP : public VariableOP {
    BufferedSaveVariableOP(BufferedFileWriter& f) : f_(f) {}
    void run(const char* name, VariableBase* var) override {
      f_.write_key_value_float(name, var->get());
    }
  private:
    BufferedFileWriter& f_;
  };

  template<class T>
  void DoVariableOp(VariableOP *op, const char* name, T& ref, T def) {
    Variable<T> var(ref, def);
    op->run(name, &var);
  }

  enum class ReadStatus {
    READ_FAIL,
    READ_OK,
    READ_END,
  };
  virtual ReadStatus Read(FileReader* f, bool reset = true) {
    if (reset) SetVariable("=", 0.0);  // This resets all variables.
    if (!f || !f->IsOpen()) return ReadStatus::READ_FAIL;
    for (; f->Available(); f->skipline()) {
      char variable[33];
      variable[0] = 0;
      f->skipwhite();
      if (f->Peek() == '#') continue;
      f->readVariable(variable);
      if (!strcmp(variable,"end")) return ReadStatus::READ_END;
      f->skipwhite();
      if (f->Peek() != '=') continue;
      f->Read();
      f->skipwhite();
#ifndef KEEP_SAVEFILES_WHEN_PROGRAMMING
      if (!strcmp(variable, "installed")) {
	if (!f->Expect(install_time)) {
	  return ReadStatus::READ_FAIL;
	}
	continue;
      }
#endif
      float v = f->readFloatValue();
#if 0
      STDOUT.print(variable);
      STDOUT.print(" = ");
      STDOUT.println(v);
#endif
      SetVariable(variable, v);
    }
    return ReadStatus::READ_OK;
  }


  virtual void SetVariable(const char* variable, float v) {
    if (!strcmp(variable, "=")) {
      SetDefaultOP op;
      iterateVariables(&op);
    } else {
      SetVariableOP op(variable, v);
      iterateVariables(&op);
    }
  }

  virtual void iterateVariables(VariableOP *op) {}

  void WriteToDir(const char* dir, const char* basename, ConfigFileExt ext) {
    PathHelper full_name(dir, basename, ext == ConfigFileExt::CONFIG_INI ? "ini" : "tmp");
    LOCK_SD(true);
    BufferedFileWriter out(full_name);
    out.write_key_value("installed", install_time);
    BufferedSaveVariableOP op(out);
    iterateVariables(&op);
    out.write_key_value("end", "1");
    out.Close(++iteration_);
    LOCK_SD(false);
  }

  ConfigFileExt read_from_ext_ = ConfigFileExt::CONFIG_UNKNOWN;
  uint32_t iteration_ = 0;
  
  void WriteToDir(const char* dir, const char* basename) {
    if (read_from_ext_ == ConfigFileExt::CONFIG_INI) {
      read_from_ext_ = ConfigFileExt::CONFIG_TMP;
    } else {
      read_from_ext_ = ConfigFileExt::CONFIG_INI;
    }
    WriteToDir(dir, basename, read_from_ext_);
  }

  void WriteToRootDir(const char* basename) { WriteToDir(NULL, basename); }
  void WriteToSaveDir(const char* basename) { WriteToDir(GetSaveDir(), basename); }

  void Print(const char* variable) {
    ConfigFile::PrintVariableOP op(variable);
    iterateVariables(&op);
  }
  
  void Set(const char* var_and_value) {
    char variable[32];
    const char* nw = SkipWord(var_and_value);
    memcpy(variable, var_and_value, nw - var_and_value);
    variable[nw - var_and_value] = 0;
    ConfigFile::SetVariableOP op(variable, parsefloat(nw));
    iterateVariables(&op);
  }

  ReadStatus Read(const char *filename, bool reset = true) {
    LOCK_SD(true);
    FileReader f;
    f.Open(filename);
    ReadStatus ret = Read(&f, reset);
    f.Close();
    LOCK_SD(false);
    return ret;
  }

#define CONFIG_VARIABLE2(X, DEF) DoVariableOp<decltype(X)>(op, #X, X, DEF)

  void ReadInCurrentDir(const char* name) {
    SetVariable("=", 0.0);
    // Search through all the directories.
    for (const char* dir = last_current_directory(); dir; dir = previous_current_directory(dir)) {
      PathHelper full_name(dir, name);
      Read(full_name, false);
    }
  }

  bool TryValidator(FileValidator* a) {
    if (!a->validateChecksum()) return false;
    ReadStatus status = Read(&a->f);
    if (status != ReadStatus::READ_FAIL) {
      read_from_ext_ = a->ext;
      iteration_ = a->iteration();
    }
    return true;
  }

  bool TryPlain(FileValidator* a) {
    if (a->f.FileSize() >= MAX_CONFIG_FILE_SIZE) return false;
    a->f.Seek(0);
    if (Read(&a->f) == ReadStatus::READ_END) {
      read_from_ext_ = a->ext;
      iteration_ = 1;
    }
    return true;
  }
  ReadStatus ReadINIFromDir(const char *dir, const char* basename) {
    SetVariable("=", 0.0);
    LOCK_SD(true);
    FileSelector fs(dir, basename);
    bool success = TryValidator(fs.a) || TryValidator(fs.b);
    if (!success) success = TryPlain(&fs.ini);
    if (!success) success = TryPlain(&fs.tmp);
    fs.Close();
    LOCK_SD(false);
    return success ? ReadStatus::READ_END : ReadStatus::READ_FAIL;
  }
  ReadStatus ReadINIFromSaveDir(const char* basename) {
    return ReadINIFromDir(GetSaveDir(), basename);
  }
  ReadStatus ReadINIFromRootDir(const char* basename) {
    return ReadINIFromDir("", basename);
  }
};


#endif
