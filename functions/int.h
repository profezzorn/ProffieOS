#ifndef FUNCTIONS_INT_H
#define FUNCTIONS_INT_H

// Usage: Int<N>
// Returns N
// N: a number
// return value: INTEGER

template<int N>
class Int {
public:
  FunctionRunResult run(BladeBase* base) {
    switch (N) {
      case 0: return FunctionRunResult::ZERO_UNTIL_IGNITION;
      case 32768: return FunctionRunResult::ONE_UNTIL_IGNITION;
      default: return FunctionRunResult::UNKNOWN;
    }
  }
  int getInteger(int led) { return N; }
};

#endif
