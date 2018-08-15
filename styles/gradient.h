#ifndef STYLES_GRADIENT_H
#define STYLES_GRADIENT_H

// Usage: Gradient<A, B>
// OR: Gradient<A, B, C>
// OR: Gradient<A, B, C, D>
// A, B, C, D: COLOR
// return value: COLOR
// Gradient, color A at base, B at tip.
// Any number of sub-blades can be put together into a gradient.

class BlaseBase;

template<class... A>
class GradientHelper {};
  
template<class A>
class GradientHelper<A> {
public:
  static const size_t size = 0;
  void run(BladeBase* blade) { a_.run(blade); }
  OverDriveColor get(int led, int partition) { return a_.getColor(led); }
  A a_;
};

template<class A, class B, class... C>
class GradientHelper<A, B, C...> {
public:
  static const size_t size = 1;
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  OverDriveColor get(int led, int partition) {
    if (partition <= 16384) {
      OverDriveColor a = a_.getColor(led);
      OverDriveColor b = b_.a_.getColor(led);
      a.c = a.c.mix2(b.c, partition);
    } else {
      return b_.get(led, partition - 16384);
    }
  }
  A a_;
  GradientHelper<B, C...> b_;
};

template<class... COLOR>
class Gradient {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    mul_ =  (GradientHelper<COLOR...>::size << 14) / blade->num_leds();
  }
  OverDriveColor getColor(int led) {
    return colors_.get(led, led * mul_);
  }
private:
  GradientHelper<COLOR...> colors_;
  int mul_;
};

#endif
