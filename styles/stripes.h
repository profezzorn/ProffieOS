#ifndef STYLES_STRIPES_H
#define STYLES_STRIPES_H

#include "../functions/int.h"
#include "../common/range.h"

// Usage: Stripes<WIDTH, SPEED, COLOR1, COLOR2, ... >
// or: StripesX<WIDTH_CLASS, SPEED, COLOR1, COLOR2, ... >
// WIDTH: integer (start with 1000, then adjust up or down)
// WIDTH_CLASS: INTEGER
// SPEED: integer  (start with 1000, then adjust up or down)
// COLOR1, COLOR2: COLOR
// return value: COLOR
// Works like Rainbow, but with any colors you like.
// WIDTH determines width of stripes
// SPEED determines movement speed
//
// If you have a ring of LEDs and you want the stripes to line up,
// you'll need to set WIDTH using the following formula:
// WIDTH = 50000 * NUM_LEDS_IN_RING / (NUM_COLORS * REPETITIONS * 341)

// Usage: HardStripes<WIDTH, SPEED, COLOR1, COLOR2, ... >
// or: HardStripesX<WIDTH_CLASS, SPEED, COLOR1, COLOR2, ... >
// WIDTH: integer (start with 1000, then adjust up or down)
// WIDTH_CLASS: INTEGER
// SPEED: integer  (start with 1000, then adjust up or down)
// COLOR1, COLOR2: COLOR
// return value: COLOR
// Works like Stripes, but with no gradient between color segments..
// * Note * Regular Stripes is recommended for very slow speeds.
// Without a 1 pixel gradient smoothing the changing pixel color,
// the animation can seem a little "choppy". At faster speeds, this is not apparent.


template<class... A>
class StripesHelper {};

template<>
class StripesHelper<> {
public:
  static const size_t size = 0;
  void run(BladeBase* blade) {}
  void get(int led, int p, SimpleColor* c) {}
  void hardGet(const Range& pixel, int led, int p, uint32_t mod, SimpleColor* c) {}
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
      OverDriveColor tmp = a_.getColor(led);
      int mul = sin_table[p];
      ret->c.r = clampi32(ret->c.r + ((tmp.c.r * mul) >> 14), 0, 65535);
      ret->c.g = clampi32(ret->c.g + ((tmp.c.g * mul) >> 14), 0, 65535);
      ret->c.b = clampi32(ret->c.b + ((tmp.c.b * mul) >> 14), 0, 65535);
    }
    b_.get(led, p - 341, ret);
  }

  void hardGet(const Range& pixel, int led, int p, uint32_t mod, SimpleColor* ret) {
    int segment_size = 341;
    uint32_t weight = pixel.intersect_with_stripes(Range(p, p + segment_size), mod);
    if (weight) {
      OverDriveColor tmp = a_.getColor(led);
      if (weight == pixel.size()) {
        ret->c = tmp.c;
        return;
      } else {
        weight = weight * 16384 / pixel.size();
        ret->c.r = clampi32(ret->c.r + ((tmp.c.r * weight) >> 14), 0, 65535);
        ret->c.g = clampi32(ret->c.g + ((tmp.c.g * weight) >> 14), 0, 65535);
        ret->c.b = clampi32(ret->c.b + ((tmp.c.b * weight) >> 14), 0, 65535);
      }
    }
    b_.hardGet(pixel, led, p + segment_size, mod, ret);
  }

  A a_;
  StripesHelper<B...> b_;
};

template<class... COLORS>
class StripesBase {
public:
  void run(BladeBase* base, int width, int speed) {
    colors_.run(base);

    uint32_t now_micros = micros();
    int32_t delta_micros = now_micros - last_micros_;
    last_micros_ = now_micros;

    m = MOD(m + delta_micros * speed / 333, colors_.size * 341 * 1024);
    mult_ = (50000 * 1024 / width);
  }
  SimpleColor getColor(int led) {
    // p = 0..341*len(colors)
    int p = ((m + led * mult_) >> 10) % (colors_.size * 341);

    SimpleColor ret;
    ret.c = Color16(0, 0, 0);
    colors_.get(led, p, &ret);
    colors_.get(led, p + 341 * colors_.size, &ret);
    return ret;
  }
protected:
  StripesHelper<COLORS...> colors_;
  uint32_t mult_;
  uint32_t last_micros_;
  int32_t m;
};

template<class... COLORS>
class HardStripesBase : public StripesBase<COLORS...> {
public:
  SimpleColor getColor(int led) {
    // p = 0..341*len(colors)
    int p = ((this->m + led * this->mult_) >> 10) % (this->colors_.size * 341);

    SimpleColor ret;
    ret.c = Color16(0, 0, 0);
    this->colors_.hardGet(Range(0, this->mult_ >> 10), led, p, this->colors_.size * 341, &ret);
    return ret;
  }
};

template<class WIDTH, class SPEED, class... COLORS>
class StripesX : public StripesBase<COLORS...> {
public:
  void run(BladeBase* base) {
    // Width cannot be zero.
    static_assert(!is_same_type<WIDTH, Int<0>>::value, "Stripes cannot have a zero WIDTH");
    width_.run(base);
    speed_.run(base);
    StripesBase<COLORS...>::run(base, width_.calculate(base), speed_.calculate(base));
  }
private:
  PONUA SVFWrapper<WIDTH> width_;
  PONUA SVFWrapper<SPEED> speed_;
};

template<class WIDTH, class SPEED, class... COLORS>
class HardStripesX : public HardStripesBase<COLORS...> {
public:
  void run(BladeBase* base) {
    // Width cannot be zero.
    static_assert(!is_same_type<WIDTH, Int<0>>::value, "HardStripes cannot have a zero WIDTH");
    this->width_.run(base);
    this->speed_.run(base);
    HardStripesBase<COLORS...>::run(base, this->width_.calculate(base), this->speed_.calculate(base));
  }
private:
  PONUA SVFWrapper<WIDTH> width_;
  PONUA SVFWrapper<SPEED> speed_;
};

template<int WIDTH, int SPEED, class... COLORS>
  using Stripes = StripesX<Int<WIDTH>, Int<SPEED>, COLORS...>;

template<int WIDTH, int SPEED, class... COLORS>
  using HardStripes = HardStripesX<Int<WIDTH>, Int<SPEED>, COLORS...>;

#endif
