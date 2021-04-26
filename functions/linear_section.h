#ifndef FUNCTION_LINEAR_SECTION_H
#define FUNCTION_LINEAR_SECTION_H

#include <algorithm>

// Usage: LinearSectionF<POSITION, FRACTION>
// POSITION: FUNCTION position on the blade, 0-32768
// FRACTION: FUNCTION how much of the blade to light up, 0 = none
// return value: FUNCTION
// creates a "block" of pixels at POSITION taking up FRACTION of blade

class BladeBase;
template<class POSITION, class FRACTION>
class LinearSectionF {
public:
  FunctionRunResult run(BladeBase* base) {
    pos_.run(base);
    FunctionRunResult ret = RunFunction(&fraction_, base);
    num_leds = base->num_leds();
    int fraction = fraction_.getInteger(0);
    int pos = pos_.getInteger(0);
    start = clampi32((pos - fraction / 2) * num_leds, 0, 32768 * num_leds);
    end = clampi32((pos + fraction / 2) * num_leds, 0, 32768 * num_leds);
    range_ = Range(start, end);
    return ret;
  }
  int getInteger(int led) {
    Range led_range(led * 32768, led * 32768 + 32768);
    return (Range(range_) & led_range).size();
  }
private:
  POSITION pos_;
  FRACTION fraction_;
  Range range_;
  uint32_t start;
  uint32_t end;
  uint32_t num_leds;
};

#endif
