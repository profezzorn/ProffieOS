#ifndef STYLES_STRIPES_H
#define STYLES_STRIPES_H

#include "../functions/int.h"

// Usage: Stripes<WIDTH, SPEED, COLOR1, COLOR2, ... >
// or: Usage: StripesX<WIDTH_CLASS, SPEED, COLOR1, COLOR2, ... >
// WIDTH: integer (start with 1000, then adjust up or down)
// WIDTH_CLASS: INTEGER
// SPEED: integer  (start with 1000, then adjust up or down)
// COLOR1, COLOR2: COLOR
// return value: COLOR
// Works like rainbow, but with any colors you like.
// WIDTH determines width of stripes
// SPEED determines movement speed

// If you have a ring of LEDs and you want the stripes to line up,
// you'll need to set WIDTH using the following formula:
// WIDTH = 50000 * NUM_LEDS_IN_RING / (NUM_COLORS * REPETITIONS * 341)

template<class... A>
class StripesHelper {};
  
template<>
class StripesHelper<> {
public:
  static const size_t size = 0;
  void run(BladeBase* blade) {}
  void get(int led, int p, SimpleColor* c) {}
};

template<class A, class... B>
class StripesHelper<A, B...> {
public:
  static const size_t size = StripesHelper<B...>::size + 1;
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  void get(int led, int p, SimpleColor* ret) {
    if (p > 0 && p < 512) {
      SimpleColor tmp = a_.getColor(led);
      int mul = sin_table[p];
      ret->c.r = clampi32(ret->c.r + ((tmp.c.r * mul) >> 14), 0, 65535);
      ret->c.g = clampi32(ret->c.g + ((tmp.c.g * mul) >> 14), 0, 65535);
      ret->c.b = clampi32(ret->c.b + ((tmp.c.b * mul) >> 14), 0, 65535);
    }
    b_.get(led, p - 341, ret);
  }
  A a_;
  StripesHelper<B...> b_;
};

// TODO: Move this somewhere common.
static inline int32_t MOD(int32_t x, int32_t m) {
  if (x > 0) return x % m;
  return m + ~(~x % m);
}

template<class... COLORS>
class StripesBase {
public:
  void run(BladeBase* base, int width, int speed) {
    colors_.run(base);
    
    uint32_t now_micros = micros();
    int32_t delta_micros = now_micros - last_micros_;
    last_micros_ = now_micros;

    m = MOD(m + delta_micros * speed / 333, colors_.size * 341*1024);
    mult_ = (50000*1024 / width);
  }
  SimpleColor getColor(int led) {
    // p = 0..341*len(colors)
    int p = ((m + led * mult_) >> 10) % (colors_.size * 341);
    
    SimpleColor ret;
    ret.c = Color16(0,0,0);
    colors_.get(led, p, &ret);
    colors_.get(led, p + 341 * colors_.size, &ret);
    return ret;
  }
private:
  StripesHelper<COLORS...> colors_;
  uint32_t mult_;
  uint32_t last_micros_;
  int32_t m;
};

template<class WIDTH, class SPEED, class... COLORS>
class StripesX : public StripesBase<COLORS...> {
public:
  void run(BladeBase* base) {
    width_.run(base);
    speed_.run(base);
    StripesBase<COLORS...>::run(base, width_.getInteger(0), speed_.getInteger(0));
  }
private:
  WIDTH width_;
  SPEED speed_;
};

template<int WIDTH, int SPEED, class... COLORS>
  using Stripes = StripesX<Int<WIDTH>, Int<SPEED>, COLORS...>;
#endif
