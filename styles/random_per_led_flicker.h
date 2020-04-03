#ifndef STYLES_RANDOM_PER_LED_FLICKER_H
#define STYLES_RANDOM_PER_LED_FLICKER_H

// Usage: RandomPerLEDFlicker<A, B>
// Or: RandomPerLEDFlickerL<B>
// A, B: COLOR
// return value: COLOR
// Mixes randomly between A and B.
// mix is chosen individually for every LED.

#include "../functions/random.h"
#include "alpha.h"
#include "layers.h"

template<class B>
  using RandomPerLEDFlickerL = AlphaL<B, RandomPerLEDF>;

template<class A, class B>
  using RandomPerLEDFlicker = Layers<A, RandomPerLEDFlickerL<B>>;

#endif
