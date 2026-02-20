/* For Sabersense Prop File v7/8-301 onwards.
============================================================
==============   SABERSENSE BASE CONFIG FILE   =============
=================            by            =================
=================       CHRIS CARTER       =================
============================================================

http://fredrik.hubbe.net/lightsaber/proffieos.html
Copyright (c) 2016-2019 Fredrik Hubinette

Sabersense(TM) Base Config File for ProffieOS.
https://www.sabersense.co.uk

Config file Copyright (c) 2024-2026 Chris Carter (Sabersense)
Built by Chris Carter with substantial support and
contributions from Fredrik Hubinette and Brian Conner.

Distributed under the terms of the
GNU General Public License v3.

==========================================================
========================== INDEX =========================

TUTORIALS
  Section 1  -  How to Use this Config            Line  38
  Section 2  -  One Button User Controls          Line  83
  Section 3  -  Two Button User Controls          Line 143
  Section 4  -  Advanced Technical Notes          Line 205
  Section 5  -  Defines Descriptions              Line 252

THE CODE
  Section 6  -  THE MAIN CONFIG                   Line 411
  Section 7  -  Config Top                        Line 727
  Section 8  -  Blade Style Elements              Line 840
  Section 9  -  The Blade Styles                  Line 873

==========================================================
================= HOW TO USE THIS CONFIG =================

This file is intended to provide a base config to set all
necessary defines for your system and then to facilitate
the simple building of blade presets for your chosen
sound fonts.

The only parts that are intended to be edited are the
CONFIG_TOP section and the Blade Styles section.

Before each define in the CONFIG_TOP section, you will notice
two slashes, like this:
//  #define SABERSENSE_BLADE_ID

The slashes tell the system to ignore that line.
Therefore if you wish to use a given define, simply delete
the slashes, like this:
#define SABERSENSE_BLADE_ID

For the blade presets, the system relies on shortcuts to
pre-built styles. A complete preset will look something
like this:

   { "MyFont;Menus", "tracks/MyMusicTrack.wav",
//  Main Blade:
StylePtr<StandardFlicker>("~ ~ 6"),
//  Crystal Chamber:
StylePtr<Crystal>("~ ~ 6"),
"myfont"},

<StandardFlicker> refers to the pre-built style used.
The full range of available styles can be seen on line 845.

The number in the brackets at the end sets the default colour
based on the table shown on line 855. So in the above
example, the default colour is set to red. Note that the user can
change blade colours themselves just using the buttons after
the saber has been programmed.

Note also that we recommend adding a Menus folder as this will
aid with feature navigation when using the saber. A free
download of a Menus folder compatible with this config is
available from the Sabersense website here:
https://sabersense.square.site/downloads

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
  Run BladeID/Array Select  Fast triple-click while OFF. (Applicable installs only).
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
  *** = Can be used for Kestis style Colour Change feature via define. In which case
        Multi-blast is accessed by four clicks of POWER with blade on.

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
  Run BladeID/Array Select  Fast triple-click. (Applicable installs only).
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
  *** = Can be used for Kestis style Colour Change feature via define. In which case
        Multi-blast is accessed by four clicks of POWER with blade on.
        
============================================================
================= ADVANCED TECHNICAL NOTES =================

1) SAVE FILES
For proper function when using BLADE_ID or ARRAY_SELECTOR with SAVE_STATE, allocate 
individual save folders per blade array. This will save separate settings per array.
If you want common saved settings, you must use BLADE_DETECT or ARRAY_SELECTOR without the
save folders but with the same number of presets per array. (Single preset arrays such as 
charging mode are still permitted).

2) CLASH CONTROLS
//  To revert to older 0 to 4 scale, insert the relevant defines below.
//  #define AUDIO_CLASH_SUPPRESSION_LEVEL 10	// Disable if using next three defines
//  #define PROFFIEOS_DONT_USE_GYRO_FOR_CLASH	// Experimental define reverts to OS 5.x clash algorithm
                                                // Fixes false clashes from buttons or motors close to board.
//  #define PROFFIEOS_MOTION_FREQUENCY 800		// Experimental define reverts to OS 5.x clash algorithm
//  #define PROFFIEOS_ACCELEROMETER_RANGE 4		// Experimental define reverts to OS 5.x clash algorithm
												// Can be set to 2, 4, 8 or 16.
For more details on false clash prevention and adjustments, visit:
https://crucible.hubbe.net/t/false-clashes-with-os7-7-still/3988/22

If button clicking is causing false clashes due to close proximity
of switch to the Proffieboard, visit:
https://crucible.hubbe.net/t/false-clash-and-thrust-when-clicking-power-button/4136/13

For a detailed description of how clashes are detected and triggered, 
and how the above settings actually work, visit:
https://crucible.hubbe.net/t/clash-settings-and-questions/4651/2

For more information on various historic clash settings, visit:
https://pod.hubbe.net/explainers/history-of-clash.html

3) BLADE DETECT
When using BLADE_DETECT with SABERSENSE_ARRAY_SELECTOR, the NO_BLADE array will be
skipped by Array Selector and only be accessed by the Blade Detect process.

4)
Further information on Proffieboards v2.2 and v3.9 can be found here:

Proffieboard v2:
https://drive.google.com/file/d/1vn9vRk-CNZSUHL4xm_hHwS6UgkfKXdO2/view?fbclid
IwY2xjawHI869leHRuA2FlbQIxMQABHTTieWfK0DRZZ1FWWvfSYSSwpfd1cDj4UKqH39EPE7Sd_
DQGtQX430zP3Q_aem_1U2VWjNEYfR981X2fEG-hA

Proffieboard v3:
https://fredrik.hubbe.net/lightsaber/v6/

===========================================================
================== DEFINES DESCRIPTIONS ===================

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
  Note that this feature requires the following system define
  to also be active to enable the scans to work correctly:
  #define ENABLE_POWER_FOR_ID PowerPINS<bladePowerPin2, bladePowerPin3>

#define SABERSENSE_ARRAY_SELECTOR
  Replaces regular BladeID and allows cycling between
  different blade/preset arrays manually, regardless
  of actual BladeID status. Plays array-specific
  arrayX.wav files when switching.
  Requires arrays to be numbered consecutively,
  starting at zero, in the field that would otherwise
  contain BladeID values. Like this:
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
  in your config, but only one NO_BLADE array is permitted.
  Note also that NO_BLADE replaces the zero array,
  meaning that Blade-In array numbering must be consecutive
  starting at 1. Like this:
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

#define SABERSENSE_HOT_SKIP_DOWN 0
#define SABERSENSE_HOT_SKIP_LEVEL 0
  Hot Skipping is distinct from Font Skipping in that
  it skips directly to a given preset, rather than
  skipping forwards or backwards x number of presets.
  These defines override the Skip-to-Last/Middle
  Preset feature, so system skips to user-defined
  preset. Note that Skip-to-First preset is fixed
  and not user-definable.

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

#define SABERSENSE_EFFECT_USER1_ALT_FONT
  This define utilises ProffieOS's EFFECT_USER1
  feature in blade styles to allow effects like
  the swing colour change seen in the Cal Kestis
  Fallen Order video game.
  It also allows the Alt Font feature to change
  certain sounds when combined with a suitable
  blade style and font folder layout.
  Further details and specific blade styles can
  be found on the Sabersense website here:
  https://sabersense.square.site/downloads

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
*/
//  *************************************************************************
//  *************************************************************************
//  ************************     THE MAIN CONFIG     ************************
//  *************************************************************************
//  *************************************************************************

#ifdef CONFIG_STYLES
//  FULL LENGTH CLASH AND LOCKUP WITH STANDARD MAIN BLADE FLICKER.
using StandardFlicker = Layers<
  Layers<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,107,205>>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,
      Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,
      Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>,
    OriginalBlastL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Red>,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<200,255,255>>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,White,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,White>,White,White,White,White,White,White,White,White,
        White,White,Yellow>,Black,20,16>>,
    LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,
      SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
    SimpleClashL<Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Yellow>,Yellow,Yellow,Yellow,White,Red,Yellow,Yellow,Yellow,
      White,White,Yellow>,Black,28,25>,260>,
    InOutTrL<TrWipe<500>,TrWipeInX<WavLen<EFFECT_RETRACTION>>>,
    TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>;

//  FULL LENGTH CLASH AND LOCKUP WITH HEAVY FLICKER.
using HeavyFlicker = Layers<
  Layers<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,107,205>>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,
      Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,
      Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,68,125>>,Rgb<0,0,125>,Rgb<0,125,125>,Rgb<0,35,125>,
      Rgb<0,125,0>,Rgb<14,125,14>,Rgb<125,0,0>,Rgb<125,0,125>,Rgb<125,40,77>,
      Rgb<125,125,0>,Rgb<125,48,0>,Rgb<110,125,125>>>>,
    OriginalBlastL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Red>,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<200,255,255>>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,White,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,White>,White,White,White,White,White,White,White,White,
        White,White,Yellow>,Black,20,16>>,
    LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,
      SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
    SimpleClashL<Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Yellow>,Yellow,Yellow,Yellow,White,Red,Yellow,Yellow,Yellow,
      White,White,Yellow>,Black,28,25>,260>,
    InOutTrL<TrWipe<500>,TrWipeInX<WavLen<EFFECT_RETRACTION>>>,
    TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>;

//  FULL LENGTH CLASH AND LOCKUP WITH REDUCED MAIN BLADE FLICKER AND SLOW IGNITION POINTING DOWN.
using SlowIgnition = Layers<
  Layers<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,107,205>>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,
      Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,
      Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,68,125>>,Rgb<0,0,125>,Rgb<0,125,125>,Rgb<0,35,125>,
      Rgb<0,125,0>,Rgb<14,125,14>,Rgb<125,0,0>,Rgb<125,0,125>,Rgb<125,40,77>,
      Rgb<125,125,0>,Rgb<125,48,0>,Rgb<110,125,125>>>>,
    OriginalBlastL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Red>,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<200,255,255>>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,White,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,White>,White,White,White,White,White,White,White,White,
        White,White,Yellow>,Black,20,16>>,
    LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,
      SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
    SimpleClashL<Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Yellow>,Yellow,Yellow,Yellow,White,Red,Yellow,Yellow,Yellow,
      White,White,Yellow>,Black,28,25>,260>,
    InOutTrL<TrWipeX<Scale<IsGreaterThan<BladeAngle<>,Int<16100>>,Int<1500>,Int<300>>>,
      TrWipeInX<WavLen<EFFECT_RETRACTION>>>,
    TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>;

//  LOCALIZED LOCKUP.
using LocalizedLockup = Layers<
  Layers<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,107,205>>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,
      Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,
      Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,68,125>>,Rgb<0,0,125>,Rgb<0,125,125>,Rgb<0,35,125>,
      Rgb<0,125,0>,Rgb<14,125,14>,Rgb<125,0,0>,Rgb<125,0,125>,Rgb<125,40,77>,
      Rgb<125,125,0>,Rgb<125,48,0>,Rgb<110,125,125>>>>,
    OriginalBlastL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Red>,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<200,255,255>>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,White,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,White>,White,White,White,White,White,White,White,White,
        White,White,Yellow>,Black,20,16>,
      Bump<Int<16800>,Int<28000>>>,
    LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,
      SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
    SimpleClashL<Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Yellow>,Yellow,Yellow,Yellow,White,Red,Yellow,Yellow,Yellow,
      White,White,Yellow>,Black,28,25>,260>,
    InOutTrL<TrWipe<500>,TrWipeInX<WavLen<EFFECT_RETRACTION>>>,
    TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>;

//  RAIN BLADE.
using RainBlade = Layers<
  Layers<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,107,205>>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,
      Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,
      Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>,
    SparkleL<White,90>,
    OriginalBlastL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Red>,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<200,255,255>>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,White,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,White>,White,White,White,White,White,White,White,White,
        White,White,Yellow>,Black,20,16>>,
    LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,
      SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
    SimpleClashL<Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Yellow>,Yellow,Yellow,Yellow,White,Red,Yellow,Yellow,Yellow,
      White,White,Yellow>,Black,28,25>,260>,
    InOutTrL<TrWipe<500>,TrWipeInX<WavLen<EFFECT_RETRACTION>>>,
    TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>;

//  SPARKTIP IGNITION.
using SparkTip = Layers<
  Layers<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,107,205>>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,
      Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,
      Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>,
    OriginalBlastL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Red>,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<200,255,255>>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,White,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,White>,White,White,White,White,White,White,White,White,
        White,White,Yellow>,Black,20,16>>,
    LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,
      SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
    SimpleClashL<Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Yellow>,Yellow,Yellow,Yellow,White,Red,Yellow,Yellow,Yellow,
      White,White,Yellow>,Black,28,25>,260>,
    InOutTrL<TrWipeSparkTip<White,500>,TrWipeInX<WavLen<EFFECT_RETRACTION>>>,
    TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>;

//  UNSTABLE.
using Unstable = Layers<
  StripesX<Int<3000>,Int<-3500>,
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomPerLEDFlicker<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,33,63>,Rgb<0,0,63>,Rgb<0,63,63>,Rgb<0,18,63>,Rgb<0,63,0>,
      Rgb<7,63,7>,Rgb<63,0,0>,Rgb<63,0,63>,Rgb<63,19,38>,Rgb<63,63,0>,Rgb<63,25,0>,
      Rgb<56,63,63>>>,Black>,
  BrownNoiseFlicker<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,19,36>>,Rgb<0,0,36>,Rgb<0,36,36>,Rgb<0,10,36>,
      Rgb<0,36,0>,Rgb<4,36,4>,Rgb<36,0,0>,Rgb<36,0,36>,Rgb<36,11,22>,Rgb<36,36,0>,
      Rgb<36,14,0>,Rgb<32,36,36>>,200>,
  RandomPerLEDFlicker<
    ColorChange<TrInstant,Rgb<80,0,0>,Rgb<80,0,80>,Rgb<80,80,60>,Rgb<80,80,0>,
      Rgb<80,32,0>,Rgb<71,80,80>,Rgb<0,45,80>,Rgb<0,0,80>,Rgb<0,80,80>,Rgb<1,24,80>,
      Rgb<0,80,0>,Rgb<9,80,9>>,
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,19,36>>,Rgb<0,0,36>,Rgb<0,36,36>,Rgb<0,10,36>,
      Rgb<0,36,0>,Rgb<4,36,4>,Rgb<36,0,0>,Rgb<36,0,36>,Rgb<36,11,22>,Rgb<36,36,0>,
      Rgb<36,14,0>,Rgb<32,36,36>>>>,
  TransitionLoopL<TrConcat<
    TrWaveX<AudioFlicker<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    BrownNoiseFlicker<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,67,126>,Rgb<0,0,126>,Rgb<0,126,126>,Rgb<0,35,126>,
      Rgb<0,126,0>,Rgb<14,126,14>,Rgb<126,0,0>,Rgb<126,0,126>,Rgb<126,39,77>,
      Rgb<126,126,0>,Rgb<126,49,0>,Rgb<112,126,126>>>,Black,200>>,
    Int<400>,Int<100>,Int<200>,Int<0>>,
    AlphaL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,Int<0>>,
    TrDelayX<Scale<SlowNoise<Int<1000>>,Int<100>,Int<1000>>>>>,
  TransitionLoopL<TrConcat<
    TrWaveX<AudioFlicker<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    BrownNoiseFlicker<Black,ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,33,63>,Rgb<0,0,63>,Rgb<0,63,63>,Rgb<0,18,63>,Rgb<0,63,0>,
      Rgb<7,63,7>,Rgb<63,0,0>,Rgb<63,0,63>,Rgb<63,19,38>,Rgb<63,63,0>,Rgb<63,25,0>,
      Rgb<56,63,63>>>,300>>,
    Int<400>,Int<100>,Int<200>,Int<0>>,
    AlphaL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,Int<0>>,
    TrDelayX<Scale<SlowNoise<Int<1000>>,Int<200>,Int<1500>>>>>,
  TransitionEffectL<TrConcat<TrInstant,BrownNoiseFlickerL<AlphaL<White,Int<16000>>,Int<50>>,
    TrSmoothFade<600>>,EFFECT_LOCKUP_END>,
  ResponsiveLockupL<Strobe<White,BrownNoiseFlicker<White,Blue,300>,50,1>,
    TrConcat<TrInstant,White,TrFade<200>>,TrFade<400>,
    Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,
    Scale<SwingSpeed<100>,Int<10000>,Int<14000>>>,
  ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,
    TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,
    TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
  ResponsiveStabL<Orange>,
  ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
  ResponsiveClashL<White,TrInstant,TrFade<400>>,
  ResponsiveDragL<BrownNoiseFlickerL<White,Int<300>>,TrWipeIn<400>,TrFade<400>>,
  ResponsiveMeltL<Mix<TwistAngle<>,OrangeRed,Orange>,TrWipeIn<600>,TrSmoothFade<600>>,
  InOutTrL<
    TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,
      BendTimePowInvX<IgnitionTime<600>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>,
      Int<500>>,
    TrWipeInX<BendTimePowX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,
      Int<98304>>>>>,
  TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
    SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
    SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>;

//  Crystal Chamber, pulses with blade off.
using Crystal = Layers<
  Layers<
    ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,DeepSkyBlue>,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,
      Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,107,205>>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,
      Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,
      Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>,
    RandomL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Rgb<0,68,125>>,Rgb<0,0,125>,Rgb<0,125,125>,Rgb<0,35,125>,
      Rgb<0,125,0>,Rgb<14,125,14>,Rgb<125,0,0>,Rgb<125,0,125>,Rgb<125,40,77>,
      Rgb<125,125,0>,Rgb<125,48,0>,Rgb<110,125,125>>>>,
    OriginalBlastL<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Red>,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<200,255,255>>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,White,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,
        Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,
      Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,White>,White,White,White,White,White,White,White,White,
        White,White,Yellow>,Black,20,16>>,
    LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,
      SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
    SimpleClashL<Strobe<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
      ColorChange<TrInstant,Yellow>,Yellow,Yellow,Yellow,White,Red,Yellow,Yellow,Yellow,
      White,White,Yellow>,Black,28,25>,260>,
    InOutTrL<TrWipe<500>,TrWipeInX<WavLen<EFFECT_RETRACTION>>,
      Pulsing<ColorSelect<Sum<Variation,IntArg<STYLE_OPTION_ARG,0>>,TrFade<50>,
        ColorChange<TrInstant,Rgb<0,68,125>>,Rgb<0,0,125>,Rgb<0,125,125>,Rgb<0,35,125>,
        Rgb<0,125,0>,Rgb<14,125,14>,Rgb<125,0,0>,Rgb<125,0,125>,Rgb<125,40,77>,
        Rgb<125,125,0>,Rgb<125,48,0>,Rgb<110,125,125>>,Black,3200>>,
    TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,
      SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>;
#endif

//  ===========================================================
//  ======================= CONFIG TOP ========================

#ifdef CONFIG_TOP
#include "proffieboard_v3_config.h"
#define NUM_BLADES  2
#define NUM_BUTTONS 2
const unsigned int maxLedsPerStrip = 144;
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD

//  VOLUME DEFINES
#define VOLUME 1700      //  Maximum volume that can be reached.
#define BOOT_VOLUME 1400 //  Default volume at bootup.
                         //  Over-ridden once user adjusts volume
                         //  due to SAVE_STATE.
//  OPTIONAL SYSTEM DEFINES
#define NO_REPEAT_RANDOM
#define FEMALE_TALKIE_VOICE
#define KILL_OLD_PLAYERS
#define IDLE_OFF_TIME 15 * 60 * 1000
//     Minutes multiplied by seconds multiplied by milliseconds. (15 * 60 * 1000 = 15 minutes).

//  SAVE SETTINGS - See Technical Note 1 above.
#define SAVE_STATE
//  #define SAVE_VOLUME
//  #define SAVE_PRESET

//  PRIMARY CLASH CONTROL - See Technical Note 2 above for further clash controls.
#define CLASH_THRESHOLD_G 3.2
//  Range is 0 to 16 by default.

//  OPTIONAL FEATURE DEFINES
#define DYNAMIC_BLADE_LENGTH
#define DYNAMIC_CLASH_THRESHOLD
#define SAVE_CLASH_THRESHOLD

#define FILTER_CUTOFF_FREQUENCY 160
#define FILTER_ORDER 4

//  #define ORIENTATION ORIENTATION_USB_TOWARDS_BLADE
//  #define ORIENTATION_ROTATION 0,-30,0  //  For curved hilt, USB pointed down relative to blade.
//  #define ORIENTATION_ROTATION 0,14,0  //  For angled Proffieboard, USB pointed up relative to blade.

//  #define SHARED_POWER_PINS

//  ARRAY SELECT AND BLADE ID
//  See Technical Note 1 above when using BLADE_ID or ARRAY_SELECTOR.
//  #define SABERSENSE_BLADE_ID   // Plays Array ident sound with BladeID detection.
//  #define ENABLE_POWER_FOR_ID PowerPINS<bladePowerPin2, bladePowerPin3>   //  Required for BLADE_ID.

#define SABERSENSE_ARRAY_SELECTOR   //  Cycles through blade/preset arrays manually, ignores actual BladeID status.
#define SABERSENSE_DEFAULT_BLADE_ARRAY 1 //  Sets the default blade array index.
//  #define SABERSENSE_DISABLE_SAVE_ARRAY  //  Means that arrays won't be saved between reboots.
                                           //  Best with systems with only one array plus charging array.

//  #define SABERSENSE_ENABLE_ARRAY_FONT_IDENT   //  Plays font ident after array ident.
                                                 //  Works with SABERSENSE BLADE_ID & ARRAY_SELECTOR.
                                                 //  used for systems that have different fonts on different arrays.
//  BLADE DETECT
//  #define BLADE_DETECT_PIN aux2Pin  //  Wire to Button 3.
//  #define SABERSENSE_NO_BLADE  //  Required to make Blade Detect work with Array Selector.

//  REQUIRED FOR BLUETOOTH MODULE.
//  #define ENABLE_SERIAL

//  REQUIRED FOR OLED MODULE
//  #define ENABLE_SSD1306
//  #define OLED_FLIP_180	// Optional
//  #define PLI_OFF_TIME 30 * 1000
//     Controls how long battery display is shown on OLED after animations end.
//     Seconds multiplied by milliseconds. (30 * 1000 = 30 seconds).
//  For OLED function, add 'oled' to each preset file list and ensure
//  oled folder is on top level of SD card.

//  MEMORY SAVING OPTIONS
//  #define DISABLE_BASIC_PARSER_STYLES   // Disables default named styles found in Proffie Workbench.
//  #define DISABLE_DIAGNOSTIC_COMMANDS   // Disables diagnostic commands in Serial Monitor.

//  CUSTOM SABERSENSE BUTTON CONTROLS
//  Gesture controls bypass Preon effects unless SABERSENSE_GESTURE_PREON define is added.
#define SABERSENSE_TWIST_ON
#define SABERSENSE_TWIST_OFF
//  #define SABERSENSE_STAB_ON
//  #define SABERSENSE_THRUST_ON
//  #define SABERSENSE_SWING_ON
//  #define SABERSENSE_GESTURE_PREON         //  Includes preon effects with gesture ignitions.

#define BUTTON_DOUBLE_CLICK_TIMEOUT 350 //  Makes button clicks a little slower and easier.
#define BUTTON_SHORT_CLICK_TIMEOUT 350  //  Makes button clicks a little slower and easier.

//  #define SABERSENSE_FONT_SKIP_A 5		 //  Set number of preset skips for two clicks. Default 5.
//  #define SABERSENSE_FONT_SKIP_B 10        //  Set number of preset skips for three clicks. default 10.
//  #define SABERSENSE_HOT_SKIP_DOWN 11      //  Sets preset skip to selected font, hilt pointing down.
//  #define SABERSENSE_HOT_SKIP_LEVEL 7      //  Sets preset skip to selected font, hilt horizontal.
//  #define SABERSENSE_DISABLE_SKIPPING      //  Removes preset skipping feature entirely.

//  #define SABERSENSE_BLAST_MAIN_AND_AUX    //  Makes blast available on MAIN and AUX on 2-button sabers.
//  #define SABERSENSE_FLIP_AUDIO_PLAYERS    //  Flips audio Players so that QUOTE plays when pointing down, not up.
//  #define SABERSENSE_RANDOM_QUOTE          //  Plays quote.wav files randomly instead of sequentially.
//  #define SABERSENSE_BUTTON_CLICKER        //  For Scavenger hilt. Plays click sound files with button pushes.
//  #define SABERSENSE_NO_COLOR_CHANGE       //  Disables Colour Change feature.
//  #define SABERSENSE_NO_LOCKUP_HOLD        //  Reverts to lockup being triggered by holding AUX while clashing.
//  #define SABERSENSE_DISABLE_RESET         //  Prevents "Restore Factory Defaults".
//  #define SABERSENSE_EFFECT_USER1_ALT_FONT //  For custom blade effects with EFFECT_USER1 in the style.
#endif

#ifdef CONFIG_PROP
#include "../props/saber_sabersense_buttons.h"
#endif
/*
============================================================
=================== BLADE STYLE ELEMENTS ===================

This config works by using a simple shortcut to a style,
and then setting the default colour in the end brackets.

The available styles are:
  StylePtr<StandardFlicker>("~ ~ 0"),
  StylePtr<HeavyFlicker>("~ ~ 0"),
  StylePtr<SlowIgnition>("~ ~ 0"), // Ignites slowly pointing downwards.
  StylePtr<LocalizedLockup>("~ ~ 0"),
  StylePtr<RainBlade>("~ ~ 0"),
  StylePtr<SparkTip>("~ ~ 0"),
  StylePtr<Unstable>("~ ~ 0"),
  StylePtr<Crystal>("~ ~ 0"),

The number in the brackets determines the default
blade colour based on the following table:
  DeepSkyBlue  ("~ ~ 0"),
  Blue         ("~ ~ 1"),
  Cyan         ("~ ~ 2"),
  DodgerBlue   ("~ ~ 3"),
  Green        ("~ ~ 4"),
  LightGreen   ("~ ~ 5"),
  Red          ("~ ~ 6"),
  Magenta      ("~ ~ 7"),
  Pink         ("~ ~ 8"),
  Yellow       ("~ ~ 9"),
  Orange       ("~ ~ 10"),
  White        ("~ ~ 11"),
*/

//  *************************************************************************
//  *************************************************************************
//  ***********************     THE BLADE STYLES     ************************
//  *************************************************************************
//  *************************************************************************
#ifdef CONFIG_PRESETS
Preset presets[] = {

   { "SmthJedi;Menus", "tracks/mars.wav",
//  Main Blade:
StylePtr<StandardFlicker>("~ ~ 0"),

//  Crystal Chamber:
StylePtr<Crystal>("~ ~ 0"),
"deepskyblue"},

//  ****************

   { "TeensySF;Menus", "tracks/venus.wav",
//  Main Blade:
StylePtr<HeavyFlicker>("~ ~ 2"),

//  Crystal Chamber:
StylePtr<Crystal>("~ ~ 2"),
"cyan"},

//  ****************

   { "RgueCmdr;Menus", "tracks/mercury.wav",
//  Main Blade:
StylePtr<LocalizedLockup>("~ ~ 6"),

//  Crystal Chamber:
StylePtr<Crystal>("~ ~ 6"),
"red"},

//  ****************

   { "SmthFuzz;Menus", "tracks/uranus.wav",
//  Main Blade:
StylePtr<Unstable>("~ ~ 4"),

//  Crystal Chamber:
StylePtr<Crystal>("~ ~ 4"),
"green"},

//  ****************

   { "SmthGrey;Menus", "tracks/venus.wav",
//  Main Blade:
StylePtr<SlowIgnition>("~ ~ 7"),

//  Crystal Chamber:
StylePtr<Crystal>("~ ~ 7"),
"magenta"},

//  ****************

   { "TthCrstl;Menus", "tracks/mars.wav",
//  Main Blade:
StylePtr<SparkTip>("~ ~ 9"),

//  Crystal Chamber:
StylePtr<Crystal>("~ ~ 9"),
"yellow"},

};
//  *************************************************************************
//  *************************************************************************
BladeConfig blades[] = {
//  Note, replace zero with NO_BLADE when using Blade Detect.
  { 0,  // KR v3.5 36 Inch.
//  Main Blade:
 	WS281XBladePtr<134, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },

  { 1,  // KR v2 36 Inch.
//  Main Blade:
 	WS281XBladePtr<132, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },

  { 2,  // LGT 36 Inch.
//  Main Blade:
 	WS281XBladePtr<128, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },

  { 3,  // KR v3.5 32 Inch.
//  Main Blade:
 	WS281XBladePtr<125, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },

  { 4,  // KR v2 32 Inch.
//  Main Blade:
 	WS281XBladePtr<122, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },

  { 5,  // LGT 32 Inch.
//  Main Blade:
 	WS281XBladePtr<114, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },

  { 6,  //  KR v2 Shoto.
//  Main Blade:
 	WS281XBladePtr<108, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },

  { 7,  //  Custom Length, Full Pixel Count.
//  Main Blade:
 	WS281XBladePtr<144, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>(),
 	WS281XBladePtr<1, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4>>(),
    CONFIGARRAY(presets) },
};
#endif
//  *************************************************************************
//  *************************************************************************
#ifdef CONFIG_BUTTONS
//  Button shorts to GND:
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
#if NUM_BUTTONS == 2
Button AuxButton(BUTTON_AUX, auxPin, "aux");

//  If you need to swap the MAIN and AUX buttons over, use this:
//  Button PowerButton(BUTTON_POWER, auxPin, "pow");
//  #if NUM_BUTTONS == 2
//  Button AuxButton(BUTTON_AUX, powerButtonPin, "aux");

//  If you need the buttons to short to Batt+ or 3.3 Volt instead of Ground, use this:
//  PullDownButton PowerButton(BUTTON_POWER, powerButtonPin, "pow");
//  #if NUM_BUTTONS == 2
//  PullDownButton AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
#endif
