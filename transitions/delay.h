#ifndef TRANSITIONS_DELAY_H
#define TRANSITIONS_DELAY_H

#include "base.h"

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
