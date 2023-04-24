#ifndef TRANSITIONS_WIPE_H
#define TRANSITIONS_WIPE_H

#include "wave.h"

// Usage: TrWipeX<MILLIS_FUNCTION>
// or: TrWipe<MILLIS>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// return value: TRANSITION
// Similar to saber ignition. In the beginning
// entire blade is color A, then color B starts at the base
// and extends up to the tip of the blade in the specified
// number of milliseconds.
template<class MILLIS>
class TrWipeX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    TransitionBaseX<MILLIS>::run(blade);
    fade_ = this->update(256 * blade->num_leds());
  }
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) -> decltype(MixColors(a,b,1,1)) {
    int mix = (Range(0, fade_) & Range(led << 8, (led << 8) + 256)).size();
    return MixColors(a, b, mix, 8);
  }
private:
  uint32_t fade_;
};

template<int MILLIS> using TrWipe = TrWipeX<Int<MILLIS>>;

// Usage: TrWipeInX<MILLIS_FUNCTION>
// or: TrWipeIn<MILLIS>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// return value: TRANSITION
// Like TrWipe, but from tip to base.
template<class MILLIS>
class TrWipeInX : public TransitionBaseX<MILLIS> {
public:
  TrWipeInX() : TransitionBaseX<MILLIS>(), fade_(0, 0) {}

  void run(BladeBase* blade) {
    TransitionBaseX<MILLIS>::run(blade);
    fade_ = Range(256 * blade->num_leds() -
		  this->update(256 * blade->num_leds()),
		  blade->num_leds() * 256);
  }
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) -> decltype(MixColors(a,b,1,1)) {
    int mix = (fade_ & Range(led << 8, (led << 8) + 256)).size();
    return MixColors(a, b, mix, 8);
  }
private:
  Range fade_;
};

template<int MILLIS> using TrWipeIn = TrWipeInX<Int<MILLIS>>;

// Usage: TrWipeSparkTip<SPARK_COLOR, MILLIS, SIZE>
// SPARK_COLOR = COLOR
// MILLIS = a number
// SIZE = a number
// return value: TRANSITION
// Same as TrWipe, but adds a "spark" tip to the
// leading edge of the wipe color.
template<class SPARK_COLOR, class MILLIS, class SIZE = Int<400>> 
	using TrWipeSparkTipX = TrJoin<TrWipeX<MILLIS>,TrSparkX<SPARK_COLOR,SIZE,MILLIS,Int<0>>>;

template<class SPARK_COLOR, int MILLIS, int SIZE = 400> 
	using TrWipeSparkTip = TrJoin<TrWipe<MILLIS>,TrSparkX<SPARK_COLOR,Int<SIZE>,Int<MILLIS>,Int<0>>>;

// Usage: TrWipeInSparkTip<SPARK_COLOR, MILLIS, SIZE>
// SPARK_COLOR = COLOR
// MILLIS = a number
// SIZE = a number
// return value: TRANSITION
// Like TrWipeSparkTip, but from tip to base.
template<class SPARK_COLOR, class MILLIS, class SIZE = Int<400>> 
	using TrWipeInSparkTipX = TrJoin<TrWipeInX<MILLIS>,TrSparkX<SPARK_COLOR,SIZE,MILLIS,Int<32768>>>;

template<class SPARK_COLOR, int MILLIS, int SIZE = 400> 
	using TrWipeInSparkTip = TrJoin<TrWipeIn<MILLIS>,TrSparkX<SPARK_COLOR,Int<SIZE>,Int<MILLIS>,Int<32768>>>;

#endif
