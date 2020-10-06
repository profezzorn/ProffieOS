// Fett263 Buttons with Gesture Controls and Battle Mode with Smart Lockup
//
// Includes Gesture Controls, "Battle Mode" with "Smart Lockup", "Multi-Blast" Mode (to enable Swing Blast control)
// "Multi-Phase" Mode, "On Demand Power Save", "On Demand Battery Level", "Fast On" Ignition (no preon) and SA22C volume menu
//
// Optional Defines
//
// FETT263_BATTLE_MODE_ALWAYS_ON
// Battle Mode is always on, normal controls deactivated
// This will disable traditional Clash and Stab effects 
//
// FETT263_LOCKUP_DELAY 200
// This is the "delay" in millis to determine Clash vs Lockup
//
// FETT263_BM_DISABLE_OFF_BUTTON
// During Battle Mode Power Button Retraction is disabled
//
// FETT263_SWING_ON
// To enable Swing On Ignition control (automatically enters Battle Mode, uses Fast On)
//
// FETT263_SWING_ON_NO_BM
// To enable Swing On Ignition control but not activate Battle Mode
//
// FETT263_SWING_ON_SPEED 250
// Adjust Swing Speed required for Ignition 250 ~ 500 recommended
//
// FETT263_TWIST_OFF
// To enable Twist Off Retraction control
//
// FETT263_STAB_ON
// To enable Stab On Ignition control (automatically enters Battle Mode, uses Fast On)
//
// FETT263_STAB_ON_NO_BM
// To enable Stab On Ignition control but not activate Battle Mode
//
// FETT263_GESTURE_PREON
// Disables Fast On ignition on gesture so Preon is used
//
// FETT263_MULTI_PHASE
// This will enable "live" preset change to create a "Multi-Phase" saber effect
// with preset changes on the fly while blade is ignited.
//
// 2 Button Controls (PWR and AUX):
// "Battle Mode" - hold AUX and Swing while blade is ON to toggle mode ON/OFF
// Ignite (ON) - click PWR while OFF (Swing On and Stab On available with defines)
// Muted Ignition (ON) - double click PWR while OFF
// Retract (OFF) - click PWR while ON (not swinging or in Battle Mode)
// Twist Off (OFF) if in Battle Mode
// Play Music Track - hold and release PWR while OFF
// Blast - click AUX while ON
// Multi-Blast Mode - hold and release AUX while ON to enter mode, Swing to initiate Blasts, click Aux to exit mode
// Clash - clash blade while ON
// Stab - thrust forward and clash blade while ON
// Lockup - hold AUX and clash while ON
// Drag - hold AUX and stab pointing down while ON
// Lightning Block - hold PWR and click AUX while ON
// Melt - hold PWR (or AUX) and thrust forward and clash while ON
// Force - hold and release PWR while ON
// Power Save - hold Aux and click PWR while ON (pointing up) to use Power Save (requires style)
// Color Change - hold AUX and click PWR while ON (parallel or down) to enter CCWheel,
// turn hilt to rotate through colors, click PWR to select/exit
// if using COLOR_CHANGE_DIRECT each button press advances one Color at a time
//
// Next Preset - click AUX while OFF (parallel or up)
// Previous Preset - click Aux while OFF (point down)
// MULTI_PHASE Next Preset - hold AUX and TWIST while ON (use define to enable)
// MULTI_PHASE Previous Preset - hold PWR and TWIST while ON (use define to enable)
// Battery Level - hold AUX and click PWR while OFF (requires style)
// Enter SA22C Volume Menu - hold and release AUX while OFF
// Volume Up (10% increment, 100% max) - click PWR while in Volume Menu while OFF
// Volume Down (10% increment) - click AUX while in Volume Menu while OFF
// Exit Volume Menu - hold and release AUX while in Volume Menu while OFF
//
// Custom sounds supported:
// On Demand Power Save - dim.wav
// On Demand Battery Level - battery.wav
// Battle Mode On (on toggle) - bmbegin.wav
// Battle Mode Off (on toggle) - bmend.wav
// Enter Volume Menu - vmbegin.wav
// Exit Volume Menu - vmend.wav
// Fast On (optional) - faston.wav
// Multi-Blast Mode On - blstbgn.wav
// Multi-Blast Mode Off - blstend.wav

#ifndef PROPS_SABER_FETT263_BUTTONS_H
#define PROPS_SABER_FETT263_BUTTONS_H

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 15 * 1000
#endif

#ifndef FETT263_SWING_ON_SPEED
#define FETT263_SWING_ON_SPEED 250
#endif

#ifndef FETT263_LOCKUP_DELAY
#define FETT263_LOCKUP_DELAY 200
#endif

#if defined(FETT263_SWING_ON) && defined(FETT263_SWING_ON_NO_BM)
#error You cannot define both FETT263_SWING_ON and FETT263_SWING_ON_NO_BM
#endif

#if defined(FETT263_STAB_ON) && defined(FETT263_STAB_ON_NO_BM)
#error You cannot define both FETT263_STAB_ON and FETT263_STAB_ON_NO_BM
#endif

#include "prop_base.h"
#include "../sound/hybrid_font.h"

#undef PROP_TYPE
#define PROP_TYPE SaberFett263Buttons

EFFECT(dim);
EFFECT(battery);
EFFECT(bmbegin);
EFFECT(bmend);
EFFECT(vmbegin);
EFFECT(vmend);
EFFECT(faston);
EFFECT(blstbgn);

// The Saber class implements the basic states and actions
// for the saber.
class SaberFett263Buttons : public PropBase {
public:
SaberFett263Buttons() : PropBase() {}
  const char* name() override { return "SaberFett263Buttons"; }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    DetectSwing();
    if (SaberBase::IsOn()) {
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > FETT263_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > FETT263_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }
    } else {
      // Swing On gesture control this portion allows fine tuning of speed needed to ignite
      if (millis() - saber_off_time_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (!swinging_ && fusor.swing_speed() > FETT263_SWING_ON_SPEED) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
      }
    }
  }

// Fast On Gesture Ignition
  virtual void FastOn() {
    if (IsOn()) return;
    if (current_style() && current_style()->NoOnOff())
      return;
    activated_ = millis();
    STDOUT.println("Ignition.");
    MountSDCard();
    EnableAmplifier();
    SaberBase::RequestMotion();
    // Avoid clashes a little bit while turning on.
    // It might be a "clicky" power button...
    IgnoreClash(500);
    SaberBase::TurnOn();
    // Optional effects
    SaberBase::DoEffect(EFFECT_FAST_ON, 0);
    hybrid_font.PlayCommon(&SFX_faston);
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
    } else {
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
    } else {
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

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT, MODE_ON):
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
#ifndef FETT263_BM_DISABLE_OFF_BUTTON
	  Off();
          saber_off_time_ = millis();
#endif
#ifndef FETT263_BATTLE_MODE_ALWAYS_ON
	  battle_mode_ = false;
#endif
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
        hybrid_font.PlayCommon(&SFX_blstbgn);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
        if (swing_blast_) {
	  SaberBase::DoBlast();
        }
        return true;


      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
        if (fusor.angle1() >  M_PI / 3) {
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
        } else {
#ifndef DISABLE_COLOR_CHANGE
          ToggleColorChangeMode();
#endif
#ifdef DISABLE_COLOR_CHANGE
	  SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
#endif
	}
        return true;


        // Lockup
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
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
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
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
          hybrid_font.PlayCommon(&SFX_vmend);
          // beeper.Beep(0.5, 3000);
          STDOUT.println("Exit Volume Menu");
        } else {
          mode_volume_ = true;
          hybrid_font.PlayCommon(&SFX_vmbegin);
          // beeper.Beep(0.5, 3000);
          STDOUT.println("Enter Volume Menu");
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
        if (fusor.angle1() < - M_PI / 4) {
          previous_preset();
        } else {
          next_preset();
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        last_twist_ = millis();
        SaberBase::DoEffect(EFFECT_BATTERY, 0);
        return true;

       // Battle Mode

#ifndef FETT263_BATTLE_MODE_ALWAYS_ON
       case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_AUX):
         if (!battle_mode_) {
           battle_mode_ = true;
           hybrid_font.PlayCommon(&SFX_bmbegin);
         } else {
           battle_mode_ = false;
           hybrid_font.PlayCommon(&SFX_bmend);;
         }
         return true;
#endif
		    
      // Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (!swinging_) {
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          auto_lockup_on_ = true;
          SaberBase::DoBeginLockup();
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (!swinging_) {
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
          auto_melt_on_ = true;
          SaberBase::DoBeginLockup();
        }
        return true;

      // Optional Gesture Controls
		    
#ifdef FETT263_SWING_ON
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        // Due to motion chip startup on boot creating false ignition we delay Swing On at boot for 3000ms
        if (millis() > 3000) {
#ifndef FETT263_GESTURE_PREON
        FastOn();
#endif
#ifdef FETT263_GESTURE_PREON
        On();
#endif
#ifndef FETT263_STAB_ON_NO_BM
	battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_TWIST_OFF
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 3000) {
          Off();
          last_twist_ = millis();
          saber_off_time_ = millis();
#ifndef FETT263_BATTLE_MODE_ALWAYS_ON
	  battle_mode_ = false;
#endif
        }
        return true;
#endif

#ifdef FETT263_STAB_ON
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
#ifndef FETT263_GESTURE_PREON
        FastOn();
#endif
#ifdef FETT263_GESTURE_PREON
        On();
#endif
#ifndef FETT263_STAB_ON_NO_BM
        battle_mode_ = true;
#endif
        return true;
#endif

#ifdef FETT263_MULTI_PHASE
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_AUX):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000) {
          last_twist_ = millis();
          next_preset();
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000) {
          last_twist_ = millis();
          previous_preset();
        }
        return true;
#endif

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
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
  bool battle_mode_ = false;
  uint32_t clash_impact_millis_ = millis();
  uint32_t last_twist_ = millis();
  uint32_t saber_off_time_ = millis();
};

#endif
