#ifndef STYLES_RANDOM_FLICKER_H
#define STYLES_RANDOM_FLICKER_H

// Usage: RandomFlicker<A, B>
// Or: RandomL<B>
// A, B: COLOR
// return value: COLOR
// Mixes randomly between A and B.
// mix is even over entire blade.

#include "../functions/random.h"
#include "alpha.h"
#include "layers.h"

template<class B>
  using RandomL = AlphaL<B, RandomF>;

template<class A, class B>
  using RandomFlicker = Layers<A, RandomL<B>>;

#endif
