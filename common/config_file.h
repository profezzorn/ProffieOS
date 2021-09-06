#ifndef COMMON_CONFIG_FILE_H
#define COMMON_CONFIG_FILE_H

#include "file_reader.h"

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
  virtual ReadStatus Read(FileReader* f) {
    SetVariable("=", 0.0);  // This resets all variables.
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

  ReadStatus Read(const char *filename) {
    LOCK_SD(true);
    FileReader f;
    f.Open(filename);
    ReadStatus ret = Read(&f);
    f.Close();
    LOCK_SD(false);
    return ret;
  }

#define CONFIG_VARIABLE(X, DEF) do {            \
    if (variable[0] == '=') X = DEF;            \
    else if (!strcasecmp(variable, #X)) {       \
      X = v;                                    \
      STDOUT.print(variable);                   \
      STDOUT.print("=");                        \
      STDOUT.println(v);                        \
      return;                                   \
    }                                           \
} while(0)

#define CONFIG_VARIABLE2(X, DEF) DoVariableOp<decltype(X)>(op, #X, X, DEF)

  void ReadInCurrentDir(const char* name) {
    // Search through all the directories.
    for (const char* dir = current_directory; dir; dir = next_current_directory(dir)) {
      PathHelper full_name(dir, name);
      if (Read(full_name) != ReadStatus::READ_FAIL)
	return;
    }
  }

  ReadStatus ReadINIFromDir(const char *dir, const char* basename) {
    PathHelper full_name(dir, basename, "ini");
    if (Read(full_name) == ReadStatus::READ_END) {
      return ReadStatus::READ_END;
    }
    full_name.Set(dir, basename, "tmp");
    return Read(full_name);
  }
  ReadStatus ReadINIFromSaveDir(const char* basename) {
    PathHelper full_name(GetSaveDir(), basename, "ini");
    if (Read(full_name) == ReadStatus::READ_END) {
      return ReadStatus::READ_END;
    }
    full_name.Set(GetSaveDir(), basename, "tmp");
    return Read(full_name);
  }
};


#endif
