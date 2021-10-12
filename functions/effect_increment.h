#ifndef FUNCTIONS_EFFECT_INCREMENT_H
#define FUNCTIONS_EFFECT_INCREMENT_H

// Usage: EffectIncrement<EFFECT, MAX, I>
// Increases by value I (up to MAX) each time EFFECT is triggered
// if greater than MAX returns 0
// I, MAX = numbers
// return value: INTEGER

template<BladeEffectType EFFECT, class MAX = Int<32768>, class I = Int<1>>
class EffectIncrement {
public:
  void run(BladeBase* blade) {
    i_.run(blade);
    max_.run(blade);
    int i = i_.getInteger(0);
    if (effect_.Detect(blade)) {
      value_ += i;
    }
    value_ %= max_.getInteger(0);
  }
  int getInteger(int led) { return value_; }

private:
  OneshotEffectDetector<EFFECT> effect_;
  I i_;
  MAX max_;
  int value_;
};

#endif
