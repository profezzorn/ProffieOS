/*
How buttons behave when using this prop file:

On/Off -                  Zero buttons saber = Twist (2 directional, like revving a motorcycle)
                          ** Note that the motion has to be done long enough to count, so a very quick flick of the wrist will not work.
                          1 button saber = Click to turn the saber on or off.
                          2 button saber = Click POW
                          ** Note, if #define DUAL_POWER_BUTTONS is added to config file,  Clicking either POW or AUX will power on.
                             Also note that POW and AUX become swapped while the saber is on if AUX used to power on.
Turn On muted -           Double-click POW button
Next preset -             Zero button saber = Point up and shake
                          1 or 2 button saber = Hold POW button and hit the blade while saber is off.
Previous Preset -         Hold AUX button and click the POW button while saber is off.
Clash -                   Hit the blade while saber is on.
Lockup -                  Hold either POW or AUX, then trigger a clash. Release button to end.
Drag -                    Hold either POW or AUX, then trigger a clash while pointing down. Release button to end.
Melt -                    Hold either POW or AUX and stab something.
Force Lightning Block -   Click AUX while holding POW.
Force -                   Long-click POW button.
Start Soundtrack -        Long-click the POW button while blade is off.
Blaster block -           Short-click AUX button.
Enter/Exit Color Change - 1 button saber = Hold button and Twist.
                          2 button saber = Hold Aux and click POW while on.
** Note Color Change only works with ProffieOS 3.x and above)
*/

#ifndef PROPS_SABER_H
#define PROPS_SABER_H

#include "prop_base.h"

#define PROP_TYPE Saber

// The Saber class implements the basic states and actions
// for the saber.
class Saber : public PROP_INHERIT_PREFIX PropBase {
public:
  Saber() : PropBase() {}
  const char* name() override { return "Saber"; }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
#ifdef DUAL_POWER_BUTTONS
    if (SaberBase::IsOn() && aux_on_) {
      if (button == BUTTON_POWER) button = BUTTON_AUX;
      if (button == BUTTON_AUX) button = BUTTON_POWER;
    }
#endif
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        if (accel_.x < -0.15) {
          pointing_down_ = true;
        } else {
          pointing_down_ = false;
        }
      return true;

#if NUM_BUTTONS == 0
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
#endif
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
        aux_on_ = false;
        On();
        return true;

#ifdef BLADE_DETECT_PIN
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = true;
        FindBladeAgain();
        SaberBase::DoBladeDetect(true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = false;
        FindBladeAgain();
        SaberBase::DoBladeDetect(false);
        return true;
#endif

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF):
#ifdef DUAL_POWER_BUTTONS
        aux_on_ = true;
        On();
#else
        next_preset();
#endif
        return true;

        // Handle double-click with preon
      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_OFF):
        if (on_pending_) {
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
          }
          return true;
        }
        return false;

      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
        if (millis() - activated_ < 500) {
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
          }
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_OFF, MODE_ON):
#if NUM_BUTTONS == 0
#undef NEED_DETECT_TWIST
#define NEED_DETECT_TWIST
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
#endif
#ifndef DISABLE_COLOR_CHANGE
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          // Just exit color change mode.
          // Don't turn saber off.
          ToggleColorChangeMode();
          return true;
        }
#endif
        Off();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        SaberBase::DoForce();
        return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_DOUBLE_CLICK, MODE_ON):
        // Avoid the base and the very tip.
        // TODO: Make blast only appear on one blade!
        SaberBase::DoBlast();
        return true;

#ifndef DISABLE_COLOR_CHANGE
#if NUM_BUTTONS == 1
#undef NEED_DETECT_TWIST
#define NEED_DETECT_TWIST
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
#endif
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
        ToggleColorChangeMode();
        return true;
#endif

        // Lockup
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          if (pointing_down_) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          }
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
        SaberBase::DoBeginLockup();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

        // Off functions
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
        StartOrStopTrack();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        SaberBase::RequestMotion();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
#if NUM_BUTTONS == 0
#define NEED_DETECT_SHAKE
      case EVENTID(BUTTON_NONE, EVENT_SHAKE, MODE_OFF):
#endif
        next_preset();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
        previous_preset();
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_CLICK_SHORT, MODE_OFF):
#ifdef DUAL_POWER_BUTTONS
        next_preset();
#else
        previous_preset();
#endif
        return true;

        // Events that needs to be handled regardless of what other buttons
        // are pressed.
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          return true;
        }
    }
    return false;
  }

#if defined(NEED_DETECT_SHAKE) || defined(NEED_DETECT_TWIST)
  void Loop() override {
    PropBase::Loop();

#ifdef NEED_DETECT_TWIST
    DetectTwist();
#endif

#ifdef NEED_DETECT_SHAKE
    DetectShake();
#endif
    // DetectSwing();

#if NUM_BUTTONS == 0
    SaberBase::RequestMotion();
#endif
  }
#endif

private:
  bool aux_on_ = true;
  bool pointing_down_ = false;
};

#endif
