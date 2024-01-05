#ifndef FUNCTIONS_WAVENUM_H
#define FUNCTIONS_WAVENUM_H

// Usage: WavNum<>
// Or: WavNum<EFFECT>
// EFFECT: effect type
// return value: INTEGER
//
// Returns which file was actually played.
// First file returns 0. Even if the file is called 'clash1.wav'.

template<EffectType T = EFFECT_NONE>
class WavNum {
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
