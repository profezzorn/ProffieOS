#ifndef FUNCTIONS_SPARKLE_H
#define FUNCXTIONS_SPARKLE_H

// Usage: SparkleF<SPARK_CHANCE_PROMILLE, SPARK_INTENSITY>
// SPARK_CHANCE_PROMILLE: a number
// SPARK_INTENSITY: a number

// SPARK_CHANCE_PROMILLE decides how often a spark is generated, defaults to 300 (30%)
// SPARK_INTENSITY specifies how intens the spark is, defaults to 1024

template<int SPARK_CHANCE_PROMILLE = 300, int SPARK_INTENSITY = 1024>
class SparkleF {
public:
  ~SparkleF() {
    delete[] sparks_; 
  }

  void run(BladeBase* blade) {
    uint32_t m = millis();
    if (!sparks_) {
      size_t N = blade->num_leds() + 4;
      sparks_ = new short[N];
      for (size_t i = 0; i < N; i++) sparks_[i] = 0;
    }
    if (m - last_update_ >= 10) {
      last_update_ = m;
      uint16_t fifo[2];
      fifo[0] = 0;
      fifo[1] = 0;
      int N = blade->num_leds();
      for (int i = 2; i < N + 2; i++) {
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

  int getInteger(int led) { return clampi32(sparks_[led + 2], 0, 256) << 7; }

private:  
  short* sparks_ = 0;
  uint32_t last_update_;
};

#endif
