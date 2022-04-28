#ifndef FUNCTIONS_SUBTRACT_H
#define FUNCTIONS_SUBTRACT_H

// Usage: SUBTRACT<A, B>
// Subtracts B from A (A - B)
// A, B: INTEGER
// return value: INTEGER

template<class A, class B>
class SubtractBase {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  int getInteger(int led) {
    return a_.getInteger(led) - b_.getInteger(led);
  }

private:
  PONUA A a_;
  PONUA B b_;
};

template<class SVFA, class SVFB>
class SubtractSVF {
 public:
  void run(BladeBase* blade) {
    svfa_.run(blade);
    svfb_.run(blade);
  }
  int calculate(BladeBase* blade) {
    return (svfa_.calculate(blade) - svfb_.calculate(blade));
  }
 private:
  PONUA SVFA svfa_;
  PONUA SVFB svfb_;
};

#endif
