#ifndef COMMON_CONFIG_FILE_H
#define COMMON_CONFIG_FILE_H

// Reads an config file, looking for variable assignments.
struct ConfigFile {
#ifdef ENABLE_SD
  void skipwhite(File* f) {
    while (f->peek() == ' ' || f->peek() == '\t')
      f->read();
  }
  void skipline(File* f) {
    while (f->available() && f->read() != '\n');
  }

  int64_t readIntValue(File* f) {
    int64_t ret = 0;
    int64_t sign = 1;
    if (f->peek() == '-') {
      sign = -1;
      f->read();
    }
    while (f->available()) {
      int c = toLower(f->peek());
      if (c >= '0' && c <= '9') {
        ret = (c - '0') + 10 * ret;
        f->read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  float readFloatValue(File* f) {
    float ret = 0.0;
    float sign = 1.0;
    float mult = 1.0;
    if (f->peek() == '-') {
      sign = -1.0;
      f->read();
    }
    while (f->available()) {
      int c = toLower(f->peek());
      if (c >= '0' && c <= '9') {
        if (mult == 1.0) {
          ret = (c - '0') + 10 * ret;
        } else {
          ret += (c - '0') * mult;
          mult /= 10.0;
        }
        f->read();
      } else if (c == '.') {
        if (mult != 1.0) return ret * sign;
        // Time to read decimals.
        mult /= 10.0;
        f->read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  void Read(File* f) {
    SetVariable("=", 0.0);  // This resets all variables.
    for (; f->available(); skipline(f)) {
      char variable[33];
      variable[0] = 0;
      skipwhite(f);
      if (f->peek() == '#') continue;
      for (int i = 0; i < 32; i++) {
        int c = toLower(f->peek());
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
          f->read();
          variable[i] = c;
          variable[i+1] = 0;
        } else {
          break;
        }
      }
      skipwhite(f);
      if (f->peek() != '=') continue;
      f->read();
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
#endif
  virtual void SetVariable(const char* variable, float v) = 0;

  void Read(const char *filename) {
#ifdef ENABLE_SD
    File f = LSFS::Open(filename);
    Read(&f);
    f.close();
#endif
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
