#ifndef STYLES_RANDOM_BLINK_H
#define STYLES_RANDOM_BLINK_H

// usage: RandomBlink<MILLIHZ, COLOR1, COLOR2>
// or: RandomBlinkX<MILLIHZ_CLASS, COLOR1, COLOR2>
// MILLIHZ: integer
// MILLHZ_CLASS: NUMBER
// COLOR1: COLOR (defaults to WHITE)
// COLOR2: COLOR (defaults to BLACK)
// return value: COLOR
// Each LED is randomly chosen as COLOR1 or COLOR2, then stays
// that color for 1000/MILLIHZ seconds.

template<class MILLIHZ, class COLOR1 = WHITE, class COLOR2 = BLACK>
class RandomBlinkX {
public:
  void run(BladeBase* blade) {
    c1_.run(blade);
    c2_.run(blade);
    millihz_.run(blade);
    uint32_t now = micros();
    if (now - last_update_ > 1000000000U / millihz_.getInteger(0)) {
      last_update_ = now;
      size_t shorts = (blade->num_leds() + 15) / 16;
      for (size_t i = 0; i < shorts; i++) bits_[i] = rand();
    }
  }
  OverDriveColor getColor(int led) {
    return (bits_[led>>4] >> (led & 0xf) & 1) ? c1_.getColor(led) : c2_.getColor(led);
  }
  
private:
  COLOR1 c1_;
  COLOR2 c2_;
  MILLIHZ millihz_;
  unsigned short bits_[(maxLedsPerStrip + 15)/ 16];
  uint32_t last_update_;
};

template<int MILLIHZ, class COLOR1 = WHITE, class COLOR2 = BLACK>
  using RandomBlink = RandomBlinkX<Int<MILLIHZ>, COLOR1, COLOR2>;

#endif
