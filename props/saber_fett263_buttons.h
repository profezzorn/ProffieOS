// Fett263 Buttons with Gesture Controls and Battle Mode with Smart Lockup
//
// Includes Gesture Controls, "Battle Mode" with "Smart Lockup", "Multi-Blast" Mode (to enable Swing Blast control)
// "Multi-Phase" Mode, "On Demand Power Save", "On Demand Battery Level", "Fast On" Ignition (no preon) and SA22C volume menu
//
// DEFAULT 2 BUTTON CONTROLS (PWR and AUX):
//
// "Battle Mode" - hold AUX and Swing while blade is ON to toggle mode ON/OFF
// Ignite (ON) - click PWR while OFF (Swing On, Twist On and Stab On available with defines)
// Muted Ignition (ON) - double click PWR while OFF
// Retract (OFF) - click PWR while ON (disabled during swinging, Twist Off available with define)
// Play/Stop Music Track - hold and release PWR while OFF or hold and release PWR while ON and pointing blade straight up
// Blast - click AUX while ON
// Multi-Blast Mode - hold and release AUX while ON to enter mode, Swing to initiate Blasts, click Aux to exit mode
//                    lockup, clash, stab, melt, drag or any button presses automatically exits mode
// Clash - clash blade while ON
//         in Battle Mode clash and pull away quickly for "Clash" (requires BEGIN_LOCKUP and END_LOCKUP styles)
// Lockup - hold AUX and clash while ON
//          in Battle Mode clash and hold steady to activate, pull away to disengage
// Drag - hold AUX and stab down while ON
//        in Battle Mode stab down, pull away to disengage
// Melt - hold PWR (or AUX) and thrust forward and clash while ON
//        in Battle Mode thrust and clash to engage, pull away to disengage
// Lightning Block - hold PWR and click AUX while ON
// Force - hold and release PWR while ON (not pointing straight up)
// Stab - thrust forward and clash blade while ON - deactivated in Battle Mode
// Power Save - hold Aux and click PWR while ON (pointing up) to use Power Save (requires style)
// Color Change - hold AUX and click PWR while ON (parallel or down) to enter CCWheel,
//                turn hilt to rotate through colors, click PWR to select/exit
//                if using COLOR_CHANGE_DIRECT each button press advances one Color at a time
// Next Preset - click AUX while OFF (parallel or up)
// Previous Preset - click Aux while OFF (pointing down)
// MULTI_PHASE Next Preset - hold AUX and TWIST while ON (use define to enable)
// MULTI_PHASE Previous Preset - hold PWR and TWIST while ON (use define to enable)
// Battery Level - hold AUX and click PWR while OFF (requires style)
// Enter SA22C Volume Menu - hold and release AUX while OFF
// Volume Up (10% increment, 100% max) - click PWR while in Volume Menu while OFF
// Volume Down (10% increment) - click AUX while in Volume Menu while OFF
// Exit Volume Menu - hold and release AUX while in Volume Menu while OFF
//
// OPTIONAL DEFINES (added to CONFIG_TOP in config.h file)
//
// FETT263_BATTLE_MODE_ALWAYS_ON
// Battle Mode is always on, toggle controls deactivated
// This will disable traditional Clash and Stab effects
// (cannot be used with FETT263_BATTLE_MODE_START_ON)
//
// or
//
// FETT263_BATTLE_MODE_START_ON
// Battle Mode is active with each ignition by default but can be toggled using Aux + Swing control
// (cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON)
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
// or
//
// FETT263_SWING_ON_PREON
// Disables Fast On ignition for Swing On so Preon is used (cannot be used with FETT263_SWING_ON)
//
// FETT263_SWING_ON_NO_BM
// To enable Swing On Ignition control but not activate Battle Mode
// (Combine with FETT263_SWING_ON or FETT263_SWING_ON_PREON, 
// cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)
//
// FETT263_SWING_ON_SPEED 250
// Adjust Swing Speed required for Ignition 250 ~ 500 recommended
//
// FETT263_TWIST_OFF
// To enable Twist Off Retraction control
//
// FETT263_TWIST_ON
// To enable Twist On Ignition control (automatically enters Battle Mode, uses Fast On)
//
// or
//
// FETT263_TWIST_ON_PREON
// Disables Fast On ignition for Twist On so Preon is used (cannot be used with FETT263_TWIST_ON)
//
// FETT263_TWIST_ON_NO_BM
// To enable Twist On Ignition control but not activate Battle Mode
// (Combine with FETT263_TWIST_ON or FETT263_TWIST_ON_PREON, 
// cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)
//
// FETT263_STAB_ON
// To enable Stab On Ignition control (automatically enters Battle Mode, uses Fast On)
//
// or
//
// FETT263_STAB_ON_PREON
// Disables Fast On ignition for Stab On so Preon is used (cannot be used with FETT263_STAB_ON)
//
// FETT263_STAB_ON_NO_BM
// To enable Stab On Ignition control but not activate Battle Mode
// (Combine with FETT263_STAB_ON or FETT263_STAB_ON_PREON, 
// cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)
//
// FETT263_THRUST_ON
// To enable Thrust On Ignition control (automatically enters Battle Mode, uses Fast On)
//
// or
//
// FETT263_THRUST_ON_PREON
// Disables Fast On ignition for Thrust On so Preon is used (cannot be used with FETT263_THRUST_ON)
//
// FETT263_THRUST_ON_NO_BM
// To enable Thrust On Ignition control but not activate Battle Mode 
// (Combine with FETT263_THRUST_ON or FETT263_THRUST_ON_PREON, 
// cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)
//
// FETT263_FORCE_PUSH
// To enable gesture controlled Force Push during Battle Mode
// (will use push.wav or force.wav if not present)
//
// FETT263_FORCE_PUSH_ALWAYS_ON
// To enable gesture controlled Force Push full time
// (will use push.wav or force.wav if not present)
//
// FETT263_FORCE_PUSH_LENGTH 5
// Allows for adjustment to Push gesture length in millis needed to trigger Force Push
// Recommended range 1 ~ 10, 1 = shortest, easiest to trigger, 10 = longest
//
// FETT263_MULTI_PHASE
// This will enable a preset change while ON to create a "Multi-Phase" saber effect
//
// MOTION_TIMEOUT 60 * 15 * 1000
// This extends the motion timeout to 15 minutes to allow gesture ignition to remain active
// Increase/decrease the "15" value as needed
//
// CUSTOM SOUNDS SUPPORTED (add to font to enable):
//
// On Demand Power Save - dim.wav
// On Demand Battery Level - battery.wav
// Battle Mode On (on toggle) - bmbegin.wav
// Battle Mode Off (on toggle) - bmend.wav
// Enter Volume Menu - vmbegin.wav
// Exit Volume Menu - vmend.wav
// Force Push - push.wav
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

#ifndef FETT263_FORCE_PUSH_LENGTH
#define FETT263_FORCE_PUSH_LENGTH 5
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_BATTLE_MODE_START_ON)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_BATTLE_MODE_START_ON
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_SWING_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_SWING_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_TWIST_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_TWIST_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_STAB_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_STAB_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_SWING_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_SWING_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_TWIST_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_TWIST_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_STAB_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_STAB_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_THRUST_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_STAB_ON_NO_BM
#endif

#if defined(FETT263_SWING_ON) && defined(FETT263_SWING_ON_PREON)
#error You cannot define both FETT263_SWING_ON and FETT263_SWING_ON_PREON
#endif

#if defined(FETT263_TWIST_ON) && defined(FETT263_TWIST_PREON)
#error You cannot define both FETT263_TWIST_ON and FETT263_TWIST_ON_PREON
#endif

#if defined(FETT263_STAB_ON) && defined(FETT263_STAB_PREON)
#error You cannot define both FETT263_STAB_ON and FETT263_STAB_ON_PREON
#endif

#if defined(FETT263_FORCE_PUSH_ALWAYS_ON) && defined(FETT263_FORCE_PUSH)
#error You cannot define both FETT263_FORCE_PUSH_ALWAYS_ON and FETT263_FORCE_PUSH
#endif

#ifdef FETT263_SWING_ON
#define SWING_GESTURE
#endif

#ifdef FETT263_SWING_ON_PREON
#define SWING_GESTURE
#endif

#if defined(FETT263_SWING_ON_NO_BM) && !defined(SWING_GESTURE)
#error FETT263_SWING_ON_NO_BM requires either FETT263_SWING_ON or FETT263_SWING_ON_PREON
#endif

#ifdef FETT263_STAB_ON
#define STAB_GESTURE
#endif

#ifdef FETT263_STAB_ON_PREON
#define STAB_GESTURE
#endif

#if defined(FETT263_STAB_ON_NO_BM) && !defined(STAB_GESTURE)
#error FETT263_STAB_ON_NO_BM requires either FETT263_STAB_ON or FETT263_STAB_ON_PREON
#endif

#ifdef FETT263_TWIST_ON
#define TWIST_GESTURE
#endif

#ifdef FETT263_TWIST_ON_PREON
#define TWIST_GESTURE
#endif

#if defined(FETT263_TWIST_ON_NO_BM) && !defined(TWIST_GESTURE)
#error FETT263_TWIST_ON_NO_BM requires either FETT263_TWIST_ON or FETT263_TWIST_ON_PREON
#endif

#ifdef FETT263_THRUST_ON
#define THRUST_GESTURE
#endif

#ifdef FETT263_THRUST_ON_PREON
#define THRUST_GESTURE
#endif

#if defined(FETT263_THRUST_ON_NO_BM) && !defined(THRUST_GESTURE)
#error FETT263_THRUST_ON_NO_BM requires either FETT263_THRUST_ON or FETT263_THRUST_ON_PREON
#endif

#ifdef FETT263_FORCE_PUSH_ALWAYS_ON
#define FORCE_PUSH_CONDITION true
#define FETT263_FORCE_PUSH
#else
#define FORCE_PUSH_CONDITION battle_mode_
#endif

#include "prop_base.h"
#include "../sound/hybrid_font.h"

#undef PROP_TYPE
#define PROP_TYPE SaberFett263Buttons

EFFECT(dim); // for EFFECT_POWERSAVE
EFFECT(battery); // for EFFECT_BATTERY_LEVEL
EFFECT(bmbegin); // for Begin Battle Mode
EFFECT(bmend); // for End Battle Mode
EFFECT(vmbegin); // for Begin Volume Menu
EFFECT(vmend); // for End Volume Menu
EFFECT(faston); // for EFFECT_FAST_ON
EFFECT(blstbgn); // for Begin Multi-Blast
EFFECT(blstend); // for End Multi-Blast
EFFECT(push); // for Force Push gesture in Battle Mode

// The Saber class implements the basic states and actions
// for the saber.
class SaberFett263Buttons : public PropBase {
public:
SaberFett263Buttons() : PropBase() {}
  const char* name() override { return "SaberFett263Buttons"; }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    Vec3 mss = fusor.mss();
    if (SaberBase::IsOn()) {
      DetectSwing();
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

      // EVENT_PUSH
      if (fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 70 &&
          fusor.swing_speed() < 30 &&
          fabs(fusor.gyro().x) < 10) {
        if (millis() - push_begin_millis_ > FETT263_FORCE_PUSH_LENGTH) {
          Event(BUTTON_NONE, EVENT_PUSH);
          push_begin_millis_ = millis();
        } 
      } else {
        push_begin_millis_ = millis();
      }
      
    } else {
      // EVENT_SWING - Swing On gesture control to allow fine tuning of speed needed to ignite
      if (millis() - saber_off_time_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (swinging_ && fusor.swing_speed() < 90) {
          swinging_ = false;
        }
        if (!swinging_ && fusor.swing_speed() > FETT263_SWING_ON_SPEED) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
      }
      // EVENT_THRUST
      if (mss.y * mss.y + mss.z * mss.z < 16.0 &&
          mss.x > 14  &&
          fusor.swing_speed() < 150) {
        if (millis() - thrust_begin_millis_ > 15) {
          Event(BUTTON_NONE, EVENT_THRUST);
          thrust_begin_millis_ = millis();
        } 
      } else {
        thrust_begin_millis_ = millis();
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
#ifdef FETT263_BATTLE_MODE_ALWAYS_ON
          battle_mode_ = true;
#endif
#ifdef FETT263_BATTLE_MODE_START_ON
          battle_mode_ = true;
#endif
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
          if (fusor.angle1() < - M_PI / 3) {
            previous_preset();
          } else {
            next_preset();
          }
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
#ifdef FETT263_BM_DISABLE_OFF_BUTTON
          if (!battle_mode_) {
            Off();
            saber_off_time_ = millis();
          }
#else
          Off();
          saber_off_time_ = millis();
          battle_mode_ = false;
#endif
#ifdef FETT263_BATTLE_MODE_ALWAYS_ON
          battle_mode_ = true;
#endif
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        if (fusor.angle1() >  M_PI / 3) {
          StartOrStopTrack();
        } else {
          SaberBase::DoForce();
        }
        return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_DOUBLE_CLICK, MODE_ON):
        // Avoid the base and the very tip.
        // TODO: Make blast only appear on one blade!
        if(swing_blast_) {
          swing_blast_ = false;
          if (SFX_blstend) {
            hybrid_font.PlayCommon(&SFX_blstend);
          } else {
            SaberBase::DoBlast();
          }
          return true;
        } else {
          SaberBase::DoBlast();
        }
        return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_ON):
        swing_blast_ = true;
        if (SFX_blstbgn) {
          hybrid_font.PlayCommon(&SFX_blstbgn);
        } else {
          hybrid_font.SB_Effect(EFFECT_BLAST, 0);
        }
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
          if (SFX_vmend) {
            hybrid_font.PlayCommon(&SFX_vmend);
          } else {
            beeper.Beep(0.5, 3000);
          }
          STDOUT.println("Exit Volume Menu");
        } else {
          mode_volume_ = true;
          if (SFX_vmbegin) {
            hybrid_font.PlayCommon(&SFX_vmbegin);
          } else {
            beeper.Beep(0.5, 3000);
          }
          STDOUT.println("Enter Volume Menu");
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

       // Battle Mode

#ifndef FETT263_BATTLE_MODE_ALWAYS_ON
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_AUX):
        if (!battle_mode_) {
          battle_mode_ = true;
          if (SFX_bmbegin) {
            hybrid_font.PlayCommon(&SFX_bmbegin);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
        } else {
          battle_mode_ = false;
          if (SFX_bmend) {
            hybrid_font.PlayCommon(&SFX_bmend);
          } else {
            beeper.Beep(0.5, 3000);
          }
        }
        return true;
#endif

      // Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (swinging_) return false;
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        auto_lockup_on_ = true;
        SaberBase::DoBeginLockup();
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

      // Optional Gesture Controls (defines listed at top)

#ifdef FETT263_SWING_ON
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        // Due to motion chip startup on boot creating false ignition we delay Swing On at boot for 3000ms
        if (millis() > 3000) {
          FastOn();
#ifndef FETT263_SWING_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_SWING_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        // Due to motion chip startup on boot creating false ignition we delay Swing On at boot for 3000ms
        if (millis() > 3000) {
          On();
#ifndef FETT263_SWING_ON_NO_BM
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

#ifdef FETT263_TWIST_ON
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000 &&
            millis() - saber_off_time_ > 1000) {
          FastOn();
#ifndef FETT263_TWIST_ON_NO_BM
          battle_mode_ = true;
#endif
          last_twist_ = millis();
        }
        return true;
#endif

#ifdef FETT263_TWIST_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000 &&
            millis() - saber_off_time_ > 1000) {
          On();
#ifndef FETT263_TWIST_ON_NO_BM
          battle_mode_ = true;
#endif
          last_twist_ = millis();
        }
        return true;
#endif

#ifdef FETT263_STAB_ON
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        if (millis() - saber_off_time_ > 1000) {
          FastOn();
#ifndef FETT263_STAB_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_STAB_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        if (millis() - saber_off_time_ > 1000) {
          On();
#ifndef FETT263_STAB_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_THRUST_ON
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        if (millis() - saber_off_time_ > 1000) {
          FastOn();
#ifndef FETT263_THRUST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_THRUST_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        if (millis() - saber_off_time_ > 1000) {
          On();
#ifndef FETT263_THRUST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_FORCE_PUSH
      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
        if (FORCE_PUSH_CONDITION &&
            millis() - last_push_ > 2000) {
          if (SFX_push) {
            hybrid_font.PlayCommon(&SFX_push);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
          last_push_ = millis();
        }
        return true;

#endif

#ifdef FETT263_MULTI_PHASE
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_AUX):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000) {
          last_twist_ = millis();
          Off();
          next_preset();
          FastOn();
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000) {
          last_twist_ = millis();
          Off();
          previous_preset();
          FastOn();
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

  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      case EFFECT_POWERSAVE:
        if (SFX_dim) {
          hybrid_font.PlayCommon(&SFX_dim);
        } else {
          beeper.Beep(0.5, 3000);
        }
        return;
      case EFFECT_BATTERY_LEVEL:
        if (SFX_battery) {
          hybrid_font.PlayCommon(&SFX_battery);
        } else {
          beeper.Beep(0.5, 3000);
        }
        return;
      case EFFECT_FAST_ON:
        if (SFX_faston) {
          hybrid_font.PlayCommon(&SFX_faston);
        }
        return;
    }
  }

private:
  bool pointing_down_ = false;
  bool swing_blast_ = false;
  bool mode_volume_ = false;
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
  bool battle_mode_ = false;
  uint32_t thrust_begin_millis_ = millis();
  uint32_t push_begin_millis_ = millis();
  uint32_t clash_impact_millis_ = millis();
  uint32_t last_twist_ = millis();
  uint32_t last_push_ = millis();
  uint32_t saber_off_time_ = millis();
};

#endif
