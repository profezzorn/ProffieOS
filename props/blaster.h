#ifndef PROPS_BLASTER_H
#define PROPS_BLASTER_H

#include "prop_base.h"

#define PROP_TYPE Blaster

class Blaster : public PropBase {
public:
  Blaster() : PropBase() {}
  const char* name() override { return "Blaster"; }

  // Mode states to handle kill vs stun effects
  enum BlasterMode {
    MODE_STUN,
    MODE_KILL,
    MODE_AUTO
  };

  BlasterMode blaster_mode = MODE_STUN;

  virtual void SetBlasterMode(BlasterMode to_mode) {
    if (!auto_firing_) {
      blaster_mode = to_mode;
      SaberBase::DoMode();
    }
  }

  virtual void NextBlasterMode() {
    switch(blaster_mode) {
      case MODE_STUN:
        SetBlasterMode(MODE_KILL);
        return;
      case MODE_KILL:
#ifdef ENABLE_BLASTER_AUTO
        SetBlasterMode(MODE_AUTO);
#else
        SetBlasterMode(MODE_STUN);
#endif
        return;
      case MODE_AUTO:
        SetBlasterMode(MODE_STUN);
        return;
    }
  }

  bool auto_firing_ = false;
  int shots_fired_ = 0;
  bool is_jammed_ = false;

#ifdef BLASTER_SHOTS_UNTIL_EMPTY
  const int max_shots_ = BLASTER_SHOTS_UNTIL_EMPTY;
#else
  const int max_shots_ = -1;
#endif

  virtual bool CheckJam(int percent) {
    int random = rand() % 100;
    return random <= percent ? true : false;
  }

  virtual void Fire() {
#ifdef ENABLE_MOTION
#ifdef BLASTER_JAM_PERCENTAGE
    // If we're already jammed then we don't need to recheck. If we're not jammed then check if we just jammed.
    is_jammed_ = is_jammed_ ? true : CheckJam(BLASTER_JAM_PERCENTAGE);

    if (is_jammed_) {
      SaberBase::DoJam();
      return;
    }
#endif
#endif

    if (max_shots_ != -1) {
      if (shots_fired_ >= max_shots_) {
        SaberBase::DoEmpty();
        return;
      }
    }

    if (blaster_mode == MODE_AUTO) {
      SelectAutoFirePair(); // Set up the autofire pairing if the font suits it.
      SaberBase::SetLockup(LOCKUP_AUTOFIRE);
      SaberBase::DoBeginLockup();
      auto_firing_ = true;
    } else {
      if (blaster_mode == MODE_STUN) {
        SaberBase::DoStun();
      } else {
        SaberBase::DoFire();
      }

      shots_fired_++;
    }
  }

  virtual void SelectAutoFirePair() {
    if (!SFX_auto.files_found() || !SFX_blast.files_found()) return;

    int autoCount = SFX_auto.files_found();
    int blastCount = SFX_blast.files_found();
    int pairSelection;

    // If we don't have a matched pair of autos and blasts, then don't override the sequence to get a matched pair.
    if (autoCount == blastCount) {
        pairSelection = rand() % autoCount;
        SFX_auto.Select(pairSelection);
        SFX_blast.Select(pairSelection);
    }
  }

  virtual void Reload() {
    shots_fired_ = 0;
    SaberBase::DoReload();
  }

  virtual void ClipOut() {
    if (max_shots_ != -1) shots_fired_ = max_shots_;
    SaberBase::DoClipOut();
  }

  virtual void ClipIn() {
    SaberBase::DoClipIn();
  }

  // Pull in parent's SetPreset, but turn the blaster on.
  void SetPreset(int preset_num, bool announce) override {
    PropBase::SetPreset(preset_num, announce);

    if (!SFX_poweron) {
      if (!SaberBase::IsOn()) {
        On();
      }
    }
  }

  void LowBatteryOff() override {
    if (SFX_poweron) {
      PropBase::LowBatteryOff();
    }
  }

  // Self-destruct pulled from detonator 
  bool armed_ = false;

  enum NextAction {
    NEXT_ACTION_NOTHING,
    NEXT_ACTION_ARM,
    NEXT_ACTION_BLOW,
  };

  NextAction next_action_ = NEXT_ACTION_NOTHING;
  uint32_t time_base_;
  uint32_t next_event_time_;
  
  void SetNextAction(NextAction what, uint32_t when) {
    time_base_ = millis();
    next_event_time_ = when;
    next_action_ = what;
  }

  void SetNextActionF(NextAction what, float when) {
    SetNextAction(what, when * 1000);
  }
  
  void PollNextAction() {
    if (millis() - time_base_ > next_event_time_) {
      switch (next_action_) {
        case NEXT_ACTION_NOTHING:
          break;
        case NEXT_ACTION_ARM:
          armed_ = true;
          // TODO: Should we have separate ARMING and ARMED states?
          break;
        case NEXT_ACTION_BLOW:
          Off(OFF_BLAST);
          break;
      }
      next_action_ = NEXT_ACTION_NOTHING;
    }
  }
  
  void beginArm() {
    SaberBase::SetLockup(SaberBase::LOCKUP_ARMED);
    SaberBase::DoBeginLockup();
#ifdef ENABLE_AUDIO    
    float len = hybrid_font.GetCurrentEffectLength();
#else    
    float len = 1.6;
#endif
    SetNextActionF(NEXT_ACTION_ARM, len);
  }

  void selfDestruct() {
    SaberBase::DoEndLockup();
#ifdef ENABLE_AUDIO    
    float len = hybrid_font.GetCurrentEffectLength();
#else    
    float len = 0.0;
#endif
    SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
    if (armed_) {
      SetNextActionF(NEXT_ACTION_BLOW, len);
    } else {
      SetNextAction(NEXT_ACTION_NOTHING, 0);
    }
  }

  void Loop() override {
    PropBase::Loop();
    PollNextAction();
  }

  // Make clash do nothing except unjam if jammed.
  void Clash(bool stab) override {
    if (is_jammed_) {
      is_jammed_ = false;
      SaberBase::DoUnJam();
    }
  }

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override {}

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

      case EVENTID(BUTTON_MODE_SELECT, EVENT_PRESSED, MODE_ON):
        NextBlasterMode();
        return true;

      case EVENTID(BUTTON_MODE_SELECT, EVENT_DOUBLE_CLICK, MODE_ON):
        next_preset();
        return true;

      case EVENTID(BUTTON_RELOAD, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_HELD_MEDIUM, MODE_ON):
        Reload();
        return true;

      case EVENTID(BUTTON_MODE_SELECT, EVENT_HELD_LONG, MODE_ON):
        StartOrStopTrack();
        return true;

      case EVENTID(BUTTON_FIRE, EVENT_PRESSED, MODE_ON):
        Fire();
        return true;

      case EVENTID(BUTTON_FIRE, EVENT_RELEASED, MODE_ON):
        if (blaster_mode == MODE_AUTO) {
          if (SaberBase::Lockup()) {
            SaberBase::DoEndLockup();
            SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
            auto_firing_ = false;
          }
        }
        return true;

      case EVENTID(BUTTON_CLIP_DETECT, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_ON, MODE_ON):
        ClipIn();
        return true;

      case EVENTID(BUTTON_CLIP_DETECT, EVENT_RELEASED, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_OFF, MODE_ON):
        ClipOut();
        return true;

      // In the event of the presence of a power button, let it control the power on events.
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        On();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
        Off();
        return true;
    }
    return false;
  }
};

#endif
