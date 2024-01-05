#ifndef FUNCTIONS_SMOOTHSTEP_H
#define FUNCTIONS_SMOOTHSTEP_H

// Usage: SmoothStep<POS, WIDTH>
// POS, WIDTH: FUNCTION
// return value: FUNCTION
// POS: specifies the middle of the smoothstep, 0 = base of blade, 32768=tip
// WIDTH: witdth of transition, 0 = no transition, 32768 = length of blade
// Example: SmoothStep<Int<16384>, Int<16384>> returns 0 up until 25% of the blade.
// From there it has a smooth transition to 32768, which will be reached at 75% of
// the blade. If WIDTH is negative, the transition will go the other way.

class SmoothStepBase {
public:
  int getInteger(int led) {
    int x = led * mult_ - location_;
    if (x < 0) return 0;
    if (x > 32768) return 32768;
    return (((x * x) >> 14) * ((3<<14) - x)) >> 15;
  }
protected:
  int mult_;
  int location_;
};

template<class POS, class WIDTH>
class SmoothStep : public SmoothStepBase {
public:
  void run(BladeBase* blade) {
    pos_.run(blade);
    width_.run(blade);
    int width = width_.calculate(blade);
    if (width == 0) {
      mult_ = 32768;
      location_ = blade->num_leds() * pos_.calculate(blade);
    } else {
      mult_ = 32768 * 32768 / width / blade->num_leds();
      location_ =  32768 * pos_.calculate(blade) / width - 16384;
    }
  }
  
  PONUA SVFWrapper<POS> pos_;
  PONUA SVFWrapper<WIDTH> width_;
};
  
#endif
