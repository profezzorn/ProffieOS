#ifndef FUNCTIONS_SCALE_H
#define FUNCTIONS_SCALE_H

#include "int.h"

// Usage: Scale<F, A, B>
// Changes values in range 0 - 32768 to A-B
// F, A, B: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class A, class B>
class Scale {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    a_.run(blade);
    b_.run(blade);
    int a = a_.getInteger(0);
    int b = b_.getInteger(0);
    mul_ = (b - a);
    add_ = a;
  }
  int getInteger(int led) {
    return (f_.getInteger(led) * mul_ >> 15) + add_;
  }
private:
  F f_;
  A a_;
  B b_;
  int add_;
  int mul_;
};

#if 0
// Optimized specialization
template<class F, int A, int B>
class Scale<F, Int<A>, Int<B>> {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
  }
  int getInteger(int led) {
    return (f_.getInteger(led) * (B - A) >> 15) + A;
  }
private:
  F f_;
};
#endif

// To simplify inverting a function's returned value
// Example InvertF<BladeAngle<>> will return 0 when up and 32768 when down
template<class F> using InvertF = Scale<F, Int<32768>, Int<0>>;

template<class SVF, int A, int B>
class SVFWrapper<Scale<SingleValueAdapter<SVF>, Int<A>, Int<B>>> {
 public:
  void run(BladeBase* blade) { svf_.run(blade); }
  int calculate(BladeBase* blade) {
    return (svf_.calculate(blade) * (B - A) >> 15) + A;
  }
 private:
  SVF svf_;
};

template<class SVFF, class SVFA, class SVFB>
class ScaleSVF {
 public:
  void run(BladeBase* blade) {
    svff_.run(blade);
    svfa_.run(blade);
    svfb_.run(blade);
  }
  int calculate(BladeBase* blade) {
    int a = svfa_.calculate(blade);
    int b = svfb_.calculate(blade);
    return (svff_.calculate(blade) * (b - a) >> 15) + a;
  }
 private:
  SVFF svff_;
  SVFA svfa_;
  SVFB svfb_;
};

template<class SVFF, class SVFA, class SVFB>
class Scale<SingleValueAdapter<SVFF>,
            SingleValueAdapter<SVFA>,
            SingleValueAdapter<SVFB>> : public SingleValueAdapter<ScaleSVF<SVFF, SVFA, SVFB>> {};

//template<class SVFF, class SVFA, class SVFB>
//class SVFWrapper<Scale<SingleValueAdapter<SVFF>,
//		       SingleValueAdapter<SVFA>,
//		       SingleValueAdapter<SVFB>>> : public ScaleSVF<SVFF, SVFA, SVFB> {};

#endif
