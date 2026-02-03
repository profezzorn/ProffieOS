// This version of dual_prop.h, has been modified by me (Oli) to cover some issues, I hve identified while creating multi_prop.h

#ifndef PROPS_DUAL_PROP_H
#define PROPS_DUAL_PROP_H

/*
dual_prop.h allows for 2 discrete prop files to be used,
alternating on a latched switch (Blade Detect) or with Blade ID..
This is useful when you want a saber to
toggle to a blaster for example,
and you want the buttons to take on different behaviors.

How to use:
Edit your config file as follows.(this example for a saber/blaster setup)
Setup the prop section like this:
#ifdef CONFIG_PROP
#include "../props/dual_prop.h"
#include "../props/saber_sa22c_buttons.h"
#include "../props/blaster.h"
#undef PROP_TYPE
#define PROP_TYPE SaberBlasterProp<SaberSA22CButtons, Blaster> // these are the classes found in the 2 props.
#endif
** Note the prop file names (saber_sa22c_buttons.h and blaster.h)
as well as the class names would change based on the prop you choose.

Now setup your CONFIG_PRESETS section to have multiple preset banks:
A no-blade (Blaster) preset bank, followed by one or more blade-in preset banks.
Also, the BladeConfig needs each blade (and no-blade) to have a description entry in
the same order of the Presets arrays. The Blade ID values will depend on whether
you choose to use Blade Detect or Blade ID.

- Blade Detect using a wired latching system:
Add #define BLADE_DETECT_PIN with the pin number used.
See here for setup info:
https://pod.hubbe.net/howto/blade-detect.html

- Blade ID using the measured resistance of the blade to switch:
Add the following to the config file
  #define ENABLE_POWER_FOR_ID PowerPINS<bladePowerPin2, bladePowerPin3> (based on the LED pads you use)
  #define SHARED_POWER_PINS
  #define BLADE_ID_SCAN_MILLIS 1000
  #define BLADE_ID_TIMES 15
// This will make the blade ID class for that range return NO_BLADE
// (use NO_BLADE as the blade definition value.)
#define NO_BLADE_ID_RANGE 685000,1000000000

See here for setup info:
https://pod.hubbe.net/howto/blade-id.html
*/

#define DUAL_PROP_CONDITION this->blade_present()

#ifdef PROP_INHERIT_PREFIX
#error dual_prop.h must be included first
#else
#define PROP_INHERIT_PREFIX virtual
#endif

#include "prop_base.h"

// Use DualProp if you want to combine two different prop types other than a blaster.
// If a blaster is involved in your prop "mix", check further down to "class SaberBlasterProp"
template<class A, class B>
class DualProp : public virtual PropBase, public A, public B {
 public:
  const char* name() override { return "DualProp"; }
/*
  // I am not sure if this part is needed (that is why it is commented out) because I didn't have a compile error
  // with mode_Event2 YET, for 2 or more props (in my quest to solve my multi_prop) using it. But I thought it would
  // be a good idea to have it here for WHEN a conflict happens, then this section can just be uncommented.
  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) {
    if (DUAL_PROP_CONDITION) {
      return A::Event2(button, event, modifiers);
    } else {
      return B::Event2(button, event, modifiers);
    }
  }
*/
  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    if (DUAL_PROP_CONDITION) {
      return A::Event2(button, event, modifiers);
    } else {
      return B::Event2(button, event, modifiers);
    }
  }

  void SetPreset(int preset_num, bool announce) override {
    if (DUAL_PROP_CONDITION) {
      A::SetPreset(preset_num, announce);
    } else {
      B::SetPreset(preset_num, announce);
    }
  }

  void Loop() override {
    if (DUAL_PROP_CONDITION) {
      A::Loop();
    } else {
      B::Loop();
    }
  }

  void Setup() override {
    A::Setup();
    B::Setup();
  }

  bool chdir(StringPiece font) override {
    if (DUAL_PROP_CONDITION) {
      return A::chdir(font);
    } else {
      return B::chdir(font);
    }
  }

  bool Parse(const char* key, const char* val) override {
    if (DUAL_PROP_CONDITION) {
      return A::Parse(key, val);
    } else {
      return B::Parse(key, val);
    }
  }

  void Clash(bool stab, float strength) override {
    if (DUAL_PROP_CONDITION) {
      A::Clash(stab, strength);
    } else {
      B::Clash(stab, strength);
    }
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    if (DUAL_PROP_CONDITION) {
      A::SB_Effect(effect, location);
    } else {
      B::SB_Effect(effect, location);
    }
  }
};

// Use SaberBlasterProp if you want to combine a saber and a blaster (like an Ezra "staple-gun/light saber" type prop,
// so that the blaster buttons "mapping" makes sense!
template<class Saber, class Blaster>
class SaberBlasterProp : public virtual Saber, public virtual Blaster {
 public:
  uint32_t map_button(uint32_t b) {
    switch (b) {
#if NUM_BUTTONS == 3
      case BUTTON_POWER: return BUTTON_RELOAD;
      case BUTTON_AUX: return BUTTON_FIRE;
      case BUTTON_AUX2: return BUTTON_MODE_SELECT;
#else
      case BUTTON_POWER: return BUTTON_FIRE;
      case BUTTON_AUX: return BUTTON_MODE_SELECT;
#endif
      default: return b;
    }
  }
  uint32_t reverse_map_button(uint32_t b) {
    switch (b) {
#if NUM_BUTTONS == 3
      case BUTTON_RELOAD: return BUTTON_POWER;
      case BUTTON_FIRE: return BUTTON_AUX;
      case BUTTON_MODE_SELECT: return BUTTON_AUX2;
#else
      case BUTTON_FIRE: return BUTTON_POWER;
      case BUTTON_MODE_SELECT: return BUTTON_AUX;
#endif
      default: return b;
    }
  }
  const char* name() override { return "DualProp"; }
  bool Event(enum BUTTON button, EVENT event) override {

#ifdef BLADE_DETECT_PIN
    if (button == BUTTON_BLADE_DETECT) {
      if (event == EVENT_LATCH_ON) {
        Saber::Off();
        Saber::blade_detected_ = true;
        Saber::FindBladeAgain();
        SaberBase::DoBladeDetect(true);
      } else if (event == EVENT_LATCH_OFF) {
        Saber::Off();
        Saber::blade_detected_ = false;
        Saber::FindBladeAgain();
        SaberBase::DoBladeDetect(false);
      }
      current_modifiers = 0;
      return true;
    }
#endif // BLADE_DETECT_PIN

    if (DUAL_PROP_CONDITION) {
      return Saber::Event(button, event);
    } else {
      button = static_cast<enum BUTTON>(map_button(button));

      // Map modifiers
      uint32_t m = current_modifiers;
      current_modifiers = 0;
      for (; m; m &= m - 1) current_modifiers |= map_button(m & -m);

      bool ret = Blaster::Event(button, event);

      // Map modifiers back
      m = current_modifiers;
      current_modifiers = 0;
      for (; m; m &= m - 1) current_modifiers |= reverse_map_button(m & -m);
      return ret;
    }
  }

/*
  // I am not sure if this part is needed (that is why it is commented out) because I didn't have a compile error
  // with mode_Event2 YET, for 2 or more props (in my quest to solve my multi_prop) using it. But I thought it would
  // be a good idea to have it here for WHEN a conflict happens, then this section can just be uncommented.
  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) {
    if (DUAL_PROP_CONDITION) {
      return Saber::Event2(button, event, modifiers);
    } else {
      return Blaster::Event2(button, event, modifiers);
    }
  }
*/

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    if (DUAL_PROP_CONDITION) {
      return Saber::Event2(button, event, modifiers);
    } else {
      return Blaster::Event2(button, event, modifiers);
    }
  }

  void SetPreset(int preset_num, bool announce) override {
    if (DUAL_PROP_CONDITION) {
      Saber::SetPreset(preset_num, announce);
      // Blaster is always on, so turn Off before going to Saber mode
      if (Saber::IsOn()) Saber::Off();
    } else {
      Blaster::SetPreset(preset_num, announce);
    }
  }

  void Loop() override {
    if (DUAL_PROP_CONDITION) {
      Saber::Loop();
    } else {
      Blaster::Loop();
    }
  }

  void Setup() override {
    Saber::Setup();
    Blaster::Setup();
  }

  bool chdir(StringPiece font) override {
    if (DUAL_PROP_CONDITION) {
      return Saber::chdir(font);
    } else {
      return Blaster::chdir(font);
    }
  }

  bool Parse(const char* key, const char* val) override {
    if (DUAL_PROP_CONDITION) {
      return Saber::Parse(key, val);
    } else {
      return Blaster::Parse(key, val);
    }
  }

  void DoMotion(const Vec3& motion, bool clear) override {
    if (DUAL_PROP_CONDITION) {
      Saber::DoMotion(motion, clear);
    } else {
      Blaster::DoMotion(motion, clear);
    }
  }

  void Clash(bool stab, float strength) override {
    if (DUAL_PROP_CONDITION) {
      Saber::Clash(stab, strength);
    } else {
      Blaster::Clash(stab, strength);
    }
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    if (DUAL_PROP_CONDITION) {
      Saber::SB_Effect(effect, location);
    } else {
      Blaster::SB_Effect(effect, location);
    }
  }
};

#endif
