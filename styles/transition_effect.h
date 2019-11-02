#ifndef STYLES_TRANSITION_EFFECT_H
#define STYLES_TRANSITION_EFFECT_H

#include "../transitions/concat.h"

// Usage: TransitionEffect<COLOR, EFFECT_COLOR, TRANSITION1, TRANSITION2, EFFECT>
// COLOR, EFFECT_COLOR: COLOR
// TRANSITION1, TRANSITION2 : TRANSITION
// EFFECT: effect type
// return value: COLOR
//
// When the specified EFFECT happens (clash/blast/etc.) transition from COLOR to
// EFFECT_COLOR using TRANSITION1. Then transition back using TRANSITION2.

template<class T, class EFFECT_COLOR, class TRANSITION1, class TRANSITION2, BladeEffectType EFFECT>
class TransitionEffect {
public:
  void run(BladeBase* blade) {
    color_.run(blade);
    transition_.run(blade);
    if (effect_.Detect(blade)) {
      transition_.begin();
      run_ = true;
    }
    if (run_ && transition_.done()) run_ = false;
  }
  OverDriveColor getColor(int led) {
    OverDriveColor ret = color_.getColor(led);
    if (run_) ret = transition_.getColor(ret, ret, led);
    return ret;
  }
private:
  bool run_ = false;
  T color_;
  TrConcat<TRANSITION1, EFFECT_COLOR, TRANSITION2> transition_;
  OneshotEffectDetector<EFFECT> effect_;
};

#endif
