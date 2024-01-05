#ifndef FUNCTIONS_LAYER_FUNCTIONS_H
#define FUNCTIONS_LAYER_FUNCTIONS_H

// Usage: LayerFunctions<F1, F2, ...>
// F1, F2: FUNCTIONS
// return value: FUNCTION
// Returns (32768 - (32768 - F1) * (32768 * F2) / 32768)
// This is the same as 1-(1-F1)*(1-F2), but multiplied by 32768.
// Basically Mix<LayerFunctions<F1, F2>, A, B> is the same as Mix<F2, Mix<F1, A, B>, B>.

template<class ... REST> class LayerFunctions {};

template<class X> class LayerFunctions<X> : public X {};

template<class A, class... B>
class LayerFunctions<A, B...> {
 public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  int getInteger(int led) {
    return 32768 - (((32768 - a_.getInteger(led)) * (32768 - b_.getInteger(led))) >> 15);
  }

private:
  PONUA A a_;
  PONUA LayerFunctions<B...> b_;
};

#endif
