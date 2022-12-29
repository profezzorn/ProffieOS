#ifndef TRANSITIONS_BLINK_H
#define TRANSITIONS_BLINK_H

#include "base.h"

// Usage: TrBlinkX<MILLIS_FUNCTION, N, WIDTH_FUNCTION>
// or: TrBlink<MILLIS, N, WIDTH>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// N: a number
// WIDTH_FUNCTION: FUNCTION, defaults to Int<16384>
// WIDTH: a number, defaults to 16384
// return value: TRANSITION
// Blinks A-B N times in MILLIS, based on WIDTH (0 ~ 32768)
// If WIDTH = 16384 A and B appear equally, lower decreases length of A, higher increases length of A
template<class MILLIS, int N, class WIDTH = Int<16384>>
class TrBlinkX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    TransitionBaseX<MILLIS>::run(blade);
    width_.run(blade);
    blink_ = (this->update(32768 * N) & 0x7fff) < width_.calculate(blade);
  }
private:
  PONUA SVFWrapper<WIDTH> width_;
  bool blink_;
public:  
  template<class A, class B>
    auto getColor(const A& a, const B& b, int led) -> decltype(MixColors(a,b,1,1)) {
      if (blink_) {
        return a;
      } else {
        return b;
      }
  }
};

template<int MILLIS, int N, int WIDTH = 16384> using TrBlink = TrBlinkX<Int<MILLIS>, N, Int<WIDTH>>;

#endif
