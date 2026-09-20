#ifndef STYLES_LAYERS_H
#define STYLES_LAYERS_H

#include "alpha.h"
#include "../functions/int.h"
#include "colors.h"
#include "../common/color.h"
#include <utility>

// Usage: Layers<BASE, LAYER1, LAYER2, ...>
// BASE: COLOR or LAYER
// LAYER1, LAYER2: LAYER
// return value: COLOR or LAYER (same as BASE)
// This style works like layers in Gimp or Photoshop: each layer adds to the stack covering the BASE.
// The written order of layers in the blade style code puts the BASE first,
// with any following layers visually covering it. (later layers draw over earlier ones)
// The rule for opacity is as follows:
//   The output of Layers<> is opaque if the BASE of that Layers<> is opaque.
//   If BASE is transparent: the overall result is transparent.
//   No additional solid opaque colors may be stacked on top of an opaque BASE.
//
// StylePtr<> expects its argument to be opaque.
// Therefore, when using StylePtr<Layers<BASE, ... >>(), BASE MUST be an opaque color.
//
// Layers<> itself can still be nested anywhere a STYLE/COLOR is allowed; only the top-level StylePtr<...>()
// requirement forces the requirement of the BASE to be opaque in that specific position.

template<class BASE, class L1>
class Compose {
public:
  static_assert(!color_details::IsOpaqueColor<decltype(std::declval<L1&>().getColor(0))>::value,
                "\n\n"
                "*** Layers<> error: Only the base color may be solid.\n\n");
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
  SimpleColor dealpha(const RGBA_um_nod& color) { return SimpleColor(color.c * color.alpha >> 15); }
  OverDriveColor dealpha(const RGBA_um& color) { return OverDriveColor(color.c * color.alpha >> 15, color.overdrive); }
  SimpleColor dealpha(const RGBA_nod& color) { return SimpleColor(color.c); }
  OverDriveColor dealpha(const RGBA& color) { return OverDriveColor(color.c, color.overdrive); }
public:
  auto getColor(int led) -> decltype(dealpha(layer_.getColor(led))) {
    return dealpha(layer_.getColor(led));
  }
};

template<class BASE, class ... LAYERS> struct LayerSelector {};
template<class BASE> struct LayerSelector<BASE> { typedef BASE type; };

// Drop fully transparent first item (Alpha 0) when there are exactly 2 args.
template<class BASE, class L1> struct LayerSelector<AlphaL<BASE, Int<0>>, L1> { typedef L1 type; };

// Drop fully transparent when it’s the second (right) item in a pair.
template<class BASE, class ABASE>
struct LayerSelector<BASE, AlphaL<ABASE, Int<0>>> { typedef BASE type; };

template<class BASE, class L1> struct LayerSelector<BASE, L1> { typedef Compose<BASE, L1> type; };
template<class BASE, class L1, class ... REST>
struct LayerSelector<BASE, L1, REST...> {
  typedef typename LayerSelector<typename LayerSelector<BASE, L1>::type, REST...>::type type;
};

template<class BASE, class... LAYERS> using Layers = typename LayerSelector<BASE, LAYERS...>::type;

#endif
