#ifndef STYLES_HUMP_FLICKER_H
#define STYLES_HUMP_FLICKER_H

// Usage: HumpFlicker<A, B, HUMP_WIDTH>
// Or: HumpFlickerL<B, HUMP_WIDTH>
// A, B: COLOR
// HUMP_WIDTH: a number
// return value: COLOR
// Makes a random "hump" which is about 2xHUMP_WIDTH leds wide.

class BladeBase;

#include "../functions/bump.h"
#include "alpha.h"
#include "layers.h"

template<class B, int HUMP_WIDTH>
  using HumpFlickerL = AlphaL<B, HumpFlickerF<HUMP_WIDTH>>;
template<class A, class B, int HUMP_WIDTH>
  using HumpFlicker = Layers<A, HumpFlickerL<B, HUMP_WIDTH>>;

#endif
