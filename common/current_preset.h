#ifndef COMMON_CURRENT_PRESET_H
#define COMMON_CURRENT_PRESET_H

#include "preset.h"
#include "file_reader.h"

class CurrentPreset {
public:
  enum { PRESET_DISK, PRESET_ROM } preset_type;
  int preset_num;
  LSPtr<char> font;
  LSPtr<char> track;
#define DEFINE_CURRENT_STYLE_STRING(N) LSPtr<char> current_style##N;
  ONCEPERBLADE(DEFINE_CURRENT_STYLE_STRING);
  LSPtr<char> name;

  #if 0
  void InitStyles() {
#define  INIT_CURRENT_STYLE(N) current_config->blade##N->SetStyle(StyleParser::Parse(current_style##N.get()));
    ONCEPERBLADE(INIT_CURRENT_STYLE)
  }
  #endif

  char *mk_builtin_str(int num, int N) {
    char tmp[30];
    strcpy(tmp, "$");
    itoa(num, tmp + strlen(tmp), 10);
    strcat(tmp, ",");
    itoa(num, tmp + strlen(tmp), 10);
    char *ret = (char *)malloc(strlen(tmp)+1);
    strcpy(ret, tmp);
    return ret;
  }

  void Set(int num) {
    num = (current_config->num_presets + num) % current_config->num_presets;
    Preset* preset = current_config->presets + num;
    preset_type = PRESET_ROM;
    preset_num = num;
    font = preset->font;
    track = preset->track;
#define MAKE_STYLE_STRING(N) current_style##N = mk_builtin_str(num, N);
    ONCEPERBLADE(MAKE_STYLE_STRING);
    name = preset->name;
  }

  bool Read(FileReader* f) {
    int preset_count = 0;
    int current_style = 0;
    if (f->Tell() == 0) preset_num = -1;
    preset_type = PRESET_DISK;
    
    for (; f->Available(); f->skipline()) {
      char variable[33];
      f->skipwhite();
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
      if (!preset_count) return false;
      if (f->Peek() != '=') continue;
      f->Read();
      f->skipwhite();

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
      if (!strcmp(variable, "style")) {
	current_style++;
	char* tmp = f->readString();
#define SET_PRESET_STYLE(N) if (current_style == N) { current_style##N = tmp; tmp = 0; }
	ONCEPERBLADE(SET_PRESET_STYLE);
	if (tmp) free(tmp);
      }
    }
    if (preset_count == 1) {
      preset_num++;
      return true;
    }
    return false;
  }

  bool Write(FileReader* f) {
    f->Write("new_preset\n");
    f->write_key_value("font", font.get());
    f->write_key_value("track", track.get());
#define WRITE_PRESET_STYLE(N) f->write_key_value("style", current_style##N.get());
    ONCEPERBLADE(WRITE_PRESET_STYLE);
    f->write_key_value("name", name.get());
    return true;
  }

  static bool isSpace(int c) {
    return c == '\n' || c == '\r' || c == ' ' || c == '\t';
  }

  bool OpenPresets(FileReader* f, const char* filename) {
    if (!f->Open(filename))
      return false;

    if (f->Tell() < 4) return false;
    int p = f->Tell() - 1;
    while (p > 0) { f->Seek(p); if (!isSpace(f->Read())) break; p--; }
    f->Seek(p - 4);
    if (!isSpace(f->Read())) return false;
    if (f->Read() != 'e') return false;
    if (f->Read() != 'n') return false;
    if (f->Read() != 'd') return false;
    f->Seek(0);

    return true;
  }

  bool UpdateINI() {
    FileReader f, f2;
    if (OpenPresets(&f2, "presets.tmp")) {
      // Found valid tmp file
      LSFS::Remove("presets.ini");
      f.Create("presets.ini");
      while (f2.Available()) f.Write(f2.Read());
      f2.Close();
      f.Close();
      return true;
    }
    return false;
  }

  bool CreateINI() {
    FileReader f;
    f.Create("presets.ini");
    CurrentPreset tmp;
    for (size_t i = 0; i < current_config->num_presets; i++) {
      tmp.Set(i);
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

  void SaveAt(int position) {
    FileReader f, out;
    if (!OpenPresets(&f, "presets.ini")) {
      if (!UpdateINI()) return false;
      if (!OpenPresets(&f, "presets.ini")) CreateINI();
      OpenPresets(&f, "presets.ini");
    }
    LSFS::Remove("presets.tmp");
    out.Create("presets.tmp");
    CurrentPreset tmp;
    int n = 0;
    while (tmp.Read(&f)) {
      n++;
      if (n == position) Write(&out);
      if (n != preset_num) tmp.Write(&out);
    }
    if (n + 1 == position) Write(&out);
    f.Close();
    out.Write("end\n");
    out.Close();
    UpdateINI();
  }



  void SetPreset(int preset) {
    LOCK_SD(true);
    if (!Load(preset)) Set(preset);
    LOCK_SD(false);
  }
};

#endif
