#ifndef FUNCTIONS_INT_H
#define FUNCTIONS_INT_H

#include "svf.h"

// Usage: Int<N>
// Returns N
// N: a number
// return value: INTEGER

template<int N>
class IntSVF {
public:
  FunctionRunResult run(BladeBase* base) {
    switch (N) {
      case 0: return FunctionRunResult::ZERO_UNTIL_IGNITION;
      case 32768: return FunctionRunResult::ONE_UNTIL_IGNITION;
      default: return FunctionRunResult::UNKNOWN;
    }
  }
  int calculate(BladeBase* blade) { return N; }
  int getInteger(int led) { return N; }
};

// Optimized specialization
template<int N> class SingleValueAdapter<IntSVF<N>> : public IntSVF<N> {};
template<int N> class SVFWrapper<IntSVF<N>> : public IntSVF<N> {};

template<int N>
using Int = SingleValueAdapter<IntSVF<N>>;

#endif
