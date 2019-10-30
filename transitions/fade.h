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
    if (this->done()) {
      fade_ = 16384;
    } else {
      fade_ = (millis() - this->start_millis_) * 16384 / this->len_;
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
    if (this->done()) {
      fade_ = 16384;
    } else {
      int x = (millis() - this->start_millis_) * 32768 / this->len_;
      fade_ = (((x * x) >> 14) * ((3<<14) - x)) >> 16;
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

template<int MILLIS> using TrSmoothFade = TrSmoothFadeX<Int<MILLIS>>;

#endif
