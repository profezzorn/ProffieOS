#ifndef FUNCTIONS_INCREMENT_F_H
#define FUNCTIONS_INCREMENT_F_H

#include "svf.h"

// Usage: IncrementModulo<PULSE, MAX, INCREMENT>
// PULSE: FUNCTION (pulse type)
// MAX: FUNCTION (not zero) defaults to Int<32768>
// INCREMENT: FUNCTION defaults to Int<1>
// Increments by I each time PULSE occurs wraps around when
// it reaches MAX.
template<class PULSE, class MAX = Int<32768>, class INCREMENT = Int<1>>
class IncrementModuloFSVF {
public:
  void run(BladeBase* blade) {
    pulse_.run(blade);
    max_.run(blade);
    increment_.run(blade);
    if (pulse_.calculate(blade)) {
      value_ = (value_ + increment_.calculate(blade)) % max_.calculate(blade);
    }
  }
  int calculate(BladeBase* blade) { return value_; }
  int getInteger(int led) { return value_; }
private:
  PONUA SVFWrapper<PULSE> pulse_;
  PONUA SVFWrapper<MAX> max_;
  PONUA SVFWrapper<INCREMENT> increment_;
  int value_ = 0;
};

template<class PULSE, class MAX = Int<32768>, class INCREMENT = Int<1>>
using IncrementModuloF = SingleValueAdapter<IncrementModuloFSVF<PULSE, MAX, INCREMENT>>;

template<class PULSE, class MAX, class INCREMENT>
class SingleValueAdapter<IncrementModuloFSVF<PULSE, MAX, INCREMENT>> : public IncrementModuloFSVF<PULSE, MAX, INCREMENT> {};


// Usage: ThresholdPulseF<F, THRESHOLD, HYST_PERCENT>
// F: FUNCTION
// THRESHOLD: FUNCTION (defaults to Int<32768>)
// HYST_PERCENT: FUNCTION (defaults to Int<66>
// Returns 32768 once when F > THRESHOLD, then waits until
// F < THRESHOLD * HYST_PERCENT / 100 before going back
// to the initial state (waiting for F > THRESHOLD).
template<class F, class THRESHOLD = Int<32768>, class HYST_PERCENT = Int<66> >
class ThresholdPulseFSVF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    threshold_.run(blade);
    hyst_percent_.run(blade);
  }
  int calculate(BladeBase* blade) {
    int f = f_.calculate(blade);
    int threshold = threshold_.calculate(blade);
    if (triggered_) {
      if (f < threshold * hyst_percent_.calculate(blade) / 100) {
	triggered_ = false;
      }
    } else {
      if (f >= threshold) {
	triggered_ = true;
	return 32768;
      }
    }
    return 0;
  }
  
private:
  PONUA SVFWrapper<F> f_;
  PONUA SVFWrapper<THRESHOLD> threshold_;
  PONUA SVFWrapper<HYST_PERCENT> hyst_percent_;
  bool triggered_ = false;
};

template<class F, class THRESHOLD = Int<32768>, class HYST_PERCENT = Int<66> >
using ThresholdPulseF = SingleValueAdapter<ThresholdPulseFSVF<F, THRESHOLD, HYST_PERCENT>>;

// Usage: IncrementF<F, V, MAX, I, HYST_PERCENT>
// Increases by value I (up to MAX) each time F >= V
// Detection resets once F drops below V * HYST_PERCENT
// if greater than MAX returns 0
// F, V, I, MAX = numbers
// HYST_PERCENT = percent (defaults to 66)
// return value: INTEGER

// NOTE: this function is designed to separate "events" for use with *Select styles.  
// This function may break up SwingSpeed effects or other continuous responsive functions.
template<class F, class V = Int<32768>, class MAX = Int<32768>, class I = Int<1>, class HYST_PERCENT = Int<66>>
using IncrementF = IncrementModuloF<ThresholdPulseF<F, V, HYST_PERCENT>, MAX, I>;

#endif
