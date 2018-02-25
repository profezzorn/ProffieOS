#ifndef SOUND_EFFECT_H
#define SOUND_EFFECT_H

// Effect represents a set of sound files.
// We keep track of the minimum number found, the maximum number found, weather
// there is a file with no number, and if there are leading zeroes or not.
// Basically, it means that files can be numbered any which way, as long as it
// is consistent and there are no gaps in the numbering.
//
// Note that *ALL* sounds use this Effect class to keep track of the sounds.
// This means that you can for instance have hum1.wav, hum2.wav, hum3.wav, and
// every time the hum loops, it will randomly pick one of them.
class Effect {
  public:

  // This class represents a specific effect file. It's main purpose
  // is to be smaller than using the filename to identify the file.
  class FileID {
   public:
    FileID(const Effect* effect, int file) : effect_(effect), file_(file) {}
    FileID() : effect_(nullptr), file_(0) {}

    bool operator==(const FileID& other) const {
      return other.effect_ == effect_ && file_ == other.file_;
    }

    operator bool() const { return effect_ != nullptr; }

    void GetName(char *filename) {
      effect_->GetName(filename, file_);
    }

   private:
    const Effect* effect_;
    int file_;
  };

  enum Extension {
    WAV,
    RAW,
    USL,
    UNKNOWN,
  };

  static Extension IdentifyExtension(const char* filename) {
    if (endswith(".wav", filename)) return WAV;
    if (endswith(".raw", filename)) return RAW;
    if (endswith(".usl", filename)) return USL;
    return UNKNOWN;
  }

  Effect(const char* name) : name_(name) {
    next_ = all_effects;
    all_effects = this;
    reset();
  }

  void reset() {
    min_file_ = 20000;
    max_file_ = -1;
    digits_ = 0;
    unnumbered_file_found_ = false;
    subdirs_ = false;
    ext_ = UNKNOWN;
    selected_ = -1;
  }

  void Scan(const char *filename) {
    const char *rest = startswith(name_, filename);
    if (!rest) return;
    if (*rest == '/') {
      subdirs_ = true;
      const char *tmp = startswith(name_, rest + 1);
      if (!tmp) return;
      rest = tmp;
    }

    int n = -1;
    if (*rest == '.') {
      unnumbered_file_found_ = true;
    } else {
      char *end;
      n = strtol(rest, &end, 0);
      if (n <= 0) return;
      max_file_ = max(max_file_, n);
      min_file_ = min(min_file_, n);
      if (*rest == '0') {
        digits_ = end - rest;
      }
    }

    if (ext_ == UNKNOWN)
      ext_ = IdentifyExtension(filename);
  }

  void Show() {
    if (files_found()) {
      STDOUT.print("Found ");
      STDOUT.print(name_);
      STDOUT.print(" files: ");
      if (min_file_ <= max_file_) {
        STDOUT.print(min_file_);
        STDOUT.print("-");
        STDOUT.print(max_file_);
        if (digits_) {
          STDOUT.print(" using ");
          STDOUT.print(digits_);
          STDOUT.print(" digits");
        }
        if (unnumbered_file_found_) {
          STDOUT.print(" + ");
        }
      }
      if (unnumbered_file_found_) {
        STDOUT.print("one unnumbered file");
      }
      if (subdirs_) {
        STDOUT.print(" in subdirs");
      }
      STDOUT.println("");
    }
  }

  static void ShowAll() {
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Show();
    }
    STDOUT.println("Done listing effects.");
  }

  size_t files_found() const {
    size_t ret = 0;
    if (min_file_ <= max_file_) {
      ret += max_file_ - min_file_ + 1;
    }
    if (unnumbered_file_found_) {
      ret ++;
    }
    return ret;
  }

  void Select(int n) {
    selected_ = n;
  }

  FileID RandomFile() const {
    int num_files = files_found();
    if (num_files < 1) {
      default_output->print("No sounds found: ");
      default_output->println(name_);
      return FileID();
    }
    int n = rand() % num_files;
    if (selected_ != -1) n = selected_;
    return FileID(this, n);
  }

  bool Play(char *filename) {
    FileID f = RandomFile();
    if (f == FileID()) return false;
    f.GetName(filename);
    return true;
  }

  // Get the name of a specific file in the set.
  void GetName(char *filename, int n) const {
    strcpy(filename, current_directory);
    strcat(filename, name_);
    if (subdirs_) {
      strcat(filename, "/");
      strcat(filename, name_);
    }
    n += min_file_;
    // n can be max_file_ + 1, which means pick the file without digits.
    if (n <= max_file_) {
      char buf[12];
      itoa(n, buf, 10);
      char *j = filename + strlen(filename);
      int num_digits = strlen(buf);
      while (num_digits < digits_) {
        *j = '0';
        ++j;
        num_digits++;
      }
      memcpy(j, buf, strlen(buf) + 1);
    }

    switch (ext_) {
      case WAV: strcat(filename, ".wav"); break;
      case RAW: strcat(filename, ".raw"); break;
      case USL: strcat(filename, ".usl"); break;
      default: break;
    }
    
    default_output->print("Playing ");
    default_output->println(filename);
  }

  static void ScanAll(const char* filename) {
    if (Effect::IdentifyExtension(filename) == Effect::UNKNOWN) {
      return;
    }

#if 0
    // TODO: "monitor scan" command?
    STDOUT.print("SCAN ");
    STDOUT.println(filename);
#endif
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Scan(filename);
    }
  }

  static void ScanDirectory(const char *directory) {
    STDOUT.print("Scanning sound font: ");
    STDOUT.print(directory);
    for (Effect* e = all_effects; e; e = e->next_) {
      e->reset();
    }

#ifdef ENABLE_SERIALFLASH
    // Scan serial flash.
    SerialFlashChip::opendir();
    uint32_t size;
    char filename[128];
    while (SerialFlashChip::readdir(filename, sizeof(filename), size)) {
      const char* f = startswith(directory, filename);
      if (f) ScanAll(f);
    }
#endif

#ifdef ENABLE_SD

#if 1
    if (LSFS::Exists(directory)) {
      for (LSFS::Iterator iter(directory); iter; ++iter) {
        if (iter.isdir()) {
          char fname[128];
          strcpy(fname, iter.name());
          strcat(fname, "/");
          char* fend = fname + strlen(fname);
          for (LSFS::Iterator i2(iter); i2; ++i2) {
            strcpy(fend, i2.name());
            ScanAll(fname);
          }
        } else {
          ScanAll(iter.name());
        }
      }
    }
#else
    if (SD.exists(directory)) {
      File dir = SD.open(directory);
      if (dir) {
        while (File f = dir.openNextFile()) {
          if (f.isDirectory()) {
            char fname[128];
            strcpy(fname, f.name());
            strcat(fname, "/");
            char* fend = fname + strlen(fname);
            while (File f2 = f.openNextFile()) {
              strcpy(fend, f2.name());
              ScanAll(fname);
              f2.close();
            }
          } else {
            ScanAll(f.name());
          }
          f.close();
        }
        dir.close();
      }
    }
#endif
    
#ifdef ENABLE_AUDIO
    else if (strlen(directory) > 8) {
      talkie.Say(spFONTDIRECTORY);
      talkie.Say(spTOOLONG);
    } else if (strlen(directory)) {
      talkie.Say(spFONTDIRECTORY);
      talkie.Say(spNOTFOUND);
    }
#endif   // ENABLE_AUDIO    
#endif   // ENABLE_SD
    STDOUT.println(" done");
  }

private:
  Effect* next_;

  // Minimum file number.
  int min_file_;

  // Maximum file number.
  int max_file_;

  // Leading zeroes are used to make it this many digits.
  int digits_;

  // If true. there is an un-numbered file as well.
  bool unnumbered_file_found_;

  // Files are in subdirectories, like "lock/lockNN.wav"
  bool subdirs_;

  // All files must start with this prefix.
  const char* name_;

  // If not -1, return this file.
  int selected_;

  // All files must end with this extension.
  Extension ext_;
};

#endif
