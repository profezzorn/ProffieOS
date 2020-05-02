// Fett263 Buttons
// Includes "Multi-Blast" Mode to enable Swing Blast control and SA22C volume menu
//
// 2 Button Controls (PWR and AUX):
// Ignite (ON) - click PWR while OFF
// Muted Ignition (ON) - double click PWR while OFF
// Retract (OFF) - click PWR while ON (not swinging)
// Play Music Track - hold and release PWR while OFF
// Blast - click AUX while ON
// Multi-Blast Mode - hold and release AUX while ON to enter mode, Swing to initiate Blasts, click Aux to exit mode
// Clash - clash blade while ON
// Stab - thrust forward and clash blade while ON
// Lockup - hold AUX and clash while ON
// Drag - hold AUX and clash pointing down while ON
// Lightning Block - hold PWR and click AUX while ON
// Melt - hold PWR (or AUX) and thrust forward and clash while ON
// Force - hold and release PWR while ON
// Color Change - hold AUX and click PWR while ON to enter CCWheel,
// turn hilt to rotate through colors, click PWR to select/exit
// if using COLOR_CHANGE_DIRECT each button press advances one Color at a time
// Next Preset - click AUX while OFF
// Previous Preset - hold AUX and click PWR while OFF
// Enter SA22C Volume Menu - hold and release AUX while OFF
// Volume Up (10% increment, 100% max) - click PWR while in Volume Menu while OFF
// Volume Down (10% increment) - click AUX while in Volume Menu while OFF
// Exit Volume Menu - hold and release AUX while in Volume Menu while OFF

#ifndef PROPS_SABER_FETT263_BUTTONS_H
#define PROPS_SABER_FETT263_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"

#undef PROP_TYPE
#define PROP_TYPE SaberFett263Buttons

// The Saber class implements the basic states and actions
// for the saber.
class SaberFett263Buttons : public PropBase {
public:
SaberFett263Buttons() : PropBase() {}
  const char* name() override { return "SaberFett263Buttons"; }

  // EVENT_SWING
  bool swinging_ = false;
  void Loop() override {
    PropBase::Loop();
    if (!swinging_ && fusor.swing_speed() > 250) {
      swinging_ = true;
      Event(BUTTON_NONE, EVENT_SWING);
    }
    if (swinging_ && fusor.swing_speed() < 100) {
      swinging_ = false;
    }
  }

  // SA22C Volume Menu
  void VolumeUp() {
    STDOUT.println("Volume up");
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1,
					     dynamic_mixer.get_volume() + VOLUME * 0.10));
      beeper.Beep(0.5, 2000);
      STDOUT.print("Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    }
    else {
      beeper.Beep(0.5, 3000);
    }
  }
  void VolumeDown() {
    STDOUT.println("Volume Down");
    if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
					     dynamic_mixer.get_volume() - VOLUME * 0.10));
      beeper.Beep(0.5, 2000);
      STDOUT.print("Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    }
    else{
      beeper.Beep(0.5, 1000);
    }
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        if (accel_.x < -0.15) {
          pointing_down_ = true;
        } else {
          pointing_down_ = false;
        }
        return true;
	
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
        if (mode_volume_) {
          VolumeUp();
        } else {
          On();
        }
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
	if (mode_volume_) {
	  VolumeDown();
        } else {
	  next_preset();
        }
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
	
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_OFF, MODE_ON):
	
#ifndef DISABLE_COLOR_CHANGE
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          // Just exit color change mode.
          // Don't turn saber off.
          ToggleColorChangeMode();
          return true;
        }
#endif
        if (!swinging_) {
          swing_blast_ = false;
          Off();
        }
        return true;
	
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        SaberBase::DoForce();
        return true;
	
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_DOUBLE_CLICK, MODE_ON):
        // Avoid the base and the very tip.
        // TODO: Make blast only appear on one blade!
        swing_blast_ = false;
        SaberBase::DoBlast();
        return true;
	
      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_ON):
        swing_blast_ = true;
        hybrid_font.SB_Blast();
        return true;
	
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
	if (swing_blast_) {
	  SaberBase::DoBlast();
	}
	return true;
	
#ifndef DISABLE_COLOR_CHANGE
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
        ToggleColorChangeMode();
        break;
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
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        break;
	
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
	swing_blast_ = false;
        SaberBase::DoBeginLockup();
        return true;
	
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          swing_blast_ = false;
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
	
      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_OFF):
        if (mode_volume_) {
          mode_volume_ = false;
          hybrid_font.SB_Change(EXIT_COLOR_CHANGE);
          // beeper.Beep(0.5, 3000);
          STDOUT.println("Exit Volume Menu");
        } else {
          mode_volume_ = true;
          hybrid_font.SB_Change(ENTER_COLOR_CHANGE);
          // beeper.Beep(0.5, 3000);
          STDOUT.println("Enter Volume Menu");
	}
	return true;
	
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
        next_preset();
        return true;
	
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
        previous_preset();
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
private:
  bool pointing_down_ = false;
  bool swing_blast_ = false;
  bool mode_volume_ = false;
};

#endif
