#ifndef STYLES_TRANSITION_PULSE_H
#define STYLES_TRANSITION_PULSE_H

#include "../transitions/concat.h"
#include "../functions/effect_increment.h"

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

// Usage: TransitionPulseLoopL<TRANSITION, BeginTr, EndTr, PULSE>
// TRANSITION, BeginTr, EndTr: TRANSITION
// PULSE: FUNCTION
// return value: COLOR
//
// While the specified PULSE is TRUE, TRANSITION layer will run in a loop
// While PULSE is FALSE, TRANSITION layer is transparent
// BeginTr runs when PULSE first becomes TRUE
// EndTr runs when PULSE first becomes FALSE

template<class TRANSITION,
  class BeginTr,
  class EndTr,
  class PULSE>
class TransitionPulseLoopL {
public:
  void run(BladeBase* blade) {
    pulse_.run(blade);
    transition_.run(blade);
    if (active_ != pulse_.calculate(blade)) {
      if ((active_ = pulse_.calculate(blade))) {
        begin_tr_.begin();
        transition_.begin();
      } else {
        end_tr_.begin();
      }
    }
    if (active_ && transition_.done()) transition_.begin();
    begin_tr_.run(blade);
    end_tr_.run(blade);
  }

private:
  bool active_ = false;
  TRANSITION transition_;
  TransitionHelper<BeginTr> begin_tr_;
  TransitionHelper<EndTr> end_tr_;
  PONUA SVFWrapper<PULSE> pulse_;
public:
  auto getColor(int led) -> decltype(
    MixColors(end_tr_.getColor(begin_tr_.getColor(RGBA_um_nod::Transparent(), transition_.getColor(RGBA_um_nod::Transparent(),RGBA_um_nod::Transparent(),led), led), RGBA_um_nod::Transparent(), led),
	      begin_tr_.getColor(end_tr_.getColor(transition_.getColor(RGBA_um_nod::Transparent(),RGBA_um_nod::Transparent(),0), RGBA_um_nod::Transparent(), led), transition_.getColor(RGBA_um_nod::Transparent(),RGBA_um_nod::Transparent(),0), led), 1, 1)) {
    SCOPED_PROFILER();
    RGBA_um_nod off_color = RGBA_um_nod::Transparent();
    if (!begin_tr_ && !end_tr_) {
      if (active_) {
        return transition_.getColor(RGBA_um_nod::Transparent(),RGBA_um_nod::Transparent(),led);
      } else {
        return off_color;
      }
    } else {
      auto on_color = transition_.getColor(RGBA_um_nod::Transparent(),RGBA_um_nod::Transparent(),led);
      if (active_) {
        return begin_tr_.getColor(end_tr_.getColor(on_color, off_color, led), on_color, led);
      } else {
        return end_tr_.getColor(begin_tr_.getColor(off_color, on_color, led), off_color, led);
      }
    }
  }
};

#endif
