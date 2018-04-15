#ifndef FUNCTIONS_INT_ARG_H
#define FUNCTIONS_INT_ARG_H

#include "../common/arg_parser.h"

template<int ARG>
class IntArg {
public:
  IntArg() {
    const char* arg = ArgParser->GetArg(ARG, "INT");
    if (arg) {
      value_ = strtol(arg, &arg, 0);
    }
  }
  void run(BladeBase* base) {}
  OverDriveColor getInteger(int led) { return value_; }
private:
  int value_;
};

#endif
