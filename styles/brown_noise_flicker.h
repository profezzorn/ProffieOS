#ifndef STYLES_BROWN_NOISE_FLICKER_H
#define STYLES_BROWN_NOISE_FLICKER_H

// Usage: BrownNoiseFlicker<A, B, grade>
// Or: BrownNoiseFlickerL<B, grade>
// A, B: COLOR
// grade: int
// return value: COLOR
// Randomly selects between A and B, but keeps nearby
// pixels looking similar.

class BladeBase;

#include "alpha.h"
#include "layers.h"
#include "../functions/brown_noise.h"
#include "../functions/int.h"

template<class B, class GRADE>
  using BrownNoiseFlickerL = AlphaL<B, BrownNoiseF<GRADE>>;

template<class A, class B, int grade>
  using BrownNoiseFlicker = Layers<A, BrownNoiseFlickerL<B, Int<grade * 128>>>;

#endif
