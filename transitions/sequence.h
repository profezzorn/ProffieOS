#ifndef TRANSITIONS_SEQUENCE_H
#define TRANSITIONS_SEQUENCE_H

#include "random.h"

// Usage: TrSequence<TR1, TR2, ...>
// TR1, TR2: TRANSITION
// return value: TRANSITION
// Each time a new transition is started, a transition is selected
// sequentially, such that the frist time a transition is started
// TR1 will be selected, then TR2, etc.
// When the end of the sequence is reached, the next transition selected
// wraps back around to TR1.

template<class... TRANSITION>
class TrSequence : public TrHelper3<TRANSITION...>{
public:
  void begin() {
    begin_ = true;
  }
  void run(BladeBase* blade) {
    if (begin_) {
      begin_ = false;
      this->selected_ = (this->selected_ + 1) % sizeof...(TRANSITION);
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
  int n_ = -1;
};

#endif
