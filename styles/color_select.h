#ifndef STYLES_COLOR_SELECT_H
#define STYLES_COLOR_SELECT_H

// Usage: ColorSelect<SELECTION, TRANSITION, COLOR1, COLOR2, ...>
// SELECTION: function
// TRANSITION: transition
// COLOR1, COLOR2, ...:  COLOR
// Return value: COLOR
// Decides what color to return based on the current selection.
// The returned color will be selection % N (where N is the number of colors arguments).
// When the selection changes, the transition will be used to change from the old color to the new color.

template<class SELECTION, class TRANSITION, class... COLORS>
class ColorSelect {
public:
  ColorSelect() {
    if (is_same_type<FUNC, Variation>::value) {
      BladeBase::HandleFeature(HANDLED_FEATURE_CHANGE_TICKED);
    }
  }
  void run(BladeBase* blade) {
    f_.run(blade);
    colors_.run(blade);
    uint8_t selection = f_.getInteger(0);
    if (selection != variation_) {
      // Start a transition
      old_variation_ = variation_;
      variation_ = f_.getInteger(0);
      transition_.begin();
    }
    if (variation_ != old_variation_) {
      transition_.run(blade);
      if (!transition_.done()) return;
      old_variation_ = variation_;
    }
  }

private:
  SELECTION f_;
  TRANSITION transition_;
  uint8_t variation_= 0;
  uint8_t old_variation_ = 0;
  MixHelper<COLORS...> colors_;
public:
  auto getColor(int led) -> decltype(transition_.getColor(colors_.getColor(variation_, led), colors_.getColor(variation_, led), led)) {
//    SCOPED_PROFILER();
    auto ret = colors_.getColor(variation_, led);
    if (variation_ != old_variation_) {
      auto old = colors_.getColor(old_variation_, led);
      return transition_.getColor(old, ret, led);
    }
    return ret;
  }
};

#endif
