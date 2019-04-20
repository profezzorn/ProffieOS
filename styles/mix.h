#ifndef STYLES_MIX_H
#define STYLES_MIX_H

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
class Mix {
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

#endif
