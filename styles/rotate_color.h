#ifndef STYLES_ROTATE_COLOR
#define STYLES_ROTATE_COLOR

// Usage: RotateColorsX<ROTATION, COLOR>
// ROTATION: FUNCTION
// COLOR: COLOR
// return value: COLOR
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

  OverDriveColor getColor(int led) {
    OverDriveColor c = color_.getColor(led);
    c.c = c.c.rotate((rotation_.getInteger(led) & 0x7fff) * 3);
    return c;
  }

  COLOR color_;
  ROTATION rotation_;
};

template<int rotation, class COLOR> using RotateColors = RotateColorsX<Int<rotation>, COLOR>;
template<class HUE> using HueX = RotateColorsX<HUE, Red>;
template<int HUE> using Hue = HueX<Int<HUE>>;

#endif
