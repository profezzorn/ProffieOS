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
class TrSelect {
public:
  void begin() {
    begin_ = true;
  }
  void run(BladeBase* blade) {
    f_.run(blade);
    if (begin_) {
      begin_ = false;
      int f = f_.calculate(blade);
      while(f < 0) f += sizeof...(TRANSITION) << 8;
      uint8_t selection = f % sizeof...(TRANSITION);
      selected_ = transitions_.get(selection % sizeof...(TRANSITION));
      selected_->begin();
    }
    selected_->run(blade); 
  }

  RGBA getColor(const RGBA& a, const RGBA& b, int led) {
    return selected_->getColor(a, b, led);
  }
  bool done() {
    if (begin_) return false;
    if (!selected_) return true;
    return selected_->done();
  }

private:
  bool begin_ = false;
  PONUA SVFWrapper<F> f_;
  PONUA TrHelper<TRANSITION...> transitions_;
  TransitionInterface* selected_ = nullptr;
};

#endif
