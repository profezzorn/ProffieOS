#ifndef SOUND_HYBRID_FONT_H
#define SOUND_HYBRID_FONT_H
#include "../common/fuse.h"

class FontConfigFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(humStart, 100);
    CONFIG_VARIABLE2(volHum, 15);
    CONFIG_VARIABLE2(volEff, 16);
    CONFIG_VARIABLE2(ProffieOSHumDelay, -1.0f);
    CONFIG_VARIABLE2(ProffieOSSwingSpeedThreshold, 250.0f);
    CONFIG_VARIABLE2(ProffieOSSwingVolumeSharpness, 0.5f);
    CONFIG_VARIABLE2(ProffieOSMaxSwingVolume, 2.0f);
    CONFIG_VARIABLE2(ProffieOSSwingOverlap, 0.5f);
    CONFIG_VARIABLE2(ProffieOSSmoothSwingDucking, 0.2f);
    CONFIG_VARIABLE2(ProffieOSSwingLowerThreshold, 200.0f);
    CONFIG_VARIABLE2(ProffieOSSlashAccelerationThreshold, 130.0f);

    CONFIG_VARIABLE2(ProffieOSMinSwingAcceleration, 0.0f);
    CONFIG_VARIABLE2(ProffieOSMaxSwingAcceleration, 0.0f);
#ifdef ENABLE_SPINS
    CONFIG_VARIABLE2(ProffieOSSpinDegrees, 360.0f);
#endif
    CONFIG_VARIABLE2(ProffieOSSmoothSwingHumstart, 0);

    for (Effect* e = all_effects; e; e = e->next_) {
      char name[32];
      strcpy(name, "ProffieOS.");
      switch (e->GetFileType()) {
        case Effect::FileType::SOUND:
          strcat(name, "SFX.");
          break;
        case Effect::FileType::IMAGE:
          strcat(name, "IMG.");
          break;
        default:
          continue;
      }
      strcat(name, e->GetName());
      strcat(name, ".");
      char* x = name + strlen(name);

      struct PairedVariable : public VariableBase {
        Effect* e_;
        PairedVariable(Effect* e) : e_(e) {}
        void set(float value) override { e_->SetPaired(value > 0.5); }
        float get() override { return e_->GetPaired(); }
        void setDefault() override { e_->SetPaired(false);  }
      };
      
      strcpy(x, "paired");
      PairedVariable var1(e);
      op->run(name, &var1);

      struct VolumeVariable : public VariableBase {
        Effect* e_;
        VolumeVariable(Effect* e) : e_(e) {}
        void set(float value) override { e_->SetVolume(value); }
        float get() override { return e_->GetVolume(); }
        void setDefault() override { e_->SetVolume(100);  }
      };
      
      strcpy(x, "volume");
      VolumeVariable var2(e);
      op->run(name, &var2);
    }
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
  // Milliseconds from beginning of out.wav to delay hum.
  // If not specified or set to -1, humStart will be used.
  // Defaults to -1.
  int ProffieOSHumDelay;
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
  // Minimum acceleration for Accent Swing file Selection
  // recommended value is 20.0 ~ 30.0
  float ProffieOSMinSwingAcceleration;
  // Maximum acceleration for Accent Swing file Selection
  // must be higher than Min value to enable selection
  // recommended value is 100.0 ~ 150.0
  float ProffieOSMaxSwingAcceleration;
#ifdef ENABLE_SPINS
  // number of degrees the blade must travel while staying above the
  // swing threshold in order to trigger a spin sound.  Default is 360 or
  // one full rotation.
  float ProffieOSSpinDegrees;
#endif
  // Make smoothswings start in sync with hum.
  // Set to 1 to sync, or 0 to resume swings where last pair left off.
  bool ProffieOSSmoothSwingHumstart;
};

FontConfigFile font_config;

// Monophonic sound fonts were once the most common.
// These fonts are fairly simple, as generally only one sound is
// played at a time. It starts with the "poweron" sound and when
// that runs out, we gaplessly transition to the "hum" sound.
//
// When an effect happens, like "clash", we do a short cross-fade
// to transition to the new sound, then we play that sound until
// it ends and gaplessly transition back to the hum sound.
class HybridFont : public SaberBase, public Looper {
public:
  const char* name() override { return "Hybrid Font"; }
  HybridFont() : SaberBase(NOLINK), Looper(NOLINK) { }
  void Activate1() {
    SetupStandardAudio();
    guess_monophonic_ = false;
    monophonic_hum_ = false;
    state_ = STATE_OFF;
    SaberBase::Link(this);
  }

  // Do font scan between Activate1 and Activate2
  
  void Activate2() {
    font_config.ReadInCurrentDir("config.ini");
    STDOUT.print("Activating ");
    // TODO: Find more reliable way to figure out if it's a monophonic or polyphonic font!!!!
    if (SFX_in || SFX_out) {
      monophonic_hum_ = false;
    } else {
      monophonic_hum_ = SFX_poweron || SFX_poweroff || SFX_pwroff || SFX_blast;
    }
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
    Looper::Link();
    SetHumVolume(1.0);
  }

  enum State {
    STATE_OFF,
    STATE_WAIT_FOR_ON,
    STATE_OUT,
    STATE_HUM_FADE_IN,
    STATE_HUM_ON,
    STATE_HUM_FADE_OUT,
  };

  bool active_state() {
    switch (state_) {
    case STATE_OFF:
    case STATE_WAIT_FOR_ON:
      return false;
    default:
      break;
    }
    return true;
  }

  void Deactivate() {
    lock_player_.Free();
    hum_player_.Free();
    next_hum_player_.Free();
    swing_player_.Free();
    SaberBase::Unlink(this);
    Looper::Unlink();
    state_ = STATE_OFF;
  }

  RefPtr<BufferedWavPlayer> hum_player_;
  RefPtr<BufferedWavPlayer> next_hum_player_;
  RefPtr<BufferedWavPlayer> swing_player_;
  RefPtr<BufferedWavPlayer> lock_player_;

  void PlayMonophonic(const Effect::FileID& f, Effect* loop, float xfade = 0.003f)  {
    EnableAmplifier();
    if (!next_hum_player_) {
      next_hum_player_ = GetFreeWavPlayer();
      if (!next_hum_player_) {
        STDOUT.println("Out of WAV players!");
        return;
      }
    }
    if (hum_player_) {
      hum_player_->set_fade_time(xfade);
      hum_player_->FadeAndStop();
      hum_player_.Free();
      next_hum_player_->set_volume_now(0);
      next_hum_player_->set_fade_time(xfade);
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

  void PlayMonophonic(Effect* f, Effect* loop, float xfade = 0.003f)  {
    PlayMonophonic(f->RandomFile(), loop, xfade);
  }

  // Use after changing alternative.
  void RestartHum(int previous_alternative) {
    if (hum_player_ && hum_player_->isPlaying()) {
      if (SFX_chhum) {
        SFX_chhum.Select(previous_alternative);
        PlayMonophonic(&SFX_chhum, getHum());
      } else {
        PlayMonophonic(getHum(), NULL, 0.2f);
      }
    }
  }

  RefPtr<BufferedWavPlayer> PlayPolyphonic(const Effect::FileID& f)  {
    EnableAmplifier();
    if (!f) return RefPtr<BufferedWavPlayer>(nullptr);
    RefPtr<BufferedWavPlayer> player = GetOrFreeWavPlayer(f.GetEffect());
    if (player) {
      player->set_volume_now(font_config.volEff / 16.0f);
      player->PlayOnce(f);
      current_effect_length_ = player->length();
    } else {
      STDOUT.println("Out of WAV players!");
    }
    return player;
  }

  RefPtr<BufferedWavPlayer> PlayPolyphonic(Effect* f)  {
    return PlayPolyphonic(f->RandomFile());
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
    // Radians per second per second
    float rss = fusor.swing_accel();
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
            Effect* effect;
              if (rss > slashThreshold && SFX_slsh) {
              effect = &SFX_slsh;
            } else if (SFX_swng) {
              effect = &SFX_swng;
            } else {
              effect = &SFX_swing;
            }
            if (font_config.ProffieOSMaxSwingAcceleration > font_config.ProffieOSMinSwingAcceleration) {
              float s = (rss - font_config.ProffieOSMinSwingAcceleration) / font_config.ProffieOSMaxSwingAcceleration;
              effect->SelectFloat(s);
            }
            if (effect == &SFX_slsh) {
              SaberBase::DoEffect(EFFECT_ACCENT_SLASH, 0);
            } else {
              SaberBase::DoEffect(EFFECT_ACCENT_SWING, 0);
            }
            swinging_ = true;
          } else {
            // Already swinging
#ifdef ENABLE_SPINS
            if (angle_ > font_config.ProffieOSSpinDegrees) {
              if (SFX_spin) {
                SaberBase::DoEffect(EFFECT_SPIN, 0);
              }
              angle_ -= font_config.ProffieOSSpinDegrees;
            }
#endif
          }
        }
      } else {
        // Monophonic guessed
        if (!swinging_) {
          PlayMonophonic(&SFX_swing, &SFX_hum);
          swinging_ = true;
        }
#ifdef ENABLE_SPINS
        if (angle_ > 360 && swinging_) {
          if (SFX_spin) {
           SaberBase::DoEffect(EFFECT_SPIN, 0);
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
    if (swing_player_) {
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

  Effect* getOut() { return SFX_out ? &SFX_out : &SFX_poweron; }
  Effect* getHum() { return SFX_humm ? &SFX_humm : &SFX_hum; }

  void SB_Preon(EffectLocation location) {
    saved_location_ = location;
    StopIdleSound();
    if (SFX_preon) {
      SFX_preon.SetFollowing(getOut());
      // PlayCommon(&SFX_preon);
      RefPtr<BufferedWavPlayer> tmp = PlayPolyphonic(&SFX_preon);

      if (monophonic_hum_) {
        getOut()->SetFollowing(getHum());
        hum_player_ = tmp;
      }
    }
    SaberBase::RequestMotion();
    state_ = STATE_WAIT_FOR_ON;
  }

  Effect::FileID getNext(RefPtr<BufferedWavPlayer> previous, Effect* next) {
    if (previous) {
      return previous->current_file_id().GetFollowing(next);
    } else {
      return next->RandomFile();
    }
  }

  void SB_Postoff() {
    // Postoff was alredy started by linked wav players, we just need to find
    // the length so that WavLen<> can use it.
    RefPtr<BufferedWavPlayer> tmp = GetWavPlayerPlaying(&SFX_pstoff);
    if (tmp) {
      tmp->UpdateSaberBaseSoundInfo();
    } else {
      SaberBase::ClearSoundInfo();
    }
  }

  void SB_On(EffectLocation location) override {
    StopIdleSound();
    // If preon exists, we've already queued up playing the poweron and hum.
    bool already_started = state_ == STATE_WAIT_FOR_ON && SFX_preon;
    bool faston = state_ != STATE_WAIT_FOR_ON;
    if (monophonic_hum_) {
      if (!already_started) {
        PlayMonophonic(&SFX_poweron, &SFX_hum);
      }
      state_ = STATE_HUM_ON;
    } else {
      state_ = STATE_OUT;
      if (!hum_player_) {
        hum_player_ = GetFreeWavPlayer();
        if (hum_player_) {
          hum_player_->set_volume_now(0);
          hum_player_->PlayOnce(getNext(GetWavPlayerPlaying(getOut()), SFX_humm ? &SFX_humm : &SFX_hum));
          hum_player_->PlayLoop(SFX_humm ? &SFX_humm : &SFX_hum);
        }
        hum_start_ = millis();
      }
      RefPtr<BufferedWavPlayer> tmp;
      if (already_started) {
        tmp = GetWavPlayerPlaying(getOut());
        // Set the length for WavLen<>
        if (tmp) {
          tmp->UpdateSaberBaseSoundInfo();
        } else {
          SaberBase::ClearSoundInfo();
        }
      } else {
        tmp = PlayPolyphonic(faston && SFX_fastout ? &SFX_fastout : getOut());
      }
      hum_fade_in_ = 0.2;
      if (SFX_humm && tmp) {
        hum_fade_in_ = tmp->length();
        STDOUT << "HUM fade-in time: " << hum_fade_in_ << "\n";
      } else if (font_config.ProffieOSHumDelay >= 0) {
        hum_start_ += font_config.ProffieOSHumDelay;
        STDOUT << "HumDelay: " << font_config.ProffieOSHumDelay << "\n";
      } else if (font_config.humStart && tmp) {
        int delay_ms = 1000 * tmp->length() - font_config.humStart;
        if (delay_ms > 0 && delay_ms < 30000) {
          hum_start_ += delay_ms;
        }
        STDOUT << "humstart: " << font_config.humStart << "\n";
      }
    }
  }

  void SB_Off(OffType off_type, EffectLocation location) override {
    bool most_blades = location.on_blade(0);
#ifdef ENABLE_IDLE_SOUND
    if (most_blades) {
      Effect* bgnidle = AvoidIdleSDAccess() ? nullptr : &SFX_bgnidle;
      if (SFX_pstoff) {
        SFX_in.SetFollowing(&SFX_pstoff);
        SFX_pstoff.SetFollowing(bgnidle);
      } else {
        SFX_in.SetFollowing(bgnidle);
      }
    } else {
      SFX_in.SetFollowing(nullptr);
    }
#else
    SFX_in.SetFollowing(most_blades ? &SFX_pstoff : nullptr);
#endif
    switch (off_type) {
      case OFF_CANCEL_PREON:
        if (state_ == STATE_WAIT_FOR_ON) {
          state_ = STATE_OFF;
        }
        break;
      case OFF_IDLE:
        StopIdleSound();
        break;
      case OFF_FAST:
#ifdef ENABLE_IDLE_SOUND
        SFX_in.SetFollowing(&SFX_bgnidle);
#else
        SFX_in.SetFollowing(nullptr);
#endif
        [[gnu::fallthrough]];
      case OFF_NORMAL:
        if (!SFX_in) {
          size_t total = SFX_poweroff.files_found() + SFX_pwroff.files_found();
          Effect* effect;
          if (total) {
            if ((rand() % total) < SFX_poweroff.files_found()) {
              effect = &SFX_poweroff;
            } else {
              effect = &SFX_pwroff;
            }
            if (monophonic_hum_) {
              state_ = STATE_OFF;
              PlayMonophonic(effect, NULL);
            } else {
              state_ = STATE_HUM_FADE_OUT;
              PlayPolyphonic(effect);
            }
            hum_fade_out_ = current_effect_length_;
          } else if (monophonic_hum_) {
            state_ = STATE_OFF;
            // No poweroff, just fade out...
            hum_player_->set_fade_time(0.2);
            hum_player_->FadeAndStop();
            hum_player_.Free();
          } else {
            state_ = STATE_HUM_FADE_OUT;
            hum_fade_out_ = 0.2;
          }
        } else {
          state_ = STATE_HUM_FADE_OUT;
          PlayPolyphonic(getNext(hum_player_, &SFX_in));
          hum_fade_out_ = 0.2;
        }
        if (state_ == STATE_HUM_FADE_OUT && !most_blades) {
          state_ = STATE_HUM_ON;
        } else {
          check_postoff_ = !!SFX_pstoff && off_type != OFF_FAST;
          saved_location_ = location;
        }
        break;
      case OFF_BLAST:
        SaberBase::DoEffect(EFFECT_BOOM, 0);
        PVLOG_VERBOSE << "+++++ BOOM!! +++++\n";
        break;
    }

    if (swing_player_) {  
      swing_player_->set_fade_time(0.3);  
      swing_player_->FadeAndStop();  
      swing_player_.Free();  
    }
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;
      case EFFECT_MENU_CHANGE:
        if (!PlayPolyphonic(&SFX_mclick)) {
          SaberBase::sound_length = 0.2;
          beeper.Beep(0.05, 2000.0);
        }
        return;
      case EFFECT_PREON: SB_Preon(location); return;
      case EFFECT_POSTOFF: SB_Postoff(); return;
      case EFFECT_ACCENT_SWING:
        if (SFX_swng) {
          swing_player_ = PlayPolyphonic(&SFX_swng);
        } else if (!guess_monophonic_) {
          swing_player_ = PlayPolyphonic(&SFX_swing);
        } else {
          PlayMonophonic(&SFX_swing, &SFX_hum);
        }
        return;
      case EFFECT_ACCENT_SLASH:
        swing_player_ = PlayPolyphonic(&SFX_slsh);
        return;
#ifdef ENABLE_SPINS
      case EFFECT_SPIN:
        if (guess_monophonic_) {
          PlayMonophonic(&SFX_spin, &SFX_hum);
        } else {
          swing_player_ = PlayPolyphonic(&SFX_spin);
        }
        return;
#endif
      case EFFECT_STAB:
        if (SFX_stab) { PlayCommon(&SFX_stab); return; }
        // If no stab sounds are found, fall through to clash
        [[gnu::fallthrough]];
      case EFFECT_CLASH: Play(&SFX_clash, &SFX_clsh); return;
      case EFFECT_FORCE: PlayCommon(&SFX_force); return;
      case EFFECT_BLAST: Play(&SFX_blaster, &SFX_blst); return;
      case EFFECT_QUOTE: PlayCommon(&SFX_quote); return;
      case EFFECT_BOOT: PlayPolyphonic(&SFX_boot); return;
      case EFFECT_NEWFONT: SB_NewFont(); return;
      case EFFECT_LOCKUP_BEGIN: SB_BeginLockup(); return;
      case EFFECT_LOCKUP_END: SB_EndLockup(); return;
      case EFFECT_LOW_BATTERY: SB_LowBatt(); return;
      case EFFECT_ALT_SOUND:
        if (num_alternatives) {
          int previous_alternative = current_alternative;
          if (SaberBase::sound_number == -1) {
            // Next alternative
            if (++current_alternative >= num_alternatives)  current_alternative = 0;
          } else {
            // Select a specific alternative.
            current_alternative = std::min<int>(SaberBase::sound_number, num_alternatives - 1);
            // Set the sound num to -1 so that the altchng sound is random.
            SaberBase::sound_number = -1;
          }
          RestartHum(previous_alternative);
        }
        PlayCommon(&SFX_altchng);
        break;
      case EFFECT_BLADEIN:
        SB_BladeDetect(SFX_bladein);
        break;
      case EFFECT_BLADEOUT:
        SB_BladeDetect(SFX_bladeout);
        break;
      case EFFECT_BOOM:
        if (monophonic_hum_) {
          if (SFX_boom) PlayMonophonic(getNext(hum_player_, &SFX_boom), NULL);
          else PlayMonophonic(getNext(hum_player_, &SFX_clash), NULL);  // Thermal-D fallback
        } else {
          state_ = STATE_HUM_FADE_OUT;
          PlayPolyphonic(getNext(lock_player_, &SFX_boom));
        }
        break;
      case EFFECT_FONT_DIRECTORY_NOT_FOUND:
        PlayErrorMessage("e_fnt_nf.wav");
        break;
      case EFFECT_VOICE_PACK_NOT_FOUND:
        PlayErrorMessage("e_vp_nf.wav");
        break;
      case EFFECT_ERROR_IN_BLADE_ARRAY:
        PlayErrorMessage("e_blade.wav");
        break;
      case EFFECT_ERROR_IN_FONT_DIRECTORY:
        PlayErrorMessage("e_in_fnt.wav");
        break;
      case EFFECT_ERROR_IN_VOICE_PACK_VERSION:
        PlayErrorMessage("e_vp_ver.wav");
        break;
    }
  }

  void SB_BladeDetect(Effect& X) {
    if (X) {
      PlayPolyphonic(&X);
      return;
    }
    // SFX_bladein/out doesn't exist, playing font.wav instead
    if (SFX_font) {
      PVLOG_STATUS << "SFX_bladein/out doesn't exist, playing font.wav instead.\n";
      PlayPolyphonic(&SFX_font);
      return;
    }
    PVLOG_STATUS << " Didn't find font.wav, playing beep instead.\n";
    // Otherwise, just beep to indicate blade status change.
    beeper.Beep(0.05, 2000.0);
  }

  void SB_NewFont() {
    if (!PlayPolyphonic(&SFX_font)) {
      beeper.Beep(0.05, 1046.5);
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
        if (PlayPolyphonic(getNext(lock_player_, end))) {
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
      if (active_state() && !hum_player_) {
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
        case STATE_WAIT_FOR_ON:
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

  bool check_postoff_ = false;
  void Loop() override {
    if (state_ == STATE_WAIT_FOR_ON) {
      if (!GetWavPlayerPlaying(&SFX_preon)) {
        SaberBase::TurnOn(saved_location_);
        return;
      }
    }
    if (check_postoff_) {
      if (!GetWavPlayerPlaying(&SFX_in) &&
          !GetWavPlayerPlaying(&SFX_poweroff) &&
          !GetWavPlayerPlaying(&SFX_pwroff)) {
        check_postoff_ = false;
        SaberBase::DoEffect(EFFECT_POSTOFF, saved_location_);
      }
    }
  }

  void StopIdleSound() {
#ifdef ENABLE_IDLE_SOUND
    RefPtr<BufferedWavPlayer> idlePlayer = GetWavPlayerPlaying(&SFX_bgnidle);
    if (!idlePlayer) idlePlayer = GetWavPlayerPlaying(&SFX_idle);
    if (idlePlayer) {
      idlePlayer->set_fade_time(0.5);
      idlePlayer->FadeAndStop();
      idlePlayer.Free();
      PVLOG_DEBUG << "**** Stopped idle or bgnidle wav\n";
    }
#endif
  }

  bool swinging_ = false;
  void SB_Motion(const Vec3& gyro, bool clear) override {
    if (active_state() && !(SFX_lockup && SaberBase::Lockup())) {
      StartSwing(gyro,
                 font_config.ProffieOSSwingSpeedThreshold,
                 font_config.ProffieOSSlashAccelerationThreshold);
    }
  }

  float GetCurrentEffectLength() const {
    return current_effect_length_;
  }

  void SB_LowBatt() {
    ProffieOSErrors::low_battery();
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
  EffectLocation saved_location_;
};

#endif
