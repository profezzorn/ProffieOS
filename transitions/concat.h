#ifndef TRANSITIONS_CONCAT_H
#define TRANSITIONS_CONCAT_H

// Usage: TrConcat<TRANSITION, INTERMEDIATE, TRANSITION, ...>
// TRANSITION: TRANSITION
// INTERMEDIATE: COLOR
// return value: TRANSITION
// Concatenates any number of transitions.
// In between each transition, we need an intermediate color
// to transition to.

template<class ... REST> class TrConcat  {};

template<class A> class TrConcat<A> : public A {};

template<class A, class INTERMEDIATE, class... B>
class TrConcat<A, INTERMEDIATE, B...> {
public:
  void begin() { a_.begin(); run_a_ = true; }
  bool done() { return !run_a_ && b_.done(); }
  void run(BladeBase* blade) {
    intermediate_.run(blade);
    if (run_a_) {
      a_.run(blade);
      if (!a_.done()) return;
      run_a_ = false;
      b_.begin();
    }
    b_.run(blade);
  }
  
private:
  bool run_a_ = false;
  A a_;
  TrConcat<B...> b_;
  INTERMEDIATE intermediate_;
public:  
  template<class X, class Y>
    auto getColor(const X& a, const Y& b, int led) -> decltype(MixColors(a_.getColor(a, intermediate_.getColor(led), led),
								 	 b_.getColor(intermediate_.getColor(led), b, led), 1,1)) {
    if (done()) return b;
    auto intermediate = intermediate_.getColor(led);
    if (run_a_) {
      return a_.getColor(a, intermediate, led);
    } else {
      return b_.getColor(intermediate, b, led);
    }
  }
};

#endif
