#ifndef STYLES_RANDOM_BLINK_H
#define STYLES_RANDOM_BLINK_H

template<int MILLIHZ, class COLOR1 = WHITE, class COLOR2 = BLACK>
class RandomBlink {
public:
  void run(BladeBase* blade) {
    c1_.run(blade);
    c2_.run(blade);
    uint32_t now = micros();
    if (now - last_update_ > 1000000000 / MILLIHZ) {
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
  unsigned short bits_[(maxLedsPerStrip + 15)/ 16];
  uint32_t last_update_;
};

#endif
