#ifndef STYLES_CHARGING_H
#define STYLES_CHARGING_H

// Usage: &style_charging
// return value: POINTER
// Charging blade style.
// Slowly pulsating battery indicator.

#include "blade_style.h"

class StyleCharging : public BladeStyle {
public:
  void activate() override {
    STDOUT.println("Charging Style");
  }
  void run(BladeBase *blade) override {
    int black_mix = 128 + 100 * sinf(millis() / 500.0);
    float volts = battery_monitor.battery();
    Color8 colors[] = {
      Color8(0,255,0),   // Green > 4.0
      Color8(0,0,255),
      Color8(255,128,0),
      Color8(255,0,0),
      Color8(255,0,0) 
    };
    float x = (4.0 - volts) * 2.0;
    int i = floorf(x);
    i = clampi32(i, 0, NELEM(colors) - 2);
    // Blend colors over 0.1 volts.
    int blend = (x - i) * 10 * 255;
    blend = clampi32(blend, 0, 255);
    Color8 c = colors[i].mix(colors[i + 1], blend);
    c = c.mix(Color8(), black_mix);
    int num_leds = blade->num_leds();

    float min_volts = 2.7;
    float max_volts = 4.2;
    float pos = (volts - min_volts) * num_leds / (max_volts - min_volts);
    int p = pos * 32;
    for (int i = 0; i < num_leds; i++) {
      blade->set(i, Color16(Color8().mix(c, std::max(0, 256 - abs(p - i * 32)))));
    }
  };

  bool NoOnOff() override { return true; }
  bool Charging() override { return true; }
  bool IsHandled(HandledFeature effect) override { return false; }
};
// No need to templetize this one, as there are no arguments.
StyleFactoryImpl<StyleCharging> style_charging;

#endif
