#ifndef FUNCTIONS_CENTER_DIST_H
#define FUNCTIONS_CENTER_DIST_H

// Usage: Remap<CenterDistF<CENTER>,COLOR>
// Distributes led COLOR from CENTER
// CENTER : FUNCTION (defaults to Int<16384>)

template<class CENTER = Int<16384>>
class CenterDistF {
public:
  void run(BladeBase* blade) {
    center_.run(blade);
    num_leds_ = blade->num_leds();
  }
  int getInteger(int led) { return abs(led * 32768 / num_leds_ - center_.getInteger(led)); }

private:
  PONUA CENTER center_;
  int num_leds_;
};

#endif
