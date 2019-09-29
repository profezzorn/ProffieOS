#ifndef TRANSITIONS_CONCAT_H
#define TRANSITIONS_CONCAT_H

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
      if (!a_.done()) {
	a_.run(blade);
	return;
      } 
      run_a_ = false;
      b_.begin();
    }
    b_.run(blade);
  }
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    if (done()) return b;
    OverDriveColor intermediate = intermediate_.getColor(led);
    if (run_a_) {
      return a_.getColor(a, intermediate, led);
    } else {
      return b_.getColor(intermediate, b, led);
    }
  }
  
private:
  bool run_a_;
  A a_;
  TrConcat<B...> b_;
  INTERMEDIATE intermediate_;
};

#endif
