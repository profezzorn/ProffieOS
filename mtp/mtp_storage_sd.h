#ifndef MTP_MTP_STORAGE_SD_H
#define MTP_MTP_STORAGE_SD_H

// Storage implementation for SD. SD needs to be already initialized.
class MTPStorage_SD : public MTPStorageInterface {
public:
  explicit MTPStorage_SD(MTPD* mtpd) { mtpd->AddStorage(this); }
private:
  File index_;

  uint8_t mode_ = 0;
  uint32_t open_file_ = 0xFFFFFFFEUL;
  File f_;
  uint32_t index_entries_ = 0;

  struct Record {
    uint32_t parent;
    uint32_t child;  // size stored here for files
    uint32_t sibling;
    uint8_t isdir;
    uint8_t scanned;
    char name[14];
  };

  bool readonly() override { return false; }
  bool has_directories() override { return true; }
  uint64_t size() override { return 1 << 30; }
  uint64_t free() override { return 1 << 29; }
  uint32_t free_objects() override { return 0xFFFFFFFEUL; }

  const char* GetName() override {
    return "SD Card";
  }

  void OpenIndex() {
    if (index_) return;
    mtp_lock_storage(true);
    index_ = SD.open("mtpindex.dat", FILE_WRITE);
    mtp_lock_storage(false);
  }

  void WriteIndexRecord(uint32_t i, const Record& r) {
    OpenIndex();
    mtp_lock_storage(true);
    index_.seek(sizeof(r) * i);
    index_.write((char*)&r, sizeof(r));
    mtp_lock_storage(false);
  }
  
  uint32_t AppendIndexRecord(const Record& r) {
    uint32_t new_record = index_entries_++;
    WriteIndexRecord(new_record, r);
    return new_record;
  }

  // TODO(hubbe): Cache a few records for speed.
  Record ReadIndexRecord(uint32_t i) {
    Record ret;
    if (i > index_entries_) {
      memset(&ret, 0, sizeof(ret));
      return ret;
    }
    OpenIndex();
    mtp_lock_storage(true);
    index_.seek(sizeof(ret) * i);
    index_.read(&ret, sizeof(ret));
    mtp_lock_storage(false);
    return ret;
  }

  void ConstructFilename(int i, char* out) {
    if (i == 0) {
      strcpy(out, "/");
    } else {
      Record tmp = ReadIndexRecord(i);
      ConstructFilename(tmp.parent, out);
      if (out[strlen(out)-1] != '/')
        strcat(out, "/");
      strcat(out, tmp.name);
    }
  }

  void OpenFileByIndex(uint32_t i, uint8_t mode = O_RDONLY) {
    if (open_file_ == i && mode_ == mode)
      return;
    char filename[256];
    ConstructFilename(i, filename);
    mtp_lock_storage(true);
    f_.close();
    f_ = SD.open(filename, mode);
    open_file_ = i;
    mode_ = mode;
    mtp_lock_storage(false);
  }

  // MTP object handles should not change or be re-used during a session.
  // This would be easy if we could just have a list of all files in memory.
  // Since our RAM is limited, we'll keep the index in a file instead.
  bool index_generated = false;
  void GenerateIndex() {
    if (index_generated) return;
    index_generated = true;

    mtp_lock_storage(true);
    SD.remove("mtpindex.dat");
    mtp_lock_storage(false);
    index_entries_ = 0;

    Record r;
    r.parent = 0;
    r.sibling = 0;
    r.child = 0;
    r.isdir = true;
    r.scanned = false;
    strcpy(r.name, "/");
    AppendIndexRecord(r);
  }

  void ScanDir(uint32_t i) {
    Record record = ReadIndexRecord(i);
    if (record.isdir && !record.scanned) {
      OpenFileByIndex(i);
      if (!f_) return;
      int sibling = 0;
      while (true) {
        mtp_lock_storage(true);
        File child = f_.openNextFile();
        mtp_lock_storage(false);
        if (!child) break;
        Record r;
        r.parent = i;
        r.sibling = sibling;
        r.isdir = child.isDirectory();
        r.child = r.isdir ? 0 : child.size();
        r.scanned = false;
        strcpy(r.name, child.name());
        sibling = AppendIndexRecord(r);
        child.close();
      }
      record.scanned = true;
      record.child = sibling;
      WriteIndexRecord(i, record);
    }
  }

  bool all_scanned_ = false;
  void ScanAll() {
    if (all_scanned_) return;
    all_scanned_ = true;

    GenerateIndex();
    for (uint32_t i = 0; i < index_entries_; i++) {
      ScanDir(i);
    }
  }

  uint32_t next_;
  bool follow_sibling_;
  void StartGetObjectHandles(uint32_t parent) override {
    GenerateIndex();
    if (parent) {
      if (parent == 0xFFFFFFFF) parent = 0;
      ScanDir(parent);
      follow_sibling_ = true;
      // Root folder?
      next_ = ReadIndexRecord(parent).child;
    } else {
      ScanAll();
      follow_sibling_ = false;
      next_ = 1;
    }
  }

  uint32_t GetNextObjectHandle() override {
    while (true) {
      if (next_ == 0)
        return 0;
      int ret = next_;
      Record r = ReadIndexRecord(ret);
      if (follow_sibling_) {
        next_ = r.sibling;
      } else {
        next_++;
        if (next_ >= index_entries_)
          next_ = 0;
      }
      if (r.name[0]) return ret;
    }
  }

  void GetObjectInfo(uint32_t handle,
                     char* name,
                     uint32_t* size,
                     uint32_t* parent) override {
    Record r = ReadIndexRecord(handle);
    strcpy(name, r.name);
    *parent = r.parent;
    *size = r.isdir ? 0xFFFFFFFFUL : r.child;
  }

  uint32_t GetSize(uint32_t handle) {
    return ReadIndexRecord(handle).child;
  }

  void read(uint32_t handle,
            uint32_t pos,
            char* out,
            uint32_t bytes) override {
    OpenFileByIndex(handle);
    mtp_lock_storage(true);
    f_.seek(pos);
    f_.read(out, bytes);
    mtp_lock_storage(false);
  }

  bool DeleteObject(uint32_t object) override {
    char filename[256];
    Record r;
    while (true) {
      r = ReadIndexRecord(object == 0xFFFFFFFFUL ? 0 : object);
      if (!r.isdir) break;
      if (!r.child) break;
      if (!DeleteObject(r.child))
        return false;
    }

    // We can't actually delete the root folder,
    // but if we deleted everything else, return true.
    if (object == 0xFFFFFFFFUL) return true;

    ConstructFilename(object, filename);
    bool success;
    mtp_lock_storage(true);
    if (r.isdir) {
      success = SD.rmdir(filename);
    } else {
      success = SD.remove(filename);
    }
    mtp_lock_storage(false);
    if (!success) return false;
    r.name[0] = 0;
    int p = r.parent;
    WriteIndexRecord(object, r);
    Record tmp = ReadIndexRecord(p);
    if (tmp.child == object) {
      tmp.child = r.sibling;
      WriteIndexRecord(p, tmp);
    } else {
      int c = tmp.child;
      while (c) {
        tmp = ReadIndexRecord(c);
        if (tmp.sibling == object) {
          tmp.sibling = r.sibling;
          WriteIndexRecord(c, tmp);
          break;
        } else {
          c = tmp.sibling;
        }
      }
    }
    return true;
  }

  uint32_t Create(uint32_t parent,
                  bool folder,
                  const char* filename) override {
    uint32_t ret;
    if (parent == 0xFFFFFFFFUL) parent = 0;
    Record p = ReadIndexRecord(parent);
    Record r;
    if (strlen(filename) > 12) return 0;
    strcpy(r.name, filename);
    r.parent = parent;
    r.child = 0;
    r.sibling = p.child;
    r.isdir = folder;
    // New folder is empty, scanned = true.
    r.scanned = 1;
    ret = p.child = AppendIndexRecord(r);
    WriteIndexRecord(parent, p);
    if (folder) {
      char filename[256];
      ConstructFilename(ret, filename);
      mtp_lock_storage(true);
      SD.mkdir(filename);
      mtp_lock_storage(false);
    } else {
      OpenFileByIndex(ret, FILE_WRITE);
    }
    return ret;
  }

  void write(const char* data, uint32_t bytes) override {
    mtp_lock_storage(true);
    f_.write(data, bytes);
    mtp_lock_storage(false);
  }

  void close() override {
    mtp_lock_storage(true);
    uint32_t size = f_.size();
    f_.close();
    mtp_lock_storage(false);
    Record r = ReadIndexRecord(open_file_);
    r.child = size;
    WriteIndexRecord(open_file_, r);
    open_file_ = 0xFFFFFFFEUL;
  }
};

#endif
