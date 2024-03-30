/*
saber_BC_buttons.h
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2019 Fredrik Hubinette
  Copyright (c) 2023 Brian Conner with contributions by:
  Fredrik Hubinette, Fernando da Rosa, Matthew McGeary, and Scott Weber.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/

Includes 1, or 2 button controls.
Incorporates an intuitive control scheme so button actions are consistant
whether blade is on or off.
Best if used with ProffieOS_Voicepack spoken menu prompts.
Download your choice of language and variation here:
http://fredrik.hubbe.net/lightsaber/sound/
Also, a large variety of FREE in-universe character Voicepacks available here:
https://crucible.hubbe.net/t/additional-voicepacks/4227
If you'd care to make a donation for my time:
https://www.buymeacoffee.com/brianconner

Features:
- Live preset changing while blade is running (skips font.wav and preon).
- Battle Mode, gesture ignitions, and multi-blast based on fett263's work.
- Auto enter/exit multi-blaster block mode.
- Spoken Battery level in volts OR percentage. Both always available.
- Dedicated Quote sounds - Always available. force.wavs can remain as force.
                           Add quote.wav files to font to use.
                           Live toggle between sequential or random quote play.
- Play / Stop track control while blade is OFF OR ON.
- Force Push is always available, not just in Battle Mode.
- Melt is always available as no button, with pull-away or button to end.
- Drag is always clash with button pressed while pointing down.
- No blade inserted = no gestures option if Blade Detect is used.
- Optional On-the-fly volume controls with Quick MIN and MAX levels.
- Bypass preon and/or postoff based on blade angle.
- Spam Blast - Enter this mode to make the button super sensitive for
                            multiple blaster blocks. Presses are prioritized over
                            other features. No limits, no lag when "rapid firing".
- Swap feature with sound - Just an additional EFFECT that can be used to trigger
                            blade animations. See below.
- No inadvertant effects during preon.
- Rotary control of Volume and Scroll Presets. (Rotate hilt like turning a dial)
- NEW Dual blade independent ignition and retraction. (Such as for a staff saber)

* Deprecated feature:
- No Volume UP/DOWN with buttons. Use rotary control instead (see above).
                          QuickMinVolume and QuickMaxVolume are still available.

---------------------------------------------------------------------------
Optional Blade style elements:
On-Demand battery level - A layer built into the blade styles that reacts
                          as the battery gets weaker, changing blade color
                          from Green to Red, and the blade length shortens.
EFFECT_USER1            - Swap feature: Use as a standalone trigger for EffectSequence<>,
                          for example as a way to have multiple blade styles in one preset.
                          Usage:EffectSequence<EFFECT_USER1, item1, item2...>
                          Custom swap.wav files can be used as the sound effect,
                          otherwise ccchange.wav is used.
EFFECT_USER2            - For blade effects with sounds that might work better without hum,
                          this plays sounds monophonically.
                          (think seismic charge silence, iceblade etc...)
                          monosfx.wav files are used. It can be just the sound, or a
                          blade effect too by using TransitionEffectL<......,EFFECT_USER2>.

---------------------------------------------------------------------------
Optional #defines:
#define BC_ENABLE_AUTO_SWING_BLAST - Multi-blast initiated by simply swinging
                                     within 1 second of last blast.
                                     Exit by not swinging for 1 second.
#define FEMALE_TALKIE_VOICE        - To use a female voice version of onboard Talkie.
#define NO_VOLUME_MENU             - Option to omit On-the-fly Volume menu control with buttons.
#define ENABLE_FASTON              - Use faston.wav for FastOn() ignitions, such as gesture ignitions or fast preset change.
                                     The faston.wav sound will be replaced with fastout.wav. 
                                     If you have a good reason to keep faston.wav as is, please post at https://crucible.hubbe.net/
// Adding the following two defines activate Dual Blade code in the prop.
#define DUAL_BLADES                - Turn on Dual Blades mode for a staff saber setup.
                                     Defaults are BLADE 1 and BLADE 2 as MAIN and SECOND blades respectively.
                                     BLADE # corresponds to the blades list in the BladeConfig section of the config file.
#define MAIN_BLADE                 - Optional defines to manually set blade # to the appropriate blade from BladeConfig.
#define SECOND_BLADE                 Example: #define MAIN_BLADE 2 #define SECOND_BLADE 4

---------------------------------------------------------------------------
Gesture Controls:
- There are four gesture types: swing, stab, thrust and twist.
  For simplicity, using gesture ignition will automatically skip the preon effect.
  * NOTE * If using Dual Blades, gestures will turn on all blades.
  Below are the options to add to the config to enable the various gestures.
#define BC_SWING_ON
#define BC_STAB_ON
#define BC_THRUST_ON
#define BC_TWIST_ON
#define BC_TWIST_OFF
#define NO_BLADE_NO_GEST_ONOFF
- If using blade detect, Gesture ignitions or retractions are disabled when no blade is used.
  * Only works when a BLADE_DETECT_PIN is defined.

#define BC_FORCE_PUSH
- This mode plays a force sound (or force push sound if the font contains push.wav) with
  a controlled pushing gesture, and is always available, not just in Battle Mode.

#define BC_FORCE_PUSH_LENGTH
- Used for adjustment to the Push gesture length in millis needed to trigger
  Force Push. Recommended range 1 ~ 10,
  1 = shortest, easiest to trigger, 10 = longest. Default value is 5.

#define BC_NO_BM
- Disable battle mode features.

#define BC_GESTURE_AUTO_BATTLE_MODE
- Makes gesture ignition ALSO enter battle mode automatically on ignition.
- *Note* - Cannot be used if #define BC_NO_BM is active.

"Battle Mode 1.0" by fett263, BC modified version:
- Once you enter battle mode, buttons are not used for lockup.
  Clashing the blade against something will automatically do lockup and then end
  when you pull away.
- Automatic lockup and grazing clash (swinging through) detection works
  by measuring delay of the saber blade pulling back from the clash.
  If you clash the blade and it does not pull back during the delay period,
  it is assumed to be a lockup and the lockup effect will show on the blade.
  If you clash the blade and pull away, only the bgn/end lockup effects will show.
  You can adjust the threshold of this detection by using:
#define BC_LOCKUP_DELAY (insert number here)
  Default value is 200.
  If you don't rebound off the object (opponent's blade) but instead clash while
  swinging through, clash will automatically trigger.
- To manually override the auto-lockup temporarily and get a regular clash,
  hold any button while clashing.
- Automatic clash/lockup uses the bgnlock and endlock effects
  so your blade style and font MUST have those capabilities to support
  battle mode.

- Melt will automatically trigger with no buttons when you physically
  stab something, and end when you pull away or push any button.

- Stab will trigger with no buttons and thrusting forward.

------------------------ GENERAL RULES ------------------------

Pointing the blade up will bypass preon at ignition.
Pointing the blade up will bypass postoff after retraction.
Gesture Ignitions will bypass preon.


====================== 1 BUTTON CONTROLS ========================
| Sorted by ON or OFF state: (what it's like while using saber) |
=================================================================

* See below for modifications to these controls if DUAL_BLADES is used.

*************   WHILE SABER BLADE IS OFF   ***************
Turn blade ON         - Short click POW. (or gestures if defined)
                        * NOTE * Gesture ignitions using FastOn bypass preon.
Turn ON without preon - Short Click POW (while pointing up), or use Gesture Ignition.
                        * Uses fastout.wav if available. If using Dual Blades, Gestures will turn on all blades.
Turn blade ON Muted   - 4x click and Hold POW.
                        * Optional mute.wav will play before silent ignition and operation. Saber unmutes on retraction.
Scroll Presets        - Click and Hold POW to toggle this mode ON/OFF. Rotate hilt to cycle next and previous presets.
                        * TWIST_ON not available in this mode. Click POW to ignite with chosen preset.
Next Preset           - Long Click and release POW.
Prev Preset           - Double Click and Hold POW, release after a second. (Click then long Click)
                        * Not supported in Dual Blade mode, see below. Use Scroll Presets rotary control instead to go backwards.
Play/Stop Track       - 4x Click POW.
Volume Menu:
        Enter/Exit    - Hold POW + Clash.
        Volume UP     - Rotate Right
        Volume DOWN   - Rotate Left
        Quick MAX Vol - Short Click POW (while in Volume Menu).
        Quick MIN Vol - Double Click POW (while in Volume Menu).
Spoken Battery Level
        in volts      - Triple Click POW.
        in percentage - Triple Click and Hold POW.
On-Demand Batt Level  - Double Click POW.
                        * NOTE * Requires EFFECT_BATTERY_LEVEL to be in blade style,and uses battery.wav sound effect.

*************   WHILE SABER BLADE IS ON   ****************
Play/Stop Track       - 4x Click POW.
Next Preset Fast      - Long Click and release POW (while pointing up).
                        * Fast switching presets bypasses preon and font.wav.
Prev Preset Fast      - Double Click and Hold POW, release after a second (while pointing up). (Click then long Click)
                        * Not supported in Dual Blade mode.
Clash                 - No button, just hit the blade against something.
                        In Battle Mode, Hold POW and Clash to temporarily
                        override the auto-lockup and do regular Clash.
Stab                  - Just Thrust forward with a stabbing motion. Works in Battle Mode.
Blaster Blocks        - Click or Double click POW. (only Double click if DUAL_BLADES used, see below)
Spam Blaster Blocks   - 3x Click and Hold POW (while pointing up). This toggles SPAM BLAST mode ON/OFF,
                        and makes the button super sensitive for multiple blaster blocks.
                        * NOTE * This gets in the way of normal features,
                        so turn off when you're done spamming. Plays mzoom.wav for enter/exit this mode.
Auto Swing Blast      - Swinging within 1 second of doing a button activated Blaster Block will start this timed mode.
                        To trigger auto blaster blocks, swing saber within 1 second of last block.
                        To exit, stop swinging for 1 second.
                        * NOTE * Requires #define BC_ENABLE_AUTO_SWING_BLAST.
Lockup                - Hold POW + Clash. Release button to end.
                        In Battle Mode, just Clash and stay there. Pull away or press POW to end lockup.
Drag                  - Hold POW + Clash (while pointing down). Release button to end.
Melt                  - No button, just stab something. Pull away or press POW to end.
Lightning Block       - Double Click and Hold POW. Release button to end.
Battle Mode           - Triple Click and Hold POW to enter and exit Battle Mode.
                        * NOTE * Power OFF is disabled while in Battle Mode. YOU MUST EXIT THE MODE WITH THIS COMBO FIRST.
Force Effect          - Hold POW + Twist (while NOT pointing up or down).
Monophonic Force      - Hold POW + Twist (while pointing up).
Color Change Mode     - Hold POW + Twist (while pointing down).
                        Rotate hilt to cycle through all available colors, or
                        Click POW to change if ColorChange<> used in blade style.
                        Click + Hold POW to save color selection and exit.
                        Triple Click POW to exit without saving, restoring original color from the preset.
    ColorChange explained:
      If RotateColorsX<Variation,COLOR> is used in the blade style:
          Rotate hilt to cycle through all available colors, 
          Hold POW to save color selection and exit.
      If ColorChange<> is used in the blade style:
          There are up to 12 colors per rotation with a ccchange.wav sound at each step.
          If also using #define COLOR_CHANGE_DIRECT in the config,
          simply entering Color Change Mode will select the next color in the list and exit Color Change Mode.
Quote Player          - Triple Click POW.
Toggle sequential or
  random quote play   - 4x Click and Hold POW (while pointing down).
Force Push            - Push hilt perpendicularly from a stop.
Swap (EffectSequence) - 4x Click and Hold POW (while NOT pointing up).
                        * NOTE * Requires EFFECT_USER1 in blade style.
PowerSave Dim Blade   - 4x Click and Hold POW (while pointing up).
                        * NOTE * Power Save should apply a transparent amount of Black to the base blade color, for example:
                        50% brightness reduction - EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<0>>>
Turn OFF blade        - Hold POW until off -or - Twist if using #define BC_TWIST_OFF.
Turn OFF bypass postoff - Turn OFF (while pointing up).

====================== 2 BUTTON CONTROLS ========================
| Sorted by ON or OFF state: (what it's like while using saber) |
=================================================================

*************   WHILE SABER BLADE IS OFF   ***************
Turn blade ON         - Short click POW. (or gestures if defined)
                        * NOTE * Gesture ignitions using FastOn bypass preon.
Turn ON without Preon - Short Click POW (while pointing up), or use Gesture Ignition.
                        * Uses fastout.wav if available. If using Dual Blades, Gestures will turn on all blades.
Turn blade ON Muted   - 4x Click and Hold POW.
Scroll Presets        - Click and Hold POW to toggle this mode ON/OFF. Rotate hilt to cycle next and previous presets.
                        * TWIST_ON not available in this mode. Click POW to ignite with chosen preset.
Next Preset           - Hold AUX + Long Click and release POW.
Prev Preset           - Hold AUX + Double Click and Hold POW, release after a second (Click then long Click).
Play/Stop Track       - Hold AUX + Double Click POW.
Volume Menu:
        Enter/Exit    - Hold AUX + Click POW.
        Volume UP     - Rotate Right
        Volume DOWN   - Rotate Left
        Quick MAX Vol - Short Click POW (while in Volume Menu).
        Quick MIN Vol - Double Click POW (while in Volume Menu).
Spoken Battery Level
        in volts      - Triple Click POW.
        in percentage - Triple Click and Hold POW.
On-Demand Batt Level  - Long Click AUX.
                        * Requires EFFECT_BATTERY_LEVEL to be in blade style. Uses battery.wav sound effect.

*************   WHILE SABER BLADE IS ON   ****************
Play/Stop Track       - Hold AUX + Double Click POW.
Next Preset Fast      - Hold AUX + Long Click and release POW (while pointing up).
                        * Fast switching presets bypasses preon and font.wav.
Prev Preset Fast      - Hold AUX + Double Click and Hold POW for a second (while pointing up). (Click then long Click)
Clash                 - No buttons, just hit the blade against something.
                        In Battle Mode, Hold any button and Clash to
                        temporarily override the auto-lockup and do regular Clash.
Stab                  - Just Thrust forward with a stabbing motion. Works in Battle Mode.
Blaster Blocks        - Click or Double click POW. (only Double click if DUAL_BLADES used, see below)
Spam Blaster Blocks   - 3x Click and Hold (while pointing up). This toggles SPAM BLAST mode on/off,
                        and makes the button super sensitive for multiple blaster blocks.
                        * NOTE * This gets in the way of normal features,
                        so turn off when you're done spamming. Plays mzoom.wav for enter/exit this mode.
Auto Swing Blast      - Swinging within 1 second of doing a button activated Blaster Block will start this timed mode.
                        To trigger auto blaster blocks, swing saber within 1 second of last block.
                        To exit, stop swinging for 1 second.
                        * NOTE * Requires #define BC_ENABLE_AUTO_SWING_BLAST.
Lockup                - Hold AUX + Clash. Release button to end.
                        In Battle Mode, just Clash and stay there. Pull away or press any button to end lockup.
Drag                  - Hold AUX + Clash (while pointing down). Release button to end.
Melt                  - No button, just stab something. Pull away or press any button to end.
Lightning Block       - Double Click and Hold POW. Release button to end.
Battle Mode           - Hold POW + Click AUX to enter and exit Battle Mode.
                        * NOTE * Power OFF is disabled while in Battle Mode. YOU MUST EXIT THE MODE WITH THIS COMBO FIRST.
Force Effect          - Hold POW + Twist (while NOT pointing up or down).
Monophonic Force      - Hold POW + Twist (while pointing up).
Color Change Mode     - Hold POW + Twist (while pointing down).
                        Rotate hilt to cycle through all available colors, or
                        Click POW to change if ColorChange<> used in blade style.
                        Click + Hold POW to save color selection and exit.
                        Triple Click POW to exit without saving, restoring original color from the preset.
    ColorChange explained:
      If RotateColorsX<Variation,COLOR> is used in the blade style:
          Rotate hilt to cycle through all available colors, 
          Hold POW to save color selection and exit.
      If ColorChange<> is used in the blade style:
          There are up to 12 colors per rotation with a ccchange.wav sound at each step.
          If also using #define COLOR_CHANGE_DIRECT in the config,
          simply entering Color Change Mode will select the next color in the list and exit Color Change Mode.
Quote Player          - Triple Click POW.
Toggle sequential or
  random quote play   - Hold AUX + Twist (while pointing down).
Force Push            - Push hilt perpendicularly from a stop.
Swap (EffectSequence) - Hold AUX + Twist (while NOT pointing up or down).
                        * NOTE * Requires EFFECT_USER1 in blade style.
PowerSave Dim Blade   - Hold AUX + Twist (while pointing up).
                        * NOTE * Power Save should apply a transparent amount of Black to the base blade color, for example:
                        50% brightness reduction - EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<0>>>
Turn OFF blade        - Hold POW until off -or - Twist if using #define BC_TWIST_OFF.
Turn OFF bypass postoff - Turn OFF (while pointing up).

======================================== DUAL BLADES CONTROLS - Any Number of Buttons ========================================
|
|  These controls override the ones above for Single blade use. If a control is not in this list, use the one from above.
|   **** Dual Blades requires defining MAIN_BLADE and SECOND_BLADE. See Optional defines above.
|   **** Pointing UP/DOWN specific controls (like color change and Power Save Dim) are still based on the MAIN blade's orientation except for:
|        - ON/OFF bypassing preon or postoff as listed below
|        - Previous preset fast (changing presets while ON) can be done with either blade pointing up
|        - Controls in the list below can use either blade pointing up.
|
|  Changes to 1 button controls:
|     - No On-Demand  battery level (On blade display, non-spoken). (was Double Click when OFF)
|                                           Use Spoken Battery Level instead,
|                                           - or - 
|                                           If the hilt has an AUX button,
|                                           Long Click AUX.
|     - No Previous Preset or Previous Preset fast. Use Scroll Presets rotary control instead to Scroll backwards.
|                                           Next Preset fast is still available with button.
|     - No Power lock if in Battle Mode since it's not likely to accidentally double Click.
|     - No Double Click Blaster Blocks. Use Spam Blast if needed.
|
|  ANY number of buttons Dual Blade controls:
|     - Turn Blade pointing up ON           - Point target blade up and Click POW. This will also bypass preon.
|                                           * If no blade is pointing up, both blades will ignite normally.
|     - Turn Main Blade ON/OFF              - Double Click POW
|     - Turn Second Blade ON/OFF            - Double Click and Hold POW, release after a second (while NOT pointing up).
|                                           - or - 
|                                           If the hilt has an AUX button,
|                                           Double Click AUX
|                                           * The AUX button on 2 button sabers can be programmed to do things as long as CONFIG_BUTTONS
|                                           has the AUX button defined, even if running 1 button controls (NUM_BUTTONS 1).
|     - Turn ON/OFF Dual Blades             - Double Click and Hold POW. (Turns on/off all blades)
|                                           * This is also the default ON for Gesture ignitions.
|                                               Turn ON behavior  - If no blades are ignited, this turns both ON.
|                                                                 If there's a blade already ignitied, 
|                                                                 doing Dual Blade button action will ignite the other blade.
|                                               Turn OFF behavior - Will only turn OFF if BOTH blades are ignited.
|
|     - Turn ON/OFF bypassing preon/postoff - Point either blade UP.
|                                           * Uses fastout.wav if available. font.wav and preon are skipped with Gesture ignitions.
|     - Turn ON Muted                       - 4x Click and Hold POW.
|                                           * Will ignite whichever blade is pointing up. If no blade is pointing up, both blades will ignite.
|                                             Optional mute.wav will play before silent ignition and operation. Saber unmutes on retraction.
|
|     - Lightning Block                     - Click and Hold POW while ON. (Was Double Click and Hold)
*/

#ifndef PROPS_SABER_BC_BUTTONS_H
#define PROPS_SABER_BC_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"
#include "../sound/sound_library.h"

#undef PROP_TYPE
#define PROP_TYPE SaberBCButtons

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 15 * 1000
#endif

#ifndef BC_SWING_ON_SPEED
#define BC_SWING_ON_SPEED 250
#endif

#ifndef BC_LOCKUP_DELAY
#define BC_LOCKUP_DELAY 200
#endif

#ifndef BC_FORCE_PUSH_LENGTH
#define BC_FORCE_PUSH_LENGTH 5
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
#define BUTTON_HELD_MEDIUM_TIMEOUT 800
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif

#ifdef BC_SWING_ON
#define SWING_GESTURE
#endif

#ifdef BC_STAB_ON
#define STAB_GESTURE
#endif

#ifdef BC_TWIST_ON
#define TWIST_GESTURE
#endif

#ifdef BC_THRUST_ON
#define THRUST_GESTURE
#endif

#ifndef MAIN_BLADE
#define MAIN_BLADE 1
#endif

#ifndef SECOND_BLADE
#define SECOND_BLADE 2
#endif

#if defined(MAIN_BLADE) && defined(SECOND_BLADE)
static_assert(MAIN_BLADE != SECOND_BLADE, "MAIN_BLADE and SECOND_BLADE cannot be the same");
#endif

#if defined(NO_BLADE_NO_GEST_ONOFF) && !defined(BLADE_DETECT_PIN)
#error Using NO_BLADE_NO_GEST_ONOFF requires a BLADE_DETECT_PIN to be defined
#endif

#if defined(BC_NO_BM) && defined(BC_GESTURE_AUTO_BATTLE_MODE)
#error You cannot define both BC_NO_BM and BC_GESTURE_AUTO_BATTLE_MODE
#endif

EFFECT(dim);        // for EFFECT_POWERSAVE
EFFECT(battery);    // for EFFECT_BATTERY_LEVEL
EFFECT(bmbegin);    // for Begin Battle Mode
EFFECT(bmend);      // for End Battle Mode
EFFECT(vmbegin);    // for Begin Volume Menu
EFFECT(vmend);      // for End Volume Menu
EFFECT(volup);      // for increse volume
EFFECT(voldown);    // for decrease volume
EFFECT(volmin);     // for minimum volume reached
EFFECT(volmax);     // for maximum volume reached
#ifdef ENABLE_FASTON
#warning The faston.wav sound will be replaced with fastout.wav. If you have a good reason to keep faston.wav as is, please post at https://crucible.hubbe.net/
EFFECT(faston);     // for EFFECT_FAST_ON. Being replaced by fastout.wav, which is already defined in the main OS.
#endif
EFFECT(blstbgn);    // for Begin Multi-Blast
EFFECT(blstend);    // for End Multi-Blast
EFFECT(push);       // for Force Push gesture
EFFECT(quote);      // for playing quotes
EFFECT(monosfx);    // for Monophonically played sounds (iceblade, seismic charge etc...)
EFFECT(swap);       // for standalone triggering EffectSequence<>
EFFECT(mute);       // Notification before muted ignition to avoid confusion.

class DelayTimer {
public:
    DelayTimer() : triggered_(false), trigger_time_(0), duration_(0) {}

    void trigger(uint32_t duration) {
        triggered_ = true;
        trigger_time_ = millis();
        duration_ = duration;
    }

    bool timerCheck() {
        if (!triggered_) return false;
        if (millis() - trigger_time_ > duration_) {
            triggered_ = false;  // Reset the timer flag
            return true;  // Timer has elapsed
        }
        return false;  // Timer is still running
    }

private:
    bool triggered_;
    uint32_t trigger_time_;
    uint32_t duration_;
};

// The Saber class implements the basic states and actions
// for the saber.
class SaberBCButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  SaberBCButtons() : PropBase() {}
  const char* name() override { return "SaberBCButtons"; }

  void Loop() override {
    PropBase::Loop();
    DetectMenuTurn();
    DetectTwist();
    Vec3 mss = fusor.mss();
    sound_library_.Poll(wav_player);
    if (wav_player && !wav_player->isPlaying()) wav_player.Free();
    if (SaberBase::IsOn()) {
      DetectSwing();
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > BC_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > BC_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }
      //EVENT_PUSH
      if (fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 70 &&
          fusor.swing_speed() < 30 &&
          fabs(fusor.gyro().x) < 10) {
        if (millis() - push_begin_millis_ > BC_FORCE_PUSH_LENGTH) {
          Event(BUTTON_NONE, EVENT_PUSH);
          push_begin_millis_ = millis();
        }
      } else {
        push_begin_millis_ = millis();
      }
    } else {  // SaberBase is not ON
      // EVENT_SWING - Swing On gesture control to allow fine tuning of speed needed to ignite
      if (millis() - saber_off_time_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (swinging_ && fusor.swing_speed() < 90) {
          swinging_ = false;
        }
        if (!swinging_ && fusor.swing_speed() > BC_SWING_ON_SPEED) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
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

    // Mute timer check - play optional mute.wav first.
    if (mute_timer_.timerCheck()) {
      DoMute();
    }
    // Scroll Presets timer check - avoid beep/wav overlap
    if (scroll_presets_timer_.timerCheck() && scroll_presets_) {
        SaberBase::DoEffect(EFFECT_NEWFONT, 0);
    }
  }  // Loop()

#ifdef SPEAK_BLADE_ID
  void SpeakBladeID(float id) override {
    if (&SFX_mnum) {
      sound_library_.SayNumber(id, SAY_WHOLE);
    } else {
      PVLOG_NORMAL << "No mnum.wav number prompts found.\n";
      beeper.Beep(0.25, 2000.0);
    }
  }
#endif

// Revert colorchange without saving (reset to Variation == 0)
  void ResetColorChangeMode() {
    if (!current_style()) return;
      PVLOG_NORMAL << "Reset Color Variation\n";
      SetVariation(0);
      PVLOG_NORMAL << "Color change mode reverted to config color. Variation = " << SaberBase::GetCurrentVariation() << "\n";
      SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
  }

// Volume Menu
    void VolumeMenu() {
    if (!mode_volume_) {
      current_menu_angle_ = fusor.angle2();
      mode_volume_ = true;
      if (SFX_vmbegin) {
        sound_library_.SayEnterVolumeMenu();
      } else {
        beeper.Beep(0.1, 1000);
        beeper.Beep(0.1, 2000);
        beeper.Beep(0.1, 3000);
      }
      PVLOG_NORMAL << "Enter Volume Menu\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    } else {
      mode_volume_ = false;
      if (SFX_vmend) {
        sound_library_.SayVolumeMenuEnd();
      } else {
        beeper.Beep(0.1, 3000);
        beeper.Beep(0.1, 2000);
        beeper.Beep(0.1, 1000);
      }
      PVLOG_NORMAL << "Exit Volume Menu\n";
    }
  }

  void VolumeUp() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    current_menu_angle_ = fusor.angle2();
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1,
        dynamic_mixer.get_volume() + VOLUME * 0.10));
      if (SFX_volup) {
        hybrid_font.PlayPolyphonic(&SFX_volup);
      } else {
        beeper.Beep(0.10, 2000);
        beeper.Beep(0.20, 2500);
      }
      PVLOG_NORMAL << "Volume Up - Current Volume: ";
      PVLOG_NORMAL << dynamic_mixer.get_volume() << "\n";
    } else {
      QuickMaxVolume();
    }
  }

  void VolumeDown() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    current_menu_angle_ = fusor.angle2();
    if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
        dynamic_mixer.get_volume() - VOLUME * 0.10));
      if (SFX_voldown) {
        hybrid_font.PlayPolyphonic(&SFX_voldown);
      } else {
        beeper.Beep(0.10, 2000);
        beeper.Beep(0.20, 1500);
      }
      PVLOG_NORMAL << "Volume Down - Current Volume: ";
      PVLOG_NORMAL << dynamic_mixer.get_volume() << "\n";
    } else {
      QuickMinVolume();
    }
  }

  void QuickMaxVolume() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    dynamic_mixer.set_volume(VOLUME);
    if (millis() - volume_range_delay_ > 2000) {
      if (SFX_volmax) {
        hybrid_font.PlayPolyphonic(&SFX_volmax);
      } else {
        beeper.Beep(0.5, 3000);
      }
      PVLOG_NORMAL << "Maximum Volume\n";
      volume_range_delay_ = millis();
    }
  }

  void QuickMinVolume() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
    dynamic_mixer.get_volume() - VOLUME * 0.90));
    if (millis() - volume_range_delay_ > 2000) {
      if (SFX_volmin) {
        hybrid_font.PlayPolyphonic(&SFX_volmin);
      } else {
        beeper.Beep(0.5, 1000);
      }
      PVLOG_NORMAL << "Minimum Volume\n";
      volume_range_delay_ = millis();
    }
  }

  void DetectMenuTurn() {
    float a = fusor.angle2() - current_menu_angle_;
    if (is_pointing_up()) return;
    if (a > M_PI) a-=M_PI*2;
    if (a < -M_PI) a+=M_PI*2;
    if (mode_volume_) {
      if (a > M_PI / 6) VolumeUp();
      if (a < -M_PI / 6) VolumeDown();
    } else if (scroll_presets_) {

      if (a > M_PI / 6) {
        beeper.Beep(0.05, 4000);
        current_menu_angle_ = fusor.angle2();
        next_preset();
        }

      if (a < -M_PI / 6) {
        beeper.Beep(0.05, 3000);
        current_menu_angle_ = fusor.angle2();
        previous_preset();
      }
    }
  }

#ifdef DUAL_BLADES
  void ControlBladeState(bool& this_blade_on_, bool other_blade_on_, bool& this_blade_first_, bool& other_blade_first_, uint16_t THIS_BLADE, uint16_t OTHER_BLADE, bool is_main_blade_) {
    const char* curblade = is_main_blade_ ? "Main" : "Second";
    // Bypass postoff if $curblade blade is pointing up
    SaberBase::OffType off_type = is_pointing_up() ? SaberBase::OFF_FAST : SaberBase::OFF_NORMAL;

    if (!mode_volume_) {
      if (!this_blade_on_) {
        if (other_blade_on_) {
          // We're not first to the party, let's just casually turn on.
          SaberBase::TurnOn(EffectLocation(0, ~BladeSet::fromBlade(OTHER_BLADE)));
        } else {
          this_blade_first_ = true;
          // Bypass preon if either blade is pointing up
          if (is_pointing_up()) {
            FastOn(EffectLocation(0, ~BladeSet::fromBlade(OTHER_BLADE)));
          } else {
            // We're first! Every blade is invited to turn on with us...except that "other" blade.
            On(EffectLocation(0, ~BladeSet::fromBlade(OTHER_BLADE)));
          }
        }
        PVLOG_NORMAL << "** " << curblade << " Blade Activated\n";
      } else if (other_blade_on_) {
        // Turn off just this blade and don't mess with muted state (don't use `Off()` here)
        SaberBase::TurnOff(OffType::OFF_NORMAL, EffectLocation(0, BladeSet::fromBlade(THIS_BLADE)));
        PVLOG_NORMAL << "** " << curblade << " Blade Deactivated\n";
      } else if (!SaberBase::Lockup()) {
        // Other blade is not on, and not in lockup, so just do normal Off() with appropriate off_type
        Off(off_type);
        PVLOG_NORMAL << "** All Blades Deactivated\n";
        this_blade_first_ = false;
        other_blade_first_ = false;
      }
      this_blade_on_ = !this_blade_on_;
      saber_off_time_ = millis();
    }
  }

  void TurnMainBladeOnOff() {
    ControlBladeState(main_blade_on_, second_blade_on_, main_blade_first_, second_blade_first_, MAIN_BLADE, SECOND_BLADE, true);
  }

  void TurnSecondBladeOnOff() {
   ControlBladeState(second_blade_on_, main_blade_on_, second_blade_first_, main_blade_on_, SECOND_BLADE, MAIN_BLADE, false);
  }
#endif //  DUAL_BLADES

  bool is_pointing_up() {
    if (fusor.angle1() > M_PI / 3) return true;
#ifdef DUAL_BLADES
    if (fusor.angle1() < -M_PI / 3) return true;
#endif
    return false;
  }

  void TurnOnHelper() {
    if (is_pointing_up()) {
      FastOn();
    } else {
      On();
    }
  }

  void TurnOffHelper() {
      if (is_pointing_up()) {
      Off(OFF_FAST);
    } else {
      Off();
    }
    saber_off_time_ = millis();
    battle_mode_ = false;
  }

  void DoMute() {
    if (SetMute(true)) {
      unmute_on_deactivation_ = true;
#ifdef DUAL_BLADES
      // The blade pointing up gets lit with single click
      if (fusor.angle1() > M_PI / 3) {
        TurnMainBladeOnOff();
        PVLOG_NORMAL << "** Main Blade Activated Muted\n";
      } else if (fusor.angle1() < -M_PI / 3) {
        TurnSecondBladeOnOff();
        PVLOG_NORMAL << "** Second Blade Activated Muted\n";
      } else {
        On();
        PVLOG_NORMAL << "** Dual Blades Activated Muted\n";
        // WHO'S ON FIRST? Doesn't matter, let's just choose one. Ok fine, Main wins.
        main_blade_first_ = true;
        main_blade_on_ = true;
        second_blade_on_ = true;
        scroll_presets_ = false;
      }
#else
      TurnOnHelper();
#endif
    }
  }

  RefPtr<BufferedWavPlayer> wav_player;

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      // storage of unused cases
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
        // if (accel_.x < -0.15) { // maybe use for drag on ceiling???
        //   pointing_down_ = true;
        // } else {
        //   pointing_down_ = false;
        // }
      return true;

// Gesture Ignition Controls
#ifdef BC_SWING_ON
    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
      if (mode_volume_) return false;
#ifdef NO_BLADE_NO_GEST_ONOFF
      if (!blade_detected_) return false;
#endif
      FastOn();
#ifdef BC_GESTURE_AUTO_BATTLE_MODE
      PVLOG_NORMAL << "Entering Battle Mode\n";
      battle_mode_ = true;
#endif
#ifdef DUAL_BLADES
      PVLOG_NORMAL << "** Dual Blades Activated\n";
      main_blade_first_ = true;
      main_blade_on_ = true;
      second_blade_on_ = true;
#endif
      return true;
#endif  // BC_SWING_ON

#ifdef BC_TWIST_ON
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
      if (mode_volume_ || scroll_presets_) return false;
#ifdef NO_BLADE_NO_GEST_ONOFF
      if (!blade_detected_) return false;
#endif
      // Delay twist events to prevent false trigger from over twisting
      if (millis() - last_twist_ > 2000 &&
        millis() - saber_off_time_ > 1000) {
        FastOn();
#ifdef BC_GESTURE_AUTO_BATTLE_MODE
        PVLOG_NORMAL << "Entering Battle Mode\n";
        battle_mode_ = true;
#endif
        last_twist_ = millis();
#ifdef DUAL_BLADES
      PVLOG_NORMAL << "** Dual Blades Activated\n";
      main_blade_first_ = true;
      main_blade_on_ = true;
      second_blade_on_ = true;
#endif
      }
      return true;
#endif  // BC_TWIST_ON

#ifdef BC_TWIST_OFF
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
#ifdef NO_BLADE_NO_GEST_ONOFF
      if (!blade_detected_) return false;
#endif
      // Delay twist events to prevent false trigger from over twisting
      if (millis() - last_twist_ > 3000) {
        last_twist_ = millis();
        TurnOffHelper();
#ifdef DUAL_BLADES
          PVLOG_NORMAL << "** All Blades Deativated\n";
          main_blade_on_ = false;
          second_blade_on_ = false;
          main_blade_first_ = false;
          second_blade_first_ = false;
#endif
      }
      return true;
#endif  // BC_TWIST_OFF

#ifdef BC_STAB_ON
    case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
      if (mode_volume_) return false;
#ifdef NO_BLADE_NO_GEST_ONOFF
      if (!blade_detected_) return false;
#endif
      // Delay Stab On at boot
      if (millis() - saber_off_time_ > 1000) {
        FastOn();
#ifdef BC_GESTURE_AUTO_BATTLE_MODE
        PVLOG_NORMAL << "Entering Battle Mode\n";
        battle_mode_ = true;
#endif
#ifdef DUAL_BLADES
      PVLOG_NORMAL << "** Dual Blades Activated\n";
      main_blade_first_ = true;
      main_blade_on_ = true;
      second_blade_on_ = true;
#endif
      }
      return true;
#endif  // BC_STAB_ON

#ifdef BC_THRUST_ON
    case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
      if (mode_volume_) return false;
#ifdef NO_BLADE_NO_GEST_ONOFF
      if (!blade_detected_) return false;
#endif
      // Delay Thrust On at boot
      if (millis() - saber_off_time_ > 1000) {
        FastOn();
#ifdef BC_GESTURE_AUTO_BATTLE_MODE
        PVLOG_NORMAL << "Entering Battle Mode\n";
        battle_mode_ = true;
#endif
#ifdef DUAL_BLADES
      PVLOG_NORMAL << "** Dual Blades Activated\n";
      main_blade_first_ = true;
      main_blade_on_ = true;
      second_blade_on_ = true;
#endif
      }
      return true;
#endif  // BC_THRUST_ON

#ifdef BC_FORCE_PUSH
    case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
      // Delay Force Push from previous Push
      if (millis() - last_push_ > 2000) {
        if (SFX_push) {
          hybrid_font.PlayCommon(&SFX_push);
        } else {
          hybrid_font.DoEffect(EFFECT_FORCE, 0);
        }
        last_push_ = millis();
      }
      return true;
#endif  // BC_FORCE_PUSH


// Turn Saber ON
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      // No power on without exiting Vol Menu first
      if (mode_volume_) {
        QuickMaxVolume();
      } else {
#ifdef DUAL_BLADES
        // The blade pointing up gets lit with single click
        if (fusor.angle1() >  M_PI / 3) {
         TurnMainBladeOnOff();
          PVLOG_NORMAL << "** Main Blade Activated\n";
        } else if (fusor.angle1() < -M_PI / 3) {
          TurnSecondBladeOnOff();
          PVLOG_NORMAL << "** Second Blade Activated\n";
        } else {
          On();
          PVLOG_NORMAL << "** Dual Blades Activated\n";
          // WHO'S ON FIRST? Doesn't matter, let's just choose one. Ok fine, Main wins.
          main_blade_first_ = true;
          main_blade_on_ = true;
          second_blade_on_ = true;
          scroll_presets_ = false;
        }
#else
        TurnOnHelper();
#endif
      }
      return true;

// Turn Saber ON Muted
case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD, MODE_OFF):
    if (!mode_volume_) {
      if (SFX_mute) {
        hybrid_font.PlayCommon(&SFX_mute);
        mute_timer_.trigger(SaberBase::sound_length * 1000);
      } else {
        DoMute();
      }
    }
    return true;

#ifdef DUAL_BLADES

// Turn Main Blade ON/OFF (Double Click POW)
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):  // pow s2
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):  // no battery level
      if (!mode_volume_) {
        TurnMainBladeOnOff();
      } else {
        QuickMinVolume();
      }
      return true;

// Turn Second Blade ON/OFF using POW button (Double Click and Hold POW for a second a.k.a. Click and long Click)
// QuickMinVolume
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_ON):  // pow L2
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_OFF):  // no previous preset or Vol down
      TurnSecondBladeOnOff();
      return true;

// Turn Second Blade ON/OFF using AUX button (Double Click AUX)
// Works on 2 button sabers even if running 1 button controls. Just a bonus.
    case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):  // aux s2
    case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        TurnSecondBladeOnOff();
      return true;

// Turn Dual Blades ON/OFF (Double Click and Hold POW)
    case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):  // pow m2
    case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
     // const char* whichBladesOff = ((!main_blade_on_ && !second_blade_on_) ? "Both OFF" : "AtLeast1 ON" );
     // const char* whichBladesOn = ((main_blade_on_ && second_blade_on_) ? "Both ON" : "AtLeast1 OFF" );
     // const char* whichBlades =       if (!mode_volume_) {
        if (!(main_blade_on_ && second_blade_on_)) {  // BOTH ARE NOT ON Turn the other blade on.
          if (main_blade_on_ && !second_blade_on_) {  // Determine which one is on
            TurnSecondBladeOnOff();
            PVLOG_NORMAL << "** Second Blade Activated\n";
          } else if (second_blade_on_ && !main_blade_on_) {
            TurnMainBladeOnOff();
            PVLOG_NORMAL << "** Main Blade Activated\n";
          } else {  // BOTH are OFF.
            TurnOnHelper();
            PVLOG_NORMAL << "** Dual Blades Activated\n";
            // WHO'S ON FIRST? Doesn't matter, let's just choose one. Ok fine, Main wins.
            main_blade_first_ = true;
            main_blade_on_ = true;
            second_blade_on_ = true;
          }
        } else if (!SaberBase::Lockup()) {  // BOTH are ON Turn BOTH Off
        // Check if we're alone and pointing up
          TurnOffHelper();
          PVLOG_NORMAL << "** Dual Blades Deativated\n";
          main_blade_on_ = false;
          second_blade_on_ = false;
          main_blade_first_ = false;
          second_blade_first_ = false;
        }
      return true;

#endif // DUAL_BLADES

// Toggle Scroll Presets
  case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
    scroll_presets_ = !scroll_presets_;
    if (scroll_presets_) {
      PVLOG_NORMAL << "** Enter Scroll Presets\n";
      // beep on enter, then play font.wav
      beeper.Beep(0.05, 2000);
      beeper.Silence(0.05);
      beeper.Beep(0.05, 2000);
      beeper.Silence(0.05);
      beeper.Beep(0.10, 3000);
      scroll_presets_timer_.trigger(350);
    } else {
      PVLOG_NORMAL << "** Exit Scroll Presets\n";
      // beep on exit
      beeper.Beep(0.05, 3000);
      beeper.Silence(0.05);
      beeper.Beep(0.05, 3000);
      beeper.Silence(0.05);
      beeper.Beep(0.10, 2000);
      // No need to play font.wav again when exiting
    }
    return true;

// Next Preset
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
#else
    // 2 or 3 button
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF | BUTTON_AUX):
#endif
      if (!mode_volume_) {
        next_preset();
      }
      return true;

// Next Preset Fast
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON):
#else
    // 2 or 3 button
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON | BUTTON_AUX):
#endif
      // Bypass NewFont and preon if a blade is pointing up
      if (is_pointing_up()) {
      // Don't change preset if in colorchange mode
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
        next_preset_fast();
      }
      return true;


// Previous Preset
#if NUM_BUTTONS == 1 && !defined(DUAL_BLADES)
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_OFF):
#endif
    // 2 or 3 button
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_OFF | BUTTON_AUX):
      previous_preset();
      return true;

// Previous Preset fast
#if NUM_BUTTONS == 1 && !defined(DUAL_BLADES)
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_ON):
#else
    // 2 or 3 button, or DUAL_BLADES mode
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_ON | BUTTON_AUX):
#endif // NUM_BUTTONS && DUAL_BLADES
      // Bypass NewFont and preon if a blade is pointing up
      if (is_pointing_up()) {
        // Don't change preset if in colorchange mode
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
        previous_preset_fast();
      }
      return true;


// Start or Stop Track
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):  // pow s4
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_ON):
#else
    // 2 or 3 button
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON | BUTTON_AUX):
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
#endif
      if (!mode_volume_) {
        StartOrStopTrack();
      }
      return true;


// Enter / Exit Volume MENU
#ifndef NO_VOLUME_MENU
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
#else
    // 2 button
    case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
#endif
      VolumeMenu();
      return true;
#endif

// Spoken Battery Level in volts
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):  // pow s3
      if (!mode_volume_) {
        // Avoid weird battery readings when using USB
        if (battery_monitor.battery() < 0.5) {
          sound_library_.SayTheBatteryLevelIs();
          sound_library_.SayDisabled();
          return true;
        }
        sound_library_.SayTheBatteryLevelIs();  // Not just using SayBatteryVolts() because I want it to use the "is" version.
        sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
        sound_library_.SayVolts();
        PVLOG_NORMAL << "Battery Voltage: " << battery_monitor.battery() << "\n";
        speaking_ = true;
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
      }
      return true;

// Spoken Battery Level in percentage
    case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD, MODE_OFF):  // pow h3
      if (!mode_volume_) {
        if (battery_monitor.battery() < 0.5) {
          sound_library_.SayTheBatteryLevelIs();
          sound_library_.SayDisabled();
          return true;
        }
        sound_library_.SayTheBatteryLevelIs();
        sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
        sound_library_.SayPercent();
        PVLOG_NORMAL << "Battery Percentage: " <<battery_monitor.battery_percent() << "%\n";
        speaking_ = true;
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
      }
      return true;

// On-Demand Battery Level
#ifndef DUAL_BLADES
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
#endif
    case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      if (!mode_volume_) {
        PVLOG_NORMAL << "Battery Voltage: " << battery_monitor.battery() << "\n";
        PVLOG_NORMAL << "Battery Percentage: " <<battery_monitor.battery_percent() << "\n";
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
      } else {
        QuickMinVolume();
      }
      return true;

// Stab
    case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_ON):
      //Don't stab if in colorchange mode
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
      if (SaberBase::IsOn()) { // prevents triggering during preon
        SaberBase::SetClashStrength(2.0);
        SaberBase::DoStab();
      }
      return true;

// Blaster Deflection
#ifndef DUAL_BLADES
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
#endif
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      // Don't blast if in colorchange mode
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
      if (!spam_blast_ && SaberBase::IsOn()) {
        SaberBase::DoBlast();
        last_blast_ = millis();
      }
      return true;

// Spam Blaster Blocks
    case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):  // pow p
      // Don't blast if in colorchange mode
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
      if (spam_blast_ && SaberBase::IsOn()) {
        SaberBase::DoBlast();
        last_blast_ = millis();
      }
      return true;

// Auto Swing Blast
    // Auto enter/exit multi-blast block with swings if swing within 1 second
#ifdef BC_ENABLE_AUTO_SWING_BLAST
    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
      if (millis() - last_blast_ < 1000) {
        SaberBase::DoBlast();
        last_blast_ = millis();
        PVLOG_NORMAL << "Auto Swing Blast mode\n";
      }
      break;
#endif

// Lockup
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
#else
  // 2 button
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
#endif
      //Don't lockup if in colorchange mode
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
      if (!SaberBase::Lockup() && SaberBase::IsOn()) {
        // pointing down
        if (fusor.angle1() < -M_PI / 4) {
          SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
        } else {
          if (!battle_mode_) {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          } else {
   // Overrides Auto-lockup if holding Button during clash, NOT pointing down
            break;
          }
        }
        SaberBase::DoBeginLockup();
        return true;
      }
      break;

// Melt
    case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
      //Don't melt if in colorchange mode
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
      if (SaberBase::IsOn()) { // prevents triggering during preon
        clash_impact_millis_ = millis();
        if (!SaberBase::Lockup() && !swinging_) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          auto_melt_on_ = true;
          SaberBase::DoBeginLockup();
        }
      }
      return true;

// Lightning Block / Save Color Change(for Dual Blade mode)
#ifdef DUAL_BLADES
  case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
#else
  case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
#endif
    if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
#ifdef DUAL_BLADES
      // Save and exit color change mode for dual blades.
      ToggleColorChangeMode();
#else
      return false; 
#endif
    }
    if (SaberBase::IsOn()) { // Prevents triggering during preon
      SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
      SaberBase::DoBeginLockup();
    }
    return true;

// Spam Blast toggle - pointing up
// Battle Mode toggle - NOT pointing up
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD, MODE_ON):  // pow h3
#else
  // 2 button
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
#endif
      if (SaberBase::IsOn()) { // prevents triggering during preon
        if (fusor.angle1() > M_PI / 3) {
          sound_library_.SayZoomingIn();
          spam_blast_ = !spam_blast_;
          return true;
        }
#ifndef BC_NO_BM
        if (!battle_mode_) {
          PVLOG_NORMAL << "Entering Battle Mode\n";
          battle_mode_ = true;
          if (SFX_bmbegin) {
            hybrid_font.PlayCommon(&SFX_bmbegin);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
        } else {
          PVLOG_NORMAL << "Exiting Battle Mode\n";
          battle_mode_ = false;
          if (SFX_bmend) {
            hybrid_font.PlayCommon(&SFX_bmend);
          } else {
            beeper.Beep(0.5, 3000);
          }
        }
#endif
      }
      return true;

  // Auto Lockup Mode
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
      if (!battle_mode_ || swinging_) return false;
      clash_impact_millis_ = millis();
      SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
      auto_lockup_on_ = true;
      SaberBase::DoBeginLockup();
      return true;

// Enter/Exit Color Change mode - pointing down
// MonoForce  -   pointing up
// Force -        NOT pointing up or down
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
      if (SaberBase::IsOn()) { // prevents triggering during preon
        // pointing down
#ifndef DISABLE_COLOR_CHANGE
        if (fusor.angle1() < -M_PI / 4) {
          ToggleColorChangeMode();
          return true;
        }
#endif
        // pointing up
        if (fusor.angle1() > M_PI / 3) {
          SaberBase::DoEffect(EFFECT_USER2, 0);
        } else {
         // NOT pointing up OR down
          SaberBase::DoForce();
        }
      }
      return true;

// Quote
// Revert Color Change without saving (reset to Variation == 0)
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):  // pow s3
      if (SaberBase::IsOn()) { // prevents triggering during preon
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          ResetColorChangeMode();
          return true;
        } else {
          if (SFX_quote) {
            if (sequential_quote_) {
              SFX_quote.SelectNext();
            } else {
              SFX_quote.Select(-1);
            }
            SaberBase::DoEffect(EFFECT_QUOTE, 0);
            // hybrid_font.PlayCommon(&SFX_quote);
          } else {
            SaberBase::DoForce();
          }
        }
      }
      return true;


// Power Save blade dimming - pointing up
// Swap effect - NOT pointing up or down
// Toggle seqential quote play - pointing down
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_MEDIUM, MODE_ON):  // pow m4
#else
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_AUX):
#endif
      if (SaberBase::IsOn()) { // prevents triggering during preon
        // pointing up
        if (fusor.angle1() >  M_PI / 3) {
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
          return true;
        } else if (fusor.angle1() < -M_PI / 4) {
        // pointing down
          sequential_quote_ = !sequential_quote_;
          sound_library_.SayRandom();
          if (sequential_quote_) {
            sound_library_.SayDisabled();
          } else {
            sound_library_.SayEnabled();
          }
          return true;
        } else {
        // NOT pointing up or down
          SaberBase::DoEffect(EFFECT_USER1, 0);
        }
      }
      return true;


// Turn Blade OFF / Save Color Change
#ifndef DUAL_BLADES
    case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
      if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          // Just Save and exit color change mode.
          // Don't turn saber off.
          ToggleColorChangeMode();  // Moved to Lightning Black in DUAL_BLADES
          return true;
        }
#endif  // DISABLE_COLOR_CHANGE
        if (!battle_mode_) {
          TurnOffHelper();
        }
      }
      return true;
#endif

// Blade Detect
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

// Events that need to be handled regardless of what other buttons are pressed.
    case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
    case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_OFF):
    case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_OFF):
      saber_off_time_ = millis();
      SaberBase::RequestMotion();
      return true;

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

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_QUOTE:
        hybrid_font.PlayCommon(&SFX_quote);
        return;
      case EFFECT_POWERSAVE:
        if (SFX_dim) {
          hybrid_font.PlayCommon(&SFX_dim);
        } else {
          beeper.Beep(0.1, 1300);
          beeper.Beep(0.1, 900);
          beeper.Beep(0.1, 600);
          beeper.Beep(0.1, 300);
        }
        return;
      // On-Demand Battery Level
      case EFFECT_BATTERY_LEVEL:
        if (speaking_) {
          speaking_ = false;
          return;
        }
        if (SFX_battery) {
          hybrid_font.PlayCommon(&SFX_battery);
        } else {
          beeper.Beep(1.0, 475);
          beeper.Beep(0.5, 693);
          beeper.Beep(0.16, 625);
          beeper.Beep(0.16, 595);
          beeper.Beep(0.16, 525);
          beeper.Beep(1.1, 950);
          beeper.Beep(0.5, 693);
          PVLOG_VERBOSE << "The Force will be with you...always.\n";
        }
        return;
      // Gesture on, bypass preon
      case EFFECT_IGNITION:
        scroll_presets_ = false;
        return;
      case EFFECT_FAST_ON:
#ifdef ENABLE_FASTON
        if (SFX_faston) hybrid_font.PlayCommon(&SFX_faston);
#endif
        scroll_presets_ = false;
        return;
      case EFFECT_FAST_OFF:
        if (SaberBase::IsOn()) {
          Off(OFF_FAST);
          saber_off_time_ = millis();
        }
        return;
      // Swap
      case EFFECT_USER1:
        if (SFX_swap) {
          hybrid_font.PlayCommon(&SFX_swap);
        } else {
          hybrid_font.PlayCommon(&SFX_ccchange);
        }
        return;
      // Monoforce
      case EFFECT_USER2:
        if (SFX_monosfx) {
          hybrid_font.PlayMonophonic(&SFX_monosfx , &SFX_hum);
        } else {
          SaberBase::DoEffect(EFFECT_FORCE, 0);
        }
        return;
    }
  }

private:
  DelayTimer mute_timer_;
  DelayTimer scroll_presets_timer_;

  float current_menu_angle_ = 0.0;
  bool mode_volume_ = false;
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
  bool battle_mode_ = false;
  bool max_vol_reached_ = false;
  bool min_vol_reached_ = false;

  bool main_blade_on_ = false;
  bool second_blade_on_ = false;
  bool main_blade_first_ = false;
  bool second_blade_first_ = false;

  bool sequential_quote_ = false;
  bool spam_blast_ = false;
  // Avoid overlap of battery.wav when doing Spoken Battery Level
  bool speaking_ = false;
  bool scroll_presets_ = false;

  uint32_t thrust_begin_millis_ = millis();
  uint32_t push_begin_millis_ = millis();
  uint32_t clash_impact_millis_ = millis();
  uint32_t last_twist_ = millis();
  uint32_t last_push_ = millis();
  uint32_t last_blast_ = millis();
  uint32_t saber_off_time_ = millis();
  uint32_t volume_range_delay_ = millis();

};

#endif
