#ifndef BLADES_FASTLED_BLADE_H
#define BLADES_FASTLED_BLADE_H

#ifdef ENABLE_FASTLED
#include "abstract_blade.h"

DMAMEM int fastLedMemory[maxLedsPerStrip * 24 / 4 + 1];

#include <FastLED.h>

// FASTLED-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
template<ESPIChipsets CHIPSET, EOrder RGB_ORDER, uint8_t SPI_DATA_RATE>
class FASTLED_Blade : public AbstractBlade, CommandParser, Looper {
public:
  FASTLED_Blade(int num_leds, PowerPinInterface* power) :
    AbstractBlade(),
    CommandParser(NOLINK),
    Looper(NOLINK),
    num_leds_(num_leds),
    power_(power) {
  }
  const char* name() override { return "FASTLED_Blade"; }

  void Power(bool on) {
    power_->Power(on);
    powered_ = on;
    allow_disable_ = false;
  }

  void Show() {
    if (spiLedSelect != -1 || spiDataOut != spiLedDataOut)
      STDOUT.println("SPI data conflict!");
    if (spiLedSelect != -1 || spiClock != spiLedClock)
      STDOUT.println("SPI clock conflict!");
    if (spiLedSelect != -1){
      SPI.beginTransaction(SPISettings(SPI_DATA_RATE, MSBFIRST, SPI_MODE0));
      digitalWrite(spiLedSelect, HIGH);  // enable access to LEDs
      FastLED.show();
      digitalWrite(spiLedSelect, LOW);
      SPI.endTransaction();   // allow other libs to use SPI again
    } else {
      // Bitbang on separate pins, no need to lock anything.
      FastLED.show();
    }
  }

  // No need for a "deactivate", the blade stays active until
  // you take it out, which also cuts the power.
  void Activate() override {
    STDOUT.print("FASTLED Blade with ");
    STDOUT.print(num_leds_);
    STDOUT.println(" leds");
    FastLED.addLeds<CHIPSET, spiLedDataOut, spiLedClock, RGB_ORDER, SPI_DATA_RATE>((struct CRGB*)fastLedMemory, num_leds_);
    power_->Init();
    Power(true);
    delay(10);

    clear();
    Show();
    CommandParser::Link();
    Looper::Link();
    AbstractBlade::Activate();
  }

  void Deactivate() override {
    Power(false);
    // de-init power pin?
    CommandParser::Unlink();
    Looper::Unlink();
    AbstractBlade::Deactivate();
  }

  // BladeBase implementation
  int num_leds() const override {
    return num_leds_;
  }
  // TODO: If needed, fix this!
  Color8::Byteorder get_byteorder() const override {
    return Color8::NONE;
  }
  bool is_on() const override {
    return on_;
  }
  bool is_powered() const override {
    return powered_;
  }
  void set(int led, Color16 c) override {
    ((Color8*)fastLedMemory)[led] = c.dither(0);
  }
  void allow_disable() override {
    if (!on_) allow_disable_ = true;
  }

  // SaberBase implementation.
  void SB_IsOn(bool* on) override {
    if (on_) *on = true;
  }
  void SB_On() override {
    AbstractBlade::SB_On();
    Power(true);
    delay(10);
    on_ = true;
  }
  void SB_Effect2(BladeEffectType type, float location) override {
    AbstractBlade::SB_Effect2(type, location);
    if (!powered_) {
      Power(true);
      delay(10);
    }
  }
  
  void SB_Off(OffType off_type) override {
    AbstractBlade::SB_Off(off_type);
    on_ = false;
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

protected:
  void Loop() override {
    if (!powered_) {
      loop_counter_.Reset();
      return;
    }
    uint32_t m = millis();
    // This limits the blade to 1000 updates per second, which
    // may not be enough for POV-style things, but I suspect
    // than running it at full speed will cause sound problems.
    // Note that the FASTLED code is so far untested, so it might
    // not work right.
    if (m == last_millis_) return;
    last_millis_ = m;
    current_style_->run(this);
    Show();
    if (allow_disable_) Power(on_);
  }

private:
  int num_leds_;
  bool on_ = false;
  bool powered_ = false;
  bool allow_disable_ = false;
  LoopCounter loop_counter_;
  uint32_t last_millis_;

  PowerPinInterface* power_;
};

template<ESPIChipsets CHIPSET, EOrder RGB_ORDER,
          uint8_t SPI_DATA_RATE, int LEDS, class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3> >
class BladeBase *FASTLEDBladePtr() {
  static_assert(LEDS <= maxLedsPerStrip, "update maxLedsPerStrip");
  static POWER_PINS power_pins;
  static FASTLED_Blade<CHIPSET, RGB_ORDER, SPI_DATA_RATE> blade(LEDS, &power_pins);
  return &blade;
}
#endif  // ENABLE_FASTLED

#endif
