#ifndef BLADES_WS2811_BLADE_H
#define BLADES_WS2811_BLADE_H

#ifdef ENABLE_WS2811
#ifdef TEENSYDUINO
#include "monopodws.h"
#else
#include "stm32_ws2811.h"
#endif

// WS2811-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class WS2811_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  WS2811_Blade(int num_leds, uint8_t config, int pin, PowerPinInterface* power) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK),
    num_leds_(num_leds),
    config_(config),
    pin_(pin),
    power_(power) {
  }
  const char* name() override { return "WS2811_Blade"; }

  void Power(bool on) {
    while (monopodws.busy());
    power_->Power(on);
//    pinMode(bladePin, on ? OUTPUT : INPUT);
    powered_ = on;
    allow_disable_ = false;
  }

  // No need for a "deactivate", the blade stays active until
  // you take it out, which also cuts the power.
  void Activate() override {
    STDOUT.print("WS2811 Blade with ");
    STDOUT.print(num_leds_);
    STDOUT.println(" leds");
    power_->Init();
    Power(true);
    delay(10);
    while (monopodws.busy());
    monopodws.begin(num_leds_, displayMemory, config_, pin_);
    monopodws.show();  // Make it black
    monopodws.show();  // Make it black
    monopodws.show();  // Make it black
    while (monopodws.busy());
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return num_leds_;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color16 c) override {
    monopodws.setPixel(led, c.dither(0));
  }
  bool clash() override {
    bool ret = clash_;
    clash_ = false;
    return ret;
  }
  void allow_disable() override {
    if (!on_) allow_disable_ = true;
  }

  // SaberBase implementation.
  void SB_IsOn(bool* on) override {
    if (on_) *on = true;
  }
  void SB_On() override {
    Power(true);
    delay(10);
    on_ = true;
  }
  void SB_Off() override {
    on_ = false;
  }

  void SB_Clash() override { clash_=true; }

  void SB_Top() override {
    STDOUT.print("blade fps: ");
    loop_counter_.Print();
    STDOUT.println("");
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
    STDOUT.println(" blade on/off - turn ws2811 blade on off");
  }

protected:
  void Loop() override {
    STATE_MACHINE_BEGIN()
    while (true) {
        while (!powered_ || !current_style_) {
          loop_counter_.Reset();
          YIELD();
        }
        // Wait until it's our turn.
        while (current_blade) YIELD();
        if (allow_disable_) {
          Power(on_);
          continue;
        }
        current_blade = this;
        current_style_->run(this);
        while (monopodws.busy()) YIELD();
#if NUM_BLADES > 1
        monopodws.begin(num_leds_, displayMemory, config_, pin_);
#endif
        monopodws.show();
        loop_counter_.Update();
        current_blade = NULL;
        YIELD();
    }
    STATE_MACHINE_END();
  }

private:
  int num_leds_;
  uint8_t config_;
  uint8_t pin_;
  bool on_ = false;
  bool powered_ = false;
  bool clash_ = false;
  bool allow_disable_ = false;
  LoopCounter loop_counter_;

  static WS2811_Blade* current_blade;
  StateMachineState state_machine_;
  PowerPinInterface* power_;
};

WS2811_Blade* WS2811_Blade::current_blade = NULL;

template<int LEDS, int CONFIG, int DATA_PIN = bladePin, class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3> >
class WS2811_Blade *WS2811BladePtr() {
  static_assert(LEDS <= maxLedsPerStrip, "update maxLedsPerStrip");
  static POWER_PINS power_pins;
  static WS2811_Blade blade(LEDS, CONFIG, DATA_PIN, &power_pins);
  return &blade;
}
#endif

#endif
