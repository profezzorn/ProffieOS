#ifndef FUNCTIONS_WAVELEN_H
#define FUNCTIONS_WAVELEN_H

// Usage: Wavnum<>
// Or: Wavnum<EFFECT>
// EFFECT: effect type
// return value: INTEGER
//
// Wavnum (length of wav file) takes the duration of a wav file sound
// and can be used to replace time integer arguments in a blade style.
// Example: TrFadeX<Wavnum<EFFECT_RETRACTION>>
// When used as Wavnum<> inside a TransitionEffectL whose EFFECT is already specified, 
// then it will automatically use the right effect.
// Example: TransitionEffectL<TrConcat<TrWipex<Wavnum<>>,White,TrWipeX<Wavnum<>>>,EFFECT_BLAST>

template<EffectType T = EFFECT_NONE>
class Wavnum {
public:
  void run(BladeBase* blade) {
    BladeEffect* effect;
    if (T == EFFECT_NONE) {
      effect = last_detected_blade_effect;
    } else {
      OneshotEffectDetector<T> detector;
      effect = detector.Detect(blade);
    }
    if (effect) value_ = effect->wavnum;
  }
  int getInteger() { return value_; }
  int getInteger(int led) { return value_; }
private:
  int value_ = 0;
};

#endif
