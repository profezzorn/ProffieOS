#ifndef STYLES_ROTATE_COLOR
#define STYLES_ROTATE_COLOR

// Usage: RotateColorsX<ROTATION, COLOR>
// ROTATION: FUNCTION
// COLOR: COLOR or LAYER
// return value: COLOR or LAYER (same as COLOR)
//
// ROTATION specifies how much to rotate the color in HSV (color wheel)
// space. 0 = none, 32768 = 360degrees

template<class ROTATION, class COLOR>
class RotateColorsX {
public:
  bool run(BladeBase* blade) {
    rotation_.run(blade);
    return RunStyle(&color_, blade);
  }

private:
  PONUA COLOR color_;
  PONUA ROTATION rotation_;
public:
  auto getColor(int led) -> decltype(color_.getColor(led)) {
    auto c = color_.getColor(led);
    c.c = c.c.rotate((rotation_.getInteger(led) & 0x7fff) * 3);
    return c;
  }
};

template<int rotation, class COLOR> using RotateColors = RotateColorsX<Int<rotation>, COLOR>;
template<class HUE> using HueX = RotateColorsX<HUE, Red>;
template<int HUE> using Hue = HueX<Int<HUE>>;

#endif
