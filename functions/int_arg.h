#ifndef FUNCTIONS_INT_ARG_H
#define FUNCTIONS_INT_ARG_H

#include "../common/arg_parser.h"

template<int ARG, int DEFAULT_VALUE>
class IntArg {
public:
  IntArg() {
    char default_value[16];
    itoa(DEFAULT_VALUE, default_value, 10);
    const char* arg = CurrentArgParser->GetArg(ARG, "INT", default_value);
    if (arg) {
      value_ = strtol(arg, NULL, 0);
    }
  }
  void run(BladeBase* base) {}
  int getInteger(int led) { return value_; }
private:
  int value_;
};

#endif
