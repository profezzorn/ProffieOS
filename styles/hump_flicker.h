#ifndef STYLES_HUMP_FLICKER_H
#define STYLES_HUMP_FLICKER_H

// Usage: HumpFlicker<A, B, HUMP_WIDTH>
// A, B: COLOR
// HUMP_WIDTH: a number
// return value: COLOR
// Makes a random "hump" which is about 2xHUMP_WIDTH leds wide.

class BladeBase;

#if 0
template<class A, class B, int HUMP_WIDTH>
class HumpFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    int num_leds_ = blade->num_leds();
    pos_ = random(num_leds_);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    int mix_ = clampi32(abs(led - pos_) * 255 / HUMP_WIDTH, 0, 255);
    a.c = a.c.mix(b.c, mix_);
    return a;
  }
private:
  A a_;
  B b_;
  int pos_;
};

#else

#include "../functions/bump.h"
#include "alpha.h"
#include "layers.h"

template<class B, int HUMP_WIDTH>
  using HumpFlickerL = AlphaL<B, HumpFlickerF<HUMP_WIDTH>>;
template<class A, class B, int HUMP_WIDTH>
  using HumpFlicker = Layers<A, HumpFlickerL<B, HUMP_WIDTH>>;


#endif

#endif
