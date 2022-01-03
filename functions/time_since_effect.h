#ifndef FUNCTIONS_TIME_SINCE_EFFECT_H
#define FUNCTIONS_TIME_SINCE_EFFECT_H

// Usage: TimeSinceEffect<>
// Or: TimeSinceEffect<EFFECT>
// EFFECT: effect type
// return value: INTEGER
//
// TimeSinceEffect returns the number of milliseconds since a particular
// effect occured.
// When used as TimeSinceEffect<> inside a TransitionEffectL whose EFFECT is already specified, 
// then it will automatically use the right effect.

#include "svf.h"

template<EffectType T = EFFECT_NONE>
class TimeSinceEffectSVF {
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
    if (effect) {
      last_occurance_us_ = effect->start_micros;
    }
    uint32_t now = micros();
    uint32_t ret = now - last_occurance_us_;
    if (ret > 1000000000) {
      // Prevent weird wraparounds.
      last_occurance_us_ = now - 1000000000UL;
    } 
    return ret / 1000; // return ms
  }
private:
  uint32_t last_occurance_us_ = 0;
};

template<EffectType T = EFFECT_NONE>
using TimeSinceEffect = SingleValueAdapter<TimeSinceEffectSVF<T>>;

#endif
