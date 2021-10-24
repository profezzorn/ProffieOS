#ifndef FUNCTIONS_STROBE_H
#define FUNCTIONS_STROBE_H

#include "../functions/int.h"

// Usage: StrobeF<STROBE_FREQUENCY, STROBE_MILLIS>
// STROBE_FREQUENCY_FUNC, STROBE_MILLIS_FUNC: FUNCTION
// return value: INTEGER
// Stroboscope-like effect, turns the color to STROBE_COLOR for STROBE_MILLIS
// STROBE_FREQUENCY times per second.

template<class STROBE_FREQUENCY, class STROBE_MILLIS>
class StrobeF {
public:
   void run(BladeBase* blade) {
     strobe_frequency_.run(blade);
     strobe_millis_.run(blade);
     uint32_t m = millis();
     uint32_t strobe_millis = strobe_millis_.calculate(0);
     uint32_t strobe_frequency = strobe_frequency_.calculate(0);
     uint32_t timeout = strobe_ ?  strobe_millis : (1000 / strobe_frequency);
     if (m - strobe_start_ > timeout) {
        strobe_start_ += timeout;
        if (m - strobe_start_ > strobe_millis + (1000 / strobe_frequency))
          strobe_start_ = m;
        strobe_ = !strobe_;
     }
   }
   int getInteger(int led) { return strobe_  * 32768; }
private:
  bool strobe_;
  PONUA SVFWrapper<STROBE_FREQUENCY> strobe_frequency_;
  PONUA SVFWrapper<STROBE_MILLIS> strobe_millis_;
  uint32_t strobe_start_;
};

#endif
