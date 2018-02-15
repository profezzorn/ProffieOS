#ifndef STYLES_SIMPLE_CLASH_H
#define STYLES_SIMPLE_CLASH_H

// Usage: SimpleClash<BASE, CLASH_COLOR, CLASH_MILLIS>
// BASE: COLOR
// CLASH_COLOR: COLOR (defaults to white)
// CLASH_MILLIS: a number (defaults to 40)
// return value: COLOR
// Turns the blade to CLASH_COLOR for CLASH_MILLIS millseconds
// when a clash occurs.

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
