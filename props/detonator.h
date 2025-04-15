#ifndef PROPS_DETONATOR_H
#define PROPS_DETONATOR_H

#include "prop_base.h"

#define PROP_TYPE Detonator

class Detonator : public PROP_INHERIT_PREFIX PropBase {
public:
  Detonator() : PropBase() {}
  const char* name() override { return "Detonator"; }

#ifdef DELAYED_OFF
  bool powered_ = false;
  void SetPower(bool on) { powered_ = on; }
#else
  bool powered_ = true;
  void SetPower(bool on) {}
#endif

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
  
  void PollNextAction() {
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

  void blast() {
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

  void Loop() override {
    PropBase::Loop();
    PollNextAction();
  }

#if NUM_BUTTONS >= 2
  // Make clash do nothing
  void Clash(bool stab, float strength) override {}
#endif

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override { }


  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
        armed_ = false;
        SetPower(true);
        On();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_OFF):
        SetPower(false);
        Off();
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_DOUBLE_CLICK, MODE_OFF):
	if (powered_) rotate_presets();
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_DOUBLE_CLICK, MODE_ON):
        StartOrStopTrack();
        return true;

      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
	beginArm();
	break;

      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ON):
        blast();
	return armed_;

        // TODO: Long click when off?
    }
    return false;
  }
};

#endif

/************************************\
|*                                  *|
|*   DETONATOR DISPLAY CONTROLLER   *|    // added by OlivierFlying747-8
|*                                  *|
\************************************/

// If you want to add more, replace all the "***xxxn***, ***XXXn***, ***Xxxn***" with appropriate
// effects name(s), and respect the CAPS, no caps & Some Caps. Or else it may not compile or work!

#ifdef PROP_BOTTOM

#define ONCE_PER_DETONATOR_EFFECT(X)  \
  X(bgnarm)                           \
  X(endarm)                           \
  X(boom) // Add a backslash to this line when adding the next line.
          // The last line in the list needs no backslash at the end!
//X(***xxx1***)
//X(***xxx2***)
//X(***xxx3***)

#ifdef INCLUDE_SSD1306

struct DetonatorDisplayConfigFile : public ConfigFile {
  DetonatorDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSBgnarmImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSEndarmImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSBoomImageDuration, 1000.0f);
  //CONFIG_VARIABLE2(ProffieOS***Xxx1***ImageDuration, 1000.0f);
  //CONFIG_VARIABLE2(ProffieOS***Xxx2***ImageDuration, 1000.0f);
  //CONFIG_VARIABLE2(ProffieOS***Xxx3***ImageDuration, 1000.0f);
  }

  // for OLED displays, the time a bgnarm.bmp will play.
  float ProffieOSBgnarmImageDuration;
  // for OLED displays, the time a endarm.bmp will play.
  float ProffieOSEndarmImageDuration;
  // for OLED displays, the time a boom.bmp will play.
  float ProffieOSBoomImageDuration;
  // for OLED displays, the time a ***xxx1***.bmp will play.
//float ProffieOS***Xxx1***ImageDuration;
  // for OLED displays, the time a ***xxx2***.bmp will play.
//float ProffieOS***Xxx2***ImageDuration;
  // for OLED displays, the time a ***xxx3***.bmp will play.
//float ProffieOS***Xxx3***ImageDuration;
};

template<typename PREFIX = ByteArray<>>
struct DetonatorDisplayEffects  {
  DetonatorDisplayEffects() : dummy_(0) ONCE_PER_DETONATOR_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_DETONATOR_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class DetonatorDisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  DetonatorDisplayEffects<PREFIX> img_;
  DetonatorDisplayConfigFile &detonator_font_config;
  DetonatorDisplayController() :
    img_(*getPtr<DetonatorDisplayEffects<PREFIX>>()),
    detonator_font_config(*getPtr<DetonatorDisplayConfigFile>()) {}

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_BGNARM:
        if (img_.IMG_bgnarm) {
          ShowFileWithSoundLength(&img_.IMG_bgnarm, detonator_font_config.ProffieOSBgnarmImageDuration);
        } else {
          this->SetMessage(" td\narmed");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_ENDARM:
        if (img_.IMG_endarm) {
          ShowFileWithSoundLength(&img_.IMG_endarm, detonator_font_config.ProffieOSEndarmImageDuration);
        } else {
          this->SetMessage("    td\ndisarmed");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_BOOM:
        if (img_.IMG_boom) {
          ShowFileWithSoundLength(&img_.IMG_boom,   detonator_font_config.ProffieOSBoomImageDuration);
        } else {
          this->SetMessage(" td\nboom");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

/*
      case EFFECT_***XXX1***:
        if (img_.IMG_***xxx1***) {
          ShowFileWithSoundLength(&img_.IMG_***xxx1***, detonator_font_config.ProffieOS***Xxx1***ImageDuration);
        } else {
          this->SetMessage("***xxx1***");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_***XXX2***:
        if (img_.IMG_***xxx2***) {
          ShowFileWithSoundLength(&img_.IMG_***xxx2***, detonator_font_config.ProffieOS***Xxx2***ImageDuration);
        } else {
          this->SetMessage("***xxx2***");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_***XXX3***:
        if (img_.IMG_***xxx3***) {
          ShowFileWithSoundLength(&img_.IMG_***xxx3***, detonator_font_config.ProffieOS***Xxx3***ImageDuration);
        } else {
          this->SetMessage("***xxx3***");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;
*/

      default:
        StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }

  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_BLAST) {
      ShowFileWithSoundLength(img_.IMG_bgnarm, detonator_font_config.ProffieOSBgnarmImageDuration);
      ShowFileWithSoundLength(img_.IMG_endarm, detonator_font_config.ProffieOSEndarmImageDuration);
      ShowFileWithSoundLength(img_.IMG_boom,   detonator_font_config.ProffieOSBoomImageDuration);
    //ShowFileWithSoundLength(img_.IMG_***xxx1***, detonator_font_config.ProffieOS***Xxx1***ImageDuration);
    //ShowFileWithSoundLength(img_.IMG_***xxx2***, detonator_font_config.ProffieOS***Xxx2***ImageDuration);
    //ShowFileWithSoundLength(img_.IMG_***xxx3***, detonator_font_config.ProffieOS***Xxx3***ImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};

#endif  // INCLUDE_SSD1306

template<int W, int H, typename PREFIX = ConcatByteArrays<typename NumberToByteArray<W>::type, ByteArray<'x'>,
         typename NumberToByteArray<H>::type>>
class DetonatorColorDisplayController : public StandarColorDisplayController<W, H, PREFIX> {
public:
  template<int w, int h>
  explicit DetonatorColorDisplayController(SizedLayeredScreenControl<w, h>* screen) :
  StandarColorDisplayController<W, H, PREFIX>(screen) ONCE_PER_DETONATOR_EFFECT(INIT_SCR) {
  }
  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_BGNARM:     this->scr_.Play(&SCR_bgnarm);     break;
      case EFFECT_ENDARM:     this->scr_.Play(&SCR_endarm);     break;
      case EFFECT_BOOM:       this->scr_.Play(&SCR_boom);       break;
    //case EFFECT_***XXX1***: this->scr_.Play(&SCR_***xxx1***); break;
    //case EFFECT_***XXX2***: this->scr_.Play(&SCR_***xxx2***); break;
    //case EFFECT_***XXX3***: this->scr_.Play(&SCR_***xxx3***); break;
      default:
        StandarColorDisplayController<W, H, PREFIX>::SB_Effect2(effect, location);
    }
  }

protected:
  ONCE_PER_DETONATOR_EFFECT(DEF_SCR);
};

#undef ONCE_PER_DETONATOR_EFFECT

#endif  // PROP_BOTTOM
