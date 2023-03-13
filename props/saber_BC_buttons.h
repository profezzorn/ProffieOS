/*
saber_BC_buttons.h
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2019 Fredrik Hubinette
  Copyright (c) 2023 Brian Conner with contributions by:
  Fredrik Hubinette, Fernando da Rosa, Matthew McGeary and Scott Weber.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/

Includes 1 and 2 button controls.
Incorporates an intuitive control scheme so button actions are consistant
whether blade is on or off.
Best if used with OS6 ProffieOS_Voicepack spoken menu prompts.
Download your choice of language and variation here:
http://fredrik.hubbe.net/lightsaber/sound/

Features:
- Live preset changing while blade is running (skips font.wav and preon).
- Battle Mode, gesture ignitions, and multi-blast based on fett263's work.
- Auto enter/exit multi-blaster block mode.
- Spoken Battery level in volts OR percentage. Both always available.
- Dedicated Quote sounds - Always available. force.wavs can remain as force.
                           Add quote.wav files to font to use.
                           Live toggle between sequential or random quote play.
- Play / Stop track control while blade is on.
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
                          blade effect too by using EFFECT_USER2 in a TransitionEffectL.

---------------------------------------------------------------------------
Optional #defines:
#define ENABLE_AUTO_SWING_BLAST - Multi-blast initiated by simply swinging
                                  within 1 second of last blast.
                                  Exit by not swinging for 1 second.
#define FEMALE_TALKIE_VOICE     - To use a female voice version of onboard Talkie.
#define NO_VOLUME_MENU          - Option to omit On-the-fly Volume menu control with buttons.

---------------------------------------------------------------------------
Gesture Controls:
- There are four gesture types: swing, stab, thrust and twist.
  For simplicity, using gesture ignition will automatically skip the preon effect.
  Below are the options to add to the config to enable the various gestures.
#define BC_SWING_ON
#define BC_STAB_ON
#define BC_THRUST_ON
#define BC_TWIST_ON
#define BC_TWIST_OFF
#define NO_BLADE_NO_GEST_ONOFF
- If using blade detect, Gesture ignitions or retractions are disabled.
  **NOTE** Only works when a BLADE_DETECT_PIN is defined. 

#define BC_FORCE_PUSH
- This mode plays a force sound (or force push sound if the font contains it) with
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
- Automatic clash/lockup uses the pre and post lockup effects
  so your blade style and font MUST have those capabilities to support
  battle mode.

- Melt will automatically trigger with no buttons when you physically
  stab something, and end when you pull away or push any button.

- Stab will trigger with no buttons and thrusting forward.
  
====================== 1 BUTTON CONTROLS ========================
| Sorted by ON or OFF state: (what it's like while using saber) |
=================================================================

*************   WHILE SABER BLADE IS OFF   ***************
Turn blade ON         - Short click POW. (or gestures if defined, uses FastOn)
                        * NOTE * Gesture ignitions using FastOn bypass preon.
Turn ON without preon - Short click POW while pointing up.
Turn blade ON Muted   - 4x click and hold POW.
Next Preset           - Long click and release POW, or TWIST while pointing up.
Prev Preset           - Double click and hold POW, release after a second (click then long click),
                        or TWIST while pointing down.
Play/Stop Track       - 4x click POW.
Volume Menu:
                      * NOTE * Tilting blade too high or low in Volume Menu will give a warning tone to 
                        tilt up or down to avoid erratic rotational volume changes at extreme blade angles.
        Enter/Exit    - Hold POW + Clash.
        Volume UP     - Rotate Right 
                      - or - 
                      - Long click and release POW while in Volume Menu. (just like next preset)
        Volume DOWN   - Rotate Left 
                      - or -
                      - Double click and hold POW, release after a second while in Volume Menu.
                        (click then long click, just like next preset)
        Quick MAX Vol - Short click POW while in Volume Menu.
        Quick MIN Vol - Double click POW while in Volume Menu.
Spoken Battery Level
        in volts      - Triple click POW.
        in percentage - Triple click and hold POW.
On-Demand Batt Level  - Double click POW.
                        (requires EFFECT_BATTERY_LEVEL to be in blade style,
                        and uses battery.wav sound effect.)

*************   WHILE SABER BLADE IS ON   ****************
Play/Stop Track       - 4x click POW.
Next Preset Fast      - Long click and release POW while pointing up.
Prev Preset Fast      - Double click and release POW after a second
                        while pointing up. (click then long click)
                      * NOTE * Fast switching bypasses preon and font.wav.
Clash                 - No buttons, just hit the blade against something.
                        In Battle Mode, Hold POW and Clash to temporarily
                        override the auto-lockup and do regular Clash.
Stab                  - Just Thrust forward with a stabbing motion.
                        Works in Battle Mode.
Blaster Blocks        - Click or Double click POW.
Spam Blaster Blocks   - 3x click and hold while pointing up. This toggles SPAM BLAST mode ON/OFF,
                        and makes the button super sensitive for multiple blaster blocks.
                        * Note * This gets in the way of normal features,
                        so turn off when you're done spamming.  Plays mzoom.wav.
Auto Swing Blast      - if #define ENABLE_AUTO_SWING_BLAST is active,
                        swinging within 1 second of doing button activated 
                        Blaster Block will start this timed mode.
                        To trigger auto blaster blocks, swing saber
                        within 1 second of last Swing Blast block.
                        To exit, stop swinging for 1 second.
Lockup                - Hold POW + Clash.
                        In Battle Mode, just Clash and stay there,
                        pull away or press POW to end lockup.
Drag                  - Hold POW + Clash while pointing down.
Melt                  - No button, just stab something. pull away or
                        press POW to end.
Lightning Block       - Double click and hold POW.
Battle Mode           - Triple click and hold POW to enter and exit.
                        Power OFF is disabled while in Battle Mode,
                        YOU MUST EXIT THE MODE WITH THIS COMBO FIRST.
Force Effect          - Hold POW + Twist. (while NOT pointing up or down)
Monophonic Force      - Hold POW + Twist. (while pointing up)
Color Change Mode     - Hold POW + Twist. (while pointing down)
                        Rotate hilt to cycle through all available colors, or
                        Click POW to change if ColorChange<> used in blade style,
                        Click + hold POW to save color selection and exit.
                        Triple click POW to cancel and restore original color.
    ColorChange explained:
          If the style uses ColorChange<>, when you activate color change mode,
          there will be up to 12 steps per rotation with a little sound at each step.
          If it does not use ColorChange<>, the color wheel will be activated,
          which has 32768 steps per rotation.
          COLOR_CHANGE_DIRECT makes it so that IF the style uses ColorChange<>,
          when you activate color change mode, it will immediately go to the
          next color and exit color change mode. If the style does not use
          ColorChange<>, it has no effect.
Quote Player          - Triple click POW.
Toggle sequential or 
  random quote play   - 4x click and hold POW. (while pointing down)
Force Push            - Push hilt perpendicularly from a stop.
Swap (EffectSequence) - 4x click and hold POW. (while NOT pointing up)
                        * Requires EFFECT_USER1 in blade style.
PowerSave Dim Blade   - 4x click and hold POW. (while pointing up)
                        To use Power Save requires AlphaL based EffectSequence in style.
Turn off blade        - Hold POW and wait until blade is off,
                        or Twist if using #define BC_TWIST_OFF.
Turn OFF without postoff - Turn OFF while pointing up.

====================== 2 BUTTON CONTROLS ========================
| Sorted by ON or OFF state: (what it's like while using saber) |
=================================================================

*************   WHILE SABER BLADE IS OFF   ***************
Turn blade ON         - Short click POW. (or gestures if defined, uses FastOn)
                        * NOTE * Gesture ignitions using FastOn bypass preon.
Turn ON without preon - Short click POW while pointing up.
Turn blade ON Muted   - 4x click and hold POW.
Next Preset           - Long click and release POW, or TWIST while pointing up.
Prev Preset           - Double click and hold POW, release after a second (click then long click),
                        or TWIST while pointing down.
Play/Stop Track       - Hold AUX + Double click POW.
Volume Menu:
                      * NOTE * Tilting blade too high or low in Volume Menu will give a warning tone to 
                        tilt up or down to avoid erratic rotational volume changes at extreme blade angles.
        Enter/Exit    - Hold POW + Clash.
        Volume UP     - Rotate Right 
                      - or - 
                      - Long click and release POW while in Volume Menu. (just like next preset)
        Volume DOWN   - Rotate Left 
                      - or -
                      - Double click and hold POW, release after a second while in Volume Menu.
                        (click then long click, just like next preset)
        Quick MAX Vol - Short click POW while in Volume Menu.
        Quick MIN Vol - Double click POW while in Volume Menu.
Spoken Battery Level
        in volts      - Triple click POW.
        in percentage - Triple click and hold POW.
On-Demand Batt Level  - Double click POW.
                        (requires EFFECT_BATTERY_LEVEL to be in blade style,
                        and uses battery.wav sound effect.)

*************   WHILE SABER BLADE IS ON   ****************
Play/Stop Track       - Hold AUX + Double click POW.
Next Preset Fast      - Hold AUX + Long click and release POW while pointing up.
Prev Preset Fast      - Hold AUX + Double click and hold POW for a second
                        while pointing up. (click then long click)
                      * NOTE * Fast switching bypasses preon and font.wav.
Clash                 - No buttons, just hit the blade against something.
                        In Battle Mode, Hold any button and Clash to
                        temporarily override the auto-lockup and do regular Clash.
Stab                  - Just Thrust forward with a stabbing motion.
                        Works in Battle Mode.
Blaster Blocks        - Click or Double click POW.
Spam Blaster Blocks   - 3x click and hold while pointing up. This toggles SPAM BLAST mode ON/OFF,
                        and makes the button super sensitive for multiple blaster blocks.
                        * Note * This gets in the way of normal features,
                        so turn off when you're done spamming.  Plays mzoom.wav.
Auto Swing Blast      - if #define ENABLE_AUTO_SWING_BLAST is active,
                        swinging within 1 second of doing button activated 
                        Blaster Block will start this timed mode.
                        To trigger auto blaster blocks, swing saber
                        within 1 second of last Swing Blast block.
                        To exit, stop swinging for 1 second.
Lockup                - Hold AUX + Clash.
                        In Battle Mode, just Clash and stay there,
                        pull away or press any button to end lockup.
Drag                  - Hold AUX + Clash while pointing down.
Melt                  - No button, just stab something,
                        pull away or press any button to end.
Lightning Block       - Double click and hold POW.
Battle Mode           - Hold POW + Click AUX to enter and exit.
                        Power OFF is disabled while in Battle Mode,
                        YOU MUST EXIT THE MODE WITH THIS COMBO FIRST.
Force Effect          - Hold POW + Twist. (while NOT pointing up or down)
Monophonic Force      - Hold POW + Twist. (while pointing up)
Color Change Mode     - Hold POW + Twist. (while pointing down)
                        Rotate hilt to cycle through all available colors, or
                        Click AUX to change if ColorChange<> used in blade style,
                        Click + hold POW to save color selection and exit.
                        Triple click POW to cancel and restore original color.
    ColorChange explained:
          If the style uses ColorChange<>, when you activate color change mode,
          there will be up to 12 steps per rotation with a little sound at each step.
          If it does not use ColorChange<>, the color wheel will be activated,
          which has 32768 steps per rotation.
          COLOR_CHANGE_DIRECT makes it so that IF the style uses ColorChange<>,
          when you activate color change mode, it will immediately go to the
          next color and exit color change mode. If the style does not use
          ColorChange<>, it has no effect.
Quote Player          - Triple click POW.
Toggle sequential or 
  random quote play   - Hold AUX + Twist. (while pointing down)
Force Push            - Push hilt perpendicularly from a stop.
Swap (EffectSequence) - Hold AUX + Twist. (while NOT pointing up)
                        * Requires EFFECT_USER1 in blade style.
PowerSave Dim Blade   - Hold AUX + Twist. (while pointing up)
          (To use Power Save requires AlphaL based EffectSequence in style)
Turn off blade        - Hold POW and wait until blade is off,
                        or Twist if using #define BC_TWIST_OFF.
Turn OFF without postoff - Turn OFF while pointing up.

*/

#ifndef PROPS_SABER_BC_BUTTONS_H
#define PROPS_SABER_BC_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"

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
EFFECT(faston);     // for EFFECT_FAST_ON
                    // *note* faston.wav does not replace out.wav.
                    // they play layered and concurrently.
EFFECT(blstbgn);    // for Begin Multi-Blast
EFFECT(blstend);    // for End Multi-Blast
EFFECT(push);       // for Force Push gesture
EFFECT(quote);      // for playing quotes
EFFECT(monosfx);    // for Monophonically played sounds (iceblade, seismic charge etc...)
EFFECT(swap);       // for standalone triggering EffectSequence<>

#include "../sound/sound_library.h"

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
    } else {
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
  }

// Revert colorchange witout saving (reset to Variation == 0)
  void ResetColorChangeMode() {
    if (!current_style()) return;
      STDOUT << "Reset Color Variation" << "\n";
      SetVariation(0);
      STDOUT << "Color change mode done, variation = " << SaberBase::GetCurrentVariation() << "\n";
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
      STDOUT << "Enter Volume Menu\n";
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
      STDOUT << "Exit Volume Menu\n";
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
      STDOUT << "Volume Up - Current Volume: ";
      STDOUT << dynamic_mixer.get_volume() << "\n";
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
      STDOUT << "Volume Down - Current Volume: ";
      STDOUT << dynamic_mixer.get_volume() << "\n";
    } else {
      QuickMinVolume();
    }
  }

  void QuickMaxVolume() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    dynamic_mixer.set_volume(VOLUME);
    if (SFX_volmax) {
      hybrid_font.PlayPolyphonic(&SFX_volmax);
    } else {
      beeper.Beep(0.5, 3000);
    }
    STDOUT << "Maximum Volume\n";
  }

  void QuickMinVolume() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
    dynamic_mixer.get_volume() - VOLUME * 0.90));
    if (SFX_volmin) {
      hybrid_font.PlayPolyphonic(&SFX_volmin);
      } else {
        beeper.Beep(0.5, 1000);
      }
      STDOUT << "Minimum Volume\n";
  }

  void DetectMenuTurn() {
    if (mode_volume_) {
      if (fusor.angle1() >  M_PI / 3) {
        if (millis() - beep_delay_ > 1000) {
          beeper.Beep(0.05, 950);
          beeper.Beep(0.05, 693);
          beeper.Beep(0.05, 625);
          beeper.Beep(0.05, 595);
          beeper.Beep(0.05, 525);
          beeper.Beep(0.05, 475);
          STDOUT << "**** Blade Too High - Tilt Down \n";
          beep_delay_ = millis();
          return;
        } else {
          return;
        }
      }
      if (fusor.angle1() < - M_PI / 4) {
        if (millis() - beep_delay_ > 1000) {
          beeper.Beep(0.05, 475);
          beeper.Beep(0.05, 525);
          beeper.Beep(0.05, 595);
          beeper.Beep(0.05, 625);
          beeper.Beep(0.05, 693);
          beeper.Beep(0.05, 950);
          STDOUT << "**** Blade Too Low - Tilt Up \n";
          beep_delay_ = millis();
          return;
        } else {
          return;
        }
      }
      float a = fusor.angle2() - current_menu_angle_;
      if (a > M_PI) a-=M_PI*2;
      if (a < -M_PI) a+=M_PI*2;
      if (a > M_PI / 6) VolumeUp();
      if (a < -M_PI / 6) VolumeDown();
    }
  }

  RefPtr<BufferedWavPlayer> wav_player;

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
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
        STDOUT << "Entering Battle Mode\n";
        battle_mode_ = true;
#endif
      return true;
#endif  // BC_SWING_ON

    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
      // pointing down
      if (fusor.angle1() < - M_PI / 4) {
        previous_preset();
        return true;
      }
      // pointing up
      if (fusor.angle1() >  M_PI / 3) {
        next_preset();
      } else {
       // NOT pointing up OR down
#ifdef BC_TWIST_ON
        if (mode_volume_) return false;
#ifdef NO_BLADE_NO_GEST_ONOFF
        if (!blade_detected_) return false;
#endif
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_ > 2000 &&
          millis() - saber_off_time_ > 1000) {
          FastOn();
#ifdef BC_GESTURE_AUTO_BATTLE_MODE
          STDOUT << "Entering Battle Mode\n";
          battle_mode_ = true;
#endif
          last_twist_ = millis();
        }
#endif  // BC_TWIST_ON
      }
      return true;

#ifdef BC_TWIST_OFF
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
#ifdef NO_BLADE_NO_GEST_ONOFF
      if (!blade_detected_) return false;
#endif
      // Delay twist events to prevent false trigger from over twisting
      if (millis() - last_twist_ > 3000) {
        last_twist_ = millis();
        saber_off_time_ = millis();
        battle_mode_ = false;
        // Bypass postoff if pointing up
        if (fusor.angle1() >  M_PI / 3) {
          Off(OFF_FAST);
        } else {
          Off();
        }
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
        STDOUT << "Entering Battle Mode\n";
        battle_mode_ = true;
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
        STDOUT << "Entering Battle Mode\n";
        battle_mode_ = true;
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

// Turns Saber ON
    case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      // No power on without exiting Vol Menu first
      if (!mode_volume_) {
      // Bypass preon if pointing up         
        if (fusor.angle1() >  M_PI / 3) {
          FastOn();
        } else {
          On();
        }
      } else {
        QuickMaxVolume();
      }
      return true;

// Turn Saber ON Muted
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD, MODE_OFF):
      if (!mode_volume_) {
        if (SetMute(true)) {
          unmute_on_deactivation_ = true;
          On();
        }
      }
      return true;

// Next Preset AND Volume Up
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON):
#else
  // 2 button
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON | BUTTON_AUX):
#endif
      // Bypass NewFont and preon if pointing up.
      if (fusor.angle1() >  M_PI / 3) {
        // Don't change preset if in colorchange mode
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
          next_preset_fast();
      }
      return true;
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      if (!mode_volume_) {
        next_preset();
      } else {
        VolumeUp();
      }
      return true;

// Previous Preset AND Volume Down
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_ON):
#else
  // 2 button
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_ON | BUTTON_AUX):
#endif
      // Bypass NewFont and preon if pointing up.
      if (fusor.angle1() >  M_PI / 3) {
        //Don't change preset if in colorchange mode
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
          previous_preset_fast();
      }
      return true;
    case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_OFF):
      if (!mode_volume_) {
        previous_preset();
      } else {
        VolumeDown();
      }
      return true;

// Start or Stop Track
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):
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
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
#endif
        VolumeMenu();
        return true;
#endif

// Spoken Battery Level in volts
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      if (!mode_volume_) {
        sound_library_.SayTheBatteryLevelIs();
        sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
        sound_library_.SayVolts();
        STDOUT << "Battery Voltage: " << battery_monitor.battery() << "\n";
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
      }
      return true;

// Spoken Battery Level in percentage
    case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD, MODE_OFF):
      if (!mode_volume_) {
        sound_library_.SayTheBatteryLevelIs();
        sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
        sound_library_.SayPercent();
        STDOUT << "Battery Percentage: " <<battery_monitor.battery_percent() << "\n";
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
      }
      return true;

// On Demand Battery Level
    case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      if (!mode_volume_) {
        STDOUT << "Battery Voltage: " << battery_monitor.battery() << "\n";
        STDOUT << "Battery Percentage: " <<battery_monitor.battery_percent() << "\n";
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
      } else {
        QuickMinVolume();
      }
      return true;

// Stab
    case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_ON):
      SaberBase::SetClashStrength(2.0);
      SaberBase::DoStab();
    return true;

// Blaster Deflection
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        if (!spam_blast_) {
          //Don't blast if in colorchange mode
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
            SaberBase::DoBlast();
            last_blast_ = millis();
          }
          return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
        if (spam_blast_) {
          //Don't blast if in colorchange mode
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return false;
            SaberBase::DoBlast();
            last_blast_ = millis();
          }
          return true;
#ifdef ENABLE_AUTO_SWING_BLAST
    // Auto enter/exit multi-blast block with swings if swing within 1 second.
    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
      if (millis() - last_blast_ < 1000) {
        SaberBase::DoBlast();
        last_blast_ = millis();
        STDOUT << "Auto Swing Blast mode\n";
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
      if (!SaberBase::Lockup()) {
        // pointing down
        if (fusor.angle1() < - M_PI / 4) {
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
      clash_impact_millis_ = millis();
      if (!SaberBase::Lockup() && !swinging_) {
        SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
        auto_melt_on_ = true;
        SaberBase::DoBeginLockup();
      }
      return true;

// Lightning Block
    case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
      SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
      SaberBase::DoBeginLockup();
      return true;

// Spam Blast toggle - pointing up
// Battle Mode toggle - NOT pointing up
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD, MODE_ON):
#else
  // 2 button
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
#endif
      if (fusor.angle1() >  M_PI / 3) {
        sound_library_.SayZoomingIn();
        spam_blast_ = !spam_blast_;
        return true;
      }
#ifndef BC_NO_BM
      if (!battle_mode_) {
        STDOUT << "Entering Battle Mode\n";
        battle_mode_ = true;
        if (SFX_bmbegin) {
          hybrid_font.PlayCommon(&SFX_bmbegin);
        } else {
          hybrid_font.DoEffect(EFFECT_FORCE, 0);
        }
      } else {
        STDOUT << "Exiting Battle Mode\n";
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
      if (!battle_mode_ || swinging_) return false;
      clash_impact_millis_ = millis();
      SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
      auto_lockup_on_ = true;
      SaberBase::DoBeginLockup();
      return true;

// Color Change - pointing down
// MonoForce  -   pointing up
// Force -        NOT pointing up or down
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
      // pointing down
#ifndef DISABLE_COLOR_CHANGE
      if (fusor.angle1() < - M_PI / 4) {
        ToggleColorChangeMode();
        return true;
      }
#endif
      // pointing up
      if (fusor.angle1() >  M_PI / 3) {
        SaberBase::DoEffect(EFFECT_USER2, 0);
      } else {
       // NOT pointing up OR down
        SaberBase::DoForce();
      }
      return true;

// Quote
// Revert colorchange witout saving (reset to Variation == 0)
    case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
      if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
        ResetColorChangeMode();
        return true;
      } else {
        if (SFX_quote) {
          // if (font_config.sequentialQuote > 0) {
          if (sequential_quote_) {
            SFX_quote.SelectNext();
          } else {
            SFX_quote.Select(-1);
          }
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
        } else {
          SaberBase::DoForce();
        }
      }
      return true;

// Power Save blade dimming - pointing up
// Swap effect - NOT pointing up or down
// Toggle seqential quote play - pointing down
#if NUM_BUTTONS == 1
    case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_MEDIUM, MODE_ON):
#else
    case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_AUX):
#endif
      // pointing up
      if (fusor.angle1() >  M_PI / 3) {
        SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
        return true;
      } else if (fusor.angle1() < - M_PI / 4) {
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
      return true;

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
        if (!battle_mode_) {
          // Bypass postoff if pointing up
          if (fusor.angle1() >  M_PI / 3) {
            Off(OFF_FAST);
          } else {
            Off();
          }
        }
      }
      saber_off_time_ = millis();
      return true;

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

  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      case EFFECT_QUOTE: hybrid_font.PlayCommon(&SFX_quote); return;
      // Dim
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
        }
        return;
      // Gesture on, bybass preon
      case EFFECT_FAST_ON:
        if (SFX_faston) {
          hybrid_font.PlayCommon(&SFX_faston);
        }
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
  float current_menu_angle_ = 0.0;
  bool mode_volume_ = false;
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
  bool battle_mode_ = false;
  bool max_vol_reached_ = false;
  bool min_vol_reached_ = false;
  bool sequential_quote_ = false;
  bool spam_blast_ = false;
  uint32_t thrust_begin_millis_ = millis();
  uint32_t push_begin_millis_ = millis();
  uint32_t clash_impact_millis_ = millis();
  uint32_t last_twist_ = millis();
  uint32_t last_push_ = millis();
  uint32_t last_blast_ = millis();
  uint32_t saber_off_time_ = millis();
  uint32_t beep_delay_ = millis();
};

#endif
