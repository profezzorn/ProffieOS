#ifndef FUNCTIONS_CLASH_IMPACT_H
#define FUNCTIONS_CLASH_IMPACT_H

#include "../common/saber_base.h"

// Usage: ClashImpact<MIN, MAX>
// MIN is minimum value Clash is detected
// MAX is maximum impact to return 32768
// Returns 0-32768 based on impact strength of clash
// returned value: INTEGER

template<class MIN, class MAX>
class ClashImpactX {
public:
  void run(BladeBase* base) {
    min_.run(base);
    max_.run(base);
    float i = SaberBase::GetClashStrength();
    value_ = clampi32(((i - min_.getInteger(0)) / max_.getInteger(0)) * 32768, 0, 32768);
  }

  int getInteger(int led) {
    return value_;
  }

private:
  MIN min_;
  MAX max_;
  int value_;
};

template<int MIN = 2, int MAX = 14>
using ClashImpact = ClashImpactX<Int<MIN>, Int<MAX>>;

#endif
