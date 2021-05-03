#ifndef STYLES_FIRE_H
#define STYLES_FIRE_H

#include "style_ptr.h"

struct FireConfiguration {
  int intensity_base;
  int intensity_rand;
  int cooling;
};

template<int INTENSITY_BASE, int INTENSITY_RAND, int COOLING>
struct FireConfig {
  static constexpr int Cooling = COOLING;
  static FireConfiguration get() {
    FireConfiguration ret;
    ret.intensity_base = INTENSITY_BASE;
    ret.intensity_rand = INTENSITY_RAND;
    ret.cooling = COOLING;
    return ret;
  }
};

template<int DELAY = 0, int SPEED = 2,
  class NORM = FireConfig<0,2000,5>,
  class CLASH = FireConfig<3000,0,0>,
  class LOCK = FireConfig<0, 5000, 10>,
  class OFF = FireConfig<0, 0, NORM::Cooling>>
class StyleFireBase {
protected:
  ~StyleFireBase() {
    delete[] heat_;
  }
  enum OnState {
    STATE_OFF = 0,
    STATE_ACTIVATING,
    STATE_ON,
  };
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
         if (millis() - on_time_ < DELAY) return false;
         state_ = STATE_ON;
      case STATE_ON:
         return true;
    }
  }
  bool run(BladeBase* blade) {
    bool keep_running = true;
    uint32_t m = millis();
    num_leds_ = blade->num_leds();
    if (!heat_) {
      size_t N = num_leds_ + SPEED + 3;
      heat_ = new unsigned short[N];
      for (size_t i = 0; i < N; i++) heat_[i] = 0;
    }
    if (m - last_update_ >= 10) {
      last_update_ = m;

      FireConfiguration config = OFF::get();
      if (clash_.Detect(blade)) {
	config = CLASH::get();
      } else if (On(blade)) {
        if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
          config = NORM::get();
        } else {
          config = LOCK::get();
        }
      }
      // Note heat_[0] is tip of blade
      for (int i = 0; i < SPEED; i++) {
         heat_[num_leds_ + i] = config.intensity_base +
           random(random(random(config.intensity_rand)));
      }
      int zero = true;
      for (int i = 0; i < num_leds_; i++) {
         int x = (heat_[i+SPEED-1] * 3  +
                  heat_[i+SPEED] * 10 +
                  heat_[i+SPEED+1] * 3) >> 4;
         heat_[i] = clampi32(x - random(config.cooling), 0, 65535);
	 if (heat_[i]) zero = false;
      }
      if (zero) keep_running = false;
    }
    return keep_running;
  }

  OneshotEffectDetector<EFFECT_CLASH> clash_;
  int num_leds_;
  uint32_t last_update_;
  unsigned short* heat_ = 0;
  OnState state_ = STATE_OFF;
  uint32_t on_time_;
};

template<class COLOR1, class COLOR2,
  int DELAY = 0, int SPEED = 2,
  class NORM = FireConfig<0,2000,5>,
  class CLASH = FireConfig<3000,0,0>,
  class LOCK = FireConfig<0, 5000, 10>,
  class OFF = FireConfig<0, 0, NORM::Cooling>>
class StyleFire : StyleFireBase<DELAY, SPEED, NORM, CLASH, LOCK, OFF>{
public:
  StyleFire() {}
  bool run(BladeBase* blade) {
    c1_.run(blade);
    c2_.run(blade);
    return StyleFireBase<DELAY, SPEED, NORM, CLASH, LOCK, OFF>::run(blade);
  }

  SimpleColor getColor(int led) {
    int h = this->heat_[this->num_leds_ - 1 - led];
    SimpleColor c;
    if (h < 256) {
      c.c = Color16().mix(c1_.getColor(led).c, h);
    } else if (h < 512) {
      c.c = c1_.getColor(led).c.mix(c2_.getColor(led).c, h - 256);
    } else if (h < 768) {
      c.c = c2_.getColor(led).c.mix(Color16(65535,65535,65535), h - 512);
    } else {
      c.c = Color16(65535, 65535, 65535);
    }
    return c;
  }

private:
  COLOR1 c1_;
  COLOR2 c2_;
};


// Note: BLADE_NUM is is now irrelevant.
template<class COLOR1, class COLOR2,
          int BLADE_NUM=0, int DELAY=0, int SPEED=2,
          int NORM_INT_BASE = 0, int NORM_INT_RAND=2000, int NORM_COOLING = 5,
          int CLSH_INT_BASE = 3000, int CLSH_INT_RAND=0, int CLSH_COOLING = 0,
          int LOCK_INT_BASE = 0, int LOCK_INT_RAND=5000, int LOCK_COOLING = 10,
          int OFF_INT_BASE = 0, int OFF_INT_RAND=0, int OFF_COOLING = 10>
StyleAllocator StyleFirePtr() {
  static StyleFactoryImpl<Style<StyleFire<COLOR1, COLOR2, DELAY, SPEED,
  FireConfig<NORM_INT_BASE, NORM_INT_RAND, NORM_COOLING>,
  FireConfig<CLSH_INT_BASE, CLSH_INT_RAND, CLSH_COOLING>,
  FireConfig<LOCK_INT_BASE, LOCK_INT_RAND, LOCK_COOLING>,
  FireConfig<OFF_INT_BASE, OFF_INT_RAND, OFF_COOLING>>>> factory;
  return &factory;
}

// Simplified Fire, negates Clash, Lockup and Off FireConfig<> to use Fire with other Layered effects
template<class COLOR1, class COLOR2, int DELAY=0, int SPEED=2, int BASE=0, int RAND=2000, int COOLING=5> 
using StaticFire = StyleFire<COLOR1, COLOR2, DELAY, SPEED, 
                FireConfig<BASE, RAND, COOLING>, 
                FireConfig<BASE, RAND, COOLING>, 
                FireConfig<BASE, RAND, COOLING>, 
                FireConfig<BASE, RAND, COOLING>>;

#endif
