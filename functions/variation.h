#ifndef FUNCTIONS_VARIATION_H
#define FUNCTIONS_VARIATION_H

// Usage: Variation
// Returns 0-32768 based on current variation.
// returned value: FUNCTION
// Note that using Variation in your style means that the
// the automatic color rotation is turned off. The color wheel
// menu is unaffected, but the style is now responsible for actually
// changing the color.
// Note that if any blade styles are using ColorChange<>,
// the variation will return "ticked" values: 0, 1, 2, etc.

class Variation {
 public:
  Variation() {
    BladeBase::HandleFeature(HANDLED_FEATURE_CHANGE);
  }
  void run(BladeBase* blade) {
  }
  int getInteger(int led) {
    return SaberBase::GetCurrentVariation() & 0x7fff;
  }
};

#endif
