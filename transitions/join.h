#ifndef TRANSITIONS_JOIN_H
#define TRANSITIONS_JOIN_H

// Usage: TrJoin<TR1, TR2, ...>
// TR1, TR2: TRANSITION
// return value: TRANSITION
// A little hard to explain, but all the specified
// transitions are run in parallel. Basically, we
// chain transitions like ((A TR1 B) TR2 B)

template<class ... REST> class TrJoin {};

template<class X> class TrJoin<X> : public X {};

template<class A, class... B>
class TrJoin<A, B...> {
 public:
  void begin() { a_.begin(); b_.begin(); }
  bool done() { return a_.done() && b_.done(); }
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
private:
  PONUA A a_;
  PONUA TrJoin<B...> b_;
public:
  template<class X, class Y>
    auto getColor(const X& a, const Y& b, int led) AUTO_RETURN(b_.getColor(a_.getColor(a, b, led), b, led))
};

// Usage: TrJoinR<TR1, TR2, ...>
// TR1, TR2: TRANSITION
// return value: TRANSITION
// Similar to TrJoin, but transitions are chained
// to the right instead of to the left. Like:
// (A TR2 (A TR1 B))

template<class ... REST> class TrJoinR {};
template<class X> class TrJoinR<X> : public X {};

template<class A, class... B>
class TrJoinR<A, B...> {
 public:
  void begin() { a_.begin(); b_.begin(); }
  bool done() { return a_.done() && b_.done(); }
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
private:
  PONUA A a_;
  PONUA TrJoinR<B...> b_;
public:
  template<class X, class Y>
    auto getColor(const X& a, const Y& b, int led) AUTO_RETURN(b_.getColor(a, a_.getColor(a, b, led), led));
};

#endif
