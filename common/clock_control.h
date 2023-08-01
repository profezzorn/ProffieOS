#ifndef COMMON_CLOCK_CONTROL_H
#define COMMON_CLOCK_CONTROL_H

#if VERSION_MAJOR >= 4

#include "looper.h"

class ClockControl : public Looper {
public:
  const char* name() override { return "ClockControl"; }
  void Loop() override {
    bool on = false;
    SaberBase::DoIsOn(&on);
    if (on
	|| Serial
	|| prop.NeedsPower()
	|| USBD_Connected()
	|| stm32l4_gpio_pin_read(GPIO_PIN_PB2)
#ifdef ENABLE_AUDIO
	|| amplifier.Active()
#endif
      ) {
      last_activity_ = millis();
    }
    // These two variables must be read in order.
    uint32_t last_activity = last_activity_;
    uint32_t now = millis();
    if (now - last_activity > 30000) {
#if 0 // #ifdef PROFFIEBOARD_VERSION
      // This saves power, but also casuses freezing.
      // TODO: FIgure out why and re-enable.
      stm32l4_system_sysclk_configure(1000000, 500000, 500000);
#else
      stm32l4_system_sysclk_configure(16000000, 8000000, 8000000);
#endif
#ifdef COMMON_I2CBUS_H
      // Motion and other things might still be going on.
      if (i2cbus.used())
        delay(5);
      else
#endif
        delay(50);
      stm32l4_system_sysclk_configure(_SYSTEM_CORE_CLOCK_, _SYSTEM_CORE_CLOCK_/2, _SYSTEM_CORE_CLOCK_/2);
    }
  }

  void AvoidSleep() { last_activity_ = millis(); }
    
  private:
    volatile uint32_t last_activity_;
};

ClockControl clock_control;

void ClockControl_AvoidSleep() { clock_control.AvoidSleep(); }

#else

void ClockControl_AvoidSleep() { }

#endif

#endif
