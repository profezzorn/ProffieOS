#ifndef FUNCTIONS_INT_SELECT_H
#define FUNCTIONS_INT_SELECT_H

// Usage: IntSelect<SELECTION, Int1, Int2...>
// SELECTION: FUNCTION
// Returns SELECTION of N
// N: numbers
// return value: INTEGER

template<class F, int... N>
class IntSelect {
public:
  FunctionRunResult run(BladeBase* blade) {
    FunctionRunResult frr = RunFunction(&f_, blade);
    int f = f_.getInteger(0);
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
  F f_;
  int value_;
};

#endif
