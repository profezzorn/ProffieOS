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
    pos_ = fract(pos_ + delta / 60000000.0 * rpm_.getInteger(0));
    int high = high_.getInteger(0);
    int low = low_.getInteger(0);
    float tmp = sin_table[(int)floorf(pos_ * 1024)] / 32768.0;
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

template<class RPM, class LOWclass=Int<0>, class HIGHclass=Int<32768>>
class Saw {
public:
  void run(BladeBase* blade) {
    rpm_.run(blade);
    high_.run(blade);
    low_.run(blade);

    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / 60000000.0 * rpm_.getInteger(0));
    int high = high_.getInteger(0);
    int low = low_.getInteger(0);
    value_ = low + pos_ * (high - low);
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

template<typename PULSE_MILLIS>
class PulsingF {
public:
  void run(BladeBase* base) {
    pulse_millis_.run(base);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / (1000.0 * pulse_millis_.getInteger(0)));
    mix_ = (sin_table[(int)floorf(pos_ * 0x400)] + 16384);
  }

  int getInteger(int led) { return mix_; }

private:
  PULSE_MILLIS pulse_millis_;
  int mix_;
  uint32_t last_micros_;
  float pos_ = 0.0;
};

#endif
