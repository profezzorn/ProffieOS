#ifndef BLADES_WS2811_BLADE_H
#define BLADES_WS2811_BLADE_H

#include "abstract_blade.h"

#ifdef ENABLE_WS2811

#ifndef EXTRA_COLOR_BUFFER_SPACE
#define EXTRA_COLOR_BUFFER_SPACE 0
#endif

Color16 color_buffer[maxLedsPerStrip + EXTRA_COLOR_BUFFER_SPACE];
BladeBase* current_blade = NULL;

class WS2811PIN {
public:
  virtual bool IsReadyForBeginFrame() = 0;
  virtual Color16 *BeginFrame() = 0;
  virtual bool IsReadyForEndFrame() = 0;
  virtual void WaitUntilReadyForEndFrame();
  virtual void EndFrame() = 0;
  virtual int num_leds() const = 0;
  virtual Color8::Byteorder get_byteorder() const = 0;
  virtual void Enable(bool enable) = 0;
  virtual int pin() const = 0;
};

#ifdef ARDUINO_ARCH_STM32L4

// Common, size adjusted to ~2000 interrupts per second.
DMAMEM uint32_t displayMemory[200];
#include "stm32l4_ws2811.h"
#define DefaultPinClass WS2811Pin
#define ProffieOS_yield() armv7m_core_yield()

#endif // STM32

#ifdef TEENSYDUINO
// Common
DMAMEM int displayMemory[maxLedsPerStrip * 24 / 4 + 1];
#ifdef USE_TEENSY4
#include "teensy4_ws2811.h"
#define DefaultPinClass OctopodWSPin
#elif defined(TEENSYDUINO)
#include "monopodws.h"
#include "ws2811_serial.h"
#define DefaultPinClass MonopodWSPin
#endif
#define ProffieOS_yield() do { } while(0)

#endif  // TEENSYDUINO

#ifdef ESP32
#include "esp32_ws2811.h"
#define DefaultPinClass RMTWSPin
#define ProffieOS_yield() yield()
#endif


#ifndef DefaultPinClass
#error WS2811 not supported on this board
#endif

#include "spiled_pin.h"

void WS2811PIN::WaitUntilReadyForEndFrame() {
  while (!IsReadyForEndFrame()) ProffieOS_yield();
}

// WS2811-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class WS2811_Blade : public AbstractBlade, CommandParser, Looper {
public:
WS2811_Blade(WS2811PIN* pin,
             PowerPinInterface* power,
             uint32_t poweroff_delay_ms) :
    AbstractBlade(),
    CommandParser(NOLINK),
    Looper(NOLINK),
    poweroff_delay_ms_(poweroff_delay_ms),
    power_(power),
    pin_(pin) {
    }
  const char* name() override { return "WS2811_Blade"; }

  void Power(bool on) {
    if (on) EnableBooster();
    if (!powered_ && on) {
      power_->Init();
      TRACE(BLADE, "Power on");
      pin_->Enable(true);
      colors_ = pin_->BeginFrame();
      for (int i = 0; i < pin_->num_leds(); i++) set(i, Color16());
      pin_->WaitUntilReadyForEndFrame();
      power_->Power(on);
      pin_->EndFrame();
      colors_ = pin_->BeginFrame();
      for (int i = 0; i < pin_->num_leds(); i++) set(i, Color16());
      pin_->EndFrame();
      colors_ = pin_->BeginFrame();
      for (int i = 0; i < pin_->num_leds(); i++) set(i, Color16());
      pin_->EndFrame();
      current_blade = NULL;
    } else if (powered_ && !on) {
      TRACE(BLADE, "Power off");
      colors_ = pin_->BeginFrame();
      for (int i = 0; i < pin_->num_leds(); i++) set(i, Color16());
      pin_->EndFrame();
      // Wait until it's sent before powering off.
      pin_->WaitUntilReadyForEndFrame();
      power_->Power(on);
      pin_->Enable(false);
      power_->DeInit();
      current_blade = NULL;
    }
    powered_ = on;
    allow_disable_ = false;
  }

  void Activate() override {
    TRACE(BLADE, "Activate");
    STDOUT.print("WS2811 Blade with ");
    STDOUT.print(pin_->num_leds());
    STDOUT.println(" leds.");
    run_ = true;
    CommandParser::Link();
    Looper::Link();
    AbstractBlade::Activate();
  }

  void Deactivate() override {
    TRACE(BLADE, "Deactivate");
    Power(false);
    CommandParser::Unlink();
    Looper::Unlink();
    AbstractBlade::Deactivate();
  }
  // BladeBase implementation
  int num_leds() const override {
    return pin_->num_leds();
  }
  Color8::Byteorder get_byteorder() const {
    return pin_->get_byteorder();
  }
  bool is_on() const override {
    return on_;
  }
  bool is_powered() const override {
    return powered_;
  }
  void set(int led, Color16 c) override {
    Color16* pos = colors_ + led;
    if (colors_ >= color_buffer && colors_ < color_buffer + NELEM(color_buffer) &&
	pos >= color_buffer + NELEM(color_buffer)) pos -= NELEM(color_buffer);
    *pos = c;
  }
  void allow_disable() override {
    if (!on_) allow_disable_ = true;
  }
  void SetStyle(BladeStyle* style) override{
    TRACE(BLADE, "SetStyle");
    AbstractBlade::SetStyle(style);
    run_ = true;
  }
  BladeStyle* UnSetStyle() override {
    TRACE(BLADE, "UnSetStyle");
    return AbstractBlade::UnSetStyle();
  }
  // SaberBase implementation.
  void SB_IsOn(bool* on) override {
    if (on_ || powered_) *on = true;
  }
  void SB_On() override {
    TRACE(BLADE, "SB_On");
    AbstractBlade::SB_On();
    run_ = true;
    on_ = true;
    power_off_requested_ = false;
    poweroff_delay_start_ = 0;
  }
  void SB_Effect2(BladeEffectType type, float location) override {
    AbstractBlade::SB_Effect2(type, location);
    run_ = true;
    power_off_requested_ = false;
    poweroff_delay_start_ = 0;
  }
  void SB_Off(OffType off_type) override {
    TRACE(BLADE, "SB_Off");
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
#ifdef ENABLE_DEVELOPER_COMMANDS      
      if (!strcmp(arg, "state")) {
	STDOUT << "WS2811 blade: on=" << on_
	       << " power=" << powered_
	       << " run=" << run_
	       << " allow_disable=" << allow_disable_
	       << " power_off_requested=" << power_off_requested_
	       << "\n";
      }
#endif      
    }
    return false;
  }

  void PowerOff() {
    if (!poweroff_delay_start_) {
      poweroff_delay_start_ = millis();
    }
    if (millis() - poweroff_delay_start_ < poweroff_delay_ms_) {
      return;
    }
    Power(false);
    run_ = false;
    power_off_requested_ = false;
    poweroff_delay_start_ = 0;
  }

#define BLADE_YIELD() do {			\
  YIELD();					\
  /* If Power() was called.... */		\
  if (current_blade != this) goto retry;	\
} while(0)
			     
protected:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
    retry:
      if (current_blade == this) current_blade = NULL;
      YIELD();
      if (!current_style_ || !run_) {
	loop_counter_.Reset();
#ifdef BLADE_ID_SCAN_MILLIS
	if (pin_->pin() == bladeIdentifyPin && ScanBladeIdNow()) {
	  pin_->Enable(powered_);
	  SLEEP(1);
	}
#endif      
	continue;
      }
      // Wait until it's our turn.
      if (current_blade) {
	continue;
      }
      current_blade = this;
      if (power_off_requested_) {
	PowerOff();
	continue;
      }

      // Update pixels
      while (!pin_->IsReadyForBeginFrame()) BLADE_YIELD();
      colors_ = pin_->BeginFrame();
      
      allow_disable_ = false;
      if (current_style_)
	current_style_->run(this);

      if (!powered_) {
	if (allow_disable_) {
	  run_ = false;
	  continue;
	}
	Power(true);
      }

      while (!pin_->IsReadyForEndFrame()) BLADE_YIELD();
#ifdef BLADE_ID_SCAN_MILLIS
      if (pin_->pin() == bladeIdentifyPin && ScanBladeIdNow()) {
	pin_->Enable(powered_);
	SLEEP(1);
	if (current_blade != this) goto retry;
      }
#endif      
      pin_->EndFrame();
      loop_counter_.Update();

      if (powered_ && allow_disable_) {
	power_off_requested_ = true;
	PowerOff();
      }
    }
    STATE_MACHINE_END();
  }

#ifdef ENABLE_DEBUG
  void LoopDebug() override {
    STDERR << "stuck somewhere after: " << state_machine_.next_state_ << "\n";
  }
#endif

private:
  // Loop should run.
  bool run_ = false;
  // Blade is "on"
  bool on_ = false;
  // Blade has power
  bool powered_ = false;
  // Style has indicated that it's ok to shutd down until the next wakeup event.
  bool allow_disable_ = false;
  // We should power off and stop running, even if the blade is on.
  bool power_off_requested_ = false;
  uint32_t poweroff_delay_ms_;
  uint32_t poweroff_delay_start_ = 0;
  LoopCounter loop_counter_;
  StateMachineState state_machine_;
  PowerPinInterface* power_;
  WS2811PIN* pin_;
  Color16* colors_;

};




#ifndef WS2811_GBR

#define WS2811_RGB      0       // The WS2811 datasheet documents this way
#define WS2811_RBG      1
#define WS2811_GRB      2       // Most LED strips are wired this way
#define WS2811_GBR      3

#define WS2811_800kHz 0x00      // Nearly all WS2811 are 800 kHz
#define WS2811_400kHz 0x10      // Adafruit's Flora Pixels
#define WS2813_800kHz 0x20      // WS2813 are close to 800 kHz but has 300 us frame set delay
#define WS2811_580kHz 0x30      // PL9823
#define WS2811_ACTUALLY_800kHz 0x40      // Normally we use 740kHz instead of 800, this uses 800.

#endif

constexpr Color8::Byteorder ByteOrderFromFlags(int CONFIG) {
  return
    (CONFIG & 0xf) == WS2811_RGB ? Color8::RGB :
    (CONFIG & 0xf) == WS2811_RBG ? Color8::RBG :
    (CONFIG & 0xf) == WS2811_GRB ? Color8::GRB :
    (CONFIG & 0xf) == WS2811_GBR ? Color8::GBR :
    Color8::BGR;
}

constexpr int FrequencyFromFlags(int CONFIG) {
  return
    (CONFIG & 0xf0) == WS2811_800kHz ? 740000 :
    (CONFIG & 0xf0) == WS2811_400kHz ? 400000 :
#ifdef WS2811_580kHz
    (CONFIG & 0xf0) == WS2811_580kHz ? 580000 :
#endif
    800000;
}

template<int LEDS, int CONFIG, int DATA_PIN = bladePin, class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>,
  template<int, int, Color8::Byteorder, int, int, int, int> class PinClass = DefaultPinClass,
  int reset_us=300, int t0h=294, int t1h=892,
  int POWER_OFF_DELAY_MS=3000>
class BladeBase *WS2811BladePtr() {
  static_assert(LEDS <= maxLedsPerStrip, "update maxLedsPerStrip");
  static POWER_PINS power_pins;
  static PinClass<LEDS, DATA_PIN, ByteOrderFromFlags(CONFIG), FrequencyFromFlags(CONFIG), reset_us, t0h, t1h> pin;
  static WS2811_Blade blade(&pin, &power_pins, POWER_OFF_DELAY_MS);
  return &blade;
}

template<int LEDS,
          int DATA_PIN = bladePin,
          Color8::Byteorder byteorder,
          class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>,
          template<int, int, Color8::Byteorder, int, int, int, int> class PinClass = DefaultPinClass,
          int frequency=800000, int reset_us=300, int t0h=294, int t1h=892,
          int POWER_OFF_DELAY_MS = 3000>
class BladeBase *WS281XBladePtr() {
  static POWER_PINS power_pins;
  static PinClass<LEDS, DATA_PIN, byteorder, frequency, reset_us, t0h, t1h> pin;
  static WS2811_Blade blade(&pin, &power_pins, POWER_OFF_DELAY_MS);
  return &blade;
}

template<int LEDS,
         int DATA_PIN = bladePin,
         int CLOCK_PIN,
         Color8::Byteorder byteorder,
         class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>,
         int max_frequency=800000,
         template<int, int, int, Color8::Byteorder, int> class PinClass = SpiLedPin,
         int POWER_OFF_DELAY_MS = 3000>
class BladeBase *SPIBladePtr() {
  static POWER_PINS power_pins;
  static PinClass<LEDS, DATA_PIN, CLOCK_PIN, byteorder, max_frequency> pin;
  static WS2811_Blade blade(&pin, &power_pins, POWER_OFF_DELAY_MS);
  return &blade;
}


#endif  // ENABLE_WS2811

#endif
