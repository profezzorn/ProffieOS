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
- Next / previous preset
- Self-Destuct overload.
- Defaults to KILL on boot.
- Separate clipin/clipout/reload sounds to match stun mode.
    (requires clipins, clipouts, and reloads.wavs in font)
- No jam if empty.
- Autofire mode counts bullets and causes empty if
    #define BLASTER_SHOTS_UNTIL_EMPTY is specified.

Requires minimum 2 buttons for operation
*note - config can use NUM_BUTTONS 1 for saber mode when using dual_prop.
      - the blaster mode will still use 2 buttons as FIRE and MODE.

** This version is designed to work well together with saber_BC_buttons.h when
using dual_prop.h: a toggle between 2 prop files triggered by blade detect.
Blade insertion and removal switches between the 2 props, (ie: Ezra blaster and saber.)
Your config file should leave SABER buttons in the CONFIG_BUTTONS section, like:
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");

For a stand-alone blaster setup, your config file should use BLASTER buttons in 
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
  #define ORIENTATION ORIENTATION_SDA_TOWARDS_BLADE // This means the SDA pad
  #define ORIENTATION ORIENTATION_SERIAL_TOWARDS_BLADE // This means the Serial pad (same side as speakers)

Optional Blade style elements:
  EFFECT_DESTRUCT                       - use in blade style to animate self destruct.
  EFFECT_BATTERY LEVEL                  - requires EFFECT_BATTERY_LEVEL to be in blade style,
                                          and uses battery.wav sound effect.

==========================================
| 2 Buttons: FIRE and MODE (POW and AUX) |
==========================================
Power On / Off            - Hold MODE, double click and hold FIRE until Power On / Off (Default is auto-power ON)
Cycle Modes               - Click MODE when ON. Cycles through KILL, AUTOFIRE, and STUN modes.
Next Preset               - Long click and release MODE.
Previous Preset           - Double click and hold MODE, release after a second.
Start/Stop Track          - 4x click MODE.
Volume Menu:
            Enter/Exit    - 4x click and hold MODE while NOT pointing down.
            Volume UP     - Long click and release MODE while in Volume Menu. (just like next preset)
            Volume DOWN   - Double click and hold MODE, release after a second while in Volume Menu.
                            (click then long click, just like next preset)
            Quick MAX Vol - Short click MODE while in Volume Menu.
            Quick MIN Vol - Double click MODE while in Volume Menu.
Spoken Battery Level:                                                       // TODO: OLED to display PLI
            in volts      - Triple click MODE.
            in percentage - Triple click and hold MODE.

On-Demand Batt Level      - Double click and hold MODE until battery level shows and sound plays.
                            Requires EFFECT_BATTERY_LEVEL to be in blade style in order to see it,
                            and uses battery.wav sound effect, otherwise, special beep :)

Self-Destruct             - Press and hold MODE and FIRE until overload starts....then run! 
                            Self-Destructed blaster needs to be powered on manually to use again.
Quote Player              - Double click MODE while NOT pointing down. (requires quote.wavs in font)
Toggle quotes
  random or sequential    - Double click MODE while pointing down.
Reload                    - Click MODE.
Fire                      - Click FIRE. (Hold to Auto / Rapid Fire when AUTO mode selected)
Clip In                   - Clip Detect pad Latched On.
Clip out                  - Clip Detect pad Latched Off.
Unjam                     - Bang the blaster or Reload.

- If there's a dedicated POWER button,
Power On / Off            - Click POW. - TODO: this with dual_prop is....weird
- If there's a dedicated Relod button, 
Reload                    - Click RELOAD.

-TODO: PLI_ON / OFF, CLIP IN etc....

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

#ifndef PROPS_BLASTER_BC_H
#define PROPS_BLASTER_BC_H

#ifndef BLASTER_DEFAULT_MODE
#define BLASTER_DEFAULT_MODE MODE_KILL
#endif
#include "prop_base.h"
#include "../sound/sound_library.h"

#define PROP_TYPE BlasterBC
#define PROP_HAS_BULLET_COUNT

EFFECT(clipin);
EFFECT(clipins);    // s for stun mode
EFFECT(clipouts);   // s for stun mode
EFFECT(clipout);
EFFECT(empty);
EFFECT(full);
EFFECT(jam);
EFFECT(mode);
EFFECT(plioff);
EFFECT(plion);
EFFECT(range);
EFFECT(reload);
EFFECT(reloads);    // s for stun mode
EFFECT(stun);
EFFECT(unjam);
EFFECT(mdstun);
EFFECT(mdkill);
EFFECT(mdauto);
EFFECT(destruct);
// For mode sounds, specific "mdstun", "mdkill", and "mdauto" may be used.
// If just a single "mode" sound for all switches exists, that will be used.
// If no mode sounds exist in the font, a talkie version will speak the mode on switching.
#ifndef PROPS_DUAL_PROP_H
EFFECT(battery);    // for EFFECT_BATTERY_LEVEL
EFFECT(vmbegin);    // for Begin Volume Menu
EFFECT(vmend);      // for End Volume Menu
EFFECT(volup);      // for increse volume
EFFECT(voldown);    // for decrease volume
EFFECT(volmin);     // for minimum volume reached
EFFECT(volmax);     // for maximum volume reached
EFFECT(quote);      // for playing quotes
#endif

class BlasterBC : public PROP_INHERIT_PREFIX PropBase {
public:
  BlasterBC() : PropBase() {}
  const char* name() override { return "BlasterBC"; }

  // Mode states to handle kill vs stun effects
  enum BlasterMode {
    MODE_STUN,
    MODE_KILL,
    MODE_AUTO
  };

  BlasterMode blaster_mode = BLASTER_DEFAULT_MODE;

  virtual void SetBlasterMode(BlasterMode to_mode) {
    if (!auto_firing_) {
      blaster_mode = to_mode;
      SaberBase::DoEffect(EFFECT_MODE, 0);
    }
  }

  virtual void NextBlasterMode() {
    switch(blaster_mode) {
      case MODE_STUN:
        stun_mode_ = false;
        SetBlasterMode(MODE_KILL);
        return;
      case MODE_KILL:
#if defined (ENABLE_BLASTER_AUTO) || defined (BLASTER_ENABLE_AUTO)
        stun_mode_ = false;
        SetBlasterMode(MODE_AUTO);
#else
        stun_mode_ = true;
        SetBlasterMode(MODE_STUN);
#endif
        return;
      case MODE_AUTO:
        stun_mode_ = true;
        SetBlasterMode(MODE_STUN);
        return;
    }
  }

  int shots_fired_ = 0;
  bool auto_firing_ = false;
  bool is_jammed_ = false;
  bool mode_volume_ = false;
  bool stun_mode_ = false;
  bool no_clip_ = false;
  // Avoid overlap of battery.wav when doing Spoken Battery Level
  bool is_speaking_ = false;
  bool sequential_quote_ = false;
  bool trigger_is_pressed_ = false;

  int auto_sound_length = 0;

#ifdef BLASTER_SHOTS_UNTIL_EMPTY
  const int max_shots_ = BLASTER_SHOTS_UNTIL_EMPTY;
#else
  const int max_shots_ = -1;
#endif

  virtual bool CheckJam(int percent) {
    int random = rand() % 100;
    return random < percent;
  }

  virtual int GetBulletCount() {
    return max_shots_ - shots_fired_;
  }

  virtual bool IsEmpty() const {
    return max_shots_ != -1 && shots_fired_ >= max_shots_;
  }


uint32_t auto_time_;

RefPtr<BufferedWavPlayer> auto_player_;

virtual void Fire() {
  if (IsEmpty() && blaster_mode != MODE_AUTO) {
    SaberBase::DoEffect(EFFECT_EMPTY, 0);
    return;
  }
#ifdef ENABLE_MOTION
#ifdef BLASTER_JAM_PERCENTAGE
  // If we're already jammed then we don't need to recheck. If we're not jammed then check if we just jammed.
  is_jammed_ = is_jammed_ ? true : CheckJam(BLASTER_JAM_PERCENTAGE);
  if (is_jammed_) {
    SaberBase::DoEffect(EFFECT_JAM, 0);
    return;
  }
#endif
#endif
  if (blaster_mode == MODE_STUN) {
    SaberBase::DoEffect(EFFECT_STUN, 0);
    shots_fired_++;
    STDOUT << "******** STUN - Remaining shots = " << GetBulletCount() << "\n";
  } else if (blaster_mode == MODE_KILL) {
    SFX_blast.Select(-1);
    SaberBase::DoEffect(EFFECT_FIRE, 0);
    shots_fired_++;
    STDOUT << "******** FIRE - Remaining shots = " << GetBulletCount() << "\n";
  } else {
    // Set up the autofire pairing if the font suits it.
    if (!IsEmpty()) {
      SelectAutoFirePair();
      auto_firing_ = true;
      SaberBase::SetLockup(LOCKUP_AUTOFIRE);
      SaberBase::DoBeginLockup();
    }
  }
}

void Loop() override {
  PropBase::Loop();
  PollNextAction();
  sound_library_.Poll(wav_player);
  if (wav_player && !wav_player->isPlaying()) wav_player.Free();

  if (blaster_mode == MODE_AUTO) {
    // First time through, let bgnauto finish first if we have one
    if (SFX_bgnauto && GetWavPlayerPlaying(&SFX_bgnauto)) return;

    // No bgnauto or bgnauto has finished playing, so initialize and start auto-firing
    if (auto_firing_ && !auto_player_) {
      auto_player_ = GetWavPlayerPlaying(&SFX_auto);
      shots_fired_++;
      auto_time_ = millis();
      STDOUT << "******** AUTOFIRING - Remaining shots = " << GetBulletCount() << "\n";
    }
    // use auto.wav's length to count shots
    if (auto_firing_ && auto_player_ && (millis() - auto_time_) > auto_sound_length) {
      auto_sound_length = auto_player_->length() * 1000;
      shots_fired_++;
      if (IsEmpty()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_firing_ = false;
        auto_player_.Free();
        auto_time_ = millis(); // Reset for the first empty sound
        return;
      }
      auto_time_ = millis();
      STDOUT << "******** AUTOFIRING - Remaining shots = " << GetBulletCount() << "\n";
    }

    // This will mimic the hammer running on no bullets at the same rate it was firing.
    // if there are different length auto.wavs, the last played's length is used.
    if (!auto_firing_ && IsEmpty() && trigger_is_pressed_ && (millis() - auto_time_) > auto_sound_length) {
      SaberBase::DoEffect(EFFECT_EMPTY, 0);
      auto_time_ = millis(); // Reset for the next empty sound
    }
  }
}

  virtual void SelectAutoFirePair() { 
    // If multiple auto.wavs exist and there are same number of blast.wavs, the same auto.wav will be repeated.
    // otherwise, all auto.wavs are played back-to-back randomly.
    if (!SFX_auto.files_found() || !SFX_blast.files_found()) return;
    int autoCount = SFX_auto.files_found();
    int blastCount = SFX_blast.files_found();
    // If we don't have a matched pair of autos and blasts, then don't override the sequence to get a matched pair.
    if (autoCount == blastCount) {
        int pairSelection = rand() % autoCount;
        SFX_auto.Select(pairSelection);
        SFX_blast.Select(pairSelection);
    }
  }

  virtual void Reload() {
    SaberBase::DoEffect(EFFECT_RELOAD, 0);
    shots_fired_ = 0;
    is_jammed_ = false;
  }

  virtual void ClipOut() {
    if (max_shots_ != -1) shots_fired_ = max_shots_;
    SaberBase::DoEffect(EFFECT_CLIP_OUT, 0);
    no_clip_ = true;
  }

  virtual void ClipIn() {
    SaberBase::DoEffect(EFFECT_CLIP_IN, 0);
    shots_fired_ = 0;
    is_jammed_ = false;
    no_clip_ = false;
  }

  // Pull in parent's SetPreset, but turn the blaster on.
  void SetPreset(int preset_num, bool announce) override {
    PropBase::SetPreset(preset_num, announce);
      // Just always default to on.
      if (!SaberBase::IsOn()) On();
  }

// Self-destruct 
  enum NextAction {
    NEXT_ACTION_NOTHING,
    NEXT_ACTION_BLOW,
  };

  NextAction next_action_ = NEXT_ACTION_NOTHING;
  uint32_t time_base_;
  uint32_t next_event_time_;
  
  void SetNextAction(NextAction what, uint32_t when) {
    time_base_ = millis();
    next_event_time_ = when;
    next_action_ = what;
  }

  void SetNextActionF(NextAction what, float when) {
    SetNextAction(what, when * 1000);
  }
  
  void PollNextAction() {
    if (millis() - time_base_ > next_event_time_) {
      switch (next_action_) {
        case NEXT_ACTION_NOTHING:
          break;
        case NEXT_ACTION_BLOW:
          Off(OFF_BLAST);
          break;
      }
      next_action_ = NEXT_ACTION_NOTHING;
    }
  }

  void selfDestruct() {
        SaberBase::DoEffect(EFFECT_DESTRUCT, 0);
    float len = hybrid_font.GetCurrentEffectLength();
    SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      SetNextActionF(NEXT_ACTION_BLOW, len);
  }
// Volume Menu
    void VolumeMenu() {
    if (!mode_volume_) {
      mode_volume_ = true;
      if (SFX_vmbegin) {
        sound_library_.SayEnterVolumeMenu();
      } else {
        beeper.Beep(0.1, 1000);
        beeper.Beep(0.1, 2000);
        beeper.Beep(0.1, 3000);
      }
      STDOUT.println("Enter Volume Menu");
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
      STDOUT.println("Exit Volume Menu");
    }
  }

  void VolumeUp() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1,
        dynamic_mixer.get_volume() + VOLUME * 0.10));
      if (SFX_volup) {
        hybrid_font.PlayPolyphonic(&SFX_volup);
      } else {
        beeper.Beep(0.10, 2000);
        beeper.Beep(0.20, 2500);
      }
      STDOUT.print("Volume Up - Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      QuickMaxVolume();
    }
  }

  void VolumeDown() {
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
        dynamic_mixer.get_volume() - VOLUME * 0.10));
      if (SFX_voldown) {
        hybrid_font.PlayPolyphonic(&SFX_voldown);
      } else {
        beeper.Beep(0.10, 2000);
        beeper.Beep(0.20, 1500);
      }
      STDOUT.print("Volume Down - Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
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
    STDOUT.print("Maximum Volume \n");
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
      STDOUT.print("Minimum Volume \n");
  }

  // Make clash do nothing except unjam if jammed.
  void Clash(bool stab, float strength) override {
    if (!mode_volume_) {
      if (is_jammed_) {
        is_jammed_ = false;
        SaberBase::DoEffect(EFFECT_UNJAM, 0);
      }
    }
  }

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override {
    PropBase::DoMotion(Vec3(0), clear);
  }

RefPtr<BufferedWavPlayer> wav_player;

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
// Power On / Off manually
      case EVENTID(BUTTON_FIRE, EVENT_SECOND_HELD_LONG, MODE_ON | BUTTON_MODE_SELECT):
      case EVENTID(BUTTON_FIRE, EVENT_SECOND_HELD_LONG, MODE_OFF | BUTTON_MODE_SELECT):
 

      // case EVENTID(BUTTON_FIRE, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON | BUTTON_MODE_SELECT):
      // case EVENTID(BUTTON_FIRE, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF | BUTTON_MODE_SELECT):
// // In the event of the presence of a power button, let it control the power on events.
//       case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
//       case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):   // weirdness using dual prop
          if (!SaberBase::IsOn()) {
            if (!SFX_out) {
              hybrid_font.PlayCommon(&SFX_reload);
            }
            On();
          } else {
            if (!SFX_in) { // SHOULDN't THIS BE LIKE A POWEROFF SOUND??
              if (SFX_clipout) {
                hybrid_font.PlayCommon(&SFX_clipout);
              } else {
                  beeper.Beep(0.1, 1300);
                  beeper.Beep(0.1, 900);
                  beeper.Beep(0.1, 600);
                  beeper.Beep(0.1, 300);
              }
            }
            Off();
          }
        return true;

// Cycle Mode
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF | BUTTON_CLIP_DETECT):
        if (mode_volume_) {
          QuickMaxVolume();
          return true;
        }
        NextBlasterMode();
        return true;

// Next Preset / Volume Up
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_OFF | BUTTON_CLIP_DETECT):
        if (!mode_volume_) {
          next_preset();
        } else {
          VolumeUp();
        }
        return true;

// Previous Preset / Volume Down
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_CLICK_LONG, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_CLICK_LONG, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_CLICK_LONG, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_CLICK_LONG, MODE_OFF | BUTTON_CLIP_DETECT):
        if (!mode_volume_) {
          previous_preset();
        } else {
          VolumeDown();
        }
        return true;
// Start or Stop Track
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF | BUTTON_CLIP_DETECT):
        if (mode_volume_) return false;
        StartOrStopTrack();
        return true;

// Enter / Exit Volume Menu
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_HELD, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_HELD, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_HELD, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FOURTH_HELD, MODE_OFF | BUTTON_CLIP_DETECT):
        VolumeMenu();
        return true;

// Spoken Battery Level in volts / show battery meter on LEDs
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF | BUTTON_CLIP_DETECT):
      // Avoid weird battery readings when using USB
        if (battery_monitor.battery() < 0.5) {
          sound_library_.SayTheBatteryLevelIs();
          sound_library_.SayDisabled();
          return true;
        }
        if (mode_volume_) return false;
        sound_library_.SayTheBatteryLevelIs();
        sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
        sound_library_.SayVolts();
        STDOUT.println(battery_monitor.battery());
        is_speaking_ = true;
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

// Spoken Battery Level in percentage / show battery meter on LEDs
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_HELD, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_HELD, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_HELD, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_THIRD_HELD, MODE_OFF | BUTTON_CLIP_DETECT):
        if (battery_monitor.battery() < 0.5) {
          sound_library_.SayTheBatteryLevelIs();
          sound_library_.SayDisabled();
          return true;
        }
        if (mode_volume_) return false;
        sound_library_.SayTheBatteryLevelIs();
        sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
        sound_library_.SayPercent();
        STDOUT.println(battery_monitor.battery_percent());
        is_speaking_ = true;
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

// On Demand Battery Level
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_HELD_MEDIUM, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_HELD_MEDIUM, MODE_OFF | BUTTON_CLIP_DETECT):
        if (mode_volume_) return false;
        STDOUT.println(battery_monitor.battery());
        STDOUT.println(battery_monitor.battery_percent());
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

// Reload
      case EVENTID(BUTTON_RELOAD, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_HELD_MEDIUM, MODE_ON | BUTTON_CLIP_DETECT):
        if (no_clip_) {
          SaberBase::DoEffect(EFFECT_EMPTY, 0); 
          return true;
        } else if (!mode_volume_) {
          Reload();
        }
        return true;

// Fire
      case EVENTID(BUTTON_FIRE, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_FIRE, EVENT_PRESSED, MODE_ON | BUTTON_CLIP_DETECT):
        if (mode_volume_) return false;
        if (no_clip_) {
          SaberBase::DoEffect(EFFECT_EMPTY, 0); 
          return true;
        } else {
          Fire();
          trigger_is_pressed_ = true;
          return true;
        }

      case EVENTID(BUTTON_FIRE, EVENT_RELEASED, MODE_ON):
        if (mode_volume_) return false;
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
        if (mode_volume_) return false;
        selfDestruct();
        return true;

// Quote
// Toggle Sequential Quote - pointing down
// QuickMin volume
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON | BUTTON_CLIP_DETECT):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF | BUTTON_CLIP_DETECT):
        if (mode_volume_) {
          QuickMinVolume();
          return true;
        }
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
        if (mode_volume_) return false;
        ClipIn();
        return true;

// Clip removed
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_RELEASED, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_RELEASED, MODE_OFF):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_OFF, MODE_OFF):
        if (mode_volume_) return false;
        ClipOut();
        return true;

// Blade Detect
#ifdef BLADE_DETECT_PIN
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        if (mode_volume_) return false;
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = true;
        FindBladeAgain();
        SaberBase::DoBladeDetect(true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        if (mode_volume_) return false;
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
  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      default: return;
      case EFFECT_STUN: hybrid_font.PlayCommon(&SFX_stun); return;
      case EFFECT_FIRE: hybrid_font.PlayCommon(&SFX_blast); return;
      case EFFECT_CLIP_IN: 
        if (stun_mode_ && SFX_clipins) {
          hybrid_font.PlayCommon(&SFX_clipins); return;
        } else {
          hybrid_font.PlayCommon(&SFX_clipin); return;
        }
      case EFFECT_CLIP_OUT: 
        if (stun_mode_ && SFX_clipouts) {
          hybrid_font.PlayCommon(&SFX_clipouts); return;
        } else { 
          hybrid_font.PlayCommon(&SFX_clipout); return;
        }
      case EFFECT_RELOAD:
        if (shots_fired_ == 0 && SFX_full) {
          hybrid_font.PlayCommon(&SFX_full); return;
        } else if (stun_mode_ && SFX_reloads) {
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

  void SayMode() {
    switch (blaster_mode) {
      case MODE_STUN:
        if (SFX_mdstun) {
          hybrid_font.PlayCommon(&SFX_mdstun);
        } else if (SFX_mode) {
          hybrid_font.PlayCommon(&SFX_mode);
        } else {
          talkie.Say(spSTUN);
        }
      break;
      case MODE_KILL:
        if (SFX_mdkill) {
          hybrid_font.PlayCommon(&SFX_mdkill);
        } else if (SFX_mode) {
          hybrid_font.PlayCommon(&SFX_mode);
        } else {
          talkie.Say(spKILL);      
        }
      break;
      case MODE_AUTO:
        if (SFX_mdauto) {
          hybrid_font.PlayCommon(&SFX_mdauto);
        } else if (SFX_mode) {
          hybrid_font.PlayCommon(&SFX_mode);
        } else {
          talkie.Say(spAUTOFIRE);       
        }
      break;
    }
  }
};

#endif

