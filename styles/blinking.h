#ifndef STYLES_BLINKING_H
#define STYLES_BLINKING_H

#include "../functions/int.h"

// Usage: Blinking<A, B, BLINK_MILLIS, BLINK_PROMILLE>
// or: BlinkingX<A, B, BLINK_MILLIS_FUNC, BLINK_PROMILLE_FUNC>
// or: BlinkingL<B, BLINK_MILLIS_FUNC, BLINK_PROMILLE_FUNC>
// A, B: COLOR
// BLINK_MILLIS: a number
// BLINK_PROMILLE: a number, defaults to 500
// BLINK_MILLIS_FUNC: FUNCTION
// BLINK_PROMILLE_FUNC: FUNCTION
// return value: COLOR
// Switches between A and B.
// A full cycle from A to B and back again takes BLINK_MILLIS milliseconds.
// If BLINK_PROMILLE is 500, we select A for the first half and B for the
// second half. If BLINK_PROMILLE is smaller, we get less A and more B.
// If BLINK_PROMILLE is 0, we get all B.
// If BLINK_PROMILLE is 1000 we get all A.
//

#include "../functions/blinking.h"

template<class COLOR2, typename BLINK_MILLIS, typename BLINK_PROMILLE>
  using BlinkingL = AlphaL<COLOR2, BlinkingF<BLINK_MILLIS, BLINK_PROMILLE>>;

template<class COLOR1, class COLOR2, typename BLINK_MILLIS, typename BLINK_PROMILLE>
  using BlinkingX = Layers<COLOR1, BlinkingL<COLOR2, BLINK_MILLIS, BLINK_PROMILLE>>;

template<class COLOR1,
  class COLOR2,
  int BLINK_MILLIS,
  int BLINK_PROMILLE = 500>
  using Blinking = BlinkingX<COLOR1, COLOR2, Int<BLINK_MILLIS>, Int<BLINK_PROMILLE> >;

#endif
