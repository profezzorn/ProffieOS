// version 066 (compiling version for saber-blaster(with bullet count)-detonator-jetpack, all with sound-effects on
//             (but-untested as my saber is not yet working)                              transition between props.
//             (switching presets sets)                                                   (provided by NoSloppy)

/* Created by OlivierFlying747-8 with lots of help from Fredrik Hubinette aka profezzorn,
  https://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2025 Fredrik Hubinette
  Copyright (c) 2025 OlivierFlying747-8 with contributions by:
  Fredrik Hubinette aka profezzorn,
  Ryan Ogurek aka ryryog25,
  Bryan Connor aka NoSloppy,
  In case of problem, you can find us at: https://crucible.hubbe.net somebody will be there to help.
  Distributed under the terms of the GNU General Public License v3.
  https://www.gnu.org/licenses/

/*
multi_prop.h allows for 4 (maybe more coming) discrete prop files to be used,
alternating on a 2 buttons extra long push (4 seconds - customisable with #define TWO_BUTTONS_X_LONG_PUSH 1000 * 4)
with or without a main blade attached.
Blade detect function has no effect on multi_prop.h (other than playing blade in/out wav)
Your prop NEEDS to have two or more buttons.
Multi_prop.h MUST be declared before the other props.
Props MUST be declared in the right order & one and only one of each type MUST be included.

This prop is for fun and is not Star Wars cannon. It only exists to add extra functionality to your saber, if you so choose.
Thank you for reading.

You can find all 4 (5) sound files (created by NoSloppy) used in multi_prop.h on my dropbox here:
https://www.dropbox.com/scl/fi/p2pj9os5v4seel0wmzwcz/Multi_prop_sounds_by_NoSloppy.zip?rlkey=hi6589mexymnx4jmxhtwqjfu0&st=3klofs5m&dl=0

How to use: add this to your config
#ifdef CONFIG_PROP                              //What to use in PROP_TYPE MultiProp <...>
  #include "../props/multi_prop.h"              //use MultiProp <...> (choose only 1 saber & 1 blaster)
//#include "../props/saber.h"                   //use Saber                | Compiling with multi_prop.h
//#include "../props/saber_BC_buttons.h"        //use SaberBCButtons       | Compiling with multi_prop.h
//#include "../props/saber_caiwyn_buttons.h"    //do not use CaiwynButtons <-- works alone | NOT compiling with multi_prop.h (I don't know why? Well, I didn't understand the error codes, back then! Might need a re-visit? Also Caiwyn's saber didn't compile with dual_prop.h back at the time of testing)
  #include "../props/saber_fett263_buttons.h"   //use SaberFett263Buttons  | Compiling with multi_prop.h
//#include "../props/saber_sa22c_buttons.h"     //use SaberSA22CButtons    | Compiling with multi_prop.h
//#include "../props/saber_sabersense_buttons.h //use SabersenseButtons    | Compiling with multi_prop.h
//#include "../props/saber_shtok_buttons.h"     //use SaberShtokButtons    | Compiling with multi_prop.h
  #include "../props/blaster.h"                 //use Blaster              | compiling with multi_prop.h (works with or without "bullet count" in CONFIG_BOTTOM)
//#include "../props/blaster_BC_buttons.h"      //use BlasterBCButtons     | compiling with multi_prop.h but only with SaberBCButtons
  #include "../props/detonator.h"               //use Detonator            | Compiling with multi_prop.h
  #include "../props/jetpack_Oli_buttons.h"     //use JetpackOliButtons    | Compiling with multi_prop.h
#undef PROP_TYPE
#define PROP_TYPE MultiProp <SaberFett263Buttons, Blaster, Detonator, JetpackOliButtons>
#endif
// also you will need to set your sets of preset(s) & sets of blade(s) arrays (0, 1, 2, 3) (4, 5 & 6 maybe later)
// accordingly (same-ish as if using blade id - but this is not compatible with using the "real" blade id).
*/

#ifndef PROPS_MULTI_PROP_H
#define PROPS_MULTI_PROP_H

#ifdef PROP_INHERIT_PREFIX
#error multi_prop.h must be included first
#else
#define PROP_INHERIT_PREFIX virtual
#endif

#if NUM_BUTTONS < 2
#error Your prop NEEDS 2 or more buttons to use multi_prop
#endif

#include "prop_base.h"

// Define FakeBladeID structure
struct FakeBladeID {                                              //
  static int return_value;  // Holds the current mode ID          //
  // Method to return the current blade ID                        //
  float id() { return return_value; }                             //
  // Method to set the blade ID based on the mode                 //
  static void SetFakeBlade(int id) {                              //
    return_value = id;                                            //
  }                                                               // Sabersense code part1
};                                                                // adapted to multi_prop.h
// Initialize return_value to a default of 0 - SABER mode         //
int FakeBladeID::return_value = 0;                                //
// Redefine the blade ID class to use FakeBladeID                 //
#undef  BLADE_ID_CLASS_INTERNAL                                   //
#define BLADE_ID_CLASS_INTERNAL FakeBladeID                       //
#undef  BLADE_ID_CLASS                                            //
#define BLADE_ID_CLASS FakeBladeID                                //

// Define sound effects
EFFECT(sabermode);
EFFECT(blastermode);
EFFECT(detonatormode);
EFFECT(jetpackmode);
//NO-EFFECT(morsecodemode); No need for a "morsecode.wav" file. This is done using "Beepers" in Morse code.
//EFFECT(droidmode);          // Uncomment if implementing droid   (sound available - but no prop yet)
//EFFECT(vehiclemode);        // Uncomment if implementing Vehicle (no sound available)

// Enumeration for modes
enum class Prop_Mode {
      SABER = 0,
    BLASTER = 1,
  DETONATOR = 2,
    JETPACK = 3,  // Ready (I think), pending review
//MORSECODE = 4,  // Not fully ready
    //DROID = 6,  // Uncomment when ready to implement Droid functionality
  //VEHICLE = 7,  // Uncomment if implementing Vehicle (I don't know what this prop should do,
                  // but it would be cool to play with STARWARS "vehicles" sounds:
                  // speeders, pod-racers, TIE's, and any other ships that we can think of!)
};

// Define the duration for TWO_BUTTONS_eXtra_LONG_PUSH (4 sec)
#ifndef TWO_BUTTONS_X_LONG_PUSH
#define TWO_BUTTONS_X_LONG_PUSH 1000 * 3.9
#endif

// Button combo state management
unsigned long holdStartTime = 0;
Prop_Mode currentMode = Prop_Mode::SABER;  // Initial state is Saber

// Helper function to set FakeBladeID and re-detect the blade
template <class PropBase>
void updateBladeIDAndDetect(PropBase* prop_instance, int blade_id) {
  FakeBladeID::SetFakeBlade(blade_id);                                //
  PVLOG_DEBUG << "Fake blade id'ed\n";                                // Sabersense code part2
  prop_instance->FindBladeAgain(); // Call FindBladeAgain             // adapted to multi_prop.h
  SaberBase::DoBladeDetect(true);  // Re-detect blades                //
}

// Switch modes helper
template <class PropBase>
void swapProp(PropBase* prop_instance, Prop_Mode modenext, const char* message, int blade_id, EffectType effect) {
  PVLOG_STATUS << message << "\n";
  currentMode = modenext;
  updateBladeIDAndDetect(prop_instance, blade_id);
  SaberBase::DoEffect(effect, 0);
  PVLOG_VERBOSE << "One prop to rule them all, and in your config bind them!\n";
}

// Function to handle mode switching between props, it was my very repetitive, repeating code!
// Set ID 0 for SABER mode, 1 for BLASTER mode, 2 for DETONATOR mode, 3 for JETPACK mode,
//        4 for MORSECODE mode, 5 for DROID mode & 6 for VEHICLE mode.
template <class PropBase>
void switchModes(PropBase* prop_instance) {
  switch (currentMode) {
    case Prop_Mode::SABER:
      swapProp(prop_instance, Prop_Mode::BLASTER,   "Blaster Mode",   1, EFFECT_BLASTERMODE);
      break;

    case Prop_Mode::BLASTER:
      swapProp(prop_instance, Prop_Mode::DETONATOR, "Detonator Mode", 2, EFFECT_DETONATORMODE);
      break;

    case Prop_Mode::DETONATOR:
      swapProp(prop_instance, Prop_Mode::JETPACK,   "Jetpack Mode",   3, EFFECT_JETPACKMODE);
      break;

    case Prop_Mode::JETPACK:
      swapProp(prop_instance, Prop_Mode::SABER,     "Saber Mode",     0, EFFECT_SABERMODE);
      break;

    /*
      swapProp(prop_instance, Prop_Mode::MORSECODE, "Morse Code Mode",4, EFFECT_MORSECODEMODE);
      break;

    case Prop_Mode::MORSECODE:
      swapProp(prop_instance, Prop_Mode::DROID,     "Droid Mode",     5, EFFECT_DROIDMODE);
      break;

    case Prop_Mode::DROID:
      swapProp(prop_instance, Prop_Mode::VEHICLE,   "Vehicle Mode",   6, EFFECT_*********);
      break;                                                 // sound effect to be created !!!

    case Prop_Mode::VEHICLE:
      swapProp(prop_instance, Prop_Mode::SABER,     "Saber Mode",     0, EFFECT_SABERMODE);
      break;

    */
  }
}

// Template to support multi-prop modes, this is the "prop class" or prop section.
// MultiProp class with an Event function that calls switchModes(this)
template<class Saber, class Blaster, class Detonator, class Jetpack/*, class MorseCode , class Droid, class Vehicle*/>
class MultiProp : public virtual Saber, public virtual Blaster, public virtual Detonator, public virtual Jetpack/*, public virtual MorseCode, public virtual Droid, public virtual Vehicle*/ {
// One prop to rule them all, and in your config bind them!
public:
  const char* name() override { return "MultiProp"; }

/*
// Only one chdir & Parse function can be used at a time, so in a dual_prop.h/multi_prop.h "environment" SaberFett263Button
// already has them.
// I initially had in multi_prop.h:
#if defined(PROPS_SABER_FETT263_BUTTONS_H) || defined(PROPS_JETPACK_PROP_H)
  // Resolve 'chdir' ambiguity
  bool chdir(StringPiece dir) override {
    return Saber::chdir(dir);
  }
  // Resolve 'Parse' ambiguity
  bool Parse(const char* key, const char* value) override {
    return Saber::Parse(key, value);
  }
#endif
// This was resolving the ambiguity but was also increasing the size of the compile by a few 100 bytes
// because the code for chdir and Parse was added twice and extra code had to be added to multi_prop.h
// Doing it "my" way reduced the size of the compile by a little bit.
// Every byte saved counts, right ?
*/

  // Button mapping for 2 and 3-button setups
    // The "Blaster mapping"
  uint32_t map_button(uint32_t b) {
    switch (b) {
#if NUM_BUTTONS == 3
      case BUTTON_AUX:   return BUTTON_FIRE;
      case BUTTON_AUX2:  return BUTTON_MODE_SELECT;
#else
      case BUTTON_POWER: return BUTTON_FIRE;
      case BUTTON_AUX:   return BUTTON_MODE_SELECT;
#endif
      default: return b;
    }
  }

    // The "return to normal mapping"
  uint32_t reverse_map_button(uint32_t b) {
    switch (b) {
#if NUM_BUTTONS == 3
      case BUTTON_FIRE:        return BUTTON_AUX;
      case BUTTON_MODE_SELECT: return BUTTON_AUX2;
#else
      case BUTTON_FIRE:        return BUTTON_POWER;
      case BUTTON_MODE_SELECT: return BUTTON_AUX;
#endif
      default: return b;
    }
  }

  // Event handling for button presses, including combos for switching modes
  bool Event(enum BUTTON button, EVENT event) override {
    static bool powerPressed_ = false;    // Tracks BUTTON_POWER press state
    static bool auxPressed_ = false;      // Tracks BUTTON_AUX   press state
    if (event == EVENT_PRESSED) {
      if (button == BUTTON_POWER) {
        powerPressed_ = true;             // BUTTON_POWER is pressed
      } else {
        if (button == BUTTON_AUX) {
        auxPressed_ = true;               // BUTTON_AUX   is pressed
        }
      }
      // Check if both buttons are pressed simultaneously or near-simultaneously
      if (powerPressed_ && auxPressed_ && holdStartTime == 0) {
        holdStartTime = millis();         // Start the hold timer when both buttons are pressed
        PVLOG_NORMAL << "Dual Long Push Initiated.\n";
      }
    } else {
      if (event == EVENT_RELEASED) {
        if (button == BUTTON_POWER) {
          powerPressed_ = false;          // BUTTON_POWER is released
        } else {
          if (button == BUTTON_AUX) {
            auxPressed_ = false;          // BUTTON_AUX   is released
          }
        }
      }
      //  Reset  the  hold  state  if  either  button  is released
      if (!powerPressed_ || !auxPressed_) {
        holdStartTime = 0;
        PVLOG_NORMAL << "One of the two (or both) button(s) was (were) released too early!\n";
      }
    }
    // Check if both buttons have been held for the required duration
    if (powerPressed_ && auxPressed_ && (millis() - holdStartTime) >= TWO_BUTTONS_X_LONG_PUSH) {
      switchModes(this);               // Pass "this" MultiProp instance to switchModes & switch the prop mode
      PVLOG_NORMAL << "Both buttons were held long enough for MultiProp to switch to the next Prop_Mode.\n";
      return true;                     // Stop further event processing
    }

    // Call the appropriate Event function based on the current mode (Baster or "not Blaster")
    switch (currentMode) {
      case Prop_Mode::SABER:     return     Saber::Event(button, event);
      case Prop_Mode::BLASTER: {
        button = static_cast<enum BUTTON>(map_button(button));
        // Map modifiers to blaster
        uint32_t m = current_modifiers;
        current_modifiers = 0;
        for (; m; m &= m - 1) current_modifiers |= map_button(m & -m);
          bool ret =  /* . . . . . . */ Blaster::Event(button, event);
          PVLOG_NORMAL << "Blaster map_button\n";

        // Map modifiers back to normal
        m = current_modifiers;
        current_modifiers = 0;
        for (; m; m &= m - 1) current_modifiers |= reverse_map_button(m & -m);
          return ret;
          PVLOG_NORMAL << "Back to normal map_button\n";
        };
      case Prop_Mode::DETONATOR: return Detonator::Event(button, event);
      case Prop_Mode::JETPACK:   return   Jetpack::Event(button, event);
    //case Prop_Mode::MORSECODE: return MorseCode::Event(button, event);
    //case Prop_Mode::DROID:     return     Droid::Event(button, event);
    //case Prop_Mode::VEHICLE:   return   Vehicle::Event(button, event);
    }
    return false;  // Event was not handled
  }

/*******************************************************************************************\
| I tried with lamdbas functions to reduce repetition further, but it was taking a lot      |
| more memory, and adding a lot more complexity. I guess, I must be like Gollum, he doesn't |
| like Elven Lemdbas bread either!!! Yes I think this is funny! ¯\_(ツ)_/¯                   |
| But at least, I lined up everything to make the code look "prettier" and easier to read!  |
\*******************************************************************************************/

  // Overriding Event2 to resolve ambiguity
  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (currentMode) {
      case Prop_Mode::SABER:     return     Saber::Event2(button, event, modifiers);
      case Prop_Mode::BLASTER:   return   Blaster::Event2(button, event, modifiers);
      case Prop_Mode::DETONATOR: return Detonator::Event2(button, event, modifiers);
      case Prop_Mode::JETPACK:   return   Jetpack::Event2(button, event, modifiers);
    //case Prop_Mode::MORSECODE: return MorseCode::Event2(button, event, modifiers);
    //case Prop_Mode::DROID:     return     Droid::Event2(button, event, modifiers);
    //case Prop_Mode::VEHICLE:   return   Vehicle::Event2(button, event, modifiers);
    }
    return false;  // Event was not handled
  }

  void SetPreset(int preset_num, bool announce) override {
    switch (currentMode) {
      case Prop_Mode::SABER:         Saber::SetPreset(preset_num, announce); break;
      case Prop_Mode::BLASTER:     Blaster::SetPreset(preset_num, announce); break;
      case Prop_Mode::DETONATOR: Detonator::SetPreset(preset_num, announce); break;
      case Prop_Mode::JETPACK:     Jetpack::SetPreset(preset_num, announce); break;
    //case Prop_Mode::MORSECODE: MorseCode::SetPreset(preset_num, announce); break;
    //case Prop_Mode::DROID:         Droid::SetPreset(preset_num, announce); break;
    //case Prop_Mode::VEHICLE:     Vehicle::SetPreset(preset_num, announce); break;
    }
  }

  void Loop() override {
    switch (currentMode) {
      case Prop_Mode::SABER:         Saber::Loop(); break;
      case Prop_Mode::BLASTER:     Blaster::Loop(); break;
      case Prop_Mode::DETONATOR: Detonator::Loop(); break;
      case Prop_Mode::JETPACK:     Jetpack::Loop(); break;
    //case Prop_Mode::MORSECODE: MorseCode::Loop(); break;
    //case Prop_Mode::DROID:         Droid::Loop(); break;
    //case Prop_Mode::VEHICLE:     Vehicle::Loop(); break;
    }
  }

  void DoMotion(const Vec3& motion, bool clear) override {
    switch (currentMode) {
      case Prop_Mode::SABER:         Saber::DoMotion(motion, clear); break;
      case Prop_Mode::BLASTER:     Blaster::DoMotion(motion, clear); break;
      case Prop_Mode::DETONATOR: Detonator::DoMotion(motion, clear); break;
      case Prop_Mode::JETPACK:     Jetpack::DoMotion(motion, clear); break;
    //case Prop_Mode::MORSECODE: MorseCode::DoMotion(motion, clear); break;
    //case Prop_Mode::DROID:         Droid::DoMotion(motion, clear); break;
    //case Prop_Mode::VEHICLE:     Vehicle::DoMotion(motion, clear); break;
    }
  }

  void Clash(bool stab, float strength) override {
    switch (currentMode) {
      case Prop_Mode::SABER:         Saber::Clash(stab, strength); break;
      case Prop_Mode::BLASTER:     Blaster::Clash(stab, strength); break;
      case Prop_Mode::DETONATOR: Detonator::Clash(stab, strength); break;
      case Prop_Mode::JETPACK:     Jetpack::Clash(stab, strength); break;
    //case Prop_Mode::MORSECODE: MorseCode::Clash(stab, strength); break;
    //case Prop_Mode::DROID:         Droid::Clash(stab, strength); break;
    //case Prop_Mode::VEHICLE:     Vehicle::Clash(stab, strength); break;
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

/* "void morsecodemode()" will be uncommented out once morsecode_prop.h will be ready
  //(600Hz is the typical "aviation" morse code audio frequency - 800Hz is the frequency usualy used in movies)
  void morsecodemode() { // It says -.- = "K" = General invitation to transmit
    const float morsePattern[] = {0.6, 0.2, 0.6};
    for (float duration : morsePattern) {
      beeper.Beep(duration, 800); // Movies morse code frequency used.
      beeper.Silence(0.2);
    }
  }
*/

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_SABERMODE:     announcemode(&SFX_sabermode);       return;
      case EFFECT_BLASTERMODE:   announcemode(&SFX_blastermode);     return;
      case EFFECT_DETONATORMODE: announcemode(&SFX_detonatormode);   return;
      case EFFECT_JETPACKMODE:   announcemode(&SFX_jetpackmode);     return;
    //case EFFECT_MORSECODEMODE: morsecodemode();                    return;
    //case EFFECT_DROIDMODE:     announcemode(&SFX_droidmode);       return;
    //case EFFECT_VEHICLEMODE:   announcemode(&SFX_vehiclemode);     return;
    }
    switch (currentMode) {
      case Prop_Mode::SABER:         Saber::SB_Effect(effect, location); break;
      case Prop_Mode::BLASTER:     Blaster::SB_Effect(effect, location); break;
      case Prop_Mode::DETONATOR: Detonator::SB_Effect(effect, location); break;
      case Prop_Mode::JETPACK:     Jetpack::SB_Effect(effect, location); break;
    //case Prop_Mode::MORSECODE: MorseCode::SB_Effect(effect, location); break;
    //case Prop_Mode::DROID:         Droid::SB_Effect(effect, location); break;
    //case Prop_Mode::VEHICLE:     Vehicle::SB_Effect(effect, location); break;
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
  X(jetpackmode)  // add backslash when uncommenting next line
  //X(morsecodemode)

#ifdef INCLUDE_SSD1306

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
      case EFFECT_SABERMODE:     this->scr_.Play(&SCR_sabermode);       break;
      case EFFECT_BLASTERMODE:   this->scr_.Play(&SCR_blastermode);     break;
      case EFFECT_DETONATORMODE: this->scr_.Play(&SCR_detonatormode);   break;
      case EFFECT_JETPACKMODE:   this->scr_.Play(&SCR_jetpackmode);     break;
    //case EFFECT_MORSECODEMODE: this->scr_.Play(&SCR_morsecodemode);   break;
      default:
        StandarColorDisplayController<W, H, PREFIX>::SB_Effect2(effect, location);
    }
  }

protected:
  ONCE_PER_MULTIPROP_EFFECT(DEF_SCR);
};

#undef ONCE_PER_MULTIPROP_EFFECT

#endif  // PROP_BOTTOM
