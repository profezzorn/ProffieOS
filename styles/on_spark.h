#ifndef STYLES_ON_SPARK_H
#define STYLES_ON_SPARK_H

// Usage: OnSpark<BASE, SPARK_COLOR, MILLIS>
// BASE: COLOR
// SPARK_COLOR: COLOR (defaults to white)
// MILLIS: a number (defaults to 200)
// return value: COLOR
// When you turn the saber on, it starts with SPARK_COLOR, and then
// fades to BASE over a peariod of MILLIS millseconds.

// Let's us use a different color right in the beginning.
template<class T, class SPARK_COLOR = Rgb<255,255,255>, class MILLIS = Int<200> >
class OnSparkX {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    millis_.run(blade);
    spark_color_.run(blade);
    uint32_t m = millis();
    if (on_ != blade->is_on()) {
      on_ = blade->is_on();
      if (on_) on_millis_ = m;
    }
    uint32_t t = millis() - on_millis_;
    uint32_t fade_millis = millis_.getInteger(0);
    if (t < fade_millis) {
      mix_ = 255 - 255 * t / fade_millis;
    } else {
      mix_ = 0;
    }
  }
  OverDriveColor getColor(int led) {
    OverDriveColor ret = base_.getColor(led);
    OverDriveColor spark = spark_color_.getColor(led);
    ret.c = ret.c.mix(spark.c, mix_);
    return ret;
  }
private:
  bool on_;
  int mix_;
  T base_;
  SPARK_COLOR spark_color_;
  uint32_t on_millis_;
  MILLIS millis_;
};

template<class T, class SPARK_COLOR = Rgb<255,255,255>, int MILLIS = 200>
  using OnSpark = OnSparkX<T, SPARK_COLOR, Int<MILLIS>>;

#endif
