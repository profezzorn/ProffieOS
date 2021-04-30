#ifndef TRANSITIONS_WAVE_H
#define TRANSITIONS_WAVE_H

// TrWave is implements a wave traveling out from a specified point.
// It's based on the Blast effect and is meant to look like a ripple starting
// at a point on the blade. Unlike other transitions, this effect starts and ends
// at the same color, and the wave is drawn using COLOR instead of the start/end
// colors like most transitions to. It's intended to be used with TransitionLoopL
// or TransitionEffectL, which takes transitions that start and begin with the same
// color.

template<class COLOR,
         class FADEOUT_MS = Int<200>,
         class WAVE_SIZE = Int<100>,
         class WAVE_MS = Int<400>,
         class WAVE_CENTER = Int<16384>>
class TrWaveX : public TransitionBaseX<FADEOUT_MS> {
public:
  void run(BladeBase *blade) {
    wave_size_.run(blade);
    wave_center_.run(blade);
    wave_ms_.run(blade);
    color_.run(blade);
    if (this->restart()) {
      center_ = wave_center_.getInteger(0);
      size_ = wave_size_.getInteger(0);
    }
    TransitionBaseX<FADEOUT_MS>::run(blade);
    mix_ = 32768 - this->update(32768);
    num_leds_ = blade->num_leds();
    offset_ = (millis() - this->start_millis()) * 32768 / wave_ms_.getInteger(0);
  }
private:
  WAVE_CENTER wave_center_;
  int center_;
  
  WAVE_SIZE wave_size_;
  int size_;

  int mix_;
  int num_leds_;
  int offset_;
  WAVE_MS wave_ms_;
  COLOR color_;
public:
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) -> decltype(MixColors(a,color_.getColor(led),1,1)) {
    int dist = std::abs(center_ - led * 32768 / num_leds_);
    int N = std::abs(dist - offset_) * size_ >> 15;
    int mix;
    if (N <= 32) {
      mix = blast_hump[N] * mix_ >> 8;
    } else {
      mix = 0;
    }
    return MixColors(a, color_.getColor(led), mix, 15);
  }
};

// TrSparkX generates a wave without Fade over the length of the blade from 
// SPARK_CENTER. Unlike other transitions, this effect starts and ends
// at the same color, and the wave is drawn using COLOR instead of the start/end
// colors like most transitions to. It's intended to be used with TransitionLoopL
// or TransitionEffectL, which takes transitions that start and begin with the same
// color.

template<class COLOR,
         class SPARK_SIZE = Int<100>,
         class SPARK_MS = Int<400>,
         class SPARK_CENTER = Int<16384>>
class TrSparkX : public TransitionBaseX<SPARK_MS> {
public:

  void run(BladeBase *blade) {
    spark_size_.run(blade);
    spark_center_.run(blade);
    spark_ms_.run(blade);
    color_.run(blade);
    if (this->restart()) {
      center_ = spark_center_.getInteger(0);
      size_ = spark_size_.getInteger(0);
    }
    TransitionBaseX<SPARK_MS>::run(blade);
    mix_ = this->update(32768);
    num_leds_ = blade->num_leds();
    offset_ = (millis() - this->start_millis()) * 32768 / spark_ms_.getInteger(0);
  }
private:
  SPARK_CENTER spark_center_;
  int center_;
  
  SPARK_SIZE spark_size_;
  int size_;

  int mix_;
  int num_leds_;
  int offset_;
  SPARK_MS spark_ms_;
  COLOR color_;
public:
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) -> decltype(MixColors(a,color_.getColor(led),1,1)) {
    int dist = std::abs(center_ - led * 32768 / num_leds_);
    int N = std::abs(dist - offset_) * size_ >> 15;
    int mix;
    if (N <= 32) {
      mix = blast_hump[N] * mix_ >> 8;
    } else {
      mix = 0;
    }
    return MixColors(a, color_.getColor(led), mix, 15);
  }
};
#endif
