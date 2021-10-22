#ifndef FUNCTIONS_SCALE_H
#define FUNCTIONS_SCALE_H

#include "int.h"

// Usage: Scale<F, A, B>
// Changes values in range 0 - 32768 to A-B
// F, A, B: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class A, class B>
class ScaleBase {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    a_.run(blade);
    b_.run(blade);
    int a = a_.calculate(blade);
    int b = b_.calculate(blade);
    mul_ = (b - a);
    add_ = a;
  }
  int getInteger(int led) {
    return (f_.getInteger(led) * mul_ >> 15) + add_;
  }
private:
  PONUA F f_;
  PONUA SVFWrapper<A> a_;
  PONUA SVFWrapper<B> b_;
  int add_;
  int mul_;
};

template<class F, int A, int B>
class ScaleBase<F, Int<A>, Int<B>> {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
  }
  int getInteger(int led) {
    return (f_.getInteger(led) * (B - A) >> 15) + A;
  }
private:
  PONUA F f_;
};

template<class SVFF, class A, class B>
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
  PONUA SVFF svff_;
  PONUA SVFWrapper<A> svfa_;
  PONUA SVFWrapper<B> svfb_;
};

template<class F, class A, class B> struct ScaleFinder { typedef ScaleBase<F, A, B> ScaleClass; };
template<class F, class A, class B>
struct ScaleFinder<SingleValueAdapter<F>, A, B> { typedef SingleValueAdapter<ScaleSVF<F, A, B>> ScaleClass; };
template<class F, class A, class B>
using Scale = typename ScaleFinder<F, A, B>::ScaleClass;
  
// To simplify inverting a function's returned value
// Example InvertF<BladeAngle<>> will return 0 when up and 32768 when down
template<class F> using InvertF = Scale<F, Int<32768>, Int<0>>;


#endif
