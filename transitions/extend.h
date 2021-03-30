#ifndef TRANSITIONS_EXTEND_H
#define TRANSITIONS_EXTEND_H

#include "base.h"

// Usage: TrExtendX<MILLIS_FUNCTION, TRANSITION>
// or: TrExtend<MILLIS, TRANSITION>
// MILLIS_FUNCTION: FUNCTION
// TRANSITION: TRANSITION
// MILLIS: a number
// return value: TRANSITION
// Runs the specified triansition, then holds the
// last value for some additional time specified by
// MILLIS_FUNCTION.
template<class MILLIS, class TRANSITION>
class TrExtendX : public TRANSITION {
public:
  void run(BladeBase* blade) {
    TRANSITION::run(blade);
    if (!extending_ && TRANSITION::done()) {
      extending_ = true;
      millis_.begin();
    }
    if (extending_) {
      millis_.run(blade);
      millis_.update(0);
    }
  }
  void begin() {
    TRANSITION::begin();
    extending_ = false;
  }
  bool done() { return extending_ && millis_.done(); }
 private:
  TransitionBaseX<MILLIS> millis_;
  bool extending_ = false;
};

template<int MILLIS, class TRANSITION> using TrExtend = TrExtendX<Int<MILLIS>, TRANSITION>;

#endif
