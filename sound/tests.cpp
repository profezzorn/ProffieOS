#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <memory.h>

#include <iostream>

#include <fcntl.h>


// cruft
#define NUM_BLADES 3
#define PROFFIE_TEST
#define ENABLE_SD

#define CHECK(X) do {						\
    if (!(X)) { fprintf(stderr, "%s failed, line %d\n", #X, __LINE__); exit(1); } \
} while(0)

#define CHECK_EQ(X, Y) do {						\
  auto x = (X);								\
  auto y = (Y);								\
  if (x != y) { std::cerr << #X << " (" << x << ") != " << #Y << " (" << y << ") line " << __LINE__;  exit(1); } \
} while(0)

#define CHECK_STREQ(X, Y) do {						\
  auto x = (X);								\
  auto y = (Y);								\
  if (strcmp(x, y)) { std::cerr << #X << " (" << x << ") != " << #Y << " (" << y << ") line " << __LINE__;  exit(1); } \
} while(0)


float fract(float x) { return x - floor(x); }

uint32_t micros_ = 0;
uint32_t micros() { return micros_; }
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

class Looper {
public:
  static void DoHFLoop() {}
};

char* itoa( int value, char *string, int radix )
{
  static char ret[33];
  sprintf(ret, "%d", value);
  return ret;
}

// This really ought to be a typedef, but it causes problems I don't understand.
#define StyleAllocator class StyleFactory*

#include "../common/linked_ptr.h"
#include "../common/strfun.h"


#define COMMON_LSFS_H
// Posix file primitives

class DoCloseFile {
public:
  static void Free(FILE* f) { if(f) fclose(f); }
};

class DoCloseDir {
public:
  static void Free(DIR* dir) { if(dir) closedir(dir); }
};

class File {
public:
  File() : file_() {}
  File(FILE* f) : file_(f) {}
  operator bool() const { return !!file_; }
  void close() { file_ = NULL; }
  int read(uint8_t *dest, size_t bytes) {
    return fread(dest, 1, bytes, file_.get());
  }
  int write(const uint8_t *dest, size_t bytes) {
    return fwrite(dest, 1, bytes, file_.get());
  }
  void seek(size_t pos) {
    fseek(file_.get(), pos, SEEK_SET);
  }
  uint32_t position() {
    return ftell(file_.get());
  }
  uint32_t available() {
    long pos = position();
    fseek(file_.get(), 0, SEEK_END);
    long end = position();
    seek(pos);
    return end - pos;
  }
  uint32_t size() {
    long pos = position();
    fseek(file_.get(), 0, SEEK_END);
    long end = position();
    seek(pos);
    return end;
  }
  int peek() {
    long pos = position();
    uint8_t ret;
    read(&ret, 1);
    seek(pos);
    return ret;
  }
  LinkedPtr<FILE, DoCloseFile> file_;
};


class LSFS {
public:
  typedef File FILE;
  static bool Begin() { return true; }
  static bool End() { return true; }
  static bool Exists(const char* path) {
    struct stat s;
    return stat(path, &s) == 0;
  }
  static bool Remove(const char* path) {
    return unlink(path) == 0;
  }
  static File Open(const char* path) {
    return fopen(path, "r");
  }
  static File OpenFast(const char* path) {
    return fopen(path, "r");
  }
  static File OpenForWrite(const char* path) {
    return fopen(path, "wct");
  }
  class Iterator {
  public:
    explicit Iterator(const char* dirname) {
      dir_ = opendir(dirname);
      CHECK(dir_);
      entry_ = readdir(dir_.get());
    }
    explicit Iterator(Iterator& other) {
      if (other.dir_) {
	dir_ = fdopendir(openat(dirfd(other.dir_.get()),
				other.entry_->d_name,
				O_RDONLY));
	CHECK(dir_);
	entry_ = readdir(dir_.get());
      }
    }
    void operator++() {
      CHECK(dir_);
      entry_ = readdir(dir_.get());
    }
    bool isdir() {
      CHECK(dir_);
      struct stat s;
      if (!strcmp(entry_->d_name, ".")) return false;
      if (!strcmp(entry_->d_name, "..")) return false;
      if (fstatat(dirfd(dir_.get()), entry_->d_name, &s, 0) != 0) return false;
      return S_ISDIR(s.st_mode);
    }
    operator bool() { return !!entry_; }
    // bool isdir() { return f_.isDirectory(); }
    const char* name() { return entry_->d_name; }
    // size_t size() { return f_.size(); }
    
  private:
    LinkedPtr<DIR, DoCloseDir> dir_;
    dirent* entry_;
  };
};

struct  Print {
  void print(const char* s) { puts(s); }
  void print(float v) { fprintf(stdout, "%f", v); }
  void write(char s) { putchar(s); }
  template<class T>
  void println(T s) { print(s); putchar('\n'); }
};

template<typename T, typename X = void> struct PrintHelper {
  static void out(Print& p, T& x) { p.print(x); }
};

template<typename T> struct PrintHelper<T, decltype(((T*)0)->printTo(*(Print*)0))> {
  static void out(Print& p, T& x) { x.printTo(p); }
};

struct ConsoleHelper : public Print {
  template<typename T, typename Enable = void>
  ConsoleHelper& operator<<(T v) {
    PrintHelper<T>::out(*this, v);
    return *this;
  }
};

ConsoleHelper STDOUT;
#define default_output (&STDOUT)
char current_directory[128] = "./";
#define LOCK_SD(X) do { } while(0)

struct TALKIEFAKE {
  int IGNORE;
};
TALKIEFAKE talkie;
#define Say(X,Y) IGNORE;

#include "effect.h"

void cleanup() {
  CHECK(!system("rm -rvf testfont >/dev/null || :"));
}

void mktestdir() {
  cleanup();
  mkdir("testfont", -1);
}

void touch(const char* filename) {
  FILE* f = fopen(filename, "w");
  CHECK(f);
  fclose(f);
}

void test_effects() {
  mktestdir();
  touch("testfont/hum.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/hum/._.hum.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(0, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hum.wav");
  touch("testfont/.DS_Store");
  touch("testfont/._.DS_Store");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hummus.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(0, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hum1.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hum99.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  touch("testfont/99blarg.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(0, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/.DS_Store");
  touch("testfont/._.DS_Store");
  touch("testfont/hum/.DS_Store");
  touch("testfont/hum/._.DS_Store");
  touch("testfont/hum/01.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/hum/hum01.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/.DS_Store");
  touch("testfont/._.DS_Store");
  touch("testfont/hum/.DS_Store");
  touch("testfont/hum/._.DS_Store");
  touch("testfont/hum/hum01.wav");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/hum/.DS_Store");
  touch("testfont/hum/._.DS_Store");
  Effect::ScanDirectory("testfont");
  CHECK_EQ(0, SFX_hum.files_found());
}

int main() {
  test_effects();
}
