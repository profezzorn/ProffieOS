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
private:
  A a_;
  B b_;
  F f_;
public:
  auto getColor(int led) -> decltype(MixColors(a_.getColor(led), b_.getColor(led), f_.getInteger(led), 15)) {
    return MixColors(a_.getColor(led), b_.getColor(led), f_.getInteger(led), 15);
  }
};

template<class... A> class MixHelper {};

template<class A>
class MixHelper<A> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
  }
private:
  A a_;
public:
  auto getColor(int x, int led) -> decltype(a_.getColor(led)) {
    return a_.getColor(led);
  }
};
  
template<class A, class... B>
class MixHelper<A, B...> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
private:
  A a_;
  MixHelper<B...> b_;
public:  
  auto getColor(int x, int led) -> decltype(MixColors(a_.getColor(led), b_.getColor(led), 1, 1)) {
    if (x == 0) return a_.getColor(led);
    return b_.getColor(x - 1, led);
  }
};


template<class F, class A, class B, class... COLORS> 
class Mix<F, A, B, COLORS...> {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    f_.run(blade);
  }
private:
  MixHelper<A, B, COLORS...> colors_;
  F f_;
public:  
  auto getColor(int led) -> decltype(colors_.getColor(1,1)) {
    int x = f_.getInteger(led);
    x *= (sizeof...(COLORS) - 1);
    auto a = colors_.getColor(x >> 15, led);
    auto b = colors_.getColor((x >> 15) + 1, led);
    return MixColors(a, b, x & 0x7fff, 15);
  }
};



#endif
