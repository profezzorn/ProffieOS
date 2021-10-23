#ifndef FUNCTIONS_IFON_H
#define FUNCTIONS_IFON_H

#include "int.h"

// Usage: Ifon<A, B>
// Returns A if saber is on, B otherwise.
// A, B: INTEGER
// return value: INTEGER

// TODO: optimize when the A & B is the same for all LEDs.

class BladeBase;

template<class IFON, class IFOFF>
class Ifon {
public:
  void run(BladeBase* blade) {
    ifon_.run(blade);
    ifoff_.run(blade);
    on_ = blade->is_on();
  }
  int getInteger(int led) {
    return on_ ? ifon_.getInteger(led) : ifoff_.getInteger(led);
  }

private:
  PONUA IFON ifon_;
  PONUA IFOFF ifoff_;
  bool on_;
};

// InOutFunc<OUT_MILLIS, IN_MILLIS>
// IN_MILLIS, OUT_MILLIS: a number
// RETURN VALUE: FUNCTION
// 0 when off, 32768 when on, takes OUT_MILLIS to go from 0 to 32768
// takes IN_MILLIS to go from 32768 to 0.
template<class OUT_MILLIS, class IN_MILLIS>
class InOutFuncX {
public:
  FunctionRunResult run(BladeBase* blade) {
    out_millis_.run(blade);
    in_millis_.run(blade);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (blade->is_on()) {
      if (extension == 0.0) {
         // We might have been off for a while, so delta might
         // be insanely high.
         extension = 0.00001;
      } else {
 	extension += delta / (out_millis_.calculate(blade) * 1000.0);
	extension = std::min(extension, 1.0f);
      }
    } else {
      extension -= delta / (in_millis_.calculate(blade) * 1000.0);
      extension = std::max(extension, 0.0f);
    }
    ret_ = extension * 32768.0;
    if (!blade->is_on() && ret_ == 0) return FunctionRunResult::ZERO_UNTIL_IGNITION;
    return FunctionRunResult::UNKNOWN;
  }
  int getInteger(int led) { return ret_; }

private:
  PONUA SVFWrapper<OUT_MILLIS> out_millis_;
  PONUA SVFWrapper<IN_MILLIS> in_millis_;
  float extension = 0.0;
  uint32_t last_micros_;
  int ret_;
};


template<class EXTENSION, bool ALLOW_DISABLE=1 >
class InOutHelperF {
public:
  FunctionRunResult run(BladeBase* blade) __attribute__((warn_unused_result)) {
    FunctionRunResult ret = RunFunction(&extension_, blade);
    thres = (extension_.calculate(blade) * blade->num_leds());
    if (ALLOW_DISABLE) {
      switch (ret) {
	case FunctionRunResult::ZERO_UNTIL_IGNITION: return FunctionRunResult::ONE_UNTIL_IGNITION;
	case FunctionRunResult::ONE_UNTIL_IGNITION: return FunctionRunResult::ZERO_UNTIL_IGNITION;
	case FunctionRunResult::UNKNOWN: break;
      }
    }
    return FunctionRunResult::UNKNOWN;
  }
  int getInteger(int led) {
    return 32768 - clampi32(thres - led * 32768, 0, 32768);
  }
private:
  PONUA SVFWrapper<EXTENSION> extension_;
  int thres = 0;
};



#include "trigger.h"
#include "scale.h"

template<int OUT_MILLIS, int IN_MILLIS>
  using InOutFunc = InOutFuncX<Int<OUT_MILLIS>, Int<IN_MILLIS>>;

template<int OUT_MILLIS, int IN_MILLIS, int EXPLODE_MILLIS>
  using InOutFuncTD = InOutFuncX<Int<OUT_MILLIS>, Scale<Trigger<EFFECT_BLAST, Int<0>, Int<EXPLODE_MILLIS>, Int<0>>, Int<IN_MILLIS>, Int<EXPLODE_MILLIS>>>;

#endif
