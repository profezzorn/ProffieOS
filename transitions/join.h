#ifndef TRANSITIONS_JOIN_H
#define TRANSITIONS_JOIN_H

template<class ... REST> class TrJoin {};

template<class X> class TrJoin<X> : public X {};

template<class A, class... B>
class TrJoin<A, B...> {
  void begin() { a_.begin(); b_.begin(); }
  bool done() { return a_.done() && b_.done(); }
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    return b_.getColor(a_.getColor(a, b, led), b, led);
  }
  
private:
  A a_;
  TrJoin<B...> b_;
};

// Right join
template<class ... REST> class TrJoinR {};
template<class X> class TrJoinR<X> : public X {};

template<class A, class... B>
class TrJoinR<A, B...> {
  void begin() { a_.begin(); b_.begin(); }
  bool done() { return a_.done() && b_.done(); }
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    return b_.getColor(a, a_.getColor(a, b, led), led);
  }
  
private:
  A a_;
  TrJoinR<B...> b_;
};
  
#endif
