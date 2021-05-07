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
  
#endif
