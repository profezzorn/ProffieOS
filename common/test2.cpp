#define INSTALL_TIME_EXTRA "-2"

#include <vector>
#include <set>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <memory.h>

#include <iostream>

// cruft
#define NUM_BLADES 3
#define PROFFIE_TEST
#define ENABLE_SD
#define GYRO_MEASUREMENTS_PER_SECOND 1600
#define ACCEL_MEASUREMENTS_PER_SECOND 1600
#define HEX 16

#define current_directory "."
#define next_current_directory(dir) nullptr
#define previous_current_directory(dir) nullptr
#define last_current_directory() "."

int random(int x) { return rand() % x; }

const char* GetSaveDir() { return NULL; }

float fract(float x) { return x - floor(x); }

uint64_t micros_ = 0;
uint32_t micros() { return micros_; }
uint32_t millis() { return micros_ / 1000; }
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

class Looper {
public:
  virtual const char* name() = 0;
  virtual void Loop() = 0;
  static void DoHFLoop() {}
};

char* itoa( int value, char *ret, int radix )
{
  sprintf(ret, "%d", value);
  return ret;
}

// This really ought to be a typedef, but it causes problems I don't understand.
#define StyleAllocator class StyleFactory*

#include "linked_ptr.h"

// Posix file primitives

#include "lsfs.h"

#define LOCK_SD(X) do { } while(0)
#define noInterrupts() do{}while(0)
#define interrupts() do{}while(0)
#define SCOPED_PROFILER() do { } while(0)


#include "stdout.h"
Print standard_print;
Print* default_output = &standard_print;
Print* stdout_output = &standard_print;
ConsoleHelper STDOUT;

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

#include "monitoring.h"
#include "current_preset.h"
#include "color.h"

Monitoring monitor;

BladeConfig* current_config;


#define CHECK(X) do {                                           \
    if (!(X)) { fprintf(stderr, "%s failed, line %d\n", #X, __LINE__); exit(1); } \
} while(0)

#define CHECK_EQ(X, Y) do {                                             \
  auto x_ = (X);                                                                \
  auto y_ = (Y);                                                                \
  if (x_ != y_) { std::cerr << #X << " (" << x_ << ") != " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)




#include "config_file.h"

class TestConfigFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(clash_threshold, 1.0);
    CONFIG_VARIABLE2(volume, -1);
    CONFIG_VARIABLE2(dimming, 16384);
  }
  float clash_threshold;
  int volume;
  int dimming;
};

void config_file_tests() {
  TestConfigFile f;
  f.ReadINIFromRootDir("testconfig");
  // Should be default value!
  CHECK_EQ(f.volume, -1);
}


int main() {
  config_file_tests();
}
