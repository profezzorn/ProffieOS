#ifndef STYLES_RANDOM_PER_LED_FLICKER_H
#define STYLES_RANDOM_PER_LED_FLICKER_H

// Usage: RandomPerLEDFlicker<A, B>
// A, B: COLOR
// return value: COLOR
// Mixes randomly between A and B.
// mix is chosen individually for every LED.

#if 0
template<class A, class B>
class RandomPerLEDFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, random(255));
    return a;
  }
private:
  A a_;
  B b_;
};

#else

#include "../functions/random.h"
#include "alpha.h"
#include "layers.h"

template<class B>
  using RandomPerLEDFlickerL = AlphaL<B, RandomPerLEDF>;

template<class A, class B>
  using RandomPerLEDFlicker = Layers<A, RandomPerLEDFlickerL<B>>;

#endif

#endif
