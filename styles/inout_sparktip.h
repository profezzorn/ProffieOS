#ifndef STYLES_INOUT_SPARKTIP_H
#define STYLES_INOUT_SPARKTIP_H
// Usage: InOutHelper<BASE, OUT_MILLIS, IN_MILLIS>
// or: InOutHelper<BASE, OUT_MILLIS, IN_MILLIS, SPARK_COLOR>
// BASE, SPARK_COLOR: COLOR
// OUT_MILLIS, IN_MILLIS: a number
// return value: COLOR
// Similar to InOutHelper, but makes the tip a different color
// during extension.

template<class T, int OUT_MILLIS, int IN_MILLIS, class SPARK_COLOR = Rgb<255,255,255> >
class InOutSparkTip {
public:
  bool run(BladeBase* blade) {
    bool keep_running = true;
    base_.run(blade);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if ((on_ = blade->is_on())) {
      if (extension == 0.0) {
         // We might have been off for a while, so delta might
         // be insanely high.
         extension = 0.00001;
      } else {
         extension += delta / (OUT_MILLIS * 1000.0);
         extension = min(extension, 1.0f);
      }
    } else {
      if (extension == 0.0) keep_running = false;
      extension -= delta / (IN_MILLIS * 1000.0);
      extension = max(extension, 0.0f);
    }
    thres = extension * (blade->num_leds() + 4) * 256;
    return keep_running;
  }

  OverDriveColor getColor(int led) {
    OverDriveColor ret = base_.getColor(led);
    if (on_) {
      OverDriveColor spark = spark_color_.getColor(led);
      int spark_mix = clampi32(thres - 1024 - led * 256, 0, 255);
      ret.c = spark.c.mix(ret.c, spark_mix);
    }
    int black_mix = clampi32(thres - led * 256, 0, 255);
    ret.c = Color16().mix(ret.c, black_mix);
    return ret;
  }
private:
  T base_;
  bool on_;
  int thres = 0;
  float extension = 0.0;
  uint32_t last_micros_;
  SPARK_COLOR spark_color_;
};

#endif
