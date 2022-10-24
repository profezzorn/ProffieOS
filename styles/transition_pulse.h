#ifndef STYLES_TRANSITION_PULSE_H
#define STYLES_TRANSITION_PULSE_H

#include "../transitions/concat.h"
#include "../functions/effect_increment.h"
#include "../transitions/random.h"

// Usage: TransitionPulseL<TRANSITION, PULSE>
// TRANSITION: TRANSITION
// PULSE: FUNCTION
// return value: COLOR
//
// When the specified PULSE happens TRANSITION layer will run.

template<class TRANSITION, class PULSE>
class TransitionPulseL {
public:
  LayerRunResult run(BladeBase* blade) {
    pulse_.run(blade);
    if (pulse_.calculate(blade)) {
      transition_.begin();
      run_ = true;
    }
    if (run_) {
      transition_.run(blade);
      if (transition_.done()) run_ = false;
    }
    last_detected_blade_effect = nullptr;
    return LayerRunResult::UNKNOWN;
  }
  
private:
  bool run_ = false;
  TRANSITION transition_;
  PONUA SVFWrapper<PULSE> pulse_;

public:
  auto getColor(int led) -> decltype(transition_.getColor(RGBA_um_nod::Transparent(),
                RGBA_um_nod::Transparent(), 1)) {
    if (run_) {
      return transition_.getColor(RGBA_um_nod::Transparent(),
          RGBA_um_nod::Transparent(), led);
    } else {
      return RGBA_um_nod::Transparent();
    }
  }
};

// Usage: TransitionSequenceWithResetL<PULSE, RESET_PULSE, TRANSITIONS...>
// PULSE, RESET_PULSE: FUNCTION
// TRANSITIONS: TRANSITION
// return value: COLOR
//
// When the specified PULSE happens TRANSITION will run in sequence
// When the specified RESET_PULSE happens the sequence will restart at 0,
// otherwise sequence will restart after last TRANSITION
// to loop sequence without resetting set RESET_PULSE to Int<0>
template<class PULSE, class RESET_PULSE, class... TRANSITION>
class TransitionSequenceWithResetL {
public:
  LayerRunResult run(BladeBase* blade) {
    pulse_.run(blade);
    reset_pulse_.run(blade);
    if (reset_pulse_.calculate(blade)) n_ = 0;
    if (pulse_.calculate(blade)) {
      while(n_ < 0) n_ += sizeof...(TRANSITION) << 8;
      uint8_t selection = n_ % sizeof...(TRANSITION);
      selected_ = transitions_.get(selection % sizeof...(TRANSITION));
      selected_->begin();
      run_ = true;
      n_ = (n_ + 1) % sizeof...(TRANSITION);
    }
    if (run_) {
      selected_->run(blade);
      if (selected_->done()) run_ = false;
    }
    if (!run_) {
      // All effects now wake the blade up again if needed, so it's safe to
      // always return TRANSPARENT_UNTIL_IGNITION
      return LayerRunResult::TRANSPARENT_UNTIL_IGNITION;
    }
    last_detected_blade_effect = nullptr;
    return LayerRunResult::UNKNOWN;
  }
  
private:
  bool run_ = false;
  PONUA TrHelper<TRANSITION...> transitions_;
  PONUA SVFWrapper<PULSE> pulse_;
  PONUA SVFWrapper<RESET_PULSE> reset_pulse_;
  int n_ = 0;
  TransitionInterface* selected_ = nullptr;

public:
  auto getColor(int led) -> decltype(selected_->getColor(RGBA_um_nod::Transparent(),
                RGBA_um_nod::Transparent(), 1)) {
    if (run_) {
      return selected_->getColor(RGBA_um_nod::Transparent(),
          RGBA_um_nod::Transparent(), led);
    } else {
      return RGBA_um_nod::Transparent();
    }
  }
};

#endif
