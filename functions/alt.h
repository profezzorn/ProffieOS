#ifndef FUNCTIONS_ALTF_H
#define FUNCTIONS_ALTF_H

#include "svf.h"

// Usage: AltF
// return value: INTEGER
// Returns current_alternative for use in ColorSelect<>, TrSelect<> or IntSelect<>

class AltSVF {
 public:
    void run(BladeBase* blade) {}
    int calculate(BladeBase* blade) { return current_alternative; }
    int getInteger(int led) { return current_alternative; }
};

// Optimized specialization
template<> class SingleValueAdapter<AltSVF> : public AltSVF {};
template<> class SVFWrapper<AltSVF> : public AltSVF {};

using AltF = SingleValueAdapter<AltSVF>;

#endif
