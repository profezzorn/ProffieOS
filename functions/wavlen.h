#ifndef FUNCTIONS_WAVELEN_H
#define FUNCTIONS_WAVELEN_H

// Usage: WavLen<EFFECT>
// EFFECT: effect type
// return value: INTEGER
//
// Example: TrFadeX<WavLen<EFFECT_RETRACTION>>
// WavLen (length of wav file) takes the duration of a wav file sound
// and can be used to replace time integer arguments in a blade style. 

template<EffectType T = EFFECT_NONE>
class WavLen {
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
private:
  int value_ = 0;
};

#endif
