#ifndef STYLES_SIMPLE_CLASH_H
#define STYLES_SIMPLE_CLASH_H

#include "../functions/smoothstep.h"
#include "layers.h"

// Usage: SimpleClash<BASE, CLASH_COLOR, CLASH_MILLIS>
// Or: SimpleClashL<CLASH_COLOR, CLASH_MILLIS>
// BASE: COLOR
// CLASH_COLOR: COLOR (defaults to white)
// CLASH_MILLIS: a number (defaults to 40)
// return value: COLOR
// Turns the blade to CLASH_COLOR for CLASH_MILLIS millseconds
// when a clash occurs.

template<class CLASH_COLOR = Rgb<255,255,255>, int CLASH_MILLIS = 40,
  BladeEffectType EFFECT = EFFECT_CLASH,
  class STAB_SHAPE = SmoothStep<Int<16384>, Int<24000>> >
class SimpleClashL {
public:
  void run(BladeBase* blade) {
    clash_color_.run(blade);
    stab_shape_.run(blade);
    // This should make us activate the clash at least one "frame".
    if (clash_ && micros() - effect_.last_detected_micros() > CLASH_MILLIS * 1000)
      clash_ = false;
    BladeEffect *e = effect_.Detect(blade);
    if (e) {
      clash_ = true;
      stab_ = EFFECT == EFFECT_CLASH && e->type == EFFECT_STAB && blade->num_leds() > 1;
    }
  }
private:
  OneshotEffectDetector<EFFECT> effect_;
  bool clash_ = false;
  bool stab_ = false;
  CLASH_COLOR clash_color_;
  STAB_SHAPE stab_shape_;
public:
  auto getColor(int led) -> decltype(OverDrive(clash_color_.getColor(led) * stab_shape_.getInteger(led))) {
    decltype(OverDrive(clash_color_.getColor(led) * stab_shape_.getInteger(led))) ret = RGBA_um_nod::Transparent();
    if (clash_) {
      ret = OverDrive(clash_color_.getColor(led));
      if (stab_) {
	ret = ret * stab_shape_.getInteger(led);
      }
    }
    return ret;
  }
};

template<class T, class CLASH_COLOR = Rgb<255,255,255>, int CLASH_MILLIS = 40,
  BladeEffectType EFFECT = EFFECT_CLASH,
  class STAB_SHAPE = SmoothStep<Int<16384>, Int<24000>> >
  using SimpleClash = Layers<T, SimpleClashL<CLASH_COLOR, CLASH_MILLIS, EFFECT, STAB_SHAPE>>;


// Usage: LocalizedClash<BASE, CLASH_COLOR, CLASH_MILLIS, CLASH_WIDTH_PERCENT=50>
// Usage: LocalizedClashL<CLASH_COLOR, CLASH_MILLIS, CLASH_WIDTH_PERCENT=50>
// BASE: COLOR
// CLASH_COLOR: COLOR (defaults to white)
// CLASH_MILLIS: a number (defaults to 40)
// return value: COLOR
// Similar to SimpleClash, but lights up a portion of the blade.
// The fraction of the blade is defined by CLASH_WIDTH_PERCENT
// The location of the clash is random within the middle half of the blade.
// Localized clashes should work well with stabs with no modifications.

// Based on exp(-x*x)
// TODO: Put in shared location as it is identical to the one in blast.h
static uint8_t clash_hump[32] = {
  255,255,252,247,240,232,222,211,
  199,186,173,159,145,132,119,106,
  94,82,72,62,53,45,38,32,
  26,22,18,14,11,9,7,5
};

template<
  class CLASH_COLOR = Rgb<255,255,255>,
  int CLASH_MILLIS = 40,
  int CLASH_WIDTH_PERCENT = 50,
  BladeEffectType EFFECT = EFFECT_CLASH>
class LocalizedClashL {
public:
  void run(BladeBase* blade) {
    clash_color_.run(blade);
    // This should make us activate the clash at least one "frame".
    if (BladeEffect* e = effect_.Detect(blade)) {
      clash_ = true;
      mult_ = NELEM(clash_hump) * 2 * 102400 / CLASH_WIDTH_PERCENT / blade->num_leds();
      clash_location_ = e->location * blade->num_leds() * mult_;
  } else if (micros() - effect_.last_detected_micros() < CLASH_MILLIS * 1000) {
      clash_ = true;
    } else {
      clash_ = false;
    }
  }
private:
  OneshotEffectDetector<EFFECT> effect_;
  bool clash_;
  int mult_;
  int clash_location_;
  CLASH_COLOR clash_color_;
public:
  auto getColor(int led) -> decltype(clash_color_.getColor(led) * 1) {
    decltype(clash_color_.getColor(led) * 1) ret = RGBA_um_nod::Transparent();
    if (clash_) {
      uint32_t dist = abs(led * mult_ - clash_location_) / 1024;
      if (dist < NELEM(clash_hump)) {
	ret = clash_color_.getColor(led) * (clash_hump[dist] * 128);
      }
    }
    return ret;
  }

};

template<class T,
  class CLASH_COLOR = Rgb<255,255,255>,
  int CLASH_MILLIS = 40,
  int CLASH_WIDTH_PERCENT = 50,
  BladeEffectType EFFECT = EFFECT_CLASH>
  using LocalizedClash = Layers<T, LocalizedClashL<CLASH_COLOR, CLASH_MILLIS, CLASH_WIDTH_PERCENT, EFFECT>>;

#endif
