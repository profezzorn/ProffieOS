#ifndef STYLES_TRANSITION_EFFECT_H
#define STYLES_TRANSITION_EFFECT_H

#include "../transitions/concat.h"

// Usage: TransitionEffect<COLOR, EFFECT_COLOR, TRANSITION1, TRANSITION2, EFFECT>
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
  void run(BladeBase* blade) {
    if (effect_.Detect(blade)) {
      transition_.begin();
      run_ = true;
    }
    if (run_) {
      transition_.run(blade);
      if (transition_.done()) run_ = false;
    }
  }
  
private:
  bool run_ = false;
  TRANSITION transition_;
  OneshotEffectDetector<EFFECT> effect_;
public:
  auto getColor(int led) -> decltype(transition_.getColor(RGBA_um::Transparent(),
							  RGBA_um::Transparent(), 1)) {
    if (run_) {
      return transition_.getColor(RGBA_um::Transparent(),
				  RGBA_um::Transparent(), led);
    } else {
      return RGBA_um::Transparent();
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
  bool run_[3];
  TRANSITION transitions_[N];
  OneshotEffectDetector<EFFECT> effect_;
public:
  RGBA getColor(int led) {
    RGBA ret(RGBA_um::Transparent());
    for (int i = N - 1; i >= 0; i--) {
      size_t x = (i + pos_) % N;
      if (run_[x]) {
	ret = ret << transitions_[x].getColor(RGBA_um::Transparent(),
					      RGBA_um::Transparent(), led);
      }
    }
    return ret;
  }
};

template<class T, class EFFECT_COLOR, class TRANSITION1, class TRANSITION2, BladeEffectType EFFECT, int N = 3>
  using MultiTransitionEffect = Layers<T, MultiTransitionEffectL<TrConcat<TRANSITION1, EFFECT_COLOR, TRANSITION2>, EFFECT, N>>;


#endif
