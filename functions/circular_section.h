#ifndef FUNCTION_CIRCULAR_SECTION_H
#define FUNCTION_CIRCULAR_SECTION_H

#include <algorithm>

// Usage: CircularSectionF<POSITION, FRACTION>
// POSITION: FUNCTION position on the circle or blade, 0-32768
// FRACTION: FUNCTION how much of the blade to light up, 0 = none, 32768 = all of it
// return value: FUNCTION
// Returns 32768 for LEDs near the position with wrap-around.
// Could be used with MarbleF<> for a marble effect, or with
// Saw<> for a spinning/colorcycle type effect.
// Example: If POSITION = 0 and FRACTION = 16384, then this function
// will return 32768 for the first 25% and the last 25% of the blade
// and 0 for the rest of the LEDs.
class BladeBase;
template<class POSITION, class FRACTION>
class CircularSectionF {
public:
  FunctionRunResult run(BladeBase* base) {
    pos_.run(base);
    FunctionRunResult ret = RunFunction(&fraction_, base);
    
    num_leds_ = base->num_leds();;
    int fraction = fraction_.getInteger(0);
    if (fraction == 32768) {
      start_ = 0;
      end_ = num_leds_ * 32768;
    } else if (fraction == 0) {
      start_ = 0;
      end_ = 0;
    } else {
      int pos = pos_.getInteger(0);
      start_ = ((pos + 32768 - fraction / 2) & 32767) * num_leds_;
      end_ = ((pos + fraction / 2) & 32767) * num_leds_;
    }
    num_leds_ *= 32768;
    return ret;
  }
  int getInteger(int led) {
    Range led_range(led * 32768, led * 32768 + 32768);
    int black_mix = 0;
    if (start_ <= end_) {
      black_mix = (Range(start_, end_) & led_range).size();
    } else {
      black_mix = (Range(0, end_) & led_range).size() +
                  (Range(start_, num_leds_) & led_range).size();
    }
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
