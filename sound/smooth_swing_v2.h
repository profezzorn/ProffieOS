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
    SetDelegate(base_font);
    if (swingl.files_found() != swingh.files_found()) {
      STDOUT.println("Warning, swingl and swingh should have the same number of files.");
    }
    swings_ = std::min<size_t>(swingl.files_found(), swingh.files_found());
    //check for swngxx files to use as accent swings
    if (swng.files_found() > 0) {
      STDOUT.print("Accent Swings Detected: ");
      STDOUT.println(swng.files_found());
      accent_swings_present = true;
    }
    else
    {
      accent_swings_present = false;
      STDOUT.print("Accent Swings NOT Detected: ");
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
    // No point in picking a new random so soon after picking one.
    if (A.player && m - last_random_ < 1000) return;
    last_random_ = m;
    float start = m / 1000.0;
    A.Stop();
    B.Stop();
    swingl.Select(swing);
    swingh.Select(swing);
    A.Play(&swingl, start);
    B.Play(&swingh, start);
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
    if (!A.player || !B.player ) {
      STDOUT.println("SmoothSwing V2 cannot allocate wav player.");
    }
  }
  void SB_Off() override {
    on_ = false;
    A.Off();
    B.Off();
    delegate_->SB_Off();
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
        //check for AccentSwingThreshold, presence of accent swings and if the accent player is stopped (this prevents clipping)
        if (speed >=smooth_swing_config.AccentSwingSpeedThreshold && accent_swings_present && !accent_player_->isPlaying() && (A.isPlaying() || B.isPlaying()))
        {
          //allocate player
          if(!accent_player_) {
            accent_player_ = GetFreeWavPlayer();
          }
          else {
          accent_player_->PlayOnce(&swng);
          //select new random swng accent
          swng.Select(accentswing);
          }
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
          
          float accent_volume =
          powf(swing_strength, smooth_swing_config.AccentSwingVolumeSharpness);
          
          hum_volume =
          1.0 - mixhum * smooth_swing_config.MaximumHumDucking / 100.0;
          
          mixhum *= smooth_swing_config.MaxSwingVolume;
          accent_volume *= smooth_swing_config.MaxAccentSwingVolume;
          
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
            STDOUT.print(hum_volume);
            STDOUT.print("  accent_volume: ");
            STDOUT.println(accent_volume);
          }
          if (on_) {
            // We need to stop setting the volume when off, or playback may never stop.
            A.set_volume(mixhum * mixab);
            B.set_volume(mixhum * (1.0 - mixab));
            //This volume will scale with swing speed but is modulated by AccentSwingVolumeSharpness.
            accent_player_->set_volume(accent_volume);
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
      player->set_volume(0.0);
      player->PlayOnce(effect, start);
      player->PlayLoop(effect);
    }
    void PlayAccent(Effect* effect){
      if (!player) {
        player = GetFreeWavPlayer();
        if (!player) return;
      }
      player->PlayOnce(effect);
    }
    bool isPlaying()
    {
      if (!player) return false;
      else
      {
        return player->isPlaying();
      }
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
  RefPtr<BufferedWavPlayer> accent_player_;
  uint32_t last_random_ = 0;
  bool on_ = false;;
  BoxFilter<Vec3, 3> gyro_filter_;
  int swings_;
  bool accent_swings_present = false;
  uint32_t last_micros_;
  SwingState state_ = SwingState::OFF;;
};

#endif
