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
    center = center_.getInteger(0);
    num_leds_ = blade->num_leds();
  }
  int getInteger(int led) { return abs(led * 32768 / num_leds_ - center); }

private:
  CENTER center_;
  int center;
  int num_leds_;
};

#endif
