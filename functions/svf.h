#ifndef FUNCTIONS_SVF_H
#define FUNCTIONS_SVF_H

#include "int.h"

// An SVF is a FUNCTION that always returns the same value for all LEDs.
// By defining these functions as SVFs instead of plain FUNCTIONS, we
// can use the template system to do some optimizations when we also only
// wish to call the function once per update.
// An SVF cannot count on calculate() being called every frame.

class SingleValueBase {
public:
  int getInteger(int led) { return value_; }
  int value_;
};

// Converts an SVF to a FUNCTION
template<class SVF>
class SingleValueAdapter : public SingleValueBase {
public:
  FunctionRunResult run(BladeBase* blade) {
    FunctionRunResult ret = RunFunction(&single_value_function_, blade);
    value_ = single_value_function_.calculate(blade);
    return ret;
  }
  PONUA SVF single_value_function_;
};

// Converts a FUNCTION to an SVF
template<class FUNC>
class SVFWrapper {
public:
  FunctionRunResult run(BladeBase* blade) { return RunFunction(&f_, blade); }
  int calculate(BladeBase* blade) { return f_.getInteger(0); }
private:
  PONUA FUNC f_;
};

// Optimized specializations
template<class SVF>
class SVFWrapper<SingleValueAdapter<SVF>> : public SVF {};


#endif
