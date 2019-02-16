#ifndef STYLES_RGB_ARG_H
#define STYLES_RGB_ARG_H

#include "../common/arg_parser.h"

template<int ARG>
class RgbArg {
public:
  RgbArg() {
    const char* arg = CurrentArgParser->GetArg(ARG, "COLOR");
    if (arg) {
      char* tmp;
      int r = strtol(arg, &tmp, 0);
      int g = strtol(tmp+1, &tmp, 0);
      int b = strtol(tmp+1, NULL, 0);
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
