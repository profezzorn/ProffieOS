#ifndef PROPS_DUAL_PROP_H
#define PROPS_DUAL_PROP_H

// How to use:
// #ifdef CONFIG_PROP
// #include "../props/saber_sa22c_buttons.h"
// #include "../props/blaster.h"
// #include "../props/dual_prop.h"
// #undef PROP_TYPE
// #define PROP_TYPE DualProp<SaberSA22CButtons, Blaster>
// #endif

#ifndef DUAL_PROP_CONDITION
#define DUAL_PROP_CONDITION blade_detected_
#endif

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
class SaberBlasterProp : public virtual PropBase, public Saber, public Blaster {
 public:
  const char* name() override { return "DualProp"; }
  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
     case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
     case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
      Off();
      blade_detected_ = true;
      FindBladeAgain();
      SaberBase::DoBladeDetect(true);
      return true;

     case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
     case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
      Off();
      blade_detected_ = false;
      FindBladeAgain();
      SaberBase::DoBladeDetect(false);
      return true;
    }    
    
    if (blade_detected_) {
      return Saber::Event2(button, event, modifiers);
    } else {
      switch (button) {
#if NUM_BUTTONS == 3
        case BUTTON_AUX: button = BUTTON_FIRE; break;
        case BUTTON_AUX2: button = BUTTON_MODE_SELECT; break;
#else
        case BUTTON_POWER:
	  if (!IsOn()) {
	    On();
	    return true;
	  }
	  button = BUTTON_FIRE;
	  break;
        case BUTTON_AUX: button = BUTTON_MODE_SELECT; break;
#endif
      }
      return Blaster::Event2(button, event, modifiers);
    }
  }

  void SetPreset(int preset_num, bool announce) override {
    if (blade_detected_) {
      Saber::SetPreset(preset_num, announce);
    } else {
      Blaster::SetPreset(preset_num, announce);
    }
  }

  void Loop() override {
    if (blade_detected_) {
      Saber::Loop();
    } else {
      Blaster::Loop();
    }
  }

  void Clash(bool stab, float strength) override {
    if (blade_detected_) {
      Saber::Clash(stab, strength);
    } else {
      Blaster::Clash(stab, strength);
    }
  }
  
  void SB_Effect(EffectType effect, float location) override {
    if (blade_detected_) {
      Saber::SB_Effect(effect, location);
    } else {
      Blaster::SB_Effect(effect, location);
    }
  }
};
 
#endif
