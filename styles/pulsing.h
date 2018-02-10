#ifndef STYLES_PULSING_H
#define STYLES_PULSING_H

template<class COLOR1, class COLOR2, int pulse_millis>
class Pulsing {
public:
  void run(BladeBase* base) {
    c1_.run(base);
    c2_.run(base);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / (1000.0 * pulse_millis));
    mix_ = (sin_table[(int)floor(pos_ * 0x400)] + 16384) >> 1;
  }

  OverDriveColor getColor(int led) {
    OverDriveColor c1 = c1_.getColor(led);
    OverDriveColor c2 = c2_.getColor(led);
    c1.c = c1.c.mix2(c2.c, mix_);
    return c1;
  }

private:
  COLOR1 c1_;
  COLOR2 c2_;
  int mix_;
  uint32_t last_micros_;
  float pos_ = 0.0;
};

#endif
