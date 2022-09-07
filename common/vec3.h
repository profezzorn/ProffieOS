#ifndef COMMON_VEC3_H
#define COMMON_VEC3_H

// Simple 3D vector.
class Vec3 {
public:
  Vec3(){}
  Vec3(float v_) : x(v_), y(v_), z(v_) {}
  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

  enum ByteOrder {
    BYTEORDER_MSB = 0,
    BYTEORDER_LSB = 1,
  };
  enum Orientation {
    ORIENTATION_NORMAL = 0x00,
    ORIENTATION_FETS_TOWARDS_BLADE = 0x00,
    ORIENTATION_USB_TOWARDS_BLADE = 0x10,
    ORIENTATION_SDA_TOWARDS_BLADE = 0x02,
    ORIENTATION_SERIAL_TOWARDS_BLADE = 0x12,
    ORIENTATION_TOP_TOWARDS_BLADE = 0x04,
    ORIENTATION_BOTTOM_TOWARDS_BLADE = 0x14,
  };

  Vec3 operator+(const Vec3& o) const {
    return Vec3(x + o.x, y + o.y, z + o.z);
  }
  Vec3 operator+(float f) const {
    return Vec3(x + f, y + f, z + f);
  }
  Vec3 operator-(const Vec3& o) const {
    return Vec3(x - o.x, y - o.y, z - o.z);
  }
  Vec3 operator-(float f) const {
    return Vec3(x - f, y - f, z - f);
  }
  Vec3 operator-() const {
    return Vec3(-x, -y, -z);
  }
  void operator+=(const Vec3& o)  {
    x += o.x;
    y += o.y;
    z += o.z;
  }
  void operator-=(const Vec3& o)  {
    x -= o.x;
    y -= o.y;
    z -= o.z;
  }
  Vec3 operator*(float f) const {
    return Vec3(x * f, y * f, z * f);
  }
  Vec3 operator*(const Vec3& o) const {
    return Vec3(x * o.x, y * o.y, z * o.z);
  }
  Vec3 operator*=(float f) {
    x*=f;
    y*=f;
    z*=f;
    return *this;
  }
  Vec3 operator/(int i) const {
    return Vec3(x / i, y / i, z / i);
  }
  float dot(const Vec3& o) const {
    return x * o.x + y * o.y + z * o.z;
  }
  Vec3 cross(const Vec3& o) const {
    return Vec3(y * o.z - z * o.y,
		z * o.x - x * o.z,
		x * o.y - y * o.x);
  }
  float len2() const { return x*x + y*y + z*z; }
  float len() const { return sqrtf(len2()); }

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

  // TODO: Check if these three rotate the right direction
  Vec3 RotateX(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return Vec3(x, y * c + z * s, y * -s + z * c);
  }
  Vec3 RotateY(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return Vec3(x * c - z * s, y, y * s + z * c);
  }
  Vec3 RotateZ(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return Vec3(x * c - y * s, x * s + y * c, z);
  }
  Vec3 Rotate(Vec3 angles) {
    return RotateX(angles.x).RotateY(angles.y).RotateZ(angles.z);
  }

  // move towards zero
  Vec3 MTZ(float delta) {
    float l = len();
    float new_length = l - delta;
    if (new_length <= 0.0) return Vec3(0.0f);
    return (*this) * (new_length / l);
  }

  void printTo(Print& p) {
    p.write('{');
    p.print(x);
    p.write(',');
    p.write(' ');
    p.print(y);
    p.write(',');
    p.write(' ');
    p.print(z);
    p.write('}');
  }
  float x, y, z;
};

struct Motion {
  Motion() {}
  explicit Motion(float v) : accel(v), gyro(v) {}
  Motion(const Vec3& a, const Vec3& g) : accel(a), gyro(g) {}

  Motion operator+(const Motion& o) const { return Motion(accel + o.accel, gyro + o.gyro); }
  Motion operator-(const Motion& o) const { return Motion(accel - o.accel, gyro - o.gyro); }
  Motion operator*(float f) const { return Motion(accel * f, gyro * f); }
  void operator+=(const Motion& o) {
    accel += o.accel;
    gyro += o.gyro;
  }
  void operator-=(const Motion& o) {
    accel -= o.accel;
    gyro -= o.gyro;
  }
  
  Vec3 accel;
  Vec3 gyro;
};

#endif
