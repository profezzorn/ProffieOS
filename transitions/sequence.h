#ifndef TRANSITIONS_SEQUENCE_H
#define TRANSITIONS_SEQUENCE_H

#include "random.h"

// Usage: TrSequence<TR1, TR2, ...>
// TR1, TR2: TRANSITION
// return value: TRANSITION
// transition options used in sequence

template<class... TRANSITION>
class TrSequence {
public:
  void begin() {
    begin_ = true;
  }
  void run(BladeBase* blade) {
    if (begin_) {
      begin_ = false;
      n_ = (n_ + 1) % sizeof...(TRANSITION);
      selected_ = transitions_.get(n_);
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
  int n_ = -1;
  PONUA TrHelper<TRANSITION...> transitions_;
  TransitionInterface* selected_ = nullptr;
};

#endif
