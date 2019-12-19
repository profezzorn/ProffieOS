#ifndef STYLES_BLINKING_H
#define STYLES_BLINKING_H

#include "../functions/int.h"

// Usage: Blinking<A, B, BLINK_MILLIS, BLINK_PROMILLE>
// or: BlinkingX<A, B, BLINK_MILLIS_FUNC, BLINK_PROMILLE_FUNC>
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
template<class COLOR1,
  class COLOR2,
  typename BLINK_MILLIS,
  typename BLINK_PROMILLE>
class BlinkingX {
public:
  void run(BladeBase* base) {
    c1_.run(base);
    c2_.run(base);
    pulse_millis_.run(base);
    pulse_promille_.run(base);
    uint32_t now = micros();
    uint32_t pulse_millis = pulse_millis_.getInteger(0);
    if (pulse_millis <= 0) return;
    uint32_t pulse_progress_micros = now - pulse_start_micros_;
    if (pulse_progress_micros > pulse_millis * 1000) {
      // Time to start a new pulse
      if (pulse_progress_micros < pulse_millis * 2000) {
	pulse_start_micros_ += pulse_millis * 1000;
      } else {
	pulse_start_micros_ = now;
      }
      pulse_progress_micros = now - pulse_start_micros_;
    }
    int32_t pulse_progress_promille = pulse_progress_micros / pulse_millis;
    on_ = pulse_progress_promille <= pulse_promille_.getInteger(0);
  }

  OverDriveColor getColor(int led) {
    return on_ ? c1_.getColor(led) : c2_.getColor(led);
  }

private:
  COLOR1 c1_;
  COLOR2 c2_;
  BLINK_MILLIS pulse_millis_;
  BLINK_PROMILLE pulse_promille_;
  uint32_t pulse_start_micros_;
  bool on_;
};

template<class COLOR1,
  class COLOR2,
  int BLINK_MILLIS,
  int BLINK_PROMILLE = 500>
  using Blinking = BlinkingX<COLOR1, COLOR2, Int<BLINK_MILLIS>, Int<BLINK_PROMILLE> >;


#endif
