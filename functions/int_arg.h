#ifndef FUNCTIONS_INT_ARG_H
#define FUNCTIONS_INT_ARG_H

#include "../common/arg_parser.h"

class IntArgBase {
public:
  void run(BladeBase* base) {}
  int getInteger(int led) { return value_; }
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
class IntArg : public IntArgBase {
public:
  IntArg() {
    value_ = DEFAULT_VALUE;
    init(ARG);
  }
};

#endif
