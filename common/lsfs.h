#ifndef COMMON_LSFS_H
#define COMMON_LSFS_H

// Filesystem abstractions

#ifdef TEENSYDUINO
class LSFS {
public:
  typedef File FILE;
  static bool Begin() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
    // Prefer the built-in sd card for Teensy 3.5/3.6 as it is faster.
    return SD.begin(BUILTIN_SDCARD);
#else 
    return SD.begin(sdCardSelectPin);
#endif    
  }
  static bool Exists(const char* path) {
    return SD.exists(path);
  }
  static File Open(const char* path) {
    return SD.open(path);
  }
  static File OpenForWrite(const char* path) {
    return SD.open(path, FILE_WRITE);
  }
  class Iterator {
  public:
    explicit Iterator(const char* dirname) {
      dir_ = SD.open(dirname);
      if (dir_.isDirectory()) {
        f_ = dir_.openNextFile();
      }
    }
    explicit Iterator(Iterator& other) {
      dir_ = other.f_;
      other.f_ = File();
      f_ = dir_.openNextFile();
    }
    ~Iterator() {
      dir_.close();
      f_.close();
    }
    void operator++() {
      f_.close();
      f_ = dir_.openNextFile();
    }
    operator bool() { return f_; }
    bool isdir() { return f_.isDirectory(); }
    const char* name() { return f_.name(); }
    size_t size() { return f_.size(); }
    
  private:
    File dir_;
    File f_;
  };
};
#else
class LSFS {
public:
  typedef File FILE;
  static bool Begin() {
    return DOSFS.begin() && DOSFS.check();
  }
  static bool Exists(const char* path) {
    return DOSFS.exists(path);
  }
  static File Open(const char* path) {
    return DOSFS.open(path, "r");
  }
  static File OpenForWrite(const char* path) {
    return DOSFS.open(path, "wc");
  }
  class Iterator {
  public:
    explicit Iterator(const char* path) {
      char filename[F_MAXPATH];

      strcpy(_path, path);

      if (path[strlen(path)-1] != '/')  
        strcat(_path, "/");

      strcpy(filename, _path);
      strcat(filename, "*.*");

      if (f_findfirst(filename, &_find) != F_NO_ERROR) {
        _find.find_clsno = 0x0fffffff;
      }
    }
    explicit Iterator(Iterator& other) {
      char filename[F_MAXPATH];

      strcpy(_path, other._path);
      strcat(_path, other.name());

      if (_path[strlen(_path)-1] != '/')  
        strcat(_path, "/");

      strcpy(filename, _path);
      strcat(filename, "*.*");

      if (f_findfirst(filename, &_find) != F_NO_ERROR) {
        _find.find_clsno = 0x0fffffff;
      }
    }

    void operator++() {
      if (f_findnext(&_find) != F_NO_ERROR) {
        _find.find_clsno = 0x0fffffff;
      }
    }
    operator bool() { return _find.find_clsno != 0x0fffffff; }
    bool isdir() { return _find.attr & F_ATTR_DIR; }
    const char* name() { return _find.filename; }
    size_t size() { return _find.filesize; }
    
  private:
    char _path[F_MAXPATH];
    F_FIND _find;
  };
};
#endif

#endif
