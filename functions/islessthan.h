#ifndef FUNCTIONS_ISLESSTHAN_H
#define FUNCTIONS_ISLESSTHAN_H

// Usage: IsLessThan<F, V>
// Returns 0 or 32768 based on V
// If F < V returns 32768, if F >= V returns 0 
// F, V: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class V>
class IsLessThan {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
  }
  int getInteger(int led) {
    return (f_.getInteger(led) < v_.getInteger(0)) << 15;
  }

private:
  F f_;
  V v_;
};

#endif
