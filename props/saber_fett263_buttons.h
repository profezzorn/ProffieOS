/* Fett263 Buttons for use with 2 Button Sabers (PWR, AUX)
Includes Gesture Controls, Battle Mode 2.0, Edit Mode, Track Player, Quote/Force Player, Real Clash, Choreography Mode
   Dual Mode Ignition Sounds, Multi-Phase Control, Multi-Blast

 ProffieOS: Control software for lightsabers and other props.
 http://fredrik.hubbe.net/lightsaber/teensy_saber.html
 Copyright (c) 2016-2019 Fredrik Hubinette

 Fett263 Button (prop) file, "Battle Mode 2.0", "Edit Mode", "Track Player", "Real Clash", "Choreography Mode", "Dual Mode Ignition",
 "Multi-Phase", "Multi-Blast"
 Copyright (c) 2020-2021 Fernando da Rosa

2 Button Control (this file does not support 1 button)

Standard Controls While Blade is OFF
  Turn On / Ignite Saber = Click PWR
  Change Preset (one at a time*) = Click AUX 
    *if pointing down will go to previous
  NEW! Scroll Presets (using twist menu) = Long Click AUX
    Turn Right = Next Preset
    Turn Left = Previous Preset
    Click PWR = Select Preset
    Click AUX = go to First Preset
  Play Track = Long Click PWR pointing up
  NEW! Track Player = Long Click PWR parallel
    Turn Right = Next Track
    Turn Left = Previous Track
    Click PWR = Play Current Track Once
    Click AUX = Random (will play current track and then randomly select next tracks)
    Hold PWR + Turn Right = Rotate (will play current track and then next sequential tracks)
    Hold PWR + Turn Left = Loop Current Track
    Long Click PWR = Stop Track Player
  NEW! Toggle Gesture Sleep* = Hold PWR + Twist
    *toggles gesture controls on/off
    *gestures sleep automatically if Blade Detect is enabled and blade is missing
  NEW Control! Volume Menu = Hold PWR, Click AUX
    Turn Right = Increase Volume (to max)
    Turn Left = Decrease Volume (to min)
    Click PWR or AUX = Exit
  Check Battery Level*  = Hold AUX, Click PWR
    *requires EFFECT_BATTERY_LEVEL style and/or FETT263_SAY_BATTERY define
Optional Gesture Controls (if enabled and Gesture Sleep is deactivated)
  Ignite Saber
    Swing On
    Stab On
    Twist On
    Thrust On

Standard Controls While Blade is ON
  Turn Off / Retract Blade* = Click PWR
    *if PowerLock is disabled
  Turn Off / Retract Blade (PowerLock Mode) = Hold PWR + Hold AUX
  Blast Effect = Click Aux
  Multi-Blast Mode = Long Click Aux
    Each Swing in Multi-Blast Mode will deflect Blast effect
    To exit, click AUX or do Clash
  Clash Effect = Clash Saber
  Stab Effect = Stab (thrust and impact tip of blade on object)
  Lockup Effect = Hold PWR + Clash Saber
  Drag Effect = Hold AUX + Stab Down
  Melt Effect = Hold AUX + Stab Parallel or Up
  Lightning Block Effect = Hold PWR + click AUX
  NEW! Force/Quote = Long Click PWR (parallel or down)
    If pointing down will toggle Force/Quote and play
    *Quote plays sequentially
    If parallel will do Force/Quote
  Start/Stop Tracks = Long Click PWR (pointing up)
    *default track only (use Track Player while OFF to select tracks or playback modes)
  Color Change = Hold AUX + Click PWR (parallel or down)
    Rotate Hilt to select color (unless ColorChange click to Change style is active)
    Click PWR to save
  NEW! Hold PWR to enter ZOOM mode to fine-tune color, Release PWR to save
  Power Save* = Hold AUX + Click PWR (pointing up)
    *requires EFFECT_POWERSAVE in style
  Multi-Phase Preset Change*
    *requires FETT263_MULTI_PHASE define
    Hold AUX + Twist =  Next Preset
    Hold PWR + Twist = Previous Preset
Optional Gesture Controls (if enabled)
  Retract Blade
    Twist Off

"Battle Mode" Controls* - While ON
    *may vary by defines
  Enter/Exit Battle Mode = Hold AUX
  Clash / Lockup = controlled by gesture
    Clash blade
      If blade swings through the clash it will do a "glancing Clash"
      If blade stops/slows on clash the saber will initiate Begin Lockup
      To perform a "clash" do an immediate Pull Away this will transition from Begin Lockup to End Lockup in quick succession
      To Lockup, steady the blade after Clash
      To end Lockup do Pull Away
  Drag / Melt = controlled by gesture
    Stab (thrust with impact at tip of blade)
      If pointing down Drag will initiate
      To end Drag pull blade up from floor at an angle
      If parallel or up Melt will initiate
      To end Melt pull blade away from object at an angle
  Blast Effect = Click AUX 
    NEW! swing within 2 seconds to enter Multi-Blast Mode
  Multi-Blast Mode = Long Click AUX
    Each Swing in Multi-Blast Mode will deflect Blast effect
    To exit, click AUX or do Clash
  Lightning Block = Hold PWR, Click AUX
  Force Push* = Push Saber
    *requires FETT263_FORCE_PUSH
  NEW! Force/Quote = Long Click PWR (parallel or down)
    If pointing down will toggle Force/Quote mode and do Force Effect or play Quote accordingly
    *Quote plays sequentially
    If parallel will do Force/Quote
  Start/Stop Tracks = Long Click PWR (pointing up)
    *default track only (use Track Player while OFF to select tracks or playback modes)

Rehearsal / Choreography Modes*
  *requires FETT263_SAVE_CHOREOGRAPHY define
  Begin Rehearsal = While Off, Hold AUX + Twist
    Saber will Ignite in Rehearsal Mode
    In Rehearsal Mode, standard Clash and Lockup controls are used to record sequence
  Clash = Clash
  Hold PWR + Clash = Lockup
    Rehearsal will also record the sound files used for each effect to repeat in Choreography
  Cancel Rehearsal Mode = Hold AUX
  Save Rehearsal = Hold PWR
  Begin Choreography = While Off, Hold AUX - or - Hold AUX + Swing
    During Choreography Mode Clashes, Lockups and sound files are replayed in sequence
    When recorded sequence completes the saber goes into Battle Mode automatically
    During Choreography PWR button is disabled
  Turn Off = Hold AUX + Hold PWR

Edit Mode*
    *requires FETT263_EDIT_MODE_MENU & ENABLE_ALL_MENU_OPTIONS defines
    *requires /common folder with all menu prompt sounds
  Enter Edit Mode = While Off, Hold AUX + Hold PWR
    If menu prompt wav files are missing from preset you will get "Error in Font Directory" warning, refer to Edit Mode setup and requirements
  
  While in Edit Mode controls are as follows:
    Rotate Forward, Increase Value, Confirm "Yes" = Turn Right
    Rotate Back, Decrease Value, Confirm "No" = Turn Left
    Select, Save, Enter = Click PWR
    Cancel, Revert, Go Back = Click AUX
  To Exit Edit Mode - Hold AUX

  "Edit Color" Additional Control
    "Color List" and "Adjust Color Hue" Zoom Mode = Long Click PWR

OPTIONAL DEFINES (added to CONFIG_TOP in config.h file)

  FETT263_EDIT_MODE_MENU
  Enable Edit Mode Menu System
  Requires ENABLE_ALL_MENU_OPTIONS

  FETT263_SAVE_CHOREOGRAPHY
  Enables Enhanced Battle Mode with Saved Choreography

  FETT263_DUAL_MODE_SOUND
  Enables odd/even out.wav ignition sound selection based on blade angle
  Up = odd number sounds, Down = even numbered sounds

  FETT263_CLASH_STRENGTH_SOUND
  Enables selection of clash, stab and lockup sounds based on clash strength
  Light clash = 01.wav, Hard clash = highest number.wav

  FETT263_MAX_CLASH 16
  The value for hardest clash level to select clash sound
  Range 8 ~ 16

  == BATTLE MODE OPTIONS ==
    Battle Mode is enabled via controls by default in this prop, you can customize further with these defines

    FETT263_BATTLE_MODE_ALWAYS_ON - Battle Mode is always on, toggle controls deactivated
      This will disable traditional Clash and Stab effects (cannot be used with FETT263_BATTLE_MODE_START_ON)

    FETT263_BATTLE_MODE_START_ON - Battle Mode is active with each ignition by default but can be toggled using Aux + Swing control
      (cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON)

    FETT263_LOCKUP_DELAY 200
      This is the "delay" in millis to determine Clash vs Lockup

    FETT263_BM_CLASH_DETECT 4
      The max value to use clashes in Battle Mode 2.0, clashes used on clash strength below this value
      This allows light clashes to produce clash effects instead of using Begin/End Lockup
      (above this value Clash is performed by quick pull away using Begin/End Lockup sounds and effect)
      Range 0 ~ 6 (note 0 will use Battle Mode 1.0 with all clashes being Begin/End Lockup)

    FETT263_BM_DISABLE_OFF_BUTTON
      During Battle Mode Power Button Retraction is disabled

  == Swing On ==
    Gesture Ignition via Swing
    You can use one of the following defines to enable swing on:

      FETT263_SWING_ON - To enable Swing On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_SWING_ON_PREON - Disables Fast On ignition for Swing On so Preon is used (cannot be used with FETT263_SWING_ON)

    FETT263_SWING_ON_NO_BM - To enable Swing On Ignition control but not activate Battle Mode
        (Combine with FETT263_SWING_ON or FETT263_SWING_ON_PREON, 
        cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

    FETT263_SWING_ON_SPEED 250
      Adjust Swing Speed required for Ignition 250 ~ 500 recommended

  == Twist Off ==
    Gesture Retraction via Twist (back and forth)

    FETT263_TWIST_OFF
    To enable Twist Off Retraction control

  == Twist On ==
    Gesture Ignition via Twist (back and forth)
    You can use one of the following defines to enable twist on:

      FETT263_TWIST_ON - To enable Twist On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_TWIST_ON_PREON - Disables Fast On ignition for Twist On so Preon is used (cannot be used with FETT263_TWIST_ON)

    FETT263_TWIST_ON_NO_BM - To enable Twist On Ignition control but not activate Battle Mode
      (Combine with FETT263_TWIST_ON or FETT263_TWIST_ON_PREON, 
      cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

  == Stab On ==
    Gesture Ignition via Stab (linear movement + clash at tip of blade)
    You can use one of the following defines to enable stab on:

      FETT263_STAB_ON - To enable Stab On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_STAB_ON_PREON - Disables Fast On ignition for Stab On so Preon is used (cannot be used with FETT263_STAB_ON)

    FETT263_STAB_ON_NO_BM - To enable Stab On Ignition control but not activate Battle Mode
    (Combine with FETT263_STAB_ON or FETT263_STAB_ON_PREON, 
    cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

  == Thrust On ==
    Gesture Ignition via Thrust (linear movement)
    You can use one of the following defines to enable thrust on:

      FETT263_THRUST_ON - To enable Thrust On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_THRUST_ON_PREON - Disables Fast On ignition for Thrust On so Preon is used (cannot be used with FETT263_THRUST_ON)

    FETT263_THRUST_ON_NO_BM - To enable Thrust On Ignition control but not activate Battle Mode 
      (Combine with FETT263_THRUST_ON or FETT263_THRUST_ON_PREON, 
      cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

  == Gesture Sleep ==
  Toggle Gesture Ignition and Retraction detection to disable or enable gesture options

  FETT263_SAVE_GESTURE_OFF - Save "Gesture Sleep" setting to turn gesture ignitions and retractions off on boot

  == Force Push ==
    Push movement triggers push.wav (or force.wav if push.wav is not present)
    You can use one of the following defines to enable force push:

      FETT263_FORCE_PUSH - To enable gesture controlled Force Push during Battle Mode
        (will use push.wav or force.wav if not present)

      FETT263_FORCE_PUSH_ALWAYS_ON - To enable gesture controlled Force Push full time
        (will use push.wav or force.wav if not present)

    FETT263_FORCE_PUSH_LENGTH 5 - Allows for adjustment to Push gesture length in millis needed to trigger Force Push
      Recommended range 1 ~ 10, 1 = shortest, easiest to trigger, 10 = longest

  FETT263_MULTI_PHASE
  This will enable a preset change while ON to create a "Multi-Phase" saber effect

  MOTION_TIMEOUT 60 * 15 * 1000
  This extends the motion timeout to 15 minutes to allow gesture ignition to remain active
  Increase/decrease the "15" value as needed

CUSTOM SOUNDS SUPPORTED (add to font to enable):

  On Demand Power Save - dim.wav
  On Demand Battery Level - battery.wav
  Battle Mode On (on toggle) - bmbegin.wav
  Battle Mode Off (on toggle) - bmend.wav
  Enter Volume Menu - vmbegin.wav
  Exit Volume Menu - vmend.wav
  Force Push - push.wav
  Fast On (optional) - faston.wav
  Multi-Blast Mode On - blstbgn.wav
  Multi-Blast Mode Off - blstend.wav
  Quotes - quote01.wav
*/

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

#ifndef FETT263_BM_CLASH_DETECT
#define FETT263_BM_CLASH_DETECT 0
#endif

#ifndef FETT263_FORCE_PUSH_LENGTH
#define FETT263_FORCE_PUSH_LENGTH 5
#endif

#if NUM_BUTTONS < 2
#error /props/saber_fett263_buttons.h requires 2 buttons for operation
#endif

#if defined(FETT263_EDIT_MODE_MENU) && !defined(ENABLE_ALL_MENU_OPTIONS)
#error ENABLE_ALL_MENU_OPTIONS must be defined to enable FETT263_EDIT_MODE_MENU
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

#if defined(FETT263_CLASH_STRENGTH_SOUND) && !defined(FETT263_MAX_CLASH)
#define FETT263_MAX_CLASH 16
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
#include "../sound/effect.h"
#include "../common/current_preset.h"

#ifdef FETT263_EDIT_MODE_MENU
#include "../common/file_reader.h"
#include "../common/malloc_helper.h"
#include "../common/color.h"
#include "../styles/edit_mode.h"
#endif

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
EFFECT(mnum); // menu numbers
EFFECT(quote); // quote on force effect
#ifdef FETT263_EDIT_MODE_MENU
EFFECT(medit); // Edit Mode
#endif

class GestureControlFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(gestureon, 1);
#ifdef SWING_GESTURE
    CONFIG_VARIABLE(swingon, 1);
#else
    CONFIG_VARIABLE(swingon, 0);
#endif
    CONFIG_VARIABLE(swingonspeed, FETT263_SWING_ON_SPEED);
#ifdef TWIST_GESTURE
    CONFIG_VARIABLE(twiston, 1);
#else
    CONFIG_VARIABLE(twiston, 0);
#endif
#ifdef THRUST_GESTURE
    CONFIG_VARIABLE(thruston, 1);
#else
    CONFIG_VARIABLE(thruston, 0);
#endif
#ifdef STAB_GESTURE
    CONFIG_VARIABLE(stabon, 1);
#else
    CONFIG_VARIABLE(stabon, 0);
#endif
#ifdef FETT263_TWIST_OFF
    CONFIG_VARIABLE(twistoff, 1);
#else
    CONFIG_VARIABLE(twistoff, 0);
#endif
#ifdef FETT263_POWER_LOCK
    CONFIG_VARIABLE(powerlock, 1);
#else
    CONFIG_VARIABLE(powerlock, 0);
#endif
#ifdef FORCE_PUSH_CONDITION
    CONFIG_VARIABLE(forcepush, 1);
#else
    CONFIG_VARIABLE(forcepush, 0);
#endif
    CONFIG_VARIABLE(forcepushlen, FETT263_FORCE_PUSH_LENGTH);
    CONFIG_VARIABLE(lockupdelay, FETT263_LOCKUP_DELAY);
    CONFIG_VARIABLE(clashdetect, FETT263_BM_CLASH_DETECT);
#ifdef FETT263_MAX_CLASH
    CONFIG_VARIABLE(maxclash, FETT263_MAX_CLASH);
#else
    CONFIG_VARIABLE(maxclash, 16);
#endif
}
  bool gestureon; // gesture controls on/off for use with "Gesture Sleep"
  bool swingon; // Swing On ignition
  int swingonspeed; // Swing On speed (200 ~ 500 range)
  bool twiston; // Twist On ignition
  bool thruston; // Thrust On ignition
  bool stabon; // Stab On ignition
  bool forcepush; // Force Push
  int forcepushlen; // Force Push Length
  int lockupdelay; // Lockup Delay (for Battle Mode)
  bool twistoff; // Twist Off retraction
  bool powerlock; // diable PWR button for retraction, for use with "Power Lock" mode to prevent button turning saber off
  int clashdetect; // maximum Clash Strength to detect Clash during Battle Mode (0 ~ 10 range)
  int maxclash; // maximum Clash Strength for Clash Sound and Detection works with CLASH_THRESHOLD_G to create range of Clash Strength (8 ~ 16 range)
};

#ifdef FETT263_SAVE_CHOREOGRAPHY
// Rehearsal / Choreography
class SavedRehearsal : public ConfigFile {
public:
  virtual ReadStatus Read(FileReader* f) override {
    int file;
    if (!f || !f->IsOpen()) return ReadStatus::READ_FAIL;
    memset(clash_rec, 0, sizeof(clash_rec));
    STDOUT.println("Reading choreo.ini");
    for (size_t i = 0; i < NELEM(clash_rec); i++) {
      char action[33];
      f->skipwhite();
      f->readVariable(action);
      if (!strcmp(action, "clash")) {
          if (SFX_clsh) {
             file = SFX_clsh.get_min_file();
          } else {
             file = SFX_clash.get_min_file();
          }
        clash_rec[i].stance = STANCE_CLASH;
        f->skipspace();
        clash_rec[i].sound_number1 = (int)(f->readFloatValue() - file);
        f->skipspace();
        continue;
      }
      if (!strcmp(action, "lockup")) {
          if (SFX_lock) {
            file = SFX_lock.get_min_file();
          } else {
            file = SFX_lockup.get_min_file();
          }
        clash_rec[i].stance = STANCE_LOCKUP;
        f->skipspace();
        clash_rec[i].sound_number1 = (int)(f->readFloatValue() - SFX_bgnlock.get_min_file());
        f->skipspace();
        clash_rec[i].sound_number2 = (int)(f->readFloatValue() - file);
        f->skipspace();
        clash_rec[i].sound_number3 = (int)(f->readFloatValue() - SFX_endlock.get_min_file());
        f->skipspace();
        continue;
      }
      clash_rec[i].stance = STANCE_END;
      return ReadStatus:: READ_END;
      break;
    }
  }

  void SetVariable(const char* variable, float v) override {
  }

  enum FormStance {
    STANCE_END = 0,
    STANCE_CLASH,
    STANCE_LOCKUP,
  };

  struct FormEntry {
    FormStance stance;
    int sound_number1;
    int sound_number2;
    int sound_number3;
  };

  FormEntry clash_rec[32];
};
#endif

#ifdef FETT263_EDIT_MODE_MENU
// Edit Length 
int length_edit_length = 0;

// Edit Length Preview for Edit Mode
// LengthEdit uses blade color and creates single white pixel at last pixel, black above last pixel
template<class BASE, class LIGHTUP, class BLACK = BLACK>
class LengthEdit {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    lightup_.run(blade);
    black_.run(blade);
    led_ = length_edit_length;
  }

  OverDriveColor getColor(int led) {
    if (led == led_) return lightup_.getColor(led);
    if (led > led_) return black_.getColor(led);
    return base_.getColor(led);
  }
private:
  int led_;
  BASE base_;
  LIGHTUP lightup_;
  BLACK black_;
};

// Edit Style Settings
static int int_edit = 0;

class IntEdit {
  public:
    void run(BladeBase* blade) {
    }
    int getInteger(int led) { return int_edit; }    
};
#endif
  
// Color List
static constexpr Color16 color_list_[] = {
    { 65535, 0, 0 }, // Red
    { 65535, 3598, 0 }, // OrangeRed
    { 65535, 17476, 0 }, // DarkOrange
    { 65535, 24929, 0 }, // Orange
    { 46260, 33410, 0 }, // Gold
    { 65535, 65535, 0 }, // Yellow
    { 27756, 65535, 1542 }, // GreenYellow
    { 0, 65535, 0 }, // Green
    { 14135, 65535, 43433 }, // Aquamarine
    { 0, 65535, 65535 }, // Cyan
    { 0, 34695, 65535 }, // DeepSkyBlue
    { 514, 18504, 65535 }, // DodgerBlue
    { 0, 0, 65535 }, // Blue
    { 7710, 15420, 51400 }, // IceBlue
    { 11102, 92, 53864 }, // Indigo
    { 24000, 0, 50536 }, // Purple
    { 30324, 0, 49768 }, // DeepPurple
    { 48768, 0, 48768 }, // Magenta
    { 65535, 0, 19275 }, // DeepPink
    { 25700, 25700, 38550 }, // Silver
    { 21845, 21845, 51400 }, // Glacier
    { 46260, 46260, 65535 }, // IceWhite
    { 49087, 65535, 65535 }, // LightCyan
    { 65535, 51142, 30582 }, // Moccasin
    { 65535, 62708, 40349 }, // LemonChiffon
    { 65535, 48059, 27756 }, // NavajoWhite
    { 65535, 65535, 65535 }, // White
    { 0, 0, 65535 }
};

// The Saber class implements the basic states and actions
// for the saber.
class SaberFett263Buttons : public virtual PropBase {
public:
SaberFett263Buttons() : PropBase() {}
  const char* name() override { return "SaberFett263Buttons"; }

  GestureControlFile saved_gesture_control;
#ifdef FETT263_SAVE_CHOREOGRAPHY
  SavedRehearsal saved_choreography;
#endif

  void RestoreGestureState() {
    saved_gesture_control.ReadINIFromDir(NULL, "gesture");
  }

  void WriteGestureState(const char* filename) {
    LOCK_SD(true);
    FileReader out;
    LSFS::Remove(filename);
    out.Create(filename);
    out.write_key_value("installed", install_time);
#ifdef FETT263_SAVE_GESTURE_OFF    
    out.write_key_value("gestureon", saved_gesture_control.gestureon);
#else
    out.write_key_value("gestureon", 1);
#endif       
    out.write_key_value("swingon", saved_gesture_control.swingon);    
    out.write_key_value("swingonspeed", saved_gesture_control.swingonspeed);    
    out.write_key_value("twiston", saved_gesture_control.twiston);   
    out.write_key_value("stabon", saved_gesture_control.stabon);   
    out.write_key_value("thruston", saved_gesture_control.thruston);   
    out.write_key_value("twistoff", saved_gesture_control.twistoff);
    out.write_key_value("powerlock", saved_gesture_control.powerlock);      
    out.write_key_value("forcepush", saved_gesture_control.forcepush);   
    out.write_key_value("forcepushlen", saved_gesture_control.forcepushlen);   
    out.write_key_value("lockupdelay", saved_gesture_control.lockupdelay);   
    out.write_key_value("clashdetect", saved_gesture_control.clashdetect);
    out.write_key_value("maxclash", saved_gesture_control.maxclash);
    out.write_key_value("end", "1");
    out.Close();
    LOCK_SD(false);
  }

  void SaveGestureState() {
    STDOUT.println("Saving Gesture State");
    WriteGestureState("gesture.tmp");
    WriteGestureState("gesture.ini");
  }

#ifdef FETT263_SAVE_CHOREOGRAPHY
  void RestoreChoreo() {
    saved_choreography.ReadInCurrentDir("choreo.ini");
  }

  void WriteChoreo(const char* filename) {
    const char* dir = nullptr;
    int file;
    if (SFX_clsh) {
      dir = SFX_clsh.get_directory();
    } else {
      dir = SFX_clash.get_directory();
    }
    if (dir == nullptr) dir = current_directory;
    PathHelper full_name(dir, filename);
    LOCK_SD(true);
    FileReader out;
    LSFS::Remove(full_name);
    out.Create(full_name);
    for (int i = 0; i < NELEM(saved_choreography.clash_rec); i++) {
      char value[64];
      switch (saved_choreography.clash_rec[i].stance) {
        case SavedRehearsal::STANCE_CLASH:
          if (SFX_clsh) {
             file = SFX_clsh.get_min_file();
          } else {
             file = SFX_clash.get_min_file();
          }
          strcpy(value, "clash ");
          itoa(saved_choreography.clash_rec[i].sound_number1 + file, value + strlen(value), 10);
          strcat(value, "\n");
          out.Write(value);
          break;
        case SavedRehearsal::STANCE_LOCKUP:
          if (SFX_lock) {
            file = SFX_lock.get_min_file();
          } else {
            file = SFX_lockup.get_min_file();
          }
          strcpy(value, "lockup ");
          itoa(saved_choreography.clash_rec[i].sound_number1 + SFX_bgnlock.get_min_file(), value + strlen(value), 10);
          strcat(value, " ");
          itoa(saved_choreography.clash_rec[i].sound_number2 + file, value + strlen(value), 10);
          strcat(value, " ");
          itoa(saved_choreography.clash_rec[i].sound_number3 + SFX_endlock.get_min_file(), value + strlen(value), 10);
          strcat(value, "\n");
          out.Write(value);
          break;
        case SavedRehearsal::STANCE_END:
        default:
          break;
      }
    }
    out.Write("end\n");
    out.Close();
    LOCK_SD(false);
  }

  void SaveChoreo() {
    STDOUT.println("Saving Choreography");
    WriteChoreo("choreo.tmp");
    WriteChoreo("choreo.ini");
  }
#endif

  void GenerateIniFiles() {
    SaveState(current_preset_.preset_num);
    SaveGestureState();
  }

#ifdef FETT263_EDIT_MODE_MENU
  // Color / Style Editing
  HSL hsl_;
  ShowColorSingleBladeTemplate<RotateColorsX<Variation,ShowColorStyle>> show_color_;
  ShowColorSingleBladeTemplate<Mix<Bump<Int<16384>,Int<14000>>,RotateColorsX<Variation,RgbArg<1,Rgb<255,0,0>>>,ShowColorStyle>> bump_color_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<Int<26000>,Int<8000>>,RotateColorsX<Variation,RgbArg<1,Rgb<255,0,0>>>,ShowColorStyle>> tip_color_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<Int<7000>,Int<-6000>>,RotateColorsX<Variation,RgbArg<1,Rgb<255,0,0>>>,ShowColorStyle>> hilt_color_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<Int<7000>,Int<-6000>>,Black,RotateColorsX<Variation,ShowColorStyle>>> pre_color_;
  ShowColorSingleBladeTemplate<LengthEdit<RotateColorsX<Variation,ShowColorStyle>,Pulsing<White,Rgb<100,100,100>,800>>> show_length_;
  ShowColorSingleBladeTemplate<Mix<Bump<IntEdit,Int<10000>>,Black,ShowColorStyle>> show_clash_location_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<IntEdit,Int<6000>>,Black,ShowColorStyle>> show_drag_size_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<IntEdit,Int<-6000>>,Black,ShowColorStyle>> show_emitter_size_;
#if NUM_BLADES > 1
  ShowColorSingleBladeTemplate<Pulsing<RotateColorsX<Variation,ShowColorStyle>,Black,800>> show_preview_;
#endif

  Color16 GetColorArg(int blade, int arg) {
    char argspace[32];
    if (style_parser.GetArgument(current_preset_.GetStyle(blade), arg + 2, argspace)) {
      char* tmp;
      int r = strtol(argspace, &tmp, 0);
      int g = strtol(tmp+1, &tmp, 0);
      int b = strtol(tmp+1, NULL, 0);
      return Color16(r,g,b);
    }
    return Color16();
  }

  // Show Full Blade Preview for Color Editing
  void ShowFull() {
    show_color_.Start(blade_num_);
    ShowColorStyle::SetColor(GetColorArg(blade_num_, effect_num_)); 
    saved_color_ = GetColorArg(blade_num_, effect_num_);
    hsl_ = saved_color_.toHSL();
    hsl_angle_ = fusor.angle2();
  }

  // Show Partial Blade Preview (Clash, Lockup, Drag, Melt, Preon, Emitter) for Color Editing
  void ShowPart() {
    ShowColorStyle::SetColor(GetColorArg(blade_num_, effect_num_)); 
    saved_color_ = GetColorArg(blade_num_, effect_num_);
    hsl_ = saved_color_.toHSL();
    hsl_angle_ = fusor.angle2();
  }

#if NUM_BLADES > 1
  void BladePreview(int blade) {
    if (blade_preview_ > 0) {
      show_preview_.Stop(blade_preview_);
    }
    blade_preview_ = blade;
    show_preview_.Start(blade);
    ShowColorStyle::SetColor(GetColorArg(blade_num_, 1)); 
  }
#endif

  void LengthPreview(int blade) {
    show_length_.Start(blade);
    length_edit_length = blade_length_ - 1;
    ShowColorStyle::SetColor(GetColorArg(blade_num_, 1)); 
  }
 
  // Copy Colors Function
  void SetColorCopy() {
    effect_num_ = 16;
    char set_copy[16];
    while (true) {
      effect_num_ -= 1;
      if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), effect_num_ + 2)) break;
    }
    saved_color_ = GetColorArg(blade_num_, effect_num_);
    hsl_ = saved_color_.toHSL();
    NewColor(blade_num_, effect_num_);
    current_preset_.Save();
    effect_num_ = 0;
  }

  void SetInOut() {
    char style_arg[10];
    itoa(calc_, style_arg, 10);
    current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, style_arg));
    current_preset_.Save();
    switch (menu_type_) {
      case MENU_STYLE_OPTION:
      case MENU_IGNITION_TIME:
      case MENU_IGNITION_OPTION:
      case MENU_PREON_OPTION:
      case MENU_PREON_SIZE:
        UpdateFont(current_preset_.preset_num);
        break;
      case MENU_RETRACTION_TIME:
      case MENU_RETRACTION_OPTION:
        UpdateStyle(current_preset_.preset_num);
        off_event_ = true;
        last_rotate_ = millis();
        break;
      default:
        break;
    }
  }

  void RevertInOut() {
    char style_arg[10];
    itoa(arg_revert_, style_arg, 10);
    current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, style_arg));
    switch (menu_type_) {
      case MENU_RETRACTION_OPTION:
      case MENU_RETRACTION_TIME:
        char ig[10];
        itoa(ignite_time_, ig, 10);
        current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 19, ig));
        break;
      default:
        break;
    }
    current_preset_.Save();
    UpdateFont(current_preset_.preset_num);
  }

  void SaveStyleSetting() {
    char style_arg[10];
    itoa(calc_, style_arg, 10);
    current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, style_arg));
    current_preset_.Save();
    // Handle location/size preview in Style Settings
    switch (set_num_) {
      // Stop Clash Location Preview
      case 21:
        show_clash_location_.Stop(blade_num_);
        break;
      // Stop Drag Size Preview
      case 22:
        show_drag_size_.Stop(blade_num_);
        break;
      // Stop Emitter Size Preview
      case 23:
        show_emitter_size_.Stop(blade_num_);
      default:
        break;
    }
    UpdateStyle(current_preset_.preset_num);
    menu_type_ = MENU_STYLE_SETTING_SUB;
    MenuSave();
  }

  void CancelStyleSetting() {
    switch (set_num_) {
      case 21:
        show_clash_location_.Stop(blade_num_);
        break;
      case 22:
        show_drag_size_.Stop(blade_num_);
        break;
      case 23:
        show_emitter_size_.Stop(blade_num_);
        break;
      default:
        break;
    }
    UpdateStyle(current_preset_.preset_num);
    menu_type_ = MENU_STYLE_SETTING_SUB;
    MenuCancel();
  }
#endif

  // Preview Color Editing on Blade
  void NewColor(int blade, int effect) {
    char new_color[32];
    Color16 color_source;
     switch (color_mode_) {
      case COLOR_LIST:
      case CC_COLOR_LIST:
        color_source = Color16(color_list_[dial_]);
        break;
      default:
        color_source = Color16(hsl_);
        break;
    }
    itoa(Color16(color_source).r, new_color, 10);
    strcat(new_color, ",");
    itoa(Color16(color_source).g, new_color + strlen(new_color), 10);
    strcat(new_color, ",");
    itoa(Color16(color_source).b, new_color + strlen(new_color), 10);
    current_preset_.SetStyle(blade,style_parser.SetArgument(current_preset_.GetStyle(blade), effect + 2, new_color));
    if (color_mode_ != CC_COLOR_LIST) color_mode_ = NONE;
  }

  void Setup() override {
    RestoreGestureState();
  }

#ifdef FETT263_SAVE_CHOREOGRAPHY
  bool chdir(const char* dir) override {
    bool ret = PropBase::chdir(dir);
    RestoreChoreo();
    clash_count_ = 0;
    return ret;
  }
#endif

  SoundQueue<16> sound_queue_;

  enum SayType {
    SAY_BATTERY,
    SAY_DECIMAL,
    SAY_WHOLE,
  };

  void TensValue(int number) {
    if (number <= 20) {
      sound_queue_.Play(SoundToPlay(&SFX_mnum, number - 1));
    } else {
      int tens = ((int)floorf(number / 10)) % 10;
      number -= (tens * 10);
      switch (tens) {
        default:
        case 2:
          sound_queue_.Play(SoundToPlay(&SFX_mnum, (tens * 10) - 1));
          break;
        case 3:
          sound_queue_.Play(SoundToPlay("thirty.wav"));
          break;
        case 4:
          sound_queue_.Play(SoundToPlay("forty.wav"));
          break;
        case 5:
          sound_queue_.Play(SoundToPlay("fifty.wav"));
          break;              
        case 6:
          sound_queue_.Play(SoundToPlay("sixty.wav"));
          break;  
        case 7:
          sound_queue_.Play(SoundToPlay("seventy.wav"));
          break;
        case 8:
          sound_queue_.Play(SoundToPlay("eighty.wav"));
          break;  
        case 9:
          sound_queue_.Play(SoundToPlay("ninety.wav"));
          break;  
      }
      if (number != 0) sound_queue_.Play(SoundToPlay(&SFX_mnum, number - 1));
    }
  }

  void SayNumber (float number, SayType say_type) {
    int thousand = ((int)floorf(number / 1000)) % 100;
    int hundred = ((int)floorf(number / 100)) % 10;
    int ones = ((int)floorf(number)) % 100;
    int tenths = ((int)floorf(number * 10)) % 10;
    int hundredths = ((int)floorf(number * 100)) % 10;
    switch (say_type) {
      case SAY_BATTERY:
      case SAY_DECIMAL:
        // Battery Level prompt
        if (say_type == SAY_BATTERY) sound_queue_.Play(SoundToPlay("mbatt.wav"));
        // Tens & Ones
        if (number == 0) {
          sound_queue_.Play(SoundToPlay("mzero.wav"));
        } else {
          TensValue(ones);
        }
        // Decimal / Point
        sound_queue_.Play(SoundToPlay("mpoint.wav"));
        // Tenths
        if (tenths == 0) {
          sound_queue_.Play(SoundToPlay("mzero.wav"));
        } else {
          sound_queue_.Play(SoundToPlay(&SFX_mnum, tenths - 1));
        }
        // Hundredths
        if (hundredths != 0) sound_queue_.Play(SoundToPlay(&SFX_mnum, hundredths - 1));
        // Volts prompt
        if (say_type == SAY_BATTERY) sound_queue_.Play(SoundToPlay("mvolts.wav"));
        break;
      case SAY_WHOLE:
        // Thousands
        if (thousand > 0) {
          TensValue(thousand);
          sound_queue_.Play(SoundToPlay("thousand.wav"));
        }
        // Hundred
        if (hundred > 0) {
          sound_queue_.Play(SoundToPlay(&SFX_mnum, hundred - 1));
          sound_queue_.Play(SoundToPlay("hundred.wav"));          
        }
        // Tens & Ones
        if (ones == 0) {
          if ((thousand + hundred) == 0) sound_queue_.Play(SoundToPlay("mzero.wav"));
        } else {
          TensValue(ones);
        }
        break;
      default:
        break;
    }
  }
   
  enum ClashType {
    CLASH_NONE,
    CLASH_CHECK,
    CLASH_BATTLE_MODE,
    CLASH_LOCKUP,
    CLASH_LOCKUP_END,
#ifdef FETT263_CLASH_STRENGTH_SOUND    
    CLASH_NORMAL,
    CLASH_STAB,
#endif
  };

#ifdef FETT263_CLASH_STRENGTH_SOUND
  void RealClashSound() {
    if (clash_type_ == CLASH_BATTLE_MODE) {
      if (SaberBase::GetClashStrength() < saved_gesture_control.clashdetect) {
        clash_type_ = CLASH_NORMAL;
      } else {
        clash_type_ = CLASH_LOCKUP;
        auto_lockup_on_ = true;
      }
    }
    int files;
    int file_number;
    if (saved_gesture_control.maxclash < 8) saved_gesture_control.maxclash = 8;
    float clash_value = (SaberBase::GetClashStrength() - GetCurrentClashThreshold()) / saved_gesture_control.maxclash;
    switch (clash_type_) {
      default:
        break;
      case CLASH_NORMAL:
        if (SFX_clash) {
          files = SFX_clash.files_found() - 1;
          file_number = clamp(files * clash_value, 0, files);
          SFX_clash.Select(file_number);
        } else {
          files = SFX_clsh.files_found() - 1;
          file_number = clamp(files * clash_value, 0, files);
          SFX_clsh.Select(file_number);
        }
        SaberBase::DoClash();
        break;
      case CLASH_STAB:
        if (SFX_stab) {
          files = SFX_stab.files_found() - 1;
        } else {
          if (SFX_clash) {
            files = SFX_clash.files_found() - 1;
            file_number = clamp(files * clash_value, 0, files);
            SFX_clash.Select(file_number);
          } else {
            files = SFX_clsh.files_found() - 1;
            file_number = clamp(files * clash_value, 0, files);
            SFX_clsh.Select(file_number);
          }          
        }
        SaberBase::DoStab();
        break;
      case CLASH_LOCKUP:
        if (battle_mode_) clash_value = (SaberBase::GetClashStrength() - saved_gesture_control.clashdetect) / saved_gesture_control.maxclash;
        if (SFX_bgnlock) {
          files = SFX_bgnlock.files_found() - 1;
          file_number = clamp(files * clash_value, 0, files);
        }
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        SaberBase::DoBeginLockup();
        break;
      case CLASH_LOCKUP_END:
        if (SFX_endlock) {
          files = SFX_endlock.files_found() - 1;
          file_number = clamp(files * (fusor.swing_speed() / 600), 0, files);
        }
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        break;
    }
    clash_type_ = CLASH_NONE;
  }
  #endif

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    Vec3 mss = fusor.mss();
    if (clash_type_ != CLASH_NONE && millis() - clash_impact_millis_ > 1) {
      // CHECK PUSH
      if (clash_type_ == CLASH_CHECK) {
        Event(BUTTON_NONE, EVENT_PUSH);
        clash_type_ = CLASH_NONE;
      }
      if (clash_type_ != CLASH_LOCKUP_END) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
        RealClashSound();
#else
      if (SaberBase::GetClashStrength() < saved_gesture_control.clashdetect) {
        SaberBase::DoClash();
        clash_type_ = CLASH_NONE;
      } else {
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        SaberBase::DoBeginLockup();
        auto_lockup_on_ = true;
        clash_type_ = CLASH_NONE;
      }
#endif
      }
    }
    sound_queue_.Poll(wav_player);
#ifdef FETT263_EDIT_MODE_MENU      
    if (next_event_ && !wav_player->isPlaying()) {
      if (rehearse_) {
        if (SaberBase::IsOn()) {
          rehearse_ = false;
          Off();
#ifdef FETT263_SAVE_CHOREOGRAPHY
          SaveChoreo();
#endif
        } else {
          PlayMenuSound("rehrsbgn.wav");
          FastOn();
        }
        next_event_ = false;
      } else {
        switch (menu_type_) {
          case MENU_IGNITION_TIME:
          case MENU_RETRACTION_TIME:
            next_event_ = false;
            SetInOut();
            break;
          default:
            next_event_ = false;
            break;
        }
      }
    }
    if (off_event_ && millis() - last_rotate_ > 200) {
      Off();
      off_event_ = false;
      restart_ = true;
      last_rotate_ = millis();
    }
    if (restart_ && millis() - last_rotate_ > calc_ + 1000) {
      restart_ = false;
      FastOn();
    }
#endif
#ifdef FETT263_EDIT_MODE_MENU
      if (edit_color_ && color_mode_ != COLOR_LIST) {
#define H_CHANGE (M_PI / 98304)
#define L_ANGLE (M_PI / 30)
#define H_ANGLE (M_PI / 16384)
#define EDIT_MODE_ZOOM (M_PI * 2 / 2000)
      float a = fusor.angle2() - hsl_angle_;
      if (a > M_PI) a-=M_PI*2;      
      if (a < -M_PI) a+=M_PI*2;
      float angle = 100;
      switch (color_mode_) {
        case EDIT_COLOR: angle = H_ANGLE; break;
        case ZOOM_COLOR: angle = EDIT_MODE_ZOOM; break;
        case EDIT_WHITE:
        case EDIT_BLACK: angle = L_ANGLE; break;
        default: break;
      }
      int steps = 0;
      if (a > angle * 2/3) {
        hsl_angle_ += angle;
        if (hsl_angle_ > M_PI) hsl_angle_ -= M_PI*2;
        steps++;
      }
      if (a < -angle * 2/3) {
        hsl_angle_ -= angle;
        if (hsl_angle_ < M_PI) hsl_angle_ += M_PI*2;
        steps--;
      }
      switch (color_mode_) {
        default: break;
        case EDIT_COLOR:
        case ZOOM_COLOR:
          hsl_.H = fract(hsl_.H - H_CHANGE * steps);
          break;
        case EDIT_WHITE:
          if (steps > 0) {
            if (hsl_.L < 1.0) {
              hsl_.L = clamp(hsl_.L + 0.01, 0.5, 1.0);
              if (hsl_.L == 1.0) {
                PlayMenuSound("mmax.wav");
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          if (steps < 0) {
            if (hsl_.L > 0.5) {
              hsl_.L = clamp(hsl_.L - 0.01, 0.5, 1.0);
              if (hsl_.L == 0.5) {
                PlayMenuSound("mmin.wav");
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          break;
        case EDIT_BLACK:
          if (steps > 0) {
            if (hsl_.L < 0.5) {
              hsl_.L = clamp(hsl_.L + 0.01, 0.01, 0.5);
              if (hsl_.L == 0.5) {
                PlayMenuSound("mmax.wav");
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          if (steps < 0) {
            if (hsl_.L > 0.01) {
              hsl_.L = clamp(hsl_.L - 0.01, 0.01, 0.5);
              if (hsl_.L == 0.01) {
                PlayMenuSound("mmin.wav");
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          break;         
        }
        ShowColorStyle::SetColor(Color16(hsl_));
      }
#endif
    if (SaberBase::IsOn()) {
      DetectSwing();
#ifdef FETT263_SAVE_CHOREOGRAPHY
      if (choreo_ && saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_END) {
        choreo_ = false;
        PlayMenuSound("chreoend.wav");
      }
#endif
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > saved_gesture_control.lockupdelay &&
          SaberBase::Lockup()) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
        clash_type_ = CLASH_LOCKUP_END;
        RealClashSound();
#else
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
#endif
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > saved_gesture_control.lockupdelay &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }
      // EVENT_PUSH
      if (fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 100 &&
          fusor.swing_speed() < 20 &&
          fabs(fusor.gyro().x) < 5) {
        if (millis() - push_begin_millis_ > saved_gesture_control.forcepushlen) {
          // Checking for Clash at end of movement
          clash_type_ = CLASH_CHECK;
          push_begin_millis_ = millis();
          clash_impact_millis_ = millis();
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
        if (!swinging_ && fusor.swing_speed() > saved_gesture_control.swingonspeed) {
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
    if (menu_ || color_mode_ == CC_COLOR_LIST) {
      DetectMenuTurn();
    }
    if (track_mode_ != PLAYBACK_OFF) {
      TrackPlayer();
    }
  }

  // Fett263 Track Player
  enum TrackMode {
    PLAYBACK_OFF,
    PLAYBACK_LOOP,
    PLAYBACK_ROTATE,
    PLAYBACK_RANDOM,
  };

  void TrackPlayer() {
    if (!track_player_) {
#ifdef ENABLE_AUDIO
      if (track_num_ == -1 && track_mode_ == PLAYBACK_LOOP) {
        StartOrStopTrack();
      } else {
        if (track_num_ > num_tracks_) track_num_ = 0;
        switch (track_mode_) {
          case PLAYBACK_ROTATE:
            track_num_ += 1;
            break;
          case PLAYBACK_RANDOM:
            track_num_ = rand() % num_tracks_;
            if (track_num_ < 0) track_num_ = num_tracks_ - 1;
            break;
          default:
            break;
        }
        char playtrack[128];
        RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, playtrack, sizeof(playtrack));
        MountSDCard();
        EnableAmplifier();
        track_player_ = GetFreeWavPlayer();
        if (track_player_) {
          track_player_->Play(playtrack);
        } else {
          STDOUT.println("No available WAV players.");
        }
      }
#else
      STDOUT.println("Audio disabled.");
#endif
    }
  }   
   
  void DetectMenuTurn() {
     if (millis() - last_rotate_ > 1000) {
      float a = fusor.angle2() - current_menu_angle_;
      if (a > M_PI) a-=M_PI*2;
      if (a < -M_PI) a+=M_PI*2;
      change_ = a;
      if (a > twist_menu_ * 2/3) {
        current_menu_angle_ += twist_menu_;
        if (current_menu_angle_ > M_PI) current_menu_angle_ -= M_PI * 2;
        Event(BUTTON_NONE, EVENT_TWIST_RIGHT);
        current_menu_angle_ = fusor.angle2();
        last_rotate_ = millis();
      }
      if (a < -twist_menu_ * 2/3) {
        current_menu_angle_ -= twist_menu_;
        if (current_menu_angle_ < M_PI) current_menu_angle_ += M_PI * 2;
        Event(BUTTON_NONE, EVENT_TWIST_LEFT);
        current_menu_angle_ = fusor.angle2();
        last_rotate_ = millis();
      }
    }
  }

#ifdef FETT263_EDIT_MODE_MENU
#define SUBMENUS 9
#define COLOR_SUB 15
#define COLOR_OPTIONS 3
#define GESTURE_OPTIONS 12
#define SET_SUBMENUS 6
#define STYLE_SETTINGS 10
#endif

  enum MenuType {
    MENU_TOP,
    MENU_PRESET,
    MENU_VOLUME,
    TRACK_PLAYER,    
    MENU_REHEARSE,
#ifdef FETT263_EDIT_MODE_MENU
    MENU_STYLE,
    MENU_COLOR,
    MENU_COLOR_CHANGE,
    MENU_FONT,
    MENU_TRACK,
    MENU_LENGTH,
    MENU_COPY,
    MENU_DELETE,
    MENU_RESET,
    MENU_EXIT,
    MENU_COLOR_SUB,
    MENU_COLOR_MODE,
    MENU_EFFECT,
    MENU_RESET_COLOR,
#if NUM_BLADES > 1
    MENU_COPY_COLOR,
    MENU_BLADE_STYLE,
    MENU_BLADE_COLOR,
    MENU_BLADE_COPY,
    MENU_BLADE_SETTING,
    MENU_BLADE_LENGTH,
#endif
    MENU_GESTURE_SUB,
    MENU_SWINGON,
    MENU_TWISTON,
    MENU_THRUSTON,
    MENU_STABON,
    MENU_FORCEPUSH,
    MENU_TWISTOFF,
    MENU_POWERLOCK,
    MENU_SETTING_SUB,
    MENU_STYLE_SETTING_SUB,
    MENU_STYLE_OPTION,
    MENU_IGNITION_OPTION,
    MENU_IGNITION_TIME,
    MENU_RETRACTION_OPTION,
    MENU_RETRACTION_TIME,
    MENU_CLASH_LOCATION,
    MENU_DRAG_SIZE,
    MENU_EMITTER_SIZE,
    MENU_PREON_OPTION,
    MENU_PREON_SIZE,
    MENU_DIM_BLADE,
    MENU_CLASH_THRESHOLD,
    MENU_SWINGON_SPEED,
    MENU_FORCEPUSH_LENGTH,
    MENU_LOCKUP_DELAY,
    MENU_CLASH_DETECT,
    MENU_MAX_CLASH,
    COLOR_BASE,
    COLOR_ALT,
    COLOR_BLAST,
    COLOR_CLASH,
    COLOR_LOCKUP,
    COLOR_DRAG,
    COLOR_LB,
    COLOR_STAB,
    COLOR_PREON,
    COLOR_IGNITE,
    COLOR_RETRACT,
    COLOR_PSTOFF,
    COLOR_SWING,
    COLOR_EMITTER,
    COLOR_OFF,
#endif  
  };

// Edit Mode Menu Select (PWR Button)
  void MenuChoice() {
    const char* tmp; 
    switch (menu_type_) {
      case MENU_PRESET:
        PlayMenuSound("mselect.wav");
        menu_type_ = MENU_TOP;
        menu_ = false;
        break;
      case MENU_REHEARSE:
        PlayMenuSound("rehrsbgn.wav");
        rehearse_ = true;
        MenuExit();
        FastOn();
      case MENU_VOLUME:
        if (SaberBase::IsOn()) {
          menu_type_ = MENU_SETTING_SUB;
          MenuSave();
        } else {
          MenuExit();
        }
        break;  
#ifdef FETT263_EDIT_MODE_MENU        
      case MENU_TOP:
        switch (menu_top_pos_) {
          case 1:   
  #if NUM_BLADES == 1
            menu_type_ = MENU_STYLE;
            num_presets_ = current_config->num_presets;
            blade_num_ = 1;
            tmp = current_preset_.GetStyle(blade_num_);
            style_num_ = FirstWord(tmp, "builtin") ? atoi(SkipWord(tmp)) : 0; 
            style_revert_ = style_num_;               
            PlayMenuSound("mselect.wav");
            break;
  #else
            menu_type_ = MENU_BLADE_STYLE;
            blade_num_ = 0;
            PlayMenuSound("mblade.wav");
            break;
  #endif
          case 2:
            if (style_parser.MaxUsedArgument(current_preset_.GetStyle(1)) == 0) {
              menu_type_ = MENU_COLOR;
              PlayMenuSound("mselect.wav");
              color_revert_ = SaberBase::GetCurrentVariation();
              ToggleColorChangeMode();
              break;
            } else {
  #if NUM_BLADES == 1
            blade_num_ = 1;
            menu_type_ = MENU_COLOR_SUB;
            menu_sub_pos_ = 0;
            PlayMenuSound("mcolorsb.wav");
            effect_num_ = 0;
            break;
  #else
            menu_type_ = MENU_BLADE_COLOR;
            blade_num_ = 0;
            PlayMenuSound("mblade.wav");
            break;                
  #endif
            }
            break; 
          case 3:
            menu_type_ = MENU_FONT;
            font_num_ = 0;                                               
            num_fonts_ = RunCommandAndGetSingleLine("list_fonts", nullptr, 0, 0, 0);
            if (!restore_point.get()) restore_point = std::move(current_preset_.font);
            PlayMenuSound("mselect.wav");
            break;
          case 4:
            num_tracks_ = RunCommandAndGetSingleLine("list_current_tracks", nullptr, 0, 0, 0);
            StartOrStopTrack();
            menu_type_ = MENU_TRACK;
            track_num_ = 0;
            if (!restore_point.get()) restore_point = std::move(current_preset_.track);
            PlayMenuSound("mselect.wav");
            break;
          case 5:
            menu_type_ = MENU_SETTING_SUB;
            PlayMenuSound("moption.wav");
            menu_sub_pos_ = 0;
            break;
          case 6:
            menu_type_ = MENU_COPY;
            PlayMenuSound("mconfirm.wav");
            break;
          case 7:
            menu_type_ = MENU_DELETE;
            PlayMenuSound("mconfirm.wav");
            break;
          case 8:
            PlayMenuSound("mselect.wav");
            MenuExit();
            break;
        }
        break;
  #if NUM_BLADES > 1          
      case MENU_BLADE_STYLE:
        if (blade_num_ == 0) {
          PlayMenuSound("mblade.wav");
          break;
        }
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        menu_type_ = MENU_STYLE;
        num_presets_ = current_config->num_presets;
        tmp = current_preset_.GetStyle(blade_num_);
        style_num_ = FirstWord(tmp, "builtin") ? atoi(SkipWord(tmp)) : 0; 
        style_revert_ = style_num_;               
        MenuSelect();
        break;
      case MENU_BLADE_COLOR:
  #if NUM_BLADES > 2
        menu_type_ = MENU_COLOR_SUB;
        menu_sub_pos_ = 0;
        copy_blade_ = blade_num_;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        PlayMenuSound("mcolorsb.wav");
        break;
  #else
        menu_type_ = MENU_COLOR_SUB;
        if (blade_num_ == 1) {
          copy_blade_ = 2;
        } else {
          copy_blade_ = 1;
        }
        menu_sub_pos_ = 0;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        PlayMenuSound("moption.wav");
        break;
  #endif
      case MENU_BLADE_COPY:
        menu_type_ = MENU_COPY_COLOR;
        SetColorCopy();
        MenuConfirm();
        break;
      case MENU_BLADE_SETTING:
        if (blade_num_ == 0) {
          PlayMenuSound("mblade.wav");
          break;
        }
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        menu_type_ = MENU_STYLE_SETTING_SUB;
        PlayMenuSound("moption.wav");
        break;
      case MENU_BLADE_LENGTH:
        PlayMenuSound("mselect.wav");
        show_preview_.Stop(blade_preview_);
        SaveState(current_preset_.preset_num);
        blade_preview_ = 0;
        max_length_ = GetMaxBladeLength(blade_num_);
        blade_length_ = GetBladeLength(blade_num_);
        if (blade_length_ < 1) blade_length_ = max_length_;
        length_revert_ = blade_length_;
        LengthPreview(blade_num_);
        menu_type_ = MENU_LENGTH;
        break;
      case MENU_COPY_COLOR:
        if (choice_) {
          current_preset_.SetStyle(copy_blade_, style_parser.CopyArguments(current_preset_.GetStyle(blade_num_), current_preset_.GetStyle(copy_blade_)));
          current_preset_.Save();
          show_preview_.Stop(blade_preview_);
          UpdateStyle(current_preset_.preset_num);
          menu_type_ = MENU_COLOR_SUB;
          MenuSelect();
          choice_ = false;
          blade_preview_ = 0;
          break;
        }          
        PlayMenuSound("mconfirm.wav");
        break;
  #endif           
      case MENU_STYLE:
        restore_point = nullptr; 
        current_preset_.Save();
  #if NUM_BLADES == 1
        menu_type_ = MENU_TOP;
        MenuSave();
  #else           
        menu_type_ = MENU_BLADE_STYLE;
        MenuSave();
  #endif
        break;
      case MENU_COLOR:
        break;
      case MENU_COLOR_SUB:
  #if NUM_BLADES == 1
        switch (menu_sub_pos_) {
          case 1:
            menu_type_ = MENU_EFFECT;
            PlayMenuSound("meffect.wav");
            break;
          case 2:
            menu_type_ = MENU_RESET_COLOR;
            PlayMenuSound("mconfirm.wav");
            break;
          default:
            PlayMenuSound("moption.wav");
        }
  #elif NUM_BLADES == 2
        switch (menu_sub_pos_) {
          case 1:
            menu_type_ = MENU_EFFECT;
            PlayMenuSound("meffect.wav");
            break;
          case 2:
            menu_type_ = MENU_COPY_COLOR;
            SetColorCopy();
            PlayMenuSound("mconfirm.wav");
            break;
          case 3:
            menu_type_ = MENU_RESET_COLOR;
            PlayMenuSound("mconfirm.wav");
            break;
          default:
            PlayMenuSound("moption.wav");
            break;
        }
  #else 
        switch (menu_sub_pos_) {
          case 1:
            menu_type_ = MENU_EFFECT;
            PlayMenuSound("meffect.wav");
            break;
          case 2:
            menu_type_ = MENU_BLADE_COPY;
            SetColorCopy();
            PlayMenuSound("mblade.wav");
            break;
          case 3:
            menu_type_ = MENU_RESET_COLOR;
            PlayMenuSound("mconfirm.wav");
            break;
          default:
            PlayMenuSound("moption.wav");
        }
  #endif
        break;
      case MENU_COLOR_MODE:
        edit_color_ = true;
        switch (sub_dial_) {
          case 0:
            twist_menu_ = M_PI / 6;
            dial_ = -1;
            color_mode_ = COLOR_LIST;
            break;
          case 1:
            color_mode_ = EDIT_COLOR;
            hsl_.S = 1.0;
            break;
          case 2: 
            color_mode_ = EDIT_WHITE;
            twist_menu_ = M_PI / 18;
            hsl_.S = 1.0;
            break;
          case 3: 
            color_mode_ = EDIT_BLACK;
            twist_menu_ = M_PI / 36;
            hsl_.S = 1.0;
            break;
          default:
            break;
        }
        switch (effect_num_) {
          case 1:
            // Base Color Edit
            menu_type_ = COLOR_BASE;
            ShowFull();
            break;
          case 2:
            // Alt Color Edit
            menu_type_ = COLOR_ALT;
            ShowFull();
            break;
          case 3:
            // Blast Color Edit
            menu_type_ = COLOR_BLAST;
            bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));                 
            ShowPart();
            break;
          case 4:
            // Clash Color Edit
            menu_type_ = COLOR_CLASH;
            bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));                 
            ShowPart();
            break;
          case 5:
            // Lockup Color Edit
            menu_type_ = COLOR_LOCKUP;
            bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));                 
            ShowPart();
            break;
          case 6:
            // Drag Color Edit
            menu_type_ = COLOR_DRAG;
            tip_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
            ShowPart();
            break;
          case 7:
            // Lightning Block Color Edit
            menu_type_ = COLOR_LB;
            bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));                 
            ShowPart();
            break;
          case 8:
            // Stab Color Edit
            menu_type_ = COLOR_STAB;
            tip_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
            ShowPart();
            break;
          case 9:
            // PreOn Color Edit
            menu_type_ = COLOR_PREON;
            pre_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
            ShowPart();
            break;
          case 10:
            // Ignition Color Edit
            menu_type_ = COLOR_IGNITE;
            ShowFull();
            break;
          case 11:
            // Retraction Color Edit
            menu_type_ = COLOR_RETRACT;
            ShowFull();
            break;
          case 12:
            // PstOff Color Edit
            menu_type_ = COLOR_PSTOFF;
            pre_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
            ShowPart();
            break;
          case 13:
            // Swing Color Edit
            menu_type_ = COLOR_SWING;
            ShowFull();
            break;
          case 14:
            // Emitter Color Edit
            menu_type_ = COLOR_EMITTER;
            hilt_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
            ShowPart();
            break;
          case 15:
            // Off Color Edit
            menu_type_ = COLOR_OFF;
            ShowFull();
            break;
          default:
            break;
        }
        PlayMenuSound("mselect.wav");
        break;
      case MENU_EFFECT:
        menu_type_ = MENU_COLOR_MODE;
        sub_dial_ = -1;
        PlayMenuSound("moption.wav");
        break;
      case COLOR_BASE:
      case COLOR_ALT:
      case COLOR_IGNITE:
      case COLOR_RETRACT:
      case COLOR_SWING:
      case COLOR_OFF:
        menu_type_ = MENU_COLOR_MODE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        NewColor(blade_num_, effect_num_); 
        current_preset_.Save();
        show_color_.Stop(blade_num_);
        UpdateStyle(current_preset_.preset_num);
        MenuSave();
        color_mode_ = NONE;
        break;
      case COLOR_BLAST:
      case COLOR_CLASH:
      case COLOR_LOCKUP:
      case COLOR_LB:
        menu_type_ = MENU_COLOR_MODE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        NewColor(blade_num_, effect_num_); 
        current_preset_.Save();
        bump_color_.Stop(blade_num_);
        UpdateStyle(current_preset_.preset_num);
        MenuSave();
        color_mode_ = NONE;
        break;
      case COLOR_DRAG:
      case COLOR_STAB:
        menu_type_ = MENU_COLOR_MODE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        NewColor(blade_num_, effect_num_); 
        current_preset_.Save();
        tip_color_.Stop(blade_num_);
        UpdateStyle(current_preset_.preset_num);
        MenuSave();
        color_mode_ = NONE;
        break;
      case COLOR_PREON:
      case COLOR_PSTOFF:
        menu_type_ = MENU_COLOR_MODE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        NewColor(blade_num_, effect_num_); 
        current_preset_.Save();
        pre_color_.Stop(blade_num_);
        UpdateStyle(current_preset_.preset_num);
        MenuSave();
        color_mode_ = NONE;
        break;
      case COLOR_EMITTER:
        menu_type_ = MENU_COLOR_MODE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        NewColor(blade_num_, effect_num_); 
        current_preset_.Save();
        hilt_color_.Stop(blade_num_);
        UpdateStyle(current_preset_.preset_num);
        MenuSave();
        color_mode_ = NONE;
        break;
      case MENU_FONT:
        restore_point = nullptr;
        menu_type_ = MENU_TOP;
        MenuSave();
        break;
      case MENU_TRACK:
        restore_point = nullptr;
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        menu_type_ = MENU_TOP;
        MenuSave();
        break;
      case MENU_LENGTH:
        menu_type_ = MENU_SETTING_SUB;
        current_preset_.Save();
        show_length_.Stop(blade_num_);
        UpdateStyle(current_preset_.preset_num);
        MenuSave();
        break;
      case MENU_COPY:          
        if (choice_) {
          int32_t pos = current_preset_.preset_num;
          current_preset_.preset_num = -1;
          current_preset_.SaveAt(pos);
          menu_type_ = MENU_TOP;
          MenuSelect();
          choice_ = false;
          break;
        }
        PlayMenuSound("mconfirm.wav");
        break;
      case MENU_RESET_COLOR:
        if (choice_) {
          SaberBase::SetVariation(0);
          current_preset_.SetStyle(blade_num_,  style_parser.ResetArguments(current_preset_.GetStyle(blade_num_)));
          current_preset_.Save();
          UpdateStyle(current_preset_.preset_num);
          menu_type_ = MENU_COLOR_SUB;
          MenuSelect();
          choice_ = false;
          break;
        }
        PlayMenuSound("mconfirm.wav");
        break;
      case MENU_GESTURE_SUB:
        switch (effect_num_) {
          case 1:
            menu_type_ = MENU_SWINGON;
            if (saved_gesture_control.swingon) {
              PlayMenuSound("mtrue.wav");
              choice_ = true;
            } else {
              PlayMenuSound("mfalse.wav");
              choice_ = false;
            }
            break;
          case 2:
            menu_type_ = MENU_SWINGON_SPEED;
            PlayMenuSound("mselect.wav");
            calc_ = saved_gesture_control.swingonspeed;
            SayNumber(calc_, SAY_WHOLE);
            break;
          case 3:
            menu_type_ = MENU_TWISTON;
            if (saved_gesture_control.twiston) {
              PlayMenuSound("mtrue.wav");
              choice_ = true;
            } else {
              PlayMenuSound("mfalse.wav");
              choice_ = false;
            }
            break;
          case 4:
            menu_type_ = MENU_THRUSTON;
            if (saved_gesture_control.thruston) {
              PlayMenuSound("mtrue.wav");
              choice_ = true;
            } else {
              PlayMenuSound("mfalse.wav");
              choice_ = false;
            }
            break;
          case 5:
            menu_type_ = MENU_STABON;
            if (saved_gesture_control.stabon) {
              PlayMenuSound("mtrue.wav");
              choice_ = true;
            } else {
              PlayMenuSound("mfalse.wav");
              choice_ = false;
            }
            break;
          case 6:
            menu_type_ = MENU_FORCEPUSH;               
            if (saved_gesture_control.forcepush) {
              PlayMenuSound("mtrue.wav");
              choice_ = true;
            } else {
              PlayMenuSound("mfalse.wav");
              choice_ = false;
            }
            break;
          case 7:
            menu_type_ = MENU_FORCEPUSH_LENGTH;
            PlayMenuSound("mselect.wav");
            calc_ = saved_gesture_control.forcepushlen;
            SayNumber(calc_, SAY_WHOLE);
            break;
          case 8:
            menu_type_ = MENU_TWISTOFF;
            if (saved_gesture_control.twistoff) {
              PlayMenuSound("mtrue.wav");
              choice_ = true;
            } else {
              PlayMenuSound("mfalse.wav");
              choice_ = false;
            }
            break;
          case 9:
            menu_type_ = MENU_LOCKUP_DELAY;
            PlayMenuSound("mselect.wav");
            calc_ = saved_gesture_control.lockupdelay;
            SayNumber(calc_, SAY_WHOLE);             
            break;
          case 10:
            menu_type_ = MENU_CLASH_DETECT;
            PlayMenuSound("mselect.wav");
            calc_ = saved_gesture_control.clashdetect;
            SayNumber(calc_, SAY_WHOLE);
            break;
          case 11:
            menu_type_ = MENU_POWERLOCK;
            if (saved_gesture_control.powerlock) {
              PlayMenuSound("mtrue.wav");
              choice_ = true;
            } else {
              PlayMenuSound("mfalse.wav");
              choice_ = false;
            }
            break;
          case 12:
            menu_type_ = MENU_MAX_CLASH;
            PlayMenuSound("mselect.wav");
            calc_ = saved_gesture_control.maxclash;
            SayNumber(calc_, SAY_WHOLE);
            break;
          default:
            break;
         }
         break;
      case MENU_SWINGON:
        if (choice_) {
          saved_gesture_control.swingon = true;
          SaveGestureState();
        } else {
          saved_gesture_control.swingon = false;
          SaveGestureState();
        }
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_SWINGON_SPEED:
        saved_gesture_control.swingonspeed = calc_;
        SaveGestureState();
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_TWISTON:
        if (choice_) {
          saved_gesture_control.twiston = true;
          SaveGestureState();
        } else {
          saved_gesture_control.twiston = false;
          SaveGestureState();
        }
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_THRUSTON:
        if (choice_) {
          saved_gesture_control.thruston = true;
          SaveGestureState();
        } else {
          saved_gesture_control.thruston = false;
          SaveGestureState();
        }
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_STABON:
        if (choice_) {
          saved_gesture_control.stabon = true;
          SaveGestureState();
        } else {
          saved_gesture_control.stabon = false;
          SaveGestureState();
        }
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_FORCEPUSH:
        if (choice_) {
          saved_gesture_control.forcepush = true;
          SaveGestureState();
        } else {
          saved_gesture_control.forcepush = false;
          SaveGestureState();
        }
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_FORCEPUSH_LENGTH:
        saved_gesture_control.forcepushlen = calc_;
        SaveGestureState();
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_TWISTOFF:
        if (choice_) {
          saved_gesture_control.twistoff = true;
          SaveGestureState();
        } else {
          saved_gesture_control.twistoff = false;
          SaveGestureState();
        }
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_LOCKUP_DELAY:
        saved_gesture_control.lockupdelay = calc_;
        SaveGestureState();
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_CLASH_DETECT:
        saved_gesture_control.clashdetect = calc_;
        SaveGestureState();
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_POWERLOCK:
        if (choice_) {
          saved_gesture_control.powerlock = true;
          SaveGestureState();
        } else {
          saved_gesture_control.powerlock = false;
          SaveGestureState();
        }
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_MAX_CLASH:
        saved_gesture_control.maxclash = calc_;
        SaveGestureState();
        menu_type_ = MENU_GESTURE_SUB;
        MenuSave();
        break;
      case MENU_SETTING_SUB:
        switch (menu_sub_pos_) {
          case 1:
            menu_type_ = MENU_VOLUME;
            PlayMenuSound("mselect.wav");
            break;
          case 2:
            effect_num_ = 0;
  #if NUM_BLADES == 1
            menu_type_ = MENU_STYLE_SETTING_SUB;
            PlayMenuSound("moption.wav");
            blade_num_ = 1;
  #else
            menu_type_ = MENU_BLADE_SETTING;
            PlayMenuSound("mblade.wav");
  #endif
            break;
          case 3:
            menu_type_ = MENU_GESTURE_SUB;
            PlayMenuSound("moption.wav");
            effect_num_ = 0;
            break;
          case 4:
            menu_type_ = MENU_CLASH_THRESHOLD;
            clash_t_ = GetCurrentClashThreshold();
            SayNumber(clash_t_, SAY_DECIMAL);
            PlayMenuSound("mselect.wav");
            break;
          case 5:
  #if NUM_BLADES == 1
            menu_type_ = MENU_LENGTH;
            SaveState(current_preset_.preset_num);
            blade_num_ = 1;
            max_length_ = GetMaxBladeLength(blade_num_);
            blade_length_ = GetBladeLength(blade_num_);
            if (blade_length_ < 1) blade_length_ = max_length_;
            length_revert_ = blade_length_;
            LengthPreview(blade_num_);
            PlayMenuSound("mselect.wav");
            break;
  #else
            menu_type_ = MENU_BLADE_LENGTH;
            blade_num_ = 0;
            PlayMenuSound("mblade.wav");
            break; 
  #endif             
          case 6:
            menu_type_ = MENU_DIM_BLADE;
            dim_revert_ = dim = pow(SaberBase::GetCurrentDimming() / 16384.0, 1/2.5);
            PlayMenuSound("mselect.wav");
            break;
        }
        break;
      case MENU_CLASH_THRESHOLD:
        SetClashThreshold(clash_t_);
        SaveGlobalState();
        menu_type_ = MENU_SETTING_SUB;
        MenuSave();
        break;
      case MENU_DIM_BLADE:
        SaveGlobalState();
        menu_type_ = MENU_SETTING_SUB;
        MenuSave();
        break;
      case MENU_STYLE_SETTING_SUB:
        char argspace[32];
        style_parser.GetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, argspace);
        calc_ = strtol(argspace, NULL, 0);
        int_edit = calc_;
        PlayMenuSound("mselect.wav");
        if (NUM_BLADES == 1) blade_num_ = 1;
        switch (set_num_) {
          case 16:
            menu_type_ = MENU_STYLE_OPTION;
            arg_revert_ = strtol (argspace, NULL, 0);
            break;
          case 17:
            menu_type_ = MENU_IGNITION_OPTION;
            arg_revert_ = strtol (argspace, NULL, 0);
            break;
          case 18:
            PlayMenuSound("mselect.wav");
            menu_type_ = MENU_IGNITION_TIME;
            arg_revert_ = strtol (argspace, NULL, 0);
            break;
          case 19:
            PlayMenuSound("mselect.wav");
            menu_type_ = MENU_RETRACTION_OPTION;
            char ig[32];
            style_parser.GetArgument(current_preset_.GetStyle(blade_num_), 19, ig);
            ignite_time_ = strtol(ig, NULL, 0);
            current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 19, "1"));
            arg_revert_ = strtol(argspace, NULL, 0);
            break;
          case 20:
            PlayMenuSound("mselect.wav");
            menu_type_ = MENU_RETRACTION_TIME;
            char igt[32];
            style_parser.GetArgument(current_preset_.GetStyle(blade_num_), 19, igt);
            ignite_time_ = strtol(igt, NULL, 0);
            current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 19, "1"));
            arg_revert_ = strtol(argspace, NULL, 0);
            break;
          case 21:
            PlayMenuSound("mselect.wav");
            menu_type_ = MENU_CLASH_LOCATION;
            ShowColorStyle::SetColor(GetColorArg(blade_num_, 4));
            show_clash_location_.Start(blade_num_);
            break;
          case 22:
            PlayMenuSound("mselect.wav");
            menu_type_ = MENU_DRAG_SIZE;
            ShowColorStyle::SetColor(GetColorArg(blade_num_, 6));
            show_drag_size_.Start(blade_num_);
            break;
          case 23:
            PlayMenuSound("mselect.wav");
            menu_type_ = MENU_EMITTER_SIZE;
            ShowColorStyle::SetColor(GetColorArg(blade_num_, 1));
            show_emitter_size_.Start(blade_num_);
            break;
          case 24:
            PlayMenuSound("moption.wav");
            if (SFX_preon) {
              menu_type_ = MENU_PREON_OPTION;
              arg_revert_ = strtol (argspace, NULL, 0);
            }
            break;
          case 25:
            if (!SFX_preon) {
              PlayMenuSound("moption.wav");
            } else {
              PlayMenuSound("mselect.wav");
              menu_type_ = MENU_PREON_SIZE;
              arg_revert_ = strtol (argspace, NULL, 0);
            }
            break;
          default:
            break;
        }
        break;
      case MENU_STYLE_OPTION:
      case MENU_IGNITION_OPTION:
      case MENU_IGNITION_TIME:
      case MENU_PREON_OPTION:
      case MENU_PREON_SIZE:
         menu_type_ = MENU_STYLE_SETTING_SUB;
         current_preset_.Save();
         MenuSave();
         break;
      case MENU_RETRACTION_OPTION:
      case MENU_RETRACTION_TIME:
         menu_type_ = MENU_STYLE_SETTING_SUB;
         char style_arg[10];
         itoa(ignite_time_, style_arg, 10);
         current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 19, style_arg));
         current_preset_.Save();
         MenuSave();
         break;
      case MENU_CLASH_LOCATION:
      case MENU_DRAG_SIZE:
      case MENU_EMITTER_SIZE:
        SaveStyleSetting();
        break;
      case MENU_DELETE:
        if (choice_) {
          current_preset_.SaveAt(-1);
          choice_ = false;
          menu_type_ = MENU_TOP;
          MenuSelect();
          break;
        }
        PlayMenuSound("maffirm.wav");
        break;
      case MENU_EXIT:
        PlayMenuSound("mselect.wav");
        MenuExit();
        break;
  #endif
      default:
        break;  
      }
  }

// Edit Mode Dial Right      
  void MenuDialRight() {
    switch (menu_type_) {
      case MENU_PRESET:
        if (SaberBase::IsOn()) {
          next_preset_fast();
        } else {
          next_preset();
        }
        break;
      case MENU_VOLUME:
         VolumeUp();
         break;
      case TRACK_PLAYER:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        track_num_ += 1;
        if (track_num_ > num_tracks_ - 1) track_num_ = 0;
        char playtrack[128];
        RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, playtrack, sizeof(playtrack));
  #ifdef ENABLE_AUDIO
        MountSDCard();
        EnableAmplifier();
        track_player_ = GetFreeWavPlayer();
        if (track_player_) {
          track_player_->Play(playtrack);
        } else {
          STDOUT.println("No available WAV players.");
        }
  #else
        STDOUT.println("Audio disabled.");
  #endif
        break;
  #ifdef FETT263_EDIT_MODE_MENU
      case MENU_TOP:
        menu_top_pos_ = (menu_top_pos_ + 1) % SUBMENUS;
        if (menu_top_pos_ == 0) menu_top_pos_ = 1;
        switch (menu_top_pos_) {
          case 1:
            PlayMenuSound("mstyle.wav");
            break;
          case 2:
            PlayMenuSound("mcolor.wav");
            break;
          case 3:
            PlayMenuSound("mfont.wav");
            break;
          case 4:
            PlayMenuSound("mtrack.wav");
            break;
          case 5:
            PlayMenuSound("msetting.wav");
            break;
          case 6:
            PlayMenuSound("mcopy.wav");
            break;
          case 7:
            PlayMenuSound("mdelete.wav");
            break;
          case 8:
            PlayMenuSound("mexit.wav");
            break;
       }
       break;
  #if NUM_BLADES > 1
      case MENU_BLADE_STYLE:
        if (blade_num_ == NUM_BLADES) blade_num_ = 0;
        blade_num_ += 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_BLADE_COLOR:
        if (blade_num_ == NUM_BLADES) blade_num_ = 0;
        blade_num_ += 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_BLADE_SETTING:
        if (blade_num_ == NUM_BLADES) blade_num_ = 0;
        blade_num_ += 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_BLADE_COPY:
        copy_blade_ = (copy_blade_ + 1) % NUM_BLADES + 1;
        if (copy_blade_ == 0) copy_blade_ = (copy_blade_ + 1) % NUM_BLADES + 1;
        if (copy_blade_ == blade_num_) copy_blade_ = (copy_blade_ + 1) % NUM_BLADES + 1;
        BladePreview(copy_blade_);
        SFX_mnum.Select(copy_blade_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;           
      case MENU_BLADE_LENGTH:
        if (blade_num_ == NUM_BLADES) blade_num_ = 0;
        blade_num_ += 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_COPY_COLOR:
        choice_ = true;
        PlayMenuSound("mconfirm.wav");
        break;
  #endif
      case MENU_STYLE:
        style_num_ = (style_num_ + 1)% num_presets_;
        char style_arg[10];
        itoa(style_num_, style_arg, 10);
        current_preset_.SetStyle(blade_num_, style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 1, style_arg)); 
        current_preset_.Save();
        PlayMenuSound("mup.wav");
        UpdateStyle(current_preset_.preset_num);
        break;
      case MENU_COLOR:
        break;
      case MENU_COLOR_SUB:
  #if NUM_BLADES == 1
        menu_sub_pos_ = (menu_sub_pos_ + 1) % 3;
        if (menu_sub_pos_ == 0) menu_sub_pos_ = 1;
        switch (menu_sub_pos_) {
          case 1:
            PlayMenuSound("mcolorop.wav");
            break;
          case 2:
            PlayMenuSound("mresetc.wav");
            break;
        }
  #else
        menu_sub_pos_ = (menu_sub_pos_ + 1) % 4;
        if (menu_sub_pos_ == 0) menu_sub_pos_ = 1;
        switch (menu_sub_pos_) {
          case 1:
            PlayMenuSound("mcolorop.wav");
            break;
          case 2:
            PlayMenuSound("mcopyc.wav");
            break;
          case 3:
            PlayMenuSound("mresetc.wav");
            break;
        }
  #endif
        break;
      case MENU_EFFECT:
        while (true) {
          effect_num_ = (effect_num_ + 1)% COLOR_SUB;
          if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), effect_num_ + 2)) break;
        }
        if (effect_num_ == 0) effect_num_ = 1;
        switch (effect_num_) {
          case 1:
            // Base Color Edit
            PlayMenuSound("mbase.wav");
            break;
          case 2:
            // Alt Color Edit
            PlayMenuSound("malt.wav");
            break;
          case 3:
            // Blast Color Edit
            PlayMenuSound("mblast.wav");
            break;
          case 4:
            // Clash Color Edit
            PlayMenuSound("mclash.wav");
            break;
          case 5:
            // Lockup Color Edit
            PlayMenuSound("mlockup.wav");
            break;
          case 6:
            // Drag Color Edit
            PlayMenuSound("mdrag.wav");
            break;
          case 7:
            // Lightning Block Color Edit
            PlayMenuSound("mlb.wav");
            break;
          case 8:
            // Stab Color Edit
            PlayMenuSound("mstab.wav");
            break;
          case 9:
            // PreOn Color Edit
            PlayMenuSound("mpreon.wav");
            break;
          case 10:
            // Ignition Color Edit
            PlayMenuSound("mignite.wav");
            break;
          case 11:
            // Retraction Color Edit
            PlayMenuSound("mretract.wav");
            break;
          case 12:
            // PstOff Color Edit
            PlayMenuSound("mpstoff.wav");
            break;
          case 13:
            // Swing Color Edit
            PlayMenuSound("mswing.wav");
            break;
          case 14:
            // Emitter Color Edit
            PlayMenuSound("memitter.wav");
            break;
          case 15:
            // Off Color Edit
            PlayMenuSound("moff.wav");
            break;
        }
        break;
      case MENU_COLOR_MODE:
        sub_dial_ = (sub_dial_ + 1) % 4;
        switch (sub_dial_) {
          case 0:
            PlayMenuSound("mcolorlt.wav");
            break;
          case 1:
            PlayMenuSound("mhue.wav");
            break;
          case 2:
            PlayMenuSound("mwhite.wav");
            break;
          case 3:
            PlayMenuSound("mblack.wav");
            break;
          default:
            PlayMenuSound("moption.wav");
            break;
          }
          break;
      case COLOR_BASE:
      case COLOR_ALT:
      case COLOR_BLAST:
      case COLOR_CLASH:
      case COLOR_LOCKUP:
      case COLOR_DRAG:
      case COLOR_LB:
      case COLOR_STAB:
      case COLOR_PREON:
      case COLOR_IGNITE:
      case COLOR_RETRACT:
      case COLOR_PSTOFF:
      case COLOR_SWING:
      case COLOR_EMITTER:
      case COLOR_OFF:
        // Color List
        if (color_mode_ == COLOR_LIST) {
          dial_ = (dial_ + 1) % NELEM(color_list_);
          ShowColorStyle::SetColor(Color16(color_list_[dial_]));
          PlayMenuSound("mup.wav");
          break;
        }
        break;
      case MENU_FONT:
        font_num_ += 1;
        if (font_num_ >= num_fonts_) font_num_ = 0;
        char font[128];
        RunCommandAndGetSingleLine("list_fonts", nullptr, font_num_, font, sizeof(font));
        strcat(font, ";common");
        current_preset_.font = mkstr(font);
        current_preset_.Save();
        UpdateFont(current_preset_.preset_num);
        hybrid_font.SB_Effect(EFFECT_NEWFONT, 0);
        break;
      case MENU_TRACK:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        track_num_ += 1;
        if (track_num_ >= num_tracks_) track_num_ = 0;
        char track[128];
        RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, track, sizeof(track));
        current_preset_.track = mkstr(track);
        current_preset_.Save();
        StartOrStopTrack();
        break;
      case MENU_LENGTH:
        // Edit Blade Length
        if (blade_length_ < max_length_) {
          blade_length_ += 1;
          length_edit_length = blade_length_ - 1;
          PlayMenuSound("mup.wav");
          SetBladeLength(blade_num_, blade_length_);
          SaveState(current_preset_.preset_num);
        } else {
          PlayMenuSound("mmax.wav");
        }
        break;
      case MENU_RESET_COLOR:           
        choice_ = true;
        PlayMenuSound("maccept.wav");
        break;         
      case MENU_GESTURE_SUB:
        effect_num_ += 1;
        if (effect_num_ == GESTURE_OPTIONS + 1) effect_num_ = 1;
        switch (effect_num_) {
          case 1:
            PlayMenuSound("mswingon.wav");
            break;
          case 2:
            PlayMenuSound("mswingsp.wav");
            break;
          case 3:
            PlayMenuSound("mtwiston.wav");
            break;
          case 4:
            PlayMenuSound("mthrston.wav");
            break;
          case 5:
            PlayMenuSound("mstabon.wav");
            break;
          case 6:
            PlayMenuSound("mpush.wav");
            break;
          case 7:
            PlayMenuSound("mpushlen.wav");
            break;
          case 8:
            PlayMenuSound("mtwstoff.wav");
            break;
          case 9:
            PlayMenuSound("mlockdly.wav");
            break;
          case 10:
            PlayMenuSound("mbmclash.wav");
            break;
          case 11:
            PlayMenuSound("mpwrlock.wav");
            break;
          case 12:
            PlayMenuSound("maxclash.wav");
            break;
        }
        break;
      case MENU_SWINGON:
        choice_ = true;
        PlayMenuSound("menable.wav");
        break;         
      case MENU_SWINGON_SPEED:
        if (calc_ < 600) {
          PlayMenuSound("mup.wav");
          calc_ += 50;
        }
        if (calc_ >= 600) {
          calc_ = 600;
          PlayMenuSound("mmax.wav");
        }
        SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_TWISTON:
        choice_ = true;
        PlayMenuSound("menable.wav");
        break;         
      case MENU_THRUSTON:
        choice_ = true;
        PlayMenuSound("menable.wav");
        break;         
      case MENU_STABON:
        choice_ = true;
        PlayMenuSound("menable.wav");
        break;         
      case MENU_FORCEPUSH:
        choice_ = true;
        PlayMenuSound("menable.wav");
        break;         
      case MENU_FORCEPUSH_LENGTH:
      case MENU_CLASH_DETECT:
        if (calc_ < 10) {
          PlayMenuSound("mup.wav");
          calc_ += 1;
        }
        if (calc_ >= 10) {
          calc_ = 10;
          PlayMenuSound("mmax.wav");
        }
        SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_MAX_CLASH:
        if (calc_ < 16) {
          PlayMenuSound("mup.wav");
          calc_ += 1;
        }
        if (calc_ >= 16) {
          calc_ = 16;
          PlayMenuSound("mmax.wav");
        }
        SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_TWISTOFF:
        choice_ = true;
        PlayMenuSound("menable.wav");
        break;         
      case MENU_LOCKUP_DELAY:
        if (calc_ < 1200) {
          PlayMenuSound("mup.wav");
          calc_ += 100;
        }
        if (calc_ >= 1200) {
          calc_ = 1200;
          PlayMenuSound("mmax.wav");
        }
        SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_POWERLOCK:
        choice_ = true;
        PlayMenuSound("menable.wav");
        break;         
      case MENU_SETTING_SUB:
        if (menu_sub_pos_ == SET_SUBMENUS) menu_sub_pos_ = 0;
        menu_sub_pos_ += 1;
        switch (menu_sub_pos_) {
          case 1:
            PlayMenuSound("mvolume.wav");
            break;
          case 2:
            PlayMenuSound("mstylset.wav");
            break;
          case 3:
            PlayMenuSound("mcontrol.wav");
            break;
          case 4:
            PlayMenuSound("mclashth.wav");
            break;
          case 5:
            PlayMenuSound("mlength.wav");
            break;
          case 6:
            PlayMenuSound("mdim.wav");
          default:
            break;
        }
        break;
      case MENU_CLASH_THRESHOLD:
        if (clash_t_ >= 4.0) {
          PlayMenuSound("mmax.wav");
          clash_t_ = 4.0;
        } else {
          PlayMenuSound("mup.wav");
          clash_t_ += 0.25;
          if (clash_t_ > 4.0) clash_t_ = 4.0;
        }
        SayNumber(clash_t_, SAY_DECIMAL);
        break;
      case MENU_DIM_BLADE:
        dim = std::min<float>(dim + 0.1, 1.0);
        if (dim == 1.0) {
          PlayMenuSound("mmax.wav");
        } else {
          PlayMenuSound("mup.wav");
        }
        SaberBase::SetDimming(pow(dim, 2.2) * 16384);
        break;
      case MENU_STYLE_SETTING_SUB:
        while (true) {
          effect_num_ = (effect_num_ + 1)% STYLE_SETTINGS;
          if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), effect_num_ + 17)) break;
        }
        if (effect_num_ == 0) effect_num_ = 1;
        set_num_ = effect_num_ + 15;
        switch (set_num_) {
          case 16:
            PlayMenuSound("mstylopt.wav");
            break;
          case 17:
            PlayMenuSound("migopt.wav");
            break;
          case 18:
            PlayMenuSound("mouttime.wav");
            break;
          case 19:
            PlayMenuSound("mrtopt.wav");
            break;
          case 20:
            PlayMenuSound("mintime.wav");
            break;
          case 21:
            PlayMenuSound("mlockpos.wav");
            break;
          case 22:
            PlayMenuSound("mdragsz.wav");
            break;
          case 23:
            PlayMenuSound("memitsz.wav");
            break;
          case 24:
            if (SFX_preon) {
              PlayMenuSound("mpreopt.wav");
            } else {
              effect_num_ = 0;
            }
            break;
          case 25:
            if (SFX_preon) {
              PlayMenuSound("mpreonsz.wav");
            } else {
              effect_num_ = 0;
            }
            break;
          default:
            PlayMenuSound("moption.wav");
            break;
        }
        break;
      case MENU_IGNITION_TIME:
      case MENU_RETRACTION_TIME:
        calc_ += 100;
        SetInOut();
        break;
      case MENU_STYLE_OPTION:
      case MENU_IGNITION_OPTION:
      case MENU_RETRACTION_OPTION:
      case MENU_PREON_OPTION:
        calc_ += 1;
        if (calc_ > 99) calc_ = 0;
        SetInOut();
        break;
      case MENU_CLASH_LOCATION:
      case MENU_EMITTER_SIZE:
      case MENU_PREON_SIZE:
        calc_ += 1000;
        int_edit = calc_;
        PlayMenuSound("mup.wav");
        break;
      case MENU_DRAG_SIZE:
        if (calc_ > 1000) {
          calc_ -= 1000;
          PlayMenuSound("mup.wav");
        } else {
          if (calc_ < 1000) {
            calc_ = 0;
            PlayMenuSound("mmax.wav");
          }
        }
        int_edit = calc_;
        break;
      case MENU_COPY:
        choice_ = true;
        PlayMenuSound("maccept.wav");
        break;
      case MENU_DELETE:
        choice_ = true;
        PlayMenuSound("maccept.wav");
        break;
  #endif            
      }
  }

// Edit Mode Dial Left
  void MenuDialLeft() {
    switch (menu_type_) {
      case MENU_PRESET:
        if (SaberBase::IsOn()) {
          previous_preset_fast();
        } else {
          previous_preset();
        }
        break;
      case MENU_VOLUME:
        VolumeDown();
        break;
      case TRACK_PLAYER:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        track_num_ -= 1;
        if (track_num_ < 0) track_num_ = num_tracks_ - 1;
        char playtrack[128];
        RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, playtrack, sizeof(playtrack));
#ifdef ENABLE_AUDIO
        MountSDCard();
        EnableAmplifier();
        track_player_ = GetFreeWavPlayer();
        if (track_player_) {
          track_player_->Play(playtrack);
        } else {
          STDOUT.println("No available WAV players.");
        }
#else
        STDOUT.println("Audio disabled.");
#endif
        break;
#ifdef FETT263_EDIT_MODE_MENU
      case MENU_TOP:
        if (menu_top_pos_ <= 1) menu_top_pos_ = SUBMENUS;  
        menu_top_pos_ = menu_top_pos_ - 1;
        switch (menu_top_pos_) {
          case 1:
            PlayMenuSound("mstyle.wav");
            break;
          case 2:
            PlayMenuSound("mcolor.wav");
            break;
          case 3:
            PlayMenuSound("mfont.wav");
            break;
          case 4:
            PlayMenuSound("mtrack.wav");
            break;
          case 5:
            PlayMenuSound("msetting.wav");
            break;
          case 6:
            PlayMenuSound("mcopy.wav");
            break;
          case 7:
            PlayMenuSound("mdelete.wav");
            break;
          case 8:
            PlayMenuSound("mexit.wav");
            break;
        }
        break;
#if NUM_BLADES > 1
      case MENU_BLADE_STYLE:
        if (blade_num_ <= 1) blade_num_ = NUM_BLADES + 1;
        blade_num_ -= 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_BLADE_SETTING:
        if (blade_num_ <= 1) blade_num_ = NUM_BLADES + 1;
        blade_num_ -= 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_BLADE_COLOR:
        if (blade_num_ <= 1) blade_num_ = NUM_BLADES + 1;
        blade_num_ -= 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_BLADE_COPY:
        if (copy_blade_ <= 1) copy_blade_ = NUM_BLADES + 1;
        copy_blade_ -= 1;
        if (copy_blade_ == blade_num_) {
          copy_blade_ -= 1;
          if (copy_blade_ == 0) copy_blade_ = NUM_BLADES;
        }
        BladePreview(copy_blade_);
        SFX_mnum.Select(copy_blade_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_BLADE_LENGTH:
        if (blade_num_ <= 1) blade_num_ = NUM_BLADES + 1;
        blade_num_ -= 1;
        BladePreview(blade_num_);
        SFX_mnum.Select(blade_num_ - 1); 
        wav_player->PlayOnce(&SFX_mnum, 0.0);
        break;
      case MENU_COPY_COLOR:
        choice_ = false;
        PlayMenuSound("mcancel.wav");
        break;
#endif           
      case MENU_STYLE:
        style_num_ -= 1;
        if (style_num_ < 0) style_num_ = num_presets_ - 1;
        char style_arg[10];
        itoa(style_num_, style_arg, 10);
        current_preset_.SetStyle(blade_num_, style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 1, style_arg)); 
        current_preset_.Save();
        UpdateStyle(current_preset_.preset_num);
        PlayMenuSound("mdown.wav");
        break;
      case MENU_COLOR:
        break;
      case MENU_COLOR_SUB:
#if NUM_BLADES == 1
        menu_sub_pos_ -= 1;
        if (menu_sub_pos_ < 1) menu_sub_pos_ = 2;  
        switch (menu_sub_pos_) {
          case 1:
            // Color Options
            PlayMenuSound("mcolorop.wav");
            break;
          case 2:
            // Reset Colors
            PlayMenuSound("mresetc.wav");
            break;
        }
#else
        menu_sub_pos_ -= 1;
        if (menu_sub_pos_ < 1) menu_sub_pos_ = 3;  
        switch (menu_sub_pos_) {
          case 1:
            // Color Options
            PlayMenuSound("mcolorop.wav");
            break;
          case 2:
            // Copy Colors
            PlayMenuSound("mcopyc.wav");
            break;
          case 3:
            // Reset Colors
            PlayMenuSound("mresetc.wav");
            break;
        }
#endif
        break;
      case MENU_EFFECT:
        while (true) {
          if (effect_num_ == 0) effect_num_ = COLOR_SUB + 1;
          effect_num_ -= 1;
          if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), effect_num_ + 2)) break;
        }
        switch (effect_num_) {
          case 1:
            // Base Color Edit
            PlayMenuSound("mbase.wav");
            break;
          case 2:
            // Alt Color Edit
            PlayMenuSound("malt.wav");
            break;
          case 3:
           // Blast Color Edit
           PlayMenuSound("mblast.wav");
           break;
          case 4:
            // Clash Color Edit
            PlayMenuSound("mclash.wav");
            break;
          case 5:
            // Lockup Color Edit
            PlayMenuSound("mlockup.wav");
            break;
          case 6:
            // Drag Color Edit
            PlayMenuSound("mdrag.wav");
            break;
          case 7:
            // Lightning Block Color Edit
            PlayMenuSound("mlb.wav");
            break;
          case 8:
            // Stab Color Edit
            PlayMenuSound("mstab.wav");
            break;
          case 9:
            // PreOn Color Edit
            PlayMenuSound("mpreon.wav");
            break;
          case 10:
            // Ignition Color Edit
            PlayMenuSound("mignite.wav");
            break;
          case 11:
            // Retraction Color Edit
            PlayMenuSound("mretract.wav");
            break;
          case 12:
            // PstOff Color Edit
            PlayMenuSound("mpstoff.wav");
            break;
          case 13:
            // Swing Color Edit
            PlayMenuSound("mswing.wav");
            break;
          case 14:
            // Emitter Color Edit
            PlayMenuSound("memitter.wav");
            break;
          case 15:
            // Off Color Edit
            PlayMenuSound("moff.wav");
            break;
        }
        break;
      case MENU_COLOR_MODE:
        sub_dial_ -= 1;
        if (sub_dial_ < 0) sub_dial_ = 3;
        switch (sub_dial_) {
          case 0:
            // Color List
            PlayMenuSound("mcolorlt.wav");
             break;
          case 1:
            // Adjust Color Hue
            PlayMenuSound("mhue.wav");
            break;
          case 2:
            // Adjust White Level
            PlayMenuSound("mwhite.wav");
            break;
          case 3:
            // Adjust Black Level
            PlayMenuSound("mblack.wav");
            break;
          default:
            break;
          }
          break;
      case COLOR_BASE:
      case COLOR_ALT:
      case COLOR_BLAST:
      case COLOR_CLASH:
      case COLOR_LOCKUP:
      case COLOR_DRAG:
      case COLOR_LB:
      case COLOR_STAB:
      case COLOR_PREON:
      case COLOR_IGNITE:
      case COLOR_RETRACT:
      case COLOR_PSTOFF:
      case COLOR_SWING:
      case COLOR_EMITTER:
      case COLOR_OFF:
        // Color List
        if (color_mode_ == COLOR_LIST) {
          if (dial_ <= 0) dial_ = NELEM(color_list_);
          dial_ = dial_ - 1;
          ShowColorStyle::SetColor(Color16(color_list_[dial_]));
          PlayMenuSound("mdown.wav");
        }
        break;
      case MENU_FONT:
        font_num_ -= 1;
        if (font_num_ < 0) font_num_ = num_fonts_ - 1;
        char font[128];
        RunCommandAndGetSingleLine("list_fonts", nullptr, font_num_, font, sizeof(font));
        strcat(font, ";common");
        current_preset_.font = mkstr(font);
        current_preset_.Save();
        UpdateFont(current_preset_.preset_num);
        hybrid_font.SB_Effect(EFFECT_NEWFONT, 0);
        break;
      case MENU_TRACK:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        track_num_ -= 1;
        if (track_num_ < 0) track_num_ = num_tracks_ - 1;
        char track[128];
        RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, track, sizeof(track));
        current_preset_.track = mkstr(track);
        current_preset_.Save();
        StartOrStopTrack();
        break;
      case MENU_LENGTH:
        if (blade_length_ > 2) {
          blade_length_ -= 1;
          PlayMenuSound("mdown.wav");
          length_edit_length = blade_length_ - 1;
          SetBladeLength(blade_num_, blade_length_);
          SaveState(current_preset_.preset_num);
        } else {
          PlayMenuSound("mmin.wav");
        }
        break;
      case MENU_RESET_COLOR:
        choice_ = false;
        PlayMenuSound("mcancel.wav");
        break;
      case MENU_GESTURE_SUB:
        effect_num_ -= 1;
        if (effect_num_ < 1) effect_num_ = GESTURE_OPTIONS;
        switch (effect_num_) {
          case 1:
            PlayMenuSound("mswingon.wav");
            break;
          case 2:
            PlayMenuSound("mswingsp.wav");
            break;
          case 3:
            PlayMenuSound("mtwiston.wav");
            break;
          case 4:
            PlayMenuSound("mthrston.wav");
            break;
          case 5:
            PlayMenuSound("mstabon.wav");
            break;
          case 6:
            PlayMenuSound("mpush.wav");
            break;
          case 7:
            PlayMenuSound("mpushlen.wav");
            break;
          case 8:
            PlayMenuSound("mtwstoff.wav");
            break;
          case 9:
            PlayMenuSound("mlockdly.wav");
            break;
          case 10:
            PlayMenuSound("mbmclash.wav");
            break;
          case 11:
            PlayMenuSound("mpwrlock.wav");
            break;
          case 12:
            PlayMenuSound("maxclash.wav");
            break;
        }
        break;
      case MENU_SWINGON:
        choice_ = false;
        PlayMenuSound("mdisable.wav");
        break;         
      case MENU_SWINGON_SPEED:
        if (calc_ > 200) {
          PlayMenuSound("mdown.wav");
          calc_ -= 50;
        }
        if (calc_ <= 200) {
          calc_ = 200;
          PlayMenuSound("mmin.wav");
        }
        SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_TWISTON:
        choice_ = false;
        PlayMenuSound("mdisable.wav");
        break;         
      case MENU_THRUSTON:
        choice_ = false;
        PlayMenuSound("mdisable.wav");
        break;         
      case MENU_STABON:
        choice_ = false;
        PlayMenuSound("mdisable.wav");
        break;         
      case MENU_FORCEPUSH:
        choice_ = false;
        PlayMenuSound("mdisable.wav");
        break;         
      case MENU_FORCEPUSH_LENGTH:
      case MENU_CLASH_DETECT:
        if (calc_ > 1) {
          PlayMenuSound("mdown.wav");
          calc_ -= 1;
        }
        if (calc_ <= 1) {
          calc_ = 1;
          PlayMenuSound("mmin.wav");
        }
        SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_MAX_CLASH:
        if (calc_ > 8) {
          PlayMenuSound("mdown.wav");
          calc_ -= 1;
        }
        if (calc_ <= 8) {
          calc_ = 8;
          PlayMenuSound("mmin.wav");
        }
        SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_TWISTOFF:
        choice_ = false;
        PlayMenuSound("mdisable.wav");
        break;         
      case MENU_LOCKUP_DELAY:
        if (calc_ > 200) {
          PlayMenuSound("mdown.wav");
          calc_ -= 100;
        }
        if (calc_ <= 200) {
          calc_ = 200;
          PlayMenuSound("mmin.wav");
        }
        SayNumber(calc_, SAY_WHOLE); 
        break;
      case MENU_POWERLOCK:
        choice_ = false;
        PlayMenuSound("mdisable.wav");
        break;         
      case MENU_SETTING_SUB:
        if (menu_sub_pos_ <= 1) menu_sub_pos_ = SET_SUBMENUS + 1;
        menu_sub_pos_ -= 1;
        switch (menu_sub_pos_) {
          case 1:
            PlayMenuSound("mvolume.wav");
            break;
          case 2:
            PlayMenuSound("mstylset.wav");
            break;
          case 3:
            PlayMenuSound("mcontrol.wav");
            break;
          case 4:
            PlayMenuSound("mclashth.wav");
            break;
          case 5:
            PlayMenuSound("mlength.wav");
            break;
          case 6:
            PlayMenuSound("mdim.wav");
            break;
          default:
            break;
        }
        break;
      case MENU_CLASH_THRESHOLD:
        if (clash_t_ <= 1.0) {
          PlayMenuSound("mmin.wav");
          clash_t_ = 1.0;
        } else {
          PlayMenuSound("mdown.wav");
          clash_t_ -= 0.25;
          if (clash_t_ < 1.0) clash_t_ = 1.0;
        }
        SayNumber(clash_t_, SAY_DECIMAL);
        break;
      case MENU_DIM_BLADE:
        dim = std::max<float>(dim - 0.1, 0.2);
        if (dim == 0.2) {
          PlayMenuSound("mmin.wav");
        } else {
          PlayMenuSound("mdown.wav");
        }
        SaberBase::SetDimming(pow(dim, 2.2) * 16384);
        break;
      case MENU_STYLE_SETTING_SUB:
        while (true) {
          if (effect_num_ == 0) effect_num_ = STYLE_SETTINGS + 1;
          effect_num_ -= 1;
          if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), effect_num_ + 17)) break;
        }
        set_num_ = effect_num_ + 15;
        switch (set_num_) {
          case 16:
            PlayMenuSound("mstylopt.wav");
            break;
          case 17:
            PlayMenuSound("migopt.wav");
            break;
          case 18:
            PlayMenuSound("mouttime.wav");
            break;
          case 19:
            PlayMenuSound("mrtopt.wav");
            break;
          case 20:
            PlayMenuSound("mintime.wav");
            break;
          case 21:
            PlayMenuSound("mlockpos.wav");
            break;
          case 22:
            PlayMenuSound("mdragsz.wav");
            break;
          case 23:
            PlayMenuSound("memitsz.wav");
            break;
          case 24:
            if (SFX_preon) {
              PlayMenuSound("mpreopt.wav");
            } else {
              effect_num_ = STYLE_SETTINGS + 1;
            }
            break;
          case 25:
            if (SFX_preon) {
              PlayMenuSound("mpreonsz.wav");
            } else {
              effect_num_ = STYLE_SETTINGS + 1;
            }
            break;
          default:
            PlayMenuSound("moption.wav");
            break;
        }
        break;
      case MENU_IGNITION_TIME:
      case MENU_RETRACTION_TIME:
        if (calc_ >= 100) {
          calc_ -= 100;
          if (calc_ >= 100) {
            SetInOut();
          } else {
          calc_ = 0;
          PlayMenuSound("mauto.wav");
          next_event_ = true;
          }
        } else {
          calc_ = 0;
          PlayMenuSound("mauto.wav");
          next_event_ = true;
        }
        break;
      case MENU_STYLE_OPTION:
      case MENU_IGNITION_OPTION:
      case MENU_RETRACTION_OPTION:
      case MENU_PREON_OPTION:
        calc_ -= 1;
        if (calc_ < 0) calc_ = 99;
        SetInOut();
        break;
      case MENU_CLASH_LOCATION:
      case MENU_EMITTER_SIZE:
      case MENU_PREON_SIZE:
        if (calc_ > 0) {
          calc_ -= 1000;
          PlayMenuSound("mdown.wav");
        } else {
          if (calc_ < 1000) {
            calc_ = 0;
            PlayMenuSound("mmin.wav");
          }
        }
        int_edit = calc_;
        break;
      case MENU_DRAG_SIZE:
        if (calc_ < 32768) {
          calc_ += 1000;
          PlayMenuSound("mdown.wav");
        } else {
          if (calc_ >= 32768) {
            calc_ = 33000;
            PlayMenuSound("mmin.wav");
          }
        }
        int_edit = calc_;
        PlayMenuSound("mdown.wav");
        break;
      case MENU_COPY:
        choice_ = false;
        PlayMenuSound("mcancel.wav");
        break;
      case MENU_DELETE:
        choice_ = false;
        PlayMenuSound("mcancel.wav");           
        break;
#endif            
      }
  }

// Edit Mode Undo (AUX Button)
  void MenuUndo() {
    switch (menu_type_) {
      case MENU_TOP:
        PlayMenuSound("mexit.wav");
        MenuExit();
        break;
      case MENU_PRESET:
        first_preset();
        PlayMenuSound("mselect.wav");
        menu_type_ = MENU_TOP;
        menu_ = false;
        break;
#ifdef FETT263_EDIT_MODE_MENU
      case MENU_VOLUME:
        menu_type_ = MENU_SETTING_SUB;
        dynamic_mixer.set_volume(VOLUME);
        MenuRevert();
        break;
#if NUM_BLADES > 1
      case MENU_BLADE_STYLE:
        menu_type_ = MENU_TOP;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
      case MENU_BLADE_COLOR:
        menu_type_ = MENU_TOP;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
      case MENU_BLADE_COPY:
        menu_type_ = MENU_COLOR_SUB;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
      case MENU_BLADE_LENGTH:
        menu_type_ = MENU_SETTING_SUB;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
      case MENU_COPY_COLOR:
        menu_type_ = MENU_COLOR_SUB;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
#endif
      case MENU_STYLE:
        char style_arg[10];
        itoa(style_revert_, style_arg, 10);
        current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 1, style_arg)); 
        current_preset_.Save();
        UpdateStyle(current_preset_.preset_num);
#if NUM_BLADES == 1
        menu_type_ = MENU_TOP;
        MenuRevert();
#else
        menu_type_ = MENU_BLADE_STYLE;
        MenuRevert();
#endif
        break;
      case MENU_COLOR:
        menu_type_ = MENU_TOP;
        SaberBase::SetVariation(color_revert_);
        ToggleColorChangeMode();
        MenuCancel();
        break;
      case MENU_COLOR_SUB:
#if NUM_BLADES == 1
        menu_type_ = MENU_TOP;
        MenuCancel();
        break;
#else
        menu_type_ = MENU_BLADE_COLOR;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        blade_num_ = 0;
        MenuCancel();
        break;
#endif
      case MENU_COLOR_MODE:
        edit_color_ = false;
        menu_type_ = MENU_EFFECT;
        MenuCancel();
        break;
      case MENU_EFFECT:
        edit_color_ = false;
        menu_type_ = MENU_COLOR_SUB;
        MenuCancel();
        break;
      case COLOR_BASE:
      case COLOR_ALT:
      case COLOR_IGNITE:
      case COLOR_RETRACT:
      case COLOR_SWING:
      case COLOR_OFF:
        menu_type_ = MENU_COLOR_MODE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        show_color_.Stop(blade_num_);
        MenuRevert();
        color_mode_ = NONE;
        break;
      case COLOR_BLAST:
      case COLOR_CLASH:
      case COLOR_LOCKUP:
      case COLOR_LB:
        menu_type_ = MENU_COLOR_MODE;
        color_mode_ = NONE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        bump_color_.Stop(blade_num_);
        MenuRevert();
        color_mode_ = NONE;
        break;
      case COLOR_DRAG:
      case COLOR_STAB:
        menu_type_ = MENU_COLOR_MODE;
        color_mode_ = NONE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        tip_color_.Stop(blade_num_);
        MenuRevert();
        color_mode_ = NONE;
        break;
      case COLOR_PREON:
      case COLOR_PSTOFF:
      case COLOR_EMITTER:
        menu_type_ = MENU_COLOR_MODE;
        color_mode_ = NONE;
        edit_color_ = false;
        twist_menu_ = M_PI / 4;
        hilt_color_.Stop(blade_num_);
        MenuRevert();
        color_mode_ = NONE;
        break;
      case MENU_FONT:
        if (restore_point.get()) current_preset_.font = std::move(restore_point);
        current_preset_.Save();
        UpdateFont(current_preset_.preset_num);
        menu_type_ = MENU_TOP;
        MenuRevert();
        break;
      case MENU_TRACK:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        if (restore_point.get()) current_preset_.track = std::move(restore_point);
        current_preset_.Save();
        menu_type_ = MENU_TOP;
        MenuRevert();
        break;
      case MENU_LENGTH:
        menu_type_ = MENU_SETTING_SUB;
        SetBladeLength(blade_num_, length_revert_);
        SaveState(current_preset_.preset_num);
        MenuRevert();
        show_length_.Stop(blade_num_);
        UpdateStyle(current_preset_.preset_num);
        break;
      case MENU_COPY:
        choice_ = false;
        menu_type_ = MENU_TOP;
        MenuCancel();
        break;
      case MENU_RESET_COLOR:
        menu_type_ = MENU_COLOR_SUB;
        MenuCancel();
        break;
      case MENU_GESTURE_SUB:
        menu_type_ = MENU_SETTING_SUB;
        MenuCancel();
        break;
      case MENU_SWINGON:
      case MENU_SWINGON_SPEED:
      case MENU_TWISTON:
      case MENU_THRUSTON:
      case MENU_STABON:
      case MENU_FORCEPUSH:
      case MENU_FORCEPUSH_LENGTH:
      case MENU_TWISTOFF:
      case MENU_LOCKUP_DELAY:
      case MENU_CLASH_DETECT:
      case MENU_POWERLOCK:
      case MENU_MAX_CLASH:
        menu_type_ = MENU_GESTURE_SUB;
        MenuCancel();
        break;           
      case MENU_SETTING_SUB:
        menu_type_ = MENU_TOP;
        MenuCancel();
        break;
      case MENU_CLASH_THRESHOLD:
        menu_type_ = MENU_SETTING_SUB;
        MenuCancel();
        break;
      case MENU_DIM_BLADE:
        SaberBase::SetDimming(pow(dim_revert_, 2.5) * 16384);
        UpdateStyle(current_preset_.preset_num);
        menu_type_ = MENU_SETTING_SUB;
        SaveGlobalState();
        MenuRevert();
        break;
      case MENU_STYLE_SETTING_SUB:
        menu_type_ = MENU_SETTING_SUB;
        MenuCancel();
        break;
      case MENU_STYLE_OPTION:
      case MENU_IGNITION_OPTION:
      case MENU_IGNITION_TIME:
      case MENU_RETRACTION_OPTION:
      case MENU_RETRACTION_TIME:
      case MENU_PREON_OPTION:
      case MENU_PREON_SIZE:
        menu_type_ = MENU_STYLE_SETTING_SUB;
        RevertInOut();
        MenuRevert();
        break;
      case MENU_CLASH_LOCATION:
      case MENU_DRAG_SIZE:
      case MENU_EMITTER_SIZE:
        CancelStyleSetting();
        break;
      case MENU_DELETE:
      case MENU_EXIT:
      default:
        choice_ = false;
        menu_type_ = MENU_TOP;
        MenuCancel();
        break;
#endif
    }
  }

// Edit Mode Exit
  void MenuExit() {
    switch (menu_type_) {
#ifdef FETT263_SAVE_CHOREOGRAPHY      
      case MENU_REHEARSE:
        PlayMenuSound("rehrsold.wav");
        break;
#endif
      case MENU_PRESET:
        PlayMenuSound("mselect.wav");
        first_preset();
        break;
      case MENU_VOLUME:
        if (SFX_vmend) {
          PlayMenuSound("vmend.wav");
        } else {
          PlayMenuSound("mexit.wav");
        }
        break;
    }
    menu_type_ = MENU_TOP;
    menu_ = false;
    wav_player.Free();
  }
   
  void MenuSave() {
     sound_queue_.Play(SoundToPlay("msave.wav"));
     MenuPrompt();
  }
  
  void MenuSelect() {
     sound_queue_.Play(SoundToPlay("mselect.wav"));
     MenuPrompt();
  }
  
  void MenuConfirm() {
     sound_queue_.Play(SoundToPlay("mconfirm.wav"));
     MenuPrompt();
  }
   
  void MenuCancel() {
     sound_queue_.Play(SoundToPlay("mcancel.wav"));
     MenuPrompt();
  }
  
  void MenuRevert() {
     sound_queue_.Play(SoundToPlay("mrevert.wav"));
     MenuPrompt();
  }
   
  void MenuPrompt() {
    switch (menu_type_) {
      case MENU_TOP:
        sound_queue_.Play(SoundToPlay("mmain.wav"));
        break;
      case MENU_COLOR_SUB:
        sound_queue_.Play(SoundToPlay("mcolorsb.wav"));
        break;
      case MENU_COLOR_MODE:
        sound_queue_.Play(SoundToPlay("mcolorop.wav"));
        break;
#if NUM_BLADES > 1
      case MENU_BLADE_COLOR:
        sound_queue_.Play(SoundToPlay("mblade.wav"));
        break;
      case MENU_BLADE_STYLE:
        sound_queue_.Play(SoundToPlay("mblade.wav"));
        break;
      case MENU_BLADE_LENGTH:
        sound_queue_.Play(SoundToPlay("mblade.wav"));
        break;
#endif
      case MENU_GESTURE_SUB:
        sound_queue_.Play(SoundToPlay("mgestsub.wav"));
        break;
      case MENU_SETTING_SUB:
        sound_queue_.Play(SoundToPlay("msetsub.wav"));
        break;
      case MENU_EFFECT:
        sound_queue_.Play(SoundToPlay("meffect.wav"));
        break;
      case MENU_STYLE_SETTING_SUB:
        sound_queue_.Play(SoundToPlay("mstylstm.wav"));
        break;
    }    
  }

  enum EditColorMode {
    NONE,
    CC_COLOR_LIST,
#ifdef FETT263_EDIT_MODE_MENU
    COLOR_LIST,
    EDIT_BLACK,
    EDIT_COLOR,
    EDIT_WHITE,
    ZOOM_COLOR,
#endif
  };

#define CC_NEW_COLOR(N) NewColor(N,1);

// Update Style
  virtual void UpdateStyle(int preset_num) {
    TRACE(PROP, "start");
    SaveColorChangeIfNeeded();
    // First free all styles, then allocate new ones to avoid memory
    // fragmentation.
#define UNSET_BLADE_STYLE(N) \
    delete current_config->blade##N->UnSetStyle();
    ONCEPERBLADE(UNSET_BLADE_STYLE)
    current_preset_.SetPreset(preset_num);
#ifdef DYNAMIC_BLADE_LENGTH
    savestate_.ReadINIFromSaveDir("curstate");
#define WRAP_BLADE_SHORTERNER(N) \
    if (savestate_.blade##N##len != -1 && savestate_.blade##N##len != current_config->blade##N->num_leds()) { \
      tmp = new BladeShortenerWrapper(savestate_.blade##N##len, tmp);	\
    }
#else
#define WRAP_BLADE_SHORTERNER(N)
#endif

    
#define SET_BLADE_STYLE(N) do {						\
    BladeStyle* tmp = style_parser.Parse(current_preset_.current_style##N.get()); \
    WRAP_BLADE_SHORTERNER(N)                                            \
    current_config->blade##N->SetStyle(tmp);				\
  } while (0);

    ONCEPERBLADE(SET_BLADE_STYLE)

#ifdef SAVE_COLOR_CHANGE
    SaberBase::SetVariation(current_preset_.variation);
#else
    SaberBase::SetVariation(0);
#endif
    TRACE(PROP, "end");
  }

// Update Font / Save Style in Edit Mode, skips Preon effect (except for Preon Editing previews) using FastOn
  virtual void UpdateFont(int preset_num) {
    TRACE(PROP, "start");
    bool on = SaberBase::IsOn();
    if (on) Off();
    SaveColorChangeIfNeeded();
    // First free all styles, then allocate new ones to avoid memory
    // fragmentation.
#define UNSET_BLADE_STYLE(N) \
    delete current_config->blade##N->UnSetStyle();
    ONCEPERBLADE(UNSET_BLADE_STYLE)
    current_preset_.SetPreset(preset_num);
#define SET_BLADE_STYLE(N) \
    current_config->blade##N->SetStyle(style_parser.Parse(current_preset_.current_style##N.get()));
    ONCEPERBLADE(SET_BLADE_STYLE)
    chdir(current_preset_.font.get());
#ifdef SAVE_COLOR_CHANGE
    SaberBase::SetVariation(current_preset_.variation);
#else
    SaberBase::SetVariation(0);
#endif
    if (on) {
#ifdef FETT263_EDIT_MODE_MENU
      if (menu_type_ == MENU_PREON_OPTION || menu_type_ == MENU_PREON_SIZE) {
        On();
      } else {
        FastOn();
      }
#else
      FastOn();
#endif      
    }
    TRACE(PROP, "end");
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (PropBase::Parse(cmd, arg)) return true;
    if (!strcmp(cmd, "list_current_tracks")) {
      // Tracks must be in: font/tracks/*.wav 
      LOCK_SD(true);
      for (const char* dir = current_directory; dir; dir = next_current_directory(dir)) {
        PathHelper path(dir, "tracks");
        ListTracks(path);
      }
      LOCK_SD(false);
      return true;
    }
    return false;
  }

  // Go to first Preset.
  virtual void first_preset() {
#ifdef SAVE_PRESET
    SaveState(0);
#endif
    UpdateFont(0);
}

  // Go to the next Preset skipping Preon effect with FastOn.
  virtual void next_preset_fast() {
#ifdef SAVE_PRESET
    SaveState(current_preset_.preset_num + 1);
#endif
    UpdateFont(current_preset_.preset_num + 1);
  }

  // Go to the previous Preset skipping Preon effect with FastOn.
  virtual void previous_preset_fast() {
#ifdef SAVE_PRESET
    SaveState(current_preset_.preset_num - 1);
#endif
    UpdateFont(current_preset_.preset_num - 1);
  }

#ifdef FETT263_DUAL_MODE_SOUND
  // Select out.wav based on blade orientation, up = odd, down = even
  void SelectIgnitionSound() {
      Effect* effect;
      if (SFX_poweron) {
        effect = &SFX_poweron;
      } else {
        effect = &SFX_out;
      }
      int f = effect->files_found();
      if (fusor.angle1() > 0) {
        f = (rand() % ((f + 1)/ 2)) * 2;
      } else {
        f = 1 + (rand() % (f / 2)) * 2;
      }
      effect->Select(f);
    }
#endif

  void PlayQuote() {
    if (SFX_quote) {
      SFX_quote.SelectNext();
      wav_player->PlayOnce(&SFX_quote, 0.0);
    } else {
      SFX_force.SelectNext();
      wav_player->PlayOnce(&SFX_force, 0.0);
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
      PlayMenuSound("volup.wav");
      STDOUT.print("Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      PlayMenuSound("volmax.wav");
    }
    return;
  }

  void VolumeDown() {
    STDOUT.println("Volume Down");
    if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
                                             dynamic_mixer.get_volume() - VOLUME * 0.10));
      PlayMenuSound("voldown.wav");
      STDOUT.print("Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      PlayMenuSound("volmin.wav");
    }
    return;
  }

RefPtr<BufferedWavPlayer> wav_player;

// Menu Sound Player
void PlayMenuSound(const char* file) {
  if (!wav_player) {
  wav_player = GetFreeWavPlayer();
  if (!wav_player) return;
  }
  wav_player->set_volume_now(1.0);
  if (wav_player->PlayInCurrentDir(file)) return;
}

  float current_menu_angle_ = 0.0;
  float current_twist_angle_ = 0.0;
  float clash_t_ = 2.00;
#ifdef FETT263_EDIT_MODE_MENU
  LSPtr<char> restore_point;
  int blade_length_;
  int max_length_;
  float dim;
  float dim_revert_;
#endif

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):                
        if (menu_) {
          MenuChoice();
          return true;
        } else {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif          
          if (SFX_preon) {
            On();
          } else {
            FastOn();
          }
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
#ifdef FETT263_SAVE_GESTURE_OFF    
        RestoreGestureState();
#else
        saved_gesture_control.gestureon = true;
#endif
        FindBladeAgain();
        SaberBase::DoBladeDetect(true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = false;
#ifdef FETT263_SAVE_GESTURE_OFF    
        SaveGestureState();
#endif
        saved_gesture_control.gestureon = false;
        FindBladeAgain();
        SaberBase::DoBladeDetect(false);
        return true;
#endif

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_POWER):
        if (!menu_) {
          current_menu_angle_ = fusor.angle2();
          menu_ = true;
          menu_type_ = MENU_VOLUME;
          if (SFX_vmbegin) {
            PlayMenuSound("vmbegin.wav");
          } else {
            PlayMenuSound("mvolume.wav");
          }
        }
        return true;
          
      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_OFF):
        current_menu_angle_ = fusor.angle2();
        PlayMenuSound("mpreset.wav");
        menu_ = true;
        menu_type_ = MENU_PRESET;
        return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF):          
        // Track Player - Play Random
        if (menu_) {        
          if (menu_type_ == TRACK_PLAYER) {
            track_mode_ = PLAYBACK_RANDOM;
            PlayMenuSound("mrandom.wav");
          }
          MenuExit();
          return true;
        } else { 
          if (fusor.angle1() < - M_PI / 4) {
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

      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_OFF):
        if (menu_ && menu_type_ == TRACK_PLAYER) {
          menu_ = false;
          menu_type_ = MENU_TOP;
          return true;
        } 
        return false;

      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF | BUTTON_POWER):
#ifdef FETT263_EDIT_MODE_MENU
        // Enter Edit Mode
        if (!menu_) {
          if (track_player_) {
            track_player_->Stop();
            track_player_.Free();
          }
          track_mode_ = PLAYBACK_OFF;
          if (!SFX_medit) {
              talkie.Say(talkie_error_in_15, 15);
              talkie.Say(talkie_font_directory_15, 15);
              STDOUT.println("Edit Mode prompts missing");
              return true;
          }
          STDOUT.println("Enter Edit Mode");
          GenerateIniFiles();
          PlayMenuSound("medit.wav");
          current_menu_angle_ = fusor.angle2();
          menu_top_pos_ = 0;
          menu_ = true;
          menu_type_ = MENU_TOP;
          FastOn();
        } 
#else
        // If Edit Mode Menu Disabled Save .ini files for SD card editing
        GenerateIniFiles();
        PlayMenuSound("msave.wav");
#endif
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF | BUTTON_POWER):
        if (menu_ && menu_type_ == TRACK_PLAYER) {
          track_mode_ = PLAYBACK_ROTATE;
          PlayMenuSound("mrotate.wav");
          return true;
        }
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF | BUTTON_POWER):
        if (menu_ && menu_type_ == TRACK_PLAYER) {
          track_mode_ = PLAYBACK_LOOP;
          PlayMenuSound("mloop.wav");
          return true;
        }
        return false;

      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_ON):
        if (menu_) {
          PlayMenuSound("mexit.wav");
          menu_ = false;
          menu_type_ = MENU_TOP;
          return true;
        }
        if (rehearse_) {
          PlayMenuSound("mcancel.wav");
          rehearse_ = false;
          return true;
        }
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

      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_ON | BUTTON_POWER):
        if (!menu_ && saved_gesture_control.powerlock || choreo_) {
          wav_player.Free();
          choreo_ = false;
          battle_mode_ = false;
          Off();
        }
        return true;


      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON):
        if (wav_player->isPlaying()) return true;
        if (color_mode_ == CC_COLOR_LIST) {
          dial_ = (dial_ + 1) % NELEM(color_list_);
          ONCEPERBLADE(CC_NEW_COLOR)
          current_preset_.Save();
          hybrid_font.PlayCommon(&SFX_ccchange);
          UpdateStyle(current_preset_.preset_num);   
          return true;
        }
        if (menu_) MenuDialRight();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON):
        if (wav_player->isPlaying()) return true;
        if (color_mode_ == CC_COLOR_LIST) {
          if (dial_ <= 0) dial_ = NELEM(color_list_);
          dial_ = dial_ - 1;
          ONCEPERBLADE(CC_NEW_COLOR)
          current_preset_.Save();
          hybrid_font.PlayCommon(&SFX_ccchange);
          UpdateStyle(current_preset_.preset_num);
          return true;
        }
        if (menu_) MenuDialLeft();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF):
        if (menu_) MenuDialRight();
        return true;
 
      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF):
        if (menu_) MenuDialLeft();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
#ifdef COLORWHEEL_ZOOM
        if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_SMOOTH) {
          SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_ZOOMED);
          PlayMenuSound("mzoom.wav");
          return true;
        } 
#endif
        return false;

      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ON):
#ifndef DISABLE_COLOR_CHANGE
        if (color_mode_ == CC_COLOR_LIST) {
          color_mode_ = NONE;
          hybrid_font.PlayCommon(&SFX_ccend);
          return true;
        }
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          ToggleColorChangeMode();
#ifdef FETT263_EDIT_MODE_MENU
          if (menu_type_ == MENU_COLOR) {
            menu_type_ = MENU_TOP;
            MenuSave();
          }
#endif
          return true;
        }
#endif
        return false;

      case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_ON):
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          PlayMenuSound("rehrsend.wav");
          clash_count_ += 1;
          saved_choreography.clash_rec[clash_count_].stance = SavedRehearsal::STANCE_END;
          next_event_ = true;
          clash_count_ = 0;
          if (SFX_clsh) {
            SFX_clsh.Select(-1);
          } else {
            SFX_clash.Select(-1);
          }
          if (SFX_lock) {
            SFX_lock.Select(-1);
          } else {
            SFX_lockup.Select(-1);
          }
          SFX_bgnlock.Select(-1);
          SFX_endlock.Select(-1);
          return true;
        } else {
          return false;
        }
#endif
        return true;        

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_OFF, MODE_ON):
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (choreo_) {
          if (saved_choreography.clash_rec[clash_count_].stance != SavedRehearsal::STANCE_END) return true;
        }
#endif        
        if (menu_) {
          MenuChoice();
          return true;
        } else {
          if (!swinging_) {
            check_blast_ = false;
            swing_blast_ = false;
#ifdef FETT263_BM_DISABLE_OFF_BUTTON
            if (!battle_mode_ && !saved_gesture_control.powerlock) {
              Off();
              saber_off_time_ = millis();
            }
#else
            if (!saved_gesture_control.powerlock) {
            Off();
            saber_off_time_ = millis();
            battle_mode_ = false;
            }
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
            if (choreo_ && saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_END) {
              choreo_ = false;
              if (SFX_clsh) {
                SFX_clsh.Select(-1);
              } else {
                SFX_clash.Select(-1);
              }
              if (SFX_lock) {
                SFX_lock.Select(-1);
              } else {
                SFX_lockup.Select(-1);
              }
              SFX_bgnlock.Select(-1);
              SFX_endlock.Select(-1);
            }
#endif        
#ifdef FETT263_BATTLE_MODE_ALWAYS_ON
            battle_mode_ = true;
#endif
          }
          return true;
        }

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_DOUBLE_CLICK, MODE_ON):
        if (menu_) {
          MenuUndo();
          return true;
        } else {
          // Avoid the base and the very tip.
          // TODO: Make blast only appear on one blade!
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            SaberBase::SetVariation(0);
            ToggleColorChangeMode();
            return true;
          }
          if (swing_blast_) {
            check_blast_ = false;
            swing_blast_ = false;
            if (SFX_blstend) {
              hybrid_font.PlayCommon(&SFX_blstend);
            } else {
              SaberBase::DoBlast();
            }
            return true;
          } else {
            SaberBase::DoBlast();
            check_blast_ = true;
            last_blast_ = millis();
          }
          return true;
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
#ifdef FETT263_EDIT_MODE_MENU
        if (menu_) {
          if (color_mode_ == COLOR_LIST || color_mode_ == EDIT_COLOR) {
            if (color_mode_ == COLOR_LIST) {
              hsl_ = Color16(color_list_[dial_]).toHSL();
            }
            color_mode_ = ZOOM_COLOR;
            hsl_angle_ = fusor.angle2();
            PlayMenuSound("mzoom.wav");
            return true;
          }
        }
#endif        
        if (fusor.angle1() > M_PI / 3) {
          track_mode_ = PLAYBACK_OFF;
          StartOrStopTrack();
          return true;
        } else {
          if (SFX_quote) { 
            if (fusor.angle1() > - M_PI / 3 && fusor.angle1() < M_PI / 3)  {
              if (force_quote_) {
                PlayQuote();
                return true;
              } else {
                SaberBase::DoForce();
                return true;
              }
            } else {
              if (!force_quote_) {
                force_quote_ = true;
                PlayQuote();
                return true;
              }
              if (force_quote_) {
                force_quote_ = false;
                SaberBase::DoForce();
                return true;
              }
            }
          } else {
            force_quote_ = false;
            SaberBase::DoForce();
          }
        }
        return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_ON):
        if (menu_) return true;
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
          return true;  
        }
        if (check_blast_ && battle_mode_ || rehearse_) {
          if (!swing_blast_ && millis() - last_blast_ > 2000) {
            swing_blast_ = true;
            hybrid_font.PlayCommon(&SFX_blstbgn);
            SaberBase::DoBlast();
            check_blast_ = false;
            return true;
          }
        }        
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
        if (fusor.angle1() > M_PI / 3) {
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
        } else {
#ifndef DISABLE_COLOR_CHANGE
          bool uses_rgb_arg;
          #define USES_RGB_ARG(N) \
          uses_rgb_arg |= style_parser.UsesArgument(current_preset_.GetStyle(N), 3);
          ONCEPERBLADE(USES_RGB_ARG)
          if (!uses_rgb_arg) {
            ToggleColorChangeMode();
          } else {  
            bool handles_color_change;
          #define USES_COLOR_CHANGE(N) \
            handles_color_change |= current_config->blade##N->current_style() && current_config->blade##N->current_style()->IsHandled(HANDLED_FEATURE_CHANGE_TICKED);
            ONCEPERBLADE(USES_COLOR_CHANGE)
            if (!handles_color_change) {
              current_preset_.Save();
              color_mode_ = CC_COLOR_LIST;
              current_menu_angle_ = fusor.angle2();
              dial_ = -1;
              hybrid_font.PlayCommon(&SFX_ccbegin);
            } else {
              ToggleColorChangeMode();
            }
          } 
#endif
#ifdef DISABLE_COLOR_CHANGE
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
#endif
        }
        return true;

        // Lockup
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        if (menu_) return true;
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          clash_count_ += 1;
          saved_choreography.clash_rec[clash_count_].stance = SavedRehearsal::STANCE_LOCKUP;
          int file_num1;
          int file_num2;
          int file_num3;
          file_num1 = rand() % SFX_bgnlock.files_found();
          SFX_bgnlock.Select(file_num1);
          saved_choreography.clash_rec[clash_count_].sound_number1 = file_num1;
          if (SFX_lock) {
            file_num2 = rand() % SFX_lock.files_found();
            SFX_lock.Select(file_num2);
            saved_choreography.clash_rec[clash_count_].sound_number2 = file_num2;
          } else {
            file_num2 = rand() % SFX_lockup.files_found();
            SFX_lockup.Select(file_num2);
            saved_choreography.clash_rec[clash_count_].sound_number2 = file_num2;
          }
          file_num3 = rand() % SFX_endlock.files_found();
          SFX_endlock.Select(file_num3);
          saved_choreography.clash_rec[clash_count_].sound_number3 = file_num3;
        }
#endif
        if (!SaberBase::Lockup()) {
          if (fusor.angle1() < - M_PI / 8) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
            SaberBase::DoBeginLockup();
          } else {
#ifdef FETT263_CLASH_STRENGTH_SOUND
            clash_impact_millis_ = millis();
            clash_type_ = CLASH_LOCKUP;
#else            
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
            SaberBase::DoBeginLockup();
#endif
          }
          check_blast_ = false;
          swing_blast_ = false;
          return true;
        }
        return true;
        break;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
        check_blast_ = false;
        swing_blast_ = false;
        SaberBase::DoBeginLockup();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
        if (menu_) return true;
        if (!SaberBase::Lockup()) {
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
          check_blast_ = false;
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        return true;
        break;

        // Off functions
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
        track_mode_ = PLAYBACK_OFF;
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
          return true;
        } else {
          if (fusor.angle1() > M_PI / 3) {
            StartOrStopTrack();
            return true;
          } else {
            track_num_ = -1;
            num_tracks_ = RunCommandAndGetSingleLine("list_current_tracks", nullptr, 0, 0, 0);
            PlayMenuSound("mselect.wav");
            current_menu_angle_ = fusor.angle2();
            menu_ = true;
            menu_type_ = TRACK_PLAYER;
            StartOrStopTrack();
            return true;
          }
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        SaberBase::RequestMotion();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
#ifdef FETT263_SAY_BATTERY
        SayNumber(battery_monitor.battery(), SAY_BATTERY);
#endif
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

      // Gesture Sleep Toggle
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF | BUTTON_POWER):
        if (!saved_gesture_control.gestureon) {
          saved_gesture_control.gestureon = true;
          PlayMenuSound("mgeston.wav");
        } else {
          saved_gesture_control.gestureon = false;
          PlayMenuSound("mgestoff.wav");
        }
        return true;

#ifdef FETT263_SAVE_CHOREOGRAPHY
      // Rehearsal Mode
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF | BUTTON_AUX):
      if (saved_choreography.clash_rec[0].stance == SavedRehearsal::STANCE_CLASH || saved_choreography.clash_rec[0].stance == SavedRehearsal::STANCE_LOCKUP) {
        PlayMenuSound("rehrsnew.wav");
        menu_ = true;
        menu_type_ = MENU_REHEARSE;
        return true;
      } else {
        memset(saved_choreography.clash_rec, 0, sizeof(saved_choreography.clash_rec));
        clash_count_ = -1;
        rehearse_ = true;
        PlayMenuSound("rehrsbgn.wav");
        STDOUT.println("Enter Rehearsal Mode");
        FastOn();
      }
      return true;

      // Choreographed Battle Mode
      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF):
        if (saved_choreography.clash_rec[0].stance == SavedRehearsal::STANCE_CLASH || saved_choreography.clash_rec[0].stance == SavedRehearsal::STANCE_LOCKUP) {
          PlayMenuSound("chreobgn.wav");
          choreo_ = true;
          battle_mode_ = true;
          clash_count_ = 0;
          STDOUT.println("Entering Choreography Mode");
          FastOn();
          return true;
        } else {
          PlayMenuSound("nochreo.wav");
          rehearse_ = true;
          clash_count_ = -1;
          next_event_ = true;
        }
        return true;
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_AUX):
        if (saved_choreography.clash_rec[0].stance == SavedRehearsal::STANCE_CLASH || saved_choreography.clash_rec[0].stance == SavedRehearsal::STANCE_LOCKUP) {
          PlayMenuSound("chreobgn.wav");
          choreo_ = true;
          battle_mode_ = true;
          clash_count_ = 0;
          STDOUT.println("Entering Choreography Mode");
          FastOn();
          return true;
        } else {
          PlayMenuSound("nochreo.wav");
          rehearse_ = true;
          clash_count_ = -1;
          next_event_ = true;
        }
        return true;
#endif

      // Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        if (menu_ || SaberBase::Lockup()) return true;
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          clash_count_ += 1;
          saved_choreography.clash_rec[clash_count_].stance = SavedRehearsal::STANCE_CLASH;
          int file_num1;
          if (SFX_clsh) {
            file_num1 = rand() % SFX_clsh.files_found();
            SFX_clsh.Select(file_num1);
            saved_choreography.clash_rec[clash_count_].sound_number1 = file_num1;
          } else {
            file_num1 = rand() % SFX_clash.files_found();
            SFX_clash.Select(file_num1);
            saved_choreography.clash_rec[clash_count_].sound_number1 = file_num1;
          }
          SaberBase::DoClash();
          return true;
        }
#endif
        if (!battle_mode_ || swinging_) {
          clash_impact_millis_ = millis();
#ifdef FETT263_CLASH_STRENGTH_SOUND
          clash_type_ = CLASH_NORMAL;
#else
          SaberBase::DoClash();
#endif
          return true;
        }
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (choreo_) {
          if (saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_CLASH) {
            if (SFX_clsh) {
              SFX_clsh.Select(saved_choreography.clash_rec[clash_count_].sound_number1);
            } else {
              SFX_clash.Select(saved_choreography.clash_rec[clash_count_].sound_number1);
            }
            check_blast_ = false;
            swing_blast_ = false;
            clash_count_ += 1;
            SaberBase::DoClash();
            return true;
          } else {
            SFX_bgnlock.Select(saved_choreography.clash_rec[clash_count_].sound_number1);
            if (SFX_lock) {
              SFX_lock.Select(saved_choreography.clash_rec[clash_count_].sound_number2);
            } else {
              SFX_lockup.Select(saved_choreography.clash_rec[clash_count_].sound_number2);
            }
            SFX_endlock.Select(saved_choreography.clash_rec[clash_count_].sound_number3);
            clash_count_ += 1;
          }
        }
#endif
        clash_impact_millis_ = millis();
        check_blast_ = false;
        swing_blast_ = false;
        if (fusor.angle1() < - ((M_PI / 2) - 0.25)) {
          SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          SaberBase::DoBeginLockup();
          auto_melt_on_ = true;
          return true;
        }
        clash_type_ = CLASH_BATTLE_MODE;
        return true;


      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
        if (menu_) return true;
        clash_impact_millis_ = millis();
        if (!battle_mode_) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
          clash_impact_millis_ = millis();
          clash_type_ = CLASH_STAB;
#else
          SaberBase::DoStab();
#endif   
          return true;       
        }
        check_blast_ = false;
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

#ifdef FETT263_SWING_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.swingon) return true;
        // Due to motion chip startup on boot creating false ignition we delay Swing On at boot for 3000ms
        if (!menu_ && millis() > 3000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
          if (SFX_preon) {
            On();
          } else {
            FastOn();
          }
#ifndef FETT263_SWING_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.swingon) return true;
        // Due to motion chip startup on boot creating false ignition we delay Swing On at boot for 3000ms
        if (!menu_ && millis() > 3000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
          FastOn();
#ifndef FETT263_SWING_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
        if (!saved_gesture_control.twistoff) return true;
        if (menu_) return true;
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 3000) {
#ifdef FETT263_SAVE_CHOREOGRAPHY
          if (choreo_) {
            if (saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_END) {
            if (SFX_clsh) {
              SFX_clsh.Select(-1);
            } else {
              SFX_clash.Select(-1);
            }
            if (SFX_lock) {
              SFX_lock.Select(-1);
            } else {
              SFX_lockup.Select(-1);
            }
            SFX_bgnlock.Select(-1);
            SFX_endlock.Select(-1);
            choreo_ = false;
            } else {
              return true;
            }
          }
#endif
          Off();
          last_twist_ = millis();
          saber_off_time_ = millis();
#ifndef FETT263_BATTLE_MODE_ALWAYS_ON
          battle_mode_ = false;
#endif
        }
        return true;

#ifdef FETT263_TWIST_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.twiston) return true;
        // Delay twist events to prevent false trigger from over twisting
        if (!menu_ && millis() - last_twist_ > 2000 &&
            millis() - saber_off_time_ > 2000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
          if (SFX_preon) {
            On();
          } else {
            FastOn();
          }

#ifndef FETT263_TWIST_ON_NO_BM
          battle_mode_ = true;
#endif
          last_twist_ = millis();
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.twiston) return true;
        // Delay twist events to prevent false trigger from over twisting
        if (!menu_ && millis() - last_twist_ > 2000 &&
            millis() - saber_off_time_ > 2000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif        
          FastOn();
#ifndef FETT263_TWIST_ON_NO_BM
          battle_mode_ = true;
#endif
          last_twist_ = millis();
        }
        return true;
#endif

#ifdef FETT263_STAB_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.stabon) return true;
        if (!menu_ && millis() - saber_off_time_ > 2000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif        
          if (SFX_preon) {
            On();
          } else {
            FastOn();
          }
#ifndef FETT263_STAB_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.stabon) return true;
        if (!menu_ && millis() - saber_off_time_ > 2000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif        
          FastOn();
#ifndef FETT263_STAB_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_THRUST_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.thruston) return true;
        if (!menu_ && millis() - saber_off_time_ > 2000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif        
          if (SFX_preon) {
            On();
          } else {
            FastOn();
          }
#ifndef FETT263_THRUST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.thruston) return true;
        if (!menu_ && millis() - saber_off_time_ > 2000) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif        
          FastOn();
#ifndef FETT263_THRUST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
        if (!saved_gesture_control.forcepush) return true;
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

#ifdef FETT263_MULTI_PHASE
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_AUX):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000) {
          last_twist_ = millis();
          Off();
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif        
          FastOn();
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000) {
          last_twist_ = millis();
          Off();
          previous_preset();
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif        
          FastOn();
        }
        return true;
#endif

      // Events that needs to be handled regardless of what other buttons
      // are pressed.
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        if (SaberBase::Lockup()) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
          clash_type_ = CLASH_LOCKUP_END;
          RealClashSound();
#else
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
#endif
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
#ifndef FETT263_SAY_BATTERY
      case EFFECT_BATTERY_LEVEL:
        if (SFX_battery) {
          hybrid_font.PlayCommon(&SFX_battery);
        } else {
          beeper.Beep(0.5, 3000);
        }
        return;
#endif
      case EFFECT_FAST_ON:
        if (SFX_faston) {
          hybrid_font.PlayCommon(&SFX_faston);
        }
        return;
      default:
        break; // avoid compiler warnings
    }
  }

private:
  bool swing_blast_ = false; // Multi-Blast Mode (Swing = Blast Deflect)
  bool check_blast_ = false; // Check if Multi-Blast mode should be enabled (Battle Mode)
  bool auto_lockup_on_ = false; // Battle Mode Lockup active
  bool auto_melt_on_ = false; // Battle Mode Melt/Drag active
  bool battle_mode_ = false; // Battle Mode active
  bool menu_ = false; // Edit Mode / Menu System active
  bool force_quote_ = false; // Quote Player active (in place of force effect)
  bool rehearse_ = false; // Rehearsal Mode active
  bool choreo_ = false; // Choreography Mode active
  uint32_t thrust_begin_millis_;
  uint32_t push_begin_millis_;
  uint32_t clash_impact_millis_;
  uint32_t last_twist_;
  uint32_t last_push_;
  uint32_t last_blast_;
  uint32_t saber_off_time_;
  uint32_t last_rotate_;
  ClashType clash_type_ = CLASH_NONE;
  MenuType menu_type_ = MENU_TOP;
  int menu_top_pos_ = 0;
  int menu_sub_pos_ = 0;
  TrackMode track_mode_ = PLAYBACK_OFF;
  int track_num_;
  int num_tracks_;
  int ignite_time_;
  int dial_ = -1;
  int sub_dial_;
  float change_ = 0.0;
  float twist_menu_ = M_PI / 4;
#ifdef FETT263_EDIT_MODE_MENU
  bool choice_ = false; // Edit Mode selection confirmed
  bool next_event_ = false; // Do next event in Edit Mode
  bool off_event_ = false; // Do off event in Edit Mode
  bool restart_ = false; // Ignite blade after off event in Edit Mode
  bool edit_color_ = false; // Color Editing Mode active
  uint32_t color_revert_;
  float change_h_ = 0.0;
  float hsl_angle_ = 0.0;
  int blade_number_ = 0;
  int preset_count_;
  int style_;
  int blade_preview_ = 0;
  Color16 saved_color_;
  EditColorMode color_mode_;
  int style_num_;
  int font_num_;
  int num_fonts_;
  int num_presets_;
  int blade_num_;
  int copy_blade_;
  int effect_num_;
  int set_num_;
  int style_revert_;
  int length_revert_;
  int arg_revert_;
  int calc_;
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
  int clash_count_ = -1; // Choreography Mode Clash counter
#endif
};

#endif
