#ifndef COMMON_CURRENT_PRESET_H
#define COMMON_CURRENT_PRESET_H

#include "preset.h"
#include "file_reader.h"
#include "blade_config.h"

class CurrentPreset {
public:
  enum { PRESET_DISK, PRESET_ROM } preset_type;
  int preset_num;
  LSPtr<char> font;
  LSPtr<char> track;
#define DEFINE_CURRENT_STYLE_STRING(N) LSPtr<char> current_style##N;
  ONCEPERBLADE(DEFINE_CURRENT_STYLE_STRING);
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

#define VALIDATE_STYLE_STRING(N) ValidateStyleString(PRE.current_style##N.get());    

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
#define CLEAR_STYLE_STRING(N) current_style##N = "";
    ONCEPERBLADE(CLEAR_STYLE_STRING);
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
#define MAKE_STYLE_STRING(N) current_style##N = ValidateStyleString(mk_builtin_str(num, N));
    ONCEPERBLADE(MAKE_STYLE_STRING);
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
      if (!preset_count) continue;
      if (f->Peek() != '=') continue;
      f->Read();
      f->skipspace();

      if (!strcmp(variable, "end")) {
	f->Seek(line_begin);
	if (preset_count == 1) {
	  preset_num++;
	  return true;
	}
	return false;
      }
      
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
	current_style++;
	char* tmp = f->readString();
	(void)ValidateStyleString(tmp);
#define SET_PRESET_STYLE(N) if (current_style == N) { current_style##N = tmp; tmp = 0; }
	ONCEPERBLADE(SET_PRESET_STYLE);
	if (tmp) free(tmp);
	continue;
      }
    }
    if (preset_count == 1) {
      preset_num++;
      return true;
    }
    return false;
  }

  bool Write(FileReader* f) {
    DOVALIDATE(*this);
    f->Write("new_preset\n");
    DOVALIDATE(*this);
    f->write_key_value("font", font.get());
    DOVALIDATE(*this);
    f->write_key_value("track", track.get());
    DOVALIDATE(*this);
#define WRITE_PRESET_STYLE(N) f->write_key_value("style", ValidateStyleString(current_style##N.get()));
    ONCEPERBLADE(WRITE_PRESET_STYLE);
    f->write_key_value("name", name.get());
    char tmp[12];
    itoa(variation, tmp, 10);
    f->write_key_value("variation", tmp);
    return true;
  }

  void Print() {
    PrintQuotedValue("FONT", font.get());
    PrintQuotedValue("TRACK", track.get());
#define PRINT_PRESET_STYLE(N) PrintQuotedValue("STYLE" #N, ValidateStyleString(current_style##N.get()));
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

  bool OpenPresets(FileReader* f, const char* filename) {
    PathHelper fn(GetSaveDir(), filename);
    if (!f->Open(fn)) {
      STDOUT << "Failed to open: " << filename << "\n";
      return false;
    }
    if (ValidatePresets(f)) {
      return true;
    } else {
      f->Close();
      return false;
    }
  }

  bool UpdateINI() {
    FileReader f, f2;
    PathHelper ini_fn(GetSaveDir(), "presets.ini");
    if (OpenPresets(&f2, "presets.tmp")) {
      uint8_t buf[512];
      // Found valid tmp file
      f2.Seek(0);
      LSFS::Remove(ini_fn);
      f.Create(ini_fn);
      while (f2.Available()) {
	int to_copy = std::min<int>(f2.Available(), sizeof(buf));
	if (f2.Read(buf, to_copy) != to_copy ||
	    f.Write(buf, to_copy) != to_copy) {
	  f2.Close();
	  f.Close();
	  LSFS::Remove(ini_fn);
	  return false;
	}
      }
      f2.Close();
      f.Close();
      return true;
    }
    return false;
  }

  bool CreateINI() {
    FileReader f;
    PathHelper ini_fn(GetSaveDir(), "presets.ini");
    LSFS::Remove(ini_fn);
    if (!f.Create(ini_fn)) {
      STDOUT << "Failed to open " << ini_fn << " for write\n";
      return false;
    }
    f.write_key_value("installed", install_time);
    CurrentPreset tmp;
    for (size_t i = 0; i < current_config->num_presets; i++) {
      tmp.Set(i);
      DOVALIDATE(tmp);
      tmp.Write(&f);
    }
    f.Write("end\n");
    f.Close();
    return true;
  }

  // preset = -1 means to load the *last* pre
  bool Load(int preset) {
    FileReader f;
    if (!OpenPresets(&f, "presets.ini")) {
      if (!UpdateINI()) return false;
      if (!OpenPresets(&f, "presets.ini")) return false;
    }
    
    int n = 0;
    while (true) {
      if (Read(&f)) {
	if (n == preset) return true;
	n++;
      } else {
	if (n && preset == -1) return true;
	if (n == preset) {
	  f.Seek(0);
	  n=0;
	  Read(&f);
	  return true;
	}
	return false;
      }
    }
  }

  void SaveAtLocked(int position) {
    DOVALIDATE(*this);
    FileReader f, out;
    if (!OpenPresets(&f, "presets.ini")) {
      if (!UpdateINI()) CreateINI();
      if (!OpenPresets(&f, "presets.ini")) {
	STDOUT << "SAVING FAILED!!!!\n";
	return;
      }
    }
    DOVALIDATE(*this);
    PathHelper tmp_fn(GetSaveDir(), "presets.tmp");
    DOVALIDATE(*this);
    LSFS::Remove(tmp_fn);
    DOVALIDATE(*this);
    out.Create(tmp_fn);
    DOVALIDATE(*this);
    out.write_key_value("installed", install_time);
    CurrentPreset tmp;
    int opos = 0;
    if (position == 0) {
      DOVALIDATE(*this);
      Write(&out);
      opos++;
    }
    while (tmp.Read(&f)) {
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
    f.Close();
    out.Write("end\n");
    out.Close();
    UpdateINI();
    preset_num = position;
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
#define SET_STYLE_N(N) case N: current_style##N = std::move(style); break;
  switch (blade) {
    ONCEPERBLADE(SET_STYLE_N)
  }
  DOVALIDATE(*this);
}
  
const char* GetStyle(int blade) {
#define GET_STYLE_N(N) case N: return current_style##N.get();
  switch (blade) {
    ONCEPERBLADE(GET_STYLE_N)
  }
  return "";
}

};

#endif
