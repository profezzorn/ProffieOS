#ifndef MOTION_MOTION_UTIL_H
#define MOTION_MOTION_UTIL_H

#ifndef ORIENTATION
#define ORIENTATION ORIENTATION_NORMAL
#endif

class MotionUtil {
public:
  static int16_t GetShort(const unsigned char *data, int LSB) {
    return (data[LSB] << 8) | data[1-LSB];
  }
  static Vec3 FromData(const unsigned char* data, float mul, int LSB,
		       Vec3::Orientation ORIENTATION) {
    Vec3 ret;
    ret.x = mul * GetShort(data + (0 + (ORIENTATION & 7)) % 6, LSB);
    ret.y = mul * GetShort(data + (2 + (ORIENTATION & 7)) % 6, LSB);
    ret.z = mul * GetShort(data + (4 + (ORIENTATION & 7)) % 6, LSB);
    if (ORIENTATION & 0x10) {
      ret.x = - ret.x;
      ret.y = - ret.y;
    }

#ifdef ORIENTATION_ROTATION
    ret = Quat(Vec3(ORIENTATION_ROTATION) * M_PI / 180.0).rotate_normalized(ret);
#endif
    return ret;
  }
};

#endif
