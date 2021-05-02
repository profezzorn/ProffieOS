#ifndef FUNCTIONS_HOLD_PEAK_H
#define FUNCTIONS_HOLD_PEAK_H

// Usage: HoldPeakF<F, hold_time_, SPEED>
// Holds Peak value of F for hold_time_ in millis.
// then transitions down over SPEED to current F
// F, SPEED: FUNCTION
// hold_time_ MILLIS
// return value: FUNCTION, same for all LEDs

class BladeBase;

template<class F, int hold_time_, class SPEED>

class HoldPeakF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    current_ = f_.getInteger(0);
    speed_.run(blade);
    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;
    if (current_ > peak_ && millis() > 2000) { // the millis is for SwingSpeed since boot causes spike
      peak_ = current_;
      hold_millis_ = millis();
      value_ = peak_;
    }
    if (peak_ > current_ && millis() - hold_millis_ > hold_time_) {
      if (delta > 1000000) delta = 1;
      delta *= speed_.getInteger(0);
      delta /= 1000000;
      int target = f_.getInteger(0);
      if (delta > abs(peak_ - target)) {
        peak_ = target;
      } else if (peak_ < target) {
        peak_ += delta;
      } else {
        peak_ -= delta;
      }
    }
    value_ = peak_;
  }

  int getInteger(int led) {
    return value_;
  }

private:
  F f_;
  SPEED speed_;
  int value_;
  int peak_;
  int current_;
  uint32_t last_micros_ = 0;
  uint32_t hold_millis_ = millis();
};

#endif
