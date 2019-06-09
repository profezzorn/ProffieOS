#ifndef PROPS_DETONATOR_H
#define PROPS_DETONATOR_H

#include "prop_base.h"

#define PROP_TYPE Detonator

class Detonator : public PropBase {
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

  void rotate_presets() {
    if (!powered_) return;
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

#if NUM_BUTTONS >= 2
  // Make clash do nothing
  void Clash() override {}
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
