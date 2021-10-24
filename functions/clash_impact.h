#ifndef FUNCTIONS_CLASH_IMPACT_H
#define FUNCTIONS_CLASH_IMPACT_H

#include "../common/saber_base.h"

// Usage: ClashImpactFX<MIN, MAX>
// MIN is minimum value Clash is detected (recommended range 0 ~ 500, default is 200)
// MAX is maximum impact to return 32768 (recommended range 1000 ~ 1600, default is 1600)
// Returns 0-32768 based on impact strength of clash
// returned value: INTEGER

#include "svf.h"

template<class MIN = Int<200>, class MAX = Int<1600>>
class ClashImpactFXSVF {
public:
  void run(BladeBase* base) {
    min_cents_.run(base);
    max_cents_.run(base);
  }
  int calculate(BladeBase* base) {
    float i = SaberBase::GetClashStrength() * 100;
    int min = min_cents_.calculate(base);
    int max = max_cents_.calculate(base);
    return clampi32((i - min) * 32768 / max, 0, 32768);
  }

private:
  PONUA SVFWrapper<MIN> min_cents_;
  PONUA SVFWrapper<MAX> max_cents_;
};

template<class MIN = Int<200>, class MAX = Int<1600>>
using ClashImpactFX = SingleValueAdapter<ClashImpactFXSVF<MIN, MAX>>;

template<int MIN = 200, int MAX = 1600>
using ClashImpactF = ClashImpactFX<Int<MIN>, Int<MAX>>;

#endif
