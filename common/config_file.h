#ifndef COMMON_CONFIG_FILE_H
#define COMMON_CONFIG_FILE_H

#include "file_reader.h"

// Reads an config file, looking for variable assignments.
// TODO(hubbe): Read config files from serialflash.
struct ConfigFile {
  bool Read(FileReader* f) {
    SetVariable("=", 0.0);  // This resets all variables.
    if (!f || !f->IsOpen()) return false;
    for (; f->Available(); f->skipline()) {
      char variable[33];
      variable[0] = 0;
      f->skipwhite();
      if (f->Peek() == '#') continue;
      f->readVariable(variable);
      f->skipwhite();
      if (f->Peek() != '=') continue;
      f->Read();
      f->skipwhite();
      float v = f->readFloatValue();
#if 0
      STDOUT.print(variable);
      STDOUT.print(" = ");
      STDOUT.println(v);
#endif

      SetVariable(variable, v);
    }
    return true;
  }

  virtual void SetVariable(const char* variable, float v) = 0;

  bool Read(const char *filename) {
    LOCK_SD(true);
    bool read = false;
    FileReader f;
    f.Open(filename);
    if (Read(&f)) {
      read = true;
    }
    f.Close();
    LOCK_SD(false);
    return read;
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
    char full_name[128];
    strcpy(full_name, current_directory);
    strcat(full_name, name);
    Read(full_name);
  }
};

#endif
