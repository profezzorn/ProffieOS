#ifndef STYLES_SIMPLE_CLASH_H
#define STYLES_SIMPLE_CLASH_H

template<class T, class CLASH_COLOR = Rgb<255,255,255>, int CLASH_MILLIS = 40>
class SimpleClash {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    clash_color_.run(blade);
    uint32_t m = millis();
    if (blade->clash()) clash_millis_ = m;
    clash_ = m - clash_millis_ < CLASH_MILLIS;
  }
  OverDriveColor getColor(int led) {
    if (clash_) {
      return clash_color_.getColor(led);
    } else {
      return base_.getColor(led);
    }
  }
private:
  bool clash_;
  T base_;
  CLASH_COLOR clash_color_;
  uint32_t clash_millis_;
};

#endif
