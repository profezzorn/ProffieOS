#ifndef TRANSITIONS_INSTANT_H
#define TRANSITIONS_INSTANT_H

#include "base.h"

class TrInstant {
public:
  bool done() { return true; }
  void begin() {}
  void run(BladeBase* blade) {}
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    return b;
  }
};

#endif
