#ifndef FUNCTIONS_SWING_SPEED_H
#define FUNCTIONS_SWING_SPEED_H

#include "../common/fuse.h"

// Usage: SwingSpeed<MAX>
// Returns 0-32768 based on swing speed
// returned value: INTEGER

template<class MAX>
class SwingSpeedX {
 public:
 void run(BladeBase* blade) {
   max_.run(blade);
   float v = fusor.swing_speed() / max_.getInteger(0);
   value_ = clampi32(v * 32768, 0, 32768);
  }
  int getInteger(int led) { return value_; }
  MAX max_;
  int value_;
};

template<int MAX>
using SwingSpeed = SwingSpeedX<Int<MAX>>;

#endif
