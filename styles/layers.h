#ifndef STYLES_LAYERS_H
#define STYLES_LAYERS_H

#include "alpha.h"
#include "../functions/int.h"
#include "colors.h"

// Usage: Layers<BASE, LAYER1, LAYER2, ...>
// BASE: COLOR or LAYER
// LAYER1, LAYER2: LAYER
// return value: COLOR or LAYER (same as BASE)
// This style works like layers in gimp or photoshop.
// In most cases, the layers are expected to be normally transparent effects
// that turn opaque when then want to paint an effect over the base color.
// If the base color is opqaque, the final result of this style will also be
// opaque. If the base color is transparent, the final result may also be transparent,
// depending on what the layers paint on top of the base color.

template<class BASE, class L1>
class Compose {
public:
  LayerRunResult run(BladeBase* blade) {
    LayerRunResult base_run_result = RunLayer(&base_, blade);
    LayerRunResult layer_run_result = RunLayer(&layer_, blade);
    switch (layer_run_result) {
      case LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION:
	return LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION;
      case LayerRunResult::TRANSPARENT_UNTIL_IGNITION:
	return base_run_result;
      case LayerRunResult::UNKNOWN:
	break;
    }
    return LayerRunResult::UNKNOWN;
  }
private:
  PONUA BASE base_;
  PONUA L1 layer_;
public:
  template<class T> T PRINT(T t, const char *f) { STDOUT << t << " @ " << f << "  type = " << __PRETTY_FUNCTION__ <<"\n"; return t; }
  auto getColor(int led) -> decltype(base_.getColor(led) << layer_.getColor(led)) {
    return base_.getColor(led) << layer_.getColor(led);
//    return PRINT(base_.getColor(led) << PRINT(layer_.getColor(led), "layer"), __PRETTY_FUNCTION__);
  }
};

// Optimize Layer<Black, LAYER>
// ALlows LAYER to be opaque.
template<class L1>
class Compose<BLACK, L1> {
public:
  LayerRunResult run(BladeBase* blade) {
    LayerRunResult layer_run_result = RunLayer(&layer_, blade);
    switch (layer_run_result) {
      case LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION:
      case LayerRunResult::TRANSPARENT_UNTIL_IGNITION:
	return LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION;
      case LayerRunResult::UNKNOWN:
	break;
    }
    return LayerRunResult::UNKNOWN;
  }
private:
  PONUA L1 layer_;
  SimpleColor dealpha(const SimpleColor& color) { return color; }
  OverDriveColor dealpha(const OverDriveColor& color) { return color; }
  SimpleColor dealpha(const RGBA_um_nod& color) { return SimpleColor(color.c * color.alpha >> 15; }
  OverDriveColor dealpha(const RGBA_um& color) { return OverDriveColor(color.c * color.alpha >> 15, color.overdrive); }
  SimpleColor dealpha(const RGBA_nod& color) { return SimpleColor(color.c); }
  OverDriveColor dealpha(const RGBA& color) { return OverDriveColor(color.c, color.overdrive); }
public:
  auto getColor(int led) -> AUTO_RETURN(dealpha(layer_.getColor(led)));
};


template<class BASE, class ... LAYERS> struct LayerSelector {};
template<class BASE> struct LayerSelector<BASE> { typedef BASE type; };
template<class BASE, class L1> struct LayerSelector<AlphaL<BASE, Int<0>>, L1> { typedef L1 type; };
template<class BASE, class L1> struct LayerSelector<BASE, L1> { typedef Compose<BASE, L1> type; };
template<class BASE, class L1, class ... REST> struct LayerSelector<BASE, L1, REST...> {
  typedef typename LayerSelector<Compose<BASE, L1>, REST...>::type type;
};

template<class BASE, class... LAYERS> using Layers = typename LayerSelector<BASE, LAYERS...>::type;

#endif
