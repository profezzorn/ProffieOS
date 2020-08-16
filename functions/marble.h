#ifndef FUNCTION_MARBLE_H
#define FUNCTION_MARBLE_H

#include <algorithm>

// Usage: MarbleF<OFFSET, PERCENT, FRICTION, ACCELERATION, GRAVITY>
// or: MarbleF<PERCENT, RPM, ON_PERCENT, ON_RPM, FADE_TIME_MILLIS>
// RPM, PERCENT, ON_PERCENT, ON_RPM, FADE_TIME_MILLIS: a number
// return value: COLOR
// This is intended for a small ring of neopixels
// A section of the ring is lit at the specified color
// and rotates at the specified speed. The size of the
// lit up section is defined by "percentage".
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

    float rad = (pos_ + offset_.getInteger(0)/32768.0) * M_PI * 2.0;
    Vec3 down = fusor.accel();
    float gravity = gravity_.getInteger(0) / 32768.0;
    float accel = (down.y * sinf(rad) + down.z * cosf(rad)) * gravity;
    accel += acceleration_.getInteger(0) / 32768.0;
    accel -= speed_ * friction_.getInteger(0) / 32768.0;
    speed_ += accel * fraction;
    pos_ = fract(pos_ + speed_ * fraction);
    value_ = pos_ * 32768;
  }
  int getInteger(int led) { return value_; }
private:
  OFFSET offset_;
  FRICTION friction_;
  ACCELERATION acceleration_;
  GRAVITY gravity_;
  
  float pos_ = 0.0;
  float speed_ = 0.0;
  uint32_t last_micros_;
  int value_;
};

#endif
