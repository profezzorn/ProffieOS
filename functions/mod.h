#ifndef FUNCTIONS_MOD_F_H
#define FUNCTIONS_MOD_F_H

#include "svf.h"

class BladeBase;

template<class F, class MAX>
class ModBase {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    max_.run(blade);
  }
  int getInteger(int led) {
    if (max_.getInteger(led) == 0) {
      return 0;
    } else {
      return MOD(f_.getInteger(led), max_.getInteger(led));
    }
  }
private:
  PONUA F f_;
  PONUA MAX max_;
};

// Usage: ModF<F, MAX>
// F: FUNCTION
// MAX: FUNCTION (not zero)
// When F is greater than MAX, F wraps to 0
// When F is less than 0, F wraps to MAX
// returns Integer

template<class F, class MAX>
class ModSVF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    max_.run(blade);
  }
  int calculate(BladeBase* blade) { 
    if (max_.calculate(blade) == 0) {
      return 0;
    } else {
      return MOD(f_.calculate(blade), max_.calculate(blade));
    }
  }
private:
  PONUA F f_;
  PONUA MAX max_;
};

template<class F, class MAX> struct ModFinder { typedef ModBase<F, MAX> ModClass; };
template<class F, class MAX> struct ModFinder<SingleValueAdapter<F>, SingleValueAdapter<MAX>> {
  typedef SingleValueAdapter<ModSVF<F, MAX>> ModClass;
};
template<class F, class MAX> using ModF = typename ModFinder<F, MAX>::ModClass;

#endif
