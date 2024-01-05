#ifndef STYLES_ALPHA_H
#define STYLES_ALPHA_H

#include "mix.h"

// Usage: AlphaL<COLOR, ALPHA>
// COLOR: COLOR or LAYER
// ALPHA: FUNCTION
// Return value: LAYER
// This function makes a color transparent. The ALPHA function specifies just how opaque it should be.
// If ALPHA is zero, the returned color will be 100% transparent. If Alpha is 32768, the returned color will
// be 100% opaque. Note that if COLOR is already transparent, it will be made more transparent. Example:
// If COLOR is 50% opaque, and ALPHA returns 16384, the result will be 25% opaque.

// TODO: Make this take an arbitrary number of alpha functions and multiply them.
template<class COLOR, class ALPHA>
class AlphaL {
public:
  LayerRunResult run(BladeBase* blade) {
    LayerRunResult base_run_result = RunLayer(&color_, blade);
    FunctionRunResult ret = RunFunction(&alpha_, blade);
    switch (ret) {
      case FunctionRunResult::ONE_UNTIL_IGNITION: return base_run_result;
      case FunctionRunResult::ZERO_UNTIL_IGNITION: return LayerRunResult::TRANSPARENT_UNTIL_IGNITION;
      case FunctionRunResult::UNKNOWN: break;
    }
    return LayerRunResult::UNKNOWN;
  }

private:
  PONUA COLOR color_;
  PONUA ALPHA alpha_;

public:
  auto getColor(int led) -> decltype(color_.getColor(led) * alpha_.getInteger(led))  {
//    SCOPED_PROFILER();
    int alpha = alpha_.getInteger(led);
    if (alpha == 0) return RGBA_um_nod::Transparent();
    return color_.getColor(led) * alpha;  // clamp?
  }
};

// To enable Gradient/Mixes constricted within Bump<> and SmoothStep<> layers
// Example: AlphaMixL<Bump<Int<16384>,Int<16384>>,Red,Green,Blue> will produce a gradient within the Bump 
template<class MIX, class... COLORS> using AlphaMixL = AlphaL<Mix<MIX, COLORS...>, MIX>;

#endif
