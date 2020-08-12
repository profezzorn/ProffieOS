#ifndef BLADES_DRIVE_LOGIC_H
#define BLADES_DRIVE_LOGIC_H

#include "led_interface.h"

template<typename T> struct ToVoid { typedef void Type; };

template<class LED, typename Enable=void>
struct ColorSelector {
  static Color8 getColor8() {
    return Color8(LED::Red, LED::Green, LED::Blue);
  }
  static int Select(Color16 c) {
    return c.select(Color16(getColor8()));
  }
};

// This allows for engergy-efficient RGBW setups
template<class LED>
struct ColorSelector<LED, typename ToVoid<typename LED::SUBTRACT>::Type>  {
  static Color8 getColor8() {
    return Color8(LED::Red, LED::Green, LED::Blue);
  }
  static int Select(Color16 c) {
    ColorSelector<typename LED::SUBTRACT> subtract_selector;
    int white = subtract_selector.Select(c);
    return Color16(c.r - white, c.g - white, c.b - white).select(Color16(getColor8()));
  }
};

// This allows for an easy way to override color selection (but not the overdrive logic)
template<class LED>
struct ColorSelector<LED, typename ToVoid<typename LED::CustomColorSelector>::Type> {
  static Color8 getColor8() {
    return Color8(LED::Red, LED::Green, LED::Blue);
  }
  static int Select(Color16 c) {
    typename LED::CustomColorSelector selector;
    return selector.Select(c);
  }
};

// This code turns down the PWM duty cycle when the battery voltage
// is too high for the LED. We use a simple affine model to approximate
// the amp/volt curve of the LED. This model requires that we know
// two amp/volt points to draw a line through. The higher of those two
// points also doubles as the maximum amps that the LED will
// tolerate. So we use the model to calculate what the amps would be
// given the current battery voltage, if the amps is higher than
// the maximum then we adjust the PWM rate so that the average amps
// equals the maximum amps.
template<class LED, class Enable=void>
class DriveLogic : public LEDInterface {
public:
  float PWMMultiplier() {
    float V = battery_monitor.battery();
    float dv = LED::MaxVolts - LED::P2Volts;
    float di = LED::MaxAmps - LED::P2Amps;
    float delta = dv / di;
    float amps = (V - LED::MaxVolts + LED::MaxAmps * delta) / (delta + LED::R);
    if (monitor.ShouldPrintMultiple(Monitoring::MonitorPWM)) {
      STDOUT << " V=" << V
	     << " A=" << amps
	     << " mult=" << (LED::MaxAmps / amps);
    }
    if (amps <= LED::MaxAmps) {
      return 1.0f;
    }

    return LED::MaxAmps / amps;
  }

  int PWM_overdrive(Color16 c) override {
    return ColorSelector<LED>::Select(c);
  }
  int PWM(Color16 c) override {
    int pwm = PWM_overdrive(c);
    float mult = PWMMultiplier();
    if (monitor.ShouldPrintMultiple(Monitoring::MonitorPWM)) {
      STDOUT << " color=" << c
             << " channel=" << getColor8()
             << " sel=" << pwm
             << " pwm=" << (int)(pwm * mult)
	     << "\n";
    }
    return pwm * mult;
  }
  Color8 getColor8() const {
    return ColorSelector<LED>::getColor8();
  }
};

// This allows LED class to override the entire drive logic.
template<class LED>
class DriveLogic<LED, typename ToVoid<typename LED::CustomDriveLogic>::Type> : public LEDInterface {
public:
  int PWM_overdrive(Color16 c) override { return drive_.PWM_overdrive(c); }
  int PWM(Color16 c) override { return drive_.PWM(c); }
private:
  typename LED::CustomDriveLogic drive_;
};


template<class LED>
class LEDInterface* LEDPtr() {
  static DriveLogic<LED> led;
  return &led;
}

#endif
