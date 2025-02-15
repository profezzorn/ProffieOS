#ifndef FUNCTIONS_CENTER_DIST_H
#define FUNCTIONS_CENTER_DIST_H

// Usage: CenterDistF<CENTER>
// CENTER : FUNCTION (defaults to Int<16384>)
// Returns distance from center, a full blade length counts as 32768.

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
