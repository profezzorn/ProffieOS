#ifndef BLADES_STRING_BLADE_H
#define BLADES_STRING_BLADE_H

#ifdef V2
// String blade, Segment LED string. All segments assumed to be the same color.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
#define STRING_SEGMENTS 6
class String_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  String_Blade(LEDInterface* c, int clash_pin, LEDInterface* clash_led) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK),
    c_(c) {
    clash_pin_.Init(clash_pin, clash_led);
  }
  const char* name() override { return "String_Blade"; }

  void Activate() override {
    STDOUT.println("String Blade");
    analogWriteResolution(16);
    for (int i = 0; i < STRING_SEGMENTS; i++) {
      analogWriteFrequency(pin_[i], 1000);
      analogWrite(pin_[i], 0);  // make it black
    }
    clash_pin_.Activate();
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return STRING_SEGMENTS;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color16 c) override {
    analogWrite(pin_[led], c_->PWM(c));
    if (led == 0) clash_pin_.set(c);
  }

  void set_overdrive(int led, Color16 c) override {
    analogWrite(pin_[led], c_->PWM_overdrive(c));
    if (led == 0) clash_pin_.set_overdrive(c);
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
    if (on_) *on = true;
  }
  void SB_On() override {
    battery_monitor.SetLoad(false);
    power_ = on_ = true;
  }
  void SB_Off() override {
    battery_monitor.SetLoad(true);
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
    STDOUT.println(" blade on/off - turn string blade on off");
  }

protected:
  void Loop() override {
    if (!power_) return;
    current_style_->run(this);
  }

private:
  LEDInterface *c_;
  PWMPin clash_pin_;
  static int pin_[STRING_SEGMENTS];
  static bool on_;
  static bool power_;
  static bool clash_;
};

int String_Blade::pin_[STRING_SEGMENTS] = {
    bladePowerPin1,
    bladePowerPin2,
    bladePowerPin3,
    bladePowerPin4,
    bladePowerPin5,
    bladePowerPin6,
};
bool String_Blade::on_ = false;
bool String_Blade::power_ = false;
bool String_Blade::clash_ = true;

// Possibly one LED driver per channel...
template<class LED, int CLASH_PIN = -1, class CLASH_LED = NoLED>
class String_Blade *StringBladePtr() {
  static String_Blade blade(LEDPtr<LED>(), CLASH_PIN, LEDPtr<CLASH_LED>());
  return &blade;
}
#endif

#endif
