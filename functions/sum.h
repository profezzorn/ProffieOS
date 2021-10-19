#ifndef FUNCTIONS_SUM_H
#define FUNCTIONS_SUM_H

// Usage: SUM<A, B, ...>
// Adds A + B...
// A, B: INTEGER
// return value: INTEGER

template<class ... REST> class Sum {};

template<class X> class Sum<X> : public X {};

template<class A, class... B>
class Sum<A, B...> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  int getInteger(int led) {
    return a_.getInteger(led) + b_.getInteger(led);
  }

private:
  PONUA A a_;
  PONUA Sum<B...> b_;
};

#endif
