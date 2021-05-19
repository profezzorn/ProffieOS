#ifndef FUNCTIONS_CLASH_IMPACT_H
#define FUNCTIONS_CLASH_IMPACT_H

#include "../common/saber_base.h"

// Usage: ClashImpact<MIN, MAX>
// MIN is minimum value Clash is detected
// MAX is maximum impact to return 32768
// Returns 0-32768 based on impact strength of clash
// returned value: INTEGER

template<class MIN, class MAX>
class ClashImpactF {
public:
  void run(BladeBase* base) {
    min_cents_.run(base);
    max_cents_.run(base);
    float i = SaberBase::GetClashStrength();
    value_ = clampi32(((i - (min_cents_.getInteger(0) / 100)) / (max_cents_.getInteger(0) / 100)) * 32768, 0, 32768);
  }

  int getInteger(int led) {
    return value_;
  }

private:
  MIN min_cents_;
  MAX max_cents_;
  int value_;
};

template<int MIN = 200, int MAX = 1400>
using ClashImpact = ClashImpactF<Int<MIN>, Int<MAX>>;

#endif
