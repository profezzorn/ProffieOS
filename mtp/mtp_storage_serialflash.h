#ifndef MTP_MTP_STORAGE_SERIALFLASH_H
#define MTP_MTP_STORAGE_SERIALFLASH_H

// Need to expose a bunch of functions in SerialFlash to make this work..
// Also need to support multiple MTPStorage in the MTP responder.
class MTPStorage_SerialFlash : public MTPStorageInterface {
public:
  explicit MTPStorage_SerialFlash(MTPD* mtpd) { mtpd->AddStorage(this); }
private:
  int entry = 0;
  int last_entry_ = -1;
  int maxfiles_ = -1;
  uint16_t readHash(int index) {
    uint16_t hash;
    mtp_lock_storage(true);
    SerialFlash.read(8 + index * 2, &hash, 2);
    mtp_lock_storage(false);                           
    return hash;
  }

  int maxfiles() {
    if (maxfiles_ != -1) return maxfiles_;
    uint32_t sig[2];
    mtp_lock_storage(true);
    SerialFlash.read(0, sig, 8);
    mtp_lock_storage(false);
    if (sig[0] == 0xFA96554C) {
      maxfiles_ = sig[1] & 0xffff;
      return maxfiles_;
    }
    return 0;
  }
  
  int last_entry() {
    if (last_entry_ == -1) {
      int max = maxfiles();
      int min = -1;
      while (max - min > 1) {
        int mid = (max + min) / 2;
        if (readHash(mid) == 0xffff) {
          max = mid;
        } else {
          min = mid;
        }
      }
      last_entry_ = min;
    }
    return last_entry_;
  }

  SerialFlashFile open(int index) {
    SerialFlashFile file;
    uint32_t buf[2];
    mtp_lock_storage(true);
    SerialFlash.read(8 + maxfiles() * 2 + index * 10, buf, 8);
    mtp_lock_storage(false);
    file.address = buf[0];
    file.length = buf[1];
    file.offset = 0;
    file.dirindex = index;
    return file;
  }

public:
  // Return true if this storage is read-only
  bool readonly() override {
    return false;
  }
 
  // Does it have directories?
  bool has_directories() override {
    return false;
  }

  // Return size of storage in bytes.
  uint64_t size() override {
    uint8_t id[5];
    mtp_lock_storage(true);
    SerialFlash.readID(id);
    uint64_t ret = SerialFlashChip::capacity(id);
    mtp_lock_storage(false);
    return ret;
  }

  // Return free space in bytes.
  uint64_t free() override {
    int entry = last_entry();
    if (entry == -1) return size();
    SerialFlashFile last = open(entry);
    return size() - (last.getFlashAddress() + last.size());
  }

  uint32_t free_objects() override {
    return maxfiles() - last_entry() - 1;
  }

  const char* GetName() override {
    return "SerialFlash";
  }

  // parent = 0 means get all handles.
  // parent = 0xFFFFFFFF means get root folder.
  void StartGetObjectHandles(uint32_t parent) override {
    entry = 0;
  }
  
  uint32_t GetNextObjectHandle() override {
    while (true) {
      if (entry >= maxfiles()) return 0;
      uint16_t hash = readHash(entry);
      if (hash == 0xFFFF) return 0;
      if (hash == 0) continue; // deleted
      entry++;
      return entry;
    };
  }
  // Size should be 0xFFFFFFFF if it's a directory.
  void GetObjectInfo(uint32_t handle,
                     char* name,
                     uint32_t* size,
                     uint32_t* parent) override {
    handle--;
    uint32_t buf[3];
    buf[2] = 0;
    mtp_lock_storage(true);
    SerialFlash.read(8 + maxfiles() * 2 + handle * 10, buf, 10);
    mtp_lock_storage(false);
    int straddr = 8 + maxfiles() * 12 + buf[2] * 4;
    name[255] = 0;
    mtp_lock_storage(true);
    SerialFlash.read(straddr, name, 255); // TODO max filename length
    mtp_lock_storage(false);
    *size = buf[1];
    *parent = 0; // Top level
  }
  uint32_t GetSize(uint32_t handle) override {
    handle--;
    return open(handle).size();
  }
  void read(uint32_t handle,
            uint32_t pos,
            char* buffer,
            uint32_t bytes) override {
    handle--;
    SerialFlashFile file = open(handle);
    mtp_lock_storage(true);
    file.seek(pos);
    file.read(buffer, bytes);
    mtp_lock_storage(false);
  }
  char new_filename[256];
  uint32_t Create(uint32_t parent,
                  bool folder,
                  const char* filename) override {
    if (strlen(filename) > 255) return 0;
    strcpy(new_filename, filename);
    return last_entry() + 1 + 1;
  }
  SerialFlashFile file;
  virtual void SendObject(uint32_t length) {
    last_entry_++;
    mtp_lock_storage(true);
    SerialFlash.create(new_filename, length);
    mtp_lock_storage(false);
    file = open(last_entry_);
  }
  void write(const char* data, uint32_t size) override {
    mtp_lock_storage(true);
    file.write(data, size);
    mtp_lock_storage(false);
  }
  void close() override {
  }
  bool DeleteObject(uint32_t object) override {
    object--;
    SerialFlashFile file = open(object);
    mtp_lock_storage(true);
    bool ret = SerialFlashChip::remove(file);
    mtp_lock_storage(false);
    return ret;
  }
  virtual bool Format() {
    mtp_lock_storage(true);
    last_entry_ = -1;
    maxfiles_ = -1;
    SerialFlashChip::eraseAll();
    while (!SerialFlashChip::ready()) {
      mtp_lock_storage(false);
      mtp_yield();
      mtp_lock_storage(true);
    }
    SerialFlash.open("bogus"); // creates signature
    mtp_lock_storage(false);
    return true;
  }
};

#endif
