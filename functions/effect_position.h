#ifndef FUNCTIONS_EFFECTPOS_H
#define FUNCTIONS_EFFECTPOS_H

// Usage: EffectPosition<>
// Or: EffectPosition<EFFECT>
// EFFECT: effect type
// return value: INTEGER
//
// EffectPosition returns the position of a particular effect. 0 = base, 32768 = tip.
// For now, this location is random, but may be set explicitly in the future.
// When used as EffectPosition<> inside a TransitionEffectL whose EFFECT is already specified, 
// then it will automatically use the right effect.

#include "svf.h"

template<EffectType T = EFFECT_NONE>
class EffectPositionSVF {
public:
  void run(BladeBase* blade) {
    BladeEffect* effect;
    if (T == EFFECT_NONE) {
      effect = last_detected_blade_effect;
    } else {
      OneshotEffectDetector<T> detector;
      effect = detector.Detect(blade);
    }
    if (effect) value_ = effect->location * 32768;
  }
  int getInteger() { return value_; }
  int getInteger(int led) { return value_; }
  int calculate(BladeBase* blade) { return value_; }
private:
  int value_ = 0;
};

template<EffectType T = EFFECT_NONE>
using EffectPosition = SingleValueAdapter<EffectPositionSVF<T>>;

// optimized specialization
template<EffectType T>
class SingleValueAdapter<EffectPositionSVF<T>> : public EffectPositionSVF<T> {};

#endif
