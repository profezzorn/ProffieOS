#ifndef STYLES_RGB_H
#define STYLES_RGB_H

// Usage: Rgb<R, G, B>
// R, G, B: a number (0-255)
// return value: COLOR

// Usage: Rgb16<R, G, B>
// R, G, B: a number (0-65536)
// return value: COLOR

// The two RGB classes are equivialent, but Rgb16<> is much more precise,
// most of the time this doesn't matter as 255 different values is accurate
// enough, but especially for very dim colors, the extra accuracy can be
// very useful.

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
  LayerRunResult run(BladeBase* base) {
    if (R == 0 && G == 0 && B == 0) return LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION;
    return LayerRunResult::UNKNOWN;
  }
  SimpleColor getColor(int led) {
    return SimpleColor(color());
  }
};

// Simple solid color with 16-bit precision.
template<int R, int G, int B>
class Rgb16 {
public:
  static Color16 color() { return Color16(R, G, B); }
  LayerRunResult run(BladeBase* base) {
    if (R == 0 && G == 0 && B == 0) return LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION;
    return LayerRunResult::UNKNOWN;
  }
  SimpleColor getColor(int led) { return SimpleColor(color()); }
};

// Simple semi-transparent color with 16-bit precision.
template<int R, int G, int B, int A>
class Rgba16 {
public:
  void run(BladeBase* base) {}
  RGBA_um_nod getColor(int led) {
    return RGBA_um_nod(Color16(R, G, B), false, A >> 1);
  }
};

#endif
