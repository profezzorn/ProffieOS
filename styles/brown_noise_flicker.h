#ifndef STYLES_BROWN_NOISE_FLICKER_H
#define STYLES_BROWN_NOISE_FLICKER_H

// Usage: BrownNoiseFlicker<A, B, grade>
// A, B: COLOR
// grade: int
// return value: COLOR
// Randomly selects between A and B, but keeps nearby
// pixels looking similar.

class BladeBase;

#if 0
template<class A, class B, int grade>
class BrownNoiseFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    mix_ = random(255);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, mix_);
    mix_ = clampi32(mix_ + random(grade * 2 + 1) - grade, 0, 255);
    return a;
  }
private:
  A a_;
  B b_;
  int mix_;
};

#else

#include "alpha.h"
#include "layers.h"
#include "../functions/brown_noise.h"
#include "../functions/int.h"

template<class B, class GRADE>
  using BrownNoiseFlickerL = AlphaL<B, BrownNoiseF<GRADE>>;

template<class A, class B, int grade>
  using BrownNoiseFlicker = Layers<A, BrownNoiseFlickerL<B, Int<grade * 128>>>;


#endif

#endif
