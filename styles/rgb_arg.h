#ifndef STYLES_RGB_ARG_H
#define STYLES_RGB_ARG_H

#include "../common/arg_parser.h"

// Usage: RgbArg<ARG, DEFAULT_COLOR>
// ARG: a number
// DEFAULT_COLOR: Must be Rgb<> or Rgb16<>
// Return value: COLOR
// This is used to create templates that can be configured dynamically.
// These templates can be assigned to presets from WebUSB or bluetooth.
// See style_parser.h for more details.

// Break out template-invariant functionality to save memory.
class RgbArgBase {
public:
  void run(BladeBase* base) {}
  SimpleColor getColor(int led) {
    return SimpleColor(color_);
  }
protected:
  void init(int argnum) {
    char default_value[32];
    itoa(color_.r, default_value, 10);
    strcat(default_value, ",");
    itoa(color_.g, default_value + strlen(default_value), 10);
    strcat(default_value, ",");
    itoa(color_.b, default_value + strlen(default_value), 10);
    
    const char* arg = CurrentArgParser->GetArg(argnum, "COLOR", default_value);
    if (arg) {
      char* tmp;
      int r = strtol(arg, &tmp, 0);
      int g = strtol(tmp+1, &tmp, 0);
      int b = strtol(tmp+1, NULL, 0);
      // TODO: color names?
      color_ = Color16(r, g, b);
    }
  }

  Color16 color_;
};

template<int ARG, class DEFAULT_COLOR>
class RgbArg : public RgbArgBase{
public:
  RgbArg() {
    DEFAULT_COLOR default_color; // Note, no run() call for default_color!
    color_ = default_color.getColor(0).c;
    init(ARG);
  }
};

#endif
