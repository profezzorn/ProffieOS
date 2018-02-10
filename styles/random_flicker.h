#ifndef STYLES_RANDOM_FLICKER_H
#define STYLES_RANDOM_FLICKER_H

// Mixes randomly between A and B.
// mix is even over entire blade.
template<class A, class B>
class RandomFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    mix_ = random(255);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, mix_);
    return a;
  }
private:
  A a_;
  B b_;
  int mix_;
};

#endif
