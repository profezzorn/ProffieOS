#ifndef STYLES_PULSING_H
#define STYLES_PULSING_H

#include "../functions/int.h"

// Usage: Pulsing<A, B, PULSE_MILLIS>
// or: Pulsing<A, B, PULSE_MILLIS_FUNC>
// A, B: COLOR
// PULSE_MILLIS: a number
// PULSE_MILLIS_FUNC: FUNCTION
// return value: COLOR
// Goes back and forth between COLOR1 and COLOR2.
// A full transition from COLOR1 to COLOR2 and back again takes PULSE_MILLIS milliseconds.

template<class COLOR1, class COLOR2, typename PULSE_MILLIS>
class PulsingX {
public:
  void run(BladeBase* base) {
    c1_.run(base);
    c2_.run(base);
    pulse_millis_.run(base);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / (1000.0 * pulse_millis_.getInteger(0)));
    mix_ = (sin_table[(int)floor(pos_ * 0x400)] + 16384) >> 1;
  }

  OverDriveColor getColor(int led) {
    OverDriveColor c1 = c1_.getColor(led);
    OverDriveColor c2 = c2_.getColor(led);
    c1.c = c1.c.mix2(c2.c, mix_);
    return c1;
  }

private:
  COLOR1 c1_;
  COLOR2 c2_;
  PULSE_MILLIS pulse_millis_;
  int mix_;
  uint32_t last_micros_;
  float pos_ = 0.0;
};

template<class COLOR1, class COLOR2, int PULSE_MILLIS>
  using Pulsing = PulsingX<COLOR1, COLOR2, Int<PULSE_MILLIS> >;


#endif
