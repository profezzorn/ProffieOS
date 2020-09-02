#ifndef FUNCTIONS_PCT_H
#define FUNCTIONS_PCT_H

// Usage: Pct<F, V>
// Gets Percentage V of value F, 
// Percentages over 100% are allowed and will effectively be a multiplier. 
// Can be used to multiply or devide any number  
// F, V: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class V>
class Pct {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int getInteger(int led) {
    return int(f_.getInteger(led) * float(v_.getInteger(led))/100.0f * 32768) >> 15;
  }
private:
  F f_;
  V v_;
};

#endif
