#ifndef COMMON_QUAT_H
#define COMMON_QUAT_H

#include "vec3.h"

class Quat {
public:
  
  Quat() : w_(1.0), v_(0.0, 0.0, 0.0) {}
  Quat(float w, Vec3 v) : w_(w), v_(v) {}
  Quat(float w, float x, float y, float z) : w_(w), v_(x,y,z) {}

  // Rotate from u to v
  Quat(Vec3 u, Vec3 v) {
    float m = sqrtf(2.f + 2.f * u.dot(v));
    w_ = 0.5f * m;
    v_ = u.cross(v) * (1.0f / m);
  }

  // Create a rotated quat (rotate angle around vector)
  Quat(Vec3 vector, float angle)
    : w_(cosf(angle/2)),
      v_(vector * sinf(angle/2)) {
  }
  
  
  Quat operator+(Quat q) {
    return Quat(w_ + q.w_, v_ + q.v_);
  }
  
  // norm
  float len2() {
    return w_ * w_ + v_.len2();
  }
  
  // magnitude
  float len() {
    return sqrtf(len2());
  }
  
  Quat normalize() {
    float l = 1.0f / len();
    return Quat(w_ * l, v_ * l);
  }
  
  // negate
  Quat operator-() {
    return Quat(-w_, -v_);
  }
  
  // conjugate
  Quat conjugate() {
    return Quat(w_, -v_);
  }
  
  Quat invert() {
    return conjugate() / len2();
  }
  
  Quat operator*(float f) {
    return Quat(w_ * f, v_ * f);
  }
  
  Quat operator*(Quat q) {
    return Quat(w_ * q.w_ - v_.dot(q.v_),
		q.v_ * w_ + v_ * q.w_ + v_.cross(q.v_));
  }
  
  Quat operator/(float f) {
    return Quat(w_ / f, v_ / f);
  }
  
  Quat operator/(Quat q) {
    return (*this) * q.invert();
  }
  
  // Create a rotated quat (euler angles)
  Quat(Vec3 angles)
  {
#if 1
    float c1 = cosf(angles.x / 2);
    float c2 = cosf(angles.y / 2);
    float c3 = cosf(angles.z / 2);
    
    float s1 = sinf(angles.x / 2);
    float s2 = sinf(angles.y / 2);
    float s3 = sinf(angles.z / 2);
		
    v_.x = s1 * c2 * c3 + c1 * s2 * s3;
    v_.y = c1 * s2 * c3 - s1 * c2 * s3;
    v_.z = c1 * c2 * s3 + s1 * s2 * c3;
    w_   = c1 * c2 * c3 - s1 * s2 * s3;
#else
    *this =
      Quat(Vec3(1,0,0), angles.x) *
      Quat(Vec3(0,1,0), angles.y) *
      Quat(Vec3(0,0,1), angles.z) ;
#endif    
  }

  // Rotation
  Vec3 operator*(Vec3 v) {
    return ((*this) * Quat(0,v) * invert()).v_;
  }

  Vec3 rotate_normalized(Vec3 v) {
#if 0
    Vec3 ret = ((*this) * Quat(0,v) * conjugate()).v_;
#else
    Vec3 ret = v_ * 2.0f * v_.dot(v) + v * (w_*w_ - v_.dot(v_)) + v_.cross(v) * 2.0f * w_;
#endif    
//    fprintf(stderr, "\nROT: %f %f %f  => %f %f %f\n", v.x, v.y, v.z, ret.x, ret.y, ret.z);
    return ret;
  }
  
  void printTo(Print& p) {
    p.write('{');
    p.print(w_);
    p.write(',');
    p.write(' ');
    v_.printTo(p);
    p.write('}');
  }

  float w_;
  Vec3 v_;
};

#endif
