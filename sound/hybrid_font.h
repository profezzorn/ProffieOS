#ifndef SOUND_HYBRID_FONT_H
#define SOUND_HYBRID_FONT_H
#include "../common/fuse.h"

class FontConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(humStart, 100);
    CONFIG_VARIABLE(volHum, 15);
    CONFIG_VARIABLE(volEff, 16);
    CONFIG_VARIABLE(ProffieOSSwingSpeedThreshold, 250.0f);
    CONFIG_VARIABLE(ProffieOSSwingVolumeSharpness, 0.5f);
    CONFIG_VARIABLE(ProffieOSMaxSwingVolume, 2.0f);
    CONFIG_VARIABLE(ProffieOSSwingOverlap, 0.5f);
    CONFIG_VARIABLE(ProffieOSSmoothSwingDucking, 0.2f);
    CONFIG_VARIABLE(ProffieOSSwingLowerThreshold, 200.0f);
    CONFIG_VARIABLE(ProffieOSSlashAccelerationThreshold, 130.0f);
    CONFIG_VARIABLE(ProffieOSAnimationFrameRate, 0.0f);
    CONFIG_VARIABLE(ProffieOSFontImageDuration, 5000.0f);
    CONFIG_VARIABLE(ProffieOSOnImageDuration, 5000.0f);
    CONFIG_VARIABLE(ProffieOSBlastImageDuration, 1000.0f);
    CONFIG_VARIABLE(ProffieOSClashImageDuration, 500.0f);
    CONFIG_VARIABLE(ProffieOSForceImageDuration, 1000.0f);
#ifdef ENABLE_SPINS
    CONFIG_VARIABLE(ProffieOSSpinDegrees, 360.0f);
#endif
  }
  // Igniter compat
  // This specifies how many milliseconds before the end of the
  // "out" sound the hum starts to fade in.
  // Defaults to 100ms.
  int humStart;
  // Hum Volume (0-16), defaults to 15.
  int volHum;
  // Effect volume (0-16) defaults to 16.
  int volEff;

  // How fast (degrees per second) we have to swing before a swing
  // effect is triggered. Defaults to 250.
  float ProffieOSSwingSpeedThreshold;
  // Bends the response curve between swing speed and swing volume.
  // Defaults to 0.5
  float ProffieOSSwingVolumeSharpness;
  // The volume when swings are at the swing speed threshold.
  // Defaults to 2.0
  float ProffieOSMaxSwingVolume;
  // Specify what fraction of swing that must be played before a
  // new swing can be started. Defaults to 0.5 (50%)
  float ProffieOSSwingOverlap;
  // How much to duck the hum when the swing is playing.
  // Defauls to 0.2 (hum volume is decreased by 20% of swing volume)
  float ProffieOSSmoothSwingDucking;
  // How slow (degrees per second) the swing has to be before it's
  // not considered a swing anymore. Defaults to 200.
  float ProffieOSSwingLowerThreshold;
  // Specifies how agressive a swing has to be to be considered
  // a slash. Once we reach, ProffieOSSwingSpeedThreshold, rate of
  // swing speed change is used to determine if it's a swing or a
  // slash. Defaults to 130 (degrees per second per second)
  float ProffieOSSlashAccelerationThreshold;
  // For OLED displays, this specifies the frame rate of animations.
  float ProffieOSAnimationFrameRate;
  // for OLED displays, the time a static BMP or loop will play when saber is off
  float ProffieOSFontImageDuration;
  // for OLED displays, the time an on.bmp will play
  float ProffieOSOnImageDuration;
  // for OLED displays, the time a blst.bmp will play
  float ProffieOSBlastImageDuration;
  // for OLED displays, the time a clsh.bmp will play
  float ProffieOSClashImageDuration;
  // for OLED displays, the time a force.bmp will play
  float ProffieOSForceImageDuration;
#ifdef ENABLE_SPINS
  // number of degrees the blade must travel while staying above the
  // swing threshold in order to trigger a spin sound.  Default is 360 or
  // one full rotation.
  float ProffieOSSpinDegrees;
#endif
};

FontConfigFile font_config;

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
    font_config.ReadInCurrentDir("config.ini");
    STDOUT.print("Activating ");
    // TODO: Find more reliable way to figure out if it's a monophonic or polyphonic font!!!!
    if (SFX_in || SFX_out) {
      monophonic_hum_ = false;
    } else {
      monophonic_hum_ = SFX_poweron || SFX_poweroff || SFX_pwroff || SFX_blast;
    }
    guess_monophonic_ = false;
    if (monophonic_hum_) {
      if (SFX_clash || SFX_blaster || SFX_swing) {
        if (SFX_humm) {
	  monophonic_hum_ = false;
          guess_monophonic_ = false;
          STDOUT.print("plecter polyphonic");
        } else {
          guess_monophonic_ = true;
          STDOUT.print("monophonic");
        }
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
    state_ = STATE_OFF;
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
      next_hum_player_->set_volume(font_config.volEff / 16.0f);
    } else {
      next_hum_player_->set_volume_now(font_config.volEff / 16.0f);
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
      player->set_volume_now(font_config.volEff / 16.0f);
      player->PlayOnce(f);
      current_effect_length_ = player->length();
    }
    return player;
  }

  void Play(Effect* monophonic, Effect* polyphonic) {
    if (polyphonic->files_found()) {
      PlayPolyphonic(polyphonic);
    } else if (SFX_humm) {
      PlayPolyphonic(monophonic);
    } else {
      PlayMonophonic(monophonic, &SFX_hum);
    }
  }

  void PlayCommon(Effect* effect) {
    if (guess_monophonic_) {
      PlayMonophonic(effect, &SFX_hum);
    } else {
      PlayPolyphonic(effect);
    }
  }

  void StartSwing(const Vec3& gyro, float swingThreshold, float slashThreshold) override {
    Vec3 gyro_slope = fusor.gyro_slope();
    // Radians per second per second
    float rss = sqrtf(gyro_slope.z * gyro_slope.z + gyro_slope.y * gyro_slope.y) * (M_PI / 180);
    float swing_speed = fusor.swing_speed();
    uint32_t now = micros();
    uint32_t delta_micros = now - last_swing_micros_;
    last_swing_micros_ = now;
    if (delta_micros > 1000000) delta_micros = 1;
    if (swing_speed > swingThreshold) {
#ifdef ENABLE_SPINS
      float delta = delta_micros * 0.000001;
      angle_ += swing_speed * delta;
#endif
      if (!guess_monophonic_) {
        if (swing_player_) {
          // avoid overlapping swings, based on value set in ProffieOSSwingOverlap.  Value is
          // between 0 (full overlap) and 1.0 (no overlap)
          if (swing_player_->pos() / swing_player_->length() >= font_config.ProffieOSSwingOverlap) {
            swing_player_->set_fade_time(swing_player_->length() - swing_player_->pos());
            swing_player_->FadeAndStop();
            swing_player_.Free();
          }
        }
        if (!swing_player_) {
          if (!swinging_) {
            if (rss > slashThreshold && SFX_slsh) {
              swing_player_ = PlayPolyphonic(&SFX_slsh);
            } else if (SFX_swng) {
              swing_player_ = PlayPolyphonic(&SFX_swng);
            } else {
              swing_player_ = PlayPolyphonic(&SFX_swing);
            }
            swinging_ = true;
          } else {
#ifdef ENABLE_SPINS
            if (angle_ > font_config.ProffieOSSpinDegrees) {
              if (SFX_spin) {
                swing_player_ = PlayPolyphonic(&SFX_spin);
              }
              angle_ -= font_config.ProffieOSSpinDegrees;
            }
#endif
          }
        }
      } else if (swing_speed > swingThreshold) {
        if (!swinging_) {
          PlayMonophonic(&SFX_swing, &SFX_hum);
          swinging_ = true;
        }
#ifdef ENABLE_SPINS
        if (angle_ > 360 && swinging_) {
          if (SFX_spin) {
            PlayMonophonic(&SFX_spin, &SFX_hum);
          }
          angle_ -= font_config.ProffieOSSpinDegrees;
        }
#endif
      }
      float swing_strength = std::min<float>(1.0, swing_speed / swingThreshold);
      SetSwingVolume(swing_strength, 1.0);
    } else if (swing_speed <= font_config.ProffieOSSwingLowerThreshold) {
      swinging_ = false;
      swing_player_.Free();
#ifdef ENABLE_SPINS
      angle_ = 0;
#endif
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
        float accent_volume = powf(
          swing_strength, font_config.ProffieOSSwingVolumeSharpness) * font_config.ProffieOSMaxSwingVolume;
        swing_player_->set_fade_time(0.04);
        swing_player_->set_volume(accent_volume);
        mixhum = mixhum - mixhum * (font_config.ProffieOSSmoothSwingDucking * accent_volume);
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

  void SB_PreOn(float* delay) override {
    if (SFX_preon) {
      RefPtr<BufferedWavPlayer> tmp = PlayPolyphonic(&SFX_preon);
      if (tmp) {
        *delay = std::max(*delay, tmp->length());
      }
    }
  }

  void SB_On() override {
    if (monophonic_hum_) {
      state_ = STATE_HUM_ON;
      PlayMonophonic(&SFX_poweron, &SFX_hum);
    } else {
      state_ = STATE_OUT;
      hum_player_ = GetFreeWavPlayer();
      if (hum_player_) {
        hum_player_->set_volume_now(0);
        hum_player_->PlayOnce(SFX_humm ? &SFX_humm : &SFX_hum);
        hum_player_->PlayLoop(SFX_humm ? &SFX_humm : &SFX_hum);
        hum_start_ = millis();
      }
      RefPtr<BufferedWavPlayer> tmp = PlayPolyphonic(SFX_out ? &SFX_out : &SFX_poweron);
      hum_fade_in_ = 0.2;
      if (SFX_humm && tmp) {
	hum_fade_in_ = tmp->length();
	STDOUT << "HUM fade-in time: " << hum_fade_in_ << "\n";
      }
      else if (font_config.humStart && tmp) {
        int delay_ms = 1000 * tmp->length() - font_config.humStart;
        if (delay_ms > 0 && delay_ms < 30000) {
          hum_start_ += delay_ms;
        }
	STDOUT << "humstart: " << font_config.humStart << "\n";
      }
    }
  }

  void SB_Off(OffType off_type) override {
    switch (off_type) {
      case OFF_IDLE:
        break;
      case OFF_NORMAL:
        if (!SFX_in) {
          size_t total = SFX_poweroff.files_found() + SFX_pwroff.files_found();
          if (total) {
            if ((rand() % total) < SFX_poweroff.files_found()) {
              PlayMonophonic(&SFX_poweroff, NULL);
            } else {
              PlayMonophonic(&SFX_pwroff, NULL);
            }
	    hum_fade_out_ = current_effect_length_;
          } else if (monophonic_hum_) {
            // No poweroff, just fade out...
            hum_player_->set_fade_time(0.2);
            hum_player_->FadeAndStop();
            hum_player_.Free();
          }
        } else {
          state_ = STATE_HUM_FADE_OUT;
          PlayPolyphonic(&SFX_in);
	  hum_fade_out_ = 0.2;
        }
	state_ = monophonic_hum_ ? STATE_OFF : STATE_HUM_FADE_OUT;
        break;
      case OFF_BLAST:
        if (monophonic_hum_) {
          if (SFX_boom) PlayMonophonic(&SFX_boom, NULL);
          else PlayMonophonic(&SFX_clash, NULL);  // Thermal-D fallback
        } else {
          state_ = STATE_HUM_FADE_OUT;
          PlayPolyphonic(&SFX_boom);
        }
        break;
    }
  }

  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      default: return;
      case EFFECT_STAB:
	if (SFX_stab) { PlayCommon(&SFX_stab); return; }
	// If no stab sounds are found, fall through to clash
      case EFFECT_CLASH: Play(&SFX_clash, &SFX_clsh); return;
      case EFFECT_FORCE: PlayCommon(&SFX_force); return;
      case EFFECT_BLAST: Play(&SFX_blaster, &SFX_blst); return;
      case EFFECT_BOOT: PlayPolyphonic(&SFX_boot); return;
      case EFFECT_NEWFONT: SB_NewFont(); return;
      case EFFECT_LOCKUP_BEGIN: SB_BeginLockup(); return;
      case EFFECT_LOCKUP_END: SB_EndLockup(); return;
      case EFFECT_LOW_BATTERY: SB_LowBatt(); return;
    }
  }

  void SB_BladeDetect(bool detected) {
    Effect &X(detected ? SFX_bladein : SFX_bladeout);
    if (X) {
      PlayPolyphonic(&X);
      return;
    }
    if (detected && SFX_boot) {
      PlayPolyphonic(&SFX_boot);
      return;
    }
    beeper.Beep(0.05, 2000.0);
  }
  void SB_NewFont() {
    if (!PlayPolyphonic(&SFX_font)) {
      beeper.Beep(0.05, 2000.0);
    }
  }
  void SB_Change(SaberBase::ChangeType change) override {
    switch (change) {
      case SaberBase::ENTER_COLOR_CHANGE:
        if (!PlayPolyphonic(&SFX_ccbegin) && !PlayPolyphonic(&SFX_color)) {
          beeper.Beep(0.20, 1000.0);
          beeper.Beep(0.20, 1414.2);
          beeper.Beep(0.20, 2000.0);
        }
        break;
      case SaberBase::EXIT_COLOR_CHANGE:
        if (!PlayPolyphonic(&SFX_ccend)) {
          beeper.Beep(0.20, 2000.0);
          beeper.Beep(0.20, 1414.2);
          beeper.Beep(0.20, 1000.0);
        }
        break;
      case SaberBase::CHANGE_COLOR:
        if (!PlayPolyphonic(&SFX_ccchange)) {
          beeper.Beep(0.05, 2000.0);
        }
        break;
    }
  }

  void SB_BeginLockup() {
    Effect *once = nullptr;
    Effect *loop = nullptr;
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_ARMED:
        if (SFX_bgnarm) once = &SFX_bgnarm;
        if (SFX_armhum) loop = &SFX_armhum;
        if (!SFX_armhum && SFX_swing) loop = &SFX_swing;  // Thermal-D fallback
        break;
      case SaberBase::LOCKUP_AUTOFIRE:
	if (SFX_bgnauto) once = &SFX_bgnauto;
	if (SFX_auto) loop = &SFX_auto;
	break;
      case SaberBase::LOCKUP_LIGHTNING_BLOCK:
	if (SFX_bgnlb) once = &SFX_bgnlb;
	if (SFX_lb) loop = &SFX_lb;
	goto normal_fallback;
      case SaberBase::LOCKUP_MELT:
	if (SFX_bgnmelt) once = &SFX_bgnmelt;
	if (SFX_melt) loop = &SFX_melt;
        // fall through
      case SaberBase::LOCKUP_DRAG:
        if (!once && SFX_bgndrag) once = &SFX_bgndrag;
        if (!loop && SFX_drag) loop = &SFX_drag;
        // fall through
      case SaberBase::LOCKUP_NORMAL:
	normal_fallback:
        if (!once && SFX_bgnlock) once = &SFX_bgnlock;
        // fall through
      case SaberBase::LOCKUP_NONE:
        break;
    }

    if (!loop) loop = SFX_lockup ? &SFX_lockup : &SFX_lock;
    if (!once) once = loop;

    if (SFX_lockup && !SFX_humm) {
      // Monophonic
      PlayMonophonic(once, loop);
    } else {
      // Polyphonic
      if (!lock_player_) {
        lock_player_ = PlayPolyphonic(once);
        if (lock_player_) lock_player_->PlayLoop(loop);
      }
    }
    if (once == loop) current_effect_length_ = 0;
  }

  void SB_EndLockup() {
    Effect *end = nullptr;
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_ARMED:
        end = &SFX_endarm;
        break;
      case SaberBase::LOCKUP_AUTOFIRE:
        if (SFX_endauto) end = &SFX_endauto; // if we have a transition use it
        if (!end) end = &SFX_blast; // if we don't, end with a blast
        break;
      case SaberBase::LOCKUP_LIGHTNING_BLOCK:
	if (SFX_endlb) end = &SFX_endlb;
	goto normal_fallback_end;
      case SaberBase::LOCKUP_MELT:
	if (SFX_endmelt) end = &SFX_endmelt;
        // fall through
      case SaberBase::LOCKUP_DRAG:
        if (!end && SFX_enddrag) end = &SFX_enddrag;
        // fall through
      case SaberBase::LOCKUP_NORMAL:
	normal_fallback_end:
        if (!end && SFX_endlock) end = &SFX_endlock;
        if (!end) end = &SFX_clash;
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
    if (SFX_lockup) {
      PlayMonophonic(end, &SFX_hum);
    }
  }

  void SetHumVolume(float vol) override {
    if (!monophonic_hum_) {
      if (state_ != STATE_OFF && !hum_player_) {
        hum_player_ = GetFreeWavPlayer();
        if (hum_player_) {
          hum_player_->set_volume_now(0);
          hum_player_->PlayOnce(SFX_humm ? &SFX_humm : &SFX_hum);
          hum_player_->PlayLoop(SFX_humm ? &SFX_humm : &SFX_hum);
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
          volume_ += (delta / 1000000.0) / hum_fade_in_; // 0.2 seconds
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
          volume_ -= (delta / 1000000.0) / hum_fade_out_; // 0.2 seconds
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
    vol *= font_config.volHum / 16.0;
    hum_player_->set_volume(vol);
  }

  bool swinging_ = false;
  void SB_Motion(const Vec3& gyro, bool clear) override {
    if (state_ != STATE_OFF && !(SFX_lockup && SaberBase::Lockup())) {
      StartSwing(gyro,
                 font_config.ProffieOSSwingSpeedThreshold,
                 font_config.ProffieOSSlashAccelerationThreshold);
    }
  }

  float GetCurrentEffectLength() const {
    return current_effect_length_;
  }

  void SB_LowBatt() {
    // play the fonts low battery sound if it exists
    if (SFX_lowbatt) {
      PlayCommon(&SFX_lowbatt);
    } else {
#ifdef ENABLE_AUDIO
      talkie.Say(talkie_low_battery_15, 15);
#endif
    }
  }

 private:
  uint32_t last_micros_;
  uint32_t last_swing_micros_;
  uint32_t hum_start_;
  float hum_fade_in_;
  float hum_fade_out_;
#ifdef ENABLE_SPINS
  float angle_;
#endif
  bool monophonic_hum_;
  bool guess_monophonic_;
  State state_;
  float volume_;
  float current_effect_length_ = 0.0;

};

#endif
