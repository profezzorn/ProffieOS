#ifndef FUNCTIONS_ON_SPARK_H
#define FUNCTIONS_ON_SPARK_H

template<class MILLIS = Int<200> >
class OnSparkF {
public:
  void run(BladeBase* blade) {
    millis_.run(blade);
    uint32_t m = millis();
    if (on_ != blade->is_on()) {
      on_ = blade->is_on();
      if (on_) on_millis_ = m;
    }
    uint32_t t = millis() - on_millis_;
    uint32_t fade_millis = millis_.getInteger(0);
    if (t < fade_millis) {
      mix_ = 32768 - 32768 * t / fade_millis;
    } else {
      mix_ = 0;
    }
  }
  int getInteger(int led) { return mix_; }
private:
  bool on_;
  int mix_;
  uint32_t on_millis_;
  MILLIS millis_;
};

#endif
