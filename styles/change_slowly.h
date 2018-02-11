#ifndef STYLES_CHANGE_SLOWLY_H
#define STYLES_CHANGE_SLOWLY_H

// Usage: ChangeSlowly<F, SPEED>
// Changes F by no more than SPEED values per second.
// F, SPEED: INTEGER
// return value: INTEGER, same for all LEDs

class BladeBase;

template<class F, class SPEED>
class ChangeSlowly {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    speed_.run(blade);

    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;

    if (delta > 1000000) delta = 1;
    delta *= speed_.getInteger();
    delta /= 1000000;
    uint32_t target = f_.getInteger();
    if (delta > abs(value_ - target)) {
      value_ = target;
    } else  if (value_ < target) {
      value_ += delta;
    } else {
      value_ -= delta;
    }
  }
  int getInteger(int led) {
    return value_;
  }
private:
  F f_;
  SPEED speed_;
  int value_ = 0;
  uint32_t last_micros_ = 0;
};
#endif
