/*
============================================================
================== SABERSENSE PROP FILE ====================
============================================================

Built on SA22C programming by Matthew McGeary.
Modified by Chris Carter with substantial support and
contributions from Fredrik Hubinette and Brian Conner.

This prop file references certain custom sound files
to aid in saber function navigation. These sound files
are optional and are available as a free download from
the Sabersense website. (Link below).

This prop file has been optimized for ProffieOS 8.x
or later. A ProffieOS 7.x compatible version of this 
prop file is available as a free download from the 
Sabersense website.

https://sabersense.square.site/downloads

============================================================
========= BUTTON SYSTEM PRINCIPLES AND LOGIC NOTES =========

The Sabersense button control system has been engineered
for simplicity and usability. Complex gesture controls and
features like Battle Mode are NOT included.

By default, the Sabersense button prop features harmonized
controls between one-button and two-button operation without
compromising the greater usability of two-button systems.

Where practicable, the same controls apply to both one-button
and two-button systems, with two-button operation benefitting
from some extra features.

Where possible without causing conflicts, some one-button
controls also appear in two-button mode, despite two-button
having its own controls for the same feature.

The overall intention is that by default, users need only
remember a minimum set of control principles in order to
access all functions. As such, the logic is that the
same button presses do the same thing within the various
states, subject to inevitable and obvious variants.

Hence:
  ONE AND TWO BUTTON:
      Single click MAIN always lights the blade...
        Short click lights blade with sound
        Long click lights blade mute

      Double click MAIN always plays a sound file...
        Character Quote or Music track with blade OFF
        Character Quote or Force Effect with blade ON
        Battery level announcement if held down on second click

      Holding down MAIN (1-button) or AUX (2-button)
      and waiting with blade OFF always skips to specific preset...
        Hilt pointing up - first preset
        Hilt horizontal - middle preset
        Hilt pointing down - last preset

      Triple-clicking MAIN is always a saber management feature...
        Colour change with blade ON
        BladeID/Array Switch with blade OFF

  TWO BUTTON ONLY:
      Short clicking AUX with blade OFF always moves to a different preset,
      forward with hilt pointing up, backwards with hilt pointing down...
        Single short click - one preset
        Double short click - five presets
        Triple short click - ten presets

      Holding MAIN and short clicking AUX always enters a control menu...
        Colour change with blade ON
        Volume menu with blade OFF

==========================================================
=================== 1 BUTTON CONTROLS ====================

MAIN FUNCTIONS
  Activate blade            Short click while OFF. *
  Activate blade mute       Long click while OFF, hilt horizontal.
                              (Hold for one second then release).
  Deactivate blade          Hold and wait until blade is off while ON.

FUNCTIONS WITH BLADE OFF
  Next preset               Long click while OFF, hilt pointing up.
                              (Hold for one second then release).
  Previous preset           Long click while OFF, hilt pointing down.
                              (Hold for one second then release).
  Skip to first preset      Press and hold until it switches, hilt pointing upwards.
  Skip to middle preset     Press and hold until it switches, hilt horizontal.
  Skip to last preset       Press and hold until it switches, hilt pointing downwards.
  Play Character Quote      Fast double-click, hilt pointing up, plays sequentially. **
  Play Music Track          Fast double-click, hilt pointing down. **
  Speak battery voltage     Fast double-click-and-hold while OFF.
  Run BladeID/Array Select  Fast triple-click while OFF. (Applicable installs only).
  Restore Factory Defaults  Fast four-clicks while OFF, hold on last click. ***
                              Release once announcement starts.
  Enter/Exit VOLUME MENU    Hold and clash while OFF.
    Volume up               Click while in VOLUME MENU, hilt pointing up.
    Volume down             Click while in VOLUME MENU, hilt pointing down.
                              Volume adjusts in increments per click.
                              You must exit VOLUME MENU to resume using lightsaber normally.

FUNCTIONS WITH BLADE ON
  Blade lockup              Hold and hit clash while ON.
  Blade tip drag            Hold and hit clash while ON pointing the blade tip down.
  Play Character Quote      Fast double-click, hilt pointing up, plays sequentially. **
  Force Effect              Fast double-click, hilt pointing down, plays randomly. **
  Lightning block           Double click and hold while ON.
  Melt                      Hold and push blade tip against wall (stab). Rotate for heat colours.
  Blaster blocks            Short click/double click/triple click while ON.
  Enter multi-blast mode    Hold while swinging for one second and release.
                              To trigger blaster block, swing saber while in multi-blast mode.
                              To exit, hold while swinging for one second and release.

COLOUR CHANGE FUNCTIONS WITH BLADE ON
  Enter COLOUR MENU         Fast triple-click while ON.
                              Announcement confirms you are in the COLOUR MENU.
  Cycle to next colour      Rotate hilt whilst in COLOUR MENU until desired colour is reached.
  Exit COLOUR MENU          Short click saves new colour and exits.
                            Fast-double-click exits and reverts to original colour.
                              Announcement confirms you are exiting COLOUR MENU.
                              You must exit COLOUR MENU to resume using lightsaber normally.

  *   = Gesture ignitions also available via defines.
  **  = Audio player orientations can be reversed using SABERSENSE_FLIP_AUDIO_PLAYERS define.
  *** = Feature must be enabled in config file using SABERSENSE_ENABLE_RESET define.

============================================================
===================== 2 BUTTON CONTROLS ====================

MAIN FUNCTIONS
  Activate blade            Short click MAIN. *
  Activate blade mute       Long click MAIN (hold for one second then release).
  Deactivate blade          Press and hold MAIN and wait until blade is off.

FUNCTIONS WITH BLADE OFF
  Next preset               Short click AUX, hilt pointing upwards.
  Previous preset           Short click AUX, hilt pointing downwards.
  Previous preset           Hold AUX and short click MAIN.
                              (Duplicate legacy command).
  Skip to first preset      Press and hold any button until it switches, hilt upwards.
  Skip to middle preset     Press and hold any button until it switches, hilt horizontal.
  Skip to last preset       Press and hold any button until it switches, hilt downwards.
  Skip forward 5 presets    Fast double-click AUX, hilt pointing upwards.
  Skip back 5 presets       Fast double-click AUX, hilt pointing downwards.
  Skip forward 10 presets   Fast triple-click AUX, hilt pointing upwards.
  Skip back 10 presets      Fast triple-click AUX, hilt pointing downwards.
  Play Character Quote      Fast double-click MAIN, hilt pointing up, plays sequentially. **
  Play Music Track          Fast double-click MAIN, pointing down. **
  Speak battery voltage     Fast double-click-and-hold MAIN.
  Run BladeID/Array Select  Fast triple-click. (Applicable installs only).
  Restore Factory Defaults  Fast four-clicks MAIN, hold on last click. ***
                              Release once announcement starts.
  Enter/Exit VOLUME MENU    Hold MAIN then quickly click AUX and release both simultaneously.
    Volume up               Click MAIN while in VOLUME MENU, hilt pointing up.
    Volume down             Click MAIN while in VOLUME MENU, hilt pointing down, OR click
                              AUX while in VOLUME MENU.
                              Volume adjusts in increments per click.
                              You must exit VOLUME MENU to resume using saber normally.

FUNCTIONS WITH BLADE ON
  Blade lockup              Press and hold AUX.
  Blade tip drag            Press and hold AUX while blade is pointing down.
  Play Character Quote      Fast double-click MAIN, hilt pointing up, plays sequentially. **
  Force Effect              Fast double-click MAIN, hilt pointing down, plays randomly. **
  Lightning block           Double-click MAIN and hold.
  Melt                      Hold MAIN and stab blade tip against wall. Rotate for heat colours.
  Blaster blocks            Short click AUX. (Add Short click MAIN using define).
  Enter multi-blast mode    Hold MAIN while swinging for one second and release.
                              Saber will play two quick blasts confirming mode.
                              Swing blade to trigger blaster block.
                              To exit multi-blast mode, fast single click AUX.

COLOUR CHANGE FUNCTIONS WITH BLADE ON
  Enter COLOUR MENU         Hold MAIN then quickly click AUX and release both
                            buttons simultaneously. Or fast triple-click MAIN.
                              Announcement confirms you are in the COLOUR MENU.
  Cycle to next colour      Rotate hilt whilst in COLOUR MENU until desired colour is reached.
                              Most Sabersense presets have 12 colour options.
  Exit COLOUR MENU          Short click saves new colour and exits.
                            Fast-double-click exits and reverts to original colour.
                              Announcement confirms you are exiting COLOUR MENU.
                              You must exit COLOUR MENU to resume using lightsaber normally.

  *   = Gesture ignitions also available via defines.
  **  = Audio player orientations can be reversed using SABERSENSE_FLIP_AUDIO_PLAYERS define.
  *** = Feature must be enabled in config file using SABERSENSE_ENABLE_RESET define.

===========================================================
=================== SABERSENSE DEFINES ====================

#define SABERSENSE_BLADE_ID
  Replaces regular BladeID with on-demand BladeID scanning.
  Plays array-specific bladeidX.wav files when switching.

#define SABERSENSE_ARRAY_SELECTOR
  Replaces regular BladeID and allows cycling between
  different blade/preset arrays manually, regardless
  of actual BladeID status. Plays array-specific
  arrayX.wav files when switching.

#define SABERSENSE_ENABLE_ARRAY_FONT_IDENT
  Plays font ident after array ident when switching arrays.
  Use with SABERSENSE_BLADE_ID and SABERSENSE_ARRAY_SELECTOR.

#define SABERSENSE_FLIP_AUDIO_PLAYERS
  Reverses UP/DOWN orientation for playing FORCE, QUOTE and
  MUSIC TRACK audio files. Default (no define) is UP for
  sequential QUOTE with blade ON and OFF, and DOWN for random
  FORCE effect (ON) and music TRACK (OFF). Define acts on
  both ON and OFF states for consistency.

#define SABERSENSE_BLAST_MAIN_AND_AUX
  Adds blaster block button to MAIN as well as AUX in
  2-button mode. Improves 1 and 2-button harmonization,
  but makes accidental blasts more likely when double-clicking
  for Quotes or Force Effect.

#define SABERSENSE_BUTTON_CLICKER
  Button Clicker to play press/release wav files when
  buttons are pressed. Intended for Scavenger hilt
  where wheel makes tactile feel difficult.
  Requires press.wav and release.wav files to work.

#define SABERSENSE_ENABLE_RESET
  Enables system to be completely reset to 'factory defaults'
  i.e. original config, using button press to delete
  all save files.

#define SABERSENSE_NO_COLOR_CHANGE
  Use instead of DISABLE_COLOR_CHANGE.

#define SABERSENSE_NO_LOCKUP_HOLD
  Applicable to two-button mode only, reverts to lockup being
  triggered by clash while holding aux.

GESTURE CONTROLS
  There are four gesture types: Twist, Stab, Swing and Thrust.
  Gesture controls bypass all preon effects.
  #define SABERSENSE_TWIST_ON
  #define SABERSENSE_TWIST_OFF
  #define SABERSENSE_STAB_ON
  #define SABERSENSE_SWING_ON
  #define SABERSENSE_THRUST_ON

============================================================
============================================================
*/
#ifndef PROPS_SABER_SABERSENSE_BUTTONS_H
#define PROPS_SABER_SABERSENSE_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"

#undef PROP_TYPE
#define PROP_TYPE SabersenseButtons

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 15 * 1000
#endif

#ifndef SABERSENSE_SWING_ON_SPEED
#define SABERSENSE_SWING_ON_SPEED 250
#endif

#ifndef SABERSENSE_LOCKUP_DELAY
#define SABERSENSE_LOCKUP_DELAY 200
#endif

#ifndef SABERSENSE_FORCE_PUSH_LENGTH
#define SABERSENSE_FORCE_PUSH_LENGTH 5
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

EFFECT(dim);      // for EFFECT_POWERSAVE
EFFECT(battery);  // for EFFECT_BATTERY_LEVEL
EFFECT(vmbegin);  // for Begin Volume Menu
EFFECT(vmend);    // for End Volume Menu
EFFECT(volup);    // for increase volume
EFFECT(voldown);  // for decrease volume
EFFECT(volmin);   // for minimum volume reached
EFFECT(volmax);   // for maximum volume reached
EFFECT(faston);   // for EFFECT_FAST_ON
EFFECT(blstbgn);  // for Begin Multi-Blast
EFFECT(blstend);  // for End Multi-Blast
EFFECT(array);    // for playing array idents
EFFECT(bladeid);  // for playing bladeid idents
EFFECT(reset);    // for playing factory default reset completed

// The Saber class implements the basic states and actions
// for the saber.
class SabersenseButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  SabersenseButtons() : PropBase() {}
  const char* name() override { return "SabersenseButtons"; }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    Vec3 mss = fusor.mss();
    if (SaberBase::IsOn()) {
      DetectSwing();
      if (auto_lockup_on_ &&
        !swinging_ &&
        fusor.swing_speed() > 120 &&
        millis() - clash_impact_millis_ > SABERSENSE_LOCKUP_DELAY &&
        SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
        !swinging_ &&
        fusor.swing_speed() > 60 &&
        millis() - clash_impact_millis_ > SABERSENSE_LOCKUP_DELAY &&
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
        if (millis() - push_begin_millis_ > SABERSENSE_FORCE_PUSH_LENGTH) {
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
        if (!swinging_ && fusor.swing_speed() > SABERSENSE_SWING_ON_SPEED) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
      }
      // EVENT_THRUST
      if (mss.y * mss.y + mss.z * mss.z < 16.0 &&
        mss.x > 14 &&
        fusor.swing_speed() < 150) {
        if (millis() - thrust_begin_millis_ > 15) {
          Event(BUTTON_NONE, EVENT_THRUST);
          thrust_begin_millis_ = millis();
        }
      } else {
        thrust_begin_millis_ = millis();
      }
    }
    // Enables sequential sounds and processes on array handling.
#ifdef SABERSENSE_BLADE_ID
    if (do_font_after_sound_ && !IsBladeidSoundPlaying()) {
      SaberBase::DoNewFont();
      do_font_after_sound_ = !do_font_after_sound_;
    }
#endif

#ifdef SABERSENSE_ARRAY_SELECTOR
    if (do_font_after_sound_ && !IsArraySoundPlaying()) {
      SaberBase::DoNewFont();
      do_font_after_sound_ = !do_font_after_sound_;
    }
#endif
  }

  // VOLUME MENU
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

  // BLADE ID OPTIONS AND ARRAY MANAGEMENT
  // True BladeID, runs scan on-demand with unique 'bladeid' ident sound.
#ifdef SABERSENSE_BLADE_ID
#ifndef ENABLE_POWER_FOR_ID
#error "SABERSENSE_BLADE_ID requires ENABLE_POWER_FOR_ID to be defined."
#endif

  bool IsBladeidSoundPlaying() {
    return !!GetWavPlayerPlaying(&SFX_bladeid);
  }

#ifdef SABERSENSE_ENABLE_ARRAY_FONT_IDENT  // Plays 'bladeid' sound AND 'font' sound.
  void TriggerBladeID() {
    FindBladeAgain();
    SFX_bladeid.Select(current_config - blades);
    hybrid_font.PlayCommon(&SFX_bladeid);
    // Calls Loop function to handle waiting for effect before running DoNewFont.
    do_font_after_sound_ = !do_font_after_sound_;
  }
#else
  // Plays 'bladeid' sound only, or 'font' sound if no 'bladeid' sound available.
  void TriggerBladeID() {
    FindBladeAgain();
    if (SFX_bladeid) {
      SFX_bladeid.Select(current_config - blades);
      hybrid_font.PlayCommon(&SFX_bladeid);  // Play 'bladeid' sound file if available.
    } else {
      SaberBase::DoNewFont();  // Play font ident if 'bladeid' sound file missing.
    }
  }
#endif
#endif

  // Manual Array Selector, switches on-demand to next array, plays 'array' ident sound.
#ifdef SABERSENSE_ARRAY_SELECTOR
#ifdef SABERSENSE_BLADE_ID    // Only one Sabersense BladeID standard permitted.
#error "SABERSENSE_ARRAY_SELECTOR and SABERSENSE_BLADE_ID cannot be defined at the same time."
#endif
  bool IsArraySoundPlaying() {
    return !!GetWavPlayerPlaying(&SFX_array);
  }

#ifdef SABERSENSE_ENABLE_ARRAY_FONT_IDENT  // Plays 'array' sound AND 'font' sound.
  void NextBladeArray() {
    FakeFindBladeAgain();
    SFX_array.Select(current_config - blades);
    hybrid_font.PlayCommon(&SFX_array);
    // Calls Loop function to handle waiting for effect before running DoNewFont.
    do_font_after_sound_ = !do_font_after_sound_;
  }
#else
  // Plays 'array' sound only, or 'font' sound if no 'array' sound available.
  void NextBladeArray() {
    FakeFindBladeAgain();
    if (SFX_array) {
      SFX_array.Select(current_config - blades);
      hybrid_font.PlayCommon(&SFX_array);  // Play 'array' sound file if available.
    } else {
      SaberBase::DoNewFont();  // Play font ident if 'array' sound file missing.
    }
  }
#endif

// Manual Blade Array Selection version of FindBladeAgain()
#undef ACTIVATE
#define ACTIVATE(N) do {                      \
  if (!current_config->blade##N) {            \
    goto bad_blade;                           \
  }                                           \
  current_config->blade##N->Activate(N);      \
} while(0);

void FakeFindBladeAgain() {
  // Reverse everything FindBladeAgain does, except for recalculating best_config.
  ONCEPERBLADE(UNSET_BLADE_STYLE)
#undef DEACTIVATE
#define DEACTIVATE(N) do {                    \
  if (current_config->blade##N)               \
    current_config->blade##N->Deactivate();   \
} while(0);
  ONCEPERBLADE(DEACTIVATE);
  SaveVolumeIfNeeded();
  if (fusor.angle1() > 0) {
    // Cycle forwards (next array) if hilt pointing up...
    current_config = blades + (current_config - blades + 1) % NELEM(blades);
  } else {
    // Cycle backwards (previous array) if hilt pointing down.
    current_config = blades + (current_config - blades - 1) % NELEM(blades);
  }

  ONCEPERBLADE(ACTIVATE);
  RestoreGlobalState();
#ifdef SAVE_PRESET
  ResumePreset();
#else
  SetPreset(0, false);
#endif // SAVE_PRESET
  // PVLOG_NORMAL << "** FakeFindBladeAgain() Completed\n";
  return;
#if NUM_BLADES != 0
bad_blade:
  ProffieOSErrors::error_in_blade_array();
#endif
}
#endif

// RESET FACTORY DEFAULTS (Delete Save Files).
// Script to determine if sound effects have finished.
#ifdef SABERSENSE_ENABLE_RESET
bool IsResetSoundPlaying() {
  return !!GetWavPlayerPlaying(&SFX_reset);
}

bool IsBootSoundPlaying() {
  return !!GetWavPlayerPlaying(&SFX_boot);
}
#endif

// CLICK PLAYER FOR BUTTON PRESSES (optional).
void PlaySound(const char* sound) {
  RefPtr<BufferedWavPlayer> player = GetFreeWavPlayer();
  if (player) {
    if (!player->PlayInCurrentDir(sound)) player->Play(sound);
  }
}

bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
  if (GetWavPlayerPlaying(&SFX_boot)) return false;
  switch (EVENTID(button, event, modifiers)) {
    case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_OFF):
    case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_OFF):
      SaberBase::RequestMotion();
#ifdef SABERSENSE_BUTTON_CLICKER
      // Intended for Scavenger hilt where wheel makes tactile feel difficult.
      PlaySound("press.wav");  // Requires press.wav file to work.
#endif
      return false;
    case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
    case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
#ifdef SABERSENSE_BUTTON_CLICKER
      // Intended for Scavenger hilt where wheel makes tactile feel difficult.
      PlaySound("release.wav");  // Requires release.wav file to work.
#endif
      if (SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        return true;
      } else {
        return false;
      }

    case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
    case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
      if (accel_.x < -0.15) {
        pointing_down_ = true;
      } else {
        pointing_down_ = false;
      }
      return true;

    // GESTURE CONTROLS
#ifdef SABERSENSE_SWING_ON
    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
      // Motion chip startup on boot can create false ignition, so delay SwingOn at boot for 3000ms
      if (millis() > 3000) {
        FastOn();
      }
      return true;
#endif

#ifdef SABERSENSE_TWIST_ON
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
      // Delay twist events to prevent false trigger from over twisting
      if (millis() - last_twist_ > 2000 && millis() - saber_off_time_ > 1000) {
        FastOn();
        last_twist_ = millis();
      }
      return true;
#endif

#ifdef SABERSENSE_TWIST_OFF
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
      // Delay twist events to prevent false trigger from over twisting
      if (millis() - last_twist_ > 3000) {
        Off();
        last_twist_ = millis();
        saber_off_time_ = millis();
      }
      return true;
#endif

#ifdef SABERSENSE_STAB_ON
    case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
      if (millis() - saber_off_time_ > 1000) {
        FastOn();
      }
      return true;
#endif

#ifdef SABERSENSE_THRUST_ON
    case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
      if (millis() - saber_off_time_ > 1000) {
        FastOn();
      }
      return true;
#endif

    // MAIN ACTIVATION
    // Saber ON AND Volume Adjust, 1 and 2 Button.
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      IgnoreClash(100);  // Hopefully prevents false clashes due to 'clicky' button.
                         // Low threshold so as not to conflict with 1-button volume menu access.
      if (!mode_volume_) {
        On();
      } else {
        if (fusor.angle1() > 0) {
          VolumeUp();
        } else {
          VolumeDown();
        }
      }
      return true;

    // 1 Button Activate Muted and next/previous preset.
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      IgnoreClash(100);  // Hopefully prevents false clashes due to 'clicky' button.
                         // Low threshold so as not to conflict with 1-button volume menu access.
#define DEGREES_TO_RADIANS (M_PI / 180)
      if (fusor.angle1() > 45 * DEGREES_TO_RADIANS) {
        // If pointing up
        next_preset();
      } else if (fusor.angle1() < -45 * DEGREES_TO_RADIANS) {
        // If pointing down
        previous_preset();
      } else {
        // If horizontal
        if (SetMute(true)) {
          unmute_on_deactivation_ = true;
          On();
        }
      }
#ifdef SAVE_PRESET
      SaveState(current_preset_.preset_num);
#endif
      return true;
#endif

    // 2 Button Activate Muted
#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        On();
      }
      return true;
#endif

    // Turn Blade OFF
    case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
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
      saber_off_time_ = millis();
      swing_blast_ = false;
      return true;

    // PRESET NAVIGATION
    // Next/previous preset and volume down. Next preset (UP), previous preset (DOWN).
#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      // backwards if pointing down
      if (!mode_volume_) {
        SetPreset(current_preset_.preset_num + (fusor.angle1() < -M_PI / 4 ? -1 : 1), true);
      } else {
        VolumeDown();
      }
#ifdef SAVE_PRESET
      SaveState(current_preset_.preset_num);
#endif
      return true;
#endif

    // 2 button: Previous Preset, retained legacy control.
#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
      if (!mode_volume_) {
        previous_preset();
      }
      return true;
#endif

    // Multiple preset skips - 2 button sabers only.
#if NUM_BUTTONS == 2
    // Skips forward five presets pointing up, back five pointing down.
    case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      // Backwards if pointing down
      SetPreset(current_preset_.preset_num + (fusor.angle1() < -M_PI / 4 ? -5 : 5), true);
#ifdef SAVE_PRESET
      SaveState(current_preset_.preset_num);
#endif
      return true;

    // Skips forward ten presets pointing up, back ten pointing down.
    case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      // Backwards if pointing down
      SetPreset(current_preset_.preset_num + (fusor.angle1() < -M_PI / 4 ? -10 : 10), true);
#ifdef SAVE_PRESET
      SaveState(current_preset_.preset_num);
#endif
      return true;
#endif

    // Skips to first preset (up) or last preset (down)
    // or middle preset if horizontal:
#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_OFF):
#endif
    case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF):
#define DEGREES_TO_RADIANS (M_PI / 180)
      if (fusor.angle1() > 45 * DEGREES_TO_RADIANS) {
        // If pointing up
        SetPreset(0, true);
      } else if (fusor.angle1() < -45 * DEGREES_TO_RADIANS) {
        // If pointing down
        SetPreset(-1, true);
      } else {
        // If horizontal
        CurrentPreset tmp;
        tmp.SetPreset(-1);
        SetPreset(tmp.preset_num / 2, true);
      }
#ifdef SAVE_PRESET
      SaveState(current_preset_.preset_num);
#endif
      return true;

    // BLADE ID OPTIONS AND ARRAY NAVIGATION
    // True Blade ID on-demand with BladeID audio idents.
#ifdef SABERSENSE_BLADE_ID
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      TriggerBladeID();
      return true;
#endif

    // Manual Array Selector with Array audio idents.
    // Cycles through blade arrays regardless of BladeID status.
    // UP cycles forward, DOWN cycles back (handled in main code).
#ifdef SABERSENSE_ARRAY_SELECTOR
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      NextBladeArray();
      return true;
#endif

    // SOUND EFFECT PLAYERS.
    // With Blade ON - UP for Character Quote, plays sequentially.
    // With Blade ON - DOWN for Force Effect, plays randomly.
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
#ifndef SABERSENSE_FLIP_AUDIO_PLAYERS
      //  Define reverses UP/DOWN options for all QUOTE/FORCE/TRACK audio player.
      //  Quote player points upwards.
      if (SFX_quote) {
        if (fusor.angle1() > 0) {
          SFX_quote.SelectNext();
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
        } else {
          SaberBase::DoForce();  // Force effect for hilt pointed DOWN.
        }
      } else {
        SaberBase::DoForce();  // Fallback: play force effect if no quotes are available.
      }
      return true;
#else
      //  Quote player points downwards.
      if (SFX_quote) {
        if (fusor.angle1() < 0) {
          SFX_quote.SelectNext();
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
        } else {
          SaberBase::DoForce();  // Force effect for hilt pointed DOWN.
        }
      } else {
        SaberBase::DoForce();  // Fallback: play force effect if no quotes are available.
      }
      return true;
#endif

    //  With Blade OFF - UP for Character Quote, plays sequentially.
    //  With Blade OFF - DOWN for Music Track.
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
#ifndef SABERSENSE_FLIP_AUDIO_PLAYERS
      // Define reverses UP/DOWN options for all QUOTE/FORCE/TRACK audio players.
      //  Quote player points upwards.
      if (SFX_quote) {
        if (fusor.angle1() > 0) {
          SFX_quote.SelectNext();
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
        } else {
          StartOrStopTrack();  // Play track for hilt pointed DOWN.
        }
      } else {
        StartOrStopTrack();  // Fallback: play track if no quotes are available.
      }
      return true;
#else
      // Quote player points downwards.
      if (SFX_quote) {
        if (fusor.angle1() < 0) {
          SFX_quote.SelectNext();
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
        } else {
          StartOrStopTrack();  // Play track for hilt pointed DOWN.
        }
      } else {
        StartOrStopTrack();  // Fallback: play track if no quotes are available.
      }
      return true;
#endif

    // COLOUR CHANGE.
#ifdef DISABLE_COLOR_CHANGE
#error "DISABLE_COLOR_CHANGE is not supported. Use SABERSENSE_NO_COLOR_CHANGE instead."
#endif

    // 1 and 2 button modes.
#ifndef SABERSENSE_NO_COLOR_CHANGE
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
      ToggleColorChangeMode();
      return true;
    // 2 button mode only.
#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
      ToggleColorChangeMode();
      return true;
#endif
#endif

    // BLASTER DEFLECTION
    // 1 Button
#if NUM_BUTTONS == 1
    // 1 button
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      swing_blast_ = false;
      SaberBase::DoBlast();
      return true;
#endif

    // 2 Button
#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
      swing_blast_ = false;
      SaberBase::DoBlast();
      return true;

    // Add blast to MAIN on 2-button sabers.
#ifdef SABERSENSE_BLAST_MAIN_AND_AUX
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      swing_blast_ = false;
      SaberBase::DoBlast();
    }
    return true;
#endif
#endif

    //  Multi-Blaster Deflection mode
    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_POWER):
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

    // LOCKUP
#if NUM_BUTTONS == 1
    // 1 button lockup
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
#elif defined(SABERSENSE_NO_LOCKUP_HOLD)
    // 2 button lockup
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
#else
    case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD, MODE_ON):
#endif

      if (accel_.x < -0.15) {
        SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
      } else {
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
      }
      swing_blast_ = false;
      SaberBase::DoBeginLockup();
      return true;

    // Lightning Block, 1 and 2 button.
    case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_ON):
      SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
      swing_blast_ = false;
      SaberBase::DoBeginLockup();
      return true;

    // Melt
    case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
      SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
      swing_blast_ = false;
      SaberBase::DoBeginLockup();
      return true;

    case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_OFF):
      SaberBase::RequestMotion();
      return true;

    // ENTER VOLUME MENU
#if NUM_BUTTONS == 1
    // 1 button
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
#else
    // 2 button
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_POWER):
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

    // RESTORE FACTORY DEFAULTS
    // Deletes all save files in root and first-level directories.
#ifdef SABERSENSE_ENABLE_RESET
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD, MODE_OFF): {
      // Lock SD card to prevent other operations during deletion.
      LOCK_SD(true);
      const char* filesToDelete[] = {
        "curstate.ini",
        "curstate.tmp",
        "preset.ini",
        "preset.tmp",
        "global.ini",
        "global.tmp"
      };
      // Delete files from the root directory.
      for (const char* targetFile : filesToDelete) {
        if (LSFS::Exists(targetFile)) {
          LSFS::Remove(targetFile);
          Serial.print("Deleted from root: ");
          Serial.println(targetFile);
        }
      }
      // Iterate over the save directories listed in blades[].
      for (const BladeConfig& blade : blades) {
        const char* saveDirName = blade.save_dir; // Replace with the correct field.

        if (saveDirName && strlen(saveDirName) > 0) {
          // Construct the path to the save directory.
          PathHelper saveDirPath("/");
          saveDirPath.Append(saveDirName);

          // Iterate over the files to delete in this save directory.
          for (const char* targetFile : filesToDelete) {
            PathHelper filePath(saveDirPath);
            filePath.Append(targetFile);

            // If the file exists in this directory, delete it.
            if (LSFS::Exists(filePath)) {
              LSFS::Remove(filePath);
              Serial.print("Deleted from ");
              Serial.print(saveDirPath);
              Serial.print(": ");
              Serial.println(targetFile);
            }
          }
        }
      }
      // Unlock SD card after deletion is complete.
      LOCK_SD(false);

      if (SFX_reset) {  // Optional confirmation sound file 'reset'.
        hybrid_font.PlayCommon(&SFX_reset);
        while(IsResetSoundPlaying());  // Lock system while sound finishes.
      } else {
        beeper.Beep(0.5, 2000); // Generate beep to confirm reset.
        delay(800); // Allow beep to play.
      }
      STM32.reset(); // Reboot saber.
    }
    break;
#endif

    // BATTERY LEVEL
    case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
      talkie.SayDigit((int)floorf(battery_monitor.battery()));
      talkie.Say(spPOINT);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
      talkie.Say(spVOLTS);
      SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
      return true;

#ifdef BLADE_DETECT_PIN
    case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
      //  Might need to do something cleaner, but let's try this for now.
      blade_detected_ = true;
      FindBladeAgain();
      SaberBase::DoBladeDetect(true);
      return true;

    case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
      //  Might need to do something cleaner, but let's try this for now.
      blade_detected_ = false;
      FindBladeAgain();
      SaberBase::DoBladeDetect(false);
      return true;
#endif

    // Events that need to be handled regardless of what other buttons are pressed.
    case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      if (SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        return true;
      }
    }
    return false;
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
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
  bool do_font_after_sound_ = false;
  bool pointing_down_ = false;
  bool swing_blast_ = false;
  bool mode_volume_ = false;
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
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
