#ifndef FUNCTIONS_WAVELEN_H
#define FUNCTIONS_WAVELEN_H

// Usage: WavLen<>
// Or: WavLen<EFFECT>
// EFFECT: effect type
// return value: INTEGER
//
// WavLen (length of wav file) takes the duration of a wav file sound
// and can be used to replace time integer arguments in a blade style.
// Example: TrFadeX<WavLen<EFFECT_RETRACTION>>
// When used as WavLen<> inside a TransitionEffectL whose EFFECT is already specified, 
// then it will automatically use the right effect.
// Example: TransitionEffectL<TrConcat<TrWipex<WavLen<>>,White,TrWipeX<WavLen<>>>,EFFECT_BLAST>

template<EffectType T = EFFECT_NONE>
class WavLenSVF {
public:
  void run(BladeBase* blade) {
    BladeEffect* effect;
    if (T == EFFECT_NONE) {
      effect = last_detected_blade_effect;
    } else {
      OneshotEffectDetector<T> detector;
      effect = detector.Detect(blade);
    }
    if (effect) value_ = effect->sound_length * 1000; // ms
  }
  int getInteger() { return value_; }
  int getInteger(int led) { return value_; }
  int calculate(BladeBase* blade) { return value_; }
private:
  int value_ = 0;
};

template<EffectType T = EFFECT_NONE>
using WavLen = SingleValueAdapter<WavLenSVF<T>>;

// optimized specialization
template<EffectType T>
class SingleValueAdapter<WavLenSVF<T>> : public WavLenSVF<T> {};

#endif
