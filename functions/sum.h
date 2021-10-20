#ifndef FUNCTIONS_SUM_H
#define FUNCTIONS_SUM_H

// Usage: SUM<A, B, ...>
// Adds A + B...
// A, B: INTEGER
// return value: INTEGER

template<class A, class B>
class SumBase {
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
  PONUA B b_;
};

template<class SVFA, class SVFB>
class SumSVF {
 public:
  void run(BladeBase* blade) {
    svfa_.run(blade);
    svfb_.run(blade);
  }
  int calculate(BladeBase* blade) {
    return (svfa_.calculate(blade) + svfb_.calculate(blade));
  }
 private:
  PONUA SVFA svfa_;
  PONUA SVFB svfb_;
};

// SVF promotion.
template<class F, class V> struct SumFinder3 { typedef SumBase<F, V> SumClass; };
template<class F, class V> struct SumFinder3<SingleValueAdapter<F>, SingleValueAdapter<V>> {
  typedef SingleValueAdapter<SumSVF<F, V>> SumClass;
};
template<class F, class V> using SumFinder2 = typename SumFinder3<F, V>::SumClass;

// Make Sum<> handle arbitrary number of arguments.
template<class ... VALUES> struct SumFinder {};
template<class A> struct SumFinder<A> { typedef A SumClass; };
template<class A, class ... B> struct SumFinder<A, B...> { typedef SumFinder2<A, typename SumFinder<B...>::SumClass> SumClass; };
template<class ... VALUES> using Sum = typename SumFinder<VALUES...>::SumClass;

#endif
