#ifndef BLADES_SAVI_BLADE_H
#define BLADES_SAVI_BLADE_H

#if VERSION_MAJOR >= 4

#include "abstract_blade.h"

// Currently only supports one blade.
class SaviBlade : public AbstractBlade, Looper {
public:
   SaviBlade(int pin, PowerPinInterface* power) :
    AbstractBlade(),
      Looper(NOLINK),
      power_(power),
      pin_(pin) {
  }

  const char* name() override { return "Savi_Blade"; }

  void Activate() override {
    STDOUT.println("Savi Blade");
#ifdef TIM7_BASE
    stm32l4_timer_create(&timer_, TIMER_INSTANCE_TIM7, STM32L4_TONE_IRQ_PRIORITY, 0);
#else
    stm32l4_timer_create(&timer_, TIMER_INSTANCE_TIM2, STM32L4_TONE_IRQ_PRIORITY, 0);
#endif
    Power(true);
    Looper::Link();
    AbstractBlade::Activate();
    digitalWrite(pin_, HIGH);
    pinMode(pin_, OUTPUT);
  }

  void Deactivate() override {
    pinMode(pin_, INPUT_ANALOG);
    Power(false);
    AbstractBlade::Deactivate();
    Looper::Unlink();
    stm32l4_timer_stop(&timer_);
    stm32l4_timer_disable(&timer_);
    stm32l4_timer_destroy(&timer_);
  }

  void Power(bool on) {
    if (on) EnableBooster();
    if (powered_ != on) {
      if (on) {
	power_->Init();
	power_->Power(true);
	if (current_style_) current_style_->run(this); // get the color
      } else {
	power_->Power(false);
	power_->DeInit();
      }
      powered_ = on;
    }
  }

  // BladeBase implementation
  int num_leds() const override { return 1; }
  Color8::Byteorder get_byteorder() const override {
    // Not quite true...
    return Color8::RGB;
  }
  bool is_on() const override {
    return on_;
  }
  bool is_powered() const override {
    return powered_;
  }
  void set(int led, Color16 c) override {
    int color = 0;
    switch ((c.r > 32768 ? 0x100 : 0x0) +
	    (c.g > 32768 ? 0x10 : 0x0) +
	    (c.b > 32768 ? 0x1 : 0x0)) {
      default:
      case 0x111: color = 0; break; // white
      case 0x100: color = 1; break; // red
      case 0x110: color = 3; break; // yellow
      case 0x010: color = 4; break; // green
      case 0x011: color = 5; break; // teal
      case 0x001: color = 6; break; // blue
      case 0x101: color = 7; break; // purple
    }
    if (color != color_ && on_) {
      SendCMD(0xA0 | color);
    }
    color_ = color;
  }

  void set_overdrive(int led, Color16 c) override {
    set(led, c);
  }

  void allow_disable() override {
    if (!on_) Power(false);
  }
  virtual void SetStyle(BladeStyle* style) {
    Power(true);
    AbstractBlade::SetStyle(style);
  }

  // SaberBase implementation
  void SB_IsOn(bool *on) override {
    if (on_ || powered_) *on = true;
  }

  void SB_On() override {
    AbstractBlade::SB_On();
    battery_monitor.SetLoad(true);
    on_ = true;
    Power(true);

    SendCMD(0x20 + color_);
    state_ = 0xA0 + color_;
    last_send_ = millis();
  }

  void SB_Off(OffType off_type) override {
    AbstractBlade::SB_Off(off_type);
    battery_monitor.SetLoad(false);
    on_ = false;
    SendCMD(0x40);
    last_send_ = millis();
    state_ = 0;
    if (off_type == OFF_IDLE) {
      Power(false);
    }
  }

  void SB_Effect(BladeEffectType type, float location) {
    AbstractBlade::SB_Effect(type, location);
    switch (type) {
      default: break;
      case EFFECT_CLASH:
      case EFFECT_STAB:
	SendCMD(0xC0);
	break;
      case EFFECT_LOCKUP_BEGIN:
      case EFFECT_DRAG_BEGIN:
	SetState(0x60);
	break;
      case EFFECT_LOCKUP_END:
      case EFFECT_DRAG_END:
	SetState(0xA0 + color_);
	break;
    }
  }

protected:
  void SendCMD(uint8_t cmd) {
    commands_[end_] = cmd;
    end_ = (end_ + 1) % NELEM(commands_);
    if (!run_) {
      run_ = true;
      n_ = 0;
      int frequency = 1000000;
      int modulus = 17000; // 17ms
      stm32l4_timer_enable(&timer_, (stm32l4_timer_clock(&timer_) / frequency) -1, modulus -1, TIMER_OPTION_COUNT_PRELOAD, &SaviBlade::savi_irq, this, TIMER_EVENT_PERIOD);
      stm32l4_timer_start(&timer_, false);
    }
  }

  void SetState(uint8_t state) {
    state_ = state;
    last_send_ = millis();
    SendCMD(state);
  }

  void Loop() override {
    if (!powered_) return;
    // Make sure the booster is running so we don't get low voltage
    // and under-drive any FETs.
    EnableBooster();
    if (current_style_)
      current_style_->run(this);

    if (millis() - last_send_ > 550) {
      if (on_) {
	SetState(state_);
      } else {
	Power(false);
      }
    }
  }

  void irq(uint32_t events) {
    if (begin_ == end_) {
      stm32l4_timer_stop(&timer_);
      stm32l4_timer_disable(&timer_);
      run_ = false;
      return;
    }
    digitalWrite(pin_, n_ & 1);
    if (n_ >= 2) {
      if (n_ & 1) {
	if (n_ == 19) {
	  n_ = -1;
	  begin_ = (begin_ + 1) % NELEM(commands_);
	} else {
	  stm32l4_timer_period(&timer_, (commands_[begin_] & 0x80) ? 1200 : 2400, false);
	  commands_[begin_] <<= 1;
	}
      } else {
	if (n_ == 18) {
	  stm32l4_timer_period(&timer_, 17000, false);
	} else {
	  stm32l4_timer_period(&timer_, (commands_[begin_] & 0x80) ? 2400 : 1200, false);
	}
      }
    }
    n_++;
  }

  static void savi_irq(void *context, uint32_t events) {
    ((SaviBlade*)context)->irq(events);
  }
  
private:
  bool on_ = false;
  bool powered_ = false;
  PowerPinInterface* power_;
  int pin_;
  stm32l4_timer_t timer_;

  uint8_t color_ = 0;
  int8_t n_ = 0;
  uint8_t state_  = 0;
  uint32_t last_send_ = 0;

  volatile uint8_t end_;
  volatile uint8_t begin_;
  volatile uint8_t commands_[8];
  uint8_t cmd_;
  volatile bool run_ = false;
};

template<int pin, class POWER_PINS = PowerPINS<> >
class BladeBase *SaviBladePtr() {
  static POWER_PINS power_pins;
  static SaviBlade blade(pin, &power_pins);
  return &blade;
}

#if PROFFIEBOARD_VERSION - 0 >= 3 && !defined(NO_STATUS_LED)
#define SaviBladePtr please_disable_status_led_to_use_savi_blade
#endif

#endif
#endif
