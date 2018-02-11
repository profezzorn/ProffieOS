#ifndef COMMON_VEC3_H
#define COMMON_VEC3_H

// Simple 3D vector.
class Vec3 {
public:
  Vec3(){}
  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

  static Vec3 MSB(const unsigned char* msb_data, float mul) {
    Vec3 ret;
    ret.x = mul * (int16_t)((msb_data[0] << 8) | (msb_data[1]));
    ret.y = mul * (int16_t)((msb_data[2] << 8) | (msb_data[3]));
    ret.z = mul * (int16_t)((msb_data[4] << 8) | (msb_data[5]));
    return ret;
  }
  static Vec3 LSB(const unsigned char* lsb_data, float mul) {
    Vec3 ret;
    ret.x = mul * (int16_t)((lsb_data[1] << 8) | (lsb_data[0]));
    ret.y = mul * (int16_t)((lsb_data[3] << 8) | (lsb_data[2]));
    ret.z = mul * (int16_t)((lsb_data[5] << 8) | (lsb_data[4]));
    return ret;
  }
  Vec3 operator-(const Vec3& o) const {
    return Vec3(x - o.x, y - o.y, z - o.z);
  }
  Vec3 operator+(const Vec3& o) const {
    return Vec3(x + o.x, y + o.y, z + o.z);
  }
  void operator+=(const Vec3& o)  {
    x += o.x;
    y += o.y;
    z += o.z;
  }
  Vec3 operator*(float f) const {
    return Vec3(x * f, y * f, z * f);
  }
  Vec3 operator/(int i) const {
    return Vec3(x / i, y / i, z / i);
  }
  Vec3 dot(const Vec3& o) const {
    return Vec3(x * o.x, y * o.y, z * o.z);
  }
  float len2() const { return x*x + y*y + z*z; }
  float len() const { return sqrt(len2()); }

  static void Rotate90(float& a, float& b) {
    float tmp = b;
    b = -a;
    a = tmp;
  }
  static void Rotate180(float& a, float& b) {
    a = -a;
    b = -b;
  }
  void RotateX90() { Rotate90(y, z); }
  void RotateZ90() { Rotate90(x, y); }
  void RotateY90() { Rotate90(z, x); }

  void RotateX180() { Rotate180(y, z); }
  void RotateZ180() { Rotate180(x, y); }
  void RotateY180() { Rotate180(z, x); }
  float x, y, z;
};

#endif
