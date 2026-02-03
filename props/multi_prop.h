// version 065.08 working version for saber-blaster(with or without bullet count)-detonator-jetpack,
// with switching presets sets, all with sound-effects on transition between props, provided by NoSloppy.

/* Created by OlivierFlying747-8,
  https://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2025 Fredrik Hubinette
  Copyright (c) 2025 OlivierFlying747-8 with contributions by:
  Fredrik Hubinette aka profezzorn,
  Ryan Ogurek aka ryryog25,
  Bryan Connor aka NoSloppy,
  Chris Carter aka Sabersense,
  In case of problem, you can find us at: https://crucible.hubbe.net somebody will be there to help.
  Distributed under the terms of the GNU General Public License v3.
  https://www.gnu.org/licenses/

Your prop NEEDS to have two or more buttons.
Multi_prop.h MUST be declared before the other props.
Props MUST be declared in the right order & one and only one of each type MUST be included.

multi_prop was created to be compatible with ProffieOS v8.x

multi_prop.h allows for 4 (maybe more coming) discrete prop files to be used, alternating on a button combo
depending on how many buttons you have or witch prop you want to use with or without a main blade attached.

A "triple clic" on AUX2 will swap between props, but "triple click" on AUX on a 2 buttons prop will also work (if you are
not using saber_sa22c_buttons.h as your saber of choice). An "EVENT_FOURTH_CLICK_LONG" on AUX should work
for all props but I am unable to perform the move.

Idealy, I would like to create a "MULTI_PROP_BUTTON_HELD_XTRA_LONG_TIMEOUT event" so multi_prop could be used
with only one button. The more I look into this, the less sure I become that it will be possible.

This prop is for fun and is not Star Wars cannon. It only exists to add extra functionality to your saber, if you so choose.
Thank you for reading.

You can find all sound files (created by NoSloppy) used in multi_prop.h on my Dropbox here:
https://www.dropbox.com/scl/fi/p2pj9os5v4seel0wmzwcz/Multi_prop_sounds_by_NoSloppy.zip?rlkey=hi6589mexymnx4jmxhtwqjfu0&st=3klofs5m&dl=0

Thank you for reading.

How to use: add this to your config in the CONFIG_PROP section:
#ifdef CONFIG_PROP
#include "../props/multi_prop.h"              //use MultiProp <...> (choose only 1 saber & 1 blaster)

// *** List of sabers (choose one and only one) ***
// ================================================
                                                //What to use in PROP_TYPE MultiProp <...>
//#include "../props/saber.h"                   //use Saber                | working with multi_prop.h
//#include "../props/saber_BC_buttons.h"        //use SaberBCButtons       | working with multi_prop.h
//#include "../props/saber_caiwyn_buttons.h"    //use CaiwynButtons        | <-- works alone but NOT compiling with multi_prop.h or dual_prop.h because it is missing "virtual" in "class CaiwynButtons : public PropBase" - easy fix!)
  #include "../props/saber_fett263_buttons.h"   //use SaberFett263Buttons  | working with multi_prop.h
//#include "../props/saber_sa22c_buttons.h"     //use SaberSA22CButtons    | working with multi_prop.h
//#include "../props/saber_sabersense_buttons.h"//use SabersenseButtons    |  untested with multi_prop.h
//#include "../props/saber_shtok_buttons.h"     //use SaberShtokButtons    | working with multi_prop.h

// *** List of blasters (choose one and only one) ***
// ==================================================
  #include "../props/blaster.h"                 //use Blaster              | working with multi_prop.h
//#include "../props/blaster_BC_buttons.h"      //use BlasterBCButtons     | working with multi_prop.h after a two lines modification
//#include "../props/laser_musket_buttons"      //use LaserMusketButtons   |  untested with multi_prop.h but it is not stock compatible because it uses pow & aux buttons instead of fire & mode select.
//#include "../props/blaster_Teas_buttons"      //use BlasterTEA           |  untested with multi_prop.h

// IMPORTANT NOTE: blaster_BC_buttons.h (at the time of writing) is not "stock" compatible with multi_prop.h
// A small modification needs to be added to it's code. I will let NoSloppy decide for the best course of action
// to modify his prop. I do have "my version" of blaster_BC_buttons.h working, so it is possible!

// *** List of other prop types (choose one of each) ***
// =====================================================
//#include "../props/detonator.h"               //use Detonator            | Compiling with multi_prop.h but it will NOT work due to it's code used for the latching button.
  #include "../props/detonator_Oli_buttons.h"   //use DetonatorOliButtons  | Working with multi_prop.h
  #include "../props/jetpack_Oli_buttons.h"     //use JetpackOliButtons    | Working with multi_prop.h
//#include "../props/morsecode_prop.h"          //use MorseCode            | Compiling with multi_prop.h but far from ready!

#undef PROP_TYPE
#define PROP_TYPE MultiProp <SaberFett263Buttons, Blaster, DetonatorOliButtons, JetpackOliButtons>
#endif

// Now setup your CONFIG_PRESETS section to have multiple preset banks like this:
// (same-ish as if using blade id - but multi_prop is not compatible with using the "real" blade id!)
// multi_prop is also not compatible with BLADE_DETECT but I can hopefully change that.

Preset presets_saber[] = {
  // with your saber presets list here.
};
Preset presets_blaster[] = {
  // with your blaster presets list here.
};
Preset presets_detonator[] = {
  // with your detonator presets list here.
};
Preset presets_jetpack[] = {
  // with your jetpack presets list here.
};

// ... for the future props when they will be ready. Dot not add for now!
Preset presets_morsecode[] = {
  // with your morsecode presets list here.
};
Preset presets_droid[] = {
  // with your droid presets list here.
};

Also, the BladeConfig needs each blade to have a description entry in
the same order of the Presets arrays like this:
BladeConfig blades[] = {
 { 0, //SABER
    // with your blade(s) definition(s) here.
    CONFIGARRAY(presets_saber), "__saber_save" },
 { 1, //BLASTER
    // with your blade(s) definition(s) here.
    CONFIGARRAY(presets_blaster), "__blaster_save" },
 { 2, //DETONATOR
    // with your blade(s) definition(s) here.
    CONFIGARRAY(presets_detonator), "__detonator_save" },
 { 3, //JETPACK
    // with your blade(s) definition(s) here.
    CONFIGARRAY(presets_jetpack), "__jetpack_save" },
};

// 4 & 5 may be added in the future, when the respective props are ready. Do not add them for now!
 { 4, //MORSE_CODE
    // with your blade(s) definition(s) here.
    CONFIGARRAY(presets_morsecode), "__morsecode_save" },
 { 5, //DROID
    // with your blade(s) definition(s) here.
    CONFIGARRAY(presets_droid), "__droid_save" },
*/
// https://pod.hubbe.net/howto/making-your-own-prop-file.html

#ifndef PROPS_MULTI_PROP_H
#define PROPS_MULTI_PROP_H

#ifdef PROP_INHERIT_PREFIX
#error multi_prop.h must be included first
#else
#define PROP_INHERIT_PREFIX virtual
#endif

#if NUM_BUTTONS < 2
#error multi_prop requires 2 Buttons for operation (for now)!
#endif

// I am still working on this!
#ifndef MULTI_PROP_BUTTON_HELD_XTRA_LONG_TIMEOUT
#define MULTI_PROP_BUTTON_HELD_XTRA_LONG_TIMEOUT 1000 * 4
#endif

// Define FakeBladeID structure
struct FakeBladeID {                                       //
  static int return_value;  // Holds the current mode ID   //
  // Method to return the current blade ID                 //
  float id() { return return_value; }                      //
  // Method to set the blade ID based on the mode          //
  static void SetFakeBlade(int id) {                       //
    return_value = id;                                     //
  }                                                        // Sabersense code part 1 of 2
};                                                         // adapted to multi_prop.h
// Initialize return_value to a default of 0 - SABER mode  //
int FakeBladeID::return_value = 0;                         //
// Redefine the blade ID class to use FakeBladeID          //
#undef  BLADE_ID_CLASS_INTERNAL                            //
#define BLADE_ID_CLASS_INTERNAL FakeBladeID                //
#undef  BLADE_ID_CLASS                                     //
#define BLADE_ID_CLASS FakeBladeID                         //

#include "prop_base.h"

// Define sound effects
EFFECT(sabermode);
EFFECT(blastermode);
EFFECT(detonatormode);
EFFECT(jetpackmode);
//NO-EFFECT(morsecodemode); No need for a "morsecode.wav" file. This is done using "Beepers" in Morse code.
//EFFECT(droidmode);          // Un-comment if implementing droid   (wav available - but no prop yet)

template<class Saber, class Blaster, class Detonator,
         class Jetpack> /*, class MorseCode, class Droid*/
class MultiProp : public virtual Saber, public virtual Blaster, public virtual Detonator,
                  public virtual Jetpack { /*, public virtual MorseCode, public virtual Droid*/
                  // One prop to rule them all, and in your config bind them!
public:
  // Button mapping for 2 and 3-button setups
    // The "Blaster mapping"
  uint32_t map_button(uint32_t b) {
    switch (b) {
/*
#ifdef BLADE_DETECT_PIN
    case BUTTON_BLADE_DETECT:  return BUTTON_CLIP_DETECT; // Could this work ???
#endif
*/

#if NUM_BUTTONS == 3
      case BUTTON_POWER:       return BUTTON_RELOAD;
      case BUTTON_AUX:         return BUTTON_FIRE;
      case BUTTON_AUX2:        return BUTTON_MODE_SELECT;
#else
      case BUTTON_POWER:       return BUTTON_FIRE;
      case BUTTON_AUX:         return BUTTON_MODE_SELECT;
#endif
      default: return b;
    }
  }

    // The "return to normal mapping"
  uint32_t reverse_map_button(uint32_t b) {
    switch (b) {
/*
#ifdef BLADE_DETECT_PIN
    case BUTTON_CLIP_DETECT:   return BUTTON_BLADE_DETECT; // Could this work ???
#endif
*/

#if NUM_BUTTONS == 3
      case BUTTON_RELOAD:      return BUTTON_POWER;
      case BUTTON_FIRE:        return BUTTON_AUX;
      case BUTTON_MODE_SELECT: return BUTTON_AUX2;
#else
      case BUTTON_FIRE:        return BUTTON_POWER;
      case BUTTON_MODE_SELECT: return BUTTON_AUX;
#endif
      default: return b;
    }
  }
  const char* name() override { return "MultiProp"; }

  // Enumeration for modes
  enum class Prop_Mode {
        SABER = 0,
      BLASTER = 1,
    DETONATOR = 2,
      JETPACK = 3,  // Ready (I think). I need to do a few tweeks!
  //MORSECODE = 4,  // Not fully ready, but in progress!
      //DROID = 6,  // Un-comment when ready to implement Droid functionality.
    //VEHICLE = 7,  // Un-comment if implementing Vehicle.
                    /* I have not decided what this prop should do, but it would be cool to play with STARWARS "vehicles" sounds:
                       speeders, pod-racers, TIE's, A/B/X/U-Wings, tanks, Star Destroyers and any other ships/vehicles
                       that we can think of!
                       On further thoughts, "vehicle prop ideas" will be merged into jetpack prop because I believe
                       they will do the same things (play with vehicles engines & weapons sounds). */
      // TRON = 8,  // I also had the idea for a Tron Disk Controller, but that has apparently been done already.
                    /* I just need to put my hands on one of those "tron_prop.h" (wink) to see if I can make multi_prop
                       compatible with it!
      Please feel free to submit more "multi_prop fun ideas" on The Crucible or to my GitHub page:
      https://github.com/olivierflying747-8/Olis-ProffieOS/tree/06_props_Oli_buttons_SUGGESTIONS_WELCOME */
  };

  Prop_Mode currentMode_ = Prop_Mode::SABER;  // Initial state is Saber

  // Helper function to set FakeBladeID and re-detect the blade
  void updateBladeIDAndDetect(int blade_id) {
    FakeBladeID::SetFakeBlade(blade_id);             //
    PVLOG_DEBUG << "*** Fake blade id'ed\n";         // Sabersense code part 2 of 2
    PropBase::FindBladeAgain();                      // adapted to multi_prop.h
    SaberBase::DoBladeDetect(true);                  //
  }

  // Switch modes helper to reduce repetition.
  void swapProp(Prop_Mode modenext, const char* message, int blade_id, EffectType effect) {
    PropBase::Off(PropBase::OFF_FAST);
    PVLOG_STATUS << message << "\n";
    currentMode_ = modenext;
    updateBladeIDAndDetect(blade_id);
    SaberBase::DoEffect(effect, 0);
    PVLOG_VERBOSE << "*** One prop to rule them all, and in your config bind them!\n";
  }

  // Function to handle mode switching between props, it was my very repetitive, repeating code!
  // Set ID 0 for SABER mode, 1 for BLASTER mode, 2 for DETONATOR mode,
  //        3 for JETPACK mode, 4 for MORSECODE mode & 5 for DROID mode.
  void switchModes() {
    switch (currentMode_) {
      // Arranged in "columns" to make it much easier to see if something is missing when adding a new "prop-mode".
      case Prop_Mode::SABER:     swapProp(Prop_Mode::BLASTER,   "*** Blaster Mode",    1, EFFECT_BLASTERMODE);   break;
      case Prop_Mode::BLASTER:   swapProp(Prop_Mode::DETONATOR, "*** Detonator Mode",  2, EFFECT_DETONATORMODE); break;
      case Prop_Mode::DETONATOR: swapProp(Prop_Mode::JETPACK,   "*** Jetpack Mode",    3, EFFECT_JETPACKMODE);   break;
      case Prop_Mode::JETPACK:   swapProp(Prop_Mode::SABER,     "*** Saber Mode",      0, EFFECT_SABERMODE);     break;
      /*
                                 swapProp(Prop_Mode::MORSECODE, "*** Morse Code Mode", 4, EFFECT_MORSECODEMODE); break;
      case Prop_Mode::MORSECODE:
                                 swapProp(Prop_Mode::DROID,     "*** Droid Mode",      5, EFFECT_DROIDMODE);     break;
      case Prop_Mode::DROID:     swapProp(Prop_Mode::SABER,     "*** Saber Mode",      0, EFFECT_SABERMODE);     break;
      */
    }
  }

  bool Event(enum BUTTON button, EVENT event) override {
    switch (EVENTID(button, event, 0)) { // To do: replace with MULTI_PROP_EVENT_HELD_XTRA_LONG (once I get it working)

#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_AUX2, EVENT_THIRD_SAVED_CLICK_SHORT, 0): // Obviously, you need 3 buttons for this to work.
#else
      case EVENTID(BUTTON_AUX,  EVENT_FOURTH_CLICK_LONG, 0):       // This should work but I can't seem to do a 4th click long?
      case EVENTID(BUTTON_AUX,  EVENT_THIRD_SAVED_CLICK_SHORT, 0): // This will work except with saber_sa22c_buttons.h who also uses it!
#endif

        switchModes();
        return true;
    }
    switch (currentMode_) {
      case Prop_Mode::SABER:     return     Saber::Event(button, event);
      case Prop_Mode::BLASTER: {
        button = static_cast<enum BUTTON>(map_button(button));

        // Map modifiers to blaster
        uint32_t m = current_modifiers;
        current_modifiers = 0;
        for (; m; m &= m - 1) current_modifiers |= map_button(m & -m);

        bool ret =  /* . . . . . . . . */ Blaster::Event(button, event);
        PVLOG_DEBUG << "*** Blaster map_button\n";

        // Map modifiers back to normal
        m = current_modifiers;
        current_modifiers = 0;

        for (; m; m &= m - 1) current_modifiers |= reverse_map_button(m & -m);
        return ret;
        PVLOG_DEBUG << "*** Back to normal map_button\n";
      }
      case Prop_Mode::DETONATOR: return Detonator::Event(button, event);
      case Prop_Mode::JETPACK:   return   Jetpack::Event(button, event);
    //case Prop_Mode::MORSECODE: return MorseCode::Event(button, event);
    //case Prop_Mode::DROID:     return     Droid::Event(button, event);
    }
    return false;
  }

/*****************************************************************************************\
| I tried with lamdbas functions to reduce repetition further, but it was taking a lot    |
| more memory, and adding a lot more complexity. I guess, I must be like Gollum, he       |
| doesn't like Elven Lemdbas bread either!!! Yes I think this is funny, and my kids too!  |
|                                                                                         |
| I lined up everything in "columns"to make the code much easier to read. IMHO! ¯\_(")_/¯ |
\*****************************************************************************************/

  // I am not sure if this part is needed because I didn't have a compile error with mode_Event2 YET!
  // But I thought it would be a good idea to have it ready here for WHEN a conflict happens.
  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) {
    switch (currentMode_) {
      case Prop_Mode::SABER:     return     Saber::mode_Event2(button, event, modifiers);
      case Prop_Mode::BLASTER:   return   Blaster::mode_Event2(button, event, modifiers);
      case Prop_Mode::DETONATOR: return Detonator::mode_Event2(button, event, modifiers);
      case Prop_Mode::JETPACK:   return   Jetpack::mode_Event2(button, event, modifiers);
    //case Prop_Mode::MORSECODE: return MorseCode::mode_Event2(button, event, modifiers);
    //case Prop_Mode::DROID:     return     Droid::mode_Event2(button, event, modifiers);
    }
    return false;
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (currentMode_) {
      case Prop_Mode::SABER:     return     Saber::Event2(button, event, modifiers);
      case Prop_Mode::BLASTER:   return   Blaster::Event2(button, event, modifiers);
      case Prop_Mode::DETONATOR: return Detonator::Event2(button, event, modifiers);
      case Prop_Mode::JETPACK:   return   Jetpack::Event2(button, event, modifiers);
    //case Prop_Mode::MORSECODE: return MorseCode::Event2(button, event, modifiers);
    //case Prop_Mode::DROID:     return     Droid::Event2(button, event, modifiers);
    }
    return false;
  }

  void SetPreset(int preset_num, bool announce) override {
    switch (currentMode_) {
      case Prop_Mode::SABER:         Saber::SetPreset(preset_num, announce); break;
      case Prop_Mode::BLASTER:     Blaster::SetPreset(preset_num, announce); break;
      case Prop_Mode::DETONATOR: Detonator::SetPreset(preset_num, announce); break;
      case Prop_Mode::JETPACK:     Jetpack::SetPreset(preset_num, announce); break;
    //case Prop_Mode::MORSECODE: MorseCode::SetPreset(preset_num, announce); break;
    //case Prop_Mode::DROID:         Droid::SetPreset(preset_num, announce); break;
    }
  }

  void Loop() override {
    switch (currentMode_) {
      case Prop_Mode::SABER:         Saber::Loop(); break;
      case Prop_Mode::BLASTER:     Blaster::Loop(); break;
      case Prop_Mode::DETONATOR: Detonator::Loop(); break;
      case Prop_Mode::JETPACK:     Jetpack::Loop(); break;
    //case Prop_Mode::MORSECODE: MorseCode::Loop(); break;
    //case Prop_Mode::DROID:         Droid::Loop(); break;
    }
  }

  void Setup() override {
        Saber::Setup();
      Blaster::Setup();
    Detonator::Setup();
      Jetpack::Setup();
  //MorseCode::Setup();
      //Droid::Setup();
  }

  bool chdir(StringPiece font) override {
    switch (currentMode_) {
      case Prop_Mode::SABER:     return     Saber::chdir(font);
      case Prop_Mode::BLASTER:   return   Blaster::chdir(font);
      case Prop_Mode::DETONATOR: return Detonator::chdir(font);
      case Prop_Mode::JETPACK:   return   Jetpack::chdir(font);
    //case Prop_Mode::MORSECODE: return MorseCode::chdir(font);
    //case Prop_Mode::DROID:     return     Droid::chdir(font);
      default: return false;
    }
  }

  bool Parse(const char* key, const char* val) override {
    switch (currentMode_) {
      case Prop_Mode::SABER:     return     Saber::Parse(key, val);
      case Prop_Mode::BLASTER:   return   Blaster::Parse(key, val);
      case Prop_Mode::DETONATOR: return Detonator::Parse(key, val);
      case Prop_Mode::JETPACK:   return   Jetpack::Parse(key, val);
    //case Prop_Mode::MORSECODE: return MorseCode::Parse(key, val);
    //case Prop_Mode::DROID:     return     Droid::Parse(key, val);
      default: return false;
    }
  }

  void DoMotion(const Vec3& motion, bool clear) override {
    switch (currentMode_) {
      case Prop_Mode::SABER:         Saber::DoMotion(motion, clear); break;
      case Prop_Mode::BLASTER:     Blaster::DoMotion(motion, clear); break;
      case Prop_Mode::DETONATOR: Detonator::DoMotion(motion, clear); break;
      case Prop_Mode::JETPACK:     Jetpack::DoMotion(motion, clear); break;
    //case Prop_Mode::MORSECODE: MorseCode::DoMotion(motion, clear); break;
    //case Prop_Mode::DROID:         Droid::DoMotion(motion, clear); break;
    }
  }

  void Clash(bool stab, float strength) override {
    switch (currentMode_) {
      case Prop_Mode::SABER:         Saber::Clash(stab, strength); break;
      case Prop_Mode::BLASTER:     Blaster::Clash(stab, strength); break;
      case Prop_Mode::DETONATOR: Detonator::Clash(stab, strength); break;
      case Prop_Mode::JETPACK:     Jetpack::Clash(stab, strength); break;
    //case Prop_Mode::MORSECODE: MorseCode::Clash(stab, strength); break;
    //case Prop_Mode::DROID:         Droid::Clash(stab, strength); break;
    }
  }

  void announcemode(Effect* sound_name) {
    if (!hybrid_font.PlayPolyphonic(sound_name)) {
      // Use beeper for fallback sounds
      beeper.Beep(0.05, 2000);
      beeper.Silence(0.05);
      beeper.Beep(0.05, 2000);
    };
  }

/* "void morsecodemode()" will be un-commented once morsecode_prop.h is ready.
  //(600Hz is the typical "aviation" morse code audio frequency - 800Hz is the frequency usually used in movies)
  void morsecodemode() { // It says -.- = "K" = General invitation to transmit // .--/-/..-. = "WTF"
    const float morsePattern[] = {0.6, 0.2, 0.6};
    for (float duration : morsePattern) {
      beeper.Beep(duration, 800); // Movies morse code frequency used.
      beeper.Silence(0.2);
    }
  }
*/

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_SABERMODE:     announcemode(&SFX_sabermode);     return;
      case EFFECT_BLASTERMODE:   announcemode(&SFX_blastermode);   return;
      case EFFECT_DETONATORMODE: announcemode(&SFX_detonatormode); return;
      case EFFECT_JETPACKMODE:   announcemode(&SFX_jetpackmode);   return;
    //case EFFECT_MORSECODEMODE: morsecodemode();                  return;
    //case EFFECT_DROIDMODE:     announcemode(&SFX_droidmode);     return;
    }
    switch (currentMode_) {
      case Prop_Mode::SABER:         Saber::SB_Effect(effect, location); break;
      case Prop_Mode::BLASTER:     Blaster::SB_Effect(effect, location); break;
      case Prop_Mode::DETONATOR: Detonator::SB_Effect(effect, location); break;
      case Prop_Mode::JETPACK:     Jetpack::SB_Effect(effect, location); break;
    //case Prop_Mode::MORSECODE: MorseCode::SB_Effect(effect, location); break;
    //case Prop_Mode::DROID:         Droid::SB_Effect(effect, location); break;
    }
  }
};

#endif // PROPS_MULTI_PROP_H

/*************************************\
|*                                   *|
|*   MULTI_PROP DISPLAY CONTROLLER   *|
|*                                   *|
\*************************************/

#ifdef PROP_BOTTOM

#define ONCE_PER_MULTIPROP_EFFECT(X)  \
  X(sabermode)                        \
  X(blastermode)                      \
  X(detonatormode)                    \
  X(jetpackmode)    // Add a backslash here when un-commenting the next line.
//X(morsecodemode)
//X(droid)

#ifdef INCLUDE_SSD1306

struct MultiPropDisplayConfigFile : public ConfigFile {
  MultiPropDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(ProffieOSSaberModeImageDuration,     1000.0f);
    CONFIG_VARIABLE2(ProffieOSBlasterModeImageDuration,   1000.0f);
    CONFIG_VARIABLE2(ProffieOSDetonatorModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSJetpackModeImageDuration,   1000.0f);
  //CONFIG_VARIABLE2(ProffieOSMorsecodeModeImageDuration, 1000.0f);
  //CONFIG_VARIABLE2(ProffieOSDroidModeImageDuration,     1000.0f);
  }

  // for OLED displays, the time a sabermode.bmp     will play.
  float ProffieOSSaberModeImageDuration;
  // for OLED displays, the time a blastermode.bmp   will play.
  float ProffieOSBlasterModeImageDuration;
  // for OLED displays, the time a detonatormode.bmp will play.
  float ProffieOSDetonatorModeImageDuration;
  // for OLED displays, the time a jetpackmode.bmp   will play.
  float ProffieOSJetpackModeImageDuration;
  // for OLED displays, the time a morsecodemode.bmp will play.
//float ProffieOSMorsecodeModeImageDuration;
  // for OLED displays, the time a droidmode.bmp     will play.
//float ProffieOSDroidModeImageDuration;
};

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
        if (img_.IMG_sabermode) {                          // Image of Luke's and Darth Vader's sabers.
          ShowFileWithSoundLength(&img_.IMG_sabermode,     multiprop_font_config.ProffieOSSaberModeImageDuration);
        } else {
          this->SetMessage("saber\nmode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_BLASTERMODE:
        if (img_.IMG_blastermode) {                        // Image of Han's blaster.
          ShowFileWithSoundLength(&img_.IMG_blastermode,   multiprop_font_config.ProffieOSBlasterModeImageDuration);
        } else {
          this->SetMessage("blaster\n mode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_DETONATORMODE:
        if (img_.IMG_detonatormode) {                      // Image of a detonator.
          ShowFileWithSoundLength(&img_.IMG_detonatormode, multiprop_font_config.ProffieOSDetonatorModeImageDuration);
        } else {
          this->SetMessage("detonator\n  mode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_JETPACKMODE:
        if (img_.IMG_jetpackmode) {                        // Image of Mando's jetpack.
          ShowFileWithSoundLength(&img_.IMG_jetpackmode,   multiprop_font_config.ProffieOSJetpackModeImageDuration);
        } else {
          this->SetMessage("jetpack\nmode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

/*
      case EFFECT_MORSECODEMODE:
        if (img_.IMG_morsecodemode) {                      // Image of S.O.S. in morse code.
          ShowFileWithSoundLength(&img_.IMG_morsecodemode, multiprop_font_config.ProffieOSMorsecodeModeImageDuration);
        } else {
          this->SetMessage("morse code\n  mode");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_DROIDMODE:
        if (img_.IMG_droidmode) {                          // Image of R2D2 & C3PO.
          ShowFileWithSoundLength(&img_.IMG_droidmode, multiprop_font_config.ProffieOSDroidModeImageDuration);
        } else {
          this->SetMessage("droid\nmode");
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

#endif  // INCLUDE_SSD1306

template<int W, int H, typename PREFIX = ConcatByteArrays<typename NumberToByteArray<W>::type, ByteArray<'x'>,
         typename NumberToByteArray<H>::type>>
class MultiPropColorDisplayController : public StandarColorDisplayController<W, H, PREFIX> {
public:
  template<int w, int h>
  explicit MultiPropColorDisplayController(SizedLayeredScreenControl<w, h>* screen) :
  StandarColorDisplayController<W, H, PREFIX>(screen) ONCE_PER_MULTIPROP_EFFECT(INIT_SCR) {
  }
  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_SABERMODE:     this->scr_.Play(&SCR_sabermode);     break;
      case EFFECT_BLASTERMODE:   this->scr_.Play(&SCR_blastermode);   break;
      case EFFECT_DETONATORMODE: this->scr_.Play(&SCR_detonatormode); break;
      case EFFECT_JETPACKMODE:   this->scr_.Play(&SCR_jetpackmode);   break;
    //case EFFECT_MORSECODEMODE: this->scr_.Play(&SCR_morsecodemode); break;
    //case EFFECT_DROIDMODE:     this->scr_.Play(&SCR_droidmode);     break;
      default:
        StandarColorDisplayController<W, H, PREFIX>::SB_Effect2(effect, location);
    }
  }

protected:
  ONCE_PER_MULTIPROP_EFFECT(DEF_SCR);
};

#undef ONCE_PER_MULTIPROP_EFFECT

#endif  // PROP_BOTTOM

