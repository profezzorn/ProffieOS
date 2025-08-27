/*
  blaster.h prop file
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2023 Fredrik Hubinette
  Fredrik Hubinette, Fernando da Rosa, Brian Conner, Matthew McGeary,
  Scott Weber and Alejandro Belluscio.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/

  This prop file makes full use soundfont and display specification, while keeping features to a minimum

Functionality:
-------------------------------------------------------------------------------
* PROP DEFINES *
Optional #defines on your config file that customize the blaster's behavior.
-------------------------------------------------------------------------------
Optional defines:
  #define ENABLE_BLASTER_AUTO           - DEPRECATED. Enable Autofire/rapid fire mode. Please replace with BLASTER_ENABLE_AUTO.
  #define BLASTER_ENABLE_AUTO           - Enable Autofire/rapid fire mode.
  #define BLASTER_DEFAULT_MODE          - Sets the mode at startup MODE_STUN|MODE_KILL|MODE_AUTO. Defaulst to MODE_STUN.
  #define BLASTER_SHOTS_UNTIL_EMPTY 15  - Whatever number, not defined = unlimited shots.
  #define BLASTER_JAM_PERCENTAGE        - Range 0-100 percent. If this is not defined, random from 0-100%.

-------------------------------------------------------------------------------
* PROP BUTTONS *
This prop manages up to six different buttons.
-------------------------------------------------------------------------------

*Dual Buttons Mode*
- Buttons: FIRE and MODE
  This is the "stock" configuration.
  Weapon will always start on the default mode (define with BLASTER_DEFAULT_MODE, STUN is default).
  Default is to start powered OFF if the font has a poweron.wav file, otherwise ON.

    Fire                      - Click FIRE.
    Cycle Modes               - Click MODE.
    Next Preset               - Long click and release MODE.
    Previous Preset           - Double click and hold MODE, release after a second.
    Reload                    - Hold MODE until Reloaded.
    Start/Stop Track          - Double click MODE.
    Unjam                     - Bang the blaster.

*Extra Buttons*
- Button: POWER
    Power On / Off            - Click POWER.

- Button: RELOAD
    Reload                    - Hold RELOAD until Reloaded.

- Button: CLIP
      Clip In                 - Latch CLIP
      Clip Out                - Unlatch CLIP


-------------------------------------------------------------------------------
* PROP SOUNDS *
This prop manages the following sounds.
-------------------------------------------------------------------------------

bgnauto.wav     Played when auto fire starts.
auto.wav        Played while auto fire is going.
endauto.wav     Played when auto fire ends.
blast.wav       Is the semi-automatic fire sound. You can have as many as you want.
boot.wav        Played when ProffieOS boots up.
clipin.wav      Sound made when inserting a clip.
clipout.wav     Sound made when dropping a clip.
empty.wav       Sound when the weapon is out of rounds.
font.wav        Name of the preset.
full.wav        Sound made when the weapon is full of ammo.
hum.wav         Constant sound looping while not firing.
jam.wav         Sound made when the weapon jamed.
unjam.wav       Sound made when unjamming the blaster.
plioff.wav      Played while retracting the PLI bargraph.
plion.wav       Played while extending the PLI bargraph.
poweron.wav     If this file is present, the blaster will start once it is turned on.
                  Otherwise, you will need a Power button to power it on.
range.wav       Sounds of increasing weapon range/power.
stun.wav        Firing sound for stun mode.
reload.wav      Reloading sound.
mdkill.wav      Sound made when switching to KILL mode
mdstun.wav      Sound made when switching to STUN mode
mdauto.wav      Sound made when switching to AUTO mode
mode.wav        Fallback sound used when switching mode if none of the 3 above exist.
                  If no mode sounds are present at all, Talkie will speak the mode (if not disabled).
*/

#ifndef PROPS_BLASTER_H
#define PROPS_BLASTER_H

#define PROP_HAS_GETBLASTERMODE

#ifndef BLASTER_DEFAULT_MODE
#define BLASTER_DEFAULT_MODE MODE_STUN
#endif

#include "prop_base.h"

#define PROP_TYPE Blaster
#define PROP_HAS_BULLET_COUNT

EFFECT(clipin);
EFFECT(clipout);
EFFECT(empty);
EFFECT(full);
EFFECT(jam);
EFFECT(mode);
EFFECT(plioff);
EFFECT(plion);
EFFECT(range);
EFFECT(reload);
EFFECT(stun);
EFFECT(unjam);
EFFECT(mdstun);
EFFECT(mdkill);
EFFECT(mdauto);
// For mode sounds, specific "mdstun", "mdkill", and "mdauto" may be used.
// If just a single "mode" sound for all switches exists, that will be used.
// If no mode sounds exist in the font, a talkie version will speak the mode on switching.
class Blaster : public PROP_INHERIT_PREFIX PropBase {
public:
  Blaster() : PropBase() {}
  const char* name() override { return "Blaster"; }

  // Mode states to handle kill vs stun effects
  enum BlasterMode {
    MODE_STUN,
    MODE_KILL,
    MODE_AUTO
  };

  BlasterMode blaster_mode = BLASTER_DEFAULT_MODE;

#ifdef BLASTER_SHOTS_UNTIL_EMPTY
  const int max_shots_ = BLASTER_SHOTS_UNTIL_EMPTY;
#else
  const int max_shots_ = -1;
#endif

  int GetBlasterMode() const {
    return blaster_mode;
  }

  virtual void SetBlasterMode(BlasterMode to_mode) {
    if (!auto_firing_) {
      blaster_mode = to_mode;
      SaberBase::DoEffect(EFFECT_MODE, 0);
    }
  }

  virtual void NextBlasterMode() {
    switch(blaster_mode) {
      case MODE_STUN:
        SetBlasterMode(MODE_KILL);
        return;
      case MODE_KILL:
#if defined (ENABLE_BLASTER_AUTO) || defined (BLASTER_ENABLE_AUTO)
        if (SFX_auto) {
          SetBlasterMode(MODE_AUTO);
        } else {
          SetBlasterMode(MODE_STUN);
        }
#else
        SetBlasterMode(MODE_STUN);
#endif
        return;
      case MODE_AUTO:
        SetBlasterMode(MODE_STUN);
        return;
    }
  }

  bool CheckEmpty() const {
    return max_shots_ != -1 && shots_fired_ >= max_shots_;
  }

  int GetBulletCount() {
    return max_shots_ - shots_fired_;
  }

  virtual bool DoEmpty() {
    if (CheckEmpty()) {
      SaberBase::DoEffect(EFFECT_EMPTY, 0);  // Trigger the empty effect
      return true;
    }
    return false;
  }

  virtual bool CheckJam(int percent) {
    int random = rand() % 100;
    return random < percent;
  }

  bool DoJam() {
#if defined(ENABLE_MOTION) && defined(BLASTER_JAM_PERCENTAGE)
    // If we're already jammed then we don't need to recheck. If we're not jammed then check if we just jammed.
    is_jammed_ = is_jammed_ ? true : CheckJam(BLASTER_JAM_PERCENTAGE);

    if (is_jammed_) {
      SaberBase::DoEffect(EFFECT_JAM, 0);
      return true;
    } else {
      return false;
    }
#else
    return false;
#endif
  }

  virtual void DoStun() {
    SaberBase::DoEffect(EFFECT_STUN, 0);
    shots_fired_++;
  }

  virtual void DoKill() {
    SFX_blast.Select(-1);
    SaberBase::DoEffect(EFFECT_FIRE, 0);
    shots_fired_++;
  }

  virtual void DoAutoFire() {
    SelectAutoFirePair(); // Set up the auto-fire pairing if the font suits it
    SaberBase::SetLockup(LOCKUP_AUTOFIRE);
    SaberBase::DoBeginLockup();
    auto_firing_ = true;
  }

  virtual void Fire() {
    if (DoJam()) return;
    if (DoEmpty()) return;

    switch (blaster_mode) {
      case MODE_STUN:
        DoStun();
        break;
      case MODE_KILL:
        DoKill();
        break;
      case MODE_AUTO:
        DoAutoFire();
        break;
    }
  }

  virtual void SelectAutoFirePair() {
    if (!SFX_auto.files_found() || !SFX_blast.files_found()) return;

    int autoCount = SFX_auto.files_found();
    int blastCount = SFX_blast.files_found();
    int pairSelection;

    // If we don't have a matched pair of autos and blasts, then don't override the sequence to get a matched pair.
    if (autoCount == blastCount) {
        pairSelection = rand() % autoCount;
        SFX_auto.Select(pairSelection);
        SFX_blast.Select(pairSelection);
    }
  }

  virtual void Reload() {
    shots_fired_ = 0;
    SaberBase::DoEffect(EFFECT_RELOAD, 0);
  }

  virtual void ClipOut() {
    if (max_shots_ != -1) shots_fired_ = max_shots_;
    SaberBase::DoEffect(EFFECT_CLIP_OUT, 0);
  }

  virtual void ClipIn() {
    if (max_shots_ != -1) shots_fired_ = 0;
    SaberBase::DoEffect(EFFECT_CLIP_IN, 0);
  }

  // Pull in parent's SetPreset, but turn the blaster on.
  void SetPreset(int preset_num, bool announce) override {
    PropBase::SetPreset(preset_num, announce);
    if (!SFX_poweron && !SaberBase::IsOn()) {
      On();
    }
  }

  void LowBatteryOff() override {
    if (SFX_poweron) {
      PropBase::LowBatteryOff();
    }
  }

  // Self-destruct pulled from Detonator. Inherit prop and add PollNextAction() to their loop function.
  // BEGINING of Detonator Code.
  bool armed_ = false;

  enum NextAction {
    NEXT_ACTION_NOTHING,
    NEXT_ACTION_ARM,
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

  virtual void PollNextAction() {
    if (millis() - time_base_ > next_event_time_) {
      switch (next_action_) {
        case NEXT_ACTION_NOTHING:
          break;
        case NEXT_ACTION_ARM:
          armed_ = true;
          // TODO: Should we have separate ARMING and ARMED states?
          break;
        case NEXT_ACTION_BLOW:
          Off(OFF_BLAST);
          break;
      }
      next_action_ = NEXT_ACTION_NOTHING;
    }
  }

  void beginArm() {
    SaberBase::SetLockup(SaberBase::LOCKUP_ARMED);
    SaberBase::DoBeginLockup();
#ifdef ENABLE_AUDIO
    float len = hybrid_font.GetCurrentEffectLength();
#else
    float len = 1.6;
#endif
    SetNextActionF(NEXT_ACTION_ARM, len);
  }

    virtual void selfDestruct() {
    SaberBase::DoEndLockup();
#ifdef ENABLE_AUDIO
    float len = hybrid_font.GetCurrentEffectLength();
#else
    float len = 0.0;
#endif
    SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
    if (armed_) {
      SetNextActionF(NEXT_ACTION_BLOW, len);
    } else {
      SetNextAction(NEXT_ACTION_NOTHING, 0);
    }
  }

// END of Detonator Code.
  // Make clash do nothing except unjam if jammed.
  void Clash(bool stab, float strength) override {
    if (is_jammed_) {
      is_jammed_ = false;
      SaberBase::DoEffect(EFFECT_UNJAM, 0);
    }
  }

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override {
    PropBase::DoMotion(Vec3(0), clear);
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        NextBlasterMode();
        return true;
      case EVENTID(BUTTON_MODE_SELECT, EVENT_FIRST_CLICK_LONG, MODE_ON):
        next_preset();
        return true;

      case EVENTID(BUTTON_MODE_SELECT, EVENT_SECOND_CLICK_LONG, MODE_ON):
        previous_preset();
        return true;

      case EVENTID(BUTTON_RELOAD, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_MODE_SELECT, EVENT_HELD_MEDIUM, MODE_ON):
        Reload();
        return true;

      case EVENTID(BUTTON_MODE_SELECT, EVENT_DOUBLE_CLICK, MODE_ON):
        StartOrStopTrack();
        return true;

      case EVENTID(BUTTON_FIRE, EVENT_PRESSED, MODE_ON):
        Fire();
        return true;

      case EVENTID(BUTTON_FIRE, EVENT_RELEASED, MODE_ON):
        if (blaster_mode == MODE_AUTO) {
          if (SaberBase::Lockup()) {
            SaberBase::DoEndLockup();
            SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
            auto_firing_ = false;
          }
        }
        return true;

      case EVENTID(BUTTON_CLIP_DETECT, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_ON, MODE_ON):
        ClipIn();
        return true;

      case EVENTID(BUTTON_CLIP_DETECT, EVENT_RELEASED, MODE_ON):
      case EVENTID(BUTTON_CLIP_DETECT, EVENT_LATCH_OFF, MODE_ON):
        ClipOut();
        return true;

      // In the event of the presence of a power button, let it control the power on events.
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        On();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
        Off();
        return true;

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
      case EFFECT_STUN:     hybrid_font.PlayCommon(&SFX_stun);    return;
      case EFFECT_FIRE:     hybrid_font.PlayCommon(&SFX_blast);   return;
      case EFFECT_CLIP_IN:  hybrid_font.PlayCommon(&SFX_clipin);  return;
      case EFFECT_CLIP_OUT: hybrid_font.PlayCommon(&SFX_clipout); return;
      case EFFECT_RELOAD:   hybrid_font.PlayCommon(&SFX_reload);  return;
      case EFFECT_MODE:     SayMode();                            return;
      case EFFECT_RANGE:    hybrid_font.PlayCommon(&SFX_range);   return;
      case EFFECT_EMPTY:    hybrid_font.PlayCommon(&SFX_empty);   return;
      case EFFECT_FULL:     hybrid_font.PlayCommon(&SFX_full);    return;
      case EFFECT_JAM:      hybrid_font.PlayCommon(&SFX_jam);     return;
      case EFFECT_UNJAM:    hybrid_font.PlayCommon(&SFX_unjam);   return;
      case EFFECT_PLI_ON:   hybrid_font.PlayCommon(&SFX_plion);   return;
      case EFFECT_PLI_OFF:  hybrid_font.PlayCommon(&SFX_plioff);  return;

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
#ifndef DISABLE_TALKIE
          talkie.Say(spSTUN);
#else
          beeper.Beep(0.05, 2000.0);
#endif
        }
      break;
      case MODE_KILL:
        if (SFX_mdkill) {
          hybrid_font.PlayCommon(&SFX_mdkill);
        } else if (SFX_mode) {
          hybrid_font.PlayCommon(&SFX_mode);
        } else {
#ifndef DISABLE_TALKIE
          talkie.Say(spKILL);
#else
          beeper.Beep(0.05, 2000.0);
#endif
        }
      break;
      case MODE_AUTO:
        if (SFX_mdauto) {
          hybrid_font.PlayCommon(&SFX_mdauto);
        } else if (SFX_mode) {
          hybrid_font.PlayCommon(&SFX_mode);
        } else {
#ifndef DISABLE_TALKIE
          talkie.Say(spAUTOFIRE);
#else
          beeper.Beep(0.05, 2000.0);
#endif
        }
      break;
    }
  }

  bool auto_firing_ = false;
  int shots_fired_ = 0;
  bool is_jammed_ = false;

};

#endif

#ifdef PROP_BOTTOM

#define ONCE_PER_BLASTER_EFFECT(X)    \
  X(blast)                            \
  X(reload)                           \
  X(empty)                            \
  X(jam)                              \
  X(clipin)                           \
  X(clipout)                          \
  X(destruct)

#ifdef INCLUDE_SSD1306

struct BlasterDisplayConfigFile : public ConfigFile {
  BlasterDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSFireImageDuration,     1000.0f);
    CONFIG_VARIABLE2(ProffieOSReloadImageDuration,   1000.0f);
    CONFIG_VARIABLE2(ProffieOSEmptyImageDuration,    1000.0f);
    CONFIG_VARIABLE2(ProffieOSJamImageDuration,      1000.0f);
    CONFIG_VARIABLE2(ProffieOSClipinImageDuration,   1000.0f);
    CONFIG_VARIABLE2(ProffieOSClipoutImageDuration,  1000.0f);
    CONFIG_VARIABLE2(ProffieOSDestructImageDuration, 10000.0f);
  }

  // for OLED displays, the time a blast.bmp    will play
  float ProffieOSFireImageDuration;
  // for OLED displays, the time a reload.bmp   will play
  float ProffieOSReloadImageDuration;
  // for OLED displays, the time a empty.bmp    will play
  float ProffieOSEmptyImageDuration;
  // for OLED displays, the time a jam.bmp      will play
  float ProffieOSJamImageDuration;
  // for OLED displays, the time a clipin.bmp   will play
  float ProffieOSClipinImageDuration;
  // for OLED displays, the time a clipout.bmp  will play
  float ProffieOSClipoutImageDuration;
  // for OLED displays, the time a destruct.bmp will play
  float ProffieOSDestructImageDuration;
};


template<typename PREFIX = ByteArray<>>
struct BlasterDisplayEffects  {
  BlasterDisplayEffects() : dummy_(0) ONCE_PER_BLASTER_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_BLASTER_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class BlasterDisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  BlasterDisplayEffects<PREFIX> img_;
  BlasterDisplayConfigFile &blaster_font_config;
  BlasterDisplayController() :
    img_(*getPtr<BlasterDisplayEffects<PREFIX>>()),
    blaster_font_config(*getPtr<BlasterDisplayConfigFile>()) {
  }

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_FIRE:
        this->ShowFileWithSoundLength(&img_.IMG_blast,   blaster_font_config.ProffieOSFireImageDuration);
        break;
      case EFFECT_RELOAD:
        this->ShowFileWithSoundLength(&img_.IMG_reload,  blaster_font_config.ProffieOSReloadImageDuration);
        break;
      case EFFECT_EMPTY:
        this->ShowFileWithSoundLength(&img_.IMG_empty,   blaster_font_config.ProffieOSEmptyImageDuration);
        break;
      case EFFECT_JAM:
        this->ShowFileWithSoundLength(&img_.IMG_jam,     blaster_font_config.ProffieOSJamImageDuration);
        break;
      case EFFECT_CLIP_IN:
        this->ShowFileWithSoundLength(&img_.IMG_clipin,  blaster_font_config.ProffieOSClipinImageDuration);
        break;
      case EFFECT_CLIP_OUT:
        this->ShowFileWithSoundLength(&img_.IMG_clipout, blaster_font_config.ProffieOSClipoutImageDuration);
        break;
      default:
        StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }

  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_BLAST) {
      this->ShowFileWithSoundLength(&img_.IMG_destruct, blaster_font_config.ProffieOSDestructImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};

#endif  // INCLUDE_SSD1306

template<int W, int H, typename PREFIX = ConcatByteArrays<typename NumberToByteArray<W>::type, ByteArray<'x'>, typename NumberToByteArray<H>::type>>
class BlasterColorDisplayController : public StandarColorDisplayController<W, H, PREFIX> {
public:
  template<int w, int h>
  explicit BlasterColorDisplayController(SizedLayeredScreenControl<w, h>* screen) : StandarColorDisplayController<W, H, PREFIX>(screen) ONCE_PER_BLASTER_EFFECT(INIT_SCR) {
  }
  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_FIRE:     this->scr_.Play(&SCR_blast);   break;
      case EFFECT_RELOAD:   this->scr_.Play(&SCR_reload);  break;
      case EFFECT_EMPTY:    this->scr_.Play(&SCR_empty);   break;
      case EFFECT_JAM:      this->scr_.Play(&SCR_jam);     break;
      case EFFECT_CLIP_IN:  this->scr_.Play(&SCR_clipin);  break;
      case EFFECT_CLIP_OUT: this->scr_.Play(&SCR_clipout); break;
      default:
        StandarColorDisplayController<W, H, PREFIX>::SB_Effect2(effect, location);
    }
  }

protected:
  ONCE_PER_BLASTER_EFFECT(DEF_SCR);
};

#undef ONCE_PER_BLASTER_EFFECT

#endif  // PROP_BOTTOM
