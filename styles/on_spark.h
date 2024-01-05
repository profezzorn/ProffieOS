#ifndef STYLES_ON_SPARK_H
#define STYLES_ON_SPARK_H

// Usage: OnSpark<BASE, SPARK_COLOR, MILLIS>
// Or: OnSparX<BASE, SPARK_COLOR, MILLI_CLASS>
// Or: OnSparL<SPARK_COLOR, MILLI_CLASS>
// BASE: COLOR
// SPARK_COLOR: COLOR (defaults to white)
// MILLIS: a number (defaults to 200)
// MILLI_CLASS: FUNCTION (defaults to Int<200>)
// return value: COLOR
// When you turn the saber on, it starts with SPARK_COLOR, and then
// fades to BASE over a peariod of MILLIS millseconds.

#include "alpha.h"
#include "layers.h"
#include "../functions/on_spark.h"

template<class SPARK_COLOR = Rgb<255,255,255>, class MILLIS = Int<200> >
  using OnSparkL = AlphaL<SPARK_COLOR, OnSparkF<MILLIS>>;

template<class T, class SPARK_COLOR = Rgb<255,255,255>, class MILLIS = Int<200> >
  using OnSparkX = Layers<T, OnSparkL<SPARK_COLOR, MILLIS>>;

template<class T, class SPARK_COLOR = Rgb<255,255,255>, int MILLIS = 200>
  using OnSpark = OnSparkX<T, SPARK_COLOR, Int<MILLIS>>;

#endif
