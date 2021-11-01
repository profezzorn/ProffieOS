#ifndef FUNCTIONS_SWING_SPEED_H
#define FUNCTIONS_SWING_SPEED_H

#include "../common/fuse.h"
#include "svf.h"

// Usage: SwingSpeed<MAX>
// Returns 0-32768 based on swing speed
// returned value: INTEGER
template<class MAX>
class SwingSpeedSVF {
 public:
  void run(BladeBase* blade) { max_.run(blade); }
 int calculate(BladeBase* blade) {
   float v = fusor.swing_speed() / max_.calculate(blade);
   return clampi32(v * 32768, 0, 32768);
  }
private:
  PONUA SVFWrapper<MAX> max_;
};

template<class MAX>
using SwingSpeedX = SingleValueAdapter<SwingSpeedSVF<MAX>>;

template<int MAX>
using SwingSpeed = SwingSpeedX<Int<MAX>>;

// Usage: SwingAcceleration<MAX>
// Returns 0-32768 based on swing acceleration
// MAX defaults to 150
// returned value: INTEGER
template<class MAX = Int<130>>
class SwingAccelerationSVF {
 public:
  void run(BladeBase* blade) { max_.run(blade); }
 int calculate(BladeBase* blade) {
   float v = fusor.swing_accel() / max_.calculate(blade);
   return clampi32(v * 32768, 0, 32768);
  }
private:
  PONUA SVFWrapper<MAX> max_;
};

template<class MAX = Int<130>>
using SwingAccelerationX = SingleValueAdapter<SwingAccelerationSVF<MAX>>;

template<int MAX = 130>
using SwingAcceleration = SwingAccelerationX<Int<MAX>>;

#endif
