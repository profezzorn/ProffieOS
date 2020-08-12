#ifndef STYLES_SPARKLE_H
#define STYLES_SPARKLE_H

// Usage: Sparkle<BASE, SPARKLE_COLOR, SPARK_CHANCE_PROMILLE, SPARK_INTENSITY>
// Or: SparkleL<SPARKLE_COLOR, SPARK_CHANCE_PROMILLE, SPARK_INTENSITY>
// BASE: COLOR
// SPARKLE_COLOR: COLOR (defaults to white)
// SPARK_CHANCE_PROMILLE: a number
// SPARK_INTENSITY: a number
// Generally displays BASE, but creates little sparkles of SPARKLE_COLOR
// SPARK_CHANCE_PROMILLE decides how often a spark is generated, defaults to 300 (30%)
// SPARK_INTENSITY specifies how intens the spark is, defaults to 1024

#include "../functions/sparkle.h"
#include "layers.h"
#include "alpha.h"

template<
  class SPARKLE_COLOR = Rgb<255,255,255>,
  int SPARK_CHANCE_PROMILLE = 300,
  int SPARK_INTENSITY = 1024>
  using SparkleL = AlphaL<SPARKLE_COLOR, SparkleF<SPARK_CHANCE_PROMILLE, SPARK_INTENSITY>>;

template<class BASE,
  class SPARKLE_COLOR = Rgb<255,255,255>,
  int SPARK_CHANCE_PROMILLE = 300,
  int SPARK_INTENSITY = 1024>
  using Sparkle = Layers<BASE, SparkleL<SPARKLE_COLOR, SPARK_CHANCE_PROMILLE, SPARK_INTENSITY>>;

#endif
