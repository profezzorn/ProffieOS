#ifndef COMMON_FUSE_H
#define COMMON_FUSE_H

#include "vec3.h"
#include "quat.h"
#include "saber_base.h"

// #define FUSE_SPEED

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


#if 1
template<class T, int SIZE = 10>
class Extrapolator {
public:
  Extrapolator() {
    uint32_t now = micros();
    for (int i = 0; i < SIZE; i++) {
      v_[i] = T(0);
      t_[i] = now;
    }
  }
  T get() { return get(micros()); }

  T avg_;
  T slope_;
  float avg_t_;
  bool needs_update_ = true;
  void update() {
    if (!needs_update_) return;
    needs_update_ = false;
    uint32_t now = t_[entry_];
    T sum(0.0f);
    float sum_t = 0.0;
    for (size_t i = 0; i < SIZE; i++) {
      float t = now - t_[i];
      sum_t += t;
      sum += v_[i];
    }
    avg_ = sum * (1.0 / SIZE);
    CHECK_NAN(avg_);
    avg_t_ = sum_t / SIZE;
    CHECK_NAN(avg_t_);

#if 0
    // Enable this code if response becomes too slow in low-power mode
    if (avg_t > 200000 /* 200ms */) {
      // Just use last value if everything is too spread out.
      avg_ = last();
      slope_ = T(0.0f);
      return;
    }
#endif    

    T dot_sum(0.0f);
    float t_square_sum = 0.0;
    for (size_t i = 0; i < SIZE; i++) {
      float t = avg_t_ - (now - t_[i]);
      T v = v_[i] - avg_;
      t_square_sum += t * t;
      dot_sum += v * t;
    }
    CHECK_NAN(dot_sum);
    CHECK_NAN(t_square_sum);
    CHECK_NAN(sum);
    if (t_square_sum == 0.0) {
      slope_ = T(0.0f);
    } else {
      slope_ = dot_sum * (1.0 / t_square_sum);
      CHECK_NAN(slope_);
    }
  }
  T get(uint32_t now) {
    update();
    return avg_ + slope_ * (now - t_[entry_] + avg_t_);
  }
  T slope() {
    update();
    return slope_;
  }
  void push(const T& value, uint32_t now) {
    entry_++;
    if (entry_ >= SIZE) entry_ = 0;
    v_[entry_] = value;
    t_[entry_] = now;
    values_++;
    needs_update_ = true;
  }
  void push(const T& value) {
    push(value, micros());
  }
  void clear(const T& value) {
    uint32_t now = micros();
    for (int i = 0; i < SIZE; i++)
      push(value, now);
    values_ = 1;
  }
  bool ready() { return values_ >= SIZE; }
  T& last() { return v_[entry_]; }
  uint32_t last_time() { return t_[entry_]; }

  uint32_t t_[SIZE];
  T v_[SIZE];
  size_t entry_;
  size_t values_;
};
#else
template<class T, int SIZE = 10>
class Extrapolator {
public:
  Extrapolator() {}
  T get() { return get(micros()); }
  T get(uint32_t now) { return value_; }
  T slope() { return 0.0; }
  void push(const T& value, uint32_t now) { value_ = value; t_ = now; }
  void push(const T& value) { push(value, micros());  }
  void clear(const T& value) { value_ = value;  }
  bool ready() { return true; }
  uint32_t last_time() { return t_; }
  T value_;
  uint32_t t_;
};
#endif
class Fusor : public SaberBase, Looper {
public:
  Fusor() :
#ifdef FUSE_SPEED
  speed_(0.0),
#endif
    down_(0.0), last_micros_(0) {
  }
  const char* name() override { return "Fusor"; }
  void SB_Motion(const Vec3& gyro, bool clear) override {
    CHECK_NAN(gyro);
    if (clear) {
      gyro_extrapolator_.clear(gyro);
    } else {
      gyro_extrapolator_.push(gyro);
    }
  }
  void SB_Accel(const Vec3& accel, bool clear) override {
    CHECK_NAN(accel);
    if (clear) {
      accel_extrapolator_.clear(accel);
      down_ = accel;
    } else {
      accel_extrapolator_.push(accel);
    }
  }

  void Loop() override {
    uint32_t now = micros();
    if (!accel_extrapolator_.ready()) return;
    if (!gyro_extrapolator_.ready()) return;
    if (now - accel_extrapolator_.last_time() > 1000000) return;
    if (now - gyro_extrapolator_.last_time() > 1000000) return;

    float delta_t = (now - last_micros_) / 1000000.0;
    last_micros_ = now;
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
    float gyro_factor = powf(0.01, delta_t / wGyro);
    CHECK_NAN(gyro_factor);
    down_ = down_ *  gyro_factor + accel_ * (1.0 - gyro_factor);
    // Might be a good idea to normalize down_, but then
    // down_ and accel_ might be slightly different length,
    // so we would need to use MTZ() on mss_.
    CHECK_NAN(down_.x);

    if (monitor.ShouldPrint(Monitoring::MonitorFusion)) {
      STDOUT << " Accel=" << accel_ << "(" << accel_.len() << ")"
             << " Gyro=" << gyro_
        // << " rotation=" << rotation << "(" << rotation.len() << ")"
             << " down=" << down_ << " (" << down_.len() << ")"
             << " mss=" << mss_  << " (" << mss_.len() << ")"
        //     << " Speed=" << speed_ << " (" << speed_.len() << ")"
             << " swing speed=" << swing_speed()
             << " wGyro=" << wGyro
             << " delta_t=" << (delta_t * 1000)
//           << " delta factor=" << delta_factor
             << " gyro factor=" << gyro_factor
             << " gyro slope=" << gyro_slope().len()
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

#ifdef FUSE_SPEED
  Vec3 speed() { return speed_; }  // m/s
#endif

private:
  Extrapolator<Vec3> accel_extrapolator_;
  Extrapolator<Vec3> gyro_extrapolator_;

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
