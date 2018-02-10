#ifndef STYLES_BROWN_NOISE_FLICKER_H
#define STYLES_BROWN_NOISE_FLICKER_H

// Randomly selects between A and B, but keeps nearby
// pixels looking similar.
template<class A, class B, int grade>
class BrownNoiseFlicker {
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
    mix_ = clampi32(mix_ + random(grade * 2 + 1) - grade, 0, 255);
    return a;
  }
private:
  A a_;
  B b_;
  int mix_;
};

#endif
