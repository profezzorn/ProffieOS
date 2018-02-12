#ifndef STYLES_CYLON_H
#define STYLES_CYLON_H

class BladeBase;

// Cylon/Knight Rider effect, a section of the strip is
// lit up and moves back and forth. Speed, color and fraction
// illuminated can be configured separately for on and off
// states.
template<class COLOR, int percentage, int rpm,
         class ON_COLOR = COLOR,
         int on_percentage = percentage,
         int on_rpm = rpm,
         int fade_time_millis = 1>
class Cylon {
public:
  void run(BladeBase* base) {
    c_.run(base);
    on_c_.run(base);

    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (delta > 1000000) delta = 1;

    float fade_delta = delta / 1000.0 / fade_time_millis;
    if (!base->is_on()) fade_delta = - fade_delta;
    fade_ = max(0.0, min(1.0, fade_ + fade_delta));

    float current_rpm = rpm * (1 - fade_) + on_rpm * fade_;
    float current_percentage =
       percentage * (1 - fade_) + on_percentage * fade_;
    fade_int_ = (int)(16384 * fade_);
    pos_ = fract(pos_ + delta / 60000000.0 * current_rpm);
    float fraction = current_percentage / 100.0;
    float pos = 0.5 + sin(pos_ * M_PI * 2) * (1.0 - fraction) / 2.0 - fraction / 2.0;
    num_leds_ = base->num_leds() * 16384;
    start_ = pos_ * num_leds_;
    end_ = (pos + fraction) * num_leds_;
    if (current_percentage == 100.0) {
      start_ = 0;
      end_ = num_leds_;
    } else if (current_percentage == 0.0) {
      start_ = 0;
      end_ = 0;
      base->allow_disable();
    } else {
      end_ = fract(pos_ + current_percentage / 100.0) * num_leds_;
    }
  }
  OverDriveColor getColor(int led) {
    led *= 16384;
    Range led_range(led, led + 16384);
    int black_mix = 0;
    if (start_ < end_) {
      black_mix = (Range(start_, end_) & led_range).size();
    } else {
      black_mix = (Range(0, end_) & led_range).size() +
                  (Range(start_, num_leds_) & led_range).size();
    }
    OverDriveColor c = c_.getColor(led);
    OverDriveColor on_c = on_c_.getColor(led);
    c.c = c.c.mix2(on_c.c, fade_int_);
    c.c = Color16().mix2(c.c, black_mix);
    return c;
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
  uint32_t last_micros_;
};

#endif
