#ifndef BLADES_SIMPLE_BLADE_H
#define BLADES_SIMPLE_BLADE_H

template<int PIN, class LED>
class PWMPin : public PWMPinInterface {
public:
  void Activate() override {
    LSanalogWriteSetup(PIN);
    LSanalogWrite(PIN, 0);  // make it black
  }
  void set(const Color16& c) override {
    LSanalogWrite(PIN, led_.PWM(c));
  }
  void set_overdrive(const Color16& c) override {
    LSanalogWrite(PIN, led_.PWM_overdrive(c));
  }

  DriveLogic<LED> led_;
};

template<class LED>
class PWMPin<-1, LED> : PWMPinInterface {
public:
  void Activate() override {}
  void set(const Color16& c) override {}
  void set_overdrive(const Color16& c) override {}
};

template<class ... LEDS>
class MultiChannelLED {};

template<>
class MultiChannelLED<> : public PWMPinInterface {
public:
  void Activate() override {}
  void set(const Color16& c) override {}
  void set_overdrive(const Color16& c) override {}
};

template<class LED, class... LEDS>
class MultiChannelLED<LED, LEDS...> : public PWMPinInterface {
public:
  void Activate() override {
    led_.Activate();
    rest_.Activate();
  }
  void set(const Color16& c) override {
    led_.set(c);
    rest_.set(c);
  }
  void set_overdrive(const Color16& c) override {
    led_.set_overdrive(c);
    rest_.set_overdrive(c);
  }
private:
  LED led_;
  MultiChannelLED<LEDS...> rest_;
};

template<class... LEDS>
class LEDArrayHelper {};

template<>
class LEDArrayHelper<> {
public:
  static const size_t size = 0;
  void InitArray(PWMPinInterface** pos) {}
  void Activate() {}
};

template<class LED, class... LEDS>
class LEDArrayHelper<LED, LEDS...> {
public:
  static const size_t size = LEDArrayHelper<LEDS...>::size + 1;
  void InitArray(PWMPinInterface** pos) {
    *pos = &led_;
    rest_.InitArray(pos + 1);
  }
  void Activate() {
    led_.Activate();
    rest_.Activate();
  }
private:
  LED led_;
  LEDArrayHelper<LEDS...> rest_;
};


// Simple blade, LED string or LED star with optional flash on clash.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
template<class ... LEDS>
class Simple_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  Simple_Blade() :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK) {
    led_structs_.InitArray(leds_);
  }
  const char* name() override { return "Simple_Blade"; }

  void Activate() override {
    STDOUT.println("Simple Blade");
    power_ = true;
    led_structs_.Activate();
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return LEDArrayHelper<LEDS...>::size;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color16 c) override {
    leds_[led]->set(c);
  }

  void set_overdrive(int led, Color16 c) override {
    leds_[led]->set_overdrive(c);
  }

  bool clash() override {
    bool ret = clash_;
    clash_ = false;
    return ret;
  }
  void allow_disable() override {
    if (!on_) power_ = false;
  }

  // SaberBase implementation
  void SB_IsOn(bool *on) override {
    if (on_ || power_) *on = true;
  }
  void SB_On() override {
    battery_monitor.SetLoad(true);
    power_ = on_ = true;
  }
  void SB_Off() override {
    battery_monitor.SetLoad(false);
    on_ = false;
  }
  void SB_Clash() override {
    clash_ = true;
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, "blade")) {
      if (!strcmp(arg, "on")) {
         SB_On();
         return true;
      }
      if (!strcmp(arg, "off")) {
         SB_Off();
         return true;
      }
    }
    return false;
  }

  void Help() override {
    STDOUT.println(" blade on/off - turn simple blade on off");
  }

protected:
  void Loop() override {
    if (!power_) return;
    // Make sure the booster is running so we don't get low voltage
    // and under-drive any FETs.
    EnableBooster();
    current_style_->run(this);
  }

private:
  PWMPinInterface* leds_[LEDArrayHelper<LEDS...>::size];
  LEDArrayHelper<LEDS...> led_structs_;

  bool on_ = false;
  bool power_ = false;
  bool clash_ = false;
};

template<class LED1, class LED2, class LED3, class LED4,
          int pin1 = bladePowerPin1,
          int pin2 = bladePowerPin2,
          int pin3 = bladePowerPin3,
          int pin4 = bladePin>
class BladeBase *SimpleBladePtr() {
  static Simple_Blade<
    MultiChannelLED<
      PWMPin<pin1, LED1>,
      PWMPin<pin2, LED2>,
      PWMPin<pin3, LED3>,
      PWMPin<pin4, LED4>
    >
  > blade;
  return &blade;
}

#if VERSION_MAJOR >= 2
// Possibly one LED driver per channel...
template<class LED, int CLASH_PIN = -1, class CLASH_LED = NoLED>
class BladeBase *StringBladePtr() {
  static Simple_Blade<
    MultiChannelLED<PWMPin<bladePowerPin1, LED>,
                    PWMPin<CLASH_PIN, CLASH_LED> >,
    PWMPin<bladePowerPin2, LED>,
    PWMPin<bladePowerPin3, LED>,
    PWMPin<bladePowerPin4, LED>,
    PWMPin<bladePowerPin5, LED>,
    PWMPin<bladePowerPin6, LED>
  > blade;
  return &blade;
}
#endif

#endif
