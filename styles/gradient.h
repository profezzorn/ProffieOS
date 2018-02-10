#ifndef STYLES_GRADIENT_H
#define STYLES_GRADIENT_H

// Gradient, color A at base, B at tip.
template<class A, class B>
class Gradient {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    num_leds_ = blade->num_leds();
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix2(b.c, led * 16384 / num_leds_);
    return a;
  }
private:
  A a_;
  B b_;
  int num_leds_;
};

#endif
