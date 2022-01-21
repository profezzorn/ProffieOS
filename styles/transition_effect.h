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
    if (effect_.DetectScoped(blade)) {
      transition_.begin();
      run_ = true;
    }
    if (run_) {
      transition_.run(blade);
      if (transition_.done()) run_ = false;
    }
    last_detected_blade_effect = nullptr;
    if (!run_) {
      // All effects now wake the blade up again if needed, so it's safe to
      // always return TRANSPARENT_UNTIL_IGNITION
      return LayerRunResult::TRANSPARENT_UNTIL_IGNITION;
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
    for (size_t i = 0; i < N; i++) {
      if (effect_.DetectScoped(blade)) {
	transitions_[pos_].begin();
	run_[pos_] = true;
	effects_[pos_] = last_detected_blade_effect;
	pos_++;
	if (pos_ >= N) pos_ = 0;
      }
    }
    for (size_t i = 0; i < N; i++) {
      if (run_[i]) {
	last_detected_blade_effect = effects_[i];
	transitions_[i].run(blade);
	if (transitions_[i].done()) run_[i] = false;
      }
    }
    last_detected_blade_effect = nullptr;
  }
  
private:
  size_t pos_ = 0;
  bool run_[N];
  TRANSITION transitions_[N];
  BladeEffect* effects_[N];
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
