#ifndef FUNCTIONS_TWIST_ANGLE_H
#define FUNCTIONS_TWIST_ANGLE_H

#include "../common/fuse.h"

// Usage: TwistAngleX<OFFSET, N>
// Returns 0-32768 based on angle of twist
// OFFSET: Adjustable offset (0-32768) to make the twistangle values line up with how you hold the hilt.
// N : Number of times the values goes from 0 to 32768 and back per hilt revolution.
// returned value: FUNCTION, same for all leds

template<int N = 2, int OFFSET = 0>
class TwistAngle {
public:
  void run(BladeBase* blade) {
    int a = fusor.angle2() * 32768 / M_PI;
    value_ = ((a + OFFSET) * N) & 0xffff;
    if (value_ >= 0x8000) value_ = 0x10000 - value_;
  }
  int getInteger(int led) { return value_; }

  int value_;
};

#endif
