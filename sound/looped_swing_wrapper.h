#ifndef SOUND_LOOPED_SWING_WRAPPER_H
#define SOUND_LOOPED_SWING_WRAPPER_H

// SmoothSwing V1
// Looped swing sounds is a new way to play swing sounds.
// Basically, two swing sounds (swingl and swingh) are always
// playing in the background, but with zero volume. When
// the saber is swung, we fade out the hum and fade in one of
// the swingl/swingh sounds. This type of swing sounds can
// be added to any font by just adding swingl.wav and swingh.wav.
class LoopedSwingWrapper : public SaberBasePassThrough {
public:
  LoopedSwingWrapper() : SaberBasePassThrough() {}

  void Activate(SaberBase* base_font) {
    STDOUT.println("Activating looped swing sounds");
    SetDelegate(base_font);
  }

  void Deactivate() {
    SetDelegate(NULL);
    low_.Free();
    high_.Free();
  }

  RefPtr<BufferedWavPlayer> low_;
  RefPtr<BufferedWavPlayer> high_;
  void SB_On() override {
    // Starts hum, etc.
    delegate_->SB_On();
    low_ = GetFreeWavPlayer();
    if (low_) {
      low_->set_volume_now(0);
      low_->PlayOnce(&SFX_swingl);
      low_->PlayLoop(&SFX_swingl);
    } else {
      STDOUT.println("Looped swings cannot allocate wav player.");
    }
    high_ = GetFreeWavPlayer();
    if (high_) {
      high_->set_volume_now(0);
      high_->PlayOnce(&SFX_swingh);
      high_->PlayLoop(&SFX_swingh);
    } else {
      STDOUT.println("Looped swings cannot allocate wav player.");
    }
#if 0
    STDOUT.print("UNITS: ");
    STDOUT.print(WhatUnit(low_));
    STDOUT.print(",");
    STDOUT.println(WhatUnit(high_));
#endif
  }
  void SB_Off(OffType off_type) override {
    if (low_) {
      low_->set_fade_time(0.3);
      low_->FadeAndStop();
      low_.Free();
    }
    if (high_) {
      high_->set_fade_time(0.3);
      high_->FadeAndStop();
      high_.Free();
    }
    delegate_->SB_Off(off_type);
  }

  void SB_Motion(const Vec3& gyro, bool clear) override {
    float speed = sqrtf(gyro.z * gyro.z + gyro.y * gyro.y);
    uint32_t t = (millis() >> 2);
    float s = sin_table[t & 1023] * (1.0/16383);
    float c = sin_table[(t + 256) & 1023] * (1.0/16383);
    float blend = c * gyro.z + s * gyro.y;
    blend = clamp(blend / 150.0, -1.0, 1.0);
    float vol = 0.99 + clamp(speed/200.0, 0.0, 2.3);
    float low = std::max<float>(0, blend);
    float high = std::max<float>(0, -blend);
    float hum = 1.0 - abs(blend);

    delegate_->SetHumVolume(vol * hum);
    if (low_) low_->set_volume(vol * low);
    if (high_) high_->set_volume(vol * high);
    if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
      STDOUT.print("S:");
      STDOUT.print(speed);
      STDOUT.print(" T:");
      STDOUT.print(t);
      STDOUT.print(" s:");
      STDOUT.print(s);
      STDOUT.print(" c:");
      STDOUT.print(c);
      STDOUT.print(" blend:");
      STDOUT.print(blend);
      STDOUT.print(" vol:");
      STDOUT.print(vol);
      STDOUT.print(" hi:");
      STDOUT.print(high);
      STDOUT.print(" lo:");
      STDOUT.print(low);
      STDOUT.print(" hum:");
      STDOUT.println(hum);
    }
  }
};

#endif
