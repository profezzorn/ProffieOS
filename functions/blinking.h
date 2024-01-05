#ifndef FUNCTIONS_BLINKING_H
#define FUNCTOINS_BLINKING_H

#include "int.h"

// Usage: BlinkingF<A, B, BLINK_MILLIS_FUNC, BLINK_PROMILLE_FUNC>
// BLINK_MILLIS: a number
// BLINK_PROMILLE: a number, defaults to 500
// BLINK_MILLIS_FUNC: FUNCTION
// BLINK_PROMILLE_FUNC: FUNCTION
// return value: FUNCTION
// Switches between 0 and 32768
// A full cycle from 0 to 328768 and back again takes BLINK_MILLIS milliseconds.
// If BLINK_PROMILLE is 500, we select A for the first half and B for the
// second half. If BLINK_PROMILLE is smaller, we get less A and more B.
// If BLINK_PROMILLE is 0, we get all 0.
// If BLINK_PROMILLE is 1000 we get all 32768.
//

#include "svf.h"

template<typename BLINK_MILLIS, typename BLINK_PROMILLE>
class BlinkingFSVF {
public:
  void run(BladeBase* base) {
    pulse_millis_.run(base);
    pulse_promille_.run(base);
  }

  int calculate(BladeBase* base) {
    uint32_t now = micros();
    uint32_t pulse_millis = pulse_millis_.calculate(base);
    if (pulse_millis <= 0) return 0;
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
    return pulse_progress_promille <= pulse_promille_.calculate(base) ? 0 : 32768;
  }

private:
  PONUA SVFWrapper<BLINK_MILLIS> pulse_millis_;
  PONUA SVFWrapper<BLINK_PROMILLE> pulse_promille_;
  uint32_t pulse_start_micros_;
};

template<typename BLINK_MILLIS, typename BLINK_PROMILLE>
using BlinkingF = SingleValueAdapter<BlinkingFSVF<BLINK_MILLIS, BLINK_PROMILLE>>;

#endif
