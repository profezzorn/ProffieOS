#ifndef STYLES_SPARKLE_H
#define STYLES_SPARKLE_H

// Usage: Sparkle<BASE, SPARKLE_COLOR, SPARK_CHANCE_PROMILLE, SPARK_INTENSITY>
// BASE: COLOR
// CLASH_COLOR: COLOR (defaults to white)
// SPARK_CHANCE_PROMILLE: a number
// SPARK_INTENSITY: a number
// Generally displays BASE, but creates little sparkles of SPARKLE_COLOR
// SPARK_CHANCE_PROMILLE decides how often a spark is generated, defaults to 300 (30%)
// SPARK_INTENSITY specifies how intens the spark is, defaults to 1024
template<class BASE,
  class SPARKLE_COLOR = Rgb<255,255,255>,
  int SPARK_CHANCE_PROMILLE = 300,
  int SPARK_INTENSITY = 1024>
class Sparkle {
public:
  Sparkle() {
    for (size_t i = 0; i < NELEM(sparks_); i++) sparks_[i] = 0;
  }

  void run(BladeBase* blade) {
    base_.run(blade);
    sparkle_color_.run(blade);
    uint32_t m = millis();
    if (m - last_update_ >= 10) {
      last_update_ = m;
      uint16_t fifo[2];
      fifo[0] = 0;
      fifo[1] = 0;
      int N = blade->num_leds();
      for (size_t i = 2; i < N + 2; i++) {
#if 0
	int32_t x = (sparks_[i - 1] + sparks_[i + 1]) * 3 - (sparks_[i - 2] + sparks_[i + 2]);
	sparks_[i-2] = fifo[0];
	fifo[0] = fifo[1];
	fifo[1] = (sparks_[i] * 11 + x) >> 4;
#else
	int32_t x = ((sparks_[i-1] + sparks_[i+1]) * 200 + sparks_[i] * 570) >> 10;
	sparks_[i-1] = fifo[0];
	fifo[0] = x;
#endif	
      }
      sparks_[N] = fifo[0];
      sparks_[N+1] = fifo[1];
      if (random(1000) < SPARK_CHANCE_PROMILLE) {
	sparks_[random(blade->num_leds())+2] += SPARK_INTENSITY;
      }
    }
  }

  OverDriveColor getColor(int led) {
    OverDriveColor a = base_.getColor(led);
    OverDriveColor b = sparkle_color_.getColor(led);
    a.c = a.c.mix(b.c, clampi32(sparks_[led + 2], 0, 256));
    return a;
  }

private:  
  BASE base_;
  SPARKLE_COLOR sparkle_color_;
  short sparks_[maxLedsPerStrip + 4];
  uint32_t last_update_;
};

#endif
