#ifndef FUNCTIONS_INT_SELECT_H
#define FUNCTIONS_INT_SELECT_H

#include "svf.h"
#include "../common/typelist.h"

// Usage: IntSelect<SELECTION, Int1, Int2...>
// SELECTION: FUNCTION
// Returns SELECTION of N 
// If SELECTION is 0, the first integer is returned, if SELECTION is 1, the second and so forth.
// N: numbers
// return value: INTEGER

template<class F, int... N>
class IntSelect {
public:
  FunctionRunResult run(BladeBase* blade) {
    FunctionRunResult frr = RunFunction(&f_, blade);
    int f = f_.calculate(blade);
    while(f < 0) f += sizeof...(N) << 8;
    f = f % sizeof...(N);
    const static int values[] = { N ... };
    value_ = values[f];
    if (frr != FunctionRunResult::UNKNOWN) {
      switch (value_) {
        case 0: return FunctionRunResult::ZERO_UNTIL_IGNITION;
        case 32768: return FunctionRunResult::ONE_UNTIL_IGNITION;
        default: break;
      }
    }
    return FunctionRunResult::UNKNOWN;
  }
  int getInteger(int led) { return value_; }

private:
  PONUA SVFWrapper<F> f_;
  int value_;
};

// Usage: IntSelectX<SELECTION, F1, F2, ...>
// SELECTION: FUNCTION
// Returns SELECTION of N 
// If SELECTION is 0, the first function is returned, if SELECTION is 1, the second and so forth.
// F1, F2...: FUNCITON
// return value: INTEGER
template<class TL> class IntSelectHelper {
public:
  typedef typename SplitTypeList<TL>::first_half FH;
  typedef typename SplitTypeList<TL>::second_half SH;
  void run(BladeBase* blade) {
    fh_.run(blade);
    sh_.run(blade);
  }
  int get(int N, int led) {
    if (N < FH::size) {
      return fh_.get(N, led); 
    } else {
      return sh_.get(N - FH::size, led); 
    }
  }
private:
  PONUA IntSelectHelper<FH> fh_;
  PONUA IntSelectHelper<SH> sh_;
};

template<class T> class IntSelectHelper<TypeList<T>> {
public:
  void run(BladeBase* blade) { f_.run(blade); }
  int get(int N, int led) { return f_.getInteger(led); }
public:
  PONUA T f_;
};

template<> class IntSelectHelper<TypeList<>> {
public:
  void run(BladeBase* blade) { }
  int get(int N, int led) { return 0; }
};

template<class F, class ... N>
class IntSelectX {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    n_.run(blade);
    f = f_.calculate(blade);
    f = f % sizeof...(N);
  }
  int getInteger(int led) { return n_.get(f, led); }
private:
  int f;
  PONUA SVFWrapper<F> f_;
  PONUA IntSelectHelper<TypeList<N...>> n_;
};

#endif
