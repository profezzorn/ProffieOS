#ifndef FUNCTIONS_MOD_F_H
#define FUNCTIONS_MOD_F_H

#include "svf.h"

// Usage: ModF<F, MAX>
// F: FUNCTION
// MAX: FUNCTION (not zero) defaults to Int<32768>
// When F is greater than MAX, F wraps to 0
// When F is less than 0, F wraps to MAX
// returns Integer

template<class F, class MAX = Int<32768>>
class ModSVF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    max_.run(blade);
  }
  int calculate(BladeBase* blade) { return MOD(f_.calculate(blade), max_.calculate(blade)); }
private:
  PONUA SVFWrapper<F> f_;
  PONUA SVFWrapper<MAX> max_;
};

template<class F, class MAX = Int<32768>>
using ModF = SingleValueAdapter<ModSVF<F, MAX>>;

template<class F, class MAX>
class SingleValueAdapter<ModSVF<F, MAX>> : public ModSVF<F, MAX> {};

#endif
