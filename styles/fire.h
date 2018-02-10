#ifndef STYLES_FIRE_H
#define STYLES_FIRE_H

struct FireConfig {
  FireConfig(int b, int r, int c) : intensity_base(b),
                                     intensity_rand(r),
                                     cooling(c) {}
  int intensity_base;
  int intensity_rand;
  int cooling;
};

// Fire-style
class StyleFire : public BladeStyle {
public:
  StyleFire(Color8 c1,
             Color8 c2,
             uint32_t delay,
             uint32_t speed,
             FireConfig normal,
             FireConfig clash,
             FireConfig lockup) :
    c1_(c1), c2_(c2),
    delay_(delay),
    speed_(speed),
    normal_(normal),
    clash_(clash),
    lockup_(lockup) {
  }
  enum OnState {
    STATE_OFF = 0,
    STATE_ACTIVATING,
    STATE_ON,
  };
  void activate() override {
    STDOUT.println("Fire Style");
    for (size_t i = 0; i < NELEM(heat_); i++) heat_[i] = 0;
  }
  bool On(BladeBase* blade) {
    if (!blade->is_on()) {
      state_ = STATE_OFF;
      return false;
    }
    switch (state_) {
      default:
         state_ = STATE_ACTIVATING;
         on_time_ = millis();
      case STATE_ACTIVATING:
         if (millis() - on_time_ < delay_) return false;
         state_ = STATE_ON;
      case STATE_ON:
         return true;
    }
  }
  void run(BladeBase* blade) override {
    uint32_t m = millis();
    int num_leds = blade->num_leds();
    if (m - last_update_ >= 10) {
      last_update_ = m;

      FireConfig config(0,0,0);
      if (blade->clash()) {
         config = clash_;
      } else if (On(blade)) {
        if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
          config = normal_;
        } else {
          config = lockup_;
        }
      } else {
         config = normal_;
         config.intensity_base = 0;
         config.intensity_rand = 0;
      }
      // Note heat_[0] is tip of blade
      for (int i = 0; i < speed_; i++) {
         heat_[num_leds + i] = config.intensity_base +
           random(random(random(config.intensity_rand)));
      }
      for (int i = 0; i < num_leds; i++) {
         int x = (heat_[i+speed_-1] * 3  +
                  heat_[i+speed_] * 10 +
                  heat_[i+speed_+1] * 3) >> 4;
         heat_[i] = clampi32(x - random(config.cooling), 0, 65535);
      }
    }
    bool zero = true;
    for (int i = 0; i < num_leds; i++) {
      int h = heat_[num_leds - 1 - i];
      Color8 c;
      if (h < 256) {
         c = Color8().mix(c1_, h);
      } else if (h < 512) {
         c = c1_.mix(c2_, h - 256);
      } else if (h < 768) {
         c = c2_.mix(Color8(255,255,255), h - 512);
      } else {
         c = Color8(255,255,255);
      }
      if (h) zero = false;
      blade->set(i, c);
    }
    if (zero) blade->allow_disable();
  }

private:
  uint32_t last_update_;
  unsigned short heat_[maxLedsPerStrip + 13];
  Color8 c1_, c2_;
  uint32_t delay_;
  OnState state_;
  uint32_t on_time_;
  int speed_;
  FireConfig normal_;
  FireConfig clash_;
  FireConfig lockup_;
};

template<class COLOR1, class COLOR2,
          int DELAY=0, int SPEED=2,
          int NORM_INT_BASE = 0, int NORM_INT_RAND=2000, int NORM_COOLING = 5,
          int CLSH_INT_BASE = 3000, int CLSH_INT_RAND=0, int CLSH_COOLING = 0,
          int LOCK_INT_BASE = 0, int LOCK_INT_RAND=5000, int LOCK_COOLING = 10>
class FireFactory : public StyleFactory {
  BladeStyle* make() override {
    return new StyleFire(
      COLOR1::color().dither(0), COLOR2::color().dither(0),
      DELAY, SPEED,
      FireConfig(NORM_INT_BASE, NORM_INT_RAND, NORM_COOLING),
      FireConfig(CLSH_INT_BASE, CLSH_INT_RAND, CLSH_COOLING),
      FireConfig(LOCK_INT_BASE, LOCK_INT_RAND, LOCK_COOLING));
  }
};

// Note: BLADE_NUM is is now irrelevant.
template<class COLOR1, class COLOR2,
          int BLADE_NUM=0, int DELAY=0, int SPEED=2,
          int NORM_INT_BASE = 0, int NORM_INT_RAND=2000, int NORM_COOLING = 5,
          int CLSH_INT_BASE = 3000, int CLSH_INT_RAND=0, int CLSH_COOLING = 0,
          int LOCK_INT_BASE = 0, int LOCK_INT_RAND=5000, int LOCK_COOLING = 10>
class StyleFactory* StyleFirePtr() {
  static FireFactory<COLOR1, COLOR2, DELAY, SPEED,
          NORM_INT_BASE, NORM_INT_RAND, NORM_COOLING,
          CLSH_INT_BASE, CLSH_INT_RAND, CLSH_COOLING,
          LOCK_INT_BASE, LOCK_INT_RAND, LOCK_COOLING> factory;
  return &factory;
}

#endif
