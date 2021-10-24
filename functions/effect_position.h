#ifndef FUNCTIONS_EFFECTPOS_H
#define FUNCTIONS_EFFECTPOS_H

#include "svf.h"

template<EffectType T = EFFECT_NONE>
class EffectPositionSVF {
public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    BladeEffect* effect;
    if (T == EFFECT_NONE) {
      effect = last_detected_blade_effect;
    } else {
      OneshotEffectDetector<T> detector;
      effect = detector.Detect(blade);
    }
    if (effect) return effect->location * 32768;
    return 0;
  }
};

template<EffectType T = EFFECT_NONE>
using EffectPosition = SingleValueAdapter<EffectPositionSVF<T>>;

#endif
