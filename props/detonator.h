#ifndef PROPS_DETONATOR_H
#define PROPS_DETONATOR_H

#include "prop_base.h"

#define PROP_TYPE Detonator

class Detonator : public PROP_INHERIT_PREFIX PropBase {
public:
  Detonator() : PropBase() {}
  const char* name() override { return "Detonator"; }

#ifdef DELAYED_OFF
  bool powered_ = false;
  void SetPower(bool on) { powered_ = on; }
#else
  constexpr bool powered_ = true;
  void SetPower(bool on) {}
#endif

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

  void blast() {
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

#if NUM_BUTTONS >= 2
  // Make clash do nothing
  void Clash(bool stab, float strength) override {}
#endif

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override { }


  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
        armed_ = false;
        SetPower(true);
        On();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_OFF):
        SetPower(false);
        Off();
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_DOUBLE_CLICK, MODE_OFF):
	if (powered_) rotate_presets();
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_DOUBLE_CLICK, MODE_ON):
        StartOrStopTrack();
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
	beginArm();
	break;

      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ON):
        blast();
	return armed_;

        // TODO: Long click when off?
    }
    return false;
  }
};

#endif
