#ifndef FUNCTIONS_ISLESSTHAN_H
#define FUNCTIONS_ISLESSTHAN_H

// Usage: IsLessThan<F, V>
// Returns 0 or 32768 based on V
// If F < V returns 32768, if F >= V returns 0 
// F, V: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class V>
class IsLessThan {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int getInteger(int led) {
    return (f_.getInteger(led) < v_.getInteger(0)) << 15;
  }

private:
  F f_;
  V v_;
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
  SVFA svfa_;
  SVFB svfb_;
};

template<class SVFA, class SVFB>
class IsLessThan<SingleValueAdapter<SVFA>,
		 SingleValueAdapter<SVFB>> : public SingleValueAdapter<IsLessThanSVF<SVFA, SVFB>> {};

template<class SVFA, class SVFB>
class SVFWrapper<IsLessThan<SingleValueAdapter<SVFA>,
			    SingleValueAdapter<SVFB>>> : public IsLessThanSVF<SVFA, SVFB> {};

template<class F, class V> using IsGreaterThan = IsLessThan<V, F>;

#endif
