#ifndef COMMON_LSFS_H
#define COMMON_LSFS_H

// Filesystem abstractions

#ifdef TEENSYDUINO

#include <SD.h>

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
  static bool End() {
    return true;
  }
  static bool Exists(const char* path) {
    return SD.exists(path);
  }
  static bool Remove(const char* path) {
    return SD.remove(path);
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

#include <FS.h>

// Workaround for badly named variable
#define private c_private
#include <dosfs_core.h>
#undef private

class LSFS {
public:
  typedef File FILE;
  static bool IsMounted() {
    return mounted_;
  }
  static bool CanMount() {
    // dosfs_volume_t *volume = DOSFS_DEFAULT_VOLUME();
    dosfs_device_t *device = DOSFS_VOLUME_DEVICE(volume);
    if (device->lock & (DOSFS_DEVICE_LOCK_VOLUME |
			DOSFS_DEVICE_LOCK_SCSI |
			DOSFS_DEVICE_LOCK_MEDIUM |
			DOSFS_DEVICE_LOCK_INIT)) return false;
    return true;
  }
  static void WhyBusy(char *tmp) {
    *tmp = 0;
    // dosfs_volume_t *volume = DOSFS_DEFAULT_VOLUME();
    dosfs_device_t *device = DOSFS_VOLUME_DEVICE(volume);
    if (device->lock & DOSFS_DEVICE_LOCK_VOLUME)
      strcat(tmp, " volume");
    if (device->lock & DOSFS_DEVICE_LOCK_SCSI)
      strcat(tmp, " scsi");
    if (device->lock & DOSFS_DEVICE_LOCK_MEDIUM)
      strcat(tmp, " medium");
    if (device->lock & DOSFS_DEVICE_LOCK_INIT)
      strcat(tmp, " init");
  }
  // This function waits until the volume is mounted.
  static bool Begin() {
    if (mounted_) return true;
    return mounted_ = (DOSFS.begin() && DOSFS.check());
  }
  static void End() {
    if (!mounted_) return;
    DOSFS.end();
    mounted_ = false;
  }
  static bool Exists(const char* path) {
    if (!mounted_) return false;
    return DOSFS.exists(path);
  }
  static bool Remove(const char* path) {
    return DOSFS.remove(path);
  }
  static File Open(const char* path) {
    if (!mounted_) return File();
    return DOSFS.open(path, "r");
  }
  static File OpenForWrite(const char* path) {
    if (!mounted_) return File();
    return DOSFS.open(path, "w");
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

      if (!mounted_ || f_findfirst(filename, &_find) != F_NO_ERROR) {
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

      if (!mounted_ || f_findfirst(filename, &_find) != F_NO_ERROR) {
        _find.find_clsno = 0x0fffffff;
      }
    }

    void operator++() {
      if (!mounted_ || f_findnext(&_find) != F_NO_ERROR) {
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
private:
  static bool mounted_;
};

bool LSFS::mounted_ = false;
#endif

#endif
