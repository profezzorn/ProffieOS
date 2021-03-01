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
