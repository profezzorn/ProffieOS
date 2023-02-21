#ifndef COMMON_CURRENT_PRESET_H
#define COMMON_CURRENT_PRESET_H

#include "preset.h"
#include "file_reader.h"
#include "blade_config.h"

class CurrentPreset {
public:
  enum { PRESET_DISK, PRESET_ROM } preset_type;
  int preset_num;
  ConfigFileExt read_from_ext_ = ConfigFileExt::CONFIG_UNKNOWN;
  uint32_t iteration_ = 0;
  LSPtr<char> font;
  LSPtr<char> track;
#if NUM_BLADES > 0
  LSPtr<char> current_style_[NUM_BLADES];
#endif  
  LSPtr<char> name;
  uint32_t variation;

  const char *mk_builtin_str(int num, int N) {
    char tmp[30];
    strcpy(tmp, "builtin ");
    itoa(num, tmp + strlen(tmp), 10);
    strcat(tmp, " ");
    itoa(N, tmp + strlen(tmp), 10);
    return mkstr(tmp);
  }

  const char *mk_preset_name(int num) {
    char tmp[30];
    strcpy(tmp, "Preset: ");
    itoa(num + 1, tmp + strlen(tmp), 10);
    return mkstr(tmp);
  }

  static bool IsValidStyleString(const char* s) {
    if (strlen(s) < 5) return false;
    for (; *s != ' '; s++) {
      if (*s >= 'a' && *s <= 'z') continue;
      return false;
    }
    for (; *s; s++) {
      if (*s >= '0' && *s <= '9') continue;
      if (*s == ' ') continue;
      if (*s == ',') continue;
      return false;
    }
    return true;
  }

  static const char* ValidateStyleStringF(const char* s, const char* file, int line) {
    if (!IsValidStyleString(s)) {
      while (true) {
	STDOUT << "INVALID STYLE STRING " << s << " @ " << file << ":" << line << "\n";
      }
    }
    return s;
  }

  static const LSPtr<char> ValidateStyleStringF(LSPtr<char> s, const char* file, int line) {
    if (!IsValidStyleString(s.get())) {
      while (true) {
	STDOUT << "INVALID STYLE STRING " << s.get() << " @ " << file << ":" << line << "\n";
      }
    }
    return s;
  }

#define VALIDATE_STYLE_STRING(N) ValidateStyleString(PRE.current_style_[N-1].get());    

#if defined(ENABLE_DEBUG) && NUM_BLADES > 0
#define ValidateStyleString(X) CurrentPreset::ValidateStyleStringF((X), __FILE__, __LINE__)
#define VSS(X) CurrentPreset::ValidateStyleStringF((X), __FILE__, __LINE__)
#define DOVALIDATE(X) do { CurrentPreset&PRE=(X); ONCEPERBLADE(VALIDATE_STYLE_STRING); } while(0)
#else
#define ValidateStyleString(X) (X)
#define VSS(X) (X)
#define DOVALIDATE(X) do {  } while(0)
#endif

  void Clear() {
    font = "";
    track = "";
#if NUM_BLADES > 0    
    for (size_t N = 0; N < NUM_BLADES; N++) current_style_[N] = "";
#endif    
    name = "";
    variation = 0;
  }

  void Set(int num) {
    num = (current_config->num_presets + num) % current_config->num_presets;
    Preset* preset = current_config->presets + num;
    preset_type = PRESET_ROM;
    preset_num = num;
    font = preset->font;
    track = preset->track;
#define MAKE_STYLE_STRING(N) current_style_[N-1] = ValidateStyleString(mk_builtin_str(num, N));
    ONCEPERBLADE(MAKE_STYLE_STRING);
// TODO Test with 3+ blades
//#if NUM_BLADES > 0    
//    for (size_t N = 0; N < NUM_BLADES; N++) current_style_[N] = ValidateStyleString(mk_builtin_str(num, N+1));
//#endif    
    if (preset->name && strlen(preset->name)) {
      name = preset->name;
    } else {
      name = mk_preset_name(num);
    }
    variation = 0;
  }

  bool Read(FileReader* f) {
    int preset_count = 0;
    int current_style = 0;
    if (f->Tell() <= sizeof(install_time) + 11) preset_num = -1;
    preset_type = PRESET_DISK;
    
    for (; f->Available(); f->skipline()) {
      char variable[33];
      f->skipspace();
      if (f->Peek() == '#') continue;
      int line_begin = f->Tell();
      f->readVariable(variable);

      //      fprintf(stderr, "VAR: %s\n", variable);

      if (!variable[0]) continue;

      if (!strcmp(variable, "new_preset")) {
	preset_count++;
	if (preset_count == 2) {
	  preset_num++;
	  // Go back to the beginning of the line
	  f->Seek(line_begin);
	  return true;
	}
	continue;
      }

      if (!strcmp(variable, "end")) {
	f->Seek(line_begin);
	if (preset_count == 0) break;
	if (preset_count == 1) {
	  preset_num++;
	  return true;
	}
	return false;
      }
      
      if (!preset_count) continue;
      if (f->Peek() != '=') continue;
      f->Read();
      f->skipspace();

      if (!strcmp(variable, "name")) {
	name = f->readString();
	continue;
      }
      if (!strcmp(variable, "font")) {
	font = f->readString();
	continue;
      }
      if (!strcmp(variable, "track")) {
	track = f->readString();
	continue;
      }
      if (!strcmp(variable, "variation")) {
	char *tmp = f->readString();
	if (tmp) {
	  variation = strtol(tmp, nullptr, 10);
	  if (tmp) free(tmp);
	}
	continue;
      }
      if (!strcmp(variable, "style")) {
	char* tmp = f->readString();
	(void)ValidateStyleString(tmp);
#if NUM_BLADES > 0
	current_style_[current_style] = tmp;
#else
	free(tmp);
#endif
	current_style++;
	continue;
      }
    }
    if (preset_count == 1) {
      preset_num++;
      return true;
    }
    return false;
  }

  bool Write(BufferedFileWriter* f) {
    DOVALIDATE(*this);
    f->Write("new_preset\n");
    DOVALIDATE(*this);
    f->write_key_value("font", font.get());
    DOVALIDATE(*this);
    f->write_key_value("track", track.get());
    DOVALIDATE(*this);
//#define WRITE_PRESET_STYLE(N) f->write_key_value("style", ValidateStyleString(current_style_[N-1].get()));
//    ONCEPERBLADE(WRITE_PRESET_STYLE);
#if NUM_BLADES > 0    
    for (size_t N = 0; N < NUM_BLADES; N++) {
      f->write_key_value("style", ValidateStyleString(current_style_[N].get()));
    }
#endif    
    f->write_key_value("name", name.get());
    char tmp[12];
    itoa(variation, tmp, 10);
    f->write_key_value("variation", tmp);
    return true;
  }

  void Print() {
    PrintQuotedValue("FONT", font.get());
    PrintQuotedValue("TRACK", track.get());
#define PRINT_PRESET_STYLE(N) PrintQuotedValue("STYLE" #N, ValidateStyleString(current_style_[N-1].get()));
    ONCEPERBLADE(PRINT_PRESET_STYLE);
    PrintQuotedValue("NAME", name.get());
    STDOUT << "VARIATION=" << variation << "\n";
  }

  static bool isSpace(int c) {
    return c == '\n' || c == '\r' || c == ' ' || c == '\t';
  }

  bool ValidatePresets(FileReader* f) {
    if (f->FileSize() < 4) return false;
    int pos = 0;
#ifndef KEEP_SAVEFILES_WHEN_PROGRAMMING
    char variable[33];
    f->readVariable(variable);
    if (strcmp(variable, "installed")) return false;
    if (f->Read() != '=') return false;
    if (!f->Expect(install_time)) return false;
    if (f->Read() != '\n') return false;
    pos = f->Tell();
#endif

    int p = f->FileSize() - 1;
    while (p > 0) { f->Seek(p); if (!isSpace(f->Read())) break; p--; }
    f->Seek(p - 3);
    if (!isSpace(f->Read())) return false;
    if (toLower(f->Read()) != 'e') return false;
    if (toLower(f->Read()) != 'n') return false;
    if (toLower(f->Read()) != 'd') return false;
    f->Seek(pos);

    return true;
  }

  bool TryValidator(FileValidator* a) {
    if (!a->validHeader()) return false;
    if (!a->validateChecksum()) return false;
    read_from_ext_ = a->ext;
    iteration_ = a->iteration();
    return true;
  }

  bool TryPlain(FileValidator* a) {
    a->f.Seek(0);
    if (!ValidatePresets(& a->f)) return false;
    read_from_ext_ = a->ext;
    iteration_ = 0;
    return true;
  }

  FileReader *OpenPresets2(FileSelector* fs) {
    if (iteration_) {
      if (read_from_ext_ == ConfigFileExt::CONFIG_INI) {
	return & fs->ini.f;
      } else if(read_from_ext_ == ConfigFileExt::CONFIG_TMP) {
	return & fs->tmp.f;
      }
    }
    if (TryValidator(fs->a)) return & fs->a->f;
    if (TryValidator(fs->b)) return & fs->b->f;
    if (TryPlain(&fs->ini)) return & fs->ini.f;
    if (TryPlain(&fs->tmp)) return & fs->tmp.f;
    return nullptr;
  }

  FileReader *OpenPresets(FileSelector* fs) {
    FileReader* ret = OpenPresets2(fs);
    if (read_from_ext_ == ConfigFileExt::CONFIG_INI) {
      fs->tmp.f.Close();
    }
    if (read_from_ext_ == ConfigFileExt::CONFIG_TMP) {
      fs->ini.f.Close();
    }
    return ret;
  }

  bool CreateINI() {
#ifndef ENABLE_SD
    return false;
#else
    PathHelper ini_fn(GetSaveDir(), "presets.ini");
    BufferedFileWriter f(ini_fn);
    f.write_key_value("installed", install_time);
    CurrentPreset tmp;
    for (size_t i = 0; i < current_config->num_presets; i++) {
      tmp.Set(i);
      DOVALIDATE(tmp);
      tmp.Write(&f);
    }
    f.Write("end\n");
    f.Close(++iteration_);
    return true;
#endif    
  }

  // preset = -1 means to load the *last* pre
  bool Load(int preset) {
#ifndef ENABLE_SD
    return false;
#else
    FileSelector fs(GetSaveDir(), "presets");
    FileReader* f = OpenPresets(&fs);
    if (!f) return false;
    int start = f->Tell();
    int n = 0;
    preset_num = -1;
    while (true) {
      if (Read(f)) {
	if (n == preset) return true;
	n++;
      } else {
	if (n && preset == -1) return true;
	if (n == preset) {
	  f->Seek(start);
	  n=0;
	  preset_num = -1;
	  Read(f);
	  return true;
	}
	return false;
      }
    }
#endif    
  }

  void SaveAtLocked(int position) {
#ifdef ENABLE_SD

    DOVALIDATE(*this);
    FileSelector fs(GetSaveDir(), "presets");
    FileReader *f = OpenPresets(&fs);
    if (!f) {
      fs.Close();
      CreateINI();
      SaveAtLocked(position);
      return;
    }
    DOVALIDATE(*this);

    if (read_from_ext_ == ConfigFileExt::CONFIG_INI) {
      read_from_ext_ = ConfigFileExt::CONFIG_TMP;
    } else {
      read_from_ext_ = ConfigFileExt::CONFIG_INI;
    }
    iteration_++;

    PathHelper tmp_fn(GetSaveDir(), "presets", read_from_ext_ == ConfigFileExt::CONFIG_INI ? "ini" : "tmp");
    STDERR << "Creating file " << tmp_fn << " iteration = " << iteration_ << "\n";

    BufferedFileWriter out(tmp_fn);
    DOVALIDATE(*this);
    out.write_key_value("installed", install_time);
    CurrentPreset tmp;
    int opos = 0;
    if (position == 0) {
      DOVALIDATE(*this);
      Write(&out);
      opos++;
    }
    tmp.preset_num = -1;
    while (tmp.Read(f)) {
      if (tmp.preset_num != preset_num) {
	DOVALIDATE(tmp);
	tmp.Write(&out);
	opos++;
	if (position == opos) {
	  DOVALIDATE(*this);
	  Write(&out);
	  opos++;
	}
      }
    }
    fs.Close();
    out.Write("end\n");
    out.Close(iteration_);
    preset_num = position;
#endif    
  }

  // position = 0 -> first spot
  // position = N -> last
  // position = -1 -> delete
  // To duplicate, set preset_num to -1
  void SaveAt(int position) {
    LOCK_SD(true);
    SaveAtLocked(position);
    LOCK_SD(false);
  }

  void Save() { SaveAt(preset_num); }

  void SetPreset(int preset) {
    Clear();
    LOCK_SD(true);
    if (!Load(preset)) Set(preset);
    LOCK_SD(false);
  }

void SetStyle(int blade, LSPtr<char> style) {
  DOVALIDATE(*this);
  ValidateStyleString(style.get());
#if NUM_BLADES > 0
  current_style_[blade-1] = std::move(style);
#endif  
  DOVALIDATE(*this);
}

const char* GetStyle(int blade) {
#if NUM_BLADES > 0
  return current_style_[blade-1].get();
#else
  return "";
#endif  
}

};

#endif
