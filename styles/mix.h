#ifndef STYLES_MIX_H
#define STYLES_MIX_H

template<class F, class... B> class Mix {};

// Usage: Mix<A, B, F>
// Mix between A and B using function F
// F: INTEGER
// A, B: COLOR
// return value: COLOR
//
// F = 0 -> return A
// F = 16384 -> return (A+B)/2
// F = 32768 -> return B
template<class F, class A, class B>
class Mix<F, A, B> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    f_.run(blade);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix3(b.c, f_.getInteger(led));
    return a;
  }
private:
  A a_;
  B b_;
  F f_;
};

template<class... A> class MixHelper {};

template<class A>
class MixHelper<A> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
  }
  OverDriveColor getColor(int x, int led) {
    return a_.getColor(led);
  }
private:
  A a_;
};
  
template<class A, class... B>
class MixHelper<A, B...> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  OverDriveColor getColor(int x, int led) {
    if (x == 0) return a_.getColor(led);
    return b_.getColor(x - 1, led);
  }
private:
  A a_;
  MixHelper<B...> b_;
};


template<class F, class A, class B, class... COLORS> 
class Mix<F, A, B, COLORS...> {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    f_.run(blade);
  }
  OverDriveColor getColor(int led) {
    int x = f_.getInteger(led);
    x *= (sizeof...(COLORS) - 1);
    OverDriveColor a = colors_.getColor(x >> 15, led);
    OverDriveColor b = colors_.getColor((x >> 15) + 1, led);
    a.c = a.c.mix3(b.c, x & 0x7fff);
    return a;
  }
private:
  A a_;
  MixHelper<A, B, COLORS...> colors_;
  F f_;
};



#endif
