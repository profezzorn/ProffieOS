#ifndef TRANSITIONS_OPTION_H
#define TRANSITIONS_OPTION_H

#include "random.h"

// Usage: TrOption<OPTION, TR1, TR2, ...>
// OPTION: a number
// TR1, TR2: TRANSITION
// return value: TRANSITION
// transition option is picked from the specified list of
// transitions based on Int<>
// with Int<0> representing first transition

template<class F, class... TRANSITION>
class TrOption {
public:
  void begin() {
    begin_ = true;
  }
  void run(BladeBase* blade) {
    f_.run(blade);
    if (begin_) {
      begin_ = false;
      unsigned int n_ = f_.getInteger(0);
      selected_ = transitions_.get(n_ % sizeof...(TRANSITION));
      selected_->begin();
    }
      selected_->run(blade); 
    }

  RGBA getColor(const RGBA& a,
		const RGBA& b,
		int led) {
    return selected_->getColor(a, b, led);
  }
  bool done() {
    if (begin_) return false;
    if (!selected_) return true;
    return selected_->done();
  }

private:
  bool begin_ = false;
  F f_;
  TrHelper<TRANSITION...> transitions_;
  TransitionInterface* selected_ = nullptr;
};

#endif
