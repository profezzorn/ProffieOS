#ifndef STYLES_CYLON_H
#define STYLES_CYLON_H

#include "../common/range.h"
#include "rgb.h"

// Usage: Cylon<COLOR, PERCENT, RPM>
// or: Cylon<OFF_COLOR, OFF_PERCENT, OFF_RPM, ON_COLOR, ON_PERCENT, ON_RPM, FADE_TIME_MILLIS, BASE_COLOR>
// OFF_COLOR, ON_COLOR, BASE_COLOR: COLOR
// OFF_RPM, OFF_PERCENT, ON_PERCENT, ON_RPM, FADE_TIME_MILLIS: a number
// return value: COLOR
// Cylon/Knight Rider effect, a section of the strip is
// lit up and moves back and forth. Speed, color and fraction
// illuminated can be configured separately for on and off
// states.
// The arguments for this style are divided into two groups of
// { COLOR, PERCENT, RPM }, one for while the blade is OFF, and the other
// while it's ON.
// BASE_COLOR is the color of pixels not part of the lit section, so if PERCENT
// is 0, the entire blade will be BASE_COLOR, and if PERCENT is 100, the entire blade
// will be OFF_COLOR or ON_COLOR (depending on blade state).
// FADE_TIME_MILLIS is the time taken to transition between the OFF and ON groups
// of values.

class BladeBase;

class CylonBase {
public:
  bool run(BladeBase* base, int off_percentage, int off_rpm, int on_percentage, int on_rpm, int fade_time_millis, bool off_color_is_black) {
    bool keep_running = true;

    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (delta > 1000000) delta = 1;

    float fade_delta = delta / 1000.0 / fade_time_millis;
    if (!base->is_on()) fade_delta = - fade_delta;
    fade_ = std::max<float>(0.0, std::min<float>(1.0, fade_ + fade_delta));

    float current_rpm = off_rpm * (1 - fade_) + on_rpm * fade_;
    float current_percentage =
       off_percentage * (1 - fade_) + on_percentage * fade_;
    fade_int_ = (int)(16384 * fade_);
    pos_ = fract(pos_ + delta / 60000000.0 * current_rpm);
    float fraction = current_percentage / 100.0;
    float pos = 0.5 + sinf(pos_ * M_PI * 2) * (1.0 - fraction) / 2.0 - fraction / 2.0;
    num_leds_ = base->num_leds() * 16384;
    start_ = pos * num_leds_;
    end_ = (pos + fraction) * num_leds_;
    if (current_percentage == 100.0) {
      start_ = 0;
      end_ = num_leds_;
    } else if (current_percentage == 0.0) {
      start_ = 0;
      end_ = 0;
      keep_running = !off_color_is_black;
    } else {
      end_ = (pos + fraction) * num_leds_;
    }
    return keep_running;
  }
protected:
  float fade_ = 0.0;
  int fade_int_;
  float pos_ = 0.0;
  uint32_t start_;
  uint32_t end_;
  uint32_t num_leds_;
  uint32_t last_micros_;
};

template<class OFF_COLOR, int off_percentage, int off_rpm,
         class ON_COLOR = OFF_COLOR,
         int on_percentage = off_percentage,
         int on_rpm = off_rpm,
         int fade_time_millis = 1,
         class BASE_COLOR = Rgb<0,0,0> >
class Cylon : public CylonBase {
public:
  bool run(BladeBase* base) {
    off_c_.run(base);
    on_c_.run(base);
    base_c_.run(base);
    return CylonBase::run(base, off_percentage, on_rpm, on_percentage, on_rpm, fade_time_millis, is_same_type<OFF_COLOR, Rgb<0,0,0> >::value);
  }
private:
  OFF_COLOR off_c_;
  ON_COLOR on_c_;
  BASE_COLOR base_c_;
public:
  auto getColor(int led) -> decltype(MixColors(base_c_.getColor(0), MixColors(off_c_.getColor(0), on_c_.getColor(0), 1, 15), 1, 15)) {
    Range led_range(led * 16384, led * 16384 + 16384);
    int black_mix = (Range(start_, end_) & led_range).size();
    auto off_c = off_c_.getColor(led);
    auto on_c = on_c_.getColor(led);
    auto base_c = base_c_.getColor(led);
    return MixColors(base_c, MixColors(off_c, on_c, fade_int_, 14),  black_mix, 14);
  }
};

#endif
