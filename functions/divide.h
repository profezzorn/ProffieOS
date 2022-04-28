#ifndef FUNCTIONS_DIVIDE_H
#define FUNCTIONS_DIVIDE_H

// Usage: Divide<F, V>
// Divide F by V
// If V = 0, returns 0
// F, V: INTEGER, 
// return value: INTEGER

class BladeBase;

template<class F, class V>
class DivideBase {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int getInteger(int led) {
    if (v_.getInteger(led) == 0) {
      return 0;
    } else {
      return (f_.getInteger(led) / v_.getInteger(led));
    }
  }
private:
  PONUA F f_;
  PONUA V v_;
};

template<class F, class V>
class DivideSVF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int calculate(BladeBase* blade) {
    if (v_.calculate(blade) == 0) {
      return 0;
    } else {
      return (f_.calculate(blade) / v_.calculate(blade));
    }
  }
private:
  PONUA F f_;
  PONUA V v_;
};

template<class F, class V> struct DivideFinder { typedef DivideBase<F, V> DivideClass; };
template<class F, class V> struct DivideFinder<SingleValueAdapter<F>, SingleValueAdapter<V>> {
  typedef SingleValueAdapter<DivideSVF<F, V>> DivideClass;
};
template<class F, class V> using Divide = typename DivideFinder<F, V>::DivideClass;

#endif
