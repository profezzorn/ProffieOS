#ifndef STYLES_PIXELATE_H
#define STYLES_PIXELATE_H


// Usage: PixelateX<COLOR, PIXEL_SIZE_FUNC>
// OR: Pixelate<COLOR, PIXEL_SIZE>
// COLOR: COLOR - color values to pixelate
// PIXEL_SIZE_FUNC: FUNCTION size of pixelation (defaults to 2)
// PIXEL_SIZE: NUMBER size of pixelation (defaults to 2)
// Returns: COLOR

// Pixelate a style.
// If you have a blade that has too many LEDs to run fast, you
// can use Pixelate<COLOR> to make it run faster, since it will
// only calculate "COLOR" half as many times.
// It can also be used as an effect of course.

template<class COLOR, class N = Int<2>>
class PixelateX {
 private:
  PONUA COLOR color_;
  PONUA N n_;
  int last_led_;
  decltype(color_.getColor(0)) last_color_;

 public:
  LayerRunResult run(BladeBase* blade) {
    n_.run(blade);
    return RunLayer(&color_, blade);
  }

  auto getColor(int led) -> decltype(color_.getColor(led)) {
    if (abs(led - last_led_) >= n_.getInteger(led)) {
      last_led_ = led;
      last_color_ = color_.getColor(led);
    }
    return last_color_;
  }
};

template<class COLOR, int N = 2>
using Pixelate = PixelateX<COLOR, Int<N>>;

#endif  // STYLES_PIXELATE_H
