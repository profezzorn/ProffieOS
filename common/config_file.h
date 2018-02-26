#ifndef COMMON_CONFIG_FILE_H
#define COMMON_CONFIG_FILE_H

#include "file_reader.h"

// Reads an config file, looking for variable assignments.
// TODO(hubbe): Read config files from serialflash.
struct ConfigFile {
  void skipwhite(FileReader* f) {
    while (f->Peek() == ' ' || f->Peek() == '\t')
      f->Read();
  }
  void skipline(FileReader* f) {
    while (f->Available() && f->Read() != '\n');
  }

  int64_t readIntValue(FileReader* f) {
    int64_t ret = 0;
    int64_t sign = 1;
    if (f->Peek() == '-') {
      sign = -1;
      f->Read();
    }
    while (f->Available()) {
      int c = toLower(f->Peek());
      if (c >= '0' && c <= '9') {
        ret = (c - '0') + 10 * ret;
        f->Read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  float readFloatValue(FileReader* f) {
    float ret = 0.0;
    float sign = 1.0;
    float mult = 1.0;
    if (f->Peek() == '-') {
      sign = -1.0;
      f->Read();
    }
    while (f->Available()) {
      int c = toLower(f->Peek());
      if (c >= '0' && c <= '9') {
        if (mult == 1.0) {
          ret = (c - '0') + 10 * ret;
        } else {
          ret += (c - '0') * mult;
          mult /= 10.0;
        }
        f->Read();
      } else if (c == '.') {
        if (mult != 1.0) return ret * sign;
        // Time to read decimals.
        mult /= 10.0;
        f->Read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  void Read(FileReader* f) {
    SetVariable("=", 0.0);  // This resets all variables.
    for (; f->Available(); skipline(f)) {
      char variable[33];
      variable[0] = 0;
      skipwhite(f);
      if (f->Peek() == '#') continue;
      for (int i = 0; i < 32; i++) {
        int c = toLower(f->Peek());
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
          f->Read();
          variable[i] = c;
          variable[i+1] = 0;
        } else {
          break;
        }
      }
      skipwhite(f);
      if (f->Peek() != '=') continue;
      f->Read();
      skipwhite(f);
      float v = readFloatValue(f);
#if 0
      STDOUT.print(variable);
      STDOUT.print(" = ");
      STDOUT.println(v);
#endif

      SetVariable(variable, v);
    }
  }

  virtual void SetVariable(const char* variable, float v) = 0;

  void Read(const char *filename) {
    FileReader f;
    if (f.Open(filename)) {
      Read(&f);
      f.Close();
    }
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
