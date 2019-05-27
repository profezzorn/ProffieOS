#ifndef PROPS_DETONATOR_H
#define PROPS_DETONATOR_H

#include "prop_base.h"

#define PROP_TYPE Detonator

class Detonator : public PropBase {
public:
  Detonator() : PropBase() {}
  const char* name() override { return "Detonator"; }

  void rotate_presets() {
#ifdef ENABLE_AUDIO
    beeper.Beep(0.05, 2000.0);
#endif
    current_preset_.Load(-1);  // load last preset
    current_preset_.SaveAt(0); // save in first position, shifting all other presets down
    SetPreset(0, true);
  }

  bool armed_ = false;
  void arm() {
    Serial.println("ARM");
    armed_ = true;
    SaberBase::SetLockup(SaberBase::LOCKUP_ARMED);
    SaberBase::DoBeginLockup();
  }

  void blast() {
    if (armed_) {
      SaberBase::DoEndLockup();
      Off(OFF_BLAST);
    }
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
	armed_ = false;
	On();
	return true;

      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
	Off();
	return true;

      case EVENTID(BUTTON_AUX2, EVENT_DOUBLE_CLICK, MODE_OFF):
	rotate_presets();
	return true;

      case EVENTID(BUTTON_AUX2, EVENT_DOUBLE_CLICK, MODE_ON):
        StartOrStopTrack();
	return true;

      case EVENTID(BUTTON_AUX2, EVENT_HELD_LONG, MODE_ON):
	arm();
	break;

      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ON):
	blast();
	return true;

	// TODO: Long click when off?
    }
    return false;
  }
};

#endif
