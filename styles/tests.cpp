#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>

// cruft
#define interrupts() do {} while(0)
#define noInterrupts() do {} while(0)
#define NELEM(X) (sizeof(X)/sizeof((X)[0]))
#define SCOPED_PROFILER() do { } while(0)

#define COMMON_FUSE_H

struct MockFuse {
  float angle1_ = 0.0;
  float angle1() { return angle1_; }
  float angle2_ = 0.0;
  float angle2() { return angle2_; }
  float swing_speed_ = 0.0;
  float swing_speed() { return swing_speed_; }
};

MockFuse fusor;

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

struct  Print {
  void print(const char* s) { fprintf(stdout, "%s", s); }
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

int random(int x) { return (rand() & 0x7fffff) % x; }
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

#define HEX 16

#define ENABLE_AUDIO

struct MockDynamicMixer {
  int32_t last_sample() const { return 4093; }
  int32_t last_sum() const { return 16384; }
  int32_t audio_volume() const { return 100000; }
};

MockDynamicMixer dynamic_mixer;

#include "../common/color.h"
#include "../blades/blade_base.h"
#include "cylon.h"
#include "style_ptr.h"
#include "colors.h"
#include "inout_helper.h"
#include "blast.h"
#include "transition_effect.h"
#include "audio_flicker.h"
#include "pulsing.h"
#include "../functions/bump.h"
#include "lockup.h"
#include "blinking.h"
#include "clash.h"
#include "color_cycle.h"
#include "../transitions/base.h"
#include "../transitions/join.h"
#include "../transitions/wipe.h"
#include "../transitions/delay.h"
#include "../transitions/concat.h"
#include "../transitions/fade.h"
#include "../transitions/instant.h"
#include "../functions/blade_angle.h"
#include "../functions/twist_angle.h"
#include "../functions/swing_speed.h"
#include "mix.h"
#include "strobe.h"
#include "hump_flicker.h"
#include "brown_noise_flicker.h"
#include "responsive_styles.h"

SaberBase* saberbases = NULL;
SaberBase::LockupType SaberBase::lockup_ = SaberBase::LOCKUP_NONE;
SaberBase::ColorChangeMode SaberBase::color_change_mode_ =
  SaberBase::COLOR_CHANGE_MODE_NONE;
bool SaberBase::on_ = false;
uint32_t SaberBase::last_motion_request_ = 0;
uint32_t SaberBase::current_variation_ = 0;

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
  mock_blade.SetStyle(style);
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

Color16 get_color_when_on(BladeStyle* style) {
  MockBlade mock_blade;
  mock_blade.SetStyle(style);
  mock_blade.colors.resize(1);
  on_ = false;
  micros_ = 0;
  STEP();
  on_ = true;
  int last = 0;

  for (int i = 0; i < 10000; i++) STEP();
  return mock_blade.colors[0];
}

Color16 get_melt_color(BladeStyle* style) {
  MockBlade mock_blade;
  mock_blade.SetStyle(style);
  mock_blade.colors.resize(10);
  on_ = false;
  micros_ = 0;
  STEP();
  on_ = true;
  int last = 0;

  STEP();
  SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
  SaberBase::DoBeginLockup();
  for (int i = 0; i < 10000; i++) STEP();
  return mock_blade.colors[9];
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

void test_style1() {
  Style<
   Layers<
     Green,
     LockupTrL<Pulsing<Yellow,Red,1000>,TrInstant,TrInstant,SaberBase::LOCKUP_MELT>,
     LockupL<Blinking<Green,Black,85,500>,AudioFlicker<Yellow,Blue>,Int<32768>,Int<32768>,Int<32768>> 
     >
    > t1;

  Color16 c = get_color_when_on(&t1);
  if (c.r != 0 || c.b != 0) {
    fprintf(stderr, "Expecting only green.\n");
    exit(1);
  }
  if (c.g < 32767) {
    fprintf(stderr, "Not green enough (%d).\n", c.g);
    exit(1);
  }
}


void test_style2() {
  Style<
   Layers<
	AudioFlicker<Rgb<0,128,0>,Green>,
	BlastFadeoutL<Blue,400>,
	LockupTrL<Pulsing<Yellow,Red,1000>,TrInstant,TrInstant,SaberBase::LOCKUP_MELT>,
	LockupL<Blinking<Green,Black,85,500>,AudioFlicker<Yellow,Blue>,Int<32768>,Int<32768>,Int<32768>>,
	SimpleClashL<Yellow,100,EFFECT_CLASH,SmoothStep<Int<0>,Int<24000>>>,
	InOutTrL<TrWipeSparkTip<Yellow,300,100>,TrInstant,Pulsing<ColorCycle<Green,10,10,Cyan,100,3000,5000>,Black,2500>>>
  > t1;

  Color16 c = get_color_when_on(&t1);
  if (c.r != 0 || c.b != 0) {
    fprintf(stderr, "Expecting only green.\n");
    exit(1);
  }
  if (c.g < 32767) {
    fprintf(stderr, "Not green enough (%d).\n", c.g);
    exit(1);
  }

  c = get_melt_color(&t1);
  if (c.b != 0) {
    fprintf(stderr, "Not expecting any blue in melt color!\n");
    exit(1);
  }
}

void test_style3() {
  Style<Layers<
    Black,
	  TransitionEffectL<TrWaveX<Green,Int<400>,Int<100>,Int<600>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<10000>,Int<30000>>,Int<10000>>>,EFFECT_LOCKUP_END>,
	  ResponsiveLockupL<Blue,TrConcat<TrInstant,AlphaL<Red,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<16000>>>,TrFade<400>>,TrInstant,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Scale<SwingSpeed<100>,Int<10000>,Int<14000>>>,
	  ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
	  ResponsiveStabL<Orange>,
	  ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
	  ResponsiveClashL<White,TrInstant,TrFade<400>>,
	  LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
	  LockupTrL<AlphaL<Mix<TwistAngle<>,Coral,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
	  InOutTrL<TrWipe<300>,TrWipeIn<500>>>> t1;

  Color16 c = get_color_when_on(&t1);
  if (c.r != 0 || c.g != 0 || c.b != 0) {
    fprintf(stderr, "Expecting black.\n");
    exit(1);
  }

}


int main() {
  test_cylon();
  test_inouthelper();
  test_style1();
  test_style2();
  test_style3();
}
