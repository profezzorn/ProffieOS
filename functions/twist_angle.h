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
    int a = fusor.angle2() * 16384 / M_PI;
    value_ = abs((((a + OFFSET) & 0X7fff) - 16384) * (N * 2));
  }
  int getInteger(int led) { return value_; }

  int value_;
};

#endif
