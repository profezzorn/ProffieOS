#ifndef FUNCTIONS_INT_ARG_H
#define FUNCTIONS_INT_ARG_H

#include "../common/arg_parser.h"
#include "svf.h"

class IntArgBase {
public:
  FunctionRunResult run(BladeBase* base) {
    switch (value_) {
      case 0: return FunctionRunResult::ZERO_UNTIL_IGNITION;
      case 32768: return FunctionRunResult::ONE_UNTIL_IGNITION;
      default: return FunctionRunResult::UNKNOWN;
    }
  }
  int getInteger(int led) { return value_; }
  int calculate(BladeBase* blade) { return value_; }
protected:
  void init(int argnum) {
    char default_value[16];
    itoa(value_, default_value, 10);
    const char* arg = CurrentArgParser->GetArg(argnum, "INT", default_value);
    if (arg) {
      value_ = strtol(arg, NULL, 0);
    }
  }
  
  int value_;
};

template<int ARG, int DEFAULT_VALUE>
class IntArgSVF : public IntArgBase {
public:
  IntArgSVF() {
    value_ = DEFAULT_VALUE;
    init(ARG);
  }
};

// Optimized specialization
template<int ARG, int DEFAULT_VALUE>
class SingleValueAdapter<IntArgSVF<ARG, DEFAULT_VALUE>> : public IntArgSVF<ARG, DEFAULT_VALUE> {};
template<int ARG, int DEFAULT_VALUE>
class SVFWrapper<IntArgSVF<ARG, DEFAULT_VALUE>> : public IntArgSVF<ARG, DEFAULT_VALUE> {};

template<int ARG, int DEFAULT_VALUE>
using IntArg = SingleValueAdapter<IntArgSVF<ARG, DEFAULT_VALUE>>;

#endif
