#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string.h>

// cruft
#define interrupts() do {} while(0)
#define noInterrupts() do {} while(0)
#define NELEM(X) (sizeof(X)/sizeof((X)[0]))
#define SCOPED_PROFILER() do { } while(0)
#define NUM_BLADES 1

// clamp(x, a, b) makes sure that x is between a and b.
float clamp(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

struct CONFIG { struct Preset* presets; size_t num_presets;};
extern CONFIG* current_config;

#define PROFFIE_TEST

#define COMMON_FUSE_H

struct V3 {
  V3(float v) { x=y=z=v; }
  float x, y, z;
};

struct MockFuse {
  float angle1_ = 0.0;
  float angle1() { return angle1_; }
  float angle2_ = 0.0;
  float angle2() { return angle2_; }
  float swing_speed_ = 0.0;
  float swing_speed() { return swing_speed_; }
  float swing_accel() { return 0.0; }
  float twist_accel() { return 0.0; }
  V3 gyro() { return V3(0.0); }
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

char* itoa(int value, char* str, int radix) {
  if (radix != 10) {
    fprintf(stderr, "Unexpected radix!\n");
    exit(1);
  }
  sprintf(str, "%d", value);
  return str;
}

uint32_t micros_ = 0;
uint32_t micros() { return micros_; }
uint32_t millis() { return micros_ / 1000; }
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

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

#include "../common/common.h"
#include "../common/stdout.h"
Print default_printer;
Print* default_output = &default_printer;
Print* stdout_output = &default_printer;
ConsoleHelper STDOUT;

Monitoring monitor;

#include "../common/stdout.h"
#include "../common/color.h"
#include "../blades/blade_base.h"
#include "cylon.h"
#include "../common/arg_parser.h"
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
#include "edit_mode.h"
#include "remap.h"
#include "stripes.h"
#include "transition_loop.h"
#include "sequence.h"
#include "../transitions/base.h"
#include "../transitions/join.h"
#include "../transitions/boing.h"
#include "../transitions/wipe.h"
#include "../transitions/delay.h"
#include "../transitions/concat.h"
#include "../transitions/fade.h"
#include "../transitions/instant.h"
#include "../transitions/random.h"
#include "../transitions/loop.h"
#include "../functions/blade_angle.h"
#include "../functions/twist_angle.h"
#include "../functions/swing_speed.h"
#include "../functions/wavlen.h"
#include "../functions/center_dist.h"
#include "../functions/effect_position.h"
#include "../functions/random.h"
#include "../functions/mult.h"
#include "mix.h"
#include "strobe.h"
#include "hump_flicker.h"
#include "brown_noise_flicker.h"
#include "responsive_styles.h"
#include "rainbow.h"
#include "legacy_styles.h"
#include "rgb_arg.h"
#include "inout_sparktip.h"
#include "on_spark.h"
#include "gradient.h"
#include "fire.h"
#include "sparkle.h"
#include "../common/command_parser.h"
#include "../common/preset.h"

Color16 TestRgbArgColors[256];

void clear_test_colors() {
  for (size_t i = 0; i < NELEM(TestRgbArgColors); i++) {
    TestRgbArgColors[i] = Color16();
  }
}

template<int arg, class COLOR>
class TestRgbArg : public RgbArg<arg, COLOR> {
public:
  TestRgbArg() : RgbArg<arg, COLOR>() {
    TestRgbArgColors[arg] = RgbArg<arg,COLOR>::color_;
  }
};

Preset presets[] = {
  { "one", "t1",
    StylePtr<Gradient<TestRgbArg<1, Red>, TestRgbArg<2, Green>, TestRgbArg<3, Blue>>>("0,0,1 0,1,0 1,0,0"),
    "uno" }
};
CONFIG preset = { presets, 1 };
CONFIG* current_config = &preset;


CommandParser* parsers = NULL;
ArgParserInterface* CurrentArgParser;

class StyleCharging : public BladeStyle {
public:
  void activate() override {}
  void run(BladeBase *blade) override {};
  bool NoOnOff() override { return true; }
  bool Charging() override { return true; }
  bool IsHandled(HandledFeature effect) override { return false; }
};
StyleFactoryImpl<StyleCharging> style_charging;

#include "style_parser.h"

SaberBase* saberbases = NULL;
SaberBase::LockupType SaberBase::lockup_ = SaberBase::LOCKUP_NONE;
SaberBase::ColorChangeMode SaberBase::color_change_mode_ =
  SaberBase::COLOR_CHANGE_MODE_NONE;
bool SaberBase::on_ = false;
uint32_t SaberBase::last_motion_request_ = 0;
uint32_t SaberBase::current_variation_ = 0;
float SaberBase::sound_length = 0.0;
int SaberBase::sound_number = -1;


bool on_ = true;
bool allow_disable_ = false;

class MockBlade : public BladeBase {
public:
  std::vector<Color16> colors;

  int num_leds() const override { return colors.size(); }
  bool is_on() const override { return on_; }
  bool is_powered() const override { return true; }
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

#define CHECK_NEAR(X, Y, D) do {                                                \
  auto x_ = (X);                                                                \
  auto y_ = (Y);                                                                \
  if (fabs(x_ - y_) > D) { std::cerr << #X << " (" << x_ << ") ~!= " << #Y << " (" << y_ << ") line " << __LINE__ << std::endl;  exit(1); } \
} while(0)

#define CHECK_COLOR(X, R, G, B, maxdelta) do {	\
  auto x = (X);					\
  CHECK_NEAR(x.r, R, maxdelta);			\
  CHECK_NEAR(x.g, G, maxdelta);			\
  CHECK_NEAR(x.b, B, maxdelta);			\
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

template<class T>
class TestStyle : public Style<T> {
public:
  TestStyle() {
    fprintf(stderr, "RAM: %lu (%s)\n",
	    sizeof(*this),
	    __PRETTY_FUNCTION__);
  }
};

template<class T>
class PrintTypeSize  : public T {
public:
  PrintTypeSize() {
    fprintf(stderr, "RAM: %lu (%s)\n",
	    sizeof(*this),
	    __PRETTY_FUNCTION__);
  }
};

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

Color16 get_color_after_ignition(BladeStyle* style, int millis) {
  MockBlade mock_blade;
  mock_blade.SetStyle(style);
  mock_blade.colors.resize(1);
  on_ = false;
  micros_ = 0;
  for (int i = 0; i < 10000; i++) STEP();
  on_ = true;
  int last = 0;

  for (int i = 0; i < millis; i++) STEP();
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
  TestStyle<Layers<
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

void test_style4() {
  ArgParser ap("foobar");
  CurrentArgParser = &ap;

  PrintTypeSize<IsLessThan< IntArg<3,600> , Int<1>>> x2;
  PrintTypeSize<IntArg<3,600>> x3;
  PrintTypeSize<WavLen<EFFECT_RETRACTION>> x4;
  PrintTypeSize<Scale<IsLessThan< IntArg<3,600> , Int<1>>, IntArg<3,600>, WavLen<EFFECT_RETRACTION>>> x1;

  PrintTypeSize<Int<1>> t1;
  PrintTypeSize<IntSVF<1>> t11;
  PrintTypeSize<BladeAngle<0,16000>> t2;
  PrintTypeSize<BladeAngleXSVF<Int<0>,Int<16000>>> t22;
  PrintTypeSize<SVFWrapper<BladeAngle<0,16000>>> t3;
  PrintTypeSize<
    Scale<BladeAngle<0,16000>,Int<10000>,Int<30000>>
    > t4;
  PrintTypeSize<
    Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<10000>,Int<30000>>,Int<10000>>
    > t5;
}

void TestCompileStyle() {
  TestStyle<Layers<
    Blue,
    MultiTransitionEffectL<
      TrRandom<
        TrConcat<
          TrInstant, AlphaL<RgbArg<BLAST_COLOR_ARG, Rgb<255, 255, 255>>,
                            Bump<Int<16384>, Int<6000>>>,
          TrFade<50>, AlphaL<RgbArg<BLAST_COLOR_ARG, Rgb<255, 255, 255>>, Int<0>>,
          TrWaveX<
            Remap<
              CenterDistF<EffectPosition<>>,
              Stripes<1500, -2000, RgbArg<BLAST_COLOR_ARG, Rgb<255, 255, 255>>,
                      Mix<Int<2096>, Black,
                          RgbArg<BLAST_COLOR_ARG, Rgb<255, 255, 255>>>>>,
            Int<160>, Int<100>, Int<300>, EffectPosition<>>>,
        TrConcat<TrInstant,
                 AlphaL<RgbArg<BLAST_COLOR_ARG, Rgb<255, 255, 255>>,
                        Bump<EffectPosition<>, Scale<EffectRandomF<EFFECT_BLAST>,
                                                     Int<7000>, Int<10000>>>>,
                 TrFade<300>>,
        TrWaveX<RgbArg<BLAST_COLOR_ARG, Rgb<255, 255, 255>>,
                Scale<EffectRandomF<EFFECT_BLAST>, Int<100>, Int<400>>, Int<100>,
                Scale<EffectRandomF<EFFECT_BLAST>, Int<100>, Int<400>>,
                Scale<EffectRandomF<EFFECT_BLAST>, Int<28000>, Int<8000>>>>,
      EFFECT_BLAST>,
    InOutTrL<TrWipe<300>, TrWipeIn<500>>>> t1;


  TestStyle<Layers<
    Black,
    TransitionEffectL< TrConcat<TrFade<1>, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, TrFade<1>, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, TrFade<1>, TrFade<1>, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, White, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, White, TrFade<1>, White, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, White, TrFade<1>, White, TrFade<1>, White, TrFade<1>>, EFFECT_BLAST>,

    TransitionEffectL< TrConcat<TrFade<1>, White, TrFade<1>, White, TrFade<1>, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, White, TrFade<1>, White, TrFade<1>, White, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, White, TrFade<1>, TrFade<1>, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, White, TrFade<1>, TrFade<1>, White, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, TrFade<1>, White, TrFade<1>, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, TrFade<1>, White, TrFade<1>, White, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, TrFade<1>, TrFade<1>, TrFade<1>>, EFFECT_BLAST>,
    TransitionEffectL< TrConcat<TrFade<1>, TrFade<1>, TrFade<1>, White, TrFade<1>>, EFFECT_BLAST>
    >> t2;
    
  TestStyle<Layers<
    Black,
    TransitionEffectL< TrLoop< TrFade<1> >, EFFECT_BLAST>,
    TransitionEffectL< TrLoopUntil< Int<1>, TrFade<1>, TrFade<1>>, EFFECT_BLAST>
	      >> t3;

  TestStyle<Layers<
    Black,
	      Remap<SmoothStep<Int<0>,SwingSpeed<250>>,AlphaL<RgbArg<BASE_COLOR_ARG,Red>,Mult<RandomPerLEDF,CenterDistF<Int<32768>>>>>>> t4;
}


void test_style5() {
  TestStyle<Layers<
    Blue,
      AlphaL<
        PulsingL<
            TransitionLoop<
                AlphaL<ColorSequence<3500,Cyan,Black,Rgb<25,10,255>,Black>,Int<8000>>,
                TrRandom<
                    TrConcat<TrBoing<500,2>,Black,TrInstant>,
                    TrJoinR<
                        TrJoin<TrWipeInX<Int<300>>,TrWaveX<White,Int<400>,Int<600>,Int<300>,Int<32768>>>,
                        TrJoin<TrWipeX<  Int<300>>,TrWaveX<White,Int<400>,Int<600>,Int<300>,Int<0>>>>>>,
        Int<1500>>,
    Int<16000>>
	      >> t1;
  Color16 c = get_color_when_on(&t1);
}

void test_style6() {
  TestStyle<Layers<WHITE, InOutTrL<TrFade<1000>, TrFade<500>>>> t1;
  CHECK_COLOR(get_color_after_ignition(&t1, 250), 16384, 16384, 16384, 200);
  CHECK_COLOR(get_color_after_ignition(&t1, 500), 32768, 32768, 32768, 200);
  CHECK_COLOR(get_color_after_ignition(&t1, 750), 49152, 49152, 49152, 200);

  TestStyle<Layers<WHITE, InOutTrL<TrFadeX<BendTimePow<1000, 65536>>, TrFade<500>>>> t2;
  CHECK_COLOR(get_color_after_ignition(&t2, 250), 4096, 4096, 4096, 200);
  CHECK_COLOR(get_color_after_ignition(&t2, 500), 16384, 16384, 16384, 200);
  CHECK_COLOR(get_color_after_ignition(&t2, 750), 36864, 36864, 36864, 200);

  TestStyle<Layers<WHITE, InOutTrL<TrFadeX<BendTimePow<1000, 16384>>, TrFade<500>>>> t3;
  CHECK_COLOR(get_color_after_ignition(&t3, 250), 32768, 32768, 32768, 200);
  CHECK_COLOR(get_color_after_ignition(&t3, 500), 46341, 46341, 46341, 200);
  CHECK_COLOR(get_color_after_ignition(&t3, 750), 56755, 56755, 56755, 200);

  TestStyle<Layers<WHITE, InOutTrL<TrFadeX<BendTimePowInv<1000, 65536>>, TrFade<500>>>> t4;
  CHECK_COLOR(get_color_after_ignition(&t4, 250), 28672, 28672, 28672, 200);
  CHECK_COLOR(get_color_after_ignition(&t4, 500), 49152, 49152, 49152, 200);
  CHECK_COLOR(get_color_after_ignition(&t4, 750), 61440, 61440, 61440, 200);
}


void testGetArg(const char* str, int arg, const char* expected) {
  char tmp[1024];
  fprintf(stderr, "testGetArg(%s, %d)\n", str, arg);
  if (!style_parser.GetArgument(str, arg, tmp)) {
    fprintf(stderr, "Expected to be able to get argument %d from %s\n", arg, str);
    exit(1);
  }
  if (strcmp(tmp, expected)) {
    fprintf(stderr, "Expected '%s', but got '%s' for argument %d from %s\n", expected, tmp, arg, str);
    exit(1);
  }
}
void testNoArg(const char* str, int arg) {
  char tmp[1024];
  if (style_parser.GetArgument(str, arg, tmp)) {
    fprintf(stderr, "Expected to NOT be able to get argument %d from %s\n", arg, str);
    exit(1);
  }
  if (*tmp) {
    fprintf(stderr, "When returning false, GetArgument should return an empty string!\n");
    exit(1);
  }
}

void testSetArg(const char* str, int arg, const char* replacement, const char* expected) {
  fprintf(stderr, "testSetArg(%s, %d)\n", str, arg);
  LSPtr<char> ret = style_parser.SetArgument(str, arg, replacement);
  if (strcmp(ret.get(), expected)) {
    fprintf(stderr, "Expected '%s' got '%s'\n", expected, ret.get());
    exit(1);
  }
}

void testResetArguments(const char* from, const char* expected) {
  fprintf(stderr, "testResetArguments(%s)\n", from);
  LSPtr<char> ret = style_parser.ResetArguments(from);
  if (strcmp(ret.get(), expected)) {
    fprintf(stderr, "Expected '%s' got '%s'\n", expected, ret.get());
    exit(1);
  }
}

void testCopyArguments(const char* from, const char *to, const char* expected) {
  fprintf(stderr, "testCopyArguments(%s, %s)\n", from, to);
  LSPtr<char> ret = style_parser.CopyArguments(from, to);
  if (strcmp(ret.get(), expected)) {
    fprintf(stderr, "Expected '%s' got '%s'\n", expected, ret.get());
    exit(1);
  }
}

void testCopyArguments(const char* from, const char *to, int N, const char* expected) {
  fprintf(stderr, "testCopyArguments(%s, %s, %d)\n", from, to, N);
  LSPtr<char> ret = style_parser.CopyArguments(from, to, N);
  if (strcmp(ret.get(), expected)) {
    fprintf(stderr, "Expected '%s' got '%s' (1)\n", expected, ret.get());
    exit(1);
  }

  int to_keep[50];
  for (int i = 0; i < 50; i++) to_keep[i] = N + i + 1;
  LSPtr<char> ret2 = style_parser.CopyArguments(from, to, to_keep, 50);
  if (strcmp(ret2.get(), expected)) {
    fprintf(stderr, "Expected '%s' got '%s' (2)\n", expected, ret2.get());
    exit(1);
  }
}

void testCopyArguments(const char* from, const char *to, int keep1, int keep2, int keep3, const char* expected) {
  int to_keep[3];
  to_keep[0] = keep1;
  to_keep[1] = keep2;
  to_keep[2] = keep3;
  fprintf(stderr, "testCopyArguments(%s, %s, %d, %d, %d)\n", from, to, keep1, keep2, keep3);
  LSPtr<char> ret = style_parser.CopyArguments(from, to, to_keep, 3);
  if (strcmp(ret.get(), expected)) {
    fprintf(stderr, "Expected '%s' got '%s' (1)\n", expected, ret.get());
    exit(1);
  }
}

void testMaxUsedArgument(const char* from, int expected) {
  fprintf(stderr, "testMaxUsedArgument(%s)\n", from);
  int args = style_parser.MaxUsedArgument(from);
  if (args != expected) {
    fprintf(stderr, "Expected %d got %d\n", expected, args);
    exit(1);
  }
}


void test_argument_parsing() {
  testGetArg("standard", 0, "standard");
  testGetArg("standard", 1, "0,65535,65535");
  testGetArg("standard ~", 1, "0,65535,65535");
  testGetArg("standard ~ 1,0,0", 2, "1,0,0");
  testGetArg("standard", 2, "65535,65535,65535");
  testGetArg("standard", 3, "300");
  testGetArg("standard", 4, "800");
  testNoArg("standard", 5);
  testGetArg("standard 1,1,1 2,2,2 3 4 5", 5, "5");
  testNoArg("standard 1,1,1 2,2,2 3 4 ~", 5);
  testGetArg("standard 1,1,1 2,2,2 3 4 5", 2, "2,2,2");
  testSetArg("standard 1,1,1 2,2,2 3 4", 1, "7,7,7", "standard 7,7,7 2,2,2 3 4");
  testSetArg("standard 1,1,1 2,2,2 3 4", 2, "7,7,7", "standard 1,1,1 7,7,7 3 4");
  testSetArg("standard 1,1,1 2,2,2 3 4", 3, "7", "standard 1,1,1 2,2,2 7 4");
  testSetArg("standard 1,1,1 2,2,2 3 4", 4, "7", "standard 1,1,1 2,2,2 3 7");

  testResetArguments("standard 1 2 3", "standard");
  testCopyArguments("standard 1 2 3", "blarg 2 3 4", "blarg 1 2 3");

  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 4, "blarg 1 2 3");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 3, "blarg 1 2 3");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 2, "blarg 1 2 9");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 1, "blarg 1 8 9");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 0, "blarg 7 8 9");

  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 1, 2, 3, "blarg 7 8 9");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 1, 1, 1, "blarg 7 2 3");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9", 1, 1, 3, "blarg 7 2 9");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9 A B C", 1, 1, 3, "blarg 7 2 9 ~ ~ ~");
  testCopyArguments("standard 1 2 3", "blarg 7 8 9 A B C", 4, 5, 6, "blarg 1 2 3 A B C");
  testCopyArguments("standard 1 2 3 4 5 6", "blarg 7 8 9", 1, 2, 3, "blarg 7 8 9 4 5 6");
  testCopyArguments("standard 1 22 333 44444 55555 666666", "blarg 7 8 9", 1, 2, 3, "blarg 7 8 9 44444 55555 666666");

  testMaxUsedArgument("charging", 0);
  testMaxUsedArgument("rainbow", 2);
  testMaxUsedArgument("fire", 2);

  testGetArg("builtin 0 1", 0, "builtin");
  testGetArg("builtin 0 1", 1, "0");
  testGetArg("builtin 0 1", 2, "1");
  testGetArg("builtin 0 1", 3, "0,0,1");
  testGetArg("builtin 0 1", 4, "0,1,0");
  testGetArg("builtin 0 1", 5, "1,0,0");

  clear_test_colors();
  testGetArg("builtin 0 1", 0, "builtin");
  CHECK_COLOR(TestRgbArgColors[1], 0, 0, 1, 0);
  CHECK_COLOR(TestRgbArgColors[2], 0, 1, 0, 0);
  CHECK_COLOR(TestRgbArgColors[3], 1, 0, 0, 0);

  clear_test_colors();
  testGetArg("builtin 0 1 1,2,3", 0, "builtin");
  CHECK_COLOR(TestRgbArgColors[1], 1, 2, 3, 0);
  CHECK_COLOR(TestRgbArgColors[2], 0, 1, 0, 0);
  CHECK_COLOR(TestRgbArgColors[3], 1, 0, 0, 0);

  clear_test_colors();
  testGetArg("builtin 0 1 1,2,3 4,5,6", 0, "builtin");
  CHECK_COLOR(TestRgbArgColors[1], 1, 2, 3, 0);
  CHECK_COLOR(TestRgbArgColors[2], 4, 5, 6, 0);
  CHECK_COLOR(TestRgbArgColors[3], 1, 0, 0, 0);

  clear_test_colors();
  testGetArg("builtin 0 1 1,2,3 4,5,6 7,8,9", 0, "builtin");
  CHECK_COLOR(TestRgbArgColors[1], 1, 2, 3, 0);
  CHECK_COLOR(TestRgbArgColors[2], 4, 5, 6, 0);
  CHECK_COLOR(TestRgbArgColors[3], 7, 8, 9, 0);

  clear_test_colors();
  testGetArg("builtin 0 1 ~ 4,5,6 7,8,9", 0, "builtin");
  CHECK_COLOR(TestRgbArgColors[1], 0, 0, 1, 0);
  CHECK_COLOR(TestRgbArgColors[2], 4, 5, 6, 0);
  CHECK_COLOR(TestRgbArgColors[3], 7, 8, 9, 0);

  clear_test_colors();
  testGetArg("builtin 0 1 ~ ~ 7,8,9", 0, "builtin");
  CHECK_COLOR(TestRgbArgColors[1], 0, 0, 1, 0);
  CHECK_COLOR(TestRgbArgColors[2], 0, 1, 0, 0);
  CHECK_COLOR(TestRgbArgColors[3], 7, 8, 9, 0);

  clear_test_colors();
  testGetArg("builtin 0 1 ~ ~ 7,8,9", 0, "builtin");
  CHECK_COLOR(TestRgbArgColors[1], 0, 0, 1, 0);
  CHECK_COLOR(TestRgbArgColors[2], 0, 1, 0, 0);
  CHECK_COLOR(TestRgbArgColors[3], 7, 8, 9, 0);
}

int main() {
  test_style6();
  test_style5();
  test_style4();
  test_cylon();
  test_inouthelper();
  test_style1();
  test_style2();
  test_style3();
  test_argument_parsing();
}
