#ifndef FUNCTIONS_BLAST_H
#define FUNCTIONS_BLAST_H

// Based on exp(-x*x)
static uint8_t blast_hump[32] = {
  255,255,252,247,240,232,222,211,
  199,186,173,159,145,132,119,106,
  94,82,72,62,53,45,38,32,
  26,22,18,14,11,9,7,5
};

template<int FADEOUT_MS = 200,
  int WAVE_SIZE=100,
  int WAVE_MS=400,
  BladeEffectType EFFECT = EFFECT_BLAST>
class BlastF {
public:
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
    num_blasts_ = blade->GetEffects(&effects_);
  }

  int getInteger(int led) {
    int mix = 0;
    for (size_t i = 0; i < num_blasts_; i++) {
      const BladeEffect& b = effects_[i];
      if (!(b.type == EFFECT)) continue;
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
    return std::min(mix << 7, 32768);
  }

private:
  int num_leds_;
  size_t num_blasts_;
  BladeEffect* effects_;
};


template<int FADEOUT_MS = 250, BladeEffectType EFFECT = EFFECT_BLAST>
class BlastFadeoutF {
public:
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
    num_blasts_ = blade->GetEffects(&effects_);
  }
  int getColor(int led) {
    if (num_blasts_ == 0) return 0;
    int mix = 0;
    for (size_t i = 0; i < num_blasts_; i++) {
      const BladeEffect& b = effects_[i];
      if (b.type != EFFECT) continue;
      uint32_t T = micros() - b.start_micros;
      int M = 1000 - T/FADEOUT_MS;
      if (M > 0) {
	mix += 32768 * M / 1000;
      }
    }
    return std::min(mix, 32768);
  }

private:
  int num_leds_;
  size_t num_blasts_;
  BladeEffect* effects_;
};

template<BladeEffectType EFFECT=EFFECT_BLAST>
class OriginalBlastF {
public:
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
    num_blasts_ = blade->GetEffects(&effects_);
  }
  int getInteger(int led) {
    if (num_blasts_ == 0) return 0;
    float mix = 0.0;
    for (size_t i = 0; i < num_blasts_; i++) {
      // TODO(hubbe): Use sin_table and avoid floats
      const BladeEffect& b = effects_[i];
      if (b.type != EFFECT) continue;
      float x = (b.location - led/(float)num_leds_) * 30.0;
      uint32_t T = micros() - b.start_micros;
      float t = 0.5 + T / 200000.0;
      if (x == 0.0) {
        mix += 2.0f / (t*t);
      } else {
        mix += std::max(0.0, 2.0 * sinf(x / (t*t)) / x);
      }
    }
    return std::min(mix, 1.0) * 32768;
  }
private:
  int num_leds_;
  size_t num_blasts_;
  BladeEffect* effects_;
};

#endif
