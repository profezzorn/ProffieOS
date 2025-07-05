#ifndef SOUND_VOLUME_OVERLAY_H
#define SOUND_VOLUME_OVERLAY_H

const uint32_t kVolumeShift = 14;
const uint32_t kMaxVolume = 1 << kVolumeShift;
const uint32_t kDefaultVolume = kMaxVolume / 2;
// 1 / 500 second for to change the volume. (2ms)
const uint32_t kDefaultSpeed = 500 * kMaxVolume / AUDIO_RATE;

template<class T>
class VolumeOverlay : public T {
public:
  VolumeOverlay() : volume_(kMaxVolume / 100), stop_when_zero_(false), use_fractional_fade_(false), fractional_speed_(0.0f), fractional_accum_(0.0f) {
    volume_.set(kDefaultVolume);
    volume_.set_target(kDefaultVolume);
    volume_.set_speed(kDefaultSpeed);
  }
  int read(int16_t* data, int elements) override {
    SCOPED_PROFILER();
    elements = T::read(data, elements);
    if (volume_.isConstant()) {
      int32_t mult = volume_.value();
      if (mult == kMaxVolume) {
        // Do nothing
      } else if (mult == 0) {
        if (stop_when_zero_.get()) {
          volume_.set_speed(kDefaultSpeed);
          T::StopFromReader();
        }
        for (int i = 0; i < elements; i++) data[i] = 0;
      } else {
        for (int i = 0; i < elements; i++) {
          data[i] = clamptoi16((data[i] * mult) >> kVolumeShift);
        }
      }
    } else {
      for (int i = 0; i < elements; i++) {
        int32_t v = (data[i] * (int32_t)volume_.value()) >> kVolumeShift;
        data[i] = clamptoi16(v);
        if (use_fractional_fade_) {
          fractional_accum_ += fractional_speed_;
          int steps = (int)fractional_accum_;
          fractional_accum_ -= steps;
          for (int s = 0; s < steps; s++) volume_.advance();
        } else {
          volume_.advance();
        }
      }
    }
    return elements;
  }
  float volume() {
    return volume_.value() * (1.0f / (1 << kVolumeShift));
  }
  int volume_target() {
    return volume_.target_;
  }
  void set_volume(int vol) {
    volume_.set_target(vol);
  }
  void set_volume_now(int vol) {
    volume_.set(vol);
    volume_.set_target(vol);
  }
  void reset_volume() {
    set_volume_now((int)kDefaultVolume);
    volume_.set_speed(kDefaultSpeed);
    stop_when_zero_.set(false);
  }
  void set_volume(float vol) {
    set_volume((int)(kDefaultVolume * vol));
  }
  void set_volume_now(float vol) {
    set_volume_now((int)(kDefaultVolume * vol));
  }
  void set_speed(int speed) {
    volume_.set_speed(speed);
  }
  void set_fade_time(float t) {
    // Prevent zero or too-small fade times
    if (t <= 0.01f) { t = 0.01f; }
    fractional_accum_ = 0.0f;
    fractional_speed_ = (float)kMaxVolume / (t * AUDIO_RATE);
    use_fractional_fade_ = true;
  }
  float fade_speed() const {
    return (kMaxVolume / (float)volume_.speed_) / AUDIO_RATE;
  }
  bool isOff() const {
    return volume_.isConstant() && volume_.value() == 0;
  }

  void FadeAndStop() {
    volume_.set_target(0);
    stop_when_zero_.set(true);
  }

  void StartAndFade() {
    if (use_fractional_fade_) {
      volume_.set_speed(1);
    }
    volume_.set(0);
    volume_.set_target(kDefaultVolume);
    stop_when_zero_.set(false);
  }

  void ResetStopWhenZero() {
    stop_when_zero_.set(false);
  }

private:
  ClickAvoiderLin volume_;
  POAtomic<bool> stop_when_zero_;
  bool use_fractional_fade_;
  float fractional_speed_;
  float fractional_accum_;
};

#endif
