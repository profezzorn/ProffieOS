#ifndef FUNCTION_MARBLE_H
#define FUNCTION_MARBLE_H

#include <algorithm>

// Usage: MarbleF<OFFSET, FRICTION, ACCELERATION, GRAVITY>
// OFFSET: FUNCTION  0-32768, adjust until "down" represents is actually down
// FRICTION: FUNCTION, higher values makes the marble slow down, usually a constant
// ACCELERATION: FUNCTION, a function specifying how much speed to add to the marble
// GRAVITY: FUNCTION higher values makes the marble heavier
// return value: FUNCTION  0-32768, representing point on a circle
// This is intended for a small ring of neopixels.
// It runs a simulation of a marble trapped in a circular
// track and returns the position of that marble.
// Meant to be used with CircularSectionF to turn the marble
// position into a lighted up section.
class BladeBase;
template<class OFFSET,
         class FRICTION,
         class ACCELERATION,
         class GRAVITY>
class MarbleF {
public:
  void run(BladeBase* base) {
    offset_.run(base);
    friction_.run(base);
    acceleration_.run(base);
    gravity_.run(base);
    SaberBase::RequestMotion();

    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (delta > 1000000) delta = 1;
    float fraction = delta / 1000000.0;

    float rad = (pos_ + offset_.calculate(base)/32768.0) * M_PI * 2.0;
    Vec3 down = fusor.accel();
    float gravity = gravity_.calculate(base) / 32768.0;
    float accel = (down.y * sinf(rad) + down.z * cosf(rad)) * gravity;
    accel += acceleration_.calculate(base) / 32768.0;
    accel -= speed_ * friction_.calculate(base) / 32768.0;
    speed_ += accel * fraction;
    pos_ = fract(pos_ + speed_ * fraction);
    value_ = pos_ * 32768;
  }
  int getInteger(int led) { return value_; }
private:
  PONUA SVFWrapper<OFFSET> offset_;
  PONUA SVFWrapper<FRICTION> friction_;
  PONUA SVFWrapper<ACCELERATION> acceleration_;
  PONUA SVFWrapper<GRAVITY> gravity_;
  
  float pos_ = 0.0;
  float speed_ = 0.0;
  uint32_t last_micros_;
  int value_;
};

#endif
