#ifndef FUNCTIONS_EFFECTPOS_H
#define FUNCTIONS_EFFECTPOS_H

template<EffectType T = EFFECT_NONE>
class EffectPosition {
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
private:
  int value_ = 0;
};

#endif
