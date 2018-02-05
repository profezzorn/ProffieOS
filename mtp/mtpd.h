#ifndef MTP_MTPD_H
#define MTP_MTPD_H

// This interface lets the MTP responder interface any storage.
// We'll need to give the MTP responder a pointer to one of these.
class MTPStorageInterface {
public:
  // Return true if this storage is read-only
  virtual bool readonly() = 0;
 
  // Does it have directories?
  virtual bool has_directories() = 0;

  // Return size of storage in bytes.
  virtual uint64_t size() = 0;

  // Return free space in bytes.
  virtual uint64_t free() = 0;

  virtual uint32_t free_objects() = 0;

  // Get Name of storage unit.
  virtual const char *GetName() = 0;

  // parent = 0 means get all handles.
  // parent = 0xFFFFFFFF means get root folder.
  virtual void StartGetObjectHandles(uint32_t parent) = 0;
  virtual uint32_t GetNextObjectHandle() = 0;

  // Size should be 0xFFFFFFFF if it's a directory.
  virtual void GetObjectInfo(uint32_t handle,
                             char* name,
                             uint32_t* size,
                             uint32_t* parent) = 0;
  virtual uint32_t GetSize(uint32_t handle) = 0;
  virtual void read(uint32_t handle,
                    uint32_t pos,
                    char* buffer,
                    uint32_t bytes) = 0;
  virtual uint32_t Create(uint32_t parent,
                          bool folder,
                          const char* filename) = 0;
  virtual void SendObject(uint32_t length) {}
  virtual void write(const char* data, uint32_t size);
  virtual void close() {}
  virtual bool DeleteObject(uint32_t object) = 0;
  virtual bool Format() { return false; }
};

// MTP Responder.
class MTPD {
public:
  void AddStorage(MTPStorageInterface* storage) {
    for (size_t i = 0; i < NELEM(storage_); i++) {
      if (!storage_[i]) {
        storage_[i] = storage;
        return;
      }
    }
  }
    
  MTPD() {
    for (size_t i = 0; i < NELEM(storage_); i++) storage_[i] = 0;
  }

private:
  MTPStorageInterface* storage_[16];

  MTPStorageInterface* Stor(uint32_t id) {
    return storage_[id >> 28];
  }

  // Make an external object id from an internal one.
  uint32_t EXT(uint32_t id, uint32_t storage) {
    if (id == 0 || id == 0xFFFFFFFFUL) return id;
    return id | (storage & 0xF0000000UL);
  }

  // Make an internal object id from an external one.
  uint32_t INT(uint32_t id) {
    if (id == 0 || id == 0xFFFFFFFFUL) return id;
    return id & 0xFFFFFFFUL;
  }

  uint32_t NextStorage(uint32_t store) {
    uint32_t i = store >> 28;
    while (true) {
      i++;
      if (i >= NELEM(storage_)) return 0;
      if (storage_[i]) return (i << 28) | 1;
    }
  }

  uint32_t FirstStorage() {
    for (size_t i = 0; i < NELEM(storage_); i++) {
      if (storage_[i]) return (i << 28) | 1;
    }
    return 0;
  }

  struct MTPHeader {
    uint32_t len;  // 0
    uint16_t type; // 4
    uint16_t op;   // 6
    uint32_t transaction_id; // 8
  } __attribute__((__may_alias__));

  struct MTPContainer {
    uint32_t len;  // 0
    uint16_t type; // 4
    uint16_t op;   // 6
    uint32_t transaction_id; // 8
    uint32_t params[5];    // 12
  } __attribute__((__may_alias__));

  void PrintPacket(const usb_packet_t *x) {
#if 0
    for (int i = 0; i < x->len; i++) {
      STDOUT.print("0123456789ABCDEF"[x->buf[i] >> 4]);
      STDOUT.print("0123456789ABCDEF"[x->buf[i] & 0xf]);
      if ((i & 3) == 3) STDOUT.print(" ");
    } 
    STDOUT.println("");
#endif
#if 0
    MTPContainer *tmp = (struct MTPContainer*)(x->buf);
    STDOUT.print(" len = ");
    STDOUT.print(tmp->len, HEX);
    STDOUT.print(" type = ");
    STDOUT.print(tmp->type, HEX);
    STDOUT.print(" op = ");
    STDOUT.print(tmp->op, HEX);
    STDOUT.print(" transaction_id = ");
    STDOUT.print(tmp->transaction_id, HEX);
    for (int i = 0; i * 4 < x->len - 12; i ++) {
      STDOUT.print(" p");
      STDOUT.print(i);
      STDOUT.print(" = ");
      STDOUT.print(tmp->params[i], HEX);
    }
    STDOUT.println("");
    STDOUT.flush();
    delay(10);
#endif
  }

  usb_packet_t *data_buffer_ = NULL;
  void get_buffer() {
    while (!data_buffer_) {
      data_buffer_ = usb_malloc();
      if (!data_buffer_) mtp_yield();
    }
  }

  void receive_buffer() {
    while (!data_buffer_) {
      data_buffer_ = usb_rx(MTP_RX_ENDPOINT);
      if (!data_buffer_) mtp_yield();
    }
  }

  bool write_get_length_ = false;
  uint32_t write_length_ = 0;
  void write(const char *data, int len) {
    if (write_get_length_) {
      write_length_ += len;
    } else {
      int pos = 0;
      while (pos < len) {
        get_buffer();
        int avail = sizeof(data_buffer_->buf) - data_buffer_->len;
        int to_copy = min(len - pos, avail);
        memcpy(data_buffer_->buf + data_buffer_->len,
               data + pos,
               to_copy);
        data_buffer_->len += to_copy;
        pos += to_copy;
        if (data_buffer_->len == sizeof(data_buffer_->buf)) {
          usb_tx(MTP_TX_ENDPOINT, data_buffer_);
          data_buffer_ = NULL;
          // Serial1.println("SENT...");
        }
      }
    }
  }
  void write8(uint8_t x) { write((char*)&x, sizeof(x)); }
  void write16(uint16_t x) { write((char*)&x, sizeof(x)); }
  void write32(uint32_t x) { write((char*)&x, sizeof(x)); }
  void write64(uint64_t x) { write((char*)&x, sizeof(x)); }
  void writestring(const char* str) {
    if (*str) {
      write8(strlen(str) + 1);
      while (*str) {
        write16(*str);
        ++str;
      }
      write16(0);
    } else {
      write8(0);
    }
  }

  void WriteDescriptor() {
    write16(100);  // MTP version
    write32(6);    // MTP extension
//    write32(0xFFFFFFFFUL);    // MTP extension
    write16(100);  // MTP version
    writestring("microsoft.com: 1.0;");
    write16(0);    // functional mode

    // Supported operations (array of uint16)
    write32(15);
    write16(0x1001);  // GetDeviceInfo
    write16(0x1002);  // OpenSession
    write16(0x1003);  // CloseSession
    write16(0x1004);  // GetStorageIDs

    write16(0x1005);  // GetStorageInfo
    write16(0x1006);  // GetNumObjects
    write16(0x1007);  // GetObjectHandles
    write16(0x1008);  // GetObjectInfo

    write16(0x1009);  // GetObject
    write16(0x100B);  // DeleteObject
    write16(0x100C);  // SendObjectInfo
    write16(0x100D);  // SendObject
    write16(0x100F);  // FormatStore

    write16(0x1014);  // GetDevicePropDesc
    write16(0x1015);  // GetDevicePropValue

//    write16(0x1010);  // Reset
//    write16(0x1019);  // MoveObject
//    write16(0x101A);  // CopyObject

    write32(0);       // Events (array of uint16)

    write32(1);       // Device properties (array of uint16)
    write16(0xd402);  // Device friendly name

    write32(0);       // Capture formats (array of uint16)

    write32(3);       // Playback formats (array of uint16)
    write16(0x3000);  // Undefined format
    write16(0x3001);  // Folders (associations)
    write16(0x3008);  // WAV

    writestring("PJRC");     // Manufacturer
    writestring("Teensy");   // Model
    writestring("1.0");      // version
    writestring("???");      // serial
  }

  void WriteStorageIDs() {
    int32_t num = 0;
    for (size_t i = FirstStorage(); i; i = NextStorage(i)) num++;
    write32(num);
    for (size_t i = FirstStorage(); i; i = NextStorage(i)) write32(i);
  }

  void GetStorageInfo(uint32_t storage) {
    write16(Stor(storage)->readonly() ? 0x0001 : 0x0004);   // storage type (removable RAM)
    write16(Stor(storage)->has_directories() ? 0x0002: 0x0001);   // filesystem type (generic hierarchical)
    write16(0x0000);   // access capability (read-write)
    write64(Stor(storage)->size());  // max capacity
    write64(Stor(storage)->free());  // free space (100M)
    write32(Stor(storage)->free_objects());  // free space (objects)
    writestring(Stor(storage)->GetName());  // storage descriptor
    writestring("");  // volume identifier
  }

  uint32_t GetNumObjects(uint32_t storage,
                         uint32_t parent) {
    uint32_t num = 0;
    if (storage == 0xFFFFFFFFUL) {
      for (size_t i = FirstStorage(); i; i = NextStorage(i)) {
        Stor(i)->StartGetObjectHandles(INT(parent));
        while (Stor(i)->GetNextObjectHandle()) num++;
      }
    } else {
      Stor(storage)->StartGetObjectHandles(INT(parent));
      while (Stor(storage)->GetNextObjectHandle()) num++;
    }
    return num;
  }

  void GetObjectHandles(uint32_t storage,
                        uint32_t parent) {
    uint32_t num = 0;
    int handle;
    if (!write_get_length_) {
      num = GetNumObjects(storage, parent);
    }
    write32(num);
    if (storage == 0xFFFFFFFFUL) {
      for (size_t i = FirstStorage(); i; i = NextStorage(i)) {
        Stor(i)->StartGetObjectHandles(INT(parent));
        while ((handle = Stor(i)->GetNextObjectHandle()))
          write32(EXT(handle, i));
      }
    } else {
      Stor(storage)->StartGetObjectHandles(INT(parent));
      while ((handle = Stor(storage)->GetNextObjectHandle()))
        write32(EXT(handle, storage));
    }
  }

  void GetObjectInfo(uint32_t handle) {
    char filename[256];
    uint32_t size, parent;
    Stor(handle)->GetObjectInfo(INT(handle), filename, &size, &parent);

    write32(handle & 0xF0000000UL); // storage
    write16(size == 0xFFFFFFFFUL ? 0x3001 : 0x0000); // format
    write16(0);  // protection
    write32(size); // size
    write16(0); // thumb format
    write32(0); // thumb size
    write32(0); // thumb width
    write32(0); // thumb height
    write32(0); // pix width
    write32(0); // pix height
    write32(0); // bit depth
    write32(parent); // parent
    write16(size == 0xFFFFFFFFUL ? 1 : 0); // association type
    write32(0); // association description
    write32(0);  // sequence number
    writestring(filename);
    writestring("");  // date created
    writestring("");  // date modified
    writestring("");  // keywords
  }

  void GetObject(uint32_t object_id) {
    uint32_t size = Stor(object_id)->GetSize(INT(object_id));
    if (write_get_length_) {
      write_length_ += size;
    } else {
      uint32_t pos = 0;
      while (pos < size) {
        get_buffer();
        uint32_t avail = sizeof(data_buffer_->buf) - data_buffer_->len;
        uint32_t to_copy = min(pos - size, avail);
        // Read directly from storage into usb buffer.
        Stor(object_id)->read(INT(object_id),
                              pos,
                              (char*)(data_buffer_->buf + data_buffer_->len),
                              to_copy);
        pos += to_copy;
        data_buffer_->len += to_copy;
        if (data_buffer_->len == sizeof(data_buffer_->buf)) {
          usb_tx(MTP_TX_ENDPOINT, data_buffer_);
          data_buffer_ = NULL;
        }
      }
    }
  }

#define CONTAINER ((struct MTPContainer*)(receive_buffer->buf))

#define TRANSMIT(FUN) do {                              \
    write_length_ = 0;                                  \
    write_get_length_ = true;                           \
    FUN;                                                \
    write_get_length_ = false;                          \
    MTPHeader header;                                   \
    header.len = write_length_ + 12;                    \
    header.type = 2;                                    \
    header.op = CONTAINER->op;                          \
    header.transaction_id = CONTAINER->transaction_id;  \
    write((char *)&header, sizeof(header));             \
    FUN;                                                \
    get_buffer();                                       \
    usb_tx(MTP_TX_ENDPOINT, data_buffer_);              \
    data_buffer_ = NULL;                                \
  } while(0)
  
  void read(char* data, uint32_t size) {
    while (size) {
      receive_buffer();
      uint32_t to_copy = data_buffer_->len - data_buffer_->index;
      to_copy = min(to_copy, size);
      if (data) {
        memcpy(data,
               data_buffer_->buf + data_buffer_->index,
               to_copy);
        data += to_copy;
      }
      size -= to_copy;
      data_buffer_->index += to_copy;
      if (data_buffer_->index == data_buffer_->len) {
        usb_free(data_buffer_);
        data_buffer_ = NULL;
      }
    }
  }

  uint32_t ReadMTPHeader() {
    MTPHeader header;
    read((char *)&header, sizeof(MTPHeader));
    // check that the type is data
    return header.len - 12;
  }

  uint8_t read8() {
    uint8_t ret;
    read((char*)&ret, sizeof(ret));
    return ret;
  }
  uint16_t read16() {
    uint16_t ret;
    read((char*)&ret, sizeof(ret));
    return ret;
  }
  uint32_t read32() {
    uint32_t ret;
    read((char*)&ret, sizeof(ret));
    return ret;
  }

  void readstring(char* buffer) {
    int len = read8();
    if (!buffer) {
      read(NULL, len * 2);
    } else {
      for (int i = 0; i < len; i++) {
        *(buffer++) = read16();
      }
    }
  }

  void read_until_short_packet() {
    bool done = false;
    while (!done) {
      receive_buffer();
      done = data_buffer_->len != sizeof(data_buffer_->buf);
      usb_free(data_buffer_);
      data_buffer_ = NULL;
    }
  }

  uint32_t new_object;
  uint32_t SendObjectInfo(uint32_t storage, uint32_t parent) {
    ReadMTPHeader();
    char filename[256];

    read32(); // storage
    bool dir = read16() == 0x3001; // format
    read16();  // protection
    read32(); // size
    read16(); // thumb format
    read32(); // thumb size
    read32(); // thumb width
    read32(); // thumb height
    read32(); // pix width
    read32(); // pix height
    read32(); // bit depth
    read32(); // parent
    read16(); // association type
    read32(); // association description
    read32();  // sequence number

    readstring(filename);
    read_until_short_packet();  // ignores dates & keywords
    if (storage == 0) storage = FirstStorage();
    new_object = EXT(Stor(storage)->Create(INT(parent), INT(dir), filename), storage);
    return new_object;
  }

  void SendObject() {
    uint32_t len = ReadMTPHeader();
    Stor(new_object)->SendObject(len);
    while (len) {
      receive_buffer();
      uint32_t to_copy = data_buffer_->len - data_buffer_->index;
      to_copy = min(to_copy, len);
      Stor(new_object)->write((char*)(data_buffer_->buf + data_buffer_->index),
                              to_copy);
      data_buffer_->index += to_copy;
      len -= to_copy;
      if (data_buffer_->index == data_buffer_->len) {
        usb_free(data_buffer_);
        data_buffer_ = NULL;
      }
    }
    Stor(new_object)->close();
  }

  void GetDevicePropValue(uint32_t prop) {
    switch (prop) {
      case 0xd402: // friendly name
        // This is the name we'll actually see in the windows explorer.
        // Should probably be configurable.
        writestring("TeensySaber");
        break;
    }
  }
  void GetObjectPropValue(uint32_t handle, uint32_t prop) {
    switch (prop) {
      case 0xdc04: // size
        write64(Stor(handle)->GetSize(INT(handle)));
        break;
    }
  }
  
  void GetDevicePropDesc(uint32_t prop) {
    switch (prop) {
      case 0xd402: // friendly name
        write16(prop);
        write16(0xFFFF); // string type
        write8(0);       // read-only
        GetDevicePropValue(prop);
        GetDevicePropValue(prop);
        write8(0);       // no form
    }
  }

public:
  void loop() {
    usb_packet_t *receive_buffer;
    if ((receive_buffer = usb_rx(MTP_RX_ENDPOINT))) {
      PrintPacket(receive_buffer);
      uint32_t return_code = 0;
      uint32_t p1 = 0;
      if (receive_buffer->len >= 12) {
        return_code = 0x2001;  // Ok
        CONTAINER->len = receive_buffer->len = 12;
        if (CONTAINER->type == 1) { // command
          switch (CONTAINER->op) {
            case 0x1001: // GetDescription
              TRANSMIT(WriteDescriptor());
              break;
            case 0x1002:  // OpenSession
              break;
            case 0x1003:  // CloseSession
              break;
            case 0x1004:  // GetStorageIDs
              TRANSMIT(WriteStorageIDs());
              break;
            case 0x1005:  // GetStorageInfo
              TRANSMIT(GetStorageInfo(CONTAINER->params[0]));
              break;
            case 0x1006:  // GetNumObjects
              if (CONTAINER->params[1]) {
                return_code = 0x2014; // spec by format unsupported
              } else {
                p1 = GetNumObjects(CONTAINER->params[0],
                                   CONTAINER->params[2]);
                CONTAINER->len = receive_buffer->len = 16;
              }
              break;
            case 0x1007:  // GetObjectHandles
              if (CONTAINER->params[1]) {
                return_code = 0x2014; // spec by format unsupported
              } else {
                TRANSMIT(GetObjectHandles(CONTAINER->params[0],
                                          CONTAINER->params[2]));
              }
              break;
            case 0x1008:  // GetObjectInfo
              TRANSMIT(GetObjectInfo(CONTAINER->params[0]));
              break;
            case 0x1009:  // GetObject
              TRANSMIT(GetObject(CONTAINER->params[0]));
              break;
            case 0x100B:  // DeleteObject
              if (CONTAINER->params[1]) {
                return_code = 0x2014; // spec by format unsupported
              } else {
                if (!Stor(CONTAINER->params[0])->DeleteObject(
                      INT(CONTAINER->params[0]))) {
                  return_code = 0x2012; // partial deletion
                }
              }
              break;
            case 0x100C:  // SendObjectInfo
              CONTAINER->params[2] =
                SendObjectInfo(CONTAINER->params[0], // storage
                               CONTAINER->params[1]); // parent
              p1 = CONTAINER->params[0];
              if (!p1) p1 = 1;
              CONTAINER->len = receive_buffer->len = 12 + 3 * 4;
              break;
            case 0x100D:  // SendObject
              SendObject();
              break;
            case 0x100F:  // FormatStore
              if (!Stor(CONTAINER->params[0])->Format()) {
                return_code = 0x201D; // invalid parameter
              }
              break;
            case 0x1014:  // GetDevicePropDesc
              TRANSMIT(GetDevicePropDesc(CONTAINER->params[0]));
              break;
            case 0x1015:  // GetDevicePropvalue
              TRANSMIT(GetDevicePropValue(CONTAINER->params[0]));
              break;
            case 0x9803:  // GetObjectPropValue
              TRANSMIT(GetObjectPropValue(CONTAINER->params[0],
                                          CONTAINER->params[1]));
              break;
            default:
              return_code = 0x2005;  // operation not supported
              break;
          }
        } else {
          return_code = 0x2000;  // undefined
        }
      }
      if (return_code) {
        CONTAINER->type = 3;
        CONTAINER->op = return_code;
        CONTAINER->params[0] = p1;
        PrintPacket(receive_buffer);
        usb_tx(MTP_TX_ENDPOINT, receive_buffer);
        receive_buffer = 0;
      } else {
        usb_free(receive_buffer);
      }
    }
    // Maybe put event handling inside mtp_yield()?
    if ((receive_buffer = usb_rx(MTP_EVENT_ENDPOINT))) {
      usb_free(receive_buffer);
    }
  }
};

#endif
