#ifndef STYLES_MIX_H
#define STYLES_MIX_H

#include "../common/typelist.h"

template<class F, class... B> class Mix {};

// Usage: Mix<F, A, B>
// Mix between A and B using function F
// F: INTEGER
// A, B: COLOR
// return value: COLOR or LAYER (if A or B is a layer)
//
// F = 0 -> return A
// F = 16384 -> return (A+B)/2
// F = 32768 -> return B

// Usage: Mix<F, A, B, C, ....>
// Mix between A and B using function F
// F: INTEGER
// A, B, C: COLOR
// return value: COLOR or LAYER (if A or B is a layer)
//
// F = 0 -> return A
// F = 1 -> mostly A, a little B
// F = 2 -> a little more A, a little less B
// F = 32767 -> mostly last color, a little of the last-but-one color
// F = 32768 -> last color

template<class F, class A, class B>
class Mix<F, A, B> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    f_.run(blade);
  }
private:
  PONUA A a_;
  PONUA B b_;
  PONUA F f_;
public:
  auto getColor(int led) -> decltype(MixColors(a_.getColor(led), b_.getColor(led), f_.getInteger(led), 15)) {
    return MixColors(a_.getColor(led), b_.getColor(led), f_.getInteger(led), 15);
  }
};

template<class A> class MixHelper2 {};

template<class A>
class MixHelper2<TypeList<A>> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
  }
private:
  PONUA A a_;
public:
  static int size() { return 1; }
  auto getColor(int x, int led) -> decltype(a_.getColor(led)) {
    return a_.getColor(led);
  }
};
  
template<class A, class... B>
class MixHelper2<TypeList<A, B...>> {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
private:
  PONUA MixHelper2<typename SplitTypeList<TypeList<A, B...>>::first_half> a_;
  PONUA MixHelper2<typename SplitTypeList<TypeList<A, B...>>::second_half> b_;
public:  
  static int size() { return 1 + sizeof...(B); }
  auto getColor(int x, int led) -> decltype(MixColors(a_.getColor(0, led), b_.getColor(0, led), 1, 1)) {
    if (x < a_.size()) return a_.getColor(x, led);
    return b_.getColor(x - a_.size(), led);
  }
};

template<class... COLORS> using MixHelper = MixHelper2<TypeList<COLORS...>>;

template<class F, class A, class B, class... COLORS> 
class Mix<F, A, B, COLORS...> {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    f_.run(blade);
  }
private:
  PONUA MixHelper<A, B, COLORS...> colors_;
  PONUA F f_;
public:  
  auto getColor(int led) -> decltype(colors_.getColor(1,1)) {
    int x = f_.getInteger(led);
    x *= (sizeof...(COLORS) + 1);
    auto a = colors_.getColor(x >> 15, led);
    auto b = colors_.getColor((x >> 15) + 1, led);
    return MixColors(a, b, x & 0x7fff, 15);
  }
};



#endif
