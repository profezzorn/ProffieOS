#ifndef STYLES_AUDIO_FLICKER_H
#define STYLES_AUDIO_FLICKER_H

// Usage: AudioFlicker<A, B>
// A, B: COLOR
// return value: COLOR
// Mixes between A and B based on audio. Quiet audio
// means more A, loud audio means more B.
// Based on a single sample instead of an average to make it flicker.

class BladeBase;

template<class A, class B>
class AudioFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
#ifdef ENABLE_AUDIO    
    mix_ = clampi32(dynamic_mixer.last_sum() >> 4, 0, 255);
#else
    mix_ = 0;
#endif
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
