#ifndef BLADES_DRIVE_LOGIC_H
#define BLADES_DRIVE_LOGIC_H

#include "led_interface.h"

// This code turns down the PWM duty cycle when the battery voltage
// is too high for the LED. We use a simple affine model to approximate
// the amp/volt curve of the LED. This model requires that we know
// two amp/volt points to draw a line through. The higher of those two
// points also doubles as the maximum amps that the LED will
// tolerate. So we use the model to calculate what the amps would be
// given the current battery voltage, if the amps is higher than
// the maximum then we adjust the PWM rate so that the average amps
// equals the maximum amps.
template<class LED>
class DriveLogic : public LEDInterface {
public:
  float PWMMultiplier() {
    float V = battery_monitor.battery();
    float dv = LED::MaxVolts - LED::P2Volts;
    float di = LED::MaxAmps - LED::P2Amps;
    float delta = dv / di;
    float amps = (V - LED::MaxVolts + LED::MaxAmps * delta) / (delta + LED::R);
    if (monitor.ShouldPrint(Monitoring::MonitorPWM)) {
      STDOUT.print("AMPS = ");
      STDOUT.print(amps);
      STDOUT.print(" / ");
      STDOUT.print(LED::MaxAmps);
      STDOUT.print(" PWM = ");
      STDOUT.println(100.0 * LED::MaxAmps / amps);
    }
    if (amps <= LED::MaxAmps) {
      return 1.0f;
    }

    return LED::MaxAmps / amps;
  }
  int PWM(Color16 c) override {
    return c.select(Color16(Color8(LED::Red, LED::Green, LED::Blue))) * PWMMultiplier();
  }
  int PWM_overdrive(Color16 c) override {
    return c.select(Color16(Color8(LED::Red, LED::Green, LED::Blue)));
  }
};

template<class LED>
class LEDInterface* LEDPtr() {
  static DriveLogic<LED> led;
  return &led;
}

#endif
