#ifndef TRANSITIONS_DELAY_H
#define TRANSITIONS_DELAY_H

#include "base.h"

// Usage: TrDelayX<MILLIS_FUNCTION>
// or: TrDelay<MILLIS>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// return value: TRANSITION
// Waits for the specified number of milliseconds, then transitions
// to second color. Menant to be used with TrConcat
template<class MILLIS>
class TrDelayX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {}
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    return this->done() ? b : a;
  }
};

template<int MILLIS> using TrDelay = TrDelayX<Int<MILLIS>>;

#endif
