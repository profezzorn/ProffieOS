#ifndef COMMON_ANGLE_H
#define COMMON_ANGLE_H

// A floating point value that stays in the
// -PI ... PI range.
class Angle {
public:
  static constexpr float TRUNC(float f) {
    return
      (f >= M_PI) ? (f - M_PI * 2) :
      (f < -M_PI) ? (f + M_PI * 2) :
      f;
  }
  constexpr Angle() : angle_(0.0f) {}
  constexpr Angle(float f) : angle_(TRUNC(f)) {}
  operator float() const { return angle_; }
  static constexpr Angle fromFixed0(int a) {
    return Angle((a >= 16384 ?  a - 32768 : a)* (M_PI / 16384.0));
  }
  static constexpr Angle fromFixed(int a) {
    return Angle((a - 16384) * (M_PI / 16384.0));
  }
  static constexpr Angle fromFixedWithMargin(int a, float margin_fraction) {
    return
      a >= 32767 ? Angle(M_PI * (1.0 - margin_fraction / 5.0)) :
      a <= 0 ? Angle(-M_PI * (1.0 - margin_fraction / 5.0))  :
      Angle((a - (16384.0 * (1.0 + margin_fraction))) * (M_PI / (16384.0 * (1.0 + margin_fraction))));
  }
  constexpr Angle operator+(Angle other) const {
    return Angle(angle_ + other.angle_);
  }
  void operator+=(Angle other) {
    angle_ = TRUNC(angle_ + other.angle_);
  }
  constexpr Angle operator-(Angle other) const {
    return Angle(angle_ - other.angle_);
  }
  void operator-=(Angle other) {
    angle_ = TRUNC(angle_ - other.angle_);
  }
  constexpr Angle operator*(float mul) const {
    return Angle(angle_ * mul);
  }
  constexpr Angle operator/(float div) const {
    return Angle(angle_ / div);
  }
  constexpr bool operator>(float f) const {
    return angle_ > f;
  }
  constexpr bool operator>(int f) const {
    return angle_ > f;
  }
  constexpr bool operator<(float f) const {
    return angle_ < f;
  }
  // Returns 1 if we hit the top rail
  // Returns -1 if we hit the bottom rail
  // Returns 0 otherwise 
  int increment_with_guardrails(Angle diff) {
    angle_ += diff.angle_;
    if (angle_ >= M_PI) {
      angle_ = M_PI;
      return 1;
    }
    if (angle_ < -M_PI) {
      angle_ = -M_PI;
      return -1;
    }
    return 0;
  }
  // 0-32767 returns 0 if angle_ is 0.0
  int fixed0() const {
    int ret = angle_ * (16384.0 / M_PI);
    if (ret < 0) ret += 32768;
    return ret;
  }
  // 0-32767 returns 16384 if angle_ is 0.0
  int fixed() const {
    int ret = angle_ * (16384.0 / M_PI);
    ret += 16384;
    return clampi32(ret, 0, 32767);
  }
  // 0-32767 returns 16384 if angle_ is 0.0
  int fixed_with_margin(float margin_fraction) const {
    float tmp = 16384.0 * (1.0 + margin_fraction);
    float ret = angle_ * (tmp / M_PI);
    ret += 16384.0;
    return clampi32(ret, 0, 32767);
  }
private:
  float angle_; // [-PI .. PI)
};

#endif  // COMMON_ANGLE_H
