#ifndef STYLES_CYLON_H
#define STYLES_CYLON_H

#include "../common/range.h"
#include "rgb.h"

// Usage: Cylon<COLOR, PERCENT, RPM>
// or: ColorCycle<COLOR, PERCENT, RPM, ON_COLOR, ON_PERCENT, ON_RPM, FADE_TIME_MILLIS, OFF_COLOR>
// COLOR, ON_COLOR, OFF_COLOR: COLOR
// RPM, PERCENT, ON_PERCENT, ON_RPM, FADE_TIME_MILLIS: a number
// return value: COLOR
// Cylon/Knight Rider effect, a section of the strip is
// lit up and moves back and forth. Speed, color and fraction
// illuminated can be configured separately for on and off
// states.

class BladeBase;

template<class COLOR, int percentage, int rpm,
         class ON_COLOR = COLOR,
         int on_percentage = percentage,
         int on_rpm = rpm,
         int fade_time_millis = 1,
         class OFF_COLOR = Rgb<0,0,0> >
class Cylon {
public:
  bool run(BladeBase* base) {
    bool keep_running = true;
    c_.run(base);
    on_c_.run(base);
    off_c_.run(base);

    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (delta > 1000000) delta = 1;

    float fade_delta = delta / 1000.0 / fade_time_millis;
    if (!base->is_on()) fade_delta = - fade_delta;
    fade_ = std::max<float>(0.0, std::min<float>(1.0, fade_ + fade_delta));

    float current_rpm = rpm * (1 - fade_) + on_rpm * fade_;
    float current_percentage =
       percentage * (1 - fade_) + on_percentage * fade_;
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
      keep_running = !is_same_type<OFF_COLOR, Rgb<0,0,0> >::value;
    } else {
      end_ = (pos + fraction) * num_leds_;
    }
    return keep_running;
  }
private:
  float fade_ = 0.0;
  int fade_int_;
  float pos_ = 0.0;
  uint32_t start_;
  uint32_t end_;
  uint32_t num_leds_;
  COLOR c_;
  ON_COLOR on_c_;
  OFF_COLOR off_c_;
  uint32_t last_micros_;
public:
  auto getColor(int led) -> decltype(MixColors(off_c_.getColor(0), MixColors(c_.getColor(0), on_c_.getColor(0), 1, 15), 1, 15)) {
    Range led_range(led * 16384, led * 16384 + 16384);
    int black_mix = 0;
    black_mix = (Range(start_, end_) & led_range).size();
    auto c = c_.getColor(led);
    auto on_c = on_c_.getColor(led);
    auto off_c = off_c_.getColor(led);
    return MixColors(off_c, MixColors(c, on_c, fade_int_, 14),  black_mix, 14);
  }
};

#endif
