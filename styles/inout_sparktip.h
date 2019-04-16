#ifndef STYLES_INOUT_SPARKTIP_H
#define STYLES_INOUT_SPARKTIP_H
// Usage: InOutHelper<BASE, OUT_MILLIS, IN_MILLIS>
// or: InOutHelper<BASE, OUT_MILLIS, IN_MILLIS, SPARK_COLOR>
// BASE, SPARK_COLOR: COLOR
// OUT_MILLIS, IN_MILLIS: a number
// return value: COLOR
// Similar to InOutHelper, but makes the tip a different color
// during extension.

template<class T, class EXTENSION, class SPARK_COLOR = Rgb<255,255,255>, class OFF_COLOR=Rgb<0,0,0>, bool ALLOW_DISABLE=1>
class InOutSparkTipX {
public:

  bool run(BladeBase* blade) __attribute__((warn_unused_result)) {
    base_.run(blade);
    extension_.run(blade);
    on_ = blade->is_on();
    thres = (extension_.getInteger(0) * (blade->num_leds() + 4)) >> 7;
    if (ALLOW_DISABLE && is_same_type<OFF_COLOR, Rgb<0,0,0> >::value && thres == 0)
      return false;
    return true;
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
  SPARK_COLOR spark_color_;
  EXTENSION extension_;
};

template<class T, int OUT_MILLIS, int IN_MILLIS, class SPARK_COLOR = Rgb<255,255,255>, class OFF_COLOR=Rgb<0,0,0>, bool ALLOW_DISABLE=1>
  using InOutSparkTip = InOutSparkTipX<T,  InOutFunc<OUT_MILLIS, IN_MILLIS>, SPARK_COLOR, OFF_COLOR, ALLOW_DISABLE>;

#endif
