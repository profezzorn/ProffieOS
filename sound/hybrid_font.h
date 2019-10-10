#ifndef SOUND_HYBRID_FONT_H
#define SOUND_HYBRID_FONT_H
#include "../common/fuse.h"

class IgniterConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(humStart, 100);
    CONFIG_VARIABLE(volHum, 15);
    CONFIG_VARIABLE(volEff, 16);
    CONFIG_VARIABLE(ProffieOSSwingSpeedThreshold, 250.0f);
    CONFIG_VARIABLE(ProffieOSSwingVolumeSharpness, 1.0f);
    CONFIG_VARIABLE(ProffieOSMaxSwingVolume, 3.0f);
    CONFIG_VARIABLE(ProffieOSSwingOverlap, 0.5f);
    CONFIG_VARIABLE(ProffieOSSmoothSwingDucking, 0.0f);
    CONFIG_VARIABLE(ProffieOSSwingLowerThreshold, 200.0f);
    CONFIG_VARIABLE(ProffieOSSlashAccelerationThreshold, 260.0f);
  }
  int humStart;
  int volHum;
  int volEff;
  float ProffieOSSwingSpeedThreshold;
  float ProffieOSSwingVolumeSharpness;
  float ProffieOSMaxSwingVolume;
  float ProffieOSSwingOverlap;
  float ProffieOSSmoothSwingDucking;
  float ProffieOSSwingLowerThreshold;
  float ProffieOSSlashAccelerationThreshold;
};


// Monophonic sound fonts are the most common.
// These fonts are fairly simple, as generally only one sound is
// played at a time. It starts with the "poweron" sound and when
// that runs out, we gaplessly transition to the "hum" sound.
//
// When an effect happens, like "clash", we do a short cross-fade
// to transition to the new sound, then we play that sound until
// it ends and gaplessly transition back to the hum sound.
class HybridFont : public SaberBase {
public:
  HybridFont() : SaberBase(NOLINK) { }
  void Activate() {
    SetupStandardAudio();
    config_.ReadInCurrentDir("config.ini");
    STDOUT.print("Activating ");
    // TODO: Find more reliable way to figure out if it's a monophonic or polyphonic font!!!!
    monophonic_hum_ = poweron.files_found() || poweroff.files_found() || pwroff.files_found();
    guess_monophonic_ = false;
    if (monophonic_hum_) {
      if ((clash.files_found() || blaster.files_found() || swing.files_found())) {
	guess_monophonic_ = true;
	STDOUT.print("monophonic");
      } else {
	guess_monophonic_ = false;
	STDOUT.print("hybrid");
      }
    } else {
      guess_monophonic_ = false;
      STDOUT.print("polyphonic");
    }

    STDOUT.println(" font.");
    SaberBase::Link(this);
    SetHumVolume(1.0);
    state_ = STATE_OFF;
  }

  enum State {
    STATE_OFF,
    STATE_OUT,
    STATE_HUM_FADE_IN,
    STATE_HUM_ON,
    STATE_HUM_FADE_OUT,
  };

  void Deactivate() {
    lock_player_.Free();
    hum_player_.Free();
    next_hum_player_.Free();
    swing_player_.Free();
    SaberBase::Unlink(this);
  }

  RefPtr<BufferedWavPlayer> hum_player_;
  RefPtr<BufferedWavPlayer> next_hum_player_;
  RefPtr<BufferedWavPlayer> swing_player_;
  RefPtr<BufferedWavPlayer> lock_player_;
  void PlayMonophonic(Effect* f, Effect* loop)  {
    EnableAmplifier();
    if (!next_hum_player_) {
      next_hum_player_ = GetFreeWavPlayer();
      if (!next_hum_player_) {
	STDOUT.println("Out of WAV players!");
	return;
      }
    }
    if (hum_player_) {
      hum_player_->set_fade_time(0.003);
      hum_player_->FadeAndStop();
      hum_player_.Free();
      next_hum_player_->set_volume_now(0);
      next_hum_player_->set_fade_time(0.003);
      next_hum_player_->set_volume(config_.volEff / 16.0);
    } else {
      next_hum_player_->set_volume_now(config_.volEff / 16.0);
    }
    hum_player_ = next_hum_player_;
    next_hum_player_.Free();
    hum_player_->PlayOnce(f);
    current_effect_length_ = hum_player_->length();
    if (loop) hum_player_->PlayLoop(loop);
  }
  RefPtr<BufferedWavPlayer> PlayPolyphonic(Effect* f)  {
    EnableAmplifier();
    if (!f->files_found()) return RefPtr<BufferedWavPlayer>(nullptr);
    RefPtr<BufferedWavPlayer> player = GetFreeWavPlayer();
    if (player) {
      player->set_volume_now(config_.volEff / 16.0);
      player->PlayOnce(f);
      current_effect_length_ = player->length();
    }
    return player;
  }

  void Play(Effect* monophonic, Effect* polyphonic) {
    if (polyphonic->files_found()) {
      PlayPolyphonic(polyphonic);
    } else {
      PlayMonophonic(monophonic, &hum);
    }
  }

  void PlayCommon(Effect* effect) {
    if (guess_monophonic_) {
      PlayMonophonic(effect, &hum);
    } else {
      PlayPolyphonic(effect);
    }
  }

  void StartSwing(const Vec3& gyro, float swingThreshold, float slashThreshold) override {
    Vec3 gyro_slope = fusor.gyro_slope();
    //Radians per second per second
    float rss = sqrtf(gyro_slope.z * gyro_slope.z + gyro_slope.y * gyro_slope.y) * (M_PI/180);
    float swing_speed = fusor.swing_speed();
    if (swing_speed > swingThreshold) {
      if (!guess_monophonic_) {
        if (swing_player_) {
          // avoid overlapping swings, based on value set in ProffieOSSwingOverlap.  Value is
          // between 0 (full overlap) and 1.0 (no overlap)
          if (swing_player_->pos() / swing_player_->length() >= config_.ProffieOSSwingOverlap) {
            swing_player_->set_fade_time(swing_player_->length() - swing_player_->pos());
            swing_player_->FadeAndStop();
            swing_player_.Free();
          }
        }
        if (!swing_player_) {
          if (!swinging_) {
            if (rss > slashThreshold && slsh.files_found()) {
              swing_player_ = PlayPolyphonic(&slsh);
            } else {
              swing_player_ = PlayPolyphonic(&swng);
            }
            swinging_ = true;
          }
        }
      } else if (!swinging_ && swing_speed > swingThreshold) {
        PlayMonophonic(&swing, &hum);
        swinging_ = true;
      }
      float swing_strength = std::min<float>(1.0, swing_speed / swingThreshold);
      SetSwingVolume(swing_strength, 1.0);
    } else if (speed <= config_.ProffieOSSwingLowerThreshold) {
      swinging_ = false;
      swing_player_.Free();
    }
    float vol = 1.0f;
    if (!swinging_) {
      vol = vol * (0.99 + clamp(swing_speed/200.0, 0.0, 2.3));
    }
    SetHumVolume(vol);
  }

  float SetSwingVolume(float swing_strength, float mixhum) override {
    if(swing_player_) {
      if (swing_player_->isPlaying()) {
        float accent_volume = powf(swing_strength, config_.ProffieOSSwingVolumeSharpness) * config_.ProffieOSMaxSwingVolume;
        swing_player_->set_fade_time(0.04);
        swing_player_->set_volume(accent_volume);
        mixhum = mixhum - mixhum * (config_.ProffieOSSmoothSwingDucking * accent_volume);
      } else {
        swing_player_.Free();
      }
    }
    // in the off chance this gets reduced below 0, we don't want to pass a negative number
    // to the mixer.
    if (mixhum > 0) {
      return mixhum;
    } else {
      return 0.0;
    }
  }

  void SB_On() override {
    if (monophonic_hum_) {
      state_ = STATE_HUM_ON;
      PlayMonophonic(&poweron, &hum);
    } else {
      state_ = STATE_OUT;
      hum_player_ = GetFreeWavPlayer();
      if (hum_player_) {
	hum_player_->set_volume_now(0);
	hum_player_->PlayOnce(&hum);
	hum_player_->PlayLoop(&hum);
	hum_start_ = millis();
      }
      RefPtr<BufferedWavPlayer> tmp = PlayPolyphonic(&out);
      if (config_.humStart && tmp) {
	int delay_ms = 1000 * tmp->length() - config_.humStart;
	if (delay_ms > 0 && delay_ms < 30000) {
	  hum_start_ += delay_ms;
	}
      }
    }
  }

  void SB_Off(OffType off_type) override {
    switch (off_type) {
      case OFF_NORMAL:
        if (monophonic_hum_) {
          size_t total = poweroff.files_found() + pwroff.files_found();
          if (total) {
            state_ = STATE_OFF;
            if ((rand() % total) < poweroff.files_found()) {
              PlayMonophonic(&poweroff, NULL);
            } else {
              PlayMonophonic(&pwroff, NULL);
            }
          }
        } else {
          state_ = STATE_HUM_FADE_OUT;
          PlayPolyphonic(&in);
        }
        break;
      case OFF_BLAST:
        if (monophonic_hum_) {
	  if (boom) PlayMonophonic(&boom, NULL);
	  else PlayMonophonic(&clash, NULL);  // Thermal-D fallback
        } else {
          state_ = STATE_HUM_FADE_OUT;
	  PlayPolyphonic(&boom);
        }
        break;
    }
  }
  void SB_Clash() override { Play(&clash, &clsh); }
  void SB_Stab() override {
    if (stab.files_found()) {
      PlayCommon(&stab);
    } else {
      // If no stab sounds are found, use a clash sound
      SB_Clash();
    }
  }
  void SB_Force() override { PlayCommon(&force); }
  void SB_Blast() override { Play(&blaster, &blst); }
  void SB_Boot() override { PlayPolyphonic(&boot); }
  void SB_NewFont() override {
    if (!PlayPolyphonic(&font)) {
      beeper.Beep(0.05, 2000.0);
    }
  }
  void SB_Change(SaberBase::ChangeType change) override {
    switch (change) {
      case SaberBase::ENTER_COLOR_CHANGE:
	if (!PlayPolyphonic(&ccbegin)) {
	  beeper.Beep(0.20, 1000.0);
	  beeper.Beep(0.20, 1414.2);
	  beeper.Beep(0.20, 2000.0);
	}
	break;
      case SaberBase::EXIT_COLOR_CHANGE:
	if (!PlayPolyphonic(&ccend)) {
	  beeper.Beep(0.20, 2000.0);
	  beeper.Beep(0.20, 1414.2);
	  beeper.Beep(0.20, 1000.0);
	}
	break;
      case SaberBase::CHANGE_COLOR:
	if (!PlayPolyphonic(&ccchange)) {
	  beeper.Beep(0.05, 2000.0);
	}
	break;
    }
  }

  void SB_BeginLockup() override {
    Effect *once = nullptr;
    Effect *loop = nullptr;
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_ARMED:
	if (bgnarm) once = &bgnarm;
	if (armhum) loop = &armhum;
	if (!armhum && swing) loop = &swing;  // Thermal-D fallback
	break;
      case SaberBase::LOCKUP_DRAG:
	if (bgndrag) once = &bgndrag;
	if (drag) loop = &drag;
	// fall through
      case SaberBase::LOCKUP_NORMAL:
	if (!once && bgnlock) once = &bgnlock;
	// fall through
      case SaberBase::LOCKUP_NONE:
	break;
    }

    if (lockup.files_found() > 0) {
      // Monophonic
      if (!loop) loop = &lockup;
      if (!once) once = loop;
      PlayMonophonic(once, loop);
    } else {
      // Polyphonic
      if (!loop) loop = &lock;
      if (!once) once = loop;
      if (!lock_player_) {
	lock_player_ = PlayPolyphonic(once);
	if (lock_player_) lock_player_->PlayLoop(loop);
      }
    }
    if (once == loop) current_effect_length_ = 0;
  }

  void SB_EndLockup() override {
    Effect *end = nullptr;
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_ARMED:
	end = &endarm;
	break;
      case SaberBase::LOCKUP_DRAG:
	if (enddrag) end = &enddrag;
	// fall through
      case SaberBase::LOCKUP_NORMAL:
	if (!end && endlock) end = &endlock;
	if (!end) end = &clash;
	// fall through
      case SaberBase::LOCKUP_NONE:
	break;
    }

    current_effect_length_ = 0;
    if (lock_player_) {
      // Polyphonic case
      lock_player_->set_fade_time(0.3);
      if (end) { // polyphonic end lock
        if (PlayPolyphonic(end)) {
          // if playing an end lock fade the lockup faster
          lock_player_->set_fade_time(0.003);
	}
      }

      lock_player_->FadeAndStop();
      lock_player_.Free();
      return;
    }
    // Monophonic case
    if (lockup.files_found()) {
      PlayMonophonic(end, &hum);
    }
  }

  void SetHumVolume(float vol) override {
    if (!monophonic_hum_) {
      if (state_ != STATE_OFF && !hum_player_) {
	hum_player_ = GetFreeWavPlayer();
	if (hum_player_) {
	  hum_player_->set_volume_now(0);
	  hum_player_->PlayOnce(&hum);
	  hum_player_->PlayLoop(&hum);
	  hum_start_ = millis();
	}
      }
      if (!hum_player_) return;
      uint32_t m = micros();
      switch (state_) {
	case STATE_OFF:
	  volume_ = 0.0f;
	  return;
	case STATE_OUT:
	  volume_ = 0.0f;
	  if (millis() - hum_start_ < 0x7fffffffUL) {
	    state_ = STATE_HUM_FADE_IN;
	  }
	  break;
	case STATE_HUM_FADE_IN: {
	  uint32_t delta = m - last_micros_;
	  volume_ += (delta / 1000000.0) / 0.2; // 0.2 seconds
	  if (volume_ >= 1.0f) {
	    volume_ = 1.0f;
	    state_ = STATE_HUM_ON;
	  }
	  break;
	}
	case STATE_HUM_ON:
	  break;
	case STATE_HUM_FADE_OUT: {
	  SaberBase::RequestMotion();
	  uint32_t delta = m - last_micros_;
	  volume_ -= (delta / 1000000.0) / 0.2; // 0.2 seconds
	  if (volume_ <= 0.0f) {
	    volume_ = 0.0f;
	    state_ = STATE_OFF;
	    hum_player_->FadeAndStop();
	    hum_player_.Free();
	  }
	  break;
	}
      }
      last_micros_ = m;
      vol *= volume_;
    }
    if (!hum_player_) return;
    hum_player_->set_volume(vol);
  }

  bool swinging_ = false;
  void SB_Motion(const Vec3& gyro, bool clear) override {
    if (state_ != STATE_OFF &&
        !(lockup.files_found() && SaberBase::Lockup())) {
      StartSwing(gyro, config_.ProffieOSSwingSpeedThreshold, config_.ProffieOSSlashAccelerationThreshold);
    }
  }

  float GetCurrentEffectLength() const {
    return current_effect_length_;
  }

 private:
  uint32_t last_micros_;
  uint32_t hum_start_;
  bool monophonic_hum_;
  bool guess_monophonic_;
  IgniterConfigFile config_;
  State state_;
  float volume_;
  float current_effect_length_ = 0.0;
};

#endif
