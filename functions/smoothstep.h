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

template<class POS, class WIDTH>
class SmoothStep {
public:
  void run(BladeBase* blade) {
    pos_.run(blade);
    width_.run(blade);
    int width = width_.calculate(blade);
    float mult;
    if (width == 0) {
      mult = 32768;
    } else {
      mult = 32768 * 32768.0 / width / blade->num_leds();
    }
    mult_ = mult;
    location_ = blade->num_leds() * mult * (pos_.calculate(blade) - width/2) / 32768;
  }
  
  int getInteger(int led) {
    int x = led * mult_ - location_;
    if (x < 0) return 0;
    if (x > 32768) return 32768;
    return (((x * x) >> 14) * ((3<<14) - x)) >> 15;
  }
  
  PONUA SVFWrapper<POS> pos_;
  PONUA SVFWrapper<WIDTH> width_;
  int mult_;
  int location_;
};
  
#endif
