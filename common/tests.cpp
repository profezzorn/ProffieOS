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

// cruft
#define NUM_BLADES 3
#define PROFFIE_TEST
#define ENABLE_SD

float fract(float x) { return x - floor(x); }

uint32_t micros_ = 0;
uint32_t micros() { return micros_; }
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
int constexpr toLower(char x) {
  return (x >= 'A' && x <= 'Z') ? x - 'A' + 'a' : x;
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

#include "linked_ptr.h"


#define COMMON_LSFS_H
// Posix file primitives

class DoCloseFile {
public:
  static void Free(FILE* f) { if(f) fclose(f); }
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
  static File OpenForWrite(const char* path) {
    return fopen(path, "wct");
  }
  class Iterator {
  public:
    explicit Iterator(const char* dirname) {
      dir_ = opendir(dirname);
      entry_ = readdir(dir_);
    }
    explicit Iterator(Iterator& other) {
      dir_ = other.dir_;
      other.dir_ = nullptr;
      entry_ = other.entry_;
    }
    ~Iterator() {
      closedir(dir_);
    }
    void operator++() {
      entry_ = readdir(dir_);
    }
    operator bool() { return !!entry_; }
    // bool isdir() { return f_.isDirectory(); }
    const char* name() { return entry_->d_name; }
    // size_t size() { return f_.size(); }
    
  private:
    DIR* dir_;
    dirent* entry_;
  };
};

struct StdoutHelper {
  void print(const char* s) { puts(s); }
  void write(char s) { putchar(s); }
  template<class T>
  void println(T s) { print(s); putchar('\n'); }
};

#define LOCK_SD(X) do { } while(0)

StdoutHelper STDOUT;

void PrintQuotedValue(const char *name, const char* str) {
  STDOUT.print(name);
  STDOUT.write('=');
  if (str) {
    while (*str) {
      switch (*str) {
        case '\n':
          STDOUT.print("\\n");
          break;
        case '\\':
          STDOUT.write('\\');
        default:
          STDOUT.write(*str);
      }
      ++str;
    }
  }
  STDOUT.write('\n');
}


#include "current_preset.h"
#include "color.h"

BladeConfig* current_config;


#define CHECK(X) do {						\
    if (!(X)) { fprintf(stderr, "%s failed, line %d\n", #X, __LINE__); exit(1); } \
} while(0)

#define CHECK_EQ(X, Y) do {						\
  auto x = (X);								\
  auto y = (Y);								\
  if (x != y) { std::cerr << #X << " (" << x << ") != " << #Y << " (" << y << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_STREQ(X, Y) do {						\
  auto x = (X);								\
  auto y = (Y);								\
  if (strcmp(x, y)) { std::cerr << #X << " (" << x << ") != " << #Y << " (" << y << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)


void create_test_presets_ini(const char* filename, int presets, bool finish) {
  FILE* f = fopen(filename, "wct");
  CHECK(f);
  for (int i = 0; i < presets; i++) {
    fprintf(f, "new_preset\n");
    fprintf(f, "FONT=font%d\n", i);
    fprintf(f, "TRACK=track%d\n", i);
    fprintf(f, "STYLE=style%d:1\n", i);
    fprintf(f, "STYLE=style%d:2\n", i);
    fprintf(f, "STYLE=style%d:3\n", i);
    fprintf(f, "NAME=preset%d\n", i);
  }
  if (finish)
    fprintf(f, "END\n");
  fclose(f);
}

void RemovePresetINI() {
  LSFS::Remove("presets.ini");
  LSFS::Remove("presets.tmp");
  CurrentPreset preset;
  CHECK(!preset.Load(0));
}

int PresetOrder() {
  CurrentPreset tmp;
  int ret = 0;
  for (int i = 0; tmp.Load(i) && tmp.preset_num == i; i++) {
    int f;
    sscanf(tmp.font.get(), "font%d", &f);
    ret = ret * 10 + f;
  }
  return ret;
}

void test_current_preset() {
  CurrentPreset preset;
  // Cleanup
  RemovePresetINI();
  create_test_presets_ini("presets.ini", 5, true);
  CHECK(preset.Load(0));
  CHECK_EQ(preset.preset_num, 0);
  CHECK_STREQ(preset.font.get(), "font0");
  CHECK_STREQ(preset.track.get(), "track0");
  CHECK_STREQ(preset.current_style1.get(), "style0:1");
  CHECK_STREQ(preset.current_style2.get(), "style0:2");
  CHECK_STREQ(preset.current_style3.get(), "style0:3");
  CHECK_STREQ(preset.name.get(), "preset0");
  
  CHECK(preset.Load(1));
  CHECK_EQ(preset.preset_num, 1);
  CHECK(preset.Load(2));
  CHECK_EQ(preset.preset_num, 2);
  CHECK(preset.Load(3));
  CHECK_EQ(preset.preset_num, 3);
  CHECK(preset.Load(4));
  CHECK_EQ(preset.preset_num, 4);
  CHECK(preset.Load(5));
  CHECK_EQ(preset.preset_num, 0);
  CHECK(preset.Load(-1));
  CHECK_EQ(preset.preset_num, 4);
  RemovePresetINI();

  // Unterminated presets.ini, do not load.
  create_test_presets_ini("presets.ini", 5, false);
  CHECK(!preset.Load(0));

  // Terminated tmp file, move and load
  create_test_presets_ini("presets.tmp", 5, true);
  CHECK(preset.Load(0));
  CHECK(LSFS::Exists("presets.ini"));

  CHECK_EQ(PresetOrder(),  1234);
  preset.SaveAt(1);
  CHECK_EQ(PresetOrder(), 10234);
  CHECK_EQ(preset.preset_num, 1);
  preset.SaveAt(2);
  CHECK_EQ(PresetOrder(), 12034);
  CHECK_EQ(preset.preset_num, 2);
  preset.SaveAt(3);
  CHECK_EQ(PresetOrder(), 12304);
  CHECK_EQ(preset.preset_num, 3);
  preset.SaveAt(4);
  CHECK_EQ(PresetOrder(), 12340);
  CHECK_EQ(preset.preset_num, 4);
}

void test_byteorder(int byteorder) {
  std::cerr << "Testing " << byteorder <<  std::endl;
  CHECK_EQ(byteorder, Color8::combine_byteorder(Color8::RGB, byteorder));
  CHECK_EQ(Color8::RGB, Color8::combine_byteorder(Color8::invert_byteorder(byteorder), byteorder));
}

void byteorder_tests() {
  CHECK_EQ(Color8::invert_byteorder(Color8::RGB), Color8::RGB);
  CHECK_EQ(Color8::invert_byteorder(Color8::BGR), Color8::BGR);
  test_byteorder(Color8::BGR);
  test_byteorder(Color8::BRG);
  test_byteorder(Color8::GBR);
  test_byteorder(Color8::GRB);
  test_byteorder(Color8::RBG);
  test_byteorder(Color8::RGB);
}

int main() {
  test_current_preset();
  byteorder_tests();
}
