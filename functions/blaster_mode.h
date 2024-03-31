#ifndef FUNCTIONS_BLASTER_MODE_H
#define FUNCTIONS_BLASTER_MODE_H

int prop_GetBlasterMode();

// Usage: BlasterModeF
// Returns the current blaster mode as an integer:
// 0 for MODE_STUN, 1 for MODE_KILL, 2 for MODE_AUTO
// This function should only be used when BLASTER_SHOTS_UNTIL_EMPTY is defined,
// indicating that the current prop is a Blaster.
// Example usage in a style for Blue Stun, Red Kill, and Green Auto:
// StylePtr<ColorSelect<BlasterModeF, TrInstant, Blue, Red, Green>>()

class BlasterModeSVF {
public:
    void run(BladeBase* blade) {}
    int getInteger(int led) { return prop_GetBlasterMode(); }
    int calculate(BladeBase* blade) { return prop_GetBlasterMode(); }
};

template<>
class SingleValueAdapter<BlasterModeSVF> : public BlasterModeSVF {};

using BlasterModeF = SingleValueAdapter<BlasterModeSVF>;

#endif  // FUNCTIONS_BLASTER_MODE_H
