#ifndef FUNCTIONS_SIN_H
#define FUNCTIONS_SIN_H

#include "svf.h"

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
class SinSVF {
public:
  void run(BladeBase* blade) {
    rpm_.run(blade);
    high_.run(blade);
    low_.run(blade);
  }

  int calculate(BladeBase* blade) {
    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / 60000000.0 * rpm_.calculate(blade));
    int high = high_.calculate(blade);
    int low = low_.calculate(blade);
    float tmp = sin_table[(int)floorf(pos_ * 1024)] / 32768.0;
    return (int)( (tmp + 0.5) * (high - low) + low );
  }
private:
  PONUA SVFWrapper<RPM> rpm_;
  PONUA SVFWrapper<LOWclass> low_;
  PONUA SVFWrapper<HIGHclass> high_;
  uint32_t last_micros_;
  float pos_;
};

template<class RPM, class LOWclass=Int<0>, class HIGHclass=Int<32768>>
using Sin = SingleValueAdapter<SinSVF<RPM, LOWclass, HIGHclass>>;

template<class RPM, class LOWclass=Int<0>, class HIGHclass=Int<32768>>
class SawSVF {
public:
  void run(BladeBase* blade) {
    rpm_.run(blade);
    high_.run(blade);
    low_.run(blade);
  }

  int calculate(BladeBase* blade) {
    uint32_t now = micros();
    uint64_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / 60000000.0 * rpm_.calculate(blade));
    int high = high_.calculate(blade);
    int low = low_.calculate(blade);
    return low + pos_ * (high - low);
  }
private:
  PONUA SVFWrapper<RPM> rpm_;
  PONUA SVFWrapper<LOWclass> low_;
  PONUA SVFWrapper<HIGHclass> high_;
  uint32_t last_micros_;
  float pos_;
};

template<class RPM, class LOWclass=Int<0>, class HIGHclass=Int<32768>>
using Saw = SingleValueAdapter<SawSVF<RPM, LOWclass, HIGHclass>>;

template<typename PULSE_MILLIS>
class PulsingFSVF {
public:
  void run(BladeBase* base) {
    pulse_millis_.run(base);
  }
  int calculate(BladeBase* base) {
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / (1000.0 * pulse_millis_.calculate(base)));
    return (sin_table[(int)floorf(pos_ * 0x400)] + 16384);
  }

private:
  PONUA SVFWrapper<PULSE_MILLIS> pulse_millis_;
  uint32_t last_micros_;
  float pos_ = 0.0;
};

template<typename PULSE_MILLIS>
using PulsingF = SingleValueAdapter<PulsingFSVF<PULSE_MILLIS>>;

#endif
