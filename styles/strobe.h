#ifndef STYLES_STROBE_H
#define STYLES_STROBE_H

#include "../functions/int.h"

// Usage: Strobe<BASE, STROBE_COLOR, STROBE_FREQUENCY, STROBE_MILLIS>
// or: StrobeX<BASE, STROBE_COLOR, STROBE_FREQUENCY_FUNC, STROBE_MILLIS_FUNC>
// BASE, STROBE_COLOR: COLOR
// STROBE_FREQUENCY, STROBE_MILLIS: a number
// STROBE_FREQUENCY_FUNC, STROBE_MILLIS_FUNC: FUNCTION
// return value: COLOR
// Stroboscope-like effect, turns the color to STROBE_COLOR for STROBE_MILLIS
// STROBE_FREQUENCY times per second.

template<class T, class STROBE_COLOR, class STROBE_FREQUENCY, class STROBE_MILLIS>
class StrobeX {
public:
   void run(BladeBase* blade) {
     base_.run(blade);
     strobe_color_.run(blade);
     strobe_frequency_.run(blade);
     strobe_millis_.run(blade);
     uint32_t m = millis();
     uint32_t strobe_millis = strobe_millis_.getInteger(0);
     uint32_t strobe_frequency = strobe_frequency_.getInteger(0);
     uint32_t timeout = strobe_ ?  strobe_millis : (1000 / strobe_frequency);
     if (m - strobe_start_ > timeout) {
        strobe_start_ += timeout;
        if (m - strobe_start_ > strobe_millis + (1000 / strobe_frequency))
          strobe_start_ = m;
        strobe_ = !strobe_;
     }
   }
   OverDriveColor getColor(int led) {
     if (strobe_) {
        return strobe_color_.getColor(led);
     } else {
        return base_.getColor(led);
     }
   }
private:
  bool strobe_;
  T base_;
  STROBE_COLOR strobe_color_;
  STROBE_FREQUENCY strobe_frequency_;
  STROBE_MILLIS strobe_millis_;
  uint32_t strobe_start_;
};

template<class T, class STROBE_COLOR, int STROBE_FREQUENCY, int STROBE_MILLIS = 1>
  using Strobe = StrobeX<T, STROBE_COLOR, Int<STROBE_FREQUENCY>, Int<STROBE_MILLIS> >;

#endif
