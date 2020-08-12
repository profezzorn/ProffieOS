#ifndef COMMON_CONFIG_FILE_H
#define COMMON_CONFIG_FILE_H

#include "file_reader.h"

// Reads an config file, looking for variable assignments.
// TODO(hubbe): Read config files from serialflash.
struct ConfigFile {

  enum class ReadStatus {
    READ_FAIL,
    READ_OK,
    READ_END,
  };
  ReadStatus Read(FileReader* f) {
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

  virtual void SetVariable(const char* variable, float v) = 0;

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
