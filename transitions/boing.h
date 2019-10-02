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
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    OverDriveColor ret = a;
    ret.c = a.c.mix2(b.c, fade_);
    return ret;
  }
private:
  uint32_t fade_;
};

template<int MILLIS, int N> using TrBoing = TrBoingX<Int<MILLIS>, N>;

#endif
