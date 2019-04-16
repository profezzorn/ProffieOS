#ifndef STYLES_BLAST_H
#define STYLES_BLAST_H

// Usage: Blast<BASE, BLAST, FADEOUT_MS, WAVE_SIZE, WAVE_MS>
// BASE, BLAST: COLOR
// FADEOUT_MS: a number (defaults to 150)
// WAVE_SIZE: a number (defaults to 100)
// WAVE_MS: a number (defaults to 400)
// return value: COLOR
// Normally shows BASE, but creates a blast effect using
// the color BLAST when a blast is requested. The effect
// is basically two humps moving out from the blast location.
// The size of the humps can be changed with WAVE_SIZE, note
// that smaller values makes the humps bigger. WAVE_MS determines
// how fast the waves travel. Smaller values makes the waves
// travel slower. Finally FADEOUT_MS determines how fast the
// humps fade back to the base color.

class BladeBase;

// Based on exp(-x*x)
static uint8_t blast_hump[32] = {
  255,255,252,247,240,232,222,211,
  199,186,173,159,145,132,119,106,
  94,82,72,62,53,45,38,32,
  26,22,18,14,11,9,7,5
};

template<class BASE,
  class BLAST,
  int FADEOUT_MS = 200,
  int WAVE_SIZE=100,
  int WAVE_MS=400,
  BladeEffectType EFFECT = EFFECT_BLAST>
class Blast {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    blast_.run(blade);
    num_leds_ = blade->num_leds();
    num_blasts_ = blade->GetEffects(&effects_);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor base = base_.getColor(led);
    if (num_blasts_ == 0) return base;
    int mix = 0;
    for (size_t i = 0; i < num_blasts_; i++) {
      const BladeEffect& b = effects_[i];
      if (!(b.type & EFFECT)) continue;
      uint32_t T = micros() - b.start_micros;
      int M = 1000 - T/FADEOUT_MS;
      if (M > 0) {
	// TODO: Get rid of float math.
	float dist = fabsf(b.location - led/(float)num_leds_);
	int N = fabsf(dist - T / (WAVE_MS * 1000.0)) * WAVE_SIZE;
	if (N <= 32) {
	  mix += blast_hump[N] * M / 1000;
	}
      }
    }
    OverDriveColor blast = blast_.getColor(led);
    base.c = base.c.mix(blast.c, std::min(mix, 256));
    return base;
  }
private:
  BASE base_;
  BLAST blast_;
  int num_leds_;
  size_t num_blasts_;
  BladeEffect* effects_;
};

// Usage: BlastFadeout<BASE, BLAST, FADEOUT_MS>
// BASE, BLAST: COLOR
// FADEOUT_MS: a number (defaults to 250)
// return value: COLOR
// Normally shows BASE, but swiches to BLAST when a blast
// is requested and then fades back to BASE. FADEOUT_MS
// specifies out many milliseconds the fade takes.
template<class BASE, class BLAST, int FADEOUT_MS = 250, BladeEffectType EFFECT = EFFECT_BLAST>
class BlastFadeout {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    blast_.run(blade);
    num_leds_ = blade->num_leds();
    num_blasts_ = blade->GetEffects(&effects_);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor base = base_.getColor(led);
    if (num_blasts_ == 0) return base;
    int mix = 0;
    for (size_t i = 0; i < num_blasts_; i++) {
      const BladeEffect& b = effects_[i];
      if (!(b.type & EFFECT)) continue;
      uint32_t T = micros() - b.start_micros;
      int M = 1000 - T/FADEOUT_MS;
      if (M > 0) {
	mix += 256 * M / 1000;
      }
    }
    OverDriveColor blast = blast_.getColor(led);
    base.c = base.c.mix(blast.c, std::min(mix, 256));
    return base;
  }
private:
  BASE base_;
  BLAST blast_;
  int num_leds_;
  size_t num_blasts_;
  BladeEffect* effects_;
};


// Usage: OriginalBlast<BASE, BLAST>
// BASE, BLAST: COLOR
// return value: COLOR
// Normally shows BASE, but creates a blast effect using
// the color BLAST when a blast is requested.
// This was the original blast effect, but it is slow and not
// very configurable.
template<class BASE, class BLAST, BladeEffectType EFFECT=EFFECT_BLAST>
class OriginalBlast {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    blast_.run(blade);
    num_leds_ = blade->num_leds();
    num_blasts_ = blade->GetEffects(&effects_);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor base = base_.getColor(led);
    if (num_blasts_ == 0) return base;
    float mix = 0.0;
    for (size_t i = 0; i < num_blasts_; i++) {
      // TODO(hubbe): Use sin_table and avoid floats
      const BladeEffect& b = effects_[i];
      if (!(b.type & EFFECT)) continue;
      float x = (b.location - led/(float)num_leds_) * 30.0;
      uint32_t T = micros() - b.start_micros;
      float t = 0.5 + T / 200000.0;
      if (x == 0.0) {
        mix += 2.0f / (t*t);
      } else {
        mix += std::max(0.0, 2.0 * sinf(x / (t*t)) / x);
      }
    }
    OverDriveColor blast = blast_.getColor(led);
    base.c = base.c.mix(blast.c, std::min(mix, 1.0) * 256);
    return base;
  }
private:
  BASE base_;
  BLAST blast_;
  int num_leds_;
  size_t num_blasts_;
  BladeEffect* effects_;
};


#endif
