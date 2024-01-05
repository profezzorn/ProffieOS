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
    speed_.run(blade);
    hold_time_millis_.run(blade);
    int current = f_.calculate(blade);
    uint32_t hold_millis = hold_time_millis_.calculate(blade);
    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;
    if (millis() - last_peak_ > hold_millis) {
      if (delta > 1000000) delta = 1;
      delta *= speed_.calculate(blade);
      delta /= 1000000;
      value_ -= delta;
    }
    if (current > value_) {
      value_ = current;
      last_peak_ = millis();
    }
  }

  int getInteger(int led) {
    return value_;
  }

private:
  PONUA SVFWrapper<F> f_;
  PONUA SVFWrapper<HOLD_MILLIS> hold_time_millis_;
  PONUA SVFWrapper<SPEED> speed_;
  int value_ = 0;
  uint32_t last_micros_ = 0;
  uint32_t last_peak_ = millis();
};

#endif
