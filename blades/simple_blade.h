#ifndef BLADES_SIMPLE_BLADE_H
#define BLADES_SIMPLE_BLADE_H

// Simple blade, LED string or LED star with optional flash on clash.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class Simple_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  Simple_Blade(LEDInterface* c1,
                LEDInterface* c2,
                LEDInterface* c3,
                LEDInterface* c4,
                int pin1,
                int pin2,
                int pin3,
                int pin4) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK) {
    pins_[0].Init(pin1, c1);
    pins_[1].Init(pin2, c2);
    pins_[2].Init(pin3, c3);
    pins_[3].Init(pin4, c4);
  }
  const char* name() override { return "Simple_Blade"; }

  void Activate() override {
    STDOUT.println("Simple Blade");
    power_ = true;
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i].Activate();
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return 1;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color16 c) override {
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i].set(c);
  }

  void set_overdrive(int led, Color16 c) override {
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i].set_overdrive(c);
  }

  bool clash() override {
    bool ret = clash_;
    clash_ = false;
    return ret;
  }
  void allow_disable() override {
    power_ = false;
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
    current_style_->run(this);
  }

private:
  PWMPin pins_[4];
  static bool on_;
  static bool power_;
  bool clash_ = false;
};

bool Simple_Blade::on_ = false;
bool Simple_Blade::power_ = false;

template<class LED1, class LED2, class LED3, class LED4,
          int pin1 = bladePowerPin1,
          int pin2 = bladePowerPin2,
          int pin3 = bladePowerPin3,
          int pin4 = bladePin>
class Simple_Blade *SimpleBladePtr() {
  static Simple_Blade blade(LEDPtr<LED1>(), LEDPtr<LED2>(), LEDPtr<LED3>(), LEDPtr<LED4>(), pin1, pin2, pin3, pin4);
  return &blade;
}

#endif
