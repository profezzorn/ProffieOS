#ifndef BLADES_WS2811_BLADE_H
#define BLADES_WS2811_BLADE_H

#include "abstract_blade.h"

#ifdef ENABLE_WS2811

// Common
DMAMEM int displayMemory[maxLedsPerStrip * 24 / 4 + 1];

#if VERSION_MAJOR >= 4
#include "stm32l4_ws2811.h"
#define DefaultPinClass WS2811Pin
#else
#include "monopodws.h"
#include "ws2811_serial.h"
#define DefaultPinClass MonopodWSPin
#endif

Color16 color_buffer[maxLedsPerStrip];
BladeBase* current_blade = NULL;

// WS2811-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
template<class WS2811PIN>
class WS2811_Blade : public AbstractBlade, CommandParser, Looper {
public:
  WS2811_Blade(int num_leds,
	       int data_pin,
	       Color8::Byteorder byteorder,
	       int frequency,
	       int reset_us,
	       int t0h_us,
	       int t1h_us,
	       PowerPinInterface* power,
	       uint32_t poweroff_delay_ms) :
    AbstractBlade(),
    CommandParser(NOLINK),
    Looper(NOLINK),
    poweroff_delay_ms_(poweroff_delay_ms),
    power_(power),
    pin_(data_pin, num_leds, byteorder, frequency, reset_us, t0h_us, t1h_us) {
    }
  const char* name() override { return "WS2811_Blade"; }

  void Power(bool on) {
    if (on) EnableBooster();
    if (!powered_ && on) {
      power_->Init();
      TRACE("Power on");
      pinMode(pin_.pin(), OUTPUT);
      pin_.BeginFrame();
      for (int i = 0; i < pin_.num_leds(); i++) pin_.Set(i, Color8());
      while (!pin_.IsReadyForEndFrame());
      power_->Power(on);
      pin_.EndFrame();
      pin_.BeginFrame();
      pin_.EndFrame();
      pin_.BeginFrame();
      pin_.EndFrame();
      current_blade = NULL;
    } else if (powered_ && !on) {
      TRACE("Power off");
      pin_.BeginFrame();
      for (int i = 0; i < pin_.num_leds(); i++) pin_.Set(i, Color8());
      while (!pin_.IsReadyForEndFrame());
      pin_.EndFrame();
      while (!pin_.IsReadyForEndFrame());
      power_->Power(on);
      pinMode(pin_.pin(), INPUT_ANALOG);
      power_->DeInit();
      current_blade = NULL;
    }
    powered_ = on;
    allow_disable_ = false;
  }

  void Activate() override {
    TRACE("Activate");
    STDOUT.print("WS2811 Blade with ");
    STDOUT.print(pin_.num_leds());
    STDOUT.println(" leds.");
    Power(true);
    CommandParser::Link();
    Looper::Link();
    AbstractBlade::Activate();
  }

  void Deactivate() override {
    TRACE("Deactivate");
    Power(false);
    CommandParser::Unlink();
    Looper::Unlink();
    AbstractBlade::Deactivate();
  }
  // BladeBase implementation
  int num_leds() const override {
    return pin_.num_leds();
  }
  Color8::Byteorder get_byteorder() const {
    return pin_.get_byteorder();
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
  void SetStyle(BladeStyle* style) override{
    TRACE("SetStyle");
    AbstractBlade::SetStyle(style);
    Power(true);
  }
  BladeStyle* UnSetStyle() override {
    TRACE("UnSetStyle");
    return AbstractBlade::UnSetStyle();
  }
  // SaberBase implementation.
  void SB_IsOn(bool* on) override {
    if (on_ || powered_) *on = true;
  }
  void SB_On() override {
    TRACE("SB_On");
    AbstractBlade::SB_On();
    Power(true);
    on_ = true;
    power_off_requested_ = false;
  }
  void SB_Off(OffType off_type) override {
    TRACE("SB_Off");
    AbstractBlade::SB_Off(off_type);
    on_ = false;
    if (off_type == OFF_IDLE) {
      power_off_requested_ = true;
    }
  }

  void SB_Top(uint64_t total_cycles) override {
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
         SB_Off(OFF_NORMAL);
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
      if (allow_disable_ || power_off_requested_) {
	if (!on_) {
	  if (!poweroff_delay_start_) {
	    poweroff_delay_start_ = millis();
	  }
	  if (millis() - poweroff_delay_start_ < poweroff_delay_ms_) {
	    YIELD();
	    continue;
	  }
	  poweroff_delay_start_ = 0;
	}
	Power(on_);
	continue;
      }
      current_blade = this;
      current_style_->run(this);
      while (!pin_.IsReadyForBeginFrame()) YIELD();
      // If Power() was called....
      if (current_blade != this) continue;
      pin_.BeginFrame();
      for (int i = 0; i < pin_.num_leds(); i++) {
	pin_.Set(i, color_buffer[i]);
	if (!(i & 0x1f)) Looper::DoHFLoop();
      }
      while (!pin_.IsReadyForEndFrame()) YIELD();
      // If Power() was called....
      if (current_blade != this) continue;
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
  bool power_off_requested_ = false;
  uint32_t poweroff_delay_ms_;
  uint32_t poweroff_delay_start_ = 0;
  LoopCounter loop_counter_;
  StateMachineState state_machine_;
  PowerPinInterface* power_;
  WS2811PIN pin_;
};


#define WS2811_RGB      0       // The WS2811 datasheet documents this way
#define WS2811_RBG      1
#define WS2811_GRB      2       // Most LED strips are wired this way
#define WS2811_GBR      3

#define WS2811_800kHz 0x00      // Nearly all WS2811 are 800 kHz
#define WS2811_400kHz 0x10      // Adafruit's Flora Pixels
#define WS2813_800kHz 0x20      // WS2813 are close to 800 kHz but has 300 us frame set delay
#define WS2811_580kHz 0x30      // PL9823
#define WS2811_ACTUALLY_800kHz 0x40      // Normally we use 740kHz instead of 800, this uses 800.

template<int LEDS, int CONFIG, int DATA_PIN = bladePin, class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>,
          class PinClass = DefaultPinClass,
  int reset_us=300, int t1h=294, int t0h=892,
  int POWER_OFF_DELAY_MS=0>
class BladeBase *WS2811BladePtr() {
  static_assert(LEDS <= maxLedsPerStrip, "update maxLedsPerStrip");
  static POWER_PINS power_pins;
  typename Color8::Byteorder byteorder = Color8::BGR;
  switch (CONFIG & 0xf) {
    case WS2811_RGB: byteorder = Color8::RGB; break;
    case WS2811_RBG: byteorder = Color8::RBG; break;
    case WS2811_GRB: byteorder = Color8::GRB; break;
    case WS2811_GBR: byteorder = Color8::GBR; break;
  }

  int frequency = 800000;
  switch (CONFIG & 0xf0) {
    case WS2811_800kHz:
      frequency = 740000;
      break;
    case WS2811_400kHz:
      frequency = 400000;
      break;
    case WS2811_580kHz:
      frequency = 580000;
      break;
    case WS2813_800kHz:
    case WS2811_ACTUALLY_800kHz:
      frequency = 800000;
      break;
  }
    
  static WS2811_Blade<PinClass> blade(LEDS, DATA_PIN, byteorder,
				      frequency, reset_us,
				      t1h, t0h, &power_pins,
				      POWER_OFF_DELAY_MS);
  return &blade;
}

template<int LEDS,
          int DATA_PIN = bladePin,
          Color8::Byteorder byteorder,
          class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>,
          class PinClass = DefaultPinClass,
          int frequency=800000, int reset_us=300, int t1h=294, int t0h=892,
          int POWER_OFF_DELAY_MS = 0>
class BladeBase *WS281XBladePtr() {
  static POWER_PINS power_pins;
  static WS2811_Blade<PinClass> blade(LEDS, DATA_PIN, byteorder,
				      frequency, reset_us,
				      t1h, t0h, &power_pins,
				      POWER_OFF_DELAY_MS);
  return &blade;
}

#endif  // ENABLE_WS2811

#endif
