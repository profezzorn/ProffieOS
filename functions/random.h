#ifndef FUNCTIONS_RANDOM_H
#define FUNCTIONS_RANDOM_H

// Usage: RandomF
// Return value: FUNCTION
// Returns a random number between 0 and 32768.
// All LEDS gets the same value.

class RandomFSVF {
public:
  int calculate(BladeBase* blade) { return random(32768); }
  void run(BladeBase* blade) {}
};

using RandomF = SingleValueAdapter<RandomFSVF>;

// Usage: RandomPerLEDF
// Return value: FUNCTION
// Returns a random number between 0 and 32768.
// Each LED gets a different random value.

class RandomPerLEDF {
public:
  void run(BladeBase* blade) {  }
  int getInteger(int led) { return random(32768); }
};

// Usage: EffectRandomF<EFFECT>
// Returns a random value between 0 and 32768 each time EVENT is triggered
// return value: INTEGER

template<BladeEffectType EFFECT>
class EffectRandomF {
public:
  void run(BladeBase* blade) {
    if (effect_.Detect(blade)) value_ = random(32768);
  }
  int getInteger(int led) { return value_; }

private:
  OneshotEffectDetector<EFFECT> effect_;
  int value_;
};

#endif
