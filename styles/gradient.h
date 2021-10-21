#ifndef STYLES_GRADIENT_H
#define STYLES_GRADIENT_H

// Usage: Gradient<A, B>
// OR: Gradient<A, B, C>
// OR: Gradient<A, B, C, D, ...>
// A, B, C, D: COLOR or LAYER
// return value: COLOR or LAYER (if any of the inputs are layers)
// Gradient, color A at base, B at tip.
// Any number of colors can be put together into a gradient.

class BlaseBase;

template<class... A>
class GradientHelper {};

template<class A>
class GradientHelper<A> {
public:
  static const size_t size = 0;
  void run(BladeBase* blade) { a_.run(blade); }
  PONUA A a_;
  auto get(int led, int partition) -> decltype(a_.getColor(led)) {
    return a_.getColor(led);
  }
};

template<class A, class B, class... C>
class GradientHelper<A, B, C...> {
public:
  static const size_t size = GradientHelper<B, C...>::size + 1;
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  PONUA A a_;
  PONUA GradientHelper<B, C...> b_;
  auto get(int led, int partition) -> decltype(MixColors(a_.getColor(0), b_.get(0,0), 1, 1)) {
    if (partition <= 16384) {
      return MixColors(a_.getColor(led), b_.a_.getColor(led), partition, 14);
    } else {
      return b_.get(led, partition - 16384);
    }
  }
};

template<class... COLOR>
class Gradient {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    mul_ =  (GradientHelper<COLOR...>::size << 14) / (blade->num_leds() - 1);
  }
private:
  PONUA GradientHelper<COLOR...> colors_;
  int mul_;
public:
  auto getColor(int led) -> decltype(colors_.get(led, led * mul_)) {
    return colors_.get(led, led * mul_);
  }
};

#endif
