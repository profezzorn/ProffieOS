/* V7/8-294.
============================================================
=================   SABERSENSE PROP FILE   =================
=================            by            =================
=================       CHRIS CARTER       =================
============================================================

http://fredrik.hubbe.net/lightsaber/proffieos.html
Copyright (c) 2016-2019 Fredrik Hubinette

Sabersense(TM) Prop File for ProffieOS.
https://www.sabersense.co.uk

Prop file Copyright (c) 2024-2026 Chris Carter (Sabersense)
Built on Matthew McGeary's SA22C button prop file for
one and two button replica lightsabers.
Modified by Chris Carter with substantial support and
contributions from Fredrik Hubinette and Brian Conner.

Distributed under the terms of the
GNU General Public License v3.

============================================================
======================= INTRODUCTION =======================

This prop file has been optimized for, and is included
with, ProffieOS 8.x or later, but is also fully
backwards compatible with ProffieOS 7.x.

This prop file references certain custom sound files
to aid in saber function navigation. These sound files
are optional and are available as a free download from
the Sabersense website:
https://sabersense.square.site/downloads

A video explaining the design principles and providing
button control user guides is available on the
Sabersense Youtube Channel:
https://www.youtube.com/watch?v=kqK0EJ9dMZs

============================================================
========= BUTTON SYSTEM PRINCIPLES AND LOGIC NOTES =========

The Sabersense button control system has been engineered
for simplicity and usability. Complex gesture controls and
features like Battle Mode are NOT included.

By default, the Sabersense button prop features harmonized
controls between one-button and two-button operation without
compromising the greater usability of two-button systems.

Where practicable, the same controls apply to both one-button
and two-button systems.

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
      Single click POWER always lights the blade...
        Short click lights blade with sound
        Long click lights blade mute

      Double click POWER always plays a sound file...
        Character Quote or Music track with blade OFF
        Character Quote or Force Effect with blade ON
        Battery level announcement if held down on second click

      Holding down POWER (1-button) or AUX (2-button)
      and waiting with blade OFF always skips to specific preset...
        Hilt pointing up - first preset
        Hilt horizontal - middle preset
        Hilt pointing down - last preset

      Triple-clicking POWER is always a saber management feature...
        Colour change with blade ON
        BladeID/Array Switch with blade OFF

  TWO BUTTON ONLY:
      Short clicking AUX with blade OFF always moves to a different preset,
      forward with hilt pointing up, backwards with hilt pointing down...
        Single short click - one preset
        Double short click - five presets
        Triple short click - ten presets

      Holding POWER and short clicking AUX always enters a control menu...
        Colour change with blade ON
        Volume menu with blade OFF

==========================================================
=================== 1 BUTTON CONTROLS ====================

POWER FUNCTIONS
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
  Skip forward 5 presets    Fast double-click-and-hold, hilt pointing upwards.
  Skip back 5 presets       Fast double-click-and-hold, hilt pointing downwards.
  Skip forward 10 presets   Fast triple-click-and-hold, hilt pointing upwards.
  Skip back 10 presets      Fast triple-click-and-hold, hilt pointing downwards.
  Play Character Quote      Fast double-click, hilt pointing up, plays sequentially. **
  Play Music Track          Fast double-click, hilt pointing down. **
  Speak battery voltage     Fast double-click-and-hold, hilt horizontal.
  Run BladeID/Array Select  Fast triple-click while OFF. (Configurable, applicable installs only).
                              Array Selector is point up for forwards, down for backwards.
  Restore Factory Defaults  Fast four-clicks while OFF, hold on last click.
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

============================================================
===================== 2 BUTTON CONTROLS ====================

POWER FUNCTIONS
  Activate blade            Short click POWER. *
  Activate blade mute       Long click POWER (hold for one second then release).
  Deactivate blade          Press and hold POWER and wait until blade is off.

FUNCTIONS WITH BLADE OFF
  Next preset               Short click AUX, hilt pointing upwards.
  Previous preset           Short click AUX, hilt pointing downwards.
  Previous preset           Hold AUX and short click POWER.
                              (Duplicate legacy command).
  Skip to first preset      Press and hold any button until it switches, hilt upwards.
  Skip to middle preset     Press and hold any button until it switches, hilt horizontal.
  Skip to last preset       Press and hold any button until it switches, hilt downwards.
  Skip forward 5 presets    Fast double-click AUX, hilt pointing upwards.
  Skip back 5 presets       Fast double-click AUX, hilt pointing downwards.
  Skip forward 10 presets   Fast triple-click AUX, hilt pointing upwards.
  Skip back 10 presets      Fast triple-click AUX, hilt pointing downwards.
  Play Character Quote      Fast double-click POWER, hilt pointing up, plays sequentially. **
  Play Music Track          Fast double-click POWER, pointing down. **
  Speak battery voltage     Fast double-click-and-hold POWER.
  Run BladeID/Array Select  Fast triple-click POWER. (Configurable, applicable installs only).
                              Array Selector is point up for forwards, down for backwards.
  Restore Factory Defaults  Fast four-clicks POWER, hold on last click.
                              Release once announcement starts.
  Enter/Exit VOLUME MENU    Hold POWER then quickly click AUX and release both simultaneously.
    Volume up               Click POWER while in VOLUME MENU, hilt pointing up.
    Volume down             Click POWER while in VOLUME MENU, hilt pointing down, OR click
                              AUX while in VOLUME MENU.
                              Volume adjusts in increments per click.
                              You must exit VOLUME MENU to resume using saber normally.

FUNCTIONS WITH BLADE ON
  Blade lockup              Press and hold AUX.
  Blade tip drag            Press and hold AUX while blade is pointing down.
  Play Character Quote      Fast double-click POWER, hilt pointing up, plays sequentially. **
  Force Effect              Fast double-click POWER, hilt pointing down, plays randomly. **
  Lightning block           Double-click POWER and hold.
  Melt                      Hold POWER and stab blade tip against wall. Rotate for heat colours.
  Blaster blocks            Short click AUX. (Add Short click POWER using define).
  Enter multi-blast mode    Hold POWER while swinging for one second and release.
                              Saber will play two quick blasts confirming mode.
                              Swing blade to trigger blaster block.
                              To exit multi-blast mode, fast single click AUX.

COLOUR CHANGE FUNCTIONS WITH BLADE ON
  Enter COLOUR MENU         Hold POWER then quickly click AUX and release both
                            buttons simultaneously. Or fast triple-click POWER.
                              Announcement confirms you are in the COLOUR MENU.
  Cycle to next colour      Rotate hilt whilst in COLOUR MENU until desired colour is reached.
                              Most Sabersense presets have 12 colour options.
  Exit COLOUR MENU          Short click saves new colour and exits.
                            Fast-double-click exits and reverts to original colour.
                              Announcement confirms you are exiting COLOUR MENU.
                              You must exit COLOUR MENU to resume using lightsaber normally.

  *   = Gesture ignitions also available via defines.
  **  = Audio player orientations can be reversed using SABERSENSE_FLIP_AUDIO_PLAYERS define.

===========================================================
=================== SABERSENSE DEFINES ====================

#define SABERSENSE_BLADE_ID
  Replaces regular BladeID with on-demand ID scanning.
  Normal BladeID relies either on continuous ID scanning,
  or it only runs at specific times like bootup
  or when changing fonts. Sabersense BladeID makes
  BladeID scanning available as a button press.
  Triple-click POWER will run the scan, identify the blade
  and switch to its associated array. This system prevents
  spurious ID readings switching blade when you don't want
  it to, especially if using SnapshotID.
  Plays array-specific bladeidX.wav files when switching.

#define SABERSENSE_ARRAY_SELECTOR
  Replaces regular BladeID and allows forwards or
  backwards cycling between different blade/preset
  arrays manually, regardless of actual BladeID status.
  Plays array-specific arrayX.wav files when switching.
  Requires arrays to be numbered consecutively,
  starting at zero, in the field that would
  otherwise contain BladeID values. Like this:
      { 0, ... }
      { 1, ... }
      { 2, ... }
      { 3, ... }
      etc.

#define SABERSENSE_NO_BLADE
  If using SABERSENSE_ARRAY_SELECTOR in combination with
  BLADE_DETECT_PIN, you must use this define in order to
  prevent a compile error.
  Note that you can have any number of Blade-In arrays
  in your config, but only one NO_BLADE array is supported.
  Note also that NO_BLADE replaces the zero array,
  meaning that Blade-In array numbering must be
  consecutive starting at 1. Like this:
      { NO_BLADE, ... }
      { 1, ... }
      { 2, ... }
      { 3, ... }
      etc.

#define SABERSENSE_DEFAULT_BLADE_ARRAY 3
  This feature is really intended for busy installers
  and sets the default blade array in multi-array systems.
  Using this feature, you can have a blade array with all
  the common blade lengths included, and then select which
  length to default to on first boot (i.e. until a save
  file is written).
  The alternative is to simply re-order the blade array
  manually, but then you have to re-order the array1.wav,
  array2.wav etc. files on the SD card to match, which
  is a hassle.
  Note that the define uses zero-based numbering, the
  same as the blade array itself, so you must use the
  number as it is shown in the blade array - i.e. number 3
  for the fourth array down the full list.

#define SABERSENSE_DISABLE_SAVE_ARRAY
  By default, SABERSENSE_ARRAY_SELECTOR saves the current
  array so that the saber will always boot into the last
  array used. This define disables that save functionality.

#define SABERSENSE_ENABLE_ARRAY_FONT_IDENT
  Plays font ident after array ident when switching arrays.
  Can be used with SABERSENSE_BLADE_ID and
  SABERSENSE_ARRAY_SELECTOR to tell user which font they
  have landed on on multiple-array systems in which each
  array has its own save files.

#define SABERSENSE_FLIP_AUDIO_PLAYERS
  Reverses UP/DOWN orientation for playing FORCE, QUOTE and
  MUSIC TRACK audio files. Default (no define) is UP for
  sequential QUOTE with blade ON and OFF, and DOWN for random
  FORCE effect (ON) and music TRACK (OFF). Define acts on
  both ON and OFF states for consistency.
  
#define SABERSENSE_RANDOM_QUOTE
  Makes the playing of quote.wav files random
  instead of sequential.

#define SABERSENSE_BLAST_PWR_AND_AUX
  Adds blaster block button to POWER button as well as AUX
  in 2-button mode. Improves 1 and 2-button harmonization,
  but makes accidental blasts more likely when double-
  clicking POWER for Quotes or Force Effect.

#define SABERSENSE_FONT_SKIP_A 5
#define SABERSENSE_FONT_SKIP_B 10
  As standard, presets can be skipped in batches to aid
  font navigation. Two skip levels are provided, A and B,
  which default to 5 and 10 fonts respectively. These
  defines allow the user to override the default
  values if required.

#define SABERSENSE_HOT_SKIP_DOWN 27
#define SABERSENSE_HOT_SKIP_LEVEL 14
  Hot Skipping is distinct from Font Skipping in that
  it skips directly to a given preset, rather than
  skipping forwards or backwards x number of presets.
  These defines override the Skip-to-Last/Middle
  Preset features, so that the system skips to a
  user-defined preset. Note that Skip-to-First
  preset is fixed and is not user-definable.

#define SABERSENSE_DISABLE_FONT_SKIPPING
  Completely disables all preset skipping, meaning
  presets may only be cycled through one at a time.
  Suitable for installs with small numbers of fonts
  and presets.

#define SABERSENSE_BUTTON_CLICKER
  Button Clicker to play press/release wav files when
  buttons are pressed. Intended for Scavenger hilt
  where wheel makes tactile feel difficult.
  Requires press.wav and release.wav files to work.

#define SABERSENSE_DISABLE_RESET
  By default, all save files can be deleted on demand with
  a button press, effectively restoring 'factory' defaults.
  This define disables that feature.

#define SABERSENSE_NO_COLOR_CHANGE
  Use instead of DISABLE_COLOR_CHANGE.

#define SABERSENSE_NO_LOCKUP_HOLD
  Applicable to two-button mode only, reverts to lockup
  being triggered by clash while holding aux.

#define SABERSENSE_F263_CUSTOM_USER_EFFECT
  Based on Fett263 'Special Abilities', this define
  enables interaction with EFFECT_USERx in standard
  (non-Fett) blade styles for custom effects like
  swing colour changing.

GESTURE CONTROLS
  There are four gesture types: Twist, Stab, Swing and Thrust.
  #define SABERSENSE_TWIST_ON
  #define SABERSENSE_TWIST_OFF
  #define SABERSENSE_STAB_ON
  #define SABERSENSE_SWING_ON
  #define SABERSENSE_THRUST_ON

#define SABERSENSE_GESTURE_PREON
  As standard, gesture controls bypass all preon effects
  unless this define is added which reinstates them.

============================================================
============================================================
*/
#ifndef PROPS_SABER_SABERSENSE_BUTTONS_H
#define PROPS_SABER_SABERSENSE_BUTTONS_H

#ifdef SABERSENSE_ARRAY_SELECTOR

#ifndef SABERSENSE_DEFAULT_BLADE_ARRAY
#ifdef BLADE_DETECT_PIN
#define SABERSENSE_DEFAULT_BLADE_ARRAY 1
#else
#define SABERSENSE_DEFAULT_BLADE_ARRAY 0
#endif
#endif

  // Check user-defined array is valid at compile time.
  static_assert(
    SABERSENSE_DEFAULT_BLADE_ARRAY < NELEM(blades),
    "SABERSENSE_DEFAULT_BLADE_ARRAY must reference a valid array (note zero-based counting)."
  );

#ifdef BLADE_DETECT_PIN
  static_assert(
    SABERSENSE_DEFAULT_BLADE_ARRAY != 0,
    "[Sabersense] ERROR: Default array index must be 1 or higher when using Blade Detect."
  );
  constexpr int min_blades_required = 3;
#else
  constexpr int min_blades_required = 2;
#endif

  // Check that Array Selector is actually needed (prevents possible unexpected behaviour).
  static_assert(
    NELEM(blades) >= min_blades_required,
    "[Sabersense] ERROR: Array Selector is not required with only one selectable blade array. "
    "Please undefine SABERSENSE_ARRAY_SELECTOR."
  );

#ifndef SABERSENSE_DISABLE_SAVE_ARRAY
class SaveArrayStateFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(sabersense_array_index, SABERSENSE_DEFAULT_BLADE_ARRAY);
  }
  int sabersense_array_index;
};
#endif

  struct SabersenseArraySelector {
    static int return_value;
    float id() {
      if (return_value < 0 || return_value >= NELEM(blades)) {
        Serial.println("[Sabersense] ALERT: User or externally-specified array index invalid. "
                       "Resetting to default.");
        return_value = SABERSENSE_DEFAULT_BLADE_ARRAY;
      }
      return return_value;
    }

    static void cycle_array(bool forward) {
      int size = NELEM(blades);
      int offset = 0;
#ifdef BLADE_DETECT_PIN
      offset = 1;
      size--;
#endif
      return_value += forward ? 1 : (size - 1);
      return_value = (return_value + size - offset) % size + offset;
    }
  };

    // Set initial index based on Blade Detect mode.
#ifndef BLADE_DETECT_PIN
    int SabersenseArraySelector::return_value = 0;
#else
    int SabersenseArraySelector::return_value = 1;
#endif

#undef BLADE_ID_CLASS_INTERNAL
#define BLADE_ID_CLASS_INTERNAL SabersenseArraySelector
#undef BLADE_ID_CLASS
#define BLADE_ID_CLASS SabersenseArraySelector
#endif  // SABERSENSE_ARRAY_SELECTOR

#include "prop_base.h"

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

#ifndef SABERSENSE_FONT_SKIP_A
#define SABERSENSE_FONT_SKIP_A 5
#endif

#ifndef SABERSENSE_FONT_SKIP_B
#define SABERSENSE_FONT_SKIP_B 10
#endif

// Hot Skip values are placeholders only.
#ifndef SABERSENSE_HOT_SKIP_DOWN
#define SABERSENSE_HOT_SKIP_DOWN 0
#endif

#ifndef SABERSENSE_HOT_SKIP_LEVEL
#define SABERSENSE_HOT_SKIP_LEVEL 0
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
#ifndef MODES_MODE_H  // Modes not part of OS7.x, so define used to differentiate systems.
EFFECT(quote);    // for playing quotes with ProffieOS 7.x.
#endif

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
  // Prop only permits one of these defines at a time.
#if defined(SABERSENSE_BLADE_ID) || defined(SABERSENSE_ARRAY_SELECTOR)
  if (do_font_after_sound_
#ifdef SABERSENSE_BLADE_ID
      && !GetWavPlayerPlaying(&SFX_bladeid)
#endif
#ifdef SABERSENSE_ARRAY_SELECTOR
      && !GetWavPlayerPlaying(&SFX_array)
#endif
  ) {
      SaberBase::DoNewFont();
      do_font_after_sound_ = false;
    }
#endif
  }

  // PRESET SKIPPING
  // Skip Value definable.
  int GetNumberOfPresets() {
    CurrentPreset tmp;
    tmp.SetPreset(-1);
    return tmp.preset_num + 1;
  }

  // Two Button system: Up/Down only - loose angle settings for ease of use.
  bool MultiFontSkip(int skip_value) {
    float angle = fusor.angle1();
    int delta = 0;

#if NUM_BUTTONS == 2
    delta = (angle < -M_PI / 4) ? -skip_value : skip_value;
#elif NUM_BUTTONS == 1
    if (angle < -M_PI / 6) {  // Pointing down
      delta = -skip_value;
    } else if (angle > M_PI / 6) {  // Pointing up
      delta = skip_value;
    } else {
      SpeakBatteryLevel();
      return true;
    }
#endif
    int count = GetNumberOfPresets();
    int new_index = current_preset_.preset_num + delta;
    // Capping/Clamping, not wrapping - less disorientating for user.
    if (new_index < 0) new_index = 0;
    if (new_index >= count) new_index = count - 1;

#ifdef SAVE_PRESET
    SaveState(new_index);
#endif

    SetPreset(new_index, true);
    return true;
  }

  // VOLUME MENU
  void VolumeUp() {
    STDOUT.println("Volume up");
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME * 1.1,
          dynamic_mixer.get_volume() + VOLUME * 0.10));
      if (SFX_volup) {
        hybrid_font.PlayCommon(&SFX_volup);
      } else {
        beeper.Beep(0.5, 2000);
      }
      STDOUT.print("Volume Up - Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      if (SFX_volmax) {
        hybrid_font.PlayCommon(&SFX_volmax);
      } else {
        beeper.Beep(0.5, 3000);
      }
      STDOUT.print("Maximum Volume: ");
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
      if (SFX_volmin) {
        hybrid_font.PlayCommon(&SFX_volmin);
      } else {
        beeper.Beep(0.5, 1000);
      }
      STDOUT.print("Minimum Volume: ");
    }
  }

  // BATTERY LEVEL INDICATOR
  void SpeakBatteryLevel() {
      talkie.SayDigit((int)floorf(battery_monitor.battery()));
      talkie.Say(spPOINT);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
      talkie.Say(spVOLTS);
      SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
  }

  // BLADE ID OPTIONS AND ARRAY MANAGEMENT
  // True BladeID, runs scan on-demand with unique 'bladeid' ident sound.
#ifdef SABERSENSE_BLADE_ID
#ifndef ENABLE_POWER_FOR_ID
#error "SABERSENSE_BLADE_ID requires ENABLE_POWER_FOR_ID to be defined."
#endif

  void TriggerBladeID() {
    FindBladeAgain();
#ifdef SABERSENSE_ENABLE_ARRAY_FONT_IDENT  // Plays 'bladeid' sound AND 'font' sound.
    SFX_bladeid.Select(current_config - blades);
    hybrid_font.PlayCommon(&SFX_bladeid);
    // Calls Loop function to handle waiting for effect before running DoNewFont.
    do_font_after_sound_ = true;
  }
#else
    // Plays 'bladeid' sound only, or 'font' sound if no 'bladeid' sound available.
    if (SFX_bladeid) {
      SFX_bladeid.Select(current_config - blades);
      hybrid_font.PlayCommon(&SFX_bladeid);  // Play 'bladeid' sound file if available.
      int saved_volume = SFX_font.GetVolume();
      SFX_font.SetVolume(0);  // Mute volume for font ident so that system can...
      SaberBase::DoNewFont();  // ...run DoNewFont to ensure various initializations.
      SFX_font.SetVolume(saved_volume);
    } else {
      SaberBase::DoNewFont();  // Play font ident if 'bladeid' sound file missing.
    }
  }
#endif
#endif

  // Manual Array Selector, switches on-demand to next array, plays 'array' ident sound.
#ifdef SABERSENSE_ARRAY_SELECTOR

#ifdef SABERSENSE_BLADE_ID  // Only one Sabersense BladeID standard permitted.
#error "SABERSENSE_ARRAY_SELECTOR and SABERSENSE_BLADE_ID cannot be defined at the same time."
#endif

#ifdef BLADE_DETECT_PIN
#ifndef SABERSENSE_NO_BLADE  // Means of informing users they can only use one NO_BLADE array.
#error "BLADE_DETECT_PIN requires SABERSENSE_NO_BLADE to be defined and is limited to one NO_BLADE array."
#error "SABERSENSE_NO_BLADE allows only one NO_BLADE array but any number of Blade-In arrays."
#endif
#endif

#ifndef SABERSENSE_DISABLE_SAVE_ARRAY
  void SaveArrayState() {
    STDOUT.print("Saving Array State\n");
    SaveArrayStateFile saved_state;
    saved_state.sabersense_array_index = SabersenseArraySelector::return_value;  // Save current array.
    saved_state.WriteToRootDir("arraysve");
  }

  void RestoreArrayState() {
    STDOUT.print("Restoring Array State\n");
    SaveArrayStateFile saved_state;
    saved_state.ReadINIFromDir(NULL, "arraysve");
    // Restore saved array index.
    SabersenseArraySelector::return_value = saved_state.sabersense_array_index;
  }

  void Setup() {
    RestoreArrayState();  // Load saved array on boot.
  }
#endif

  void PlayArraySound() {
#ifdef SABERSENSE_ENABLE_ARRAY_FONT_IDENT  // Plays 'array' sound AND 'font' sound.
    SFX_array.Select(current_config - blades);
    hybrid_font.PlayCommon(&SFX_array);
    // Calls Loop function to handle waiting for effect before running DoNewFont.
    do_font_after_sound_ = true;
  }
#else
    // Plays 'array' sound only, or 'font' sound if no 'array' sound available.
    if (SFX_array) {
      SFX_array.Select(current_config - blades);
      hybrid_font.PlayCommon(&SFX_array);  // Play 'array' sound file if available.
      int saved_volume = SFX_font.GetVolume();
      SFX_font.SetVolume(0);  // Mute volume for font ident so that system can...
      SaberBase::DoNewFont();  // ...run DoNewFont to ensure various initializations.
      SFX_font.SetVolume(saved_volume);
    } else {
      SaberBase::DoNewFont();  // Play font ident if 'array' sound file missing.
    }
  }
#endif
#endif

// RESET FACTORY DEFAULTS (Delete Save Files).
// Script to determine if sound effects have finished.
#ifndef SABERSENSE_DISABLE_RESET
  bool IsSoundPlaying(const Effect* sound) {
    return !!GetWavPlayerPlaying(sound);
  }
#endif

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
      // Intended to play click sound on button presses, primarily for Scavenger
      // hilt where wheel makes tactile switch feel difficult.
      PlaySound("press.wav");  // Requires press.wav file to work.
#endif
      return false;
    case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
    case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
#ifdef SABERSENSE_BUTTON_CLICKER
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
#ifdef SABERSENSE_GESTURE_PREON
        On();
#else
        FastOn();
#endif    
      }
      return true;
#endif

#ifdef SABERSENSE_TWIST_ON
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
      // Delay twist events to prevent false trigger from over twisting
      if (millis() - last_twist_ > 2000 && millis() - saber_off_time_ > 1000) {
#ifdef SABERSENSE_GESTURE_PREON
        On();
#else
        FastOn();
#endif  
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
#ifdef SABERSENSE_GESTURE_PREON
        On();
#else
        FastOn();
#endif  
      }
      return true;
#endif

#ifdef SABERSENSE_THRUST_ON
    case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
      if (millis() - saber_off_time_ > 1000) {
#ifdef SABERSENSE_GESTURE_PREON
        On();
#else
        FastOn();
#endif  
      }
      return true;
#endif

    // Based on Fett263 'Special Abilities'.
#ifdef SABERSENSE_F263_CUSTOM_USER_EFFECT
#if NUM_BUTTONS >= 1
    case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON | BUTTON_POWER):
      SaberBase::DoEffect(EFFECT_USER5, 0);
      return true;

    case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON | BUTTON_POWER):
      SaberBase::DoEffect(EFFECT_USER6, 0);
      return true;      

    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_POWER):
      SaberBase::DoEffect(EFFECT_USER7, 0);
      return true;  
#endif

#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON | BUTTON_AUX):
      SaberBase::DoEffect(EFFECT_USER3, 0);
      return true;

    case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON | BUTTON_AUX):
      SaberBase::DoEffect(EFFECT_USER4, 0);
      return true;

    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_AUX):
      SaberBase::DoEffect(EFFECT_USER1, 0);
      return true;

    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_AUX):
      SaberBase::DoEffect(EFFECT_USER2, 0);
      return true;
#endif
#endif  // SABERSENSE_F263_CUSTOM_USER_EFFECT

    // MAIN ACTIVATION
    // Saber ON AND Volume Adjust, 1 and 2 Button.
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      IgnoreClash(100);  // Hopefully prevents false clashes due to 'clicky' button.
                         // Low threshold so as not to conflict with 1-button volume menu access.
      if (!mode_volume_) {
        On();
      } else {
#if NUM_BUTTONS == 1
        if (fusor.angle1() > 0) {
          VolumeUp();
        } else {
          VolumeDown();
        }
      }
#else
        VolumeUp();
      }
#endif
      return true;

    // 1 Button Activate Muted and next/previous preset.
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      IgnoreClash(100);  // Hopefully prevents false clashes due to 'clicky' button.
                         // Low threshold so as not to conflict with 1-button volume menu access.
      if (fusor.angle1() > M_PI / 6) {
        // Pointing up
        next_preset();
      } else if (fusor.angle1() < -M_PI / 6) {
        // Pointing down
        previous_preset();
      } else {
        // Horizontal
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
#ifndef SABERSENSE_NO_COLOR_CHANGE
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
      // backwards if pointing down (anything ≤ 0 is down)
      if (!mode_volume_) {
        SetPreset(current_preset_.preset_num + (fusor.angle1() > 0 ? 1 : -1), true);
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

    // PRESET SKIPPING
    // Skips forward when pointing up, backward when pointing down.
    // Uses SABERSENSE_FONT_SKIP_A/B for skip values.
#ifndef SABERSENSE_DISABLE_FONT_SKIPPING
#if NUM_BUTTONS == 1
    // First skip value (define A - default 5)
    case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF): {
      return MultiFontSkip(SABERSENSE_FONT_SKIP_A);
    }
    // Second skip value (define B - default 10)
    case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD_MEDIUM, MODE_OFF): {
      return MultiFontSkip(SABERSENSE_FONT_SKIP_B);
    }
#else
    // First skip value (define A - default 5)
    case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF): {
      return MultiFontSkip(SABERSENSE_FONT_SKIP_A);
    }
    // Second skip value (define B - default 10)
    case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF): {
      return MultiFontSkip(SABERSENSE_FONT_SKIP_B);
    }
#endif

    // Skips to first preset (up), last or user-defined preset (down),
    // or middle or user-defined preset (horizontal [level]):
#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_OFF):
#endif
    case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF): {
      CurrentPreset tmp;
      tmp.SetPreset(-1);
      int num_presets = tmp.preset_num + 1;

      int target_preset;
      float angle = fusor.angle1();

      if (angle > M_PI / 6) {
        target_preset = 0;
      } else if (angle < -M_PI / 6) {
        // Get and check user values. Clamp to last preset if invalid.
        int hot_skip_down = (SABERSENSE_HOT_SKIP_DOWN > 0) ? (SABERSENSE_HOT_SKIP_DOWN - 1) : -1;
        if (hot_skip_down >= num_presets || hot_skip_down < 0) {
          hot_skip_down = num_presets - 1;
        }

        target_preset = hot_skip_down;
      } else {
        int hot_skip_level = (SABERSENSE_HOT_SKIP_LEVEL > 0) ? (SABERSENSE_HOT_SKIP_LEVEL - 1) : -1;
        if (hot_skip_level >= num_presets || hot_skip_level < 0) {
          hot_skip_level = num_presets / 2;
        }

        target_preset = hot_skip_level;
      }

#ifdef SAVE_PRESET
      SaveState(target_preset);
#endif
      SetPreset(target_preset, true);
      break;
    }
#endif

    // BLADE ID OPTIONS AND ARRAY NAVIGATION
    // Blade ID on-demand scanning with BladeID audio idents.
#ifdef SABERSENSE_BLADE_ID
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      TriggerBladeID();
      return true;
#endif

    // Manual blade array selector.
#ifdef SABERSENSE_ARRAY_SELECTOR
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      // Check for blade present if using Blade Detect.
#ifdef BLADE_DETECT_PIN
      if (!blade_detected_) return true; // Do nothing if no blade detected.
#endif
      {
        // Cycles through blade arrays regardless of BladeID status.
        bool forward = fusor.angle1() > 0;
        SabersenseArraySelector::cycle_array(forward);
      }
      FindBladeAgain();
      PlayArraySound();
#ifndef SABERSENSE_DISABLE_SAVE_ARRAY
      SaveArrayState();
#endif
      return true;
#endif

    // SOUND EFFECT PLAYERS.
    // With Blade ON - UP for Character Quote, plays sequentially.
    // With Blade ON - DOWN for Force Effect, plays randomly.
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      if (SFX_quote) {
#ifndef SABERSENSE_FLIP_AUDIO_PLAYERS
      //  Define reverses UP/DOWN options for all QUOTE/FORCE/TRACK audio player.
      //  Quote player points upwards.
        if (fusor.angle1() > 0)
#else
      // Quote player points downwards.
        if (fusor.angle1() < 0)
#endif
        {
#ifndef SABERSENSE_RANDOM_QUOTE
          SFX_quote.SelectNext();
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
#else
          SaberBase::DoEffect(EFFECT_QUOTE, -1);  // Repetition and '-1' required for OS-7.
#endif
        } else {
          SaberBase::DoForce();  // Force effect for hilt pointed DOWN.
        }
      } else {
        SaberBase::DoForce();  // Fallback: play force effect if no quotes are available.
      }
      return true;

    //  With Blade OFF - UP for Character Quote, plays sequentially.
    //  With Blade OFF - DOWN for Music Track.
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      if (SFX_quote) {
#ifndef SABERSENSE_FLIP_AUDIO_PLAYERS
      // Define reverses UP/DOWN options for all QUOTE/FORCE/TRACK audio players.
      //  Quote player points upwards.
        if (fusor.angle1() > 0) {
#else
      // Quote player points downwards.
        if (fusor.angle1() < 0) {
#endif
#ifndef SABERSENSE_RANDOM_QUOTE
          SFX_quote.SelectNext();
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
#else
          SaberBase::DoEffect(EFFECT_QUOTE, -1);  // Repetition and '-1' required for OS-7.
#endif
        } else {
          StartOrStopTrack();  // Play track for hilt pointed DOWN.
        }
      } else {
        StartOrStopTrack();  // Fallback: play track if no quotes are available.
      }
      return true;

    // COLOUR CHANGE.
    // Colour Change and Blast OS-7 Compatibility Section.
    // Designed for identical user operation under both
    // OS-7 (without Modes) and OS-8 (with Modes).
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
    // 1 Button (and 2 button with extra define).
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
      // For harmonized 'Exit Colour Menu' control in OS-7.x. Ignored in OS-8 due to Modes.
      ToggleColorChangeMode();
      return true;
    }

#if NUM_BUTTONS == 1 || defined(SABERSENSE_BLAST_PWR_AND_AUX)
      // If NOT in color change mode and 1 button system (or 2 button with define) do blast.
      swing_blast_ = false;
      SaberBase::DoBlast();
#endif
      return true;

#if NUM_BUTTONS == 2
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
      swing_blast_ = false;
      SaberBase::DoBlast();
      return true;
#endif

    // Multi-Blaster Deflection mode
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
#ifndef SABERSENSE_DISABLE_RESET
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD, MODE_OFF): {
      // Lock SD card to prevent other operations during deletion.
      LOCK_SD(true);
      const char* filesToDelete[] = {
        "curstate.ini",
        "curstate.tmp",
        "presets.ini",
        "presets.tmp",
        "global.ini",
        "global.tmp",
        "arraysve.ini",
        "arraysve.tmp"
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
        const char* saveDirName = blade.save_dir; // Replace with correct field.

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
              STDOUT << "Deleted from "
                  << saveDirPath << ": "
                  << targetFile <<"\n";
            }
          }
        }
      }
      // Unlock SD card after deletion is complete.
      LOCK_SD(false);

      if (SFX_reset) {  // Optional confirmation sound file 'reset'.
        hybrid_font.PlayCommon(&SFX_reset);
        while(IsSoundPlaying(&SFX_reset));  // Lock system while sound finishes.
      } else {
        beeper.Beep(0.5, 2000); // Generate beep to confirm reset.
        delay(800); // Allow beep to play.
      }
      STM32.reset(); // Reboot saber.
    }
    break;
#endif

    // 1 Button feature handled in Preset Skipping unless skipping disabled.
#if (NUM_BUTTONS == 2) || (NUM_BUTTONS == 1 && defined(SABERSENSE_DISABLE_FONT_SKIPPING))
    case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
      SpeakBatteryLevel();
      return true;
#endif

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

#ifndef MODES_MODE_H  // Modes not part of OS7.x, so define used to differentiate systems.
  void SB_Effect(EffectType effect, float location) override  //  Required for ProffieOS 7.x.
#else
  void SB_Effect(EffectType effect, EffectLocation location) override  //  For ProffieOS 8.x.
#endif
  {
    switch (effect) {
#ifndef MODES_MODE_H
      case EFFECT_QUOTE: hybrid_font.PlayCommon(&SFX_quote); return;
#endif
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
