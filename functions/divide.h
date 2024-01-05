#ifndef FUNCTIONS_DIVIDE_H
#define FUNCTIONS_DIVIDE_H

// Usage: Divide<F, V>
// Divide F by V
// If V = 0, returns 0
// F, V: FUNCTION, 
// return value: FUNCTION
// Please note that Divide<> isn't an exact inverse of Mult<> because mult uses fixed-point mathematics
// (it divides the result by 32768) while Divide<> doesn't, it just returns F / V

class BladeBase;

template<class F, class V>
class DivideBase {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int getInteger(int led) {
    int v = v_.getInteger(led);
    if (v == 0) {
      return 0;
    } else {
      return f_.getInteger(led) / v;
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
    int v = v_.calculate(blade);
    if (v == 0) {
      return 0;
    } else {
      return f_.calculate(blade) / v;
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
