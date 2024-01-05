#ifndef FUNCTIONS_ON_SPARK_H
#define FUNCTIONS_ON_SPARK_H

// Usage: OnsparkF<MILLIS>
// MILLIS: FUNCTION (defaults to Int<200>)
// return value: FUNCTION
// When the blade turns on, this function starts returning
// 32768, then fades back to zero over MILLIS milliseconds.
// This is intended to be used with Mix<> or AlphaL<> to
// to create a flash of color or white when the blade ignites.

template<class MILLIS = Int<200> >
class OnSparkFSVF {
public:
  void run(BladeBase* blade) {
    millis_.run(blade);
  }
  int calculate(BladeBase* blade) {
    uint32_t m = millis();
    if (on_ != blade->is_on()) {
      on_ = blade->is_on();
      if (on_) on_millis_ = m;
    }
    uint32_t t = millis() - on_millis_;
    uint32_t fade_millis = millis_.calculate(blade);
    if (t < fade_millis) {
      return 32768 - 32768 * t / fade_millis;
    } else {
      return 0;
    }
  }
private:
  bool on_;
  uint32_t on_millis_;
  PONUA SVFWrapper<MILLIS> millis_;
};

template<class MILLIS = Int<200> >
using OnSparkF = SingleValueAdapter<OnSparkFSVF<MILLIS>>;


#endif
