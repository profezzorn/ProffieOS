#ifndef STYLES_TRANSITION_PULSE_H
#define STYLES_TRANSITION_PULSE_H

#include "../transitions/concat.h"
#include "../functions/effect_increment.h"

// Usage: TransitionPulseL<TRANSITION, PULSE>
// TRANSITION: TRANSITION
// PULSE: FUNCTION
// return value: COLOR
//
// When the specified PULSE happens TRANSITION layer will run.

template<class TRANSITION, class PULSE>
class TransitionPulseL {
public:
  LayerRunResult run(BladeBase* blade) {
    SaveLastDetectedBladeEffectScoped save;
    pulse_.run(blade);
    if (pulse_.calculate(blade)) {
      transition_.begin();
      run_ = true;
    }
    if (run_) {
      transition_.run(blade);
      if (transition_.done()) run_ = false;
    }
    return LayerRunResult::UNKNOWN;
  }
  
private:
  bool run_ = false;
  TRANSITION transition_;
  PONUA SVFWrapper<PULSE> pulse_;

public:
  auto getColor(int led) -> decltype(transition_.getColor(RGBA_um_nod::Transparent(),
                RGBA_um_nod::Transparent(), 1)) {
    if (run_) {
      return transition_.getColor(RGBA_um_nod::Transparent(),
          RGBA_um_nod::Transparent(), led);
    } else {
      return RGBA_um_nod::Transparent();
    }
  }
};

#endif
