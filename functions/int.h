#ifndef FUNCTIONS_INT_H
#define FUNCTIONS_INT_H

// Usage: Int<N>
// Returns N
// N: a number
// return value: INTEGER

template<int N>
class Int {
public:
  LayerRunResult run(BladeBase* base) {
    switch (N) {
      case 0: return LayerRunResult::TRANSPARENT_UNTIL_IGNITION;
      case 32768: return LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION;
      default: return LayerRunResult::UNKNOWN;
    }
  }
  int getInteger(int led) { return N; }
};

#endif
