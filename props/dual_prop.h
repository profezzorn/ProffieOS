#ifndef PROPS_DUAL_PROP_H
#define PROPS_DUAL_PROP_H

/*
dual_prop.h allows for 2 discrete prop files to be used, 
alternating on a latched switch (Blade Detect).
This is useful when you want a saber to 
toggle to a blaster for example, 
and you want the buttons to take on different behaviors.

How to use:
#ifdef CONFIG_PROP
#include "../props/dual_prop.h"
#include "../props/saber_sa22c_buttons.h"
#include "../props/blaster.h"
#undef PROP_TYPE
#define PROP_TYPE DualProp<SaberSA22CButtons, Blaster>
#endif
** Note the prop file SaberSA22CButtons here would change
to the file name of the prop you choose.

Then setup your config file for 2 states:
Bladein preset bank, and a BladeOut preset bank.
Give the BladeConfig 2 sets of descriptions, and
#define BLADE_DETECT_PIN properly.
*/


#ifndef DUAL_PROP_CONDITION
#define DUAL_PROP_CONDITION blade_detected_
#endif

#ifdef PROP_INHERIT_PREFIX
#error dual_prop.h must be included first
#else
#define PROP_INHERIT_PREFIX virtual
#endif

#include "prop_base.h"

template<class A, class B>
class DualProp : public virtual PropBase, public A, public B {
 public:
  const char* name() override { return "DualProp"; }
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

  void Clash(bool stab, float strength) override {
    if (DUAL_PROP_CONDITION) {
      A::Clash(stab, strength);
    } else {
      B::Clash(stab, strength);
    }
  }
  
  void SB_Effect(EffectType effect, float location) override {
    if (DUAL_PROP_CONDITION) {
      A::SB_Effect(effect, location);
    } else {
      B::SB_Effect(effect, location);
    }
  }
};

template<class Saber, class Blaster>
class SaberBlasterProp : public virtual Saber, public virtual Blaster {
 public:
  uint32_t map_button(uint32_t b) {
    switch (b) {
#if NUM_BUTTONS == 3
        case BUTTON_AUX: return BUTTON_FIRE;
        case BUTTON_AUX2: return BUTTON_MODE_SELECT;
#else
        case BUTTON_POWER: return  BUTTON_FIRE;
        case BUTTON_AUX: return BUTTON_MODE_SELECT;
#endif
      default: return b;
    }
  }
  uint32_t reverse_map_button(uint32_t b) {
    switch (b) {
#if NUM_BUTTONS == 3
        case BUTTON_FIRE: return BUTTON_AUX;
        case BUTTON_MODE_SELECT: return BUTTON_AUX2;
#else
        case BUTTON_FIRE: return  BUTTON_POWER;
        case BUTTON_MODE_SELECT: return BUTTON_AUX;
#endif
      default: return b;
    }
  }
  const char* name() override { return "DualProp"; }
  bool Event(enum BUTTON button, EVENT event) override {
    if (button == BUTTON_BLADE_DETECT) {
      if (event == EVENT_LATCH_ON) {
	Saber::Off();
	Saber::blade_detected_ = true;
	Saber::FindBladeAgain();
	SaberBase::DoBladeDetect(true);
	current_modifiers = 0;
	return true;
      } else  if (event == EVENT_LATCH_OFF) {
	Saber::Off();
	Saber::blade_detected_ = false;
	Saber::FindBladeAgain();
	SaberBase::DoBladeDetect(false);
	current_modifiers = 0;
	return true;
      }
    }
    if (Saber::blade_detected_) {
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

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    if (Saber::blade_detected_) {
      return Saber::Event2(button, event, modifiers);
    } else {
      return Blaster::Event2(button, event, modifiers);
    }
  }

  void SetPreset(int preset_num, bool announce) override {
    if (Saber::blade_detected_) {
      Saber::SetPreset(preset_num, announce);
    } else {
      Blaster::SetPreset(preset_num, announce);
    }
  }

  void Loop() override {
    if (Saber::blade_detected_) {
      Saber::Loop();
    } else {
      Blaster::Loop();
    }
  }

  void DoMotion(const Vec3& motion, bool clear) override {
    if (Saber::blade_detected_) {
      Saber::DoMotion(motion, clear);
    } else {
      Blaster::DoMotion(motion, clear);
    }
  }

  void Clash(bool stab, float strength) override {
    if (Saber::blade_detected_) {
      Saber::Clash(stab, strength);
    } else {
      Blaster::Clash(stab, strength);
    }
  }
  
  void SB_Effect(EffectType effect, float location) override {
    if (Saber::blade_detected_) {
      Saber::SB_Effect(effect, location);
    } else {
      Blaster::SB_Effect(effect, location);
    }
  }
};
 
#endif
