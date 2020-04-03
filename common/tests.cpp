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
int constexpr toLower(char x) {
  return (x >= 'A' && x <= 'Z') ? x - 'A' + 'a' : x;
}

class Looper {
public:
  virtual const char* name() = 0;
  virtual void Loop() = 0;
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

// Posix file primitives

#include "lsfs.h"

#define HEX 16

struct  Print {
  void print(const char* s) { puts(s); }
  void print(float v) { fprintf(stdout, "%f", v); }
  void print(int v, int base) { fprintf(stdout, "%d", v); }
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

#define LOCK_SD(X) do { } while(0)
#define noInterrupts() do{}while(0)
#define interrupts() do{}while(0)
#define SCOPED_PROFILER() do { } while(0)

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
  if (!(x_ < y_)) { std::cerr << #X << " (" << x_ << ") >= " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_GT(X, Y) do {                                             \
  auto x_ = (X);                                                                \
  auto y_ = (Y);                                                                \
  if (!(x_ > y_)) { std::cerr << #X << " (" << x_ << ") <= " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_STREQ(X, Y) do {                                          \
  auto x = (X);                                                         \
  auto y = (Y);                                                         \
  if (!x || !y || strcmp(x, y)) {                                       \
    std::cerr << #X << " (" << (x?x:"null") << ") != " << #Y << " (" << (y?y:"null") << ") line " << __LINE__ << std::endl;  exit(1); \
  }						                        \
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
  CHECK_NEAR(result.r, x.r, 2);
  CHECK_NEAR(result.g, x.g, 2);
  CHECK_NEAR(result.b, x.b, 2);
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

int main() {
  fuse_tests();
  test_rotate();
  test_current_preset();
  byteorder_tests();
  extrapolator_test();
}
