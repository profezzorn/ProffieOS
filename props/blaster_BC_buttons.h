/*
blaster_BC_buttons.h prop file.
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2023 Fredrik Hubinette
  Copyright (c) 2023 Brian Conner with contributions by:
  Fredrik Hubinette, Fernando da Rosa, Matthew McGeary, and Scott Weber.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/

Includes:
- Volume Menu with QuickMAX and QuickMIN.
- Spoken battery level in both volts or percentage.
- On-Demand battery level.
- Quote player with On-the-fly sequential option.
- Next / previous preset.
- Self-Destuct overload.
- Defaults to KILL on boot.
- Separate clipin/clipout/reload sounds to match stun mode.
    (requires clipins, clipouts, and reloads.wavs in font.)
- No jam if empty.
- Autofire mode counts bullets and causes empty if
    #define BLASTER_SHOTS_UNTIL_EMPTY is specified.
    Empty repeats at same rate auto was firing for added realism. Cool, eh?

Requires minimum 2 buttons for operation.
If there's a 3rd button installed, it should be set in the config as:
Button Aux2Button(BUTTON_RELOAD, aux2Pin, "aux2");
It's coded to be Reload by default.
However, to use it as a dedicated POWER ON/OFF button instead, use the following define:
#define RELOAD_BUTTON_IS_POWER

  *Notes  - When using dual_prop, the config file can optionally be set to use NUM_BUTTONS 1 for saber mode,
              and the blaster mode will still use 2 buttons as FIRE and MODE.

** This version is designed to work well together with saber_BC_buttons.h when
using dual_prop.h; a toggle between 2 prop files triggered by Blade Detect or Blade ID.
Blade insertion and removal switches between the 2 props, (ie: Ezra blaster and saber).
With dual_prop use, the config file should use SABER buttons in the CONFIG_BUTTONS section, like:
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");

For a stand-alone blaster setup, the config file should use BLASTER buttons in
the CONFIG_BUTTONS section, like:
Button FireButton(BUTTON_FIRE, powerButtonPin, "fire");
Button ModeButton(BUTTON_MODE_SELECT, auxPin, "modeselect");

Optional defines:
  #define BLASTER_ENABLE_AUTO           - Enable Autofire/rapid fire mode.
  #define BLASTER_SHOTS_UNTIL_EMPTY 15  - whatever number. Not defined = unlimited shots.
  #define BLASTER_JAM_PERCENTAGE        - If this is not defined, random from 0-100%.
  #define BLASTER_DEFAULT_MODE          - MODE_STUN|MODE_KILL|MODE_AUTO. Not defined defaults to MODE_KILL.

Depending on the install, one of the following orientation defines might be appropiate to use:
  #define ORIENTATION ORIENTATION_FETS_TOWARDS_BLADE // default
  #define ORIENTATION ORIENTATION_USB_TOWARDS_BLADE
  #define ORIENTATION ORIENTATION_TOP_TOWARDS_BLADE
  #define ORIENTATION ORIENTATION_BOTTOM_TOWARDS_BLADE
  #define ORIENTATION ORIENTATION_USB_CCW_FROM_BLADE // This means the BATT(+) pad side.
  #define ORIENTATION ORIENTATION_USB_CW_FROM_BLADE // This means the speaker pads side.
  #define ORIENTATION ORIENTATION_SDA_TOWARDS_BLADE // Previous version of USB_CCW_FROM_BLADE.
  #define ORIENTATION ORIENTATION_SERIAL_TOWARDS_BLADE // Previous version of USB_CW_FROM_BLADE.

Optional Blade style elements:
  EFFECT_DESTRUCT                       - Use in blade style to animate self destruct.
  EFFECT_BATTERY_LEVEL                  - Use EFFECT_BATTERY_LEVEL in blade style,
                                            and uses battery.wav sound effect.
  EFFECT_VOLUME_LEVEL                   - Use EFFECT_VOLUME_LEVEL in blade style.
                                            Useful if the blaster has a segmented accent LED strip or similar.
==================================================
| 2 Buttons: FIRE and MODE (saber's POW and AUX) |
==================================================
Power On / Off            - Hold MODE then Press and Hold FIRE until Power ON / OFF.
                              Default is auto-powered ON at boot.
                              Power ON required after Self Destruct.
                              Plays wavs named poweron and poweroff.
Cycle Modes               - Click and Hold MODE. Cycles through KILL, AUTOFIRE, and STUN modes.
Next Preset               - Long Click and release MODE (while NOT pointing UP or DOWN).
Previous Preset           - Long Click and release MODE (while pointing DOWN).
Jump to First Preset      - Long Click and release MODE (while pointing UP).
Start/Stop Track          - 4x Click MODE.
Volume Menu:
  * Note - must exit Volume Menu before any other functions will work.
        Enter Menu        - Hold MODE + Click FIRE.
        Volume UP         - Rotate Right.
        Volume DOWN       - Rotate Left.
        Quick MAX Vol     - Hold FIRE while in Volume Menu.
        Quick MIN Vol     - Double Click and Hold FIRE while in Volume Menu.
        Save and Exit     - Click FIRE.
        Cancel and Exit   - Double Click FIRE.
Spoken Battery Level
        in percentage     - 3x Click and Hold POW.
        in volts          - 3x Click and Hold POW (while pointing DOWN).
                            * Will show On-blade display if EFFECT_BATTERY_LEVEL is used in blade style.
On-Demand Batt Level      - 3x Click and Hold POW, release after a second. (Double Click then Long Click)
                            * Requires EFFECT_BATTERY_LEVEL to be in blade style.
                            * Plays battery.wav sound effect if it exists in font or common folder,
                              otherwise a familiar tune of beeps :)

Self-Destruct             - Hold FIRE then Press and Hold MODE until overload starts....then run!
                            Self-Destructed blaster needs to be powered on manually to use again.
Quote Player              - Double Click MODE (while NOT pointing DOWN). (requires quote.wavs in font.)
Toggle quotes playback,
  random or sequential    - Double Click MODE (while pointing DOWN).
Reload                    - Click MODE.
Fire                      - Click FIRE. (Hold to Auto / Rapid Fire when AUTO mode selected.)
Clip In                   - Clip Detect pad Latched On.
Clip out                  - Clip Detect pad Latched Off.
Unjam                     - Bang the blaster or Reload.

=======================================
| 3 Buttons: FIRE, MODE, RELOAD/POWER |
=======================================
- If defined RELOAD_BUTTON_IS_POWER
  Power ON/OFF              - HOLD RELOAD until power ON/OFF.
  Reload                    - Click MODE.
  Cycle Modes               - HOLD MODE. Cycles through KILL, AUTOFIRE, and STUN modes.

- Otherwise (Reload Button is NOT Power)
  Reload                    - Click RELOAD.
  Cycle Modes               - Click MODE.


Wavs to use for talking Mode (else Talkie voice):
- If these are not present, mode.wav will be used for all modes.
- If no mode.wav either, then Talkie voice speaks selected mode.
  mdstun.wav
  mdkill.wav
  mdauto.wav
Additionally:
  quote.wav      // for playing quotes
  in.wav         // for blaster power off
  out.wav        // for blaster power on
  destruct.wav   // for self destruct overload
  boom.wav       // for self destruct explosion
These can be used to match a more "plasmatic" stun sound.
  clipins.wav
  clipouts.wav
  reloads.wav

The following are likely best kept in a 'common' folder in your Font Search Path, but of course
can be specific versions within a font as well.
Choose a Voice Pack containing all these sounds here: https://fredrik.hubbe.net/lightsaber/sound/

  Battery level wavs:
    mzero.wav
    mnum 1-20 wavs, as well as the "tens" thirty.wav - ninety.wav
    battery.wav    // for accompanying EFFECT_BATTERY_LEVEL
    battlevl.wav   // for Spoken Battery Level
    mpercent.wav
    mpoint.wav
    mvolts.wav
  vmbegin.wav    // for Begin Volume Menu
  vmend.wav      // for End Volume Menu
  volup.wav      // for increse volume
  voldown.wav    // for decrease volume
  volmin.wav     // for minimum volume reached
  volmax.wav     // for maximum volume reached
  mfalse.wav     // "disabled" used for sequential quote mode
  mtrue.wav      // "enabled" used for sequential quote mode
  mrandom.wav    // "ramdom" used for sequential quote mode
*/

#ifndef PROPS_BLASTER_BC_BUTTONS_H
#define PROPS_BLASTER_BC_BUTTONS_H

#ifndef BLASTER_DEFAULT_MODE
#define BLASTER_DEFAULT_MODE MODE_KILL
#endif

#include "blaster.h"
#include "../sound/sound_library.h"
#include "../modes/settings_menues.h"

#ifdef PROP_TYPE
#undef PROP_TYPE
#endif

#define PROP_TYPE BlasterBCButtons


// Additional effects for BC buttons prop.
EFFECT(clipins);    // s for stun mode
EFFECT(clipouts);   // s for stun mode
EFFECT(reloads);    // s for stun mode
EFFECT(destruct);
#ifndef PROPS_DUAL_PROP_H
EFFECT(battery);    // for EFFECT_BATTERY_LEVEL
EFFECT(vmbegin);    // for Begin Volume Menu
EFFECT(vmend);      // for End Volume Menu
EFFECT(volup);      // for increse volume
EFFECT(voldown);    // for decrease volume
EFFECT(volmin);     // for minimum volume reached
EFFECT(volmax);     // for maximum volume reached
#endif

class BlasterBCButtons : public Blaster {
public:
  BlasterBCButtons() : Blaster() {}
  const char* name() override { return "BlasterBCButtons"; }

  void EnterVolumeMenu() {
        sound_library_.SayEditVolume();
        pushMode<MKSPEC<MENU_SPEC_TEMPLATE>::ChangeVolumeMode>();
  }

  void Fire() override {
    DoJam();
    if (blaster_mode != MODE_AUTO && DoEmpty()) return;

    switch (blaster_mode) {
      case MODE_STUN:
        SaberBase::DoEffect(EFFECT_STUN, 0);
        shots_fired_++;
        PVLOG_NORMAL << "******** STUN - Remaining shots = " << GetBulletCount() << "\n";
        break;
      case MODE_KILL:
        SFX_blast.Select(-1);
        SaberBase::DoEffect(EFFECT_FIRE, 0);
        shots_fired_++;
        PVLOG_NORMAL << "******** FIRE - Remaining shots = " << GetBulletCount() << "\n";
        break;
      case MODE_AUTO:
        if (!CheckEmpty()) {
          SelectAutoFirePair();
          auto_firing_ = true;
          SaberBase::SetLockup(LOCKUP_AUTOFIRE);
          SaberBase::DoBeginLockup();
        }
        break;
    }
  }

 void DoAutoFire() override {
      // First time through, let bgnauto finish first if we have one
    if (SFX_bgnauto && GetWavPlayerPlaying(&SFX_bgnauto)) return;

    // No bgnauto or bgnauto has finished playing, so initialize and start auto-firing
    if (auto_firing_ && !auto_player_) {
      auto_player_ = GetWavPlayerPlaying(&SFX_auto);
      shots_fired_++;
      auto_time_ = millis();
      PVLOG_NORMAL << "******** AUTOFIRING - Remaining shots = " << GetBulletCount() << "\n";
    }
    // use auto.wav's length to count shots
    if (auto_firing_ && auto_player_ && (millis() - auto_time_) > auto_sound_length) {
      auto_sound_length = auto_player_->length() * 1000;
      shots_fired_++;
      if (CheckEmpty()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_firing_ = false;
        auto_player_.Free();
        auto_time_ = millis(); // Reset for the first empty sound
        return;
      }
      auto_time_ = millis();
      PVLOG_NORMAL << "******** AUTOFIRING - Remaining shots = " << GetBulletCount() << "\n";
    }

    // This will mimic the hammer running on no bullets at the same rate it was firing.
    // if there are different length auto.wavs, the last played's length is used.
    if (!auto_firing_ && CheckEmpty() && trigger_is_pressed_ && (millis() - auto_time_) > auto_sound_length) {
      SaberBase::DoEffect(EFFECT_EMPTY, 0);
      auto_time_ = millis(); // Reset for the next empty sound
    }
 }

  void Loop() override {
    PropBase::Loop();
    PollNextAction();
    sound_library_.Poll(wav_player);
    if (wav_player && !wav_player->isPlaying()) wav_player.Free();

    if (blaster_mode == MODE_AUTO) {
      DoAutoFire();
    }
  }

  void Reload() override {
    Blaster::Reload();
    is_jammed_ = false;
  }

  void ClipOut() override {
    Blaster::ClipOut();
    no_clip_ = true;
  }

  void ClipIn() override {
    Blaster::ClipIn();
    is_jammed_ = false;
    no_clip_ = false;
  }

  // Pull in parent's SetPreset, but turn the blaster on.
  void SetPreset(int preset_num, bool announce) override {
    PropBase::SetPreset(preset_num, announce);
      // Just always default to on.
       if (!SaberBase::IsOn()) On();
  }

  void selfDestruct() override {
    Blaster::selfDestruct();
    SaberBase::DoEffect(EFFECT_DESTRUCT, 0);
    SetNextActionF(NEXT_ACTION_BLOW, hybrid_font.GetCurrentEffectLength());
  }

  // Clash to unjam or Enter/Exit Volume Menu.
  void Clash(bool stab, float strength) override {
    PropBase::Clash(stab, strength);
    if (is_jammed_) {
      is_jammed_ = false;
      SaberBase::DoEffect(EFFECT_UNJAM, 0);
    }
  }

  // Previous, next, or first preset, depending on angle
  void DoChangePreset() {
    if (fusor.angle1() > M_PI / 3) {
    // Muzzle pointing UP
      first_preset();
      PVLOG_NORMAL << "** Jumped to first preset\n";
    } else if (fusor.angle1() < -M_PI / 3) {
    // Muzzle pointing DOWN
      previous_preset();
      PVLOG_NORMAL << "** Previous preset\n";
    } else {
    // Muzzle NOT pointing UP or DOWN
      next_preset();
      PVLOG_NORMAL << "** Next preset\n";
    }
  }

  void DoSpokenBatteryLevel() {
    // Avoid weird battery readings when using USB
    if (battery_monitor.battery() < 0.5) {
      sound_library_.SayTheBatteryLevelIs();
      sound_library_.SayDisabled();
      return;
    }
    sound_library_.SayTheBatteryLevelIs();
    // pointing DOWN
    if (fusor.angle1() < -M_PI / 4) {
      sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
      sound_library_.SayVolts();
      PVLOG_NORMAL << "Battery Voltage: " << battery_monitor.battery() << "\n";
      is_speaking_ = true;
      SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
    } else {
      sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
      sound_library_.SayPercent();
      PVLOG_NORMAL << "Battery Percentage: " <<battery_monitor.battery_percent() << "%\n";
      is_speaking_ = true;
      SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
    }
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

#ifdef RELOAD_BUTTON_IS_POWER
// Power button control
      case EVENTID(BUTTON_RELOAD, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
        On();
        return true;

      case EVENTID(BUTTON_RELOAD, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        Off();
        return true;
#endif

// Power On / Off manually
      case EVENTID(BUTTON_FIRE, EVENT_FIRST_HELD_LONG, MODE_OFF | BUTTON_MODE_SELECT):
        if (!hybrid_font.PlayPolyphonic(&SFX_out)) {  // Are we doing poweron.wavs instead??
          beeper.Beep(0.1, 300);
          beeper.Beep(0.1, 600);
          beeper.Beep(0.1, 900);
          beeper.Beep(0.1, 1300);
        }
        On();
        return true;
      case EVENTID(BUTTON_FIRE, EVENT_FIRST_HELD_LONG, MODE_ON | BUTTON_MODE_SELECT):
        if (!hybrid_font.PlayPolyphonic(&SFX_in)) {  // Are we doing poweroff.wavs instead??
          beeper.Beep(0.1, 1300);
          beeper.Beep(0.1, 900);
          beeper.Beep(0.1, 600);
          beeper.Beep(0.1, 300);
        }
        Off();
        return true;

// Cycle Mode
#if NUM_BUTTONS == 3 && !defined(RELOAD_BUTTON_IS_POWER)
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
#else
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
#endif
        NextBlasterMode();
        return true;

// Change Preset
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        DoChangePreset();
        return true;

// Start or Stop Track
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):
        StartOrStopTrack();
        return true;

// Enter Menu
      case EVENTID(BUTTON_FIRE, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON | BUTTON_MODE_SELECT):
      case EVENTID(BUTTON_FIRE, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF | BUTTON_MODE_SELECT):
        EnterVolumeMenu();
        return true;

// Spoken Battery Level in percentage
// Spoken Battery Level in volts - pointing DOWN
// Show Battery Meter on LEDs
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_HELD_MEDIUM, MODE_OFF):
        DoSpokenBatteryLevel();
        return true;

// On Demand Battery Level
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_CLICK_LONG, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_CLICK_LONG, MODE_OFF):
        PVLOG_NORMAL << "Battery Voltage: " << battery_monitor.battery() << "\n";
        PVLOG_NORMAL << "Battery Percentage: " <<battery_monitor.battery_percent() << "\n";
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

// Reload
#if NUM_BUTTONS == 3 && !defined(RELOAD_BUTTON_IS_POWER)
      case EVENTID(BUTTON_RELOAD, EVENT_PRESSED, MODE_ON):
#else
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
#endif
        if (no_clip_) {
          SaberBase::DoEffect(EFFECT_EMPTY, 0); 
          return true;
        } else {
          Reload();
        }
        return true;

// Fire
      case EVENTID(BUTTON_FIRE, EVENT_PRESSED, MODE_ON):
        if (no_clip_) {
          SaberBase::DoEffect(EFFECT_EMPTY, 0); 
          return true;
        } else {
          Fire();
          trigger_is_pressed_ = true;
          return true;
        }

      case EVENTID(BUTTON_FIRE, EVENT_RELEASED, MODE_ON):
        if (blaster_mode == MODE_AUTO) {
          if (SaberBase::Lockup()) {
            SaberBase::DoEndLockup();
            SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
            auto_firing_ = false;
            auto_player_.Free();
          }
        }
        trigger_is_pressed_ = false;
        return true;



// Self-Destuct / overload
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_HELD_LONG, MODE_ON | BUTTON_FIRE):
        selfDestruct();
        return true;

// Quote
// Toggle Sequential Quote - pointing down
// QuickMin volume
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        // pointing down
        if (fusor.angle1() < - M_PI / 4) {
          sequential_quote_ = !sequential_quote_;
          sound_library_.SayRandom();
          if (sequential_quote_) {
            sound_library_.SayDisabled();
          } else {
            sound_library_.SayEnabled();
          }
          return true;
        } else if (sequential_quote_) {
            SFX_quote.SelectNext();
          } else {
            SFX_quote.Select(-1);
          }
        hybrid_font.PlayCommon(&SFX_quote);
        return true;

// Clip inserted
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_ON, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_PRESSED, MODE_OFF):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_ON, MODE_OFF):
        ClipIn();
        return true;

// Clip removed
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_RELEASED, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_RELEASED, MODE_OFF):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_OFF, MODE_OFF):
        ClipOut();
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
    }
    return false;
  }

  // Blaster effects, auto fire is handled by begin/end lockup
  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;
      case EFFECT_STUN: hybrid_font.PlayCommon(&SFX_stun); return;
      case EFFECT_FIRE: hybrid_font.PlayCommon(&SFX_blast); return;
      case EFFECT_CLIP_IN: 
        if (blaster_mode == MODE_STUN && SFX_clipins) {
          hybrid_font.PlayCommon(&SFX_clipins); return;
        } else {
          hybrid_font.PlayCommon(&SFX_clipin); return;
        }
      case EFFECT_CLIP_OUT: 
        if (blaster_mode == MODE_STUN && SFX_clipouts) {
          hybrid_font.PlayCommon(&SFX_clipouts); return;
        } else { 
          hybrid_font.PlayCommon(&SFX_clipout); return;
        }
      case EFFECT_RELOAD:
        if (shots_fired_ == 0 && SFX_full) {
          hybrid_font.PlayCommon(&SFX_full); return;
        } else if (blaster_mode == MODE_STUN && SFX_reloads) {
          hybrid_font.PlayCommon(&SFX_reloads); return;
        } else {
          hybrid_font.PlayCommon(&SFX_reload); return;
        }
      case EFFECT_MODE: SayMode(); return;
      case EFFECT_RANGE: hybrid_font.PlayCommon(&SFX_range); return;
      case EFFECT_EMPTY: hybrid_font.PlayCommon(&SFX_empty); return;
      case EFFECT_FULL: hybrid_font.PlayCommon(&SFX_full); return;
      case EFFECT_JAM: hybrid_font.PlayCommon(&SFX_jam); return;
      case EFFECT_UNJAM: hybrid_font.PlayCommon(&SFX_unjam); return;
      case EFFECT_PLI_ON: hybrid_font.PlayCommon(&SFX_plion); return;
      case EFFECT_PLI_OFF: hybrid_font.PlayCommon(&SFX_plioff); return;
      case EFFECT_DESTRUCT: hybrid_font.PlayCommon(&SFX_destruct); return;
      case EFFECT_BATTERY_LEVEL:
        if (is_speaking_) {
          is_speaking_ = false;
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
        }
    }
  }

private:
  bool no_clip_ = false;
  bool is_speaking_ = false; // Prevent overlap
  bool sequential_quote_ = false;
  bool trigger_is_pressed_ = false;
  int auto_sound_length = 0;
  uint32_t auto_time_;
  RefPtr<BufferedWavPlayer> auto_player_;
  RefPtr<BufferedWavPlayer> wav_player;

};


#endif  // PROPS_BLASTER_BC_H
