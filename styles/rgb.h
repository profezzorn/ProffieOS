#ifndef STYLES_RGB_H
#define STYLES_RGB_H

// We use these templates instead of the Color class, because
// the compiler can inline everything. That means that we can
// have the flexibility of using a dynamically calculated color
// when we want without slowing anything down when we specify
// a constant color.
// See colors.h for a list of pre-defined colors.

// Simple solid color.
template<int R, int G, int B>
class Rgb {
public:
  static Color16 color() { return Color16(Color8(R,G,B)); }
  void run(BladeBase* base) {}
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.c = color();
    ret.overdrive = false;
    return ret;
  }
};

// Simple solid color with 16-bit precision.
template<int R, int G, int B>
class Rgb16 {
public:
  static Color16 color() { return Color16(R, G, B); }
  void run(BladeBase* base) {}
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.c = color();
    ret.overdrive = false;
    return ret;
  }
};

#endif
