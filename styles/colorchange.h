#ifndef STYLES_COLORCHANGE_H
#define STYLES_COLORCHANGE_H

// Usage: ColorChange<TRANSITION, COLOR1, COLOR2, ...>
// TRANSITION: transition
// COLOR1, COLOR2, ...:  COLOR
// Return value: COLOR
// Decides what color to return based on the current variation.
// The returned color will be current_variation % N (where N is the number of colors arguments).
// When the variation changes, the transition will be used to change from the old color to the new color.

template<class TRANSITION, class... COLORS>
class ColorChange {
public:
  ColorChange() {
    BladeBase::HandleEffectType(EFFECT_CHANGE);
  }
  uint8_t GetVariation() {
    return (SaberBase::GetCurrentVariation() +
	    (sizeof...(COLORS) << 8)) % sizeof...(COLORS);
 }
  void run(BladeBase* blade) {
    colors_.run(blade);
    if (GetVariation() != variation_) {
      // Start a transition
      old_variation_ = variation_;
      variation_ = GetVariation();
      transition_.begin();
      transition_.run(blade);
    }
    if (variation_ != old_variation_) {
      transition_.run(blade);
      if (!transition_.done()) return;
      old_variation_ = variation_;
    }
  }

  OverDriveColor getColor(int led) {
    OverDriveColor ret = colors_.getColor(variation_, led);
    if (variation_ != old_variation_) {
      OverDriveColor old = colors_.getColor(old_variation_, led);
      ret = transition_.getColor(old, ret, led);
    }
    return ret;
  }
private:
  TRANSITION transition_;
  uint8_t variation_= 0;
  uint8_t old_variation_ = 0;
  MixHelper<COLORS...> colors_;
};

#endif
