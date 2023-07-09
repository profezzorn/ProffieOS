#ifndef SOUND_SMOOTH_SWING_V2_H
#define SOUND_SMOOTH_SWING_V2_H

// SmoothSwing V2, based on Thexter's excellent work.
// For more details, see:
// http://therebelarmory.com/thread/9138/smoothswing-v2-algorithm-description
//
class SmoothSwingV2 : public SaberBasePassThrough {
public:
  SmoothSwingV2() : SaberBasePassThrough() {}

  void Activate(SaberBase* base_font) {
    STDOUT.println("Activating SmoothSwing V2");
    if (SFX_swingl) {
      L = &SFX_swingl;
      H = &SFX_swingh;
    } else {
      L = &SFX_lswing;
      H = &SFX_hswing;
    }
    SetDelegate(base_font);
    if (L->files_found() != H->files_found()) {
      STDOUT.println("Warning, swingl and swingh should have the same number of files.");
    }
    // check for swngxx files to use as accent swings
    if ((SFX_swng || SFX_swing) > 0 && smooth_swing_config.AccentSwingSpeedThreshold > 0.0) {
      STDOUT.println("Accent Swings Enabled.");
      STDOUT.print("Polyphonic swings: ");
      STDOUT.println(SFX_swng.files_found());
      STDOUT.print("Monophonic swings: ");
      STDOUT.println(SFX_swing.files_found());
      accent_swings_present = true;
      if (SFX_slsh && smooth_swing_config.AccentSlashAccelerationThreshold > 0.0) {
        STDOUT.println("Accent Slashes Enabled.");
        STDOUT.print("Polyphonic slashes: ");
        STDOUT.println(SFX_slsh.files_found());
        accent_slashes_present = true;
      } else {
        accent_slashes_present = false;
        STDOUT.println("Accent Slashes NOT Detected: ");
      }
    } else {
      accent_swings_present = false;
      STDOUT.println("Accent Swings NOT Detected: ");
    }
  }

  void Deactivate() {
    SetDelegate(NULL);
    A.Free();
    B.Free();
  }

  void Swap() {
    Data C = A;
    A = B;
    B = C;
  }

  // Should only be done when the volume is near zero.
  void PickRandomSwing() {
    if (!on_) return;
    uint32_t m = millis();
    RefPtr<BufferedWavPlayer> humplayer = GetWavPlayerPlaying(hybrid_font.getHum());
    if (!humplayer) return;
    float start = (font_config.ProffieOSSmoothSwingHumstart == 0) ? m / 1000.0 : humplayer->pos();
    // No point in picking a new random so soon after picking one.
    if (A.player && m - last_random_ < 1000) return;
    last_random_ = m;
    int swing = random(L->files_found());
    A.Stop();
    B.Stop();
    L->Select(swing);
    H->Select(swing);
    A.Play(L, start);
    B.Play(H, start);
    if (random(2)) Swap();
    float t1_offset = random(1000) / 1000.0 * 50 + 10;
    A.SetTransition(t1_offset, smooth_swing_config.Transition1Degrees);
    B.SetTransition(t1_offset + 180.0,
      smooth_swing_config.Transition2Degrees);
  }

  void SB_On() override {
    on_ = true;
    // Starts hum, etc.
    delegate_->SB_On();
    PickRandomSwing();
  }
  void SB_Off(OffType off_type) override {
    on_ = false;
    A.Off();
    B.Off();
    delegate_->SB_Off(off_type);
  }

  enum class SwingState {
    OFF, // waiting for swing to start
    ON,  // swinging
    OUT, // Waiting for sound to fade out
  };

  void SB_Motion(const Vec3& raw_gyro, bool clear) override {
    if (clear) {
      gyro_filter_.filter(raw_gyro);
      gyro_filter_.filter(raw_gyro);
    }
    Vec3 gyro = gyro_filter_.filter(raw_gyro);
    // degrees per second
    // May not need to smooth gyro since volume is smoothed.
    float speed = sqrtf(gyro.z * gyro.z + gyro.y * gyro.y);
    uint32_t t = micros();
    uint32_t delta = t - last_micros_;
    if (delta > 1000000) delta = 1;
    last_micros_ = t;
    float hum_volume = 1.0;

    switch (state_) {
      case SwingState::OFF:
	if (!A.player || !B.player) {
	  PickRandomSwing();
	}
        if (speed < smooth_swing_config.SwingStrengthThreshold) {
#if 1
          if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
            STDOUT.print("speed: ");
            STDOUT.println(speed);
          }
#endif
          break;
        }
        state_ = SwingState::ON;

      case SwingState::ON:
        // trigger accent swing
        if (accent_swings_present && (A.isPlaying() || B.isPlaying())) {
          delegate_->StartSwing(gyro, smooth_swing_config.AccentSwingSpeedThreshold,
          smooth_swing_config.AccentSlashAccelerationThreshold);
        }
        if (speed >= smooth_swing_config.SwingStrengthThreshold * 0.9) {
          float swing_strength =
            std::min<float>(1.0, speed / smooth_swing_config.SwingSensitivity);
          A.rotate(-speed * delta / 1000000.0);
          // If the current transition is done, switch A & B,
          // and set the next transition to be 180 degrees from the one
          // that is done.
          while (A.end() < 0.0) {
            B.midpoint = A.midpoint + 180.0;
	    Swap();
          }
          float mixab = 0.0;
          if (A.begin() < 0.0)
            mixab = clamp(- A.begin() / A.width, 0.0, 1.0);

          float mixhum =
            powf(swing_strength, smooth_swing_config.SwingSharpness);

          hum_volume =
            1.0 - mixhum * smooth_swing_config.MaximumHumDucking / 100.0;

          mixhum *= smooth_swing_config.MaxSwingVolume;

          if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
            STDOUT.print("speed: ");
            STDOUT.print(speed);
            STDOUT.print(" R: ");
            STDOUT.print(-speed * delta / 1000000.0);
            STDOUT.print(" MP: ");
            STDOUT.print(A.midpoint);
            STDOUT.print(" B: ");
            STDOUT.print(A.begin());
            STDOUT.print(" E: ");
            STDOUT.print(A.end());
            STDOUT.print("  mixhum: ");
            STDOUT.print(mixhum);
            STDOUT.print("  mixab: ");
            STDOUT.print(mixab);
            STDOUT.print("  hum_volume: ");
            STDOUT.println(hum_volume);
          }
          if (on_) {
            // We need to stop setting the volume when off, or playback may never stop.
            mixhum = delegate_->SetSwingVolume(swing_strength, mixhum);
            A.set_volume(mixhum * mixab);
            B.set_volume(mixhum * (1.0 - mixab));
          }
          break;
        }
        A.set_volume(0);
        B.set_volume(0);
        state_ = SwingState::OUT;

      case SwingState::OUT:
        if (!A.isOff() || !B.isOff()) {
          if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
            Serial.println("Waiting for volume = 0");
          }
        }
        PickRandomSwing();
        state_ = SwingState::OFF;
    }
    // Must always set hum volume, or fade-out doesn't work.
    delegate_->SetHumVolume(hum_volume);
  }

private:
  struct Data {
    void set_volume(float v) {
      if (player) player->set_volume(v);
    }
    void Play(Effect* effect, float start = 0.0) {
      if (!player) {
	player = GetFreeWavPlayer();
	if (!player) return;
      }
      player->set_volume(0.0f);
      player->PlayOnce(effect, start);
      player->PlayLoop(effect);
    }
    bool isPlaying() {
      if (!player) return false;
      return player->isPlaying();
    }
    void Off() {
      if (!player) return;
      player->set_fade_time(0.2);  // Read from config file?
      player->FadeAndStop();
    }
    void Free() {
      if (!player) return;
      player.Free();
    }
    void Stop() {
      if (!player) return;
      player->Stop();
    }
    bool isOff() {
      if (!player) return true;
      return player->isOff();
    }
    void SetTransition(float mp, float w) {
      midpoint = mp;
      width = w;
    }
    float begin() const { return midpoint - width / 2; }
    float end() const { return midpoint + width / 2; }
    void rotate(float degrees) {
      midpoint += degrees;
    }
    RefPtr<BufferedWavPlayer> player;
    float midpoint = 0.0;
    float width = 0.0;
  };
  Data A;
  Data B;

  uint32_t last_random_ = 0;
  bool on_ = false;;
  bool accent_swings_present = false;
  bool accent_slashes_present = false;
  BoxFilter<Vec3, 3> gyro_filter_;
  uint32_t last_micros_;
  SwingState state_ = SwingState::OFF;;
  Effect *L, *H;
};

#endif
