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

#define KEEP_SAVEFILES_WHEN_PROGRAMMING

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
#include "fuse.h"

SaberBase* saberbases = NULL;
SaberBase::LockupType SaberBase::lockup_ = SaberBase::LOCKUP_NONE;
bool SaberBase::on_ = false;
uint32_t SaberBase::last_motion_request_ = 0;
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

#define CHECK_NEAR(X, Y, D) do {                                                \
  auto x_ = (X);                                                                \
  auto y_ = (Y);                                                                \
  if (fabs(x_ - y_) > D) { std::cerr << #X << " (" << x_ << ") ~!= " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_LT(X, Y) do {                                             \
  auto x_ = (X);                                                                \
  auto y_ = (Y);                                                                \
  if (!(x_ < y_)) { std::cerr << #X << " (" << x_ << ") < " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_LE(X, Y) do {                                             \
  auto x_ = (X);                                                                \
  auto y_ = (Y);                                                                \
  if (!(x_ <= y_)) { std::cerr << #X << " (" << x_ << ") <= " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_GT(X, Y) do {                                             \
  auto x_ = (X);                                                                \
  auto y_ = (Y);                                                                \
  if (!(x_ > y_)) { std::cerr << #X << " (" << x_ << ") > " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_STREQ(X, Y) do {                                          \
  auto x = (X);                                                         \
  auto y = (Y);                                                         \
  if (!x || !y || strcmp(x, y)) {                                       \
    std::cerr << #X << " (" << (x?x:"null") << ") != " << #Y << " (" << (y?y:"null") << ") line " << __LINE__ << std::endl;  exit(1); \
  }						                        \
} while(0)

bool extras = false;

void create_test_presets_ini(const char* filename, int presets, bool finish, const char* it) {
  FILE* f = fopen(filename, "wct");
  CHECK(f);
  if (it) 
    fprintf(f, "installed=%s\n", it);
  if (extras) fprintf(f, "unknown_variable=blarg\n");
  for (int i = 0; i < presets; i++) {
    fprintf(f, "new_preset\n");
    fprintf(f, "FONT=font%d\n", i);
    fprintf(f, "TRACK=track%d\n", i);
    fprintf(f, "STYLE=style%d:1\n", i);
    if (extras) fprintf(f, "unknown_variable=blarg\n");
    fprintf(f, "STYLE=style%d:2\n", i);
    fprintf(f, "STYLE=style%d:3\n", i);
    fprintf(f, "NAME=preset%d\n", i);
    fprintf(f, "VARIATION=%d\n", i);
    if (extras) fprintf(f, "unknown_variable=blarg\n");
  }
  if (extras) fprintf(f, "unknown_variable=blarg\n");
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
  CurrentPreset p;
  p.Load(0);

  CurrentPreset preset;
  // Cleanup
  RemovePresetINI();
  create_test_presets_ini("presets.ini", 5, true, install_time);
  CHECK(preset.Load(0));
  CHECK_EQ(preset.preset_num, 0);
  CHECK_STREQ(preset.font.get(), "font0");
  CHECK_STREQ(preset.track.get(), "track0");
  CHECK_STREQ(preset.current_style_[0].get(), "style0:1");
  CHECK_STREQ(preset.current_style_[1].get(), "style0:2");
  CHECK_STREQ(preset.current_style_[2].get(), "style0:3");
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
  create_test_presets_ini("presets.ini", 5, false, install_time);
  CHECK(!preset.Load(0));

  // Wrong install time presets.ini, do not load.
  create_test_presets_ini("presets.ini", 5, false, "yesterday");
  CHECK(!preset.Load(0));

  // No install time, do not load.
  create_test_presets_ini("presets.ini", 5, false, NULL);
  CHECK(!preset.Load(0));

  // Terminated tmp file, move and load
  create_test_presets_ini("presets.tmp", 5, true, install_time);
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

void test_rotate(Color16 c, int angle) {
  Color16 x = c.rotate(angle);
  double R = c.r/65535.0;
  double G = c.g/65535.0;
  double B = c.b/65535.0;
  double Max = std::max(R, std::max(G, B));
  double Min = std::min(R, std::min(G, B));
  double H;
  if (Max == Min) {
    H = 0.0;
  } else if (R == Max) {
    H = (G - B) / (Max - Min);
  } else if (G == Max) {
    H = 2 + (B - R) / (Max - Min);
  } else if (B == Max) {
    H = 4 + (R - G) / (Max - Min);
  }
  H *= 60;
  double S = Max == 0 ? 0 : (Max - Min) / Max;
  double V = Max;

  // We have HSV
  H += angle * 360.0 / (32768 * 3);
  while (H < 0) H += 360;
  while (H >= 360.0) H -= 360;

  double Hp =H / 60.0;
  double Hp2 = Hp;
  while (Hp2 > 2.0) Hp2-=2.0; // mod 2
  double C = V * S;
  double X = C * (1 - fabs(Hp2 - 1));

  Color16 y;
  if (Hp <= 1)      { R=C; G=X; B=0; }
  else if (Hp <= 2) { R=X; G=C; B=0; }
  else if (Hp <= 3) { R=0; G=C; B=X; }
  else if (Hp <= 4) { R=0; G=X; B=C; }
  else if (Hp <= 5) { R=X; G=0; B=C; }
  else              { R=C; G=0; B=X; }
  R+=V - C;
  G+=V - C;
  B+=V - C;
  Color16 result(R*65535, G*65535, B*65535);
  CHECK_NEAR(result.r, x.r, 5);
  CHECK_NEAR(result.g, x.g, 5);
  CHECK_NEAR(result.b, x.b, 5);

  // Test HSL
  HSL hsl = c.toHSL();
  hsl = hsl.rotate(angle / (float)(32768 * 3));
//  fprintf(stderr, "Angle = %d HSL={%f,%f,%f} RGB=%d,%d,%d\n", angle, hsl.H, hsl.S, hsl.L, c.r, c.g, c.b);
  CHECK_LE(0.0, hsl.H);
  CHECK_LE(0.0, hsl.S);
  CHECK_LE(0.0, hsl.L);
  CHECK_LE(hsl.H, 1.0);
  CHECK_LE(hsl.S, 1.0);
  CHECK_LE(hsl.L, 1.0);
  Color16 result2(hsl);
//  fprintf(stderr, "result2 = %d, %d, %d\n", result2.r, result2.g, result2.b);
  CHECK_NEAR(result2.r, x.r, 5);
  CHECK_NEAR(result2.g, x.g, 5);
  CHECK_NEAR(result2.b, x.b, 5);
}

void test_rotate(Color16 c) {
  test_rotate(c, 0);
  test_rotate(c, 32768);
  test_rotate(c, 100);
  test_rotate(c, 1000);
  test_rotate(c, 10000);
  test_rotate(c, 80000);
  test_rotate(c, 90000);
  test_rotate(c, 32768 * 3 - 1);
}

void test_rotate() {
  test_rotate(Color16(65535,65535,65535));
  test_rotate(Color16(0,65535,65535));
  test_rotate(Color16(65535,0,65535));
  test_rotate(Color16(65535,65535,0));
  test_rotate(Color16(65535,0,0));
  test_rotate(Color16(0,65535,0));
  test_rotate(Color16(0,0,65535));
  test_rotate(Color16(Color8(0,135,255)));
}


void extrapolator_test() {
  Extrapolator<float> x;
  x.push(0.0, 0);
  x.push(1.0, 1);
  x.push(2.0, 2);
  x.push(3.0, 3);
  x.push(4.0, 4);
  x.push(5.0, 5);
  x.push(6.0, 6);
  x.push(7.0, 7);
  x.push(8.0, 8);
  x.push(9.0, 9);
  CHECK_GT(x.slope(), 0.99);
  CHECK_LT(x.slope(), 1.01);
  CHECK_GT(x.get(10),  9.99);
  CHECK_LT(x.get(10), 10.01);
  CHECK_GT(x.get(11), 10.99);
  CHECK_LT(x.get(11), 11.01);

  x.push(-0.0, 0);
  x.push(-1.0, 1);
  x.push(-2.0, 2);
  x.push(-3.0, 3);
  x.push(-4.0, 4);
  x.push(-5.0, 5);
  x.push(-6.0, 6);
  x.push(-7.0, 7);
  x.push(-8.0, 8);
  x.push(-9.0, 9);
  CHECK_GT(-x.slope(), 0.99);
  CHECK_LT(-x.slope(), 1.01);
  CHECK_GT(-x.get(10),  9.99);
  CHECK_LT(-x.get(10), 10.01);
  CHECK_GT(-x.get(11), 10.99);
  CHECK_LT(-x.get(11), 11.01);

  x.push(-1.0, 0);
  x.push( 1.0, 1);
  x.push(-1.0, 2);
  x.push( 1.0, 3);
  x.push(-1.0, 4);
  x.push( 1.0, 5);
  x.push(-1.0, 6);
  x.push( 1.0, 7);
  x.push(-1.0, 8);
  x.push( 1.0, 9);
  CHECK_GT(x.slope(), 0.0);
  CHECK_LT(x.slope(), 0.1);
  CHECK_GT(x.get(10), 0.0);
  CHECK_LT(x.get(10), 0.5);
  CHECK_GT(x.get(11), 0.0);
  CHECK_LT(x.get(11), 0.5);
}

#ifdef FUSE_SPEED

#define FUSE_DEBUG
#define XYZL(X) (X).x, (X).y, (X).z, (X).len()

class FuseTester {
public:
  Fusor fuse;

  void motion(float x, float y, float z) {
    fuse.SB_Motion(Vec3(x, y, z), false);
#ifdef FUSE_DEBUG
    fprintf(stderr," motion %f %f %f\n", x, y, z);
#endif
  }
  void accel(float x, float y, float z) {
    fuse.SB_Accel(Vec3(x, y, z), false);
#ifdef FUSE_DEBUG
    fprintf(stderr," accel %f %f %f\n", x, y, z);
#endif
  }

  void init() {
#ifdef FUSE_DEBUG
    fprintf(stderr," INIT FUSOR\n");
#endif
    fuse = Fusor();

    fuse.SB_Motion(Vec3(0,0,0), true);
    fuse.SB_Accel(Vec3(0,0,1), true);
    for (int i = 0; i < 10; i++) {
      micros_ += 1000;
      motion(0,0,0);
      accel(0,0,1);
    }
  }

  void dump(const char *x) {
#ifdef FUSE_DEBUG
    fprintf(stderr, "%s SPEED: %f, %f, %f  (%f) DOWN: %f, %f, %f  (%f)  MSS: %f, %f, %f, (%f)\n",
            x,
            XYZL(fuse.speed()),
            XYZL(fuse.down()),
            XYZL(fuse.mss()));
#endif
  }

  void fuse_rotate_test() {
    init();

    // 1-second rotation test
    for (int i = 1; i < 104; i++) {
      micros_ += 10000;
      motion(360.0f,0.0f,0.0f); // 360 degrees / second
      float angle = M_PI * 2 * i / 100.0;
      accel(0.0f, sinf(angle), cosf(angle));
      fuse.Loop();
      dump("rot");
    }

    CHECK_LT(fuse.speed().len(), 0.6);
  }

  void fuse_translate_test() {
    init();

    // 1-second translation test
    for (int i = 0; i < 250; i++) {
      micros_ += 1000;
      motion(0.0f,0.0f,0.0f);
      accel(1.0f, 0.0f, 1.0f);
      fuse.Loop();
      dump("GO");
    }
    for (int i = 0; i < 500; i++) {
      micros_ += 1000;
      motion(0.0f,0.0f,0.0f);
      accel(0.0f, 0.0f, 1.0f);
      fuse.Loop();
      dump("coast");
      CHECK_GT(fuse.speed().len(), 1);
      CHECK_LT(fuse.speed().len(), 2.5);
    }
    for (int i = 0; i < 250; i++) {
      micros_ += 1000;
      motion(0.0f,0.0f,0.0f);
      accel(-1.0f, 0.0f, 1.0f);
      fuse.Loop();
      dump("STOP");
    }
    CHECK_LT(fuse.speed().len(), 1.0);
  }

  void run() {
    fuse_rotate_test();
    fuse_translate_test();
  }
};

void fuse_tests() {
  FuseTester a;
  a.run();
}

#else // FUSE_SPEED
void fuse_tests() {}
// TODO: Write fuse tests that don't rely on speed()
#endif

std::set<int> N16;
std::set<int> N15;
int64_t tests = 0;

#define CHECK_RET(EXTRAOUT)	do {					\
  if (ret.c.r > ret.alpha * 2 || ret.alpha > 32768) {			\
    STDOUT << " ret = " << ret << " @ " << __LINE__ << " " << EXTRAOUT << "\n"; \
    exit(1);								\
  }									\
} while (0)

void add2set(int i) {
  if (i < 0) return;
  if (i > 65535) return;
  N16.insert(i);
  if (i <= 32768) N15.insert(i);
}

template<class T1, class T2>
void runPaintTest(T1 A, T2 B) {
  RGBA ret = A << B;
  tests++;
  CHECK_RET("A = " << A << "B = " << B);

  // Test mixing
  for (int d : N15) {
    tests++;
    ret = MixColors(A, B, d, 15);
    CHECK_RET("A = " << A << " B = " << B << " mix = " << d);
  }

  // Test mixing
  for (int d : N15) {
    tests++;
    ret = MixColors(A, B, d>>1, 14);
    CHECK_RET("A = " << A << " B = " << B << " mix = " << (d >> 1));
  }
}

template<class T1>
void runPaintTest(T1 A) {
  for (int d : N15) {
    tests++;
    RGBA ret = A * d;
    CHECK_RET("A = " << A << " mult = " << d);
    
    for (int c : N16) {
      RGBA_um B(Color16(c,c,c), false, d);
      runPaintTest(A, B);
      RGBA_um_nod C(Color16(c,c,c), d);
      runPaintTest(A, C);
      if (c > d * 2) continue;
      RGBA B2(Color16(c,c,c), false, d);
      runPaintTest(A, B2);
    }
  }
}

void color_tests() {
  for (int i = 0; i < 65536; i = i + 1 + (i >> 2) + (i >> 5) * 4711) {
    add2set(i);
    add2set(65535 - i);
    add2set(32767 - i);
    add2set(32768 + i);
  }
  STDOUT << " N16 size = " << N16.size() << " N15 size = " << N15.size() << "\n";
  for (int a : N16) {
    for (int b : N15) {
      RGBA_um A(Color16(a,a,a), false, b);
      runPaintTest(A);
      RGBA_um_nod A3(Color16(a,a,a), b);
      runPaintTest(A3);
      if (a <= b * 2) {
	RGBA A2(Color16(a,a,a), false, b);
	runPaintTest(A2);
      }
    }
  }
  STDOUT << tests << " tests.\n";
}

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
  LSFS::Remove("testconfig.ini");
  LSFS::Remove("testconfig.tmp");
  TestConfigFile f;
  f.ReadINIFromRootDir("testconfig");

  for (int v = 5; v < 10; v++) {
    f.volume = v;
    f.WriteToRootDir("testconfig");
  
    TestConfigFile f2;
    f2.ReadINIFromRootDir("testconfig");
    CHECK_EQ(f2.volume, v);
  }
  
  f.WriteToRootDir("testconfig");
  f.WriteToRootDir("testconfig");
  f.WriteToRootDir("testconfig");
}

#include "command_parser.h"
CommandParser* parsers = NULL;

class TestParser : public CommandParser {
public:
  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp("lines", cmd)) {
      STDOUT << "C line\n";
      STDOUT << "F line\n";
      STDOUT << "D line\n";
      STDOUT << "B line\n";
      STDOUT << "A line\n";
      STDOUT << "E line\n";
      return true;
    }
    return false;
  }
};

TestParser test_parser;

void command_parser_test() {
  char tmp[100];
  char tmp2[100];
  int lines;
  lines = RunCommandAndGetSingleLine("lines", nullptr, 1, tmp, 100);
  CHECK_EQ(lines, 6);
  CHECK_STREQ(tmp, "C line");
  lines = RunCommandAndGetSingleLine("lines", nullptr, 2, tmp, 100);
  CHECK_EQ(lines, 6);
  CHECK_STREQ(tmp, "F line");
  lines = RunCommandAndGetSingleLine("lines", nullptr, 6, tmp, 100);
  CHECK_EQ(lines, 6);
  CHECK_STREQ(tmp, "E line");

  bool x;
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, nullptr, tmp, false);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp, "A line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp, tmp2, false);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp2, "B line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp2, tmp, false);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp, "C line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp, tmp2, false);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp2, "D line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp2, tmp, false);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp, "E line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp, tmp2, false);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp2, "F line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp2, tmp, false);
  CHECK_EQ(x, false);

  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, nullptr, tmp, true);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp, "F line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp, tmp2, true);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp2, "E line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp2, tmp, true);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp, "D line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp, tmp2, true);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp2, "C line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp2, tmp, true);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp, "B line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp, tmp2, true);
  CHECK_EQ(x, true);
  CHECK_STREQ(tmp2, "A line");
  x = RunCommandAndFindNextSortedLine<100>("lines", nullptr, tmp2, tmp, true);
  CHECK_EQ(x, false);
}

int main() {
  command_parser_test();
  
  extras = false;
  test_current_preset();
  fprintf(stderr, "Extra variables enabled....\n");
  extras = true;
  test_current_preset();

  config_file_tests();
  fuse_tests();
  test_rotate();
  byteorder_tests();
  extrapolator_test();
  color_tests();
}
