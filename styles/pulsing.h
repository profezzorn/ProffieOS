#ifndef STYLES_PULSING_H
#define STYLES_PULSING_H

#include "../functions/int.h"

// Usage: Pulsing<A, B, PULSE_MILLIS>
// or: PulsingX<A, B, PULSE_MILLIS_FUNC>
// or: PulsingL<B, PULSE_MILLIS_FUNC>
// A, B: COLOR
// PULSE_MILLIS: a number
// PULSE_MILLIS_FUNC: FUNCTION
// return value: COLOR
// Goes back and forth between COLOR1 and COLOR2.
// A full transition from COLOR1 to COLOR2 and back again takes PULSE_MILLIS milliseconds.

#include "../functions/sin.h"

template<class COLOR2, typename PULSE_MILLIS>
  using PulsingL = AlphaL<COLOR2, PulsingF<PULSE_MILLIS>>;

template<class COLOR1, class COLOR2, typename PULSE_MILLIS>
  using PulsingX = Layers<COLOR1, PulsingL<COLOR2, PULSE_MILLIS>>;

template<class COLOR1, class COLOR2, int PULSE_MILLIS>
  using Pulsing = PulsingX<COLOR1, COLOR2, Int<PULSE_MILLIS> >;


#endif
