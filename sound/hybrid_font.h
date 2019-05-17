#ifndef SOUND_HYBRID_FONT_H
#define SOUND_HYBRID_FONT_H

class IgniterConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(humStart, 100);
    CONFIG_VARIABLE(volHum, 15);
    CONFIG_VARIABLE(volEff, 16);
    CONFIG_VARIABLE(ProffieOSSwingSpeedThreshold, 250.0f);
    CONFIG_VARIABLE(ProffieOSSwingVolumeSharpness, 0.5f);
    CONFIG_VARIABLE(ProffieOSMaxSwingVolume, 3.0f);
    CONFIG_VARIABLE(ProffieOSSwingOverlap, 0.6f);
    CONFIG_VARIABLE(ProffieOSSmoothSwingDucking, 0.25f);
  }
  int humStart;
  int volHum;
  int volEff;
  float ProffieOSSwingSpeedThreshold;
  float ProffieOSSwingVolumeSharpness;
  float ProffieOSMaxSwingVolume;
  float ProffieOSSwingOverlap;
  float ProffieOSSmoothSwingDucking;
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
    if (loop) hum_player_->PlayLoop(loop);
  }
							   
  RefPtr<BufferedWavPlayer> PlayPolyphonic(Effect* f)  {
    EnableAmplifier();
    RefPtr<BufferedWavPlayer> player = GetFreeWavPlayer();
    if (player) {
      player->set_volume_now(config_.volEff / 16.0);
      player->PlayOnce(f);
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
  
  void StartSwing() override {
    if (!guess_monophonic_) {
      if (swing_player_) {
        // avoid overlapping swings, based on value set in ProffieOSSwingOverlap.  Value is
        // between 0 (no overlap) and 1.0 (full overlap)
        if (swing_player_->pos() / swing_player_->length() >= config_.ProffieOSSwingOverlap) {
          swing_player_.Free();
          swing_player_ = PlayPolyphonic(&swng);
        }
      }
      else if (!swing_player_) {
        swing_player_ = PlayPolyphonic(&swng);
        swing_player_->set_volume_now(0);
      }
    } else {
      PlayMonophonic(&swing, &hum);
    }
  }

  float SetSwingVolume(float swing_strength, float mixhum) override {
    if(swing_player_) {
      if (swing_player_->isPlaying()) {
        float accent_volume = powf(swing_strength, config_.ProffieOSSwingVolumeSharpness) * config_.ProffieOSMaxSwingVolume;
        swing_player_->set_volume(accent_volume);
        mixhum = mixhum - mixhum * (config_.ProffieOSSmoothSwingDucking * accent_volume);
      }
      else {
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

  void SB_Off() override {
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
  }
  void SB_Clash() override { Play(&clash, &clsh); }
  void SB_Stab() override { PlayCommon(&stab); }
  void SB_Force() override { PlayCommon(&force); }
  void SB_Blast() override { Play(&blaster, &blst); }
  void SB_Boot() override { PlayPolyphonic(&boot); }
  void SB_NewFont() override { PlayPolyphonic(&font); }

  void SB_BeginLockup() override {
    if (lockup.files_found()) {
      if (SaberBase::Lockup() == SaberBase::LOCKUP_DRAG &&
	  drag.files_found()) {
	PlayMonophonic(&drag, &drag);
      } else if (lockup.files_found()) {
        if (bgnlock.files_found()) {
          PlayMonophonic(&bgnlock, &lockup);
        } else {
          PlayMonophonic(&lockup, &lockup);
	}
      }
    } else {
      Effect* e = &lock;
      if (SaberBase::Lockup() == SaberBase::LOCKUP_DRAG &&
	  drag.files_found()) {
	e = &drag;
      }
      if (!lock_player_) {
        if (bgnlock.files_found()) {
          lock_player_ = PlayPolyphonic(&bgnlock);
        } else {
          lock_player_ = PlayPolyphonic(e);
        }

	if (lock_player_) {
	  lock_player_->PlayLoop(e);
	}
      }
    }
  }

  void SB_EndLockup() override {
    if (lock_player_) {
      // Polyphonic case
      lock_player_->set_fade_time(0.3);

      if (endlock.files_found()) { // polyphonic end lock
        if (PlayPolyphonic(&endlock)) {
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
      if (endlock.files_found()) { // Plecter font endlock support
        PlayMonophonic(&endlock, &hum);
      } else {
        PlayMonophonic(&clash, &hum);
      }
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
    float speed = sqrtf(gyro.z * gyro.z + gyro.y * gyro.y);
    if (speed > config_.ProffieOSSwingSpeedThreshold) {
      if (!swinging_ && state_ != STATE_OFF &&
	  !(lockup.files_found() && SaberBase::Lockup())) {
        swinging_ = true;
        StartSwing();
      }
      float swing_strength = std::min<float>(1.0, speed / config_.ProffieOSSwingSpeedThreshold);
      SetSwingVolume(swing_strength, 1.0);
    } else if (swinging_ && speed <= config_.ProffieOSSwingSpeedThreshold * 0.8) {
      swinging_ = false;
    }
    float vol = 1.0f;
    if (!swinging_) {
      vol = vol * (0.99 + clamp(speed/200.0, 0.0, 2.3));
    }
    SetHumVolume(vol);
  }

 private:
  uint32_t last_micros_;
  uint32_t hum_start_;
  bool monophonic_hum_;
  bool guess_monophonic_;
  IgniterConfigFile config_;
  State state_;
  float volume_;
};

#endif
