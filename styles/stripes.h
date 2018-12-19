#ifndef STYLES_STRIPES_H
#define STYLES_STRIPES_H

// Usage: Stripes
// return value: COLOR
// Basic RGB stripes.

template<class... A>
class StripesHelper {};
  
template<>
class StripesHelper<> {
public:
  static const size_t size = 0;
  void run(BladeBase* blade) {}
  void get(int p, OverDriveColor& c) {}
};

template<class A, class B, class... C>
class StripesHelper<A, B, C...> {
public:
  static const size_t size = StripesHelper<B, C...>::size + 1;
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  void get(int p, OverDriveColor& ret) {
    b_.get(p + 341, ret);
    if (p > 0 && p < 512) {
      OverDriveColor tmp = a_.getColor();
      int mul = sin_table[p];
      ret.c.r += (tmp.c.r * mul) >> 14;
      ret.c.g += (tmp.c.g * mul) >> 14;
      ret.c.b += (tmp.c.b * mul) >> 14;
    }
  }
  A a_;
  StripesHelper<B, C...> b_;
};


template<int WIDTH=1000, int SPEED = 1000, class... COLORS>
class Stripes {
public:
  void run(BladeBase* base) {
    uint32_t now_micros = micros();
    uint32_t delta_micros = now_micros - last_micros_;
    last_micros_ = now_micros;
    m = (m + delta_micros * SPEED / 333333) % (colors_.size() * 341);
    colors_.run(base);
  }
  OverDriveColor getColor(int led) {
    // p = 0..341*len(colors)
    int p = ((m + led * (50000 / WIDTH)) >> 10) % (colors_.size() * 341);
    
    OverDriveColor ret;
    ret.overdrive = false;
    ret.c = Color16(0,0,0);
    colors_.get(p, &ret);
    colors_.get(p - 341 * colors_.size(), &ret);
    return ret;
  }
private:
  StripesHelper<COLORS...> colors_;
  uint32_t last_micros_;
  uint32_t m;
};

#endif
