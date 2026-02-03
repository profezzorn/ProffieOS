/* This version of detonator.h, has been modified by me (Oli) to make it usable on a two button saber (with dual_prop or multi_prop)
without the need of a latching button or Profezzorn's specialized helper board ( https://oshpark.com/shared_projects/9y1xfZRV )

However, if you want to use this detonator.h as a "stand-alone" with latching power button but without the specialized helper board,
you will need to install a kill switch in your prop.

============= LIST OF BUTTON & MOTION COMMANDS! =================================================================================
Button Power: (BUTTON_POWER can either be latching or it needs to be held)
=============
Latch ON (or press and hold) : ON & ARMED
Latch OFF (or release) : OFF & DISARMED (will stop the "countdown")

Button Aux:
===========
Double click while off : Next preset
Double click while on : Start/Stop track
'Press and Release' while Armed : Plays beginarm.wav & armhum.wav "countdown" on repeat.
Long click while "counting-down" : stops the "countdown" and plays boom.wav.

============= LIST OF .wav USED in this detonator: ==============================================================================
armhum.wav
bgnarm.wav
boom.wav
hum.wav (if you don't want to hear it, make it a silent hum or you will get error in font directory!)

Optional .wav files:
====================
boot.wav
font.wav

.wav files you do not want:
===========================
endarm.wav (Please note that if you have an endarm.wav in your font, it will play before boom!)

Thank you for reading.
*/

#ifndef PROPS_DETONATOR_H
#define PROPS_DETONATOR_H

#if NUM_BUTTONS < 2
#error Your prop NEEDS 2 buttons to use this detonator
#endif

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
  bool powered_ = true;
  void SetPower(bool on) {}
#endif

  enum NextAction {
    NEXT_ACTION_NOTHING,
    NEXT_ACTION_ARM,
    NEXT_ACTION_BLOW,
  };

  NextAction next_action_ = NEXT_ACTION_NOTHING;
  uint32_t time_base_;
  uint32_t next_event_time_;
  bool armed_ = false;

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
          armed_ = false;
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
  void DoMotion(const Vec3& motion, bool clear) override {}

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        armed_ = false;
        SetPower(true);
        On();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        armed_ = false;
        SetPower(false);
        Off();
        return true;

      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        if (powered_) rotate_presets();
        return true;

      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        StartOrStopTrack();
        return true;

      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        beginArm();
        break;

      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_LONG, MODE_ON):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        blast();
        return true;

        // Available To Use: case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF):
    }
    return false;
  } // Event2
}; // class Detonator

#endif // PROPS_DETONATORS_H
