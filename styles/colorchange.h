#ifndef STYLES_COLORCHANGE_H
#define STYLES_COLORCHANGE_H

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
