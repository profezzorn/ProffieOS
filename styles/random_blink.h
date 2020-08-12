#ifndef STYLES_RANDOM_BLINK_H
#define STYLES_RANDOM_BLINK_H

// usage: RandomBlink<MILLIHZ, COLOR1, COLOR2>
// or: RandomBlinkX<MILLIHZ_CLASS, COLOR1, COLOR2>
// or: RandomBlinkL<MILLIHZ_CLASS, COLOR1>
// MILLIHZ: integer
// MILLHZ_CLASS: NUMBER
// COLOR1: COLOR (defaults to WHITE)
// COLOR2: COLOR (defaults to BLACK)
// return value: COLOR
// Each LED is randomly chosen as COLOR1 or COLOR2, then stays
// that color for 1000/MILLIHZ seconds.

#include "../functions/random_blink.h"

template<class MILLIHZ, class COLOR1 = WHITE>
  using RandomBlinkL = AlphaL<COLOR1, RandomBlinkF<MILLIHZ>>;

template<class MILLIHZ, class COLOR1 = WHITE, class COLOR2 = BLACK>
  using RandomBlinkX = Layers<COLOR2, RandomBlinkL<MILLIHZ, COLOR1>>;

template<int MILLIHZ, class COLOR1 = WHITE, class COLOR2 = BLACK>
  using RandomBlink = RandomBlinkX<Int<MILLIHZ>, COLOR1, COLOR2>;

#endif
