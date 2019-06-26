#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// cruft
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
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

class Looper {
public:
  static void DoHFLoop() {}
};

template<class T, class U>
struct is_same_type { static const bool value = false; };
 
template<class T>
struct is_same_type<T, T> { static const bool value = true; };


// This really ought to be a typedef, but it causes problems I don't understand.
#define StyleAllocator class StyleFactory*

#include "../common/color.h"
#include "../blades/blade_base.h"
#include "cylon.h"
#include "style_ptr.h"
#include "colors.h"
#include "inout_helper.h"

bool on_ = true;

class MockBlade : public BladeBase {
public:
  std::vector<Color16> colors;

  int num_leds() const override { return colors.size(); }
  bool is_on() const override { return on_; }
  void set(int led, Color16 c) override {
//    fprintf(stderr, "SETILISET %d = %d %d %d\n", led, c.r, c.g, c.b);
    colors[led] = c;
  }
  void set_overdrive(int led, Color16 c) override {
    fprintf(stderr, "NOT IMPLEMENTED\n");
    exit(1);
  }
  size_t GetEffects(BladeEffect** blade_effects) override {
    return 0;
  }
  void allow_disable() override { }
  void Activate() override {
    fprintf(stderr, "NOT IMPLEMENTED\n");
    exit(1);
  }
};

struct TestResult {
  int black = 0;
  int white = 0;
  float balance = 0.0;
};

TestResult test_style(BladeStyle* s) {
  MockBlade mock_blade;
  mock_blade.colors.resize(100);
  s->run(&mock_blade);
  TestResult ret;
  float total = 0.0;
  for (int i = 0; i < 100; i++) {
    Color16 c = mock_blade.colors[i];
    int sum = c.r + c.g + c.b;
    if (sum == 0) ret.black++;
    if (sum >= 65200 * 3) ret.white++;
    ret.balance += sum * i;
    total += sum;
  }
  if (total > 0.0) ret.balance /= total;
  return ret;
};

void test_style(BladeStyle* s, uint32_t m, int black, int white, float balance) {
  micros_ = m;
  TestResult t = test_style(s);
  if (t.black != black || t.white != white || fabs(t.balance - balance) > 0.01) {
    fprintf(stderr, "Test failed, expected: black=%d white=%d balance=%f  got black=%d white=%d balance=%f\n",
	    black, white, balance,
	    t.black, t.white, t.balance);
    exit(1);
  }
  fprintf(stderr, "PASS\n");
}

void test_cylon() {
  Style<Cylon<WHITE, 10, 1> > t1;
  test_style(&t1, 0, 90, 10, 49.5);

  Style<Cylon<WHITE, 0, 1> > t2;
  test_style(&t2, 0, 100, 0, 0.0);

  Style<Cylon<WHITE, 100, 1> > t3;
  test_style(&t3, 0, 0, 100, 49.5);
}


void test_inouthelper() {
  Style<InOutHelper<Rgb16<65535,65535,65535>, 100, 100, Rgb16<0,0,0>>> t1;
  MockBlade mock_blade;
  mock_blade.colors.resize(1);
  on_ = false;
  micros_ = 0;
  t1.run(&mock_blade);
  if (mock_blade.colors[0].r != 0) {
    fprintf(stderr, "InOutHelper fails to make blade completely black.\n");
    exit(1);
  }
  on_ = true;
  for (int i = 0; i < 200; i++) {
    micros_ = i * 1000;
    t1.run(&mock_blade);
  }
  t1.run(&mock_blade);
  if (mock_blade.colors[0].r != 65535) {
    fprintf(stderr, "InOutHelper fails to make blade completely white: %d != %d\n", mock_blade.colors[0].r, 65535);
    exit(1);
  }
}

int main() {
  test_cylon();
  test_inouthelper();
}
