#ifndef FUNCTIONS_INCREMENT_F_H
#define FUNCTIONS_INCREMENT_F_H

// Usage: IncrementF<F, V, MAX, I>
// Increases by value I (up to MAX) each time F >= V
// Detection resets once F drops below V * HYST_PERCENT
// if greater than MAX returns 0
// F, V, I, MAX = numbers
// HYST_PERCENT = percent (defaults to 66)
// return value: INTEGER

// NOTE: this function is designed to separate "events" for use with *Select styles.  
// This function may break up SwingSpeed effects or other continuous responsive functions.

template<class F, class V = Int<32768>, class MAX = Int<32768>, class I = Int<1>, class HYST_PERCENT = Int<66>>
class IncrementF {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    v_.run(blade);
    i_.run(blade);
    max_.run(blade);
    hyst_.run(blade);
    int f = f_.getInteger(0);
    int threshold = v_.getInteger(0);
    if (f >= threshold) {
      if (!active_) {
        value_ += i_.getInteger(0);;
        active_ = true;
      }      
    }
    if (f < threshold * hyst_.getInteger(0) / 100) {
      active_ = false;
    }
    value_ %= max_.getInteger(0);
  }
  int getInteger(int led) { return value_; }

private:
  F f_;
  V v_;
  I i_;
  MAX max_;
  HYST_PERCENT hyst_;
  bool active_ = false;
  int value_;
};

#endif
