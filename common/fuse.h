#ifndef COMMON_FUSE_H
#define COMMON_FUSE_H

#include "vec3.h"
#include "quat.h"
#include "saber_base.h"
#include "extrapolator.h"
#include "box_filter.h"

// #define FUSE_SPEED

#ifndef ACCEL_MEASUREMENTS_PER_SECOND
#define ACCEL_MEASUREMENTS_PER_SECOND 800
#endif
#ifndef GYRO_MEASUREMENTS_PER_SECOND
#define GYRO_MEASUREMENTS_PER_SECOND 800
#endif

#if 1 // def DEBUG

#if 0
static bool my_isnan(float val) {
  if (val < 10000000 && val > -1000000) return false;
  return true;
}
#else
static bool my_isnan(float val) {
  // Returns true for +/- infinit as well.
  union { uint32_t i; float v; } tmp;
  tmp.v = val;
  return (tmp.i & 0x7f800000UL) == 0x7f800000UL;
}
#endif

static bool my_isnan(Vec3 v) {
  return my_isnan(v.x) || my_isnan(v.y) || my_isnan(v.z);
}
static bool my_isnan(Quat q) {
  return my_isnan(q.w_) || my_isnan(q.v_);
}

int nan_count = 0;

#define CHECK_NAN(X) do {                       \
 if (nan_count < 20 && my_isnan((X))) {         \
    nan_count++;                                \
    STDOUT << "NAN " << #X << " = " << (X) << " LINE " << __LINE__ << "\n"; \
  }                                             \
}while(0)

#else

#define CHECK_NAN(X) do { } while(0)

#endif

float exp2_fast(float x) {
  int i = floorf(x);
  x -= i;
  return ldexpf(-8.24264/(x-3.41421)-1.41421356237, i);
}


class Fusor : public Looper {
public:
  Fusor() :
#ifdef FUSE_SPEED
  speed_(0.0),
#endif
    down_(0.0), last_micros_(0) {
  }
  const char* name() override { return "Fusor"; }
  void DoMotion(const Vec3& gyro, bool clear) {
    CHECK_NAN(gyro);
    if (clear) {
      gyro_extrapolator_.clear(gyro);
      gyro_clash_filter_.clear(gyro);
    } else {
      gyro_extrapolator_.push(gyro);
      gyro_clash_filter_.push(gyro);
    }
  }
  void DoAccel(const Vec3& accel, bool clear) {
    CHECK_NAN(accel);
    if (clear) {
      accel_extrapolator_.clear(accel);
      accel_clash_filter_.clear(accel);
      down_ = accel;
      last_clear_ = micros();
    } else {
      accel_extrapolator_.push(accel);
      accel_clash_filter_.push(accel);
    }
  }

#ifndef GYRO_STABILIZATION_TIME_MS
#define GYRO_STABILIZATION_TIME_MS 64
#endif

  void Loop() override {
    uint32_t last_accel = accel_extrapolator_.last_time();
    uint32_t last_gyro = gyro_extrapolator_.last_time();
    uint32_t now = micros();
    if (!accel_extrapolator_.ready() ||
	!gyro_extrapolator_.ready() ||
	now - last_accel > 200000 ||
	now - last_gyro > 200000 ||
	now - last_clear_ < GYRO_STABILIZATION_TIME_MS * 1000) {
      gyro_ = Vec3(0.0f);
      accel_ = Vec3(0.0f);
      swing_speed_ = 0.0f;
      return;
    }

    float delta_t = (now - last_micros_) / 1000000.0;
    last_micros_ = now;
    // Update last_clear_ so we won't have wrap-around issues.
    last_clear_ = now - GYRO_STABILIZATION_TIME_MS * 1000;
    CHECK_NAN(delta_t);

    accel_ = accel_extrapolator_.get(now);
    CHECK_NAN(accel_);
    gyro_ = gyro_extrapolator_.get(now);
    CHECK_NAN(gyro_);
    swing_speed_ = -1.0f;
    angle1_ = 1000.0f;
    angle2_ = 1000.0f;

    // Quat rotation = Quat(gyro_ * -(std::min(delta_t, 0.01f) * M_PI / 180.0));
    Quat rotation = Quat(1.0, gyro_ * -(std::min(delta_t, 0.01f) * M_PI / 180.0 / 2.0)).normalize();
    // fprintf(stderr, "ACCL={%f,%f,%f} ", accel_.x, accel_.y, accel_.z);
    // fprintf(stderr, "ROT={%f, %f,%f,%f} ", rotation.w_, rotation.v_.x, rotation.v_.y, rotation.v_.z);
    // fprintf(stderr, "GX=%f ", gyro_.x * (std::min(delta_t, 0.01f) * M_PI / 180.0));
    CHECK_NAN(rotation);
    // down_ = down_.Rotate(gyro_ * (std::min(delta_t, 0.01f) * M_PI / 180.0));
    down_ = rotation.rotate_normalized(down_);
    //    down_ = rotation * speed_;
    CHECK_NAN(down_);

#ifdef FUSE_SPEED
    speed_ = rotation.rotate_normalized(speed_);
//    speed_ = rotation * speed_;
    CHECK_NAN(speed_);
#endif

#define G_constant 9.80665

    float wGyro = 1.0;
    CHECK_NAN(wGyro);
    // High gyro speed means trust acceleration less.
    wGyro += gyro_.len() / 100.0;
    CHECK_NAN(wGyro);

    // If acceleration is not near "down", don't trust it.
    //wGyro += (accel_ - down_).len2();

    // If acceleration is not 1.0G, don't trust it.
    wGyro += fabsf(accel_.len() - 1.0f) * 50.0;

    // If acceleration is changing rapidly, don't trust it.
    wGyro += accel_extrapolator_.slope().len() * 1000;
    CHECK_NAN(wGyro);

    mss_ = (accel_ - down_) * G_constant; // change unit from G to m/s/s
    CHECK_NAN(mss_);

#ifdef FUSE_SPEED
    speed_ += mss_ * std::min(delta_t, 0.01f);
    CHECK_NAN(speed_);

    // FIXME?
    // Speed higher than 100m/s? How about "no"
    if (speed_.len() > 100.0) {
      speed_ *= 100.0 / speed_.len();
    }

#if 1
    speed_ = speed_.MTZ(0.2 * delta_t);
#else
    float delta_factor = powf(0.75, delta_t);
    CHECK_NAN(delta_factor);
    speed_ = speed_ * delta_factor;
    CHECK_NAN(speed_.x);
#endif
#endif
    // goes towards 1.0 when moving.
    // float gyro_factor = powf(0.01, delta_t / wGyro);
    // float gyro_factor = expf(logf(0.01) * delta_t / wGyro);
    float gyro_factor = expf(logf(0.01) * delta_t / wGyro);
    CHECK_NAN(gyro_factor);
    down_ = down_ *  gyro_factor + accel_ * (1.0 - gyro_factor);
    // Might be a good idea to normalize down_, but then
    // down_ and accel_ might be slightly different length,
    // so we would need to use MTZ() on mss_.
    CHECK_NAN(down_.x);

    if (monitor.ShouldPrint(Monitoring::MonitorFusion)) {
      STDOUT << "Acl" << accel_ << "(" << accel_.len() << ")"
             << " Gyro" << gyro_
        // << " rotation=" << rotation << "(" << rotation.len() << ")"
             << " dn" << down_ << "(" << down_.len() << ")"
             << " mss" << mss_  << "(" << mss_.len() << ")"
        //     << " Speed=" << speed_ << " (" << speed_.len() << ")"
             << " ss=" << swing_speed()
             << " wGyro=" << wGyro
             << " dt=" << (delta_t * 1000)
//           << " delta factor=" << delta_factor
             << " factor=" << gyro_factor
             << " slope=" << gyro_slope().len()
             << "\n";
    }
  }

  bool freefall() const {
    // TODO: Cancel out centripital force?
    return accel_.len2() < 0.1;
  }

  // PI/2 = straight up, -PI/2 = straight down
  float angle1() {
    // use orientation!
    if (angle1_ == 1000.0f) {
      angle1_ = atan2f(down_.x, sqrtf(down_.z * down_.z + down_.y * down_.y));
    }
    return angle1_;
  }

  // Twist angle.
  // Note: Twist speed is simply gyro().z!
  float angle2() {
    if (angle2_ == 1000.0f) {
      angle2_ = atan2f(down_.y, down_.z);
    }
    return angle2_;
  }

  // 0 = up, +/-PI = down, PI/2 = left, -PI/2 = right
  float pov_angle() {
    return atan2f(down_.y, down_.x);
  }

#if 1
  float swing_speed() {
    if (swing_speed_ < 0) {
      swing_speed_ = sqrtf(gyro_.z * gyro_.z + gyro_.y * gyro_.y);
      if (nan_count < 20 && my_isnan(swing_speed_)) {
	nan_count++;
	STDOUT << "\nNAN swing_speed_ " << gyro_;
      }
    }
    return swing_speed_;
  }
#else
  // Distance in meters.
  float swing_speed_at_distance(float dist) {
    float y = gyro_.z * dist * (M_PI / 180.0) + speed_.y;
    float z = gyro_.y * dist * (M_PI / 180.0) - speed_.z;
    return sqrtf(y*y + z*z);
  }
  float func(float A, float B, float C, float D, float x) {
    float d1 = A * x + B;
    float d2 = C * x + D;
    return sqrtf(d1 * d1 + d2 * d2);
  }
  // Integral of F()dx
  float FIntegral(float A, float B, float C, float D, float x) {
    float a = A * A + C * C;
    float b = 2.0f * (A * B + C * D);
    float c = B * B + D * D;
    float f = func(A, B, C, D, x);
    float a2sq = 2.0 * sqrtf(a);
    float ax2 = 2.0 * a * x;
    float ax2_b = ax2 + b;
    float a2sqf = a2sq * f;
    return ((4.0 * a * c - b * b) * logf(a2sqf + ax2_b) + a2sqf * ax2_b)/(4.0 * a * a2sq);
  }
  float swing_speed_integral(float x) { // x = meters
    float A = gyro_.z * (M_PI / 180.0); // radians/s
    float C = gyro_.y * (M_PI / 180.0); // radians/s
    float B =  speed_.y;  // m/s
    float D = -speed_.z;  // m/s
    return FIntegral(A, B, C, D, x);
  }
  float swing_speed() {
    if (swing_speed_ < 0) {
      if (speed_.len2() < 0.00001) {
        swing_speed_ = sqrtf(gyro_.z * gyro_.z + gyro_.y * gyro_.y);
      } else if (gyro_.len2() < 0.00001) {
        swing_speed_ = sqrtf(speed_.z * speed_.z + speed_.y * speed_.y);
        swing_speed_ *= (360 / M_PI);        // Scale it back to degrees per second, sort of.
      } else {
        swing_speed_ = (swing_speed_integral(0.9) - swing_speed_integral(0.1));
        swing_speed_ *= (360 / M_PI) / 0.8;  // Scale it back to degrees per second, sort of.
      }
    }

    return swing_speed_;
  }
#endif

  Vec3 gyro() { return gyro_; }    // degrees/s
  Vec3 gyro_slope() {
    // degrees per second per second
    return gyro_extrapolator_.slope() * 1000000;
  }
  Vec3 accel() { return accel_; }  // G/s/s
  Vec3 mss() { return mss_; }      // m/s/s (acceleration - down vector)
  Vec3 down() { return down_; }    // G/s/s (length should be close to 1.0)

  // Meters per second per second
  Vec3 clash_mss() {
    return accel_clash_filter_.get() - down_;
  }

  // Meters per second per second
  float gyro_clash_value() {
#if 0    
    static uint32_t last_printout=0;
    if (millis() - last_printout > 1000) {
      last_printout = millis();
      STDOUT << "GYRO CLASH FILTER: "<< gyro_clash_filter_.get()
	     << " XTRAPOLATOR: "<< gyro_extrapolator_.get(micros())
	     << "\n";
    }
#endif    
    // degrees per microsecond
    float v = (gyro_clash_filter_.get() - gyro_extrapolator_.get(micros())).len();
    // Translate into meters per second per second, assuming blade is one meter.
    return v / 9.81;
  }
  
#ifdef FUSE_SPEED
  Vec3 speed() { return speed_; }  // m/s
#endif

  // Acceleration into swing in radians per second per second
  float swing_accel() {
    Vec3 gyro_slope_ = gyro_slope();
    return sqrtf(gyro_slope_.z * gyro_slope_.z + gyro_slope_.y * gyro_slope_.y) * (M_PI / 180);
  }

  // Acceleration into twist (one direction) in degrees per second per second
  float twist_accel() {
    return gyro_slope().x;
  }

  void dump() {
    STDOUT << " Accel=" << accel_ << " ("<<  accel_.len() << ")"
	   << " Gyro=" << gyro_ << " (" << gyro_.len() << ")"
	   << " down=" << down_ << " (" << down_.len() << ")"
	   << " mss=" << mss_  << " (" << mss_.len() << ")"
	   << "\n";
    STDOUT << " ready=" << ready()
	   << " swing speed=" << swing_speed()
	   << " gyro slope=" << gyro_slope().len()
	   << " last_micros_ = " << last_micros_
	   << " now = " << micros()
	   << "\n";
    STDOUT << " acceleration extrapolator data:\n";
    accel_extrapolator_.dump();
    STDOUT << " gyro extrapolator data:\n";
    gyro_extrapolator_.dump();
  }

  bool ready() { return micros() - last_micros_ < 50000; }

private:
  uint32_t last_clear_ = 0;
  static const int filter_hz = 80;
  static const int clash_filter_hz = 1600;
  Extrapolator<Vec3, ACCEL_MEASUREMENTS_PER_SECOND/filter_hz> accel_extrapolator_;
  Extrapolator<Vec3, GYRO_MEASUREMENTS_PER_SECOND/filter_hz> gyro_extrapolator_;
  BoxFilter<Vec3, ACCEL_MEASUREMENTS_PER_SECOND/clash_filter_hz> accel_clash_filter_;
  BoxFilter<Vec3, ACCEL_MEASUREMENTS_PER_SECOND/clash_filter_hz> gyro_clash_filter_;

#ifdef FUSE_SPEED
  Vec3 speed_;
#endif
  Vec3 down_;
  Vec3 mss_;
  uint32_t last_micros_;
  Vec3 accel_;
  Vec3 gyro_;
  float swing_speed_;
  float angle1_;
  float angle2_;
};

Fusor fusor;

// For debugging
template<class T, int N>
class PeakPrinter {
public:
  void Add(T v, bool print) {
    for (int i = 0; i < N-1; i++) tmp_[i] = tmp_[i+1];
    tmp_[N-1] = v;
    if (print)  {
      HighPeak();
      LowPeak();
    }
  }
  void HighPeak() {
    for (int i = 0; i < N-1; i++) {
      if (tmp_[i] > tmp_[N/2]) return;
    }
    STDOUT << " HIGHPEAK: " << tmp_[N/2] << "\n";
  }
  void LowPeak() {
    for (int i = 0; i < N-1; i++) {
      if (tmp_[i] < tmp_[N/2]) return;
    }
    STDOUT << " LOWPEAK: " << tmp_[N/2] << "\n";
  }
private:
  T tmp_[N];
};

#endif
