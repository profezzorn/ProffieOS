#ifndef FUNCTIONS_ISLESSTHAN_H
#define FUNCTIONS_ISLESSTHAN_H

// Usage: IsLessThan<F, V>
// Returns 0 or 32768 based on V
// If F < V returns 0, if F >= V returns 32768 
// F, V: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class V>
class IsLessThan {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
    int f = f_.getInteger(0);
    int v = v_.getInteger(0);
    if(f >= v) {
    value_ = 0;
    } else {
    value_ = 32768;
    }
  }
  int getInteger(int led) { return value_; }

private:
  F f_;
  V v_;
  int value_;
  
};

#endif
