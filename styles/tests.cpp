#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// cruft
#define NELEM(X) (sizeof(X)/sizeof((X)[0]))
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

int random(int x) { return (random(x) & 0x7fffff) % x; }
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

struct SaberBase {
  static uint32_t GetCurrentVariation() {
    return 0;
  }
};

#define HEX 16

struct  Print {
  void print(const char* s) { puts(s); }
  void print(int v, int base) { fprintf(stdout, "%d", v); }
  void print(float v) { fprintf(stdout, "%f", v); }
  void write(char s) { putchar(s); }
  template<class T>
  void println(T s) { print(s); putchar('\n'); }
};


#include "../common/color.h"
#include "../blades/blade_base.h"
#include "cylon.h"
#include "style_ptr.h"
#include "colors.h"
#include "inout_helper.h"
#include "blast.h"
#include "transition_effect.h"
#include "../transitions/base.h"
#include "../transitions/join.h"
#include "../transitions/wipe.h"
#include "../transitions/delay.h"
#include "../transitions/concat.h"
#include "../transitions/fade.h"
#include "../transitions/instant.h"
#include "../functions/bump.h"

bool on_ = true;
bool allow_disable_ = false;

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
  void allow_disable() override {
    allow_disable_ = true;
  }
  void Activate() override {
    fprintf(stderr, "NOT IMPLEMENTED\n");
    exit(1);
  }
  void Deactivate() override {
    fprintf(stderr, "NOT IMPLEMENTED\n");
    exit(1);
  }
  Color8::Byteorder get_byteorder() const {
    return Color8::RGB;
  }
  bool IsPrimary() override {
    return true;
  }
  void SetStyle(BladeStyle* style) override {
    // current_style should be nullptr;
    current_style_ = style;
    if (current_style_) {
      current_style_->activate();
    }
  }

  BladeStyle* UnSetStyle() override {
    BladeStyle *ret = current_style_;
    if (ret) {
      ret->deactivate();
    }
    current_style_ = nullptr;
    return ret;
  }
  
  BladeStyle* current_style() const override {
    return current_style_;
  }
protected:
  BladeStyle *current_style_ = nullptr;
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


#define STEP() do {				\
  micros_ += 1000;				\
  /*  fprintf(stderr, "micros = %d on_ = %d\n", micros_, on_);	*/ 	\
  allow_disable_ = false;			\
  style->run(&mock_blade);			\
} while(0)

void test_inouthelper(BladeStyle* style) {
  MockBlade mock_blade;
  mock_blade.colors.resize(1);
  on_ = false;
  micros_ = 0;
  STEP();
  
  if (!allow_disable_) {
    fprintf(stderr, "Should be able to turn off after first run.\n");
    exit(1);
  }
  if (mock_blade.colors[0].r != 0) {
    fprintf(stderr, "InOutHelper fails to make blade completely black.\n");
    exit(1);
  }
  on_ = true;
  int last = 0;

  STEP();
  if (allow_disable_) {
    fprintf(stderr, "Should not be able to turn off when on.\n");
    exit(1);
  }

  for (int i = 0; i < 90; i++) {
    STEP();
    if (allow_disable_) {
      fprintf(stderr, "Should not be able to turn off when extending.\n");
      exit(1);
    }
    if (mock_blade.colors[0].r <= last || mock_blade.colors[0].r == 65536) {
      fprintf(stderr, "InOutHelper failed to brighten blade at t = %d red = %d last = %d\n", micros_, last, mock_blade.colors[0].r);
      exit(1);
    }
    last = mock_blade.colors[0].r;
  }
  for (int i = 0; i < 110; i++) {
    STEP();
    if (allow_disable_) {
      fprintf(stderr, "Should not be able to turn off when on.\n");
      exit(1);
    }
  }
  if (mock_blade.colors[0].r != 65535) {
    fprintf(stderr, "InOutHelper fails to make blade completely white: %d != %d\n", mock_blade.colors[0].r, 65535);
    exit(1);
  }
  last = 65535;
  on_ = false;
  STEP();
  for (int i = 0; i < 90; i++) {
    STEP();
    if (allow_disable_) {
      fprintf(stderr, "Should not be able to turn off when retracting.\n");
      exit(1);
    }
    if (mock_blade.colors[0].r >= last || mock_blade.colors[0].r == 0) {
      fprintf(stderr, "InOutHelper failed to dim blade at t = %d r = %d last = %d\n", micros_,
	      mock_blade.colors[0].r, last);
      exit(1);
    }
    last = mock_blade.colors[0].r;
  }
  for (int i = 0; i < 110; i++) {
    STEP();
  }
  if (!allow_disable_) {
    fprintf(stderr, "Should  be able to turn off when retracted.\n");
    exit(1);
  }
  if (mock_blade.colors[0].r != 0) {
    fprintf(stderr, "InOutHelper fails to make blade completely black.\n");
    exit(1);
  }
}

void test_inouthelper() {
  fprintf(stderr, "Testing InOutHelper...\n");
  Style<InOutHelper<Rgb16<65535,65535,65535>, 100, 100, Rgb16<0,0,0>>> t1;
  test_inouthelper(&t1);
  fprintf(stderr, "Testing InOutTr...\n");
  Style<InOutTr<Rgb16<65535,65535,65535>, TrWipe<100>, TrWipeIn<100>, Rgb16<0,0,0>>> t2;
  test_inouthelper(&t2);
  fprintf(stderr, "Testing InOutTr #2...\n");
  Style<InOutTr<Rgb16<65535,65535,65535>,
		TrWipe<100>,
		TrWipeIn<100>,
		Layers<Black,
		       TransitionEffectL<TrConcat<TrDelay<1500>, Black, TrFade<1000>, AlphaL<Red, Bump<Int<0>, Int<6000>>>,TrFade<3000>>, EFFECT_BOOT>,
		       TransitionEffectL<TrConcat<TrInstant, AlphaL<Red,Bump<Int<0>, Int<6000>>>,TrFade<3000>>,EFFECT_NEWFONT>>
		>> t3;
  test_inouthelper(&t3);
}

int main() {
  test_cylon();
  test_inouthelper();
}
