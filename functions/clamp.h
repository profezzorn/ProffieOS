#ifndef FUNCTIONS_CLAMP_H
#define FUNCTIONS_CLAMP_H

#include "int.h"
#include "svf.h"

// Usage: ClampF<F, MIN, MAX>
// Or:    ClampFX<F, MINCLASS, MAXCLASS>
// Clamps value between MIN and MAX
// F, MIN, MAX: INTEGER
// MINCLASS, MAXCLASS: FUNCTION
// return value: INTEGER

template<class F, class MIN, class MAX>
class ClampBase {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    min_.run(blade);
    max_.run(blade);
  }
  int getInteger(int led) {
    int min = min_.getInteger(led);
    int max = max_.getInteger(led);
    return clampi32(f_.getInteger(led), min, max);
  }

private:
  PONUA F f_;
  PONUA MIN min_;
  PONUA MAX max_;
};

template<class F, class MIN, class MAX>
class ClampSVF {
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

template<class F, class MIN, class MAX> struct ClampFinder { typedef ClampBase<F, MIN, MAX> ClampClass; };
template<class F, class MIN, class MAX> struct ClampFinder<SingleValueAdapter<F>, SingleValueAdapter<MIN>, SingleValueAdapter<MAX>> {
  typedef SingleValueAdapter<ClampSVF<F, MIN, MAX>> ClampClass;
};

template<class F, class MIN = Int<0>, class MAX = Int<32768>>
using ClampFX = typename ClampFinder<F, MIN, MAX>::ClampClass;

template<class F, int MIN = 0, int MAX = 32768>
using ClampF = ClampFX<F, Int<MIN>, Int<MAX>>;


#endif
