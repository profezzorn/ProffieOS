#ifndef FUNCTIONS_SUM_H
#define FUNCTIONS_SUM_H

// Usage: SUM<A, B>
// Adds A + B
// A, B: INTEGER
// return value: INTEGER

class BladeBase;

template<class A, class B>
class Sum {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  int getInteger(int led) {
    sum_ = a_ + b_;
    return sum_.getInteger(led);
  }

private:
  A a_;
  B b_;
  int sum_;
};

#endif
