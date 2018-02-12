#ifndef FUNCTIONS_SIN_H
#define FUNCTIONS_SIN_H

// Usage: Sin<RPM, LOW, HIGH>
// pulses between LOW - HIGH RPM times per minute
// LOW: INTEGER (defaults to Int<0>)
// HIGH: INTEGER (defaults to Int<32768>)
// RPM: INTEGER
// return value: INTEGER

#include "int.h"
#include "../common/sin_table.h"

class BladeBase;

template<class RPM, class LOWclass=Int<0>, class HIGHclass=Int<32768>>
class Sin {
public:
  void run(BladeBase* blade) {
    rpm_.run(blade);
    high_.run(blade);
    low_.run(blade);

    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / 60000000.0 * rpm_.getInteger());
    int high = high_.getInteger();
    int low = low_.getInteger();
    float tmp = sin_table[(int)floor(pos_ * 1024)] / 32768.0;
    value_ = (int)( (tmp + 0.5) * (high - low) + low );
  }
  int getInteger(int led) { return value_; }
private:
  RPM rpm_;
  LOWclass low_;
  HIGHclass high_;
  int value_;
  uint32_t last_micros_;
  float pos_;
};

#endif
