#ifndef STYLES_COLORCHANGE_H
#define STYLES_COLORCHANGE_H

#include "../functions/variation.h"
#include "color_select.h"

// Usage: ColorChange<TRANSITION, COLOR1, COLOR2, ...>
// TRANSITION: transition
// COLOR1, COLOR2, ...:  COLOR
// Return value: COLOR
// Decides what color to return based on the current variation.
// The returned color will be current_variation % N (where N is the number of colors arguments).
// When the variation changes, the transition will be used to change from the old color to the new color.

template<class TRANSITION, class... COLORS>
using ColorChange = ColorSelect<Variation, TRANSITION, COLORS...>;

#endif
