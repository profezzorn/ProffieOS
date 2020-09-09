#ifndef FUNCTIONS_MULT_H
#define FUNCTIONS_MULT_H

// Usage: Mult<F, V>
// Fixed point multiplication of values F * V, 
// fixed point 16.15 arithmetic (32768 = 1.0)
// (2*2 would not result in 4), 
// (16384 * 16384 = 8192, representation of 0.5*0.5=0.25) 
// most blade functions use this method of fixed point calculations
// F, V: INTEGER, 
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
    return (f_.getInteger(led) * v_.getInteger(led)) >> 15;
  }
private:
  F f_;
  V v_;
};

// Usage: Percentage<F, V>
// Gets Percentage V of value F, 
// Percentages over 100% are allowed and will effectively be a multiplier. 
// F, V: INTEGER
// example Percentage<Int<16384>,25>
// this will give you 25% of Int<16384> and returns Int<4096>
// return value: INTEGER

template<class F, int V> //floats do no work in class templates...
  using Percentage = Mult<F, Int< V * 32768 / 100 >>;

#endif
