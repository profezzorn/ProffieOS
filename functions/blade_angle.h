#ifndef FUNCTIONS_BLADE_ANGLE_H
#define FUNCTIONS_BLADE_ANGLE_H

#include "../common/fuse.h"

// Usage: BladeAngleX<MIN, MAX>
// Returns 0-32768 based on angle of blade
// MIN : FUNCTION (defaults to Int<0>)
// MAX : FUNCTION (defaults to Int<32768>)
// MIN and MAX specifies the range of angles which are used.
// For MIN and MAX 0 means down and 32768 means up and 16384 means
// pointing towards the horizon.
// So if MIN=16484 and MAX=32768, BladeAngle will return zero when you
// point the blade towards the horizon and 32768 when you point it
// straight up. Any angle below the horizon will also return zero.
// returned value: FUNCTION, same for all leds

template<class MIN = Int<0>, class MAX = Int<32768>>
class BladeAngleX {
public:
  void run(BladeBase* blade) {
    min_.run(blade);
    max_.run(blade);
    int min = min_.getInteger(0);
    int max = max_.getInteger(0);
    // v is 0-32768, 0 = straight down, 32768 = straight up
    float v = (fusor.angle1() + M_PI / 2) * 32768 / M_PI;
    value_ = clampi32((v - min) * 32768.0f / (max - min), 0, 32768);
  }
  int getInteger(int led) { return value_; }

  MIN min_;
  MAX max_;
  int value_;
};

template<int MIN = 0, int MAX = 32768>
using BladeAngle = BladeAngleX<Int<MIN>, Int<MAX>>;

#endif
