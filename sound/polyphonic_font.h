#ifndef SOUND_POLYPHONIC_FONT_H
#define SOUND_POLYPHONIC_FONT_H

class IgniterConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(humStart, 100);
    CONFIG_VARIABLE(volHum, 15);
    CONFIG_VARIABLE(volEff, 16);
  }
  int humStart;
  int volHum;
  int volEff;
};

// With polyphonic fonts, sounds are played more or less
// independently. Hum is faded in/out by changing the volume
// and all other sound effects are just played in parallel
// when needed.
class PolyphonicFont : public SaberBase {
public:
  PolyphonicFont() : SaberBase(NOLINK) { }
  void Activate() {
    STDOUT.println("Activating polyphonic font.");
    SetupStandardAudio();
    wav_players[0].set_volume_now(0);
    config_.ReadInCurrentDir("config.ini");
    SaberBase::Link(this);
    state_ = STATE_OFF;
    lock_player_ = NULL;
  }
  enum State {
    STATE_OFF,
    STATE_OUT,
    STATE_HUM_FADE_IN,
    STATE_HUM_ON,
    STATE_HUM_FADE_OUT,
  };
  void Deactivate() { SaberBase::Unlink(this); }

  void SB_On() override {
    state_ = STATE_OUT;
    wav_players[0].PlayOnce(&hum);
    wav_players[0].PlayLoop(&hum);
    hum_start_ = millis();
    if (config_.humStart) {
      BufferedWavPlayer* tmp = Play(&out);
      if (tmp) {
        int delay_ms = 1000 * tmp->length() - config_.humStart;
        if (delay_ms > 0 && delay_ms < 30000) {
          hum_start_ += delay_ms;
        }
      }
    }
  }

  void SB_Off() override {
    state_ = STATE_HUM_FADE_OUT;
    Play(&in);
  }

  BufferedWavPlayer* Play(Effect* f)  {
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    BufferedWavPlayer* player = GetFreeWavPlayer();
    if (player) {
      player->set_volume_now(config_.volEff / 16.0);
      player->PlayOnce(f);
    }
    return player;
  }
  void SB_Clash() override { Play(&clsh); }
  void SB_Stab() override { Play(&stab); }
  void SB_Force() override { Play(&force); }
  void SB_Blast() override { Play(&blst); }
  void SB_Boot() override { Play(&boot); }
  void SB_NewFont() override { Play(&font); }

  BufferedWavPlayer* lock_player_ = NULL;
  void SB_BeginLockup() override {
    Effect* e = &lock;
    if (SaberBase::Lockup() == SaberBase::LOCKUP_DRAG &&
        drag.files_found()) {
      e = &drag;
    }
    if (!lock_player_) {
      lock_player_ = Play(e);
      if (lock_player_) {
        lock_player_->PlayLoop(e);
      }
    }
  }

  void SB_EndLockup() override {
    if (lock_player_) {
      lock_player_->set_fade_time(0.3);
      lock_player_->FadeAndStop();
      lock_player_ = NULL;
    }
  }

  bool swinging_ = false;
  uint32_t hum_start_;
  uint32_t last_micros_;

  void SetHumVolume(float vol) override {
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
        uint32_t delta = m - last_micros_;
        volume_ -= (delta / 1000000.0) / 0.2; // 0.2 seconds
        if (volume_ <= 0.0f) {
          volume_ = 0.0f;
          state_ = STATE_OFF;
	  wav_players[0].FadeAndStop();
        }
        break;
      }
    }
    last_micros_ = m;
    wav_players[0].set_volume(vol * volume_);
  }
  
  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    if (speed > 250.0) {
      if (!swinging_ && state_ != STATE_OFF) {
        swinging_ = true;
        Play(&swng);
      }
    } else {
      swinging_ = false;
    }
    float vol = 1.0f;
    if (!swinging_) {
      vol = vol * (0.99 + clamp(speed/200.0, 0.0, 2.3));
    }
    SetHumVolume(vol);
  }

  IgniterConfigFile config_;
  State state_;
  float volume_;
};

#endif
