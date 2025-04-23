#ifndef STYLES_BLACKPOWER_H
#define STYLES_BLACKPOWER_H

#include "blade_style.h"

// Usage:
// Provides blade style that keeps pogo
// pins live for Blade Plug Charging.
// Use with ChargingStylePtr to prevent
// IDLE_OFF_TIME globally shutting down
// LED power pads.

class BlackPower : public BladeStyle {
public:
    void run(BladeBase* base) override {
    }

    OverDriveColor getColor(int led) override {
      return OverDriveColor(Color16(0, 0, 0));
    }

    bool NoOnOff() override {
      return true;  // Pins stay live for blade plug charging.
    }

    bool IsHandled(HandledFeature effect) override {
      return false;
    }
  };

StyleFactoryImpl<BlackPower> black_power;

#endif
