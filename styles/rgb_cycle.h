#ifndef STYLES_RGB_CYCLE_H
#define STYLES_RGB_CYCLE_H

// Usage: RgbCycle
// (no arguments)
// return value: COLOR
// Very fast Red, Green, Blue cycle, result should essentially be white
// until you start swinging it around.
class RGBCycle {
public:
  void run(BladeBase* base) {
    if (millis() == millis_at_last_call_)
      return;
    millis_at_last_call_ = millis();
    n_++;
    if (n_ >= 3) n_ = 0;
  }  
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.overdrive = false;
    switch (n_) {
      case 0: ret.c = Color16(65535, 0, 0); break;
      case 1: ret.c = Color16(0, 65535, 0); break;
      case 2: ret.c = Color16(0, 0, 65535); break;
    }
    return ret;
  }
private:
  uint32_t millis_at_last_call_;
  uint8_t n_;
};

#endif
