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


#endif
