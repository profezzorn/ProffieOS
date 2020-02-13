#ifndef FUNCTIONS_BLADE_ANGLE_H
#define FUNCTIONS_BLADE_ANGLE_H

#include "../common/fuse.h"

// Usage: BladeAngleX<MIN, MAX>
// Returns 0-32768 based on angle of blade
// MIN = 0=down 32768=up. If 16384, the pointing the blade towards the horizon will return 0.
// MAX = 0=down 32768=up. If 16384, the pointing the blade towards the horizon will return 32768.
// returned value: INTEGER

template<class MIN, class MAX>
class BladeAngleX {
public:
  void run(BladeBase* blade) {
    min_.run(blade);
    max_.run(blade);
    int min = min_.getInteger(0);
    int max = max_.getInteger(0);
    float v = (fusor.angle1() + M_PI / 2) * 32768 / M_PI; // Now v is 0-32768, 0 = straight down, 32768 = straight up
    value_ = clampi32((v - min) * 32768.0f / (max - min), 0, 32768);
  }
  int getInteger(int led) { return value_; }
  
  MIN min_;
  MAX max_;
  int value_;
};

template<int MIN, int MAX>
using BladeAngle = BladeAngleX<Int<MIN>, Int<MAX>>;

#endif
