#ifndef STYLES_INOUT_HELPER_H
#define STYLES_INOUT_HELPER_H

template<class T, int OUT_MILLIS, int IN_MILLIS, class OFF_COLOR=Rgb<0,0,0> >
class InOutHelper {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    off_color_.run(blade);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (blade->is_on()) {
      if (extension == 0.0) {
         // We might have been off for a while, so delta might
         // be insanely high.
         extension = 0.00001;
      } else {
         extension += delta / (OUT_MILLIS * 1000.0);
         extension = min(extension, 1.0f);
      }
    } else {
      if (is_same_type<OFF_COLOR, Rgb<0,0,0> >::value)
        if (extension == 0.0)
          blade->allow_disable();
      extension -= delta / (IN_MILLIS * 1000.0);
      extension = max(extension, 0.0f);
    }
    thres = extension * blade->num_leds() * 256;
  }
  OverDriveColor getColor(int led) {
    int black_mix = clampi32(thres - led * 256, 0, 255);
    OverDriveColor ret = base_.getColor(led);
    OverDriveColor off_color  = off_color_.getColor(led);
    ret.c = off_color.c.mix(ret.c, black_mix);
    return ret;
  }
private:
  T base_;
  OFF_COLOR off_color_;
  int thres = 0;
  float extension = 0.0;
  uint32_t last_micros_;
};

#endif
