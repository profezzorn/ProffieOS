#ifndef STYLES_RGB_ARG_H
#define STYLES_RGB_ARG_H

#include "../common/arg_parser.h"

template<int ARG, class DEFAULT_COLOR>
class RgbArg {
public:
  RgbArg() {
    char default_value[32];
    DEFAULT_COLOR default_color; // Note, no run() call for default_color!
    color_ = default_color.getColor(0).c;
    itoa(color_.r, default_value, 10);
    strcat(default_value, ",");
    itoa(color_.g, default_value + strlen(default_value), 10);
    strcat(default_value, ",");
    itoa(color_.b, default_value + strlen(default_value), 10);
    
    const char* arg = CurrentArgParser->GetArg(ARG, "COLOR", default_value);
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
