#ifndef FUNCTIONS_EFFECT_INCREMENT_F_H
#define FUNCTIONS_EFFECT_INCREMENT_F_H

#include "svf.h"
#include "increment.h"

// Usage: EffectPulse<EFFECT>
// EFFECT: BladeEffectType
// Returns 32768 once for each time the given effect occurs.
template<BladeEffectType EFFECT>
class EffectPulseSVF {
public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    return effect_.Detect(blade) ? 32768 : 0;
  }
private:
  OneshotEffectDetector<EFFECT> effect_;
};

template<BladeEffectType EFFECT>
using EffectPulseF = SingleValueAdapter<EffectPulseSVF<EFFECT>>;

// Usage: LockupPulseF<LOCKUP_TYPE>
// LOCKUP_TYPE: a SaberBase::LockupType
// Returns 32768 once for each time the given lockup occurs.
template<SaberBase::LockupType LOCKUP_TYPE>
class LockupPulseSVF {
public:
  LockupPulseSVF() {
    BladeBase::HandleFeature(FeatureForLockupType(LOCKUP_TYPE));
  }
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    if (SaberBase::Lockup() == LOCKUP_TYPE) {
      return 32768;
    } else {
      return 0;
    }
  }
};

template<SaberBase::LockupType LOCKUP_TYPE>
using LockupPulseF = SingleValueAdapter<LockupPulseSVF<LOCKUP_TYPE>>;

// Usage: IncrementWithReset<PULSE, RESET_PULSE, MAX, I>
// PULSE: FUNCTION (pulse type) 
// RESET_PULSEE: FUNCTION (pulse type) defaults to Int<0> (no reset)
// MAX, I: FUNCTION
// Starts at zero, increments by I each time the PULSE occurse.
// If it reaches MAX it stays there.
// Resets back to zero when RESET_PULSE occurs.
template<class PULSE, class RESET_PULSE=Int<0>, class MAX = Int<32768>, class I = Int<1>>
class IncrementWithResetSVF {
public:
  void run(BladeBase* blade) {
    pulse_.run(blade);
    reset_pulse_.run(blade);
    max_.run(blade);
    i_.run(blade);
    if (reset_pulse_.calculate(blade)) {
      value_ = 0;
    }
    if (pulse_.calculate(blade)) {
      value_ = std::min<int>(value_ + i_.calculate(blade), max_.calculate(blade));
    }
  }
  int calculate(BladeBase* blade) { return value_; }
  int getInteger(int led) { return value_; }
private:
  PONUA SVFWrapper<PULSE> pulse_;
  PONUA SVFWrapper<RESET_PULSE> reset_pulse_;
  PONUA SVFWrapper<MAX> max_;
  PONUA SVFWrapper<I> i_;
  int value_ = 0;
};

template<class PULSE, class RESET_PULSE=Int<0>, class MAX = Int<32768>, class I = Int<1>>
using IncrementWithReset = SingleValueAdapter<IncrementWithResetSVF<PULSE, RESET_PULSE, MAX, I>>;

// optimized specialization
template<class PULSE, class RESET_PULSE, class MAX, class I>
class SingleValueAdapter<IncrementWithResetSVF<PULSE, RESET_PULSE, MAX, I>> : public IncrementWithResetSVF<PULSE, RESET_PULSE, MAX, I> {};

// Usage: EffectIncrementF<EFFECT, MAX, I>
// Increases by value I (up to MAX) each time EFFECT is triggered
// If current value + I = MAX, it returns 0.
// If adding I exceeds MAX, the function returns 0 + any remainder in excesss of MAX 
// I, MAX = numbers
// return value: INTEGER
template<BladeEffectType EFFECT, class MAX = Int<32768>, class I = Int<1>>
using EffectIncrementF = IncrementModuloF<EffectPulseF<EFFECT>, MAX, I>;

#endif
