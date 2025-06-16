// DetonatorOliButtons Revision 14

/* Created by OlivierFlying747-8, based on detonator.h by Fredrik Hubinette
  https://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2025 Fredrik Hubinette
  Copyright (c) 2025 OlivierFlying747-8 with contributions by:
  Fredrik Hubinette aka profezzorn and Brian Conner aka NoSloppy
  In case of problems, you can find us at: https://crucible.hubbe.net where somebody will be there to help.
  Distributed under the terms of the GNU General Public License v3.
  https://www.gnu.org/licenses/

Welcome to my version of the detonator. I modified the code from detonator.h to make it function as closely as possible to
what I can see in Episode VI, with additional modes of operation as seen in the Clone Wars & Rebels.

I added some functionality (volume up/down, clash will make it detonate, you can set a timer via #define DETONATOR_TIMER_DURATION
and a stealth timer via #define DETONATOR_STEALTH_TIMER_DURATION, battery level).

You can arm/disarm it or arm and make it detonate (via clash for immediate detonation or via timer/stealth timer for
delayed detonation).

If you want to use "stealth timer", your sound font should be setup for Alt Sounds with two different armhum.wav & hum.wav
Option 1: You can engage stealth timer while alt000/hum.wav is playing, you will then hear alt001/hum.wav (for a very silent timer).
Option 2: You can engage stealth timer while alt000/armhum.wav is playing, you will then hear alt001/armhum.wav (for a "not so silent" timer).

Once a timer is active, it can't be turned off and will go boom. Once armed and/or a timer is active, clash will make it go boom.

It has also been modified to be used with non-latching buttons so it is compatible with my multi_prop. However, if you want to
use it as stand-alone on your physical detonator prop with a latching POWER_BUTTON, you can use "#define BUTTON_POWER_IS_LATCH"
in your config.

Your prop NEEDS to have one or more buttons.

This detonator version uses pow & aux buttons (unlike detonator.h which uses pow & aux2).

This detonator prop was created to be compatible with ProffieOS v8.x

Includes 1 and 2 button controls.

============= LIST OF BUTTON & MOTION COMMANDS: =================================================================================
1 Button:
SWING while OFF : Turn ON (not armed)
SWING while ON  : Turn OFF (disarm if armed, will stop armhum.wav & plays endarm.wav if armhum.wav was playing)
                  However, if a countdown was started, it will continue until boom.wav

POWER Button:
  - Short click while ON (not in volume menu):
      - If disarmed → Arm (plays bgnarm.wav followed by armhum.wav)
      - If armed    → Start 6s countdown to boom (continues armhum.wav)
  - Long click while ON (before countdown): Start 12s silent "stealth" timer (only initial beep, blade stays on)
  - Double click (ON or OFF) : Start/Stop track
  - Short click and hold (ON or OFF) : Enter/Exit Volume Menu
  - Short click (while in Volume Menu):
      - Pointing UP   → Volume Up
      - Pointing DOWN → Volume Down
      - Level         → No action
  - Short click while OFF (not in Volume Menu):
      - Pointing UP   → Next Preset
      - Pointing DOWN → Previous Preset
      - Level         → Jumped to first preset
  - Triple click while OFF:
      - Pointing UP   → Battery Level in %
      - Pointing DOWN → Battery Level in Volts
      - Level         → Battery Level in % & Volts

CLASH (while ON and armed or while ON and stealth timer running option 1 or 2):
  - Instantly trigger boom (interrupts any countdown), resets everything, turns the detonator OFF.
=================================================================================================================================
2 Buttons:

POWER Button:
  - Short click while OFF : Turn ON (not armed)
  - Short click while ON  : Turn OFF (disarm if armed, will stop armhum.wav & plays endarm.wav if armhum.wav was playing)
                            However, if a countdown was started, it will continue until boom.wav
OR if your POWER Button is Latching:
  - Latch ON  : Turn ON (not armed)
  - Latch OFF : Turn OFF (disarm if armed, will stop armhum.wav & plays endarm.wav if armhum.wav was playing)
                However, if a countdown was started, it will continue until boom.wav

AUX Button:
  - Short click while ON (not in volume menu):
      - If disarmed → Arm (plays bgnarm.wav followed by armhum.wav)
      - If armed    → Start 6s countdown to boom (continues armhum.wav)
  - Long click while ON (before countdown): Start 12s silent "stealth" timer (only initial beep, blade stays on)
  - Double click (ON or OFF) : Start/Stop track
  - Short click and hold (ON or OFF) : Enter/Exit Volume Menu
  - Short click (while in Volume Menu):
      - Pointing UP   → Volume Up
      - Pointing DOWN → Volume Down
      - Level         → No action
  - Short click while OFF (not in Volume Menu):
      - Pointing UP   → Next Preset
      - Pointing DOWN → Previous Preset
      - Level         → Jumped to first preset
  - Triple click while OFF:
      - Pointing UP   → Battery Level in %
      - Pointing DOWN → Battery Level in Volts
      - Level         → Battery Level in % & Volts

CLASH (while ON and armed or while ON and stealth timer running option1 or 2):
  - Instantly trigger boom (interrupts any countdown), resets everything, turns the detonator OFF.

============= LIST OF .wav USED in this detonator: ==============================================================================
This prop version REQUIRES a ProffieOS Voicepack V1 or V2 for some of the sounds to work.
Typically, that is a folder named "common" on the root level of the SD card.

Download your choice of language and variation here:
https://fredrik.hubbe.net/lightsaber/sound/
Also, a large variety of FREE in-universe character Voicepacks available here:
https://crucible.hubbe.net/t/additional-voicepacks/4227
If you'd care to make a donation for Brian Conner's time making these Voicepacks:
https://www.buymeacoffee.com/brianconner

Your sound font should contain the below listed files to use detonator_Oli_buttons.h to it's full potential:

alt000/armhum.wav (for regular timer)
alt001/armhum.wav (for stealth timer option2)
bgnarm.wav
boom.wav
endarm.wav
alt000/hum.wav (for regular ON but not armed)
alt001/hum.wav (for stealth timer option1)
(if you don't want to hear hum.wav, make it a silent hum or you will get error in font directory!)

Optional .wav files:
====================
boot.wav
font.wav

============= TO DO: ============================================================================================================
    Quote(s) from C3PO "He agrees !"
    EFFECT_TIMER for visual countdown on OLED
    Dim when ON, Bright when armed. <-- to do in blade style

============= List of optional detonator define that you can add to your config. ========================================================
#define DETONATOR_BUTTON_POWER_IS_LATCH        // If your detonator pow button is latching and has been defined as such in your config.
#define DETONATOR_TIMER_DURATION 6.0f          // default is 6 seconds during "loud timer" delayed detonation.  (set timing in seconds)
#define DETONATOR_STEALTH_TIMER_DURATION 12.0f // default is 12 seconds during "quiet timer" delayed detonation.  (set timing in seconds)
#define DETONATOR_TWIST                        // If you want twist ON/OFF on your two buttons setup.
#define DETONATOR_AUTO_ON                      // If you want the detonator to turn ON automatically when you land on it with
                                               // multi_prop or when you change to next/prev/1st preset.
#define VOLUME_MENU_CYCLE                      // Cycle through ends of Volume Menu option (same as in saber_sa22c_buttons.h)

=================================================================================================================================
Please feel free to submit more fun ideas on The Crucible or to this GitHub page:
https://github.com/olivierflying747-8/Olis-ProffieOS/tree/06_props_Oli_buttons_SUGGESTIONS_WELCOME

=================================================================================================================================

Thank you for reading!
======================
I hope you will have fun with my version of a detonator.
*/

#ifndef PROPS_DETONATOR_OLI_BUTTONS_H
#define PROPS_DETONATOR_OLI_BUTTONS_H

#if NUM_BUTTONS < 1
#error detonator_oli_buttons requires 1 or 2 Buttons for operation
#endif

#include "prop_base.h"
#include "../sound/sound_library.h"

#ifndef DETONATOR_TIMER_DURATION
#define DETONATOR_TIMER_DURATION 6.0f // default is 6 seconds (set timing in seconds)
#endif

#ifndef DETONATOR_STEALTH_TIMER_DURATION
#define DETONATOR_STEALTH_TIMER_DURATION 12.0f // default is 12 seconds (set timing in seconds)
#endif

#if NUM_BUTTONS == 1
#ifndef DETONATOR_TWIST
#define DETONATOR_TWIST
#endif
#endif

#ifdef DETONATOR_TWIST
#ifndef TWIST_GESTURE
#define TWIST_GESTURE
#endif
#endif

#define PROP_TYPE DetonatorOliButtons

// ==============================================================================================================================

#ifdef BC_VOLUME_MENU
// Based on BlasterBC:
template<class SPEC>
struct DetonatorOliVolumeMode : public SPEC::SteppedMode {
  const int max_volume_ = VOLUME;
  const int min_volume_ = VOLUME * 0.10;
  float initial_volume_ = 0.0;
  int initial_percentage_ = 0;
  int percentage_ = 0;

  void mode_activate(bool onreturn) override {
    PVLOG_DEBUG << "** Enter Volume Menu\n";
    initial_volume_ = dynamic_mixer.get_volume();
    initial_percentage_ = round((initial_volume_ / max_volume_) * 10) * 10;
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    mode::getSL<SPEC>()->SayEditVolume();
    announce_volume();
    SPEC::SteppedMode::mode_activate(onreturn);
  }

  void announce_volume() {
    if (percentage_ <= 10) {
      mode::getSL<SPEC>()->SayMinimumVolume();
    } else if (percentage_ >=100) {
      mode::getSL<SPEC>()->SayMaximumVolume();
    } else {
      mode::getSL<SPEC>()->SayWhole(percentage_);
      mode::getSL<SPEC>()->SayPercent();
    }
  }

  void mode_deactivate() override {
    announce_volume();
    mode::getSL<SPEC>()->SayVolumeMenuEnd();
    SPEC::SteppedMode::mode_deactivate();
  }

  void next() override {
    int current_volume_ = dynamic_mixer.get_volume();
    if (current_volume_ < max_volume_) {
      current_volume_ += max_volume_ * 0.10;
      if (current_volume_ >= max_volume_) {
        current_volume_ = max_volume_;
        QuickMaxVolume();
      } else {
        mode::getSL<SPEC>()->SayVolumeUp();
      }
      dynamic_mixer.set_volume(current_volume_);
    }
  }

  void prev() override {
    int current_volume_ = dynamic_mixer.get_volume();
    if (current_volume_ > min_volume_) {
      current_volume_ -= max_volume_ * 0.10;
      if (current_volume_ <= min_volume_) {
        current_volume_ = min_volume_;
        QuickMinVolume();
      } else {
        mode::getSL<SPEC>()->SayVolumeDown();
      }
      dynamic_mixer.set_volume(current_volume_);
    }
  }

  void QuickMaxVolume() {
    dynamic_mixer.set_volume(max_volume_);
    PVLOG_DEBUG << "** Maximum Volume\n";
    mode::getSL<SPEC>()->SayMaximumVolume();
  }

  void QuickMinVolume() {
    dynamic_mixer.set_volume(min_volume_);
    PVLOG_DEBUG << "** Minimum Volume\n";
    mode::getSL<SPEC>()->SayMinimumVolume();
  }

  void update() override {  // Overridden to substitute the tick sound
    float volume = dynamic_mixer.get_volume();
    percentage_ = round((volume / max_volume_) * 10) * 10;
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
  }

  void select() override {
    PVLOG_DEBUG << "** Saved - Exiting Volume Menu\n";
    mode::getSL<SPEC>()->SaySave();
    SPEC::SteppedMode::select();
  }

  void exit() override {
    PVLOG_DEBUG << "** Cancelled - Exiting Volume Menu\n";
    percentage_ = initial_percentage_;
    dynamic_mixer.set_volume(initial_volume_);
    mode::getSL<SPEC>()->SayCancel();
    SPEC::SteppedMode::exit();
  }

  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, 0)) {
      // Custom button controls for BCVolumeMode
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, 0):
        QuickMaxVolume();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, 0):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, 0):
        QuickMinVolume();
        return true;
    }
    // Use the select and exit controls from SelectCancelMode
    return SPEC::SelectCancelMode::mode_Event2(button, event, modifiers);
  }
};

template<class SPEC>
struct DetonatorOliMenuSpec {
  typedef DetonatorOliVolumeMode<SPEC> DetonatorOliVolumeMenu;
  typedef mode::SelectCancelMode SelectCancelMode;
  typedef mode::SteppedMode<SPEC> SteppedMode;
  typedef mode::SteppedModeBase<SPEC> SteppedModeBase;
  typedef mode::MenuBase<SPEC> MenuBase;
  typedef SoundLibraryV2 SoundLibrary;
};
#endif
// ==============================================================================================================================

class DetonatorOliButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  DetonatorOliButtons() : PropBase() {}
  const char* name() override { return "DetonatorOliButtons"; }

  enum NextAction {
    NEXT_ACTION_NOTHING,
    NEXT_ACTION_ARM,
    NEXT_ACTION_BLOW,
  };

  void SetPower(bool on) {}

  void SetNextAction(NextAction what, float when_sec) {
    time_base_ = millis();
    next_event_time_ = when_sec * 1000;
    next_action_ = what;
    timer_active_ = (what == NEXT_ACTION_BLOW);
  }

  void PollNextAction() {
    if (millis() - time_base_ > next_event_time_) {
      switch (next_action_) {
        case NEXT_ACTION_NOTHING:
          break;
        case NEXT_ACTION_ARM:
          armed_ = true;
          break;
        case NEXT_ACTION_BLOW:
          SaberBase::DoEffect(EFFECT_BOOM, 0);
          Off(OFF_BLAST);
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          if (stealth_mode_) {
            stealth_mode_ = false;
            SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, 0);
          }
          armed_ = false;
          timer_active_ = false;
          break;
      }
      next_action_ = NEXT_ACTION_NOTHING;
    }
  }

  void BeginArm() {
    SaberBase::SetLockup(SaberBase::LOCKUP_ARMED);
    SaberBase::DoBeginLockup();
    SaberBase::DoEffect(EFFECT_BGNARM, 0);
    len = hybrid_font.GetCurrentEffectLength();
    SetNextAction(NEXT_ACTION_ARM, len);
  }

  void Blast() {
    if (armed_) {
      beeper.Beep(0.1, 2200);
      SetNextAction(NEXT_ACTION_BLOW, len);
    } else {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      SaberBase::DoEffect(EFFECT_ENDARM, 0);
      SetNextAction(NEXT_ACTION_NOTHING, 0);
    }
  }

  void StartSilentTimer() {
    stealth_mode_ = true;
    SaberBase::DoEffect(EFFECT_TIMER, 0);
    SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, 1);
    SetNextAction(NEXT_ACTION_BLOW, DETONATOR_STEALTH_TIMER_DURATION);
  }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    PollNextAction();
    sound_library_.Poll(wav_player);
    if (wav_player && !wav_player->isPlaying()) wav_player.Free();
  }

#ifdef DETONATOR_AUTO_ON
  // Pull in parent's SetPreset, but turn the detonator on.
  void SetPreset(int preset_num, bool announce) override {
    PropBase::SetPreset(preset_num, announce);
    if (!SFX_poweron && !SaberBase::IsOn()) {
      On();
    }
  }
#endif

  void DetonatorOn() {
    if (!timer_active_) {
      armed_ = false;
      SetPower(true);
      On();
    }
  }

  void DetonatorOff() {
    if (!timer_active_) {
      armed_ = false;
      SetPower(false);
      Off();
    }
  }

  void DetonatorTwistOn() {
    // Delay twist events to prevent false trigger from over twisting
    PVLOG_DEBUG << "** DETONATOR EVENT_TWIST ON\n";
    if (millis() - last_twist_ > 2000 && millis() - detonator_off_time_ > 1000) {
      DetonatorOn();
      last_twist_ = millis();
    }
  }

  void DetonatorTwistOff() {
    PVLOG_DEBUG << "** DETONATOR EVENT_TWIST OFF\n";
    // Delay twist events to prevent false trigger from over twisting
    if (millis() - last_twist_ > 3000) {
      DetonatorOff();
      last_twist_ = millis();
      detonator_off_time_ = millis();
    }
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      // ON / OFF with TWIST
#if NUM_BUTTONS ==1 // TWIST mandatory
  case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
    DetonatorTwistOn();
    return true;

  case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
    DetonatorTwistOff();
    return true;
#else // NUM_BUTTON >= 2
#ifdef DETONATOR_TWIST // TWIST optional
  case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
    DetonatorTwistOn();
    return true;

  case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
    DetonatorTwistOff();
    return true;
#endif
#endif

#if NUM_BUTTONS >=2
      // ON with buttons (2 buttons only)
#ifdef DETONATOR_BUTTON_POWER_IS_LATCH
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
#else
      // This line is for using this detonator with dual_prop.h or multi_prop.h without a latching button.
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
#endif
        DetonatorOn();
        return true;

      // OFF with buttons (2 buttons only)
#ifdef DETONATOR_BUTTON_POWER_IS_LATCH
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_OFF):
#else
      // This line is for using this detonator with dual_prop.h or multi_prop.h without a latching button.
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
#endif
        DetonatorOff();
        return true;
#endif

      // ARMED or START TIMER (6 seconds) or Volume Up/Down (while ON & in SA22C Volume menu)
#if NUM_BUTTONS >=2
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
#else // NUM_BUTTONS == 1
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
#endif
        if (!mode_volume_) {
          if (!armed_) {
            BeginArm();
          } else {
            len = DETONATOR_TIMER_DURATION;
            SaberBase::DoEffect(EFFECT_TIMER, 0);
            Blast();
          }
        } else {
#ifndef BC_VOLUME_MENU
          FusorVolume();
#endif
        }
        return true;

      // START STEALTH TIMER (12 seconds)
#if NUM_BUTTONS >=2
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
#else // NUM_BUTTONS == 1
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
#endif
        if (!timer_active_) {
          if (!armed_) {
            armed_ = true;
          }
          StartSilentTimer();
        }
        return true;

      // CLASH for BOOM if ARMED or when any timer is running
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        len = 0.0f;
        Blast();
        return true;

      // Enter/Exit volume menu
#if NUM_BUTTONS >=2
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
#else // NUM_BUTTONS == 1
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
#endif

#ifdef BC_VOLUME_MENU
        EnterExit_BC_VolumeMenu();
#else // SA22C_VOLUME_MENU
        EnterExit_SA22C_VolumeMenu();
#endif
        return true;

      // Next Preset or Volume Up/Down (while OFF & in SA22C Volume menu)
#if NUM_BUTTONS >=2
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
#else // NUM_BUTTONS == 1
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
#endif

#ifndef BC_VOLUME_MENU
        if (mode_volume_) {
          FusorVolume();
        } else
#endif
        FusorPreset();
        return true;

      // Start or Stop track
#if NUM_BUTTONS >=2
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
#else // NUM_BUTTONS == 1
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
#endif
        StartOrStopTrack();
        return true;

      // Battery Level
#if NUM_BUTTONS >=2
      case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
#else // NUM_BUTTONS == 1
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
#endif
        FusorBatteryLevel();
        return true;

/*
// BLADE_DETECT_PIN ORI
#ifdef BLADE_DETECT_PIN
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        blade_detected_ = true;
        FindBladeAgain();
        SaberBase::DoBladeDetect(true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        blade_detected_ = false;
        FindBladeAgain();
        SaberBase::DoBladeDetect(false);
        return true;
#endif // BLADE_DETECT_PIN
*/

/* For multiple blade detect part 3/4
// (Need to add part 1/4 to events.h 2/4 to config, part 3/4 to props, part 4/4 to to ProffieOS.ino)
*/

#ifdef BLADE_DETECT_PIN
  case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
  case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
    UpdateBladeInserted (1, true);
    return true;

  case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
  case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
    UpdateBladeInserted (1, false);
    return true;
#endif // BLADE_DETECT_PIN

#ifdef BLADE_DETECT_PIN2
  case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
  case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
    UpdateBladeInserted (2, true);
    return true;

  case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
  case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
    UpdateBladeInserted (2, false);
    return true;
#endif // BLADE_DETECT_PIN2

#ifdef BLADE_DETECT_PIN3
  case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
  case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
    UpdateBladeInserted (3, true);
    return true;

  case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
  case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
    UpdateBladeInserted (3, false);
    return true;
#endif // BLADE_DETECT_PIN3

    }  // switch (EVENTID)
    return false;
  }  // Event2

  void UpdateBladeInserted (int blade_detect_index, bool blade_inserted) {
    switch (blade_detect_index) { // blade_detect_index = 1, 2 or 3
#ifdef BLADE_DETECT_PIN
      case 1:
        blade_detected_ = blade_inserted;
        break;
#endif
#ifdef BLADE_DETECT_PIN2
      case 2:
        blade2_detected_ = blade_inserted;
        break;
#endif
#ifdef BLADE_DETECT_PIN3
      case 3:
        blade3_detected_ = blade_inserted;
        break;
#endif
    }
    FindBladeAgain();
    SaberBase::DoBladeDetect(blade_inserted);
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;
      // System effects: (info found in config/styles/blade-effects.md, https://pod.hubbe.net/config/styles/blade-effects.html)
      // ===============
        // Shows volume level visually on blade, great for using with volume menu feature.
      case EFFECT_VOLUME_LEVEL:    PVLOG_DEBUG << "*** Blade Effect Volume Level\n";           return;
        // Typically used to apply a dimming effect on blade to conserve battery.
        // Here it will be used for "stealth mode".
      case EFFECT_POWERSAVE:       sound_library_.SayDim();
                                   //hybrid_font.PlayPolyphonic(&SFX_dim);
                                   PVLOG_DEBUG << "*** Effect Powersave/Dim\n";                return;
        // On-Demand Battery Level. Shows battery level visually on blade.
      case EFFECT_BATTERY_LEVEL:   sound_library_.SayBatteryLevel();
                                   //hybrid_font.PlayPolyphonic(&SFX_battery);
                                   PVLOG_DEBUG << "*** Blade Effect Battery Level\n";
                                   PVLOG_NORMAL << "***** May the Force be with you. *****\n"; return;
        // Triggers the change for sets of sounds within the font from one alternative to another.
      case EFFECT_ALT_SOUND:                                                                   return;
        // https://pod.hubbe.net/sound/alt_sounds.html#have-the-prop-control-the-alt "SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, N);" N is the alt value!
    }  // switch (effect)
  }  // SB_Effect

  RefPtr<BufferedWavPlayer> wav_player;

  void FusorPreset() {
    if (fusor.angle1() > M_PI / 9) {
      // Pointing towards UP (above +20°)
      next_preset();
      PVLOG_DEBUG << "** Next preset\n";
    } else {
      if (fusor.angle1() < -M_PI / 9) {
        // Pointing  towards DOWN (below -20°)
        previous_preset();
        PVLOG_DEBUG << "** Previous preset\n";
      } else {
        // Not pointing towards UP or DOWN (between -20° & +20°)
        first_preset();
        PVLOG_DEBUG << "** Jumped to first preset\n";
      }
    }
  }

#ifdef BC_VOLUME_MENU
  void Setup() override {
    MKSPEC<DetonatorOliMenuSpec>::SoundLibrary::init();
  }

  void EnterExit_BC_VolumeMenu() {
    pushMode<MKSPEC<DetonatorOliMenuSpec>::DetonatorOliVolumeMenu>();
  }
#else
  // SA22C_VOLUME_MENU
  void QuickMaxVolume() {
    dynamic_mixer.set_volume(max_volume_);
    PVLOG_DEBUG << "** Maximum Volume\n";
    sound_library_.SayMaximumVolume();
  }

  void QuickMinVolume() {
    dynamic_mixer.set_volume(min_volume_);
    PVLOG_DEBUG << "** Minimum Volume\n";
    sound_library_.SayMinimumVolume();
  }

  void FusorVolume() {
    // Pointing UP (above +60°)
    if (fusor.angle1() > M_PI / 3) {
      QuickMaxVolume();
    } else {
      if (fusor.angle1() > 0) {
        // Pointing towards UP (above +0°)
        VolumeUp();
      } else {
        if (fusor.angle1() < -M_PI / 3) {
          // Pointing DOWN (below -60°)
          QuickMinVolume();
        } else {
          // Pointing towards DOWN (below -0°)
          VolumeDown();
        }
      }
    }
  }

  // Based on SA22C Volume Menu
  void EnterExit_SA22C_VolumeMenu() {
    if (!mode_volume_) {
      mode_volume_ = true;
      sound_library_.SayEnterVolumeMenu();
      PVLOG_DEBUG << "* Enter Volume Menu\n";
    } else {
      mode_volume_ = false;
      sound_library_.SayVolumeMenuEnd();
      PVLOG_DEBUG << "* Exit Volume Menu\n";
    }
  }

  // Based on SA22C Volume Menu
  void VolumeUp() {
    PVLOG_DEBUG << "* Volume Up\n";
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1, dynamic_mixer.get_volume() + VOLUME * 0.1));
      sound_library_.SayVolumeUp();
      PVLOG_DEBUG << "* Volume Up - Current Volume: " << dynamic_mixer.get_volume() << "\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    } else {
      // Cycle through ends of Volume Menu option
#ifdef VOLUME_MENU_CYCLE
      if (!max_vol_reached_) {
        sound_library_.SayMaximumVolume();
        PVLOG_DEBUG << "* Maximum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        max_vol_reached_ = true;
      } else {
        dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
        dynamic_mixer.get_volume() - VOLUME * 0.9));
        sound_library_.SayMinimumVolume();
        PVLOG_DEBUG << "* Minimum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        max_vol_reached_ = false;
      }
#else
      sound_library_.SayMaximumVolume();
      PVLOG_DEBUG << "* Maximum Volume: \n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
#endif
    }
  }

  // Based on SA22C Volume Menu
  void VolumeDown() {
    PVLOG_DEBUG << "* Volume Down\n";
    if (dynamic_mixer.get_volume() > (0.1 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1, dynamic_mixer.get_volume() - VOLUME * 0.1));
      sound_library_.SayVolumeDown();
      PVLOG_DEBUG << "* Volume Down - Current Volume: " << dynamic_mixer.get_volume() << "\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    } else {
      // Cycle through ends of Volume Menu option
#ifdef VOLUME_MENU_CYCLE
      if (!min_vol_reached_) {
        sound_library_.SayMinimumVolume();
        PVLOG_DEBUG << "* Minimum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        min_vol_reached_ = true;
      } else {
        dynamic_mixer.set_volume(VOLUME);
        sound_library_.SayMaximumVolume();
        PVLOG_DEBUG << "* Maximum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        min_vol_reached_ = false;
      }
#else
      sound_library_.SayMinimumVolume();
      PVLOG_DEBUG << "* Minimum Volume: \n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
#endif
    }
  }
#endif

  // Based on BlasterBC DoSpokenBatteryLevel()
  void FusorBatteryLevel() {
    // Avoid weird battery readings when using USB
    if (battery_monitor.battery() < 0.5) {
      sound_library_.SayTheBatteryLevelIs();
      sound_library_.SayDisabled();
      return;
    }
    SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
    sound_library_.SayTheBatteryLevelIs();
    // Pointing towards UP (above +20°)
    if (fusor.angle1() > M_PI / 9) {
      // Battery Level in Percent
      sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
      sound_library_.SayPercent();
      PVLOG_DEBUG << "***Audio Battery Percentage: " <<battery_monitor.battery_percent() << "%, Battery Voltage: " << battery_monitor.battery() << " Volts\n";
      //is_speaking_ = true;
    } else {
      // Pointing towards DOWN (below -20°)
      if (fusor.angle1() < -M_PI / 9) {
        // Battery Level in Volts
        sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
        sound_library_.SayVolts();
        PVLOG_DEBUG << "***Audio Battery Voltage: " << battery_monitor.battery() << " Volts, Battery Percentage: " <<battery_monitor.battery_percent() << "%\n";
        //is_speaking_ = true;
      } else {
        // Not pointing towards UP or DOWN (between -20° & +20°)
        // Battery Level in Percent & Volts
        sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
        sound_library_.SayPercent();
        sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
        sound_library_.SayVolts();
        PVLOG_DEBUG << "***Audio Both *** Battery Percentage: " <<battery_monitor.battery_percent() << "%, Battery Voltage: " << battery_monitor.battery() << " Volts\n";
      }
    }
  }

  // From BlasterBC
  void DoQuote() {
    if (SFX_quote) {
      if (GetWavPlayerPlaying(&SFX_quote)) return;  // Simple prevention of quote overlap
      sequential_quote_ ? SFX_quote.SelectNext() : SFX_quote.Select(-1);
      hybrid_font.PlayCommon(&SFX_quote);
    }
  }

private:
  // State variables
  NextAction next_action_       = NEXT_ACTION_NOTHING;
  uint32_t time_base_           = 0;            // from original detonator.h
  uint32_t next_event_time_     = 0;            // from original detonator.h
  //bool powered_               = true;         // from original detonator.h, but not used any more.
  float len                     = 0.0f;         // timer duration (in seconds) (0.0f for clash, 6.0f for "grenade", 12.0f for "stealth")
  bool armed_                   = false;        // once armed_, it can go boom with clash
  bool timer_active_            = false;        // once timer_active_, it will do boom.wav and can't be disarmed (unless you kill the power)
  bool stealth_mode_            = false;        // tracks Alt-Sounds for stealth mode
  uint32_t last_twist_          = millis();     // from sa22c for twist ON/OFF
  uint32_t detonator_off_time_  = millis();     // renamed from sa22c for twist OFF

  // Sound effects handling
  //bool is_speaking_           = false;        // from blasterBC for prevent overlap
  bool sequential_quote_        = false;        // from blasterBC for quote

  // Volume handling
  bool mode_volume_             = false;        // from sa22c for volume menu
  bool max_vol_reached_         = false;        // from sa22c for volume menu
  bool min_vol_reached_         = false;        // from sa22c for volume menu
#ifndef BC_VOLUME_MENU
  const int max_volume_         = VOLUME;       //from BC volume menu (for sa22c max volume)
  const int min_volume_         = VOLUME * 0.1; //from BC volume menu (for sa22c min volume)
#endif

  // multiple blade detect
#ifdef BLADE_DETECT_PIN2
  bool blade2_detected_ = false;
#endif

#ifdef BLADE_DETECT_PIN3
  bool blade3_detected_ = false;
#endif

}; // class DetonatorOliButtons

#endif // PROPS_DETONATOR_OLI_BUTTONS_H

/************************************************\
|*                                              *|
|*   DETONATOR OLI BUTTONS DISPLAY CONTROLLER   *|
|*                                              *|
\************************************************/

// If you want to add more, replace all the "***xxxn***, ***XXXn***, ***Xxxn***" with appropriate
// effects name(s), and respect the CAPS, no caps & Some Caps. Or else it may not compile or work!

#ifdef PROP_BOTTOM

#define ONCE_PER_DETONATOR_EFFECT(X)  \
  X(bgnarm)                           \
  X(armhum)                           \
  X(endarm)                           \
  X(boom)                             \
  X(timer) // Add a backslash to this line when adding the next line.
          // The last line in the list needs no backslash at the end!
//X(***xxx1***)
//X(***xxx2***)
//X(***xxx3***)

#ifdef INCLUDE_SSD1306

struct DetonatorDisplayConfigFile : public ConfigFile {
  DetonatorDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSBgnarmImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSArmhumImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSEndarmImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSBoomImageDuration,   1000.0f);
    CONFIG_VARIABLE2(ProffieOSTimerImageDuration,  1000.0f);
  //CONFIG_VARIABLE2(ProffieOS***Xxx1***ImageDuration, 1000.0f);
  //CONFIG_VARIABLE2(ProffieOS***Xxx2***ImageDuration, 1000.0f);
  //CONFIG_VARIABLE2(ProffieOS***Xxx3***ImageDuration, 1000.0f);
  }

  // for OLED displays, the time a bgnarm.bmp will play.
  float ProffieOSBgnarmImageDuration;
  // for OLED displays, the time a armhum.bmp will play.
  float ProffieOSArmhumImageDuration;
  // for OLED displays, the time a endarm.bmp will play.
  float ProffieOSEndarmImageDuration;
  // for OLED displays, the time a boom.bmp   will play.
  float ProffieOSBoomImageDuration;
  // for OLED displays, the time a timer.bmp  will play.
  float ProffieOSTimerImageDuration;
  // for OLED displays, the time a ***xxx1***.bmp will play.
//float ProffieOS***Xxx1***ImageDuration;
  // for OLED displays, the time a ***xxx2***.bmp will play.
//float ProffieOS***Xxx2***ImageDuration;
  // for OLED displays, the time a ***xxx3***.bmp will play.
//float ProffieOS***Xxx3***ImageDuration;
};

template<typename PREFIX = ByteArray<>>
struct DetonatorDisplayEffects  {
  DetonatorDisplayEffects() : dummy_(0) ONCE_PER_DETONATOR_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_DETONATOR_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class DetonatorDisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  DetonatorDisplayEffects<PREFIX> img_;
  DetonatorDisplayConfigFile &detonator_font_config;
  DetonatorDisplayController() :
    img_(*getPtr<DetonatorDisplayEffects<PREFIX>>()),
    detonator_font_config(*getPtr<DetonatorDisplayConfigFile>()) {}

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_BGNARM:
        if (img_.IMG_bgnarm) {
          ShowFileWithSoundLength(&img_.IMG_bgnarm, detonator_font_config.ProffieOSBgnarmImageDuration);
        } else {
          this->SetMessage(" td\narming");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_ARMHUM:
        if (img_.IMG_armhum) {
          ShowFileWithSoundLength(&img_.IMG_armhum, detonator_font_config.ProffieOSArmhumImageDuration);
        } else {
          this->SetMessage(" td\narmed");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_ENDARM:
        if (img_.IMG_endarm) {
          ShowFileWithSoundLength(&img_.IMG_endarm, detonator_font_config.ProffieOSEndarmImageDuration);
        } else {
          this->SetMessage("    td\ndisarming");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_BOOM:
        if (img_.IMG_boom) {
          ShowFileWithSoundLength(&img_.IMG_boom,   detonator_font_config.ProffieOSBoomImageDuration);
        } else {
          this->SetMessage(" td\nboom");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_TIMER:
        if (img_.IMG_timer) {
          ShowFileWithSoundLength(&img_.IMG_timer,  detonator_font_config.ProffieOSTimerImageDuration);
        } else {
          this->SetMessage(" td\n5,4,3,2,1");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

/*
      case EFFECT_***XXX1***:
        if (img_.IMG_***xxx1***) {
          ShowFileWithSoundLength(&img_.IMG_***xxx1***, detonator_font_config.ProffieOS***Xxx1***ImageDuration);
        } else {
          this->SetMessage("***xxx1***");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_***XXX2***:
        if (img_.IMG_***xxx2***) {
          ShowFileWithSoundLength(&img_.IMG_***xxx2***, detonator_font_config.ProffieOS***Xxx2***ImageDuration);
        } else {
          this->SetMessage("***xxx2***");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_***XXX3***:
        if (img_.IMG_***xxx3***) {
          ShowFileWithSoundLength(&img_.IMG_***xxx3***, detonator_font_config.ProffieOS***Xxx3***ImageDuration);
        } else {
          this->SetMessage("***xxx3***");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;
*/

      default:
        StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }

  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_BLAST) {
      ShowFileWithSoundLength(img_.IMG_bgnarm, detonator_font_config.ProffieOSBgnarmImageDuration);
      ShowFileWithSoundLength(img_.IMG_armhum, detonator_font_config.ProffieOSArmhumImageDuration);
      ShowFileWithSoundLength(img_.IMG_endarm, detonator_font_config.ProffieOSEndarmImageDuration);
      ShowFileWithSoundLength(img_.IMG_boom,   detonator_font_config.ProffieOSBoomImageDuration);
      ShowFileWithSoundLength(img_.IMG_timer,  detonator_font_config.ProffieOSTimerImageDuration);
    //ShowFileWithSoundLength(img_.IMG_***xxx1***, detonator_font_config.ProffieOS***Xxx1***ImageDuration);
    //ShowFileWithSoundLength(img_.IMG_***xxx2***, detonator_font_config.ProffieOS***Xxx2***ImageDuration);
    //ShowFileWithSoundLength(img_.IMG_***xxx3***, detonator_font_config.ProffieOS***Xxx3***ImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};

#endif  // INCLUDE_SSD1306

template<int W, int H, typename PREFIX = ConcatByteArrays<typename NumberToByteArray<W>::type, ByteArray<'x'>,
         typename NumberToByteArray<H>::type>>
class DetonatorColorDisplayController : public StandarColorDisplayController<W, H, PREFIX> {
public:
  template<int w, int h>
  explicit DetonatorColorDisplayController(SizedLayeredScreenControl<w, h>* screen) :
  StandarColorDisplayController<W, H, PREFIX>(screen) ONCE_PER_DETONATOR_EFFECT(INIT_SCR) {
  }
  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_BGNARM:     this->scr_.Play(&SCR_bgnarm);     break;
      case EFFECT_ARMHUM:     this->scr_.Play(&SCR_armhum);     break;
      case EFFECT_ENDARM:     this->scr_.Play(&SCR_endarm);     break;
      case EFFECT_BOOM:       this->scr_.Play(&SCR_boom);       break;
      case EFFECT_TIMER:      this->scr_.Play(&SCR_timer);      break;
    //case EFFECT_***XXX1***: this->scr_.Play(&SCR_***xxx1***); break;
    //case EFFECT_***XXX2***: this->scr_.Play(&SCR_***xxx2***); break;
    //case EFFECT_***XXX3***: this->scr_.Play(&SCR_***xxx3***); break;
      default:
        StandarColorDisplayController<W, H, PREFIX>::SB_Effect2(effect, location);
    }
  }

protected:
  ONCE_PER_DETONATOR_EFFECT(DEF_SCR);
};

#undef ONCE_PER_DETONATOR_EFFECT

#endif  // PROP_BOTTOM


