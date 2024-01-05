#ifndef FUNCTIONS_ISLESSTHAN_H
#define FUNCTIONS_ISLESSTHAN_H

// Usage: IsLessThan<F, V>
// Returns 0 or 32768 based on V
// If F < V returns 32768, if F >= V returns 0 
// F, V: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class V>
class IsLessThanBase {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int getInteger(int led) {
    return (f_.getInteger(led) < v_.getInteger(led)) << 15;
  }

private:
  PONUA F f_;
  PONUA V v_;
};

template<class SVFA, class SVFB>
class IsLessThanSVF {
 public:
  void run(BladeBase* blade) {
    svfa_.run(blade);
    svfb_.run(blade);
  }
  int calculate(BladeBase* blade) {
    return (svfa_.calculate(blade) < svfb_.calculate(blade)) << 15;
  }
 private:
  PONUA SVFA svfa_;
  PONUA SVFB svfb_;
};

template<class F, class V> struct IsLessThanFinder { typedef IsLessThanBase<F, V> IsLessThanClass; };
template<class F, class V> struct IsLessThanFinder<SingleValueAdapter<F>, SingleValueAdapter<V>> {
  typedef SingleValueAdapter<IsLessThanSVF<F, V>> IsLessThanClass;
};
template<class F, class V> using IsLessThan = typename IsLessThanFinder<F, V>::IsLessThanClass;

template<class F, class V> using IsGreaterThan = IsLessThan<V, F>;

#endif
