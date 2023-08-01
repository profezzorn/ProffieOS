#ifndef FUNCTIONS_BUMP_H
#define FUNCTIONS_BUMP_H

// Usage: Bump<BUMP_POSITION, BUMP_WIDTH_FRACTION>
// Returns different values for each LED, forming a bump shape.
// If BUMP_POSITION is 0, bump will be at the hilt.
// If BUMP_POSITION is 32768, the bump will be at the tip.
// If BUMP_WIDTH_FRACTION is 1, bump will be extremely narrow.
// If BUMP_WIDTH_FRACTION is 32768, it will fill up most/all of the blade.
// BUMP_POSITION, BUMP_WIDTH_FRACTION: INTEGER

// Based on exp(-x*x)
static uint8_t bump_shape[33] = {
  255,255,252,247,240,232,222,211,
  199,186,173,159,145,132,119,106,
  94,82,72,62,53,45,38,32,
  26,22,18,14,11,9,7,5,0
};

template<class BUMP_POSITION, class BUMP_WIDTH_FRACTION = Int<16385> >
class Bump {
public:
  void run(BladeBase* blade) {
    pos_.run(blade);
    fraction_.run(blade);
    int fraction = fraction_.calculate(blade);
    if (fraction == 0) {
      mult_ = 1;
      location_ = -10000;
      return;
    }
    float mult = 32 * 2.0 * 128 * 32768 / fraction / blade->num_leds();
    mult_ = mult;
    location_ = (pos_.calculate(blade) * blade->num_leds() * mult) / 32768;
  }
  int getInteger(int led) {
    uint32_t dist = abs(led * mult_ - location_);
    uint32_t p = dist >> 7;
    if (p >= NELEM(bump_shape) - 1) return 0;
    int m = dist & 0x3f;
    return bump_shape[p] * (128 - m) + bump_shape[p+1] * m;
  }
private:
  PONUA SVFWrapper<BUMP_POSITION> pos_;
  PONUA SVFWrapper<BUMP_WIDTH_FRACTION> fraction_;
  int location_;
  int mult_;
};

// Usage: HumpFlickerFX<FUNCTION>
// or: HumpFlickerF<N>
// FUNCTION: FUNCTION
// N: NUMBER
// return value: INTEGER
// Creates hump shapes that randomize over the blade.
// The returned INTEGER is the size of the humps.
// Large values can give the blade a shimmering look, 
// while small values look more like speckles.

template<class HUMP_WIDTH>
class HumpFlickerFX {
public:
  void run(BladeBase* blade) {
    hump_width_.run(blade);
    int num_leds = blade->num_leds();
    pos_ = random(num_leds);
  }
  int getInteger(int led) {
    return clampi32(abs(led - pos_) * 32768 / hump_width_.getInteger(led), 0, 32768);
  }
private:
  PONUA HUMP_WIDTH hump_width_;
  int pos_;
};

template<int HUMP_WIDTH> using HumpFlickerF = HumpFlickerFX<Int<HUMP_WIDTH>>;

#endif
