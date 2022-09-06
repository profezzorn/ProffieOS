#ifndef FUNCTIONS_TWIST_ANGLE_H
#define FUNCTIONS_TWIST_ANGLE_H

#include "../common/fuse.h"

// Usage: TwistAngle<N, OFFSET>
// Returns 0-32768 based on angle of twist
// OFFSET: Adjustable offset (0-32768) to make the twistangle values line up with how you hold the hilt.
// N : Number of times the values goes from 0 to 32768 and back per hilt revolution.
// returned value: FUNCTION, same for all leds

template<int N = 2, int OFFSET = 0>
class TwistAngleSVF {
public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    int a = fusor.angle2() * 32768 / M_PI;
    int value = ((a + OFFSET) * N) & 0xffff;
    if (value >= 0x8000) value = 0x10000 - value;
    return value;
  }
};

template<int N = 2, int OFFSET = 0>
using TwistAngle = SingleValueAdapter<TwistAngleSVF<N, OFFSET>>;

// Usage: TwistAcceleration<MAX>
// Returns 0-32768 based on acceleration of twist in one direction
// MAX : Maximum acceleration needed to return 32768
// returned value: FUNCTION, same for all leds

template<class MAX>
class TwistAccelerationSVF {
 public:
  void run(BladeBase* blade) { max_.run(blade); }
  int calculate(BladeBase* blade) {
    float accel = fusor.twist_accel();
    if (fusor.gyro().z < 0) accel = -accel;
    float v = (accel * 32768 / 360) / max_.calculate(blade);
    return clampi32(v, 0, 32768);
  }
private:
  PONUA SVFWrapper<MAX> max_;
};

template<int MAX = 90>
using TwistAcceleration = SingleValueAdapter<TwistAccelerationSVF<Int<MAX>>>;
#endif
