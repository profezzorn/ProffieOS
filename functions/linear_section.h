#ifndef FUNCTION_LINEAR_SECTION_H
#define FUNCTION_LINEAR_SECTION_H

#include <algorithm>

// Usage: LinearSectionF<POSITION, FRACTION>
// POSITION: FUNCTION position on the blade, 0-32768
// FRACTION: FUNCTION how much of the blade to light up, 0 = none, 32768 = all of it
// return value: FUNCTION
// creates a "block" of pixels at POSITION taking up FRACTION of blade

class BladeBase;
template<class POSITION, class FRACTION>
class LinearSectionF {
public:
  FunctionRunResult run(BladeBase* base) {
    pos_.run(base);
    FunctionRunResult ret = RunFunction(&fraction_, base);
    
    num_leds_ = base->num_leds();
    int fraction = fraction_.getInteger(0);
    if (fraction == 32768) {
      start_ = 0;
      end_ = num_leds_ * 32768;
    } else if (fraction == 0) {
      start_ = 0;
      end_ = 0;
    } else {
      int pos = pos_.getInteger(0);
      start_ = clampi32((pos - fraction / 2) * num_leds_, 0, 32768 * num_leds_);
      end_ = clampi32((pos + fraction / 2) * num_leds_, 0, 32768 * num_leds_);
    }
    num_leds_ *= 32768;
    return ret;
  }
  int getInteger(int led) {
    Range led_range(led * 32768, led * 32768 + 32768);
     int black_mix = 0;
      black_mix = (Range(start_, end_) & led_range).size();
    return black_mix;
  }
private:
  POSITION pos_;
  FRACTION fraction_;
  
  uint32_t start_;
  uint32_t end_;
  uint32_t num_leds_;
};

#endif
