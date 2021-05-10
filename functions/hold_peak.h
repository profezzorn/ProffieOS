#ifndef FUNCTIONS_HOLD_PEAK_H
#define FUNCTIONS_HOLD_PEAK_H

// Usage: HoldPeakF<F, HOLD_MILLIS, SPEED>
// Holds Peak value of F for HOLD_MILLIS.
// then transitions down over SPEED to current F
// F, HOLD_MILLIS and SPEED: FUNCTION
// return value: FUNCTION, same for all LEDs

class BladeBase;

template<class F, class HOLD_MILLIS, class SPEED>
class HoldPeakF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    current = f_.getInteger(0);
    hold_time_millis.run(blade);
    hold_millis = hold_time_millis.getInteger(0);
    speed_.run(blade);
    uint32_t now = micros();
    uint64_t delta = now - last_micros;
    last_micros = now;
    if (current > value_) {
      value_ = current;
      last_peak = millis();
    }
    if (value_ > current && millis() - last_peak > hold_millis) {
      if (delta > 1000000) delta = 1;
      delta *= speed_.getInteger(0);
      delta /= 1000000;
      int target = current;
      if (delta > abs(value_ - target)) {
        value_ = target;
      } else if (value_ < target) {
        value_ += delta;
      } else {
        value_ -= delta;
      }
    }
  }

  int getInteger(int led) {
    return value_;
  }

private:
  F f_;
  HOLD_MILLIS hold_time_millis;
  SPEED speed_;
  int value_ = 0;
  int current;
  uint32_t last_micros = 0;
  uint32_t hold_millis;
  uint32_t last_peak = millis();
};

#endif
