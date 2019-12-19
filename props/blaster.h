#ifndef PROPS_BLASTER_H
#define PROPS_BLASTER_H

#include "prop_base.h"

#define PROP_TYPE Blaster

class Blaster : public PropBase {
public:
  Blaster() : PropBase() {}
  const char* name() override { return "Blaster"; }

/*
#ifdef DELAYED_OFF
  bool powered_ = false;
  void SetPower(bool on) { powered_ = on; }
#else
  constexpr bool powered_ = true;
  void SetPower(bool on) {}
#endif
*/

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
    
#ifdef ENABLE_AUDIO
      // Initiate mode change effect
      if (SFX_mode.files_found()) {
        hybrid_font.PlayCommon(&SFX_mode);
      }
#endif
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

  virtual void Fire() {
    if (!SaberBase::IsOn()) return; // Don't allow firing when off.

    if (blaster_mode == MODE_AUTO) {
      SaberBase::SetLockup(LOCKUP_AUTOFIRE);
      SaberBase::DoBeginLockup();
      auto_firing_ = true;
    } else {
      SaberBase::DoBlast();
#ifdef ENABLE_AUDIO
      if (blaster_mode == MODE_STUN) {
        hybrid_font.PlayCommon(&SFX_stun);
      } else {
        hybrid_font.PlayCommon(&SFX_blast);
      }
#endif
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

  // Make clash do nothing
  void Clash(bool stab) override {}

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override {}

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
        armed_ = false;
#ifdef ENABLE_AUDIO
        hybrid_font.PlayCommon(&SFX_unjam);
#endif
        On();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
#ifdef ENABLE_AUDIO
        hybrid_font.PlayCommon(&SFX_jam);
#endif
        Off();
        return true;

      case EVENTID(BUTTON_AUX, EVENT_DOUBLE_CLICK, MODE_OFF):
        next_preset();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
        StartOrStopTrack();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
        NextBlasterMode();
        return true;

      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        Fire();
        return true;

      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ON):
        if (blaster_mode == MODE_AUTO) {
          if (SaberBase::Lockup()) {
            SaberBase::DoEndLockup();
            SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
            auto_firing_ = false;
          }
        }
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
        beginArm();
        break;

      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ON):
        selfDestruct();
        return armed_;

        // TODO: Long click when off?
    }
    return false;
  }
};

#endif
