#ifndef FUNCTIONS_EFFECT_INCREMENT_F_H
#define FUNCTIONS_EFFECT_INCREMENT_F_H

// Usage: EffectIncrementF<EFFECT, MAX, I>
// Increases by value I (up to MAX) each time EFFECT is triggered
// if greater than MAX returns 0
// I, MAX = numbers
// return value: INTEGER

template<BladeEffectType EFFECT, class MAX = Int<32768>, class I = Int<1>>
class EffectIncrementF {
public:
  void run(BladeBase* blade) {
    i_.run(blade);
    max_.run(blade);
    int i = i_.calculate(blade);
    if (effect_.Detect(blade)) value_ += i;
    value_ %= max_.calculate(blade);
  }
  int getInteger(int led) { return value_; }

private:
  OneshotEffectDetector<EFFECT> effect_;
  PONUA SVFWrapper<I> i_;
  PONUA SVFWrapper<MAX> max_;
  int value_;
};

#endif
