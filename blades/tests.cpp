#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define NOTEST

// cruft
#define SCOPED_PROFILER() do { } while(0)
template<class A, class B>
constexpr auto min(A&& a, B&& b) -> decltype(a < b ? std::forward<A>(a) : std::forward<B>(b)) {
  return a < b ? std::forward<A>(a) : std::forward<B>(b);
}
template<class A, class B>
constexpr auto max(A&& a, B&& b) -> decltype(a < b ? std::forward<A>(a) : std::forward<B>(b)) {
  return a >= b ? std::forward<A>(a) : std::forward<B>(b);
}
float fract(float x) { return x - floor(x); }

uint32_t micros_ = 0;
uint32_t micros() { return micros_; }
uint32_t millis() { return micros_ / 1000; }
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

struct BM {
  float battery() { return 3.7; }
};

BM battery_monitor;

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

#include "../common/monitoring.h"
#include "../common/color.h"
#include "drive_logic.h"
#include "leds.h"

#define EXPECT_EQ(X,Y) do {					\
  auto x = (X);							\
  auto y = (Y);							\
  if (x != y) {							\
    std::cerr << #X << " != " << #Y << " on line " << __LINE__ << std::endl; \
    std::cerr << #X << " = " << x  << std::endl;				\
    std:: cerr << #Y << " = " << y << std::endl;				\
    exit(1);							\
  }								\
} while(0)

struct CreeTest {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 3.4;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 3.1;
  static constexpr float R = 0.24;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
  typedef CreeXPE2White SUBTRACT;
//  typedef int CustomColorSelector;
};


void test_color_selector() {
  ColorSelector<CreeXPE2White> s1;
  EXPECT_EQ(0xFFFF, s1.Select(Color16(0xFFFF,0xFFFF,0xFFFF)));
  EXPECT_EQ(20, s1.Select(Color16(20,0xFFFF,0xFFFF)));
  EXPECT_EQ(20, s1.Select(Color16(200,20,0xFFFF)));
  EXPECT_EQ(20, s1.Select(Color16(100, 200,20)));
  ColorSelector<CreeXPE2Blue> s2;
  EXPECT_EQ(0xFFFF, s2.Select(Color16(0xFFFF,0xFFFF,0xFFFF)));
  EXPECT_EQ(0x0000, s2.Select(Color16(0xFFFF,0xFFFF,0x0000)));
  EXPECT_EQ(0xFFFF, s2.Select(Color16(0xFFFF,0x0000,0xFFFF)));
  ColorSelector<CreeTest> s3;
  EXPECT_EQ(0x0000, s3.Select(Color16(0xFFFF,0xFFFF,0xFFFF)));
  EXPECT_EQ(0x0000, s3.Select(Color16(0xFFFF,0xFFFF,0x0000)));
  EXPECT_EQ(0xFFFF, s3.Select(Color16(0xFFFF,0x0000,0xFFFF)));
}


int main() {
  test_color_selector();
}
