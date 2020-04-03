#ifndef STYLES_STROBE_H
#define STYLES_STROBE_H

#include "../functions/int.h"

// Usage: Strobe<BASE, STROBE_COLOR, STROBE_FREQUENCY, STROBE_MILLIS>
// or: StrobeX<BASE, STROBE_COLOR, STROBE_FREQUENCY_FUNC, STROBE_MILLIS_FUNC>
// or: StrobeL<STROBE_COLOR, STROBE_FREQUENCY_FUNC, STROBE_MILLIS_FUNC>
// BASE, STROBE_COLOR: COLOR
// STROBE_FREQUENCY, STROBE_MILLIS: a number
// STROBE_FREQUENCY_FUNC, STROBE_MILLIS_FUNC: FUNCTION
// return value: COLOR
// Stroboscope-like effect, turns the color to STROBE_COLOR for STROBE_MILLIS
// STROBE_FREQUENCY times per second.

#include "../functions/strobe.h"

template<class STROBE_COLOR, class STROBE_FREQUENCY, class STROBE_MILLIS>
  using StrobeL = AlphaL<STROBE_COLOR, StrobeF<STROBE_FREQUENCY, STROBE_MILLIS>>;

template<class T, class STROBE_COLOR, class STROBE_FREQUENCY, class STROBE_MILLIS>
  using StrobeX = Layers<T, StrobeL<STROBE_COLOR, STROBE_FREQUENCY, STROBE_MILLIS>>;

template<class T, class STROBE_COLOR, int STROBE_FREQUENCY, int STROBE_MILLIS = 1>
  using Strobe = StrobeX<T, STROBE_COLOR, Int<STROBE_FREQUENCY>, Int<STROBE_MILLIS> >;

#endif
