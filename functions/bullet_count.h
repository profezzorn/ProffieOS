#ifndef FUNCTIONS_BULLETCOUNT_H
#define FUNCTIONS_BULLETCOUNT_H

#ifdef BLASTER_SHOTS_UNTIL_EMPTY

int prop_GetBulletCount();

// Usage: BulletCount
// Return 0-N (number of bullets in the magazine.)
// Will cause compilation error if your prop doesn't have bullets.
// You must define BLASTER_SHOTS_UNTIL_EMPTY for this to be available.

class BulletCountSVF {
 public:
  void run(BladeBase* blade) {}
  int getInteger(int led) { return prop_GetBulletCount(); }
  int calculate(BladeBase* blade) { return prop_GetBulletCount(); }
};

// Optimized specialization
template<>
class SingleValueAdapter<BulletCountSVF> : public BulletCountSVF {};

using BulletCountF = SingleValueAdapter<BulletCountSVF>;


// Usage: BlasterCharge
// Return 0-32768 based on the fullness of the clip / magazine.
// Will cause compilation error if your prop doesn't have bullets.
// You must define BLASTER_SHOTS_UNTIL_EMPTY for this to be available.
// Recommended to be used as the EXTENSION argument to InOutHelperL.

class BlasterChargeSVF {
 public:
    void run(BladeBase* blade) {}
    int calculate(BladeBase* blade) {
      return prop_GetBulletCount() * 32768 / BLASTER_SHOTS_UNTIL_EMPTY;
    }
    int getInteger(int led) {
      return prop_GetBulletCount() * 32768 / BLASTER_SHOTS_UNTIL_EMPTY;
    }
};

// Optimized specialization
template<>
class SingleValueAdapter<BlasterChargeSVF> : public BlasterChargeSVF {};

using BlasterChargeF = SingleValueAdapter<BlasterChargeSVF>;

#endif

#endif  // FUNCTIONS_BULLETCOUNT_H
