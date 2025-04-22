/*
blaster_BC_buttons.h prop file.
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2025 Fredrik Hubinette
  Copyright (c) 2025 Brian Conner with contributions by:
  Fredrik Hubinette, Fernando da Rosa, Matthew McGeary, and Scott Weber.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/

Includes:
- Volume Menu with QuickMAX and QuickMIN.
- Spoken battery level in both volts or percentage.
- On-Demand battery level (Shows Green to Red if EFFECT_BATTERY_LEVEL is used in Blade Style).
- Quote player with On-the-fly sequential option.
- Next / previous preset.
- Self-Destuct overload.
- Defaults to KILL on boot.
- Option to use separate effect sounds to match the current mode.
    By using alt sound folders in the font, each mode can have better matched effects
    (reload, full, clipin/clipout, etc...).
    Sounds for the modes should go in the following alt folders:
    - STUN mode       - alt000
    - KILL mode       - alt001
    - AUTO mode       - alt002

- No jam if empty.
- AUTOFIRE mode is enabled simply by having auto.wav files available in the sound font.
    This mode also counts bullets and causes an "empty clip" if you use
    #define BLASTER_SHOTS_UNTIL_EMPTY.
    Each shot is timed to the length of the wav file that's playing, and 
    the empty sound repeats at same rate auto was firing for added realism. Cool, eh?

This prop version requires a ProffieOS Voicepack for menus to work right.

Button assignment:
This prop requires a minimum of 2 buttons for operation.
If there's a 3rd button installed, it should be set in the config as:
Button ReloadButton(BUTTON_RELOAD, aux2Pin, "reload");
It's coded to be Reload by default.
However, to use it as a dedicated POWER ON/OFF button instead, use the following define:
#define RELOAD_BUTTON_IS_POWER
The blaster defaults to auto-powered ON at boot and on Change Preset.
To use the RELOAD button as a POWER button to initially power ON instead,
use a poweron.wav file in the font.

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
Power Blaster ON          - Hold MODE.
Power Blaster OFF         - Hold FIRE then Double Click MODE.
                              Default is auto-powered ON at Boot and on Change Preset.
                              Power ON required after Self Destruct.
                              Plays out.wavs for power ON and in.wavs for power OFF.
Cycle Modes               - Hold MODE. Cycles through KILL, AUTOFIRE, and STUN modes.
Change Preset             - Long Click and release MODE.
        Next                  (while NOT pointing UP or DOWN).
        Previous              (while pointing DOWN).
        Jump to First         (while pointing UP).
Start/Stop Track          - 4x Click MODE.
Volume Menu:
  * Note - must exit Volume Menu before any other functions will work.
        Enter Menu        - Hold MODE then Click FIRE.
        Volume UP         - Rotate Right.
        Volume DOWN       - Rotate Left.
        Quick MAX Vol     - Hold FIRE.
        Quick MIN Vol     - Hold MODE.
        Save and Exit     - Click FIRE.
        Cancel and Exit   - Click MODE.
Spoken Battery Level
        in percentage     - 3x Click and Hold MODE.
        in volts          - 3x Click and Hold MODE (while pointing DOWN).
                            * Will show On-blade display if EFFECT_BATTERY_LEVEL is used in blade style.
On-Demand Batt Level      - 3x Click and Hold MODE, release after a second. (Double Click then Long Click)
                            * Requires EFFECT_BATTERY_LEVEL to be in blade style.
                            * Plays battery.wav sound effect if it exists in font or common folder,
                              otherwise a familiar tune of beeps :)

Self-Destruct             - Hold MODE then Hold FIRE until overload starts....then run!
                            Self-Destructed blaster needs to be powered ON manually to use again.
Quote Player              - Double Click MODE (while NOT pointing DOWN). (requires quote.wavs in font.)
Toggle quotes playback,
  random or sequential    - Double Click MODE (while pointing DOWN).
Reload                    - Click MODE.
Fire                      - Click FIRE. (Hold to Auto / Rapid Fire when AUTO mode selected.)
Clip In                   - Clip Detect pad Latched On.
Clip out                  - Clip Detect pad Latched Off.
Unjam                     - Bang the blaster or Reload, or Change Preset.

=================================
| 3 Buttons: FIRE, MODE, RELOAD |
=================================
*** Controls are the same as 2 button above, but with these changes ***

Reload                    - Click RELOAD.
Cycle Modes               - Click MODE.
Change Preset             - Hold MODE
        Next                  (while NOT pointing UP or DOWN).
        Previous              (while pointing DOWN).
        Jump to First         (while pointing UP).

- If defined RELOAD_BUTTON_IS_POWER
  Power ON/OFF              - Hold RELOAD until power ON/OFF.


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

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 1200
#endif

// Additional effects for BC buttons prop.
EFFECT(destruct);
#ifndef PROPS_DUAL_PROP_H
EFFECT(battery);    // for EFFECT_BATTERY_LEVEL
#endif

template<class SPEC>
struct BCBlasterVolumeMode : public SPEC::SteppedMode {
  const int max_volume_ = VOLUME;
  const int min_volume_ = VOLUME * 0.10;
  float initial_volume_ = 0.0;
  int initial_percentage_ = 0;
  int percentage_ = 0;

  void mode_activate(bool onreturn) override {
    PVLOG_NORMAL << "** Enter Volume Menu\n";
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
    PVLOG_NORMAL << "** Maximum Volume\n";
    mode::getSL<SPEC>()->SayMaximumVolume();
  }

  void QuickMinVolume() {
    dynamic_mixer.set_volume(min_volume_);
    PVLOG_NORMAL << "** Minimum Volume\n";
    mode::getSL<SPEC>()->SayMinimumVolume();
  }

  void update() override {  // Overridden to substitute the tick sound
    float volume = dynamic_mixer.get_volume();
    percentage_ = round((volume / max_volume_) * 10) * 10;
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
  }

  void select() override {
    PVLOG_NORMAL << "** Saved - Exiting Volume Menu\n";
    mode::getSL<SPEC>()->SaySave();
    SPEC::SteppedMode::select();
  }

  void exit() override {
    PVLOG_NORMAL << "** Cancelled - Exiting Volume Menu\n";
    percentage_ = initial_percentage_;
    dynamic_mixer.set_volume(initial_volume_);
    mode::getSL<SPEC>()->SayCancel();
    SPEC::SteppedMode::exit();
  }

// For Blasters
  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, 0)) {

      case EVENTID(BUTTON_FIRE, EVENT_FIRST_HELD_MEDIUM, 0):  // fire m1
        QuickMaxVolume();
        return true;
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_HELD_MEDIUM, 0):  // mode m1
        QuickMinVolume();
        return true;

      case EVENTID(BUTTON_FIRE, EVENT_FIRST_SAVED_CLICK_SHORT, 0):  // fire s1
        select();
        return true;
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, 0):  // mode s1
        exit();
        return true;
    }
    return SPEC::SelectCancelMode::mode_Event2(button, event, modifiers);
  }
};

template<class SPEC>
struct BCBlasterMenuSpec {
  typedef BCBlasterVolumeMode<SPEC> BCBlasterVolumeMenu;
  typedef mode::SelectCancelMode SelectCancelMode;
  typedef mode::SteppedMode<SPEC> SteppedMode;
  typedef mode::SteppedModeBase<SPEC> SteppedModeBase;
  typedef mode::MenuBase<SPEC> MenuBase;
  typedef SoundLibraryV2 SoundLibrary;
};

class BlasterBCButtons : public Blaster {
public:
  BlasterBCButtons() : Blaster() {}
  const char* name() override { return "BlasterBCButtons"; }

  void Setup() override {
    MKSPEC<BCBlasterMenuSpec>::SoundLibrary::init();
  }

  void EnterVolumeMenu() {
    pushMode<MKSPEC<BCBlasterMenuSpec>::BCBlasterVolumeMenu>();
  }

  void Fire() override {
    // Can't use OR here otherwise empty will play even when jammed
    if (DoEmpty()) return;
    if (DoJam()) return;

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
    if (!SFX_auto) return;
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
#ifdef BLASTER_SHOTS_UNTIL_EMPTY
    if (max_shots_ != -1 && (GetBulletCount() == BLASTER_SHOTS_UNTIL_EMPTY)) {
      SaberBase::DoEffect(EFFECT_FULL, 0);
    } else {
      Blaster::Reload();
    }
    is_jammed_ = false;
#endif
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

  void selfDestruct() override {
    Blaster::selfDestruct();
    SaberBase::DoEffect(EFFECT_DESTRUCT, 0);
    SetNextActionF(NEXT_ACTION_BLOW, hybrid_font.GetCurrentEffectLength());
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

  void DoQuote() {
    if (SFX_quote) {
      if (GetWavPlayerPlaying(&SFX_quote)) return;  // Simple prevention of quote overlap
      sequential_quote_ ? SFX_quote.SelectNext() : SFX_quote.Select(-1);
      hybrid_font.PlayCommon(&SFX_quote);
    }
  }

  void ToggleSequentialQuote() {
    sequential_quote_ = !sequential_quote_;
    PVLOG_NORMAL << (sequential_quote_ ? "** Quotes play sequentially\n" : "** Quotes play randomly\n");

    if (!SFX_mnum) {  // No voice pack available
      beeper.Beep(0.5, sequential_quote_ ? 3000 : 1000);
      return;
    } else {
      if (sequential_quote_) {
        sound_library_v2.SaySequential();
      } else {
        sound_library_.SayRandom();
      }
    }
  }

  void TurnPowerOn() {
    if (!hybrid_font.PlayPolyphonic(&SFX_poweron)) {
      if (!hybrid_font.PlayPolyphonic(&SFX_out)) {
        beeper.Beep(0.1, 300);
        beeper.Beep(0.1, 600);
        beeper.Beep(0.1, 900);
        beeper.Beep(0.1, 1300);
      }
    }
    On();
  }

  void TurnPowerOff() {
    if (!hybrid_font.PlayPolyphonic(&SFX_in)) {  // Are we doing poweroff.wavs instead??
      beeper.Beep(0.1, 1300);
      beeper.Beep(0.1, 900);
      beeper.Beep(0.1, 600);
      beeper.Beep(0.1, 300);
    }
    Off();
  }

  void ResetCurrentAlternative() override {
    // If no auto.wav files found, fall back to STUN.
    if (blaster_mode == MODE_AUTO && !SFX_auto) blaster_mode = MODE_STUN;
    current_alternative = blaster_mode;
  }

  void SetBlasterMode(BlasterMode to_mode) override {
    Blaster::SetBlasterMode(to_mode);
    SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, to_mode);
  }

  void NextBlasterMode() override {
    switch(blaster_mode) {
      case MODE_STUN:
        SetBlasterMode(MODE_KILL);
        return;
      case MODE_KILL:
        if (SFX_auto) {
          SetBlasterMode(MODE_AUTO);
        } else {
          SetBlasterMode(MODE_STUN);
        }
        return;
      case MODE_AUTO:
        SetBlasterMode(MODE_STUN);
        return;
    }
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

#ifdef RELOAD_BUTTON_IS_POWER
  // Power ON / OFF manually
      case EVENTID(BUTTON_RELOAD, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
        TurnPowerOn();
        return true;

      case EVENTID(BUTTON_RELOAD, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        TurnPowerOff();
        return true;
#else
  // Power ON / OFF manually
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
        TurnPowerOn();
        return true;

      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON | BUTTON_FIRE):
        TurnPowerOff();
        return true;
#endif

#if NUM_BUTTONS == 3 && !defined(RELOAD_BUTTON_IS_POWER)
  // Cycle Mode
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        NextBlasterMode();
        return true;

  // Change Preset
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        DoChangePreset();
        return true;
#else
  // Cycle Mode
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_HELD_LONG, MODE_ON):
        NextBlasterMode();
        return true;

  // Change Preset
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        DoChangePreset();
        return true;
#endif

  // Start or Stop Track
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):
        StartOrStopTrack();
        return true;

  // Enter Menu (mode fire s1)
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
      case EVENTID(BUTTON_FIRE, EVENT_FIRST_HELD_LONG, MODE_ON | BUTTON_MODE_SELECT):
        selfDestruct();
        return true;

  // Quote
  // Toggle Sequential Quote - pointing down
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        if (fusor.angle1() < -M_PI / 4) {
          // pointing DOWN
          ToggleSequentialQuote();
        } else {
          DoQuote();
        }
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

  void SB_Effect(EffectType effect, EffectLocation location) override {
    Blaster::SB_Effect(effect, location);
    switch (effect) {
      default: return;
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
