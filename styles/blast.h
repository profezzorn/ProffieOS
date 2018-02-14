#ifndef STYLES_BLAST_H
#define STYLES_BLAST_H

// Usage: Blaset<BASE, BLAST>
// BASE, BLAST: COLOR
// return value: COLOR
// Normally shows BASE, but creates a blast effect using
// the color BLAST when a blast is requested.

class BladeBase;

template<class BASE, class BLAST>
class Blast {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    blast_.run(blade);
    num_leds_ = blade->num_leds();
    num_blasts_ = SaberBase::NumBlasts();
  }
  OverDriveColor getColor(int led) {
    OverDriveColor base = base_.getColor(led);
    if (num_blasts_ == 0) return base;
    float mix = 0.0;
    for (size_t i = 0; i < num_blasts_; i++) {
      // TODO(hubbe): Use sin_table and avoid floats
      const SaberBase::Blast b = SaberBase::getBlast(i);
      float x = (b.location - led/(float)num_leds_) * 30.0;
      uint32_t T = micros() - b.start_micros;
      float t = 0.5 + T / 200000.0;
      if (x == 0.0) {
        mix += 1.0f / (t*t);
      } else {
        mix += sinf(x / (t*t)) / x;
      }
    }
    OverDriveColor blast = blast_.getColor(led);
    base.c = base.c.mix(blast.c, min(mix, 1.0) * 256);
    return base;
  }
private:
  BASE base_;
  BLAST blast_;
  int num_leds_;
  size_t num_blasts_;
};

#endif
