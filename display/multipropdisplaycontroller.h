// version 05 jetpack animations simplified & morsecodedisplaycontroller added

#ifndef MULTIPROPDISPLAYCONTROLLER_H
#define MULTIPROPDISPLAYCONTROLLER_H

/* add to ssd1306.h
#ifdef PROPS_MULTI_PROP_H               // added by Oli
#include "multipropdisplaycontroller.h" // added by Oli
#endif                                  // added by Oli
*/

struct MultiPropDisplayConfigFile : public ConfigFile {
  MultiPropDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSSaberModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSBlasterModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSDetonatorModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSJetpackModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMorsecodeModeImageDuration, 1000.0f);
  }

  // for OLED displays, the time a sabermode.bmp will play
  float ProffieOSSaberModeImageDuration;
  // for OLED displays, the time a blastermode.bmp will play
  float ProffieOSBlasterModeImageDuration;
  // for OLED displays, the time a detonatormode.bmp will play
  float ProffieOSDetonatorModeImageDuration;
  // for OLED displays, the time a jetpackmode.bmp will play
  float ProffieOSJetpackModeImageDuration;
  // for OLED displays, the time a morsecodemode.bmp will play
  float ProffieOSMorsecodeModeImageDuration;
};

#define ONCE_PER_MULTIPROP_EFFECT(X)  \
  X(sabermode)                        \
  X(blastermode)                      \
  X(detonatormode)                    \
  X(jetpackmode)                      \
  X(morsecodemode)

template<typename PREFIX = ByteArray<>>
struct MultiPropDisplayEffects  {
  MultiPropDisplayEffects() : dummy_(0) ONCE_PER_MULTIPROP_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_MULTIPROP_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class MultiPropDisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  MultiPropDisplayEffects<PREFIX> img_;
  MultiPropDisplayConfigFile &multiprop_font_config;
  MultiPropDisplayController() :
    img_(*getPtr<MultiPropDisplayEffects<PREFIX>>()),
    multiprop_font_config(*getPtr<MultiPropDisplayConfigFile>()) {}

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_SABERMODE:
       if (img_.IMG_sabermode)
  ShowFileWithSoundLength(&img_.IMG_sabermode,     multiprop_font_config.ProffieOSSaberModeImageDuration);
       else {
  this->SetMessage("saber\nmode");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_BLASTERMODE:
        if (img_.IMG_blastermode)
  ShowFileWithSoundLength(&img_.IMG_blastermode,   multiprop_font_config.ProffieOSBlasterModeImageDuration);
       else {
  this->SetMessage("blaster\n mode");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_DETONATORMODE:
        if (img_.IMG_detonatormode)
  ShowFileWithSoundLength(&img_.IMG_detonatormode, multiprop_font_config.ProffieOSDetonatorModeImageDuration);
       else {
  this->SetMessage("detonator\n  mode");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_JETPACKMODE:
        if (img_.IMG_jetpackmode)
  ShowFileWithSoundLength(&img_.IMG_jetpackmode,   multiprop_font_config.ProffieOSJetpackModeImageDuration);
        else {
  this->SetMessage("jetpack\nmode");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_MORSECODEMODE:
        if (img_.IMG_morsecodemode)
  ShowFileWithSoundLength(&img_.IMG_morsecodemode, multiprop_font_config.ProffieOSMorsecodeModeImageDuration);
        else {
  this->SetMessage("morse code\n  mode");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      default:
  StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }


  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_BLAST) {
      ShowFileWithSoundLength(img_.IMG_sabermode, multiprop_font_config.ProffieOSSaberModeImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};

/**********************************\
|*                                *|
|*   JETPACK DISPLAY CONTROLLER   *|
|*                                *|
\**********************************/

struct JetpackDisplayConfigFile : public ConfigFile {
  JetpackDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSFalseStartImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSStutteringImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSSelfDestructImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSAimingImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSTargettingImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMissileLaunchImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMissileGoesBoomImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMandoTalkImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSDisarmImageDuration, 1000.0f);
  }
  // for OLED displays, the time a falsestart.bmp will play
  float ProffieOSFalseStartImageDuration;
  // for OLED displays, the time a stuttering.bmp will play
  float ProffieOSStutteringImageDuration;
  // for OLED displays, the time a shutdown.bmp will play
  float ProffieOSSelfDestructImageDuration;
  // for OLED displays, the time a aiming.bmp will play
  float ProffieOSAimingImageDuration;
  // for OLED displays, the time a targetting.bmp will play
  float ProffieOSTargettingImageDuration;
  // for OLED displays, the time a missilelaunch.bmp will play
  float ProffieOSMissileLaunchImageDuration;
  // for OLED displays, the time a missilegoesboom.bmp will play
  float ProffieOSMissileGoesBoomImageDuration;
  // for OLED displays, the time a mandotalk.bmp will play
  float ProffieOSMandoTalkImageDuration;
  // for OLED displays, the time a disarm.bmp will play
  float ProffieOSDisarmImageDuration;
};

#define ONCE_PER_JETPACK_EFFECT(X)  \
  X(startidlemode)                  \
  X(idlemode)                       \
  X(falsestart)                     \
  X(startflightmode)                \
  X(flightmode)                     \
  X(stuttering)                     \
  X(shutdown)                       \
  X(selfdestruct)                   \
  X(aiming)                         \
  X(targetting)                     \
  X(missilelaunch)                  \
  X(missilegoesboom)                \
  X(mandotalk)                      \
  X(disarm)

template<typename PREFIX = ByteArray<>>
struct JetpackDisplayEffects  {
  JetpackDisplayEffects() : dummy_(0) ONCE_PER_JETPACK_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_JETPACK_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class JetpackDisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  JetpackDisplayEffects<PREFIX> img_;
  JetpackDisplayConfigFile &jetpack_font_config;
  JetpackDisplayController() :
    img_(*getPtr<JetpackDisplayEffects<PREFIX>>()),
    jetpack_font_config(*getPtr<JetpackDisplayConfigFile>()) {}

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      // jetpack missile display effects: Make animations for OLED from the episode from Mando S02E06
      // ================================
      case EFFECT_AIMING:
       if (img_.IMG_aiming)          // animation of viewfinder coming down
  ShowFileWithSoundLength(&img_.IMG_aiming,          jetpack_font_config.ProffieOSAimingImageDuration);
       else {
  this->SetMessage("aiming");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_TARGETTING:
       if (img_.IMG_targetting)      // animation of targetting
  ShowFileWithSoundLength(&img_.IMG_targetting,      jetpack_font_config.ProffieOSTargettingImageDuration);
       else {
  this->SetMessage("targetting");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_MISSILELAUNCH:
       if (img_.IMG_missilelaunch)   // animation of jetpack launching missile
  ShowFileWithSoundLength(&img_.IMG_missilelaunch,   jetpack_font_config.ProffieOSMissileLaunchImageDuration);
       else {
  this->SetMessage("launching\nmissile");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_MISSILEGOESBOOM:
       if (img_.IMG_missilegoesboom) // animation of explosion
  ShowFileWithSoundLength(&img_.IMG_missilegoesboom, jetpack_font_config.ProffieOSMissileGoesBoomImageDuration);
       else {
  this->SetMessage("missile\nexplodes");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_MANDOTALK:
       if (img_.IMG_mandotalk)       // animation of mando & boba talking: "Nice shot! I was aiming for the other one!"
  ShowFileWithSoundLength(&img_.IMG_mandotalk,       jetpack_font_config.ProffieOSMandoTalkImageDuration);
       else {
  this->SetMessage("nice shot");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_DISARM:
       if (img_.IMG_disarm)          // animation of viewfinder going back up
  ShowFileWithSoundLength(&img_.IMG_disarm,          jetpack_font_config.ProffieOSDisarmImageDuration);
       else {
  this->SetMessage("disarming");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      // jetpack misshaps display effects:
      // =================================
      case EFFECT_FALSESTART:
       if (img_.IMG_falsestart) // Jetpack with "smoke fart" animation
  ShowFileWithSoundLength(&img_.IMG_falsestart,      jetpack_font_config.ProffieOSFalseStartImageDuration);
       else {
  this->SetMessage("jetpack\nfalse start");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_STUTTERING:
       if (img_.IMG_stuttering) // Jetpack with trail of "smoke farts" animation
  ShowFileWithSoundLength(&img_.IMG_stuttering,      jetpack_font_config.ProffieOSStutteringImageDuration);
       else {
  this->SetMessage("jetpack\nstuttering");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      case EFFECT_SELFDESTRUCT:
       if (img_.selfdestruct) // Jetpack exploding animation
  ShowFileWithSoundLength(&img_.selfdestruct,        jetpack_font_config.ProffieOSSelfDestructImageDuration);
       else {
  this->SetMessage("jetpack\nself destruct");
  this->SetScreenNow(SCREEN_MESSAGE);
       } break;
      // jetpack normal operation display effects: (I do not have any idea of animations for these!)
      // ========================================= (So I think the text only will do, at least for now?)
      case EFFECT_STARTIDLEMODE:
  this->SetMessage("jetpack\nigniting");
  this->SetScreenNow(SCREEN_MESSAGE);
        break;
      case EFFECT_IDLEMODE:
  this->SetMessage("jetpack\nidling");
  this->SetScreenNow(SCREEN_MESSAGE);
        break;
      case EFFECT_STARTFLIGHTMODE:
  this->SetMessage("jetpack\nstarting");
  this->SetScreenNow(SCREEN_MESSAGE);
        break;
      case EFFECT_FLIGHTMODE:
  this->SetMessage("jetpack\nflying");
  this->SetScreenNow(SCREEN_MESSAGE);
        break;
      case EFFECT_STOPFLIGHTMODE:
  this->SetMessage("jetpack\nshutdown");
  this->SetScreenNow(SCREEN_MESSAGE);
        break;
     case EFFECT_STOPIDLEMODE:
  this->SetMessage("  jetpack\ncompletely off");
  this->SetScreenNow(SCREEN_MESSAGE);
        break;
      default:
  StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }

  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_BLAST) {
      ShowFileWithSoundLength(img_.IMG_disarm, jetpack_font_config.ProffieOSDisarmImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};

/************************************\
|*                                  *|
|*   MORSECODE DISPLAY CONTROLLER   *|
|*                                  *|
\************************************/
/*
struct MorseCodeDisplayConfigFile : public ConfigFile {
  MorseCodeDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSShowMorseCodeImageDuration, 1000.0f);
  }

  // for OLED displays, the time a showmorsecode.bmp will play
  float ProffieOSShowMorseCodeImageDuration;
};

#define ONCE_PER_MORSECODE_EFFECT(X)  \
  X(showmorsecode)

template<typename PREFIX = ByteArray<>>
struct MorseCodeDisplayEffects  {
  MorseCodeDisplayEffects() : dummy_(0) ONCE_PER_MORSECODE_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_MORSECODE_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class MorseCodeDisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  MorseCodeDisplayEffects<PREFIX> img_;
  MorseCodeDisplayConfigFile &morsecode_font_config;
  MorseCodeDisplayController() :
    img_(*getPtr<MorseCodeDisplayEffects<PREFIX>>()),
    morsecode_font_config(*getPtr<MorseCodeDisplayConfigFile>()) {}

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_SHOWMORSECODE:
  this->SetMessage("add morsecode\ndisplay variable here"); // To do: create a function in morsecode_prop.h that will set the
  this->SetScreenNow(SCREEN_MESSAGE);                       // value of "variable" that can be used here.
        break;
      default:
  StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }

  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_SHOWMORSECODE) {
      ShowFileWithSoundLength(img_.IMG_showmorsecode, morsecode_font_config.ProffieOSShowMorseCodeImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};
*/

#endif  //MULTIPROPDISPLAYCONTROLLER_H

// ¯\_(ツ)_/¯
