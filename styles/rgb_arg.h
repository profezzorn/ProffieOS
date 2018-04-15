#ifndef STYLES_RGB_ARG_H
#define STYLES_RGB_ARG_H

#include "../common/arg_parser.h"

template<int ARG>
class RgbArg {
public:
  RgbArg() {
    const char* arg = ArgParser->GetArg(ARG, "COLOR");
    if (arg) {
      int r = strtol(arg, &arg, 0);
      arg++;
      int g = strtol(arg, &arg, 0);
      arg++;
      int b = strtol(arg, &arg, 0);
      // TODO: color names?
      color_ = Color16(r, g, b);
    }
  }
  void run(BladeBase* base) {}
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.c = color_;
    ret.overdrive = false;
    return ret;
  }
private:
  Color16 color_;
};

#endif
