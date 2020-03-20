#ifndef TRANSITIONS_BOING_H
#define TRANSITIONS_BOING_H

#include "base.h"

// Usage: TrFadeX<MILLIS_FUNCTION, N>
// or: TrFade<MILLIS, N>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// N: a number
// return value: TRANSITION
// Similar to TrFade, but transitions back and forth between the two
// colors several times. (As specified by N). If N is 0, it's equal to
// TrFade. If N is 1 it transitions A-B-A-B, if N is 2, it is A-B-A-B-A-B,
// and so on.
template<class MILLIS, int N>
class TrBoingX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    TransitionBaseX<MILLIS>::run(blade);
    if (this->done()) {
      fade_ = 16384;
    } else {
      fade_ = (millis() - this->start_millis_) * 16384 * (N * 2 + 1) / this->len_;
      if (fade_ & 0x4000) {
	fade_ = 0x4000 - (fade_ & 0x3FFF);
      } else {
	fade_ &= 0x3FFF;
      }
    }
  }
private:
  uint32_t fade_;
public:  
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) AUTO_RETURN(MixColors(a, b, fade_, 14))
};

template<int MILLIS, int N> using TrBoing = TrBoingX<Int<MILLIS>, N>;

#endif
