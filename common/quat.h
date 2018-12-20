#ifndef COMMON_QUAT_H
#define COMMON_QUAT_H

#include "vec3.h"

class Quat {
public:
  
  Quat() : w_(1.0), v_(0.0, 0.0, 0.0) {}
  Quat(float w, Vec3 v) : w_(w), v_(v) {}
  Quat(float w, float x, float y, float z) : w_(w), v_(x,y,z) {}

  // Create a rotated quat (rotate angle around vector)
  Quat(Vec3 vector, float angle)
    : w_(cosf(angle/2)),
      v_(vector * sinf(angle/2)) {
  }
  
  
  Quat operator+(Quat q) {
    return Quat(w+q.w, v+q.v);
  }
  
  // norm
  float len2() {
    return w*w + v.len2();
  }
  
  // magnitude
  float len() {
    return sqrtf(len2());
  }
  
  Quat normalize() {
    float l=1.0f / len();
    return Quat(w*l, v*l);
  }
  
  // negate
  Quat operator-() {
    return Quat(-w,-v);
  }
  
  // conjugate
  Quat conjugate() {
    return Quat(w,-v);
  }
  
  Quat invert() {
    return conjugate() / len2();
  }
  
  Quat operator*(float f) {
    return Quat(w * f, v * f);
  }
  
  Quat operator*(Quat q) {
    return Quat(w * q.w - v.dot(q.v),
		q.v * w + v * q.w + v.cross(q.v));
  }
  
  Quat operator/(float f) {
    return Quat(w / f, v / f);
  }
  
  Quat operator/(Quat q) {
    return (*this) * q.invert();
  }
  
  // Create a rotated quat (euler angles)
  Quat(Vec3 angles) {
    *this =
      Quat(Vec3(1,0,0), angles.x) *
      Quat(Vec3(0,1,0), angles.y) *
      Quat(Vec3(0,0,1), angles.z) ;
  }

  // Rotation
  Vec3 operator*(Vec3 v) {
    return ((*this) * Quat(0,v) * invert()).v;
  }
  
private:
  float w_;
  Vec3 v_;
};
