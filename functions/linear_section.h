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
    int num_leds = base->num_leds();
    int fraction = fraction_.calculate(base);
    int pos = pos_.calculate(base);
    range_ = Range(clampi32((pos - fraction / 2) * num_leds, 0, 32768 * num_leds), clampi32((pos + fraction / 2) * num_leds, 0, 32768 * num_leds));
    return ret;
  }
  int getInteger(int led) {
    Range led_range(led * 32768, led * 32768 + 32768);
    return (Range(range_) & led_range).size();
  }
private:
  PONUA SVFWrapper<POSITION> pos_;
  PONUA SVFWrapper<FRACTION> fraction_;
  Range range_;
};

#endif
