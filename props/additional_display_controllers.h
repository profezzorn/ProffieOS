// version 07

/* Created by OlivierFlying747-8, inspired from the blaster display controller from ssd1306.
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2019 Fredrik Hubinette
  Copyright (c) 2025 OlivierFlying747-8 with contributions by:
  Fredrik Hubinette aka profezzorn,
  In case of problem, you can find us at: https://crucible.hubbe.net somebody will be there to help.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/
*/

/*
== How does this work? ==
=========================

You need to have in your config:
#define INCLUDE_SSD1306

If you have in your config:
#define ENABLE_SSD1306
only the standard display controller will be used and any custom display controller
below, will be ignored!

Add to each prop files that has it's own custom display controller
#ifdef INCLUDE_SSD1306
#ifndef EXTRA_DISPLAY_CONTROLLER_INCLUDE
#define EXTRA_DISPLAY_CONTROLLER_INCLUDE "props/additional_display_controllers.h"
#endif
#endif
*/

/*
If you want to create your own custom display controller for your prop(s), go to the bottom of this file, copy paste
everything below the "Create your own custom display controller" box. Replace any **yourstuff** with yourfiles.
Be mindful of the "ALL CAPS/no caps/Some Caps", just follow the examples. Hope you find this usefull. MTFBWY
*/

#ifndef ADDITIONAL_DISPLAY_CONTROLLERS_H
#define ADDITIONAL_DISPLAY_CONTROLLERS_H

/*************************************\
|*                                   *|
|*   MULTI_PROP DISPLAY CONTROLLER   *|
|*                                   *|
\*************************************/

#ifdef PROPS_MULTI_PROP_H
struct MultiPropDisplayConfigFile : public ConfigFile {
  MultiPropDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSSaberModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSBlasterModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSDetonatorModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSJetpackModeImageDuration, 1000.0f);
    //CONFIG_VARIABLE2(ProffieOSMorsecodeModeImageDuration, 1000.0f);
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
  //float ProffieOSMorsecodeModeImageDuration;
};

#define ONCE_PER_MULTIPROP_EFFECT(X)  \
  X(sabermode)                        \
  X(blastermode)                      \
  X(detonatormode)                    \
  X(jetpackmode)  // add backslash when uncommenting next line
  //X(morsecodemode)

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
        if (img_.IMG_sabermode) {                          // image of a saber
          ShowFileWithSoundLength(&img_.IMG_sabermode,     multiprop_font_config.ProffieOSSaberModeImageDuration);
        } else {
          this->SetMessage("saber\nmode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_BLASTERMODE:
        if (img_.IMG_blastermode) {                        // image of Han's blaster
          ShowFileWithSoundLength(&img_.IMG_blastermode,   multiprop_font_config.ProffieOSBlasterModeImageDuration);
        } else {
          this->SetMessage("blaster\n mode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_DETONATORMODE:
        if (img_.IMG_detonatormode) {                      // image of a detonator
          ShowFileWithSoundLength(&img_.IMG_detonatormode, multiprop_font_config.ProffieOSDetonatorModeImageDuration);
        } else {
          this->SetMessage("detonator\n  mode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_JETPACKMODE:
        if (img_.IMG_jetpackmode) {                        // image of Mando's jetpack
          ShowFileWithSoundLength(&img_.IMG_jetpackmode,   multiprop_font_config.ProffieOSJetpackModeImageDuration);
        } else {
          this->SetMessage("jetpack\nmode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

/*
      case EFFECT_MORSECODEMODE:
        if (img_.IMG_morsecodemode) {                      // Image of S.O.S. in morse code
          ShowFileWithSoundLength(&img_.IMG_morsecodemode, multiprop_font_config.ProffieOSMorsecodeModeImageDuration);
        } else {
          this->SetMessage("morse code\n  mode");
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
      ShowFileWithSoundLength(img_.IMG_sabermode, multiprop_font_config.ProffieOSSaberModeImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};
#endif // PROPS_MULTI_PROP_H

/**********************************************\
|*                                            *|
|*   JETPACK OLI BUTTONS DISPLAY CONTROLLER   *|
|*                                            *|
\**********************************************/

#ifdef PROPS_JETPACK_OLI_BUTTONS_H
struct JetpackDisplayConfigFile : public ConfigFile {
  JetpackDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    // == jetpack ==
    CONFIG_VARIABLE2(ProffieOSStartIdleModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSIdleModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSStartFlightModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSFlightModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSStopFlightModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSStopIdleModeImageDuration, 1000.0f);
    // ==  misshaps ==
    CONFIG_VARIABLE2(ProffieOSFalseStartImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSStutteringImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSSelfDestructImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMeltdownImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSDudImageDuration, 1000.0f);
    // == missile ==
    CONFIG_VARIABLE2(ProffieOSAimingImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSTargettingImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMissileLaunchImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMissileGoesBoomImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMandoTalkImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSDisarmImageDuration, 1000.0f);
  }
  // == jetpack ==
  // for OLED displays, the time a idlemode.bmp will play
  float ProffieOSIdleModeImageDuration;
  // for OLED displays, the time a startidlemode.bmp will play
  float ProffieOSStartIdleModeImageDuration;
  // for OLED displays, the time a startflightmode.bmp will play
  float ProffieOSStartFlightModeImageDuration;
  // for OLED displays, the time a flightmode.bmp will play
  float ProffieOSFlightModeImageDuration;
  // for OLED displays, the time a stopflightmode.bmp will play
  float ProffieOSStopFlightModeImageDuration;
  // for OLED displays, the time a stopidlemode.bmp will play
  float ProffieOSStopIdleModeImageDuration;
  // ==  misshaps ==
  // for OLED displays, the time a falsestart.bmp will play
  float ProffieOSFalseStartImageDuration;
  // for OLED displays, the time a stuttering.bmp will play
  float ProffieOSStutteringImageDuration;
  // for OLED displays, the time a selfdestruct.bmp will play
  float ProffieOSSelfDestructImageDuration;
  // for OLED displays, the time a meltdown.bmp will play
  float ProffieOSMeltdownImageDuration;
  // for OLED displays, the time a dud.bmp will play
  float ProffieOSDudImageDuration;
  // == missile ==
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
/* == jetpack == */                 \
  X(startidlemode)                  \
  X(idlemode)                       \
  X(startflightmode)                \
  X(flightmode)                     \
  X(stopflightmode)                 \
  X(stopidlemode)                   \
/* ==  misshaps == */               \
  X(falsestart)                     \
  X(stuttering)                     \
  X(selfdestruct)                   \
  X(meltdown)                       \
  X(dud)                            \
/* == missile == */                 \
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
      // jetpack normal operation display effects:
      // =========================================
      case EFFECT_STARTIDLEMODE:
        if (img_.startidlemode) {                            // animation of flame going from zero to small
          ShowFileWithSoundLength(&img_.startidlemode,       jetpack_font_config.ProffieOSStartIdleModeImageDuration);
        } else {
          this->SetMessage("jetpack\nigniting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_IDLEMODE:
        if (img_.idlemode) {                                 // animation of small oscillating flame
          ShowFileWithSoundLength(&img_.idlemode,            jetpack_font_config.ProffieOSIdleModeImageDuration);
        } else {
          this->SetMessage("jetpack\nidling");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STARTFLIGHTMODE:
        if (img_.startflightmode) {                          // animation of flame going from small to large
          ShowFileWithSoundLength(&img_.startflightmode,     jetpack_font_config.ProffieOSStartFlightModeImageDuration);
        } else {
          this->SetMessage("jetpack\nstarting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_FLIGHTMODE:
        if (img_.flightmode) {                               // animation of large oscillating flame
          ShowFileWithSoundLength(&img_.flightmode,          jetpack_font_config.ProffieOSFlightModeImageDuration);
        } else {
          this->SetMessage("jetpack\nflying");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STOPFLIGHTMODE:
        if (img_.stopflightmode) {                           // animation of flame going from large to small
          ShowFileWithSoundLength(&img_.stopflightmode,      jetpack_font_config.ProffieOSStopFlightModeImageDuration);
        } else {
          this->SetMessage("jetpack\nshutdown");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STOPIDLEMODE:
        if (img_.stopidlemode) {                             // animation of flame going small to zero
          ShowFileWithSoundLength(&img_.stopidlemode,        jetpack_font_config.ProffieOSStopIdleModeImageDuration);
        } else {
          this->SetMessage("  jetpack\ncompletely off");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      // jetpack misshaps display effects:
      // =================================
      case EFFECT_FALSESTART:
        if (img_.IMG_falsestart) {                           // Jetpack with "smoke fart" animation
          ShowFileWithSoundLength(&img_.IMG_falsestart,      jetpack_font_config.ProffieOSFalseStartImageDuration);
        } else {
          this->SetMessage("jetpack\nfalse start");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STUTTERING:
        if (img_.IMG_stuttering) {                           // Jetpack with trail of "smoke farts" animation
          ShowFileWithSoundLength(&img_.IMG_stuttering,      jetpack_font_config.ProffieOSStutteringImageDuration);
        } else {
          this->SetMessage("jetpack\nstuttering");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_SELFDESTRUCT:
        if (img_.selfdestruct) {                             // Jetpack exploding animation
          ShowFileWithSoundLength(&img_.selfdestruct,        jetpack_font_config.ProffieOSSelfDestructImageDuration);
        } else {
          this->SetMessage("jetpack\nself destruct");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MELTDOWN:
        if (img_.meltdown) {                                 // Jetpack melting animation
          ShowFileWithSoundLength(&img_.meltdown,            jetpack_font_config.ProffieOSMeltdownImageDuration);
        } else {
          this->SetMessage("jetpack\nmelting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_DUD:
        if (img_.dud) {
          ShowFileWithSoundLength(&img_.dud,                 jetpack_font_config.ProffieOSDudImageDuration);
        } else {
          this->SetMessage("it's a\ndud");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      // jetpack missile display effects: Make animations for OLED from the episode from Mando S02E06
      // ================================
      case EFFECT_AIMING:
        if (img_.IMG_aiming) {                               // animation of viewfinder coming down
          ShowFileWithSoundLength(&img_.IMG_aiming,          jetpack_font_config.ProffieOSAimingImageDuration);
        } else {
          this->SetMessage("aiming");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_TARGETTING:
        if (img_.IMG_targetting) {                           // animation of targetting
          ShowFileWithSoundLength(&img_.IMG_targetting,      jetpack_font_config.ProffieOSTargettingImageDuration);
        } else {
          this->SetMessage("targetting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MISSILELAUNCH:
        if (img_.IMG_missilelaunch) {                        // animation of jetpack launching missile
          ShowFileWithSoundLength(&img_.IMG_missilelaunch,   jetpack_font_config.ProffieOSMissileLaunchImageDuration);
        } else {
          this->SetMessage("launching\nmissile");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MISSILEGOESBOOM:
        if (img_.IMG_missilegoesboom) {                      // animation of explosion
          ShowFileWithSoundLength(&img_.IMG_missilegoesboom, jetpack_font_config.ProffieOSMissileGoesBoomImageDuration);
        } else {
          this->SetMessage("missile\nexplodes");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MANDOTALK:
        if (img_.IMG_mandotalk) {                            // animation of mando & boba talking
          ShowFileWithSoundLength(&img_.IMG_mandotalk,       jetpack_font_config.ProffieOSMandoTalkImageDuration);
        } else {
          this->SetMessage("nice shot");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_DISARM:
        if (img_.IMG_disarm) {                               // animation of viewfinder going back up
          ShowFileWithSoundLength(&img_.IMG_disarm,          jetpack_font_config.ProffieOSDisarmImageDuration);
        } else {
          this->SetMessage("disarming");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
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
#endif // PROPS_JETPACK_OLI_BUTTONS_H

/************************************\
|*                                  *|
|*   MORSECODE DISPLAY CONTROLLER   *|
|*                                  *|
\************************************/

/*
// morsecode_prop.h is not ready and will most likely miss the OS8.x release.
#ifdef PROPS_MORSECODE_PROP_H
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
#endif // PROPS_MORSECODE_PROP_H
*/

#endif // ADDITIONAL_DISPLAY_CONTROLLERS_H

/*************************************************\       Insert your files names below,
|*                                               *|       replace everything between
|*   Create your own custom display controller   *|       double asterix '**' and
|*                                               *|       delete the **xxxx** parts.
\*************************************************/    // Be mindful of the "ALL CAPS/no caps/Some Caps"!
/*
#ifdef **YOUR_PROP_NAME**_H
struct **NameHereYour**DisplayConfigFile : public ConfigFile {
  **NameHereYour**DisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOS**YourImageOrAnimationName1Here**ImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOS**YourImageOrAnimationName2Here**ImageDuration, 1000.0f);
    //... for as many images or animations that you need;
  }

  // for OLED displays, the time a **yourimageoranimation1namehere**.bmp will play
  float ProffieOS**YourImageOrAnimation2NameHere**ImageDuration;
  // for OLED displays, the time a **yourimageoranimation2namehere**.bmp will play
  float ProffieOS**YourImageOrAnimation2NameHere**ImageDuration;
  //... for as many images or animations that you need;
};

#define ONCE_PER_**YOURPROPNAMEHERE**_EFFECT(X)  \
  X(**yourimageoranimation1namehere**)           \
  X(**yourimageoranimation2namehere**)
  //... for as many images or animations that you need, no backslash '\' on the last line of the list.

template<typename PREFIX = ByteArray<>>
struct **YourPropName**DisplayEffects  {
  **YourPropName**DisplayEffects() : dummy_(0) ONCE_PER_**YOURPROPNAMEHERE**_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_**YOURPROPNAMEHERE**_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class **YourProp**DisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  **YourProp**DisplayEffects<PREFIX> img_;
  **YourProp**DisplayConfigFile &morsecode_font_config;
  **YourProp**DisplayController() :
    img_(*getPtr<**YourProp**DisplayEffects<PREFIX>>()),
    **yourprop**_font_config(*getPtr<**YourProp**DisplayConfigFile>()) {}

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_**YOURIMAGEORANIMATION1NAMEHERE**:
        if (img_.IMG_**yourimageoranimation1namehere**) { // optional description of animation or image 1
          ShowFileWithSoundLength(&img_.IMG_**yourimageoranimation1namehere**, **yourprop**_font_config.ProffieOS**YourImageOrAnimation1NameHere**ImageDuration);
        } else {
          this->SetMessage("**your text1**");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_**YOURIMAGEORANIMATION2NAMEHERE**:
        if (img_.IMG_**yourimageoranimation2namehere**) { // optional description of animation or image 2
          ShowFileWithSoundLength(&img_.IMG_**yourimageoranimation2namehere**, **yourprop**_font_config.ProffieOS**YourImageOrAnimation2NameHere**ImageDuration);
        } else {
          this->SetMessage("**your text1**");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case   //... for as many images ,animations or texts that you need:
        ...
        break;

      default:
        StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }

  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_**YOURIMAGEORANIMATION1NAMEHERE**) {
      ShowFileWithSoundLength(img_.IMG_**yourimageoranimation1namehere**, **yourprop**_font_config.ProffieOS**YourImageOrAnimation1NameHere**ImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};
#endif // **YOUR_PROP_NAME**_H
*/
