#ifndef FUNCTIONS_MULT_H
#define FUNCTIONS_MULT_H

// Usage: Pct<F, V>
// Gets Percentage V of value F, 
// Percentages over 100% are allowed and will effectively be a multiplier. 
// Can be used to multiply or devide any number  
// F, V: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class V>
class Mult {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int getInteger(int led) {
    return clampi32((f_.getInteger(led) * v_.getInteger(led)) >> 15, 0, 32768);
  }
private:
  F f_;
  V v_;
};

#endif
