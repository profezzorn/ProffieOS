#ifndef FUNCTIONS_CLAMP_H
#define FUNCTIONS_CLAMP_H

#include "int.h"
#include "svf.h"

// Usage: Clamp<F, MIN, MAX>
// Clamps value between MIN and MAX
// F, A, B: INTEGER
// return value: INTEGER

template<class F, class MIN = Int<0>, class MAX = Int<32768>>
class ClampXSVF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    min_.run(blade);
    max_.run(blade);
  }
  int calculate(BladeBase* blade) {
    int min = min_.calculate(blade);
    int max = max_.calculate(blade);
    return clampi32(f_.calculate(blade), min, max);
  }
private:
  PONUA SVFWrapper<F> f_;
  PONUA SVFWrapper<MIN> min_;
  PONUA SVFWrapper<MAX> max_;
};

template<class F, class MIN = Int<0>, class MAX = Int<32768>>
using ClampX = SingleValueAdapter<ClampXSVF<F, MIN, MAX>>;

template<class F, int MIN = 0, int MAX = 32768>
using Clamp = ClampX<F, Int<MIN>, Int<MAX>>;

#endif
