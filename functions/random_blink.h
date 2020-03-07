#ifndef FUNCTIONS_RANDOM_BLINK_H
#define FUNCTIONS_RANDOM_BLINK_H

template<class MILLIHZ>
class RandomBlinkF {
public:
  void run(BladeBase* blade) {
    millihz_.run(blade);
    uint32_t now = micros();
    if (now - last_update_ > 1000000000U / millihz_.getInteger(0)) {
      last_update_ = now;
      size_t shorts = (blade->num_leds() + 15) / 16;
      for (size_t i = 0; i < shorts; i++) bits_[i] = rand();
    }
  }
  int getInteger(int led) {
    return (bits_[led>>4] >> (led & 0xf) & 1) * 32768;
  }
  
private:
  MILLIHZ millihz_;
  unsigned short bits_[(maxLedsPerStrip + 15)/ 16];
  uint32_t last_update_;
};

#endif
