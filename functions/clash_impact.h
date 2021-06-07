#ifndef FUNCTIONS_CLASH_IMPACT_H
#define FUNCTIONS_CLASH_IMPACT_H

#include "../common/saber_base.h"

// Usage: ClashImpact<MIN, MAX>
// MIN is minimum value Clash is detected (recommended range 0 ~ 500, default is 200)
// MAX is maximum impact to return 32768 (recommended range 1000 ~ 1600, default is 1600)
// Returns 0-32768 based on impact strength of clash
// returned value: INTEGER

template<class MIN, class MAX>
class ClashImpactFX {
public:
  void run(BladeBase* base) {
    min_cents_.run(base);
    max_cents_.run(base);
    float i = SaberBase::GetClashStrength() * 100;
    int min = min_cents_.getInteger(0);
    int max = max_cents_.getInteger(0);
    value_ = clampi32((i - min) * 32768 / (max - min), 0, 32768);
  }

  int getInteger(int led) {
    return value_;
  }

private:
  MIN min_cents_;
  MAX max_cents_;
  int value_;
};

template<int MIN = 200, int MAX = 1600>
using ClashImpactF = ClashImpactFX<Int<MIN>, Int<MAX>>;

#endif
