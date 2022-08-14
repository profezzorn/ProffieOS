#ifndef COMMON_FILE_READER_H
#define COMMON_FILE_READER_H

#include <new>

#include "common.h"
#include "lsfs.h"
#include "strfun.h"
#include "stdout.h"

// inline void* operator new(size_t, void* p) { return p; }

#ifdef ENABLE_SD
#define IF_SD(X) X
#else
#define IF_SD(X)
#endif

#ifdef ENABLE_SERIALFLASH
#define IF_SF(X) X
#else
#define IF_SF(X)
#endif

#define IF_MEM(X) X

#define RUN_ALL(X)				\
  switch (type_) {				\
    IF_SD(case TYPE_SD: return sd_file_.X;)	\
    IF_SF(case TYPE_SF: return sf_file_.X;)	\
    IF_MEM(case TYPE_MEM: return mem_file_.X;)	\
  }

#define RUN_ALL_VOID(X)				\
  switch (type_) {				\
    IF_SD(case TYPE_SD: sd_file_.X; return;)	\
    IF_SF(case TYPE_SF: sf_file_.X; return;)	\
    IF_MEM(case TYPE_MEM: mem_file_.X; return;)	\
  }

class MemFile {
public:
  MemFile(const uint8_t* data, size_t size) :
    data_(data), size_(size), pos_(0) {
  }
  MemFile() : data_(nullptr), size_(0), pos_(0) {
  }
  int read(uint8_t* dest, size_t bytes) {
    size_t to_copy = std::min<size_t>(available(), bytes);
    if (to_copy) {
      memcpy(dest, data_ + pos_, to_copy);
      pos_ += to_copy;
    }
    return to_copy;
  }
  int write(const uint8_t* dest, size_t bytes) { return 0; }
  size_t available() {
    return size_ - pos_;
  }
  int peek() {
    if (!available()) return -1;
    return data_[pos_];
  }
  void seek(size_t pos) {
    pos_ = std::min<size_t>(size_, pos);
  }
  size_t position() const {
    return pos_;
  }
  size_t size() const {
    return size_;
  }
  operator bool() const {
    return data_ != 0;
  }
  void close() {
    data_ = 0;
  }
private:
  const uint8_t* data_;
  size_t size_;
  size_t pos_;
};

// TODO: Make proper assignment or use std::variant instead.
class FileReader {
public:
  FileReader() : type_(TYPE_MEM) {
    mem_file_ = MemFile();
  }
  ~FileReader() { Close(); }
  bool Open(const char* filename) {
    Close();
#ifdef ENABLE_SERIALFLASH
    new (&sf_file_) SerialFlashFile;
    type_ = TYPE_SF;
    sf_file_ = SerialFlashChip::open(filename);
    if (sf_file_) {
      return true;
    } else {
      Close();
    }
#endif
#ifdef ENABLE_SD
    new (&sd_file_) File;
    type_ = TYPE_SD;
    sd_file_ = LSFS::Open(filename);
    if (sd_file_) {
      return true;
    } else {
      Close();
    }
#endif
    return false;
  }

  bool OpenFast(const char* filename) {
    Close();
#ifdef ENABLE_SERIALFLASH
    new (&sf_file_) SerialFlashFile;
    type_ = TYPE_SF;
    sf_file_ = SerialFlashChip::open(filename);
    if (sf_file_) {
      return true;
    } else {
      Close();
    }
#endif
#ifdef ENABLE_SD
    new (&sd_file_) File;
    type_ = TYPE_SD;
    sd_file_ = LSFS::OpenFast(filename);
    if (sd_file_) {
      return true;
    } else {
      Close();
    }
#endif
    return false;
  }
  bool Create(const char* filename) {
    Close();
#ifdef ENABLE_SD
    new (&sd_file_) File;
    sd_file_ = LSFS::OpenForWrite(filename);
    if (sd_file_) {
      type_ = TYPE_SD;
      return true;
    }
#endif
    return false;
  }
  bool OpenRW(const char* filename) {
    Close();
#ifdef ENABLE_SD
    new (&sd_file_) File;
    sd_file_ = LSFS::OpenRW(filename);
    if (sd_file_) {
      type_ = TYPE_SD;
      return true;
    }
#endif
    return false;
  }
  bool OpenMem(const uint8_t* data, uint32_t length) {
    Close();
    type_ = TYPE_MEM;
    MemFile tmp(data, length);
    mem_file_ = tmp;
    return true;
  }
  bool IsOpen() {
    switch (type_) {
      IF_SD(case TYPE_SD: return !!sd_file_;)
      IF_SF(case TYPE_SF: return !!sf_file_;)
      IF_MEM(case TYPE_MEM: return !!mem_file_;)
    }
    return false;
  };
  void Close() {
    switch (type_) {
      IF_SD(case TYPE_SD: sd_file_.close(); sd_file_.~File(); break;)
      IF_SF(case TYPE_SF: sf_file_.close(); sf_file_.~SerialFlashFile(); break;)
      IF_MEM(case TYPE_MEM: mem_file_.close(); mem_file_.~MemFile(); break;)
    }
    type_ = TYPE_MEM;
    mem_file_ = MemFile();
  }
  int Read(uint8_t* dest, int bytes) {
    RUN_ALL(read(dest, bytes))
    return 0;
  }
  int Read() {
    uint8_t tmp;
    if (Read(&tmp, 1)) {
      return tmp;
    } else {
      return -1;
    }
  }
  int Write(const uint8_t* dest, int bytes) {
    RUN_ALL(write(dest, bytes))
    return 0;
  }
  int Write(uint8_t c) { return Write(&c, 1); }
  int Write(const char *str) { return Write((uint8_t*)str, strlen(str)); }
  void Seek(uint32_t n) {
    RUN_ALL_VOID(seek(n))
  }
  uint32_t Available() {
    RUN_ALL(available());
    return 0;
  }
  uint32_t Tell() {
    RUN_ALL(position());
    return 0;
  }
  uint32_t FileSize() {
    RUN_ALL(size());
    return 0;
  }
  int Peek() {
    switch (type_) {
      IF_SD(case TYPE_SD: return sd_file_.peek(););
#ifdef ENABLE_SERIALFLASH
      case TYPE_SF: {
	uint8_t tmp;
	if (sf_file_.read(&tmp, 1)) {
	  sf_file_.seek(sf_file_.position() - 1);
	  return tmp;
	} else {
	  return -1;
	}
      }
#endif
      case TYPE_MEM:
	return mem_file_.peek();
    }
    return -1;
  }
  bool Expect(const char* str) {
    while (*str) {
      if (Peek() != *str) return false;
      Read();
      str++;
    }
    return true;
  }
  int AlignRead(int n) {
#ifdef ENABLE_SD
    if (type_ == TYPE_SD) {
      uint32_t pos = Tell();
      uint32_t next_block = (pos + 512u) & ~511u;
      int bytes_to_end_of_block = next_block - pos;
      return std::min<int>(n, bytes_to_end_of_block);
    }
#endif
    return n;
  }
  void Skip(int n) { Seek(Tell() + n); }
  void Rewind() { Seek(0); }

  void skipwhite() {
    while (true) {
      switch (Peek()) {
	case ' ':
	case '\t':
	case '\n':
	case '\r':
	  Read();
	  continue;
	default:
	  return;
      }
    }
  }
  void skipspace() {
    while (true) {
      switch (Peek()) {
	case ' ':
	case '\t':
	  Read();
	  continue;
	default:
	  return;
      }
    }
  }
  // Skip rest of line.
  void skipline() {
    while (Available() && Read() != '\n');
  }

  // Note: Byte order may be an issue!!
  // This code generally assumes it's running on a little-endian machine.
  template<typename T> T ReadType() {
    T ret;
    Read((uint8_t*)&ret, sizeof(ret));
    return ret;
  }

  int64_t readIntValue() {
    int64_t ret = 0;
    int64_t sign = 1;
    if (Peek() == '-') {
      sign = -1;
      Read();
    }
    while (Available()) {
      int c = Peek();
      if (c >= '0' && c <= '9') {
        ret = (c - '0') + 10 * ret;
        Read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  float readFloatValue() {
    float ret = 0.0;
    float sign = 1.0;
    float mult = 1.0;
    if (Peek() == '-') {
      sign = -1.0;
      Read();
    }
    while (Available()) {
      int c = Peek();
      if (c >= '0' && c <= '9') {
        if (mult == 1.0) {
          ret = (c - '0') + 10 * ret;
        } else {
          ret += (c - '0') * mult;
          mult /= 10.0;
        }
        Read();
      } else if (c == '.') {
        if (mult != 1.0) return ret * sign;
        // Time to read decimals.
        mult /= 10.0;
        Read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  bool readVariable(char variable[33]) {
    variable[0] = 0;
    skipspace();
    for (int i = 0; i < 32; i++) {
      int c = toLower(Peek());
      if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '.') {
	Read();
	variable[i] = c;
	variable[i+1] = 0;
      } else {
	break;
      }
    }
    skipspace();
    return true;
  }

  // LSPtr<> ?
  char* readString() {
    int len = 0;
    int space = 16;
    char* ret = (char*)malloc(space);
    while (Available()) {
      int c = Read();
      switch (c) {
	case '\n':
	  ret[len] = 0;
	  Seek(Tell() - 1);
	  return ret;
	case '\\':
	  switch (c = Read()) {
	    case '\\': ret[len++] = '\\'; break;
	    case 'n': ret[len++] = '\n'; break;
	    case 't': ret[len++] = '\t'; break;
	    default: ret[len++] = c; break;
	  }
	  break;
	default:
	  ret[len++] = c;
      }
      if (len == space) {
	int new_space = space * 3/2 + 8;
	char* tmp = (char*) realloc(ret, new_space);
	if (!tmp) {
	  STDOUT.println("Line too long.");
	  free(tmp);
	  return nullptr;
	}
	space = new_space;
	ret = tmp;
      }
    }
    ret[len] = 0;
    return ret;
  }

  void write_key_value(const char* key,
		       const char* value) {
    Write(key);
    Write('=');
    for (;*value; value++) {
      switch (*value) {
	case '\n': Write("\\n"); break;
	case '\t': Write("\\t"); break;
	case '\\': Write("\\\\"); break;
	default: Write(*value);
      }
    }
    Write('\n');
  }

  void write_key_value(const char* key, int v) {
    char value[30];
    itoa(v, value, 10);
    write_key_value(key, value);
  }
// to save float with two decimal places
  void write_key_value_float(const char* key, float f) {
    char new_value[16];
    itoa((int)floorf(f), new_value, 10);
    strcat(new_value, ".");
    itoa(((int)floorf(f * 10)) % 10, new_value + strlen(new_value), 10);
    itoa(((int)floorf(f * 100)) % 10, new_value + strlen(new_value), 10);
    write_key_value(key, new_value);
  }
private:
  enum {
#ifdef ENABLE_SD
    TYPE_SD,
#endif
#ifdef ENABLE_SERIALFLASH
    TYPE_SF,
#endif
    TYPE_MEM
  } type_;
  union {
    IF_SD(File sd_file_;)
    IF_SF(SerialFlashFile sf_file_;)
    MemFile mem_file_;
  };
};

class CheckSummer {
public:
  uint32_t checksum_ = 0;
  void Write(const uint8_t* data, int bytes) {
    uint32_t tmp = checksum_;
    for (int i = 0; i < bytes; i++) {
      tmp = tmp * 997 + data[i];
    }
    checksum_ = tmp;
  }
};

struct SafeFileHeader {
  uint32_t magic;
  uint32_t checksum;
  uint32_t iteration;
  uint32_t length;
};

struct SafeFileHeader2 {
  SafeFileHeader a;
  SafeFileHeader b;
  char installed[sizeof(install_time) - sizeof("")];
};

enum class ConfigFileExt {
  CONFIG_UNKNOWN,
  CONFIG_INI,
  CONFIG_TMP,
};

class FileValidator {
public:
  FileReader f;
  SafeFileHeader header;
  ConfigFileExt ext;

  FileValidator(const char* dir, const char* basename, ConfigFileExt x) : ext(x) {
    PathHelper full_name(dir, basename, ext == ConfigFileExt::CONFIG_INI ? "ini" : "tmp");
    f.Open(full_name);
    header.magic = 0;
    header.length = 0;
    SafeFileHeader2 tmp;
    memset(&tmp, 0, sizeof(tmp));
    f.Read((uint8_t*)&tmp, sizeof(tmp));
    if (memcmp(&tmp.a, &tmp.b, sizeof(tmp.a))) {
      return;
    }
#ifndef KEEP_SAVEFILES_WHEN_PROGRAMMING
    if (memcmp(&tmp.installed, install_time, sizeof(tmp.installed))) {
      return;
    }
#endif
    header = tmp.a;
    f.Seek(512);
  }

  bool validHeader() const {
    return header.magic == 0xFF1E5AFE;
  }

  int32_t iteration() const {
    return validHeader() ? header.iteration : 0;
  }

  bool validateChecksum() {
    if (!validHeader()) return false;
    uint8_t buffer[512];
    int l = header.length;
    CheckSummer checksum;
    while (l > 0) {
      int n = std::min<int>(l, NELEM(buffer));
      f.Read(buffer, n);
      checksum.Write(buffer, n);
      l -= n;
    }
    f.Seek(512);
    return header.checksum == checksum.checksum_;
  }
};


class FileSelector {
public:
  FileValidator ini;
  FileValidator tmp;
  FileValidator *a, *b;
  FileSelector(const char* dir, const char* basename) :
    ini(dir, basename, ConfigFileExt::CONFIG_INI),
    tmp(dir, basename, ConfigFileExt::CONFIG_TMP) {
    a = &ini;
    b = &tmp;
    
    if (ini.iteration() < tmp.iteration()) {
      a = &tmp;
      b = &ini;
    }
  }
  
  void Close() {
    ini.f.Close();
    tmp.f.Close();
  }
  ~FileSelector() {
    ini.f.Close();
    tmp.f.Close();
  }
};

#ifndef MAX_CONFIG_FILE_SIZE
#define MAX_CONFIG_FILE_SIZE (256  * 1024)
#endif

class BufferedFileWriter {
private:
  FileReader f;
  CheckSummer checksum;
#ifdef PROFFIEOS_BUFFER_WRITES
  uint32_t position = 0;
  uint8_t buffer[512];
#endif  

public:

  BufferedFileWriter(const char* path) {
    f.OpenRW(path);
    if (!f.IsOpen()) {
      STDOUT << "OpenRW fail: " << path << "\n";
      return;
    }
    // Allocate file size.
    if (f.FileSize() < MAX_CONFIG_FILE_SIZE) {
#ifndef PROFFIEOS_BUFFER_WRITES
      uint8_t buffer[512];
#endif
      memset(buffer, 0, sizeof(buffer));
      for (int i = (MAX_CONFIG_FILE_SIZE - f.FileSize() + 511) / 512; i > 0; i--) {
	f.Write(buffer, NELEM(buffer));
      }
    }
    f.Seek(512);
  }

  void Flush() {
#ifdef PROFFIEOS_BUFFER_WRITES    
    uint32_t pos_in_buffer = position & (NELEM(buffer)-1);
    if (pos_in_buffer) {
      memset(buffer + pos_in_buffer, 0, NELEM(buffer) - pos_in_buffer);
      int ret = f.Write(buffer, NELEM(buffer));
      if (ret != NELEM(buffer)) {
	STDERR << "FILE WRITE FAILED: " << ret << "\n";
      }
    }
#endif
  }

  void Close(uint32_t iteration) {
#ifdef PROFFIEOS_BUFFER_WRITES
    uint32_t length = position;
#else    
    uint32_t length = f.Tell() - 512;
#endif    
    uint32_t cksum = checksum.checksum_;
    Flush();
    f.Seek(0);
    // position = 0;
    Write32(0xFF1E5AFE); Write32(cksum); Write32(iteration); Write32(length);
    Write32(0xFF1E5AFE); Write32(cksum); Write32(iteration); Write32(length);
    Write(install_time);
    Flush();
    f.Close();
  }
  
  int Write(const uint8_t* dest, int bytes) {
    checksum.Write(dest, bytes);
#ifdef PROFFIEOS_BUFFER_WRITES    
    while (bytes) {
      uint32_t pos_in_buffer = position & (NELEM(buffer)-1);
      uint32_t to_copy = std::min<uint32_t>(bytes, NELEM(buffer) - pos_in_buffer);
      memcpy(buffer + pos_in_buffer, dest, bytes);

      position += to_copy;
      bytes -= to_copy;
      dest -= to_copy;

      if ((pos_in_buffer + to_copy) == NELEM(buffer)) {
	f.Write(buffer, NELEM(buffer));
      }
    }
#else
    f.Write(dest, bytes);
#endif    
    return 0;
  }
  int Write(uint8_t c) { return Write(&c, 1); }
  int Write32(uint32_t c) { return Write((uint8_t*)&c, 4); }
  int Write(const char *str) { return Write((uint8_t*)str, strlen(str)); }
  
  void write_key_value(const char* key,
		       const char* value) {
    Write(key);
    Write('=');
    for (;*value; value++) {
      switch (*value) {
	case '\n': Write("\\n"); break;
	case '\t': Write("\\t"); break;
	case '\\': Write("\\\\"); break;
	default: Write(*value);
      }
    }
    Write('\n');
  }

  void write_key_value(const char* key, int v) {
    char value[30];
    itoa(v, value, 10);
    write_key_value(key, value);
  }
// to save float with two decimal places
  void write_key_value_float(const char* key, float f) {
    char new_value[16];
    itoa((int)floorf(f), new_value, 10);
    strcat(new_value, ".");
    itoa(((int)floorf(f * 10)) % 10, new_value + strlen(new_value), 10);
    itoa(((int)floorf(f * 100)) % 10, new_value + strlen(new_value), 10);
    write_key_value(key, new_value);
  }
};

#endif
