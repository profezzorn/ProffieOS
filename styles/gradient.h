#ifndef STYLES_GRADIENT_H
#define STYLES_GRADIENT_H

// Usage: Gradient<A, B>
// OR: Gradient<A, B, C>
// OR: Gradient<A, B, C, D, ...>
// A, B, C, D: COLOR or LAYER
// return value: COLOR or LAYER (if any of the inputs are layers)
// Gradient, color A at base, B at tip.
// Any number of colors can be put together into a gradient.

#include "mix.h"

template<class... COLORS> 
class Gradient {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    mul_ =  ((sizeof...(COLORS)-1) << 15) / (blade->num_leds() - 1);
  }
private:
  PONUA MixHelper<COLORS...> colors_;
  int mul_;
public:  
  auto getColor(int led) -> decltype(colors_.getColor(1,1)) {
    int x = led * mul_;
    auto a = colors_.getColor(x >> 15, led);
    auto b = colors_.getColor((x >> 15) + 1, led);
    return MixColors(a, b, x & 0x7fff, 15);
  }
};

#endif
