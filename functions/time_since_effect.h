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

template<EffectType T = EFFECT_NONE>
class TimeSinceEffect {
public:
  void run(BladeBase* blade) {
    BladeEffect* effect;
    if (T == EFFECT_NONE) {
      effect = last_detected_blade_effect;
    } else {
      OneshotEffectDetector<T> detector;
      effect = detector.Detect(blade);
    }
    if (effect) value_ = (micros() - effect->start_micros) / 1000; // ms
  }
  int getInteger() { return value_; }
  int getInteger(int led) { return value_; }
private:
  int value_ = 0;
};

#endif
