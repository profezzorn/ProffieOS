#ifndef SOUND_MONOPHONIC_FONT_H
#define SOUND_MONOPHONIC_FONT_H

// Monophonic sound fonts are the most common.
// These fonts are fairly simple, as generally only one sound is
// played at a time. It starts with the "poweron" sound and when
// that runs out, we gaplessly transition to the "hum" sound.
//
// When an effect happens, like "clash", we do a short cross-fade
// to transition to the new sound, then we play that sound until
// it ends and gaplessly transition back to the hum sound.
class MonophonicFont : public SaberBase {
public:
  MonophonicFont() : SaberBase(NOLINK) { }
  void Activate() {
    STDOUT.println("Activating monophonic font.");
    ActivateAudioSplicer();
    SaberBase::Link(this);
    SetHumVolume(1.0);
    on_ = false;
  }

  void Deactivate() { SaberBase::Unlink(this); }

  void SB_On() override {
    on_ = true;
    audio_splicer.Play(&poweron, &hum);
  }

  void SB_Off() override {
    on_ = false;
    size_t total = poweroff.files_found() + pwroff.files_found();
    if (total) {
      if ((rand() % total) < poweroff.files_found()) {
        audio_splicer.Play(&poweroff, NULL);
      } else {
        audio_splicer.Play(&pwroff, NULL);
      }
    }
  }
  void SB_Clash() override { audio_splicer.Play(&clash, &hum); }
  void SB_Stab() override { audio_splicer.Play(&stab, &hum); }
  void SB_Force() override { audio_splicer.Play(&force, &hum); }
  void SB_Blast() override { audio_splicer.Play(&blaster, &hum); }
  void SB_Boot() override { audio_splicer.Play(&boot,  NULL); }
  void SB_NewFont() override { audio_splicer.Play(&font,  NULL); }

  void SB_BeginLockup() override {
    if (SaberBase::Lockup() == SaberBase::LOCKUP_DRAG &&
        drag.files_found()) {
      audio_splicer.Play(&drag, &drag);
    } else if (lockup.files_found()) {
      audio_splicer.Play(&lockup, &lockup);
    }
  }

  void SB_EndLockup() override {
    if (lockup.files_found()) {
      audio_splicer.Play(&clash, &hum);
    }
  }

  void SetHumVolume(float vol) override {
    audio_splicer.set_volume(vol);
  }

  bool on_ = false;
  bool swinging_ = false;
  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    if (speed > 250.0) {
      if (!swinging_ && on_ && !SaberBase::Lockup()) {
        swinging_ = true;
        audio_splicer.Play(&swing, &hum);
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
};

#endif
