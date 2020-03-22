#ifndef TRANSITIONS_FADE_H
#define TRANSITIONS_FADE_H

#include "base.h"

// Usage: TrFadeX<MILLIS_FUNCTION>
// or: TrFade<MILLIS>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// return value: TRANSITION
// Linear fading between two colors in specified number of milliseconds.

template<class MILLIS>
class TrFadeX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    TransitionBaseX<MILLIS>::run(blade);
    fade_ = this->update(16384);
  }
private:
  uint32_t fade_;
public:  
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) AUTO_RETURN(MixColors(a, b, fade_, 14))
};

template<int MILLIS> using TrFade = TrFadeX<Int<MILLIS>>;

// Usage: TrSmoothFadeX<MILLIS_FUNCTION>
// or: TrSmoothFade<MILLIS>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// return value: TRANSITION
// Similar to TrFade, but uses a cubic fading function
// so fading starts slow, speeds up in the middle, then
// slows down at the end.
template<class MILLIS>
class TrSmoothFadeX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    TransitionBaseX<MILLIS>::run(blade);
    uint32_t x = this->update(32768);
    fade_ = (((x * x) >> 14) * ((3<<14) - x)) >> 16;
  }
private:
  uint32_t fade_;
public:  
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) AUTO_RETURN(MixColors(a, b, fade_, 14))
};

template<int MILLIS> using TrSmoothFade = TrSmoothFadeX<Int<MILLIS>>;

#endif
