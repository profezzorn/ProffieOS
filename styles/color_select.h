#ifndef STYLES_COLOR_SELECT_H
#define STYLES_COLOR_SELECT_H

#include "../functions/variation.h"
#include "../functions/svf.h"

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
    if (is_same_type<SELECTION, Variation>::value) {
      BladeBase::HandleFeature(HANDLED_FEATURE_CHANGE_TICKED);
    }
  }
  void run(BladeBase* blade) {
    f_.run(blade);
    colors_.run(blade);
    int f = f_.calculate(blade);
    while(f < 0) f += sizeof...(COLORS) << 8;
    uint8_t selection = f % sizeof...(COLORS);
    if (selection != selection_) {
      // Start a transition
      old_selection_ = selection_;
      selection_ = f % sizeof...(COLORS);
      transition_.begin();
    }
    if (selection_ != old_selection_) {
      transition_.run(blade);
      if (!transition_.done()) return;
      old_selection_ = selection_;
    }
  }

private:
  PONUA SVFWrapper<SELECTION> f_;
  PONUA TRANSITION transition_;
  uint8_t selection_= 0;
  uint8_t old_selection_ = 0;
  PONUA MixHelper<COLORS...> colors_;
public:
  auto getColor(int led) -> decltype(transition_.getColor(colors_.getColor(selection_, led), colors_.getColor(selection_, led), led)) {
//    SCOPED_PROFILER();
    auto ret = colors_.getColor(selection_, led);
    if (selection_ != old_selection_) {
      auto old = colors_.getColor(old_selection_, led);
      return transition_.getColor(old, ret, led);
    }
    return ret;
  }
};

#endif
