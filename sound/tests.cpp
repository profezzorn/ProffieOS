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
#define NO_REPEAT_RANDOM
#define AUDIO_RATE 44100
#define SCOPED_PROFILER() do {} while(0)

#define CHECK(X) do {						\
    if (!(X)) { fprintf(stderr, "%s failed, line %d\n", #X, __LINE__); exit(1); } \
} while(0)

#define CHECK_EQ(X, Y) do {						\
  auto x = (X);								\
  auto y = (Y);								\
  if (x != y) { std::cerr << #X << " (" << x << ") != " << #Y << " (" << y << ") line " << __LINE__ << "\n";  exit(1); } \
} while(0)

#define CHECK_STREQ(X, Y) do {						\
  auto x = (X);								\
  auto y = (Y);								\
  if (strcmp(x, y)) { std::cerr << #X << " (" << x << ") != " << #Y << " (" << y << ") line " << __LINE__;  exit(1); } \
} while(0)


bool myglob(const char* pattern, const char* s) {
  for (;*pattern;pattern++,s++) {
    switch (*pattern) {
    case '*':
      if (myglob(pattern + 1, s)) return true;
      pattern--;
      break;
    case '#': if (*s < '0' || *s > '9') return false;
    case '?': break;
    default: if (*s != *pattern) return false;
    }
    if (!*s) return false;
  }
  return *s == 0;
}

#define CHECK_GLOB(PAT, Y) do {						\
  auto p = (PAT);								\
  auto y = (Y);								\
  if (!myglob(p, y)) { std::cerr << #Y << " (" << y << ") does not match '"  << p << "' line " << __LINE__;  exit(1); } \
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
int16_t clamptoi16(int32_t x) {
  return clampi32(x, -32768, 32767);
}
int32_t clamptoi24(int32_t x) {
  return clampi32(x, -8388608, 8388607);
}
// clamp(x, a, b) makes sure that x is between a and b.
float clamp(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
float Fmod(float a, float b) {
  return a - floorf(a / b) * b;
}

class Looper {
public:
  static void DoHFLoop() {}
};

char* itoa( int value, char *string, int radix )
{
  sprintf(string, "%d", value);
  return string;
}

// This really ought to be a typedef, but it causes problems I don't understand.
#define StyleAllocator class StyleFactory*

#include "../common/linked_ptr.h"
#include "../common/strfun.h"
#include "../common/lsfs.h"
#include "../common/monitoring.h"
Monitoring monitor;
#include "../common/stdout.h"
#include "../common/errors.h"



Print standard_print;
Print* default_output = &standard_print;
Print* stdout_output = &standard_print;
ConsoleHelper STDOUT;

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

enum EFFECTS {
  EFFECT_SD_CARD_NOT_FOUND,
  EFFECT_FONT_DIRECTORY_NOT_FOUND,
  EFFECT_ERROR_IN_BLADE_ARRAY,
  EFFECT_ERROR_IN_FONT_DIRECTORY,
};

class SaberBase {
public:
  static int sound_number;
  static void DoEffect(int x, float y) {}
};

int SaberBase::sound_number = -1;

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
  if (!f) {
    fprintf(stderr, "Failed to create file: %s\n", filename);
    exit(1);
  }
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

  mktestdir();
  mkdir("testfont", -1);
  mkdir("testfont/alt000", -1);
  mkdir("testfont/alt001", -1);
  mkdir("testfont/alt002", -1);
  touch("testfont/alt000/hum.wav");
  touch("testfont/alt001/hum.wav");
  touch("testfont/alt002/hum.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(1, SFX_hum.files_found());
  CHECK_EQ(3, num_alternatives);

  mktestdir();
  mkdir("testfont/hum", -1);
  mkdir("testfont/hum/001", -1);
  mkdir("testfont/hum/002", -1);
  mkdir("testfont/hum/003", -1);
  touch("testfont/hum/001/000.wav");
  touch("testfont/hum/001/001.wav");
  touch("testfont/hum/001/002.wav");
  touch("testfont/hum/002/000.wav");
  touch("testfont/hum/002/001.wav");
  touch("testfont/hum/002/002.wav");
  touch("testfont/hum/003/000.wav");
  touch("testfont/hum/003/001.wav");
  touch("testfont/hum/003/002.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(9, SFX_hum.expected_files());
  CHECK_EQ(3, SFX_hum.files_found());
  CHECK_EQ(0, num_alternatives);

  mktestdir();
  mkdir("testfont/hum", -1);
  mkdir("testfont/hum/000", -1);
  mkdir("testfont/hum/001", -1);
  mkdir("testfont/hum/002", -1);
  touch("testfont/hum/000/000.wav");
  touch("testfont/hum/000/001.wav");
  touch("testfont/hum/000/002.wav");
  touch("testfont/hum/001/000.wav");
  touch("testfont/hum/001/001.wav");
  touch("testfont/hum/001/002.wav");
  touch("testfont/hum/002/000.wav");
  touch("testfont/hum/002/001.wav");
  touch("testfont/hum/002/002.wav");
  Effect::ScanCurrentDirectory();
  CHECK_EQ(9, SFX_hum.expected_files());
  CHECK_EQ(3, SFX_hum.files_found());
  CHECK_EQ(0, num_alternatives);

  SFX_hum.Select(0);
  char name[256];
  SFX_hum.RandomFile().GetName(name);
  CHECK_GLOB("testfont/hum/000/###.wav", name);

  SFX_hum.Select(1);
  SFX_hum.RandomFile().GetName(name);
  CHECK_GLOB("testfont/hum/001/###.wav", name);

  SFX_hum.Select(2);
  SFX_hum.RandomFile().GetName(name);
  CHECK_GLOB("testfont/hum/002/###.wav", name);
}

#include "playwav.h"

#define STRIFY(X) std::string((char *)&(X), sizeof(X))

std::string mkchunk(std::string chnk,
		    std::string data) {
  std::string ret = chnk;
  uint32_t tmp = data.size();
  ret += STRIFY(tmp);
  ret += data;
  return ret;
}

void mktestsample(int hz, const char* filename) {
  std::string samples;
  int16_t sample = 0;
  for (int i = 0; i < 1024; i++) {
    samples += STRIFY(sample);
  }
  struct Fmt {
    uint16_t pcm = 1;
    uint16_t channels = 1;
    uint32_t rate = 0;
    uint32_t byterate = 0;
    uint16_t block_align = 2;
    uint16_t bits_per_sample = 16;
  };

  Fmt fmt;
  fmt.rate = hz;
  fmt.byterate = hz * 2;
  std::string wav = mkchunk("RIFF",
			    "WAVE" +
			    mkchunk("fmt ", STRIFY(fmt)) +
			    mkchunk("data", samples));
  FILE* f = fopen(filename, "wcb");
  fwrite(wav.c_str(), 1, wav.size(), f);
  fclose(f);
}

int readallsamples(PlayWav* wav) {
  int ret = 0;
  int16_t samples[44];

  while (!wav->eof()) {
    ret += wav->read(samples, 44);
  }

  return ret;
}

void test_playwav() {
  mktestsample(44100,"test44k.wav");
  mktestsample(22050,"test22k.wav");
  mktestsample(48000,"test48k.wav");

  PlayWav wav;
  
  wav.Play("test44k.wav");
  CHECK_EQ(1023, readallsamples(&wav));

  wav.Play("test22k.wav");
  CHECK_EQ(2046, readallsamples(&wav));

  // This should fail
  wav.Play("test48k.wav");
  CHECK_EQ(0, readallsamples(&wav));

  // This should pass
  wav.Play("test44k.wav");
  CHECK_EQ(1023, readallsamples(&wav));
}

int main() {
  test_effects();
  test_playwav();
}

#define PROFFIEOS_DEFINE_FUNCTION_STAGE
#include "../common/errors.h"
