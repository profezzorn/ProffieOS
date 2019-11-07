// sa22c props file, includes the following changes
//
// New #define NO_LOCKUP_HOLD
// reverts to lockup being triggered only by clash + pow or aux
// Button configs:
//
// 1 Button:
// Activate Muted - fast double click while OFF
// Activate - short click while OFF
// Play/Stop Music - hold and release while ON
// Turn off blade - hold and wait till blade is off while ON
// Next Preset - hold and release while OFF
// Prev Preset - hold and wait while OFF
// Lockup - hold + hit clash while ON
// Drag - hold + hit clash while ON pointing the blade tip down
// Blaster Blocks - short click while ON
// Force Effects - hold + twist the hilt while ON (while pointing up)
// Color Change mode - hold + twist the hilt while ON (pointing down)
//
// 2 Button:
// POWER
// Activate Muted - fast double click while OFF
// Activate - short click while OFF
// Play/Stop Music - hold and release while OFF
// Turn off blade - hold and wait till blade is off while ON
// Force Effects - double click while ON
// Volume UP - short click while OFF and in VOLUME MENU
// Prev Preset - hold and wait while OFF
// Color Change mode - hold + toggle AUX while ON
// AUX
// Blaster blocks - short click while ON
// Next Preset - short click while OFF
// Lockup - hold while ON
// Drag - hold while ON pointing the blade tip down
// Enter VOLUME MENU - long click while OFF
// Volume down - short click while OFF and in VOLUME MENU
// Battery level - hold while off
//
// 3 Button: Same as two button except for VOLUME MENU
// AUX
// Volume UP - short click while OFF and in VOLUME MENU
// AUX2
// Lockup - hold while ON
// Previous Preset - short click while OFF
// Volume DOWN - short click while OFF and in VOLUME MENU

#ifndef PROPS_SABER_SA22C_BUTTONS_H
#define PROPS_SABER_SA22C_BUTTONS_H

#include "prop_base.h"

#undef PROP_TYPE
#define PROP_TYPE SaberSA22CButtons

// The Saber class implements the basic states and actions
// for the saber.
class SaberSA22CButtons : public PropBase {
public:
  SaberSA22CButtons() : PropBase() {}
  const char* name() override { return "SaberSA22CButtons"; }

  void ChangeVolume(bool up) {
    if (up) {
      STDOUT.println("Volume up");
      if (dynamic_mixer.get_volume() < VOLUME) {
        dynamic_mixer.set_volume(dynamic_mixer.get_volume() + VOLUME * 0.10);
        beeper.Beep(0.5, 2000);
        STDOUT.print("Current Volume: ");
        STDOUT.println(dynamic_mixer.get_volume());
      }
      else {
        beeper.Beep(0.5, 3000);
      }
    } else {
      STDOUT.println("Volume Down");
      if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
        dynamic_mixer.set_volume(dynamic_mixer.get_volume() - VOLUME * 0.10);
        beeper.Beep(0.5, 2000);
        STDOUT.print("Current Volume: ");
        STDOUT.println(dynamic_mixer.get_volume());
      }
      else{
        beeper.Beep(0.5, 1000);
      }
    }
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
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
 #if NUM_BUTTONS == 2
        if (mode_volume_){
          ChangeVolume(true);
        }
#endif
        if (!mode_volume_) {
          aux_on_ = false;
          On();
        }
	return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF):
#if NUM_BUTTONS == 2
        if (mode_volume_) {
          ChangeVolume(false);
        }
#endif
#if NUM_BUTTONS == 3
        if (mode_volume_) {
          ChangeVolume(true);
        }
#endif
        if (!mode_volume_) {
#ifdef DUAL_POWER_BUTTONS
          aux_on_ = true;
          On();
#else
      if (!mode_volume_) {
        next_preset();
	  }

#endif
        }
	return true;

#if NUM_BUTTONS > 1
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
        SaberBase::DoBlast();
        return true;
#endif
#if NUM_BUTTONS == 1
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
#ifndef DISABLE_COLOR_CHANGE
        if (accel_.x < -0.15) {
          ToggleColorChangeMode();
        } else {
          SaberBase::DoForce();
        }
#else
        SaberBase::DoForce();
#endif
        return true;
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        StartOrStopTrack();
        return true;
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
        SaberBase::DoBlast();
        return true;
#endif
      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
        if (millis() - activated_ < 500) {
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
          }
        }
#if NUM_BUTTONS > 1
        else {
          SaberBase::DoForce();
        }
#endif
	return true;

#if NUM_BUTTONS == 1
        case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_ON):
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
	        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
	          // Just exit color change mode.
	          // Don't turn saber off.
	          ToggleColorChangeMode();
	          return true;
	        }
#endif
          Off();
        }
        return true;
#endif

#ifndef DISABLE_COLOR_CHANGE
#ifdef NO_LOCKUP_HOLD
    case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
      ToggleColorChangeMode();
      break;
#else //NO_LOCKUP_HOLD
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
      ToggleColorChangeMode();
      break;
#endif //NO_LOCKUP_HOLD
#endif //DISABLE_COLOR_CHANGE
#if NUM_BUTTONS > 1
      case EVENTID(BUTTON_POWER, EVENT_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_OFF, MODE_ON):
#endif
#if NUM_BUTTONS == 0
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
#endif
#if NUM_BUTTONS != 1
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            // Just exit color change mode.
            // Don't turn saber off.
            ToggleColorChangeMode();
            return true;
          }
#endif
          Off();
        }
#endif
        return true;

    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
#ifndef NO_LOCKUP_HOLD
	  case EVENTID(BUTTON_AUX, EVENT_HELD, MODE_ON):
#endif
      if (!SaberBase::Lockup()) {
        if (pointing_down_) {
          SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
        } else {
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        }
        SaberBase::DoBeginLockup();
        return true;
      }

        // Off functions
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
#if NUM_BUTTONS == 1
	  if (!mode_volume_);
        next_preset();

        return true;
#endif
#if NUM_BUTTONS > 1
        StartOrStopTrack();
#endif
	return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_OFF):
      if (mode_volume_){
        mode_volume_ = false;
        beeper.Beep(0.5, 3000);
        STDOUT.println("Exit Volume Menu");
      }
      else{
        mode_volume_ = true;
        beeper.Beep(0.5, 3000);
        STDOUT.println("Enter Volume Menu");
      }
      return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        SaberBase::RequestMotion();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
      if (!mode_volume_) {
          next_preset();
        }
        return true;

#if NUM_BUTTONS < 3
      case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_OFF):
#endif
        if (!mode_volume_) {
          previous_preset();
		}
	return true;
      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF):
      talkie.SayDigit((int)floorf(battery_monitor.battery()));
      talkie.Say(spPOINT);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
      talkie.Say(spVOLTS);
      return true;

      case EVENTID(BUTTON_AUX2, EVENT_CLICK_SHORT, MODE_OFF):
      STDOUT.println(NUM_BUTTONS);
#if NUM_BUTTONS == 3
        if (mode_volume_) {
          ChangeVolume(false);
        }

#endif

#ifdef DUAL_POWER_BUTTONS
        if (!mode_volume_) {
          next_preset();
        }
#else
        if (!mode_volume_) {
          previous_preset();
        }
#endif
  return true;

  // Events that needs to be handled regardless of what other buttons
  // are pressed.
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      if (SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        return true;
      }
    }
    return false;
  }
private:
  bool aux_on_ = true;
  bool pointing_down_ = false;
  bool mode_volume_ = false;
};

#endif
