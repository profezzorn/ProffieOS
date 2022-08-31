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
uint32_t millis() { return micros_ / 1000; }
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
// clamp(x, a, b) makes sure that x is between a and b.
float clamp(float x, float a, float b) {
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
#include "../common/lsfs.h"

char current_directory[128] = "testfont\0\0";
const char *next_current_directory(const char *dir) {
  return NULL;
}
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
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/hum/._.hum.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(0, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hum.wav");
  touch("testfont/.DS_Store");
  touch("testfont/._.DS_Store");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hummus.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(0, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hum1.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  touch("testfont/hum99.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  touch("testfont/99blarg.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(0, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/.DS_Store");
  touch("testfont/._.DS_Store");
  touch("testfont/hum/.DS_Store");
  touch("testfont/hum/._.DS_Store");
  touch("testfont/hum/01.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/hum/hum01.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/.DS_Store");
  touch("testfont/._.DS_Store");
  touch("testfont/hum/.DS_Store");
  touch("testfont/hum/._.DS_Store");
  touch("testfont/hum/hum01.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());

  mktestdir();
  mkdir("testfont/hum", -1);
  touch("testfont/hum/.DS_Store");
  touch("testfont/hum/._.DS_Store");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(0, SFX_hum.files_found());
}

int main() {
  test_effects();
}
