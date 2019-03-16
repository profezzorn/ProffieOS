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
  uint8_t select(const Color8& other) const {
    uint8_t ret = 255;
    if (other.r) ret = std::min<int>(ret, r * 255 / other.r);
    if (other.g) ret = std::min<int>(ret, g * 255 / other.g);
    if (other.b) ret = std::min<int>(ret, b * 255 / other.b);
    return ret;
  }

  enum Byteorder {
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
  Color16() : r(0), g(0), b(0) {}
  Color16(const Color8& c) : r(c.r * 0x101), g(c.g * 0x101), b(c.b * 0x101) {}
  Color16(uint16_t r_, uint16_t g_, uint16_t b_) : r(r_), g(g_), b(b_) {}
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

  uint16_t r, g, b;
};

struct OverDriveColor {
  Color16 c;
  bool overdrive;
};

#endif
