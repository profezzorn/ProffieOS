#ifndef STYLES_TRANSITION_EFFECT_H
#define STYLES_TRANSITION_EFFECT_H

#include "../transitions/concat.h"

// Usage: TransitionEffect<COLOR, EFFECT_COLOR, TRANSITION1, TRANSITION2, EFFECT>
// Or: TransitionEffectL<EFFECT_COLOR, TRANSITION1, TRANSITION2, EFFECT>
// COLOR, EFFECT_COLOR: COLOR
// TRANSITION1, TRANSITION2 : TRANSITION
// EFFECT: effect type
// return value: COLOR
//
// When the specified EFFECT happens (clash/blast/etc.) transition from COLOR to
// EFFECT_COLOR using TRANSITION1. Then transition back using TRANSITION2.

template<class TRANSITION, BladeEffectType EFFECT>
class TransitionEffectL {
public:
  LayerRunResult run(BladeBase* blade) {
    if (effect_.Detect(blade)) {
      transition_.begin();
      run_ = true;
    }
    if (run_) {
      transition_.run(blade);
      if (transition_.done()) run_ = false;
    }
    if (!run_) {
      switch (EFFECT) {
	// This a list of effects that cannot occur after retraction is done.
	case EFFECT_BOOT:
	case EFFECT_PREON:
	case EFFECT_IGNITION:
	case EFFECT_RETRACTION:

	  // NEWFONT is a special case, it CAN occur after retraction is done, but
	  // we also power the blade on at that time, so it's fine.
	case EFFECT_NEWFONT:
	  return LayerRunResult::TRANSPARENT_UNTIL_IGNITION;
	default: break;
      }
    }
    return LayerRunResult::UNKNOWN;
  }
  
private:
  bool run_ = false;
  TRANSITION transition_;
  OneshotEffectDetector<EFFECT> effect_;
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

template<class T, class EFFECT_COLOR, class TRANSITION1, class TRANSITION2, BladeEffectType EFFECT>
  using TransitionEffect = Layers<T, TransitionEffectL<TrConcat<TRANSITION1, EFFECT_COLOR, TRANSITION2>, EFFECT>>;

template<class TRANSITION, BladeEffectType EFFECT, int N=3>
class MultiTransitionEffectL {
public:
  MultiTransitionEffectL() { for (size_t i = 0; i < N; i++) run_[i] = false; }

void run(BladeBase* blade) {
    for (size_t i = 0; i < N; i++) 
    if (effect_.Detect(blade)) {
      transitions_[pos_].begin();
      run_[pos_] = true;
      pos_++;
      if (pos_ >= N) pos_ = 0;
    }
    for (size_t i = 0; i < N; i++) {
      if (run_[i]) {
	transitions_[i].run(blade);
	if (transitions_[i].done()) run_[i] = false;
      }
    }
  }
  
private:
  size_t pos_ = 0;
  bool run_[N];
  TRANSITION transitions_[N];
  OneshotEffectDetector<EFFECT> effect_;
public:
  auto getColor(int led) -> decltype(RGBA_um_nod::Transparent() << transitions_[0].getColor(RGBA_um_nod::Transparent(), RGBA_um_nod::Transparent(), led)) {
    decltype(getColor(0)) ret(RGBA_um_nod::Transparent());
    for (int i = N - 1; i >= 0; i--) {
      size_t x = (i + pos_) % N;
      if (run_[x]) {
	ret = ret << transitions_[x].getColor(RGBA_um_nod::Transparent(),
					      RGBA_um_nod::Transparent(), led);
      }
    }
    return ret;
  }
};

template<class T, class EFFECT_COLOR, class TRANSITION1, class TRANSITION2, BladeEffectType EFFECT, int N = 3>
  using MultiTransitionEffect = Layers<T, MultiTransitionEffectL<TrConcat<TRANSITION1, EFFECT_COLOR, TRANSITION2>, EFFECT, N>>;


#endif
