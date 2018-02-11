#ifndef FUNCTIONS_SIN_H
#define FUNCTIONS_SIN_H

// Usage: Sin<RPM, AMPLITUDE>
// pulses between 0-amplitude RPM times per minute
// AMPLITUDE: INTEGER (defaults to Int<32768>)
// RPM: INTEGER
// return value: INTEGER

#include "int.h"
#include "../common/sin_table.h"

class BladeBase;

template<class RPM, class AMPLITUDE=Int<32768>>
class Sin {
public:
  void run(BladeBase* blade) {
    rpm_.run(blade);
    amplitude_.run(blade);

    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / 60000000.0 * rpm_.getInteger());
    value_ = (sin_table[(int)floor(pos_ * 1024)] + 16384) * amplitude_.getInteger() >> 15;
  }
  int getInteger(int led) { return value_; }
private:
  RPM rpm_;
  AMPLITUDE amplitude_;
  int value_;
  uint32_t last_micros_;
  float pos_;
};

#endif
