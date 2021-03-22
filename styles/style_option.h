#ifndef STYLES_STYLE_OPTION_H
#define STYLES_STYLE_OPTION_H

// Usage StyleOption<OPTION, COLORS...>
// OPTION: a number
// COLORS: a color or style
// return value: COLORS
// color/style option is picked from the specified list of styles/colors
// based on OPTION value Int<>
// with Int<0> representing first color/style

template<class F, class... COLORS> 
class StyleOption {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    colors_.run(blade);
    n_ = f_.getInteger(0);
  }
private:
  F f_;
  int n_;
  MixHelper<COLORS...> colors_;

public:  
  auto getColor(int led) -> decltype(colors_.getColor(n_, led)) {
    return colors_.getColor(n_, led);
  }

};

#endif
