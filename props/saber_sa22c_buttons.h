// sa22c props file, includes 1,2 and 3 button modes.  Incorporates multi-blast,
// battle mode and gesture ignitions from fett263 plus  on-the-fly volume
// controls and full access to all features with 1,2 or 3 button sabers
//
// New #define SA22C_NO_LOCKUP_HOLD
// reverts to lockup being triggered only by clash + aux in 2-button mode
// Also sets multi-blast to trigger while holding aux and swinging, rather than
// double click and hold
//
// Gesture Controls
// There are four gesture types: swing, stab, twist and thrust.  For simplicity,
// using gesture ignition will automatically skip the preon effect.
// Below are the options to add to the config to enable
// the various gestures
//
// #define SA22C_STAB_ON
// #define SA22C_SWING_ON
// #define SA22C_TWIST_ON
// #define SA22C_THRUST_ON
// #define SA22C_TWIST_OFF
// #define SA22C_FORCE_PUSH
//
// #define SA22C_FORCE_PUSH_LENGTH 5
// Allows for adjustment to Push gesture length in millis needed to trigger Force Push
// Recommended range 1 ~ 10, 1 = shortest, easiest to trigger, 10 = longest
//
// If you want the gesture ignition to ALSO enter battle mode automatically
// on ignition, add this define
//
// #define GESTURE_AUTO_BATTLE_MODE
//
// Battle mode by fett263
//
// Once you enter battle mode, button functions will be disabled for lockup
// stab, melt, etc.  Blaster blocks and lightning block will continue to be
// triggered by button controls.  Automatic lockup/clash detection works
// by measuring delay of the saber blade pulling back from the clash.
// If you clash the blade and it does not pull back during the delay period,
// it is assumed to be a lockup and the lockup effect will show on the blade.
// If you clash the blade and pull away, only the bgn/end lockup effects will show.
//
// You can adjust the threshold of this detection by using
// #define SA22C_LOCKUP_DELAY  (insert number here)
// Default value is 200
//
// Battle mode features automatic clash and lockup detection plus a new
// force push mode that will play a force or force push sound with a controlled
// pushing gesture.  Automatic clash/lockup uses the pre and post lock effects
// so your blade style and font MUST have those capabilities to support
// battle mode.  Kudos to fett263 for his very impressive additions for OS 5
//
// Tightened click timings
// I've shortened the timeout for short and double click detection from 500ms
// to 300ms.  I think it feels more responsive this way but still gives enough
// timeout to ensure all button actions can be achieved consistently
// I've included all button timings so they can be easily tweaked to suit
// individual tastes.
//
// Button configs:
//
// 1 Button:
// Activate Muted - double click and hold while OFF
// Activate - short click while OFF
// Play/Stop Music - double click while OFF
// Turn off blade - hold and wait till blade is off while ON
// Next Preset - hold and release while OFF
// Prev Preset - hold and wait while OFF
// Lockup - hold + hit clash while ON
// Stab - thrust forward clash while ON
// Lightning Block - double click and hold while ON
// Melt - hold + thust forward clash while ON
// Drag - hold + hit clash while ON pointing the blade tip down
// Blaster Blocks - short click/double click/triple click while on
// Multi-Blast - hold while swinging for one second and release
//               to trigger blaster block, swing saber while in multi-blast mode
//               to exit, hold while swinging for one second and release
// Battle Mode - triple-click and hold while on
// Force Effects - hold + twist the hilt while ON (while pointing up)
// Color Change mode - hold + twist the hilt while ON (pointing down)
// Enter Volume - Menu hold + clash while OFF
// Volume UP - hold and release while in Volume Menu
// Volume DOWN - click while in Volume Menu
// Exit Volume Menu - Menu hold + clash while OFF
// Battery Level - triple click while OFF
//
//
// 2 Button:
// POWER
// Activate Muted - double click and hold while OFF
// Activate - short click while OFF
// Play/Stop Music - hold and release while OFF
// Turn off blade - hold and wait till blade is off while ON
// Force Effects - double click while ON
// Volume UP - short click while OFF and in VOLUME MENU
// Prev Preset - hold and wait while OFF
// Color Change mode - hold + toggle AUX while ON
// Lightning Block - double click and hold while ON
// Melt - hold while stabbing (clash with forward motion, horizontal)
// Battle Mode - triple-click and hold for half a second while on
// AUX
// Blaster blocks - short click/double click/triple click while ON
// Multi-Blast - double-click and hold for half a second
//               to trigger blaster block, swing saber while in multi-blast mode
//               to exit, double-click and hold for half a second
// Next Preset - short click while OFF
// Lockup - hold while ON
// Drag - hold while ON pointing the blade tip down
// Enter VOLUME MENU - long click while OFF
// Volume down - short click while OFF and in VOLUME MENU
// Battery level - hold while off
//
// 3 Button: Same as two button except for the following
//
// AUX2
// Lightning Block - hold while ON
// Battle Mode - double-click and hold while on
// Previous Preset - short click while OFF


#ifndef PROPS_SABER_SA22C_BUTTONS_H
#define PROPS_SABER_SA22C_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"

#undef PROP_TYPE
#define PROP_TYPE SaberSA22CButtons

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 15 * 1000
#endif

#ifndef SA22C_SWING_ON_SPEED
#define SA22C_SWING_ON_SPEED 250
#endif

#ifndef SA22C_LOCKUP_DELAY
#define SA22C_LOCKUP_DELAY 200
#endif

#ifndef SA22C_FORCE_PUSH_LENGTH
#define SA22C_FORCE_PUSH_LENGTH 5
#endif

#ifndef BUTTON_DOUBLE_CLICK_TIMEOUT
#define BUTTON_DOUBLE_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_SHORT_CLICK_TIMEOUT
#define BUTTON_SHORT_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_TIMEOUT
#define BUTTON_HELD_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_MEDIUM_TIMEOUT
#define BUTTON_HELD_MEDIUM_TIMEOUT 1000
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif

#ifdef SA22C_SWING_ON
#define SWING_GESTURE
#endif

#ifdef SA22C_STAB_ON
#define STAB_GESTURE
#endif

#ifdef SA22C_TWIST_ON
#define TWIST_GESTURE
#endif

#ifdef SA22C_THRUST_ON
#define THRUST_GESTURE
#endif

#define FORCE_PUSH_CONDITION battle_mode_

EFFECT(dim);      // for EFFECT_POWERSAVE
EFFECT(battery);  // for EFFECT_BATTERY_LEVEL
EFFECT(bmbegin);  // for Begin Battle Mode
EFFECT(bmend);    // for End Battle Mode
EFFECT(vmbegin);  // for Begin Volume Menu
EFFECT(vmend);    // for End Volume Menu
EFFECT(volup);    // for increse volume
EFFECT(voldown);  // for decrease volume
EFFECT(volmin);   // for minimum volume reached
EFFECT(volmax);   // for maximum volume reached
EFFECT(faston);   // for EFFECT_FAST_ON
EFFECT(blstbgn);  // for Begin Multi-Blast
EFFECT(blstend);  // for End Multi-Blast
EFFECT(push);     // for Force Push gesture in Battle Mode

// The Saber class implements the basic states and actions
// for the saber.
class SaberSA22CButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  SaberSA22CButtons() : PropBase() {}
  const char* name() override { return "SaberSA22CButtons"; }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    Vec3 mss = fusor.mss();
    if (SaberBase::IsOn()) {
      DetectSwing();
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > SA22C_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > SA22C_LOCKUP_DELAY &&
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
        if (millis() - push_begin_millis_ > SA22C_FORCE_PUSH_LENGTH) {
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
        if (!swinging_ && fusor.swing_speed() > SA22C_SWING_ON_SPEED) {
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

// Volume Menu
  void VolumeUp() {
    STDOUT.println("Volume up");
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1,
        dynamic_mixer.get_volume() + VOLUME * 0.10));
      if (SFX_volup) {
        hybrid_font.PlayCommon(&SFX_volup);
      } else {
        beeper.Beep(0.5, 2000);
      }
      STDOUT.print("Volume Up - Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      // Cycle through ends of Volume Menu option
      #ifdef VOLUME_MENU_CYCLE
        if (!max_vol_reached_) {
          if (SFX_volmax) {
            hybrid_font.PlayCommon(&SFX_volmax);
          } else {
            beeper.Beep(0.5, 3000);
          }
          STDOUT.print("Maximum Volume: ");
          max_vol_reached_ = true;
        } else {
          dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
          dynamic_mixer.get_volume() - VOLUME * 0.90));
          if (SFX_volmin) {
            hybrid_font.PlayCommon(&SFX_volmin);
          } else {
            beeper.Beep(0.5, 1000);
          }
          STDOUT.print("Minimum Volume: ");
          max_vol_reached_ = false;
        }
      #else
        if (SFX_volmax) {
          hybrid_font.PlayCommon(&SFX_volmax);
        } else {
          beeper.Beep(0.5, 3000);
        }
        STDOUT.print("Maximum Volume: ");
      #endif
    }
  }

  void VolumeDown() {
    STDOUT.println("Volume Down");
    if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
        dynamic_mixer.get_volume() - VOLUME * 0.10));
      if (SFX_voldown) {
        hybrid_font.PlayCommon(&SFX_voldown);
      } else {
        beeper.Beep(0.5, 2000);
      }
      STDOUT.print("Volume Down - Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      #ifdef VOLUME_MENU_CYCLE
        if (!min_vol_reached_) {
          if (SFX_volmin) {
            hybrid_font.PlayCommon(&SFX_volmin);
          } else {
            beeper.Beep(0.5, 1000);
          }
          STDOUT.print("Minimum Volume: ");
          min_vol_reached_ = true;
        } else {
          dynamic_mixer.set_volume(VOLUME);
          if (SFX_volmax) {
            hybrid_font.PlayCommon(&SFX_volmax);
          } else {
            beeper.Beep(0.5, 3000);
          }
          STDOUT.print("Maximum Volume: ");
          min_vol_reached_ = false;
        }
      #else
        if (SFX_volmin) {
          hybrid_font.PlayCommon(&SFX_volmin);
        } else {
          beeper.Beep(0.5, 1000);
        }
        STDOUT.print("Minimum Volume: ");
      #endif
      
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

// Battle Mode
#if NUM_BUTTONS == 3
  case EVENTID(BUTTON_AUX2, EVENT_SECOND_HELD, MODE_ON):
#else
  case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD, MODE_ON):
#endif
    if (!battle_mode_) {
      STDOUT.println("Entering Battle Mode");
      battle_mode_ = true;
      if (SFX_bmbegin) {
        hybrid_font.PlayCommon(&SFX_bmbegin);
      } else {
        hybrid_font.DoEffect(EFFECT_FORCE, 0);
      }
    } else {
      STDOUT.println("Exiting Battle Mode");
      battle_mode_ = false;
      if (SFX_bmend) {
        hybrid_font.PlayCommon(&SFX_bmend);
      } else {
        beeper.Beep(0.5, 3000);
      }
    }
    return true;

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

  // Gesture Controls
#ifdef SA22C_SWING_ON
  case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
    // Due to motion chip startup on boot creating false ignition we delay Swing On at boot for 3000ms
    if (millis() > 3000) {
      FastOn();
#ifdef GESTURE_AUTO_BATTLE_MODE
      STDOUT.println("Entering Battle Mode");
      battle_mode_ = true;
#endif
    }
    return true;
#endif

#ifdef SA22C_TWIST_ON
  case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
    // Delay twist events to prevent false trigger from over twisting
    if (millis() - last_twist_ > 2000 &&
        millis() - saber_off_time_ > 1000) {
      FastOn();
#ifdef GESTURE_AUTO_BATTLE_MODE
      STDOUT.println("Entering Battle Mode");
      battle_mode_ = true;
#endif
      last_twist_ = millis();
    }
    return true;
#endif

#ifdef SA22C_TWIST_OFF
  case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
    // Delay twist events to prevent false trigger from over twisting
    if (millis() - last_twist_ > 3000) {
      Off();
      last_twist_ = millis();
      saber_off_time_ = millis();
      battle_mode_ = false;
    }
    return true;
#endif

#ifdef SA22C_STAB_ON
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        if (millis() - saber_off_time_ > 1000) {
          FastOn();
#ifdef GESTURE_AUTO_BATTLE_MODE
          STDOUT.println("Entering Battle Mode");
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef SA22C_THRUST_ON
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        if (millis() - saber_off_time_ > 1000) {
          FastOn();
#ifdef GESTURE_AUTO_BATTLE_MODE
          STDOUT.println("Entering Battle Mode");
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef SA22C_FORCE_PUSH
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

// Saber ON AND Volume Up
  case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
    if (!mode_volume_) {
      On();
    } else {
      VolumeUp();
    }
    return true;

  case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
  // 1-button: Next Preset AND Volume Down
#if NUM_BUTTONS == 1
    if (!mode_volume_) {
      next_preset();
    } else {
      VolumeDown();
    }
    return true;
#else
  // 2 and 3 button: Start or Stop Track
    StartOrStopTrack();
    return true;
#endif

  // 2 and 3 button: Next Preset and Volume Down
  case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT, MODE_OFF):
    if (!mode_volume_) {
      next_preset();
    } else {
      VolumeDown();
    }
    return true;

#if NUM_BUTTONS == 3
  // 3 button: Previous Preset
  case EVENTID(BUTTON_AUX2, EVENT_FIRST_CLICK_SHORT, MODE_OFF):
#else
   // 1 and 2 button: Previous Preset
  case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF):
#endif
    if (!mode_volume_) {
      previous_preset();
    }
    return true;

// Activate Muted
  case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_OFF):
    if (SetMute(true)) {
      unmute_on_deactivation_ = true;
      On();
    }
    return true;

// Turn Blade OFF
#if NUM_BUTTONS > 1
// 2 and 3 button
  case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
#else
// 1 button
  case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_ON):
#endif
    if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
        // Just exit color change mode.
        // Don't turn saber off.
        ToggleColorChangeMode();
        return true;
      }
#endif
      if (!battle_mode_) {
        Off();
      }
    }
    saber_off_time_ = millis();
    battle_mode_ = false;
    swing_blast_ = false;
    return true;

// 1 button Force and Color Change mode
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
#else
  // 2 and 3 button Force effect
  case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
    SaberBase::DoForce();
    return true;
  // 2 and 3 button color change
#ifndef DISABLE_COLOR_CHANGE
  case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT, MODE_ON | BUTTON_POWER):
    ToggleColorChangeMode();
    return true;
#endif
#endif

// Blaster Deflection
#if NUM_BUTTONS == 1
  // 1 button
  case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
  case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
  case EVENTID(BUTTON_POWER, EVENT_THIRD_CLICK_SHORT, MODE_ON):
#else
  // 2 and 3 button
  case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
  case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
  case EVENTID(BUTTON_AUX, EVENT_THIRD_CLICK_SHORT, MODE_ON):
#endif
    swing_blast_ = false;
    SaberBase::DoBlast();
    return true;

// Multi-Blaster Deflection mode
#if NUM_BUTTONS == 1
  // 1 button
  case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_POWER):
#else
  // 2 and 3 button
#ifndef SA22C_NO_LOCKUP_HOLD
  case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD, MODE_ON):
#else
  // in SA22C_NO_LOCKUP_HOLD mode, multi-blaster is triggered by holding aux
  // while swinging
  case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_AUX):
#endif
#endif
    swing_blast_ = !swing_blast_;
    if (swing_blast_) {
      if (SFX_blstbgn) {
        hybrid_font.PlayCommon(&SFX_blstbgn);
      } else {
        hybrid_font.SB_Effect(EFFECT_BLAST, 0);
      }
    } else {
      if (SFX_blstend) {
        hybrid_font.PlayCommon(&SFX_blstend);
      } else {
        hybrid_font.SB_Effect(EFFECT_BLAST, 0);
      }
    }
    return true;

  case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
    if (swing_blast_) {
      SaberBase::DoBlast();
    }
    return true;

// Lockup
#if NUM_BUTTONS == 1
  // 1 button lockup
  case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
#else
#ifndef SA22C_NO_LOCKUP_HOLD
  // 2 and 3 button lockup
  case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD, MODE_ON):
#else
  case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
#endif
#endif
    if (!SaberBase::Lockup() && !battle_mode_) {
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

// Lightning Block
#if NUM_BUTTONS < 3
  // 1 and 2 button
  case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_ON):
#else
  // 3 button
  case EVENTID(BUTTON_AUX2, EVENT_FIRST_HELD, MODE_ON):
#endif
    if (!battle_mode_) {
      SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
      swing_blast_ = false;
      SaberBase::DoBeginLockup();
      return true;
    }
    break;

// Melt
  case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
    if (!SaberBase::Lockup() && !battle_mode_) {
      SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
      swing_blast_ = false;
      SaberBase::DoBeginLockup();
      return true;
    }
    break;

// Start or Stop Track
#if NUM_BUTTONS == 1
  // 1 button
  case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
    StartOrStopTrack();
    return true;
#endif

  case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
  case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_OFF):
  case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_OFF):
    SaberBase::RequestMotion();
    return true;

// Enter Volume MENU
#if NUM_BUTTONS == 1
  // 1 button
  case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
#else
  // 2 and 3 button
  case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
#endif
    if (!mode_volume_) {
      mode_volume_ = true;
      if (SFX_vmbegin) {
        hybrid_font.PlayCommon(&SFX_vmbegin);
      } else {
        beeper.Beep(0.5, 3000);
      }
      STDOUT.println("Enter Volume Menu");
    } else {
      mode_volume_ = false;
      if (SFX_vmend) {
        hybrid_font.PlayCommon(&SFX_vmend);
      } else {
        beeper.Beep(0.5, 3000);
      }
      STDOUT.println("Exit Volume Menu");
    }
    return true;

// Battery level
#if NUM_BUTTONS == 1
  // 1 button
  case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
#else
  // 2 and 3 button
  case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_OFF):
#endif
    talkie.SayDigit((int)floorf(battery_monitor.battery()));
    talkie.Say(spPOINT);
    talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
    talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
    talkie.Say(spVOLTS);
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

      default: break; // avoids compiler warning
    }
  }

private:
  bool pointing_down_ = false;
  bool swing_blast_ = false;
  bool mode_volume_ = false;
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
  bool battle_mode_ = false;
  bool max_vol_reached_ = false;
  bool min_vol_reached_ = false;
  uint32_t thrust_begin_millis_ = millis();
  uint32_t push_begin_millis_ = millis();
  uint32_t clash_impact_millis_ = millis();
  uint32_t last_twist_ = millis();
  uint32_t last_push_ = millis();
  uint32_t saber_off_time_ = millis();
};

#endif
