#ifndef FUNCTIONS_BROWN_NOISE_H
#define FUNCTIONS_BROWN_NOISE_H

// Usage: BrownNoiseF<GRADE>
// return value: FUNCTION
// Returns a value between 0 and 32768 with nearby pixels being similar.
// GRADE controls how similar nearby pixels are.

class BladeBase;

template<class GRADE>
class BrownNoiseF {
public:
  void run(BladeBase* blade) {
    grade_.run(blade);
    mix_ = random(32768);
  }
  int getInteger(int led) {
    int grade = grade_.getInteger(led);
    mix_ = clampi32(mix_ + random(grade * 2 + 1) - grade, 0, 32768);
    return mix_;
  }
private:
  GRADE grade_;
  uint16_t mix_;
};

#endif
