#ifndef COMMON_COLOR_H
#define COMMON_COLOR_H

// Used to represent a color. Uses normal 8-bit-per channel RGB.
// Note that these colors are in linear space and their interpretation
// depends on the blade.
class Color8 {
  public:
  Color8() : r(0), g(0), b(0) {}
  Color8(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
  // x = 0..256
  Color8 mix(const Color8& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color8( ((256-x) * r + x * other.r) >> 8,
                   ((256-x) * g + x * other.g) >> 8,
                   ((256-x) * b + x * other.b) >> 8);
  }
  // x = 0..16384
  Color8 mix2(const Color8& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color8( ((16384-x) * r + x * other.r) >> 14,
                   ((16384-x) * g + x * other.g) >> 14,
                   ((16384-x) * b + x * other.b) >> 14);
  }
  // x = 0..32768
  Color8 mix3(const Color8& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color8( ((32768-x) * r + x * other.r) >> 15,
                   ((32768-x) * g + x * other.g) >> 15,
                   ((32768-x) * b + x * other.b) >> 15);
  }
  uint8_t select(const Color8& other) const {
    uint8_t ret = 255;
    if (other.r) ret = std::min<int>(ret, r * 255 / other.r);
    if (other.g) ret = std::min<int>(ret, g * 255 / other.g);
    if (other.b) ret = std::min<int>(ret, b * 255 / other.b);
    return ret;
  }

  enum Byteorder {
    NONE = 0,

    // RGB colors
    BGR=0x321,
    BRG=0x312,
    GBR=0x231,
    GRB=0x213,
    RBG=0x132,
    RGB=0x123,

    // RGBW colors
    BGRW=0x3214,
    BRGW=0x3124,
    GBRW=0x2314,
    GRBW=0x2134,
    RBGW=0x1324,
    RGBW=0x1234,

    WBGR=0x4321,
    WBRG=0x4312,
    WGBR=0x4231,
    WGRB=0x4213,
    WRBG=0x4132,
    WRGB=0x4123,

    // Energy-efficient RGBW colors (white *replaces RGB, not in addition to RGB)
    BGRw=0x7654,
    BRGw=0x7564,
    GBRw=0x6754,
    GRBw=0x6574,
    RBGw=0x5764,
    RGBw=0x5674,

    wBGR=0x4765,
    wBRG=0x4756,
    wGBR=0x4675,
    wGRB=0x4657,
    wRBG=0x4576,
    wRGB=0x4567,
  };

  static int num_bytes(int byteorder) {
    return byteorder <= 0xfff ? 3 : 4;
  }

  uint8_t getByte(int byteorder, int byte) {
    switch (byteorder >> (byte * 4) & 0x7) {
      default: return r;
      case 2: return g;
      case 3: return b;
      case 4: return std::min(r, std::min(g, b));
      case 5: return r - std::min(r, std::min(g, b));
      case 6: return g - std::min(r, std::min(g, b));
      case 7: return b - std::min(r, std::min(g, b));
    }
  }

  // RGB combine X = X
  static Byteorder combine_byteorder(int byteorder1, int byteorder2) {
    int ret = 0;
    for (int i = num_bytes(byteorder1) - 1; i >= 0; i--) {
      int b = (byteorder1 >> (i * 4) & 7);
      int pos = 3 - b;
      b = byteorder2 >> (pos * 4) & 7;
      ret |= b << (i * 4);
    }
    return (Byteorder)ret;
  }

  // invert(X) combine X = RGB
  // invert(RGB) = RGB
  static Byteorder invert_byteorder(int byteorder) {
    int ret = 0;
    for (int i = num_bytes(byteorder) - 1; i >= 0; i--) {
      int b = byteorder >> (i * 4) & 7;
      int pos = 3 - b;
      ret |= (3-i) << (pos * 4);
    }
    return (Byteorder)ret;
  }

  Color8 operator*(uint8_t v) const {
    return Color8(r * v / 255, g * v / 255, b * v / 255);
  }

  Color8 operator|(const Color8& other) const {
    return Color8(std::max(r, other.r), std::max(g, other.g), std::max(b, other.b));
  }

  void printTo(Print& p) {
    p.write('#');
    if (r < 16) p.write('0');
    p.print(r, HEX);
    if (g < 16) p.write('0');
    p.print(g, HEX);
    if (b < 16) p.write('0');
    p.print(b, HEX);
  }

  uint8_t r, g, b;
};

static int8_t color16_dither_matrix[4][4] = {
  { -127, 111,  -76,  94 },
  {    9, -59,   60,  -8 },
  {  -93,  77, -110, 127 },
  {   43, -25,   26, -42 },
};

class Color16 {
  public:
  constexpr Color16() : r(0), g(0), b(0) {}
  Color16(const Color8& c) : r(c.r * 0x101), g(c.g * 0x101), b(c.b * 0x101) {}
  constexpr Color16(uint16_t r_, uint16_t g_, uint16_t b_) : r(r_), g(g_), b(b_) {}
  // x = 0..256
  Color16 mix(const Color16& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color16( ((256-x) * r + x * other.r) >> 8,
                    ((256-x) * g + x * other.g) >> 8,
                    ((256-x) * b + x * other.b) >> 8);
  }
  Color16 mix_clamped(const Color16& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color16( clampi32(((256-x) * r + x * other.r) >> 8, 0, 65536),
                    clampi32(((256-x) * g + x * other.g) >> 8, 0, 65536),
                    clampi32(((256-x) * b + x * other.b) >> 8, 0, 65536));
  }
  // x = 0..16384
  Color16 mix2(const Color16& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color16( ((16384-x) * r + x * other.r) >> 14,
                    ((16384-x) * g + x * other.g) >> 14,
                    ((16384-x) * b + x * other.b) >> 14);
  }
  // x = 0..32768
  Color16 mix3(const Color16& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color16(((32768-x) * r + x * other.r) >> 15,
                   ((32768-x) * g + x * other.g) >> 15,
                   ((32768-x) * b + x * other.b) >> 15);
  }
  uint16_t select(const Color16& other) const {
    uint32_t ret = 65535;
    uint32_t tmp = 65535;
    if (other.r) ret = std::min<uint32_t>(ret, r * tmp / other.r);
    if (other.g) ret = std::min<uint32_t>(ret, g * tmp / other.g);
    if (other.b) ret = std::min<uint32_t>(ret, b * tmp / other.b);
    return ret;
  }

  Color8 dither(int n) const {
    return Color8(clampi32((r+n) >> 8, 0, 255),
                  clampi32((g+n) >> 8, 0, 255),
                  clampi32((b+n) >> 8, 0, 255));
  }

  Color8 dither(int x, int y) const {
    return dither(color16_dither_matrix[x & 3][y & 3]);
  }

  uint16_t getShort(int byteorder, int byte) {
    switch (byteorder >> (byte * 4) & 0x7) {
      default: return r;
      case 2: return g;
      case 3: return b;
      case 4: return std::min(r, std::min(g, b));
      case 5: return r - std::min(r, std::min(g, b));
      case 6: return g - std::min(r, std::min(g, b));
      case 7: return b - std::min(r, std::min(g, b));
    }
  }

  void printTo(Print& p) {
    p.write('#');
    p.print(r);
    p.write(',');
    p.print(g);
    p.write(',');
    p.print(b);
  }

private:
  static int f(int n, int C, int MAX) {
    int k = n % 98304;
    return MAX - C * clampi32(std::min(k, 65536 - k), 0, 16384) / 16384;
  }

public:
  // angle = 0 - 98304 (32768 * 3) (non-inclusive)
  Color16 rotate(int angle) const {
    int H;
    if (!angle) return *this;
    int MAX = std::max(r, std::max(g, b));
    int MIN = std::min(r, std::min(g, b));
    int C = MAX - MIN;
    if (C == 0) return *this;  // Can't rotate something without color.
    // Note 16384 = 60 degrees.
    if (r == MAX) {
      // r is biggest
      H = 16384 * (g - b) / C;
    } else if (g == MAX) {
      // g is biggest
      H = 16384 * (b - r) / C + 16384 * 2;
    } else {
      // b is biggest
      H = 16384 * (r - g) / C + 16384 * 4;
    }
    H += angle;
    return Color16(f(5*16384+H, C, MAX),
                   f(3*16384+H, C, MAX),
                   f(1*16384+H, C, MAX));
  }

  uint16_t r, g, b;
};

struct OverDriveColor {
  OverDriveColor() {}
  OverDriveColor(const Color16 &c_, bool o) : c(c_), overdrive(o) {}
  Color16 c;
  bool overdrive;
  
  void printTo(Print& p) {
    if (overdrive) p.write('!');
    c.printTo(p);
  }
};

// SIMPLE
// Opaque.fade -> Opaque or Alpha?
// Alpha.fade -> Alpha
// PMAlpha.fade -> PMAlpha?
// Opaque mix Opaque -> Opaque
// Opaque mix Alpha -> PmAlpha
// Opaque mix PmAlpha -> PmAlpha
// Alpha mix PmAlpha -> PmAlpha
// Opaque paste Opaque -> Opaque
// Opaque paste Alpha -> Opaque
// Opaque paste PmAlpha -> Opaque
// Alpha paste Alpha -> PmAlpha
// Alpha paste PmAlpha -> PmAlpha
// Alpha paste PmAlpha -> PmAlpha

// mix: A, B  A * (1-x) + B * x
// paint B over A: B + A * (1 - B.a)

struct Color32 {
  Color32(uint32_t r_, uint32_t g_, uint32_t b_) : r(r_), g(g_), b(b_) {}
  Color16 operator>>(int shift) {
    return Color16(r >> shift, g >> shift, b >> shift);
  }
  Color32 operator+(const Color32& other) { return Color32(r + other.r, g + other.g, b + other.b); }
  uint32_t r, g, b;
};

inline Color32 operator*(const Color16& c, uint16_t x) {
  return Color32(c.r * x, c.g * x, c.b * x);
}
inline Color16 operator+(const Color16& a, const Color16 &b) {
  return Color16(a.r + b.r, a.g + b.g, a.b + b.b);
}

// Unmultiplied RGBA, used as a temporary and makes optimization easier.
struct RGBA_um {
  constexpr RGBA_um(Color16 c_, bool od, uint16_t a) : c(c_), alpha(a), overdrive(od) {}
  constexpr RGBA_um(const OverDriveColor& o) : c(o.c), alpha(32768), overdrive(o.overdrive) {}
  static RGBA_um Transparent() { return RGBA_um(Color16(), false, 0); }
  Color16 c;
  uint16_t alpha;
  bool overdrive;

  void printTo(Print& p) {
    if (overdrive) p.write('!');
    c.printTo(p);
    p.write(',');
    p.print(alpha);
  }
};

// Premultiplied ALPHA
struct RGBA {
  constexpr RGBA(Color16 c_, bool od, uint16_t a) : c(c_), alpha(a), overdrive(od) {}
  RGBA(const RGBA_um& rgba) : c(rgba.c * rgba.alpha >> 15), alpha(rgba.alpha), overdrive(rgba.overdrive)  {}
  RGBA(const OverDriveColor& o) : c(o.c), alpha(32768), overdrive(o.overdrive) {}
  Color16 c;
  uint16_t alpha;
  bool overdrive;
  void printTo(Print& p) {
    if (overdrive) p.write('!');
    c.printTo(p);
    p.write('*');
    p.print(alpha);
  }
};

inline RGBA_um operator*(const OverDriveColor& a, uint16_t x) {
  return RGBA_um(a.c, a.overdrive, x);
}

inline RGBA_um operator*(const RGBA_um& a, uint16_t x) {
  return RGBA_um(a.c, a.overdrive, a.alpha * x >> 15);
}

inline RGBA operator*(const RGBA& a, uint16_t x) {
  return RGBA(a.c * x >> 15, a.overdrive, (a.alpha * x + 0x7fff) >> 15);
}

inline OverDriveColor MixColors(OverDriveColor a, OverDriveColor b, int x, int shift) {
  return OverDriveColor( (a.c * ((1 << shift) - x) + b.c * x) >> shift,
			 x > (1 << (shift - 1)) ? b.overdrive : a.overdrive);
}
inline RGBA MixColors(RGBA a, RGBA b, int x, int shift) {
  return RGBA( (a.c * ((1 << shift) - x) + b.c * x) >> shift,
	       x > (1 << (shift - 1)) ? b.overdrive : a.overdrive,
	       (a.alpha * ((1 << shift) - x) + b.alpha * x + ((1<<shift) - 1)) >> shift);
}

// Paint over operators
// There is probably a better way to do this.
inline OverDriveColor operator<<(const OverDriveColor& base, const RGBA_um& over) {
  SCOPED_PROFILER();
  if (!over.alpha) return base;
  uint16_t ac = 32768 - over.alpha;
  return OverDriveColor((base.c * ac + over.c * over.alpha) >> 15,
			over.alpha >= 16384 ? over.overdrive : base.overdrive);
}
inline OverDriveColor operator<<(const OverDriveColor& base, const RGBA& over) {
  SCOPED_PROFILER();
  if (!over.alpha) return base;
  uint16_t ac = 32768 - over.alpha;
  return OverDriveColor((base.c * ac >> 15) + over.c,
			over.alpha >= 16384 ? over.overdrive : base.overdrive);
}

inline RGBA operator<<(const RGBA_um& base, const RGBA_um& over) {
  SCOPED_PROFILER();
//  if (!over.alpha) return base;
  uint16_t ac = 32768 - over.alpha;
  return RGBA((base.c * (base.alpha * ac >> 15) + over.c * over.alpha) >> 15,
	      over.alpha > 16384 ? over.overdrive : base.overdrive,
	      (base.alpha * ac >> 15) + over.alpha);
}

inline RGBA operator<<(const RGBA_um& base, const RGBA& over) {
  SCOPED_PROFILER();
//  if (!over.alpha) return base;
  uint16_t ac = 32768 - over.alpha;
  return RGBA((base.c * (base.alpha * ac >> 15) >> 15) + over.c,
	      over.alpha > 16384 ? over.overdrive : base.overdrive,
	      (base.alpha * ac >> 15) + over.alpha);
}

inline RGBA operator<<(const RGBA& base, const RGBA_um& over) {
  SCOPED_PROFILER();
//  if (!over.alpha) return base;
  uint16_t ac = 32768 - over.alpha;
  return RGBA((base.c * ac + over.c * over.alpha) >> 15,
	      over.alpha >= 16384 ? over.overdrive : base.overdrive,
	      ((base.alpha * ac + 0x7fff) >> 15) + over.alpha);
}

inline RGBA operator<<(const RGBA& base, const RGBA& over) {
    SCOPED_PROFILER();
//  if (!over.alpha) return base;
  uint16_t ac = 32768 - over.alpha;
  return RGBA((base.c * ac >> 15) + over.c,
	      over.alpha >= 16384 ? over.overdrive : base.overdrive,
	      ((base.alpha * ac + 0x7fff) >> 15) + over.alpha);
}

#endif
