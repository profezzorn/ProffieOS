#ifndef STYLES_GRADIENT_H
#define STYLES_GRADIENT_H

// Usage: Gradient<A, B>
// OR: Gradient<A, B, C>
// OR: Gradient<A, B, C, D, ...>
// A, B, C, D: COLOR or LAYER
// return value: COLOR or LAYER (if any of the inputs are layers)
// Gradient, color A at base, B at tip.
// Any number of colors can be put together into a gradient.

#include "../functions/ramp.h"

template<class... COLORS> using Gradient = Mix<RampF, COLORS...>;

#endif
