#ifndef FUNCTIONS_SCALE_H
#define FUNCTIONS_SCALE_H

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
    int a = a_.getInteger();
    int b = a_.getInteger();
    mul_ = (b - a) * 16384;
    add_ = a_;
  }
  int getInteger(int led) {
    return (f_.getInteger(led) * mul_ >> 14) + add_;
  }
private:
  F f_;
  A a_;
  B b_;
  int add_;
  int mul_;
};

#endif
