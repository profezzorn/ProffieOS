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

// Usage: SlowNoise<SPEED>
// return value: FUNCTION
// Returns a value between 0 and 32768 which changes randomly up and
// down over time. All pixels gets the same value.
// SPEED controls how quickly the value changes.
template<class SPEED>
class SlowNoise {
public:
  SlowNoise() {
    rnd_ = random(32768000);
  }
  void run(BladeBase* blade) {
    speed_.run(blade);
    uint32_t now = millis();
    uint32_t delta = now - last_millis_;
    if (delta > 100) delta = 1;
    last_millis_ = now;
    int speed = speed_.getInteger(0);
    // This makes the random value update exactly 1000 times per second.
    while (delta--)
      rnd_ = clampi32(rnd_ + (random(speed * 2 + 1) - speed), 0, 32768000);
    value_ = rnd_ / 1000;
  }
  int getInteger(int led) { return value_ ; }
private:
  SPEED speed_;
  uint32_t last_millis_;
  int rnd_;
  int value_;
};


#endif
