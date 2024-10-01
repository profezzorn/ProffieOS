#ifndef TRANSITIONS_SELECT_H
#define TRANSITIONS_SELECT_H

#include "random.h"

// Usage: TrSelect<SELECTION, TR1, TR2, ...>
// SELECTION: FUNCTION
// TR1, TR2: TRANSITION
// return value: TRANSITION
// transition option is picked from the specified list of
// transitions based on Int<>
// with Int<0> representing first transition

template<class F, class... TRANSITION>
class TrSelect : public TrHelper3<TRANSITION...> {
public:
  void begin() {
    begin_ = true;
  }
  void run(BladeBase* blade) {
    f_.run(blade);
    if (begin_) {
      begin_ = false;
      this->selected_ = MOD(f_.calculate(blade), sizeof...(TRANSITION));
      TrHelper3<TRANSITION...>::begin();
    }
    TrHelper3<TRANSITION...>::run(blade);
  }

  bool done() {
    if (begin_) return false;
    return TrHelper3<TRANSITION...>::done();
  }

private:
  bool begin_ = false;
  PONUA SVFWrapper<F> f_;
};

#endif
