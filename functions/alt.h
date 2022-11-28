#ifndef FUNCTIONS_ALTF_H
#define FUNCTIONS_ALTF_H

#include "svf.h"
#include "../common/looper.h"

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


// Usage: SyncAltToVarianceF
// return value: INTEGER (always zero)
// Enables Bidirectional synchronization between ALT and VARIANCE.
// If variance changes, so does alt, if alt changes, so does variance.
class SyncAltToVarianceSVF : private Looper {
public:
  FunctionRunResult run(BladeBase* blade) {
    return FunctionRunResult::ZERO_UNTIL_IGNITION;
  }

  const char* name() override { return "SyncAltToVariance"; }
  void Loop() override {
    if (num_alternatives == 0) return;
    int var = MOD(SaberBase::GetCurrentVariation(), num_alternatives);
    if (var == last_ && current_alternative == last_) return;
    if (last_ == 0x7fffffff) {
      // Change quietly.
      current_alternative = var;
    } else if (var != last_) {
      SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, var);
    } else {
      SaberBase::SetVariation(var = current_alternative);
    }
    last_ = var;
  }
  int calculate() { return 0; }
  int getInteger(int led) { return 0; }
private:
  int last_ = 0x7fffffff;
};

// Optimized specialization
template<> class SingleValueAdapter<SyncAltToVarianceSVF> : public SyncAltToVarianceSVF {};
template<> class SVFWrapper<SyncAltToVarianceSVF> : public SyncAltToVarianceSVF {};

using SyncAltToVarianceF = SingleValueAdapter<SyncAltToVarianceSVF>;

// Usage: SyncAltToVarianceL
// return value: LAYER (transparent)
// Synchronizes alt to variance, just put it somewhere in the layer stack. (but not first)
using SyncAltToVarianceL = AlphaL<Black, SyncAltToVarianceF>;

#endif
