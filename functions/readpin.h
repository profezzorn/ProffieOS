#ifndef FUNCTIONS_READPIN_H
#define FUNCTIONS_READPIN_H

#include "../common/analog_read.h"

// Usage: ReadPinF<PIN>
// or: ReadPinF<PIN, PIN_MODE>
// PIN: int, pin you want your style to respond to
// PIN_MODE: int, one of INPUT, INPUT_PULLUP or INPUT_PULLDOWN, defaults to INPUT
// returns INTEGER, 0 if pin is low and 32768 if pin is high

template<int pin, int pin_mode = INPUT>
class ReadPinSVF {
public:
  ReadPinSVF() {
    pinMode(pin, pin_mode);
  }
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    return digitalRead(pin) == HIGH ? 32768 : 0;
  }
};

template<int pin, int pin_mode = INPUT>
using ReadPinF = SingleValueAdapter<ReadPinSVF<pin, pin_mode>>;


// Usage: AnalogReadPinF<PIN>
// or: AnalogReadPinF<PIN, PIN_MODE>
// PIN: int, pin you want your style to respond to
// PIN_MODE: int, one of INPUT, INPUT_PULLUP or INPUT_PULLDOWN, defaults to INPUT
// returns INTEGER, 0-32768 depending on input reading.
// Notes:
//   * May cause slowdowns
//   * may not update every run() call
//   * pin modes other than INPUT may not be supported,
//   * Only analog-capable pins will work.

template<int pin, int pin_mode = INPUT>
class AnalogReadPinSVF {
public:
  AnalogReadPinSVF() : reader_(pin, pin_mode) {}
  void run(BladeBase* blade) {
    do {
      if (!started_) {
	if (reader_.Start()) {
	  started_ = true;
	}
      } else {
	if (reader_.Done()) {
	  value_ = reader_.Value12() * 8;
	  started_ = false;
	}
      }
    } while (value_ == -1);
  }
  int calculate(BladeBase* blade) {
    return value_;
  }
  int value_ = -1;
  AnalogReader reader_;
  bool started_ = false;
};

template<int pin, int pin_mode = INPUT>
using AnalogReadPinF = SingleValueAdapter<AnalogReadPinSVF<pin, pin_mode>>;


#endif  // FUNCTIONS_READPIN_H
