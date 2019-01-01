#ifndef BLADES_WS2811_BLADE_H
#define BLADES_WS2811_BLADE_H

#include "abstract_blade.h"

#ifdef ENABLE_WS2811
#include "stm32l4_ws2811.h"

// WS2811-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class WS2811_Blade : public AbstractBlade, CommandParser, Looper {
public:
  WS2811_Blade(int num_leds,
	       int data_pin,
	       WS2811Pin::Byteorder byteorder,
	       int frequency,
	       int reset_us,
	       int t0h_us,
	       int t1h_us,
	       PowerPinInterface* power) :
    AbstractBlade(),
    CommandParser(NOLINK),
    Looper(NOLINK),
    power_(power) {
    pin_.begin(data_pin, num_leds, byteorder, frequency, reset_us, t0h_us, t1h_us);
  }
  const char* name() override { return "STM32L4_WS2811_Blade"; }

  void Power(bool on) {
    while (!pin_.IsReadyForEndFrame());
    pinMode(pin_.pin(), on ? OUTPUT : INPUT);
    power_->Power(on);
    powered_ = on;
    allow_disable_ = false;
    if (on) EnableBooster();
  }

  // No need for a "deactivate", the blade stays active until
  // you take it out, which also cuts the power.
  void Activate() override {
    STDOUT.print("WS2811 Blade with ");
    STDOUT.print(pin_.num_leds());
    STDOUT.println(" leds.");
    power_->Init();
    Power(true);
    delay(10);
    pin_.BeginFrame();
    for (int i = 0; i < pin_.num_leds(); i++) pin_.Set(i, Color16());
    pin_.EndFrame();
    pin_.BeginFrame();
    pin_.EndFrame();
    pin_.BeginFrame();
    pin_.EndFrame();
    CommandParser::Link();
    Looper::Link();
    AbstractBlade::Activate();
  }

  // BladeBase implementation
  int num_leds() const override {
    return pin_.num_leds();
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color16 c) override {
    color_buffer[led] = c;
  }
  void allow_disable() override {
    if (!on_) allow_disable_ = true;
  }
  virtual void SetStyle(BladeStyle* style) {
    AbstractBlade::SetStyle(style);
    Power(true);
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
    STATE_MACHINE_BEGIN();
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
      while (!pin_.IsReadyForBeginFrame()) YIELD();
      pin_.BeginFrame();
      for (int i = 0; i < pin_.num_leds(); i++) {
	pin_.Set(i, color_buffer[i]);
	if (!(i & 0x1f)) Looper::DoHFLoop();
      }
      while (!pin_.IsReadyForEndFrame()) YIELD();
      pin_.EndFrame();
      loop_counter_.Update();
      current_blade = NULL;
      YIELD();
    }
    STATE_MACHINE_END();
  }
  
private:
  bool on_ = false;
  bool powered_ = false;
  bool allow_disable_ = false;
  LoopCounter loop_counter_;

  static Color16 color_buffer[maxLedsPerStrip];
  static WS2811_Blade* current_blade;
  StateMachineState state_machine_;
  PowerPinInterface* power_;
  WS2811Pin pin_;
};

Color16 WS2811_Blade::color_buffer[maxLedsPerStrip];
WS2811_Blade* WS2811_Blade::current_blade = NULL;

#define WS2811_RGB      0       // The WS2811 datasheet documents this way
#define WS2811_RBG      1
#define WS2811_GRB      2       // Most LED strips are wired this way
#define WS2811_GBR      3

#define WS2811_800kHz 0x00      // Nearly all WS2811 are 800 kHz
#define WS2811_400kHz 0x10      // Adafruit's Flora Pixels
#define WS2813_800kHz 0x20      // WS2813 are close to 800 kHz but has 300 us frame set delay
#define WS2811_580kHz 0x30      // PL9823
#define WS2811_ACTUALLY_800kHz 0x40      // Normally we use 740kHz instead of 800, this uses 800.

template<int LEDS, int CONFIG, int DATA_PIN = bladePin, class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3> >
class WS2811_Blade *WS2811BladePtr() {
  static_assert(LEDS <= maxLedsPerStrip, "update maxLedsPerStrip");
  static POWER_PINS power_pins;
  typename WS2811Pin::Byteorder byteorder = WS2811Pin::BGR;
  switch (CONFIG & 0xf) {
    case WS2811_RGB: byteorder = WS2811Pin::RGB; break;
    case WS2811_RBG: byteorder = WS2811Pin::RBG; break;
    case WS2811_GRB: byteorder = WS2811Pin::GRB; break;
    case WS2811_GBR: byteorder = WS2811Pin::GBR; break;
  }

  int frequency = 800000;
  int reset_us = 300;
  switch (CONFIG & 0xf0) {
    case WS2811_800kHz:
      frequency = 740000;
      break;
    case WS2811_400kHz:
      frequency = 400000;
      break;
    case WS2813_800kHz:
    case WS2811_ACTUALLY_800kHz:
      frequency = 800000;
      break;
  }
    
  static WS2811_Blade blade(LEDS, DATA_PIN, byteorder,
			    frequency, reset_us,
			    294, 862, &power_pins);
  return &blade;
}
#endif

#endif
