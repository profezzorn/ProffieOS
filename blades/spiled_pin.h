#ifndef BLADES_SPILEDPIN_H
#define BLADES_SPILEDPIN_H

class SpiLedPinBase : public WS2811PIN {
public:
  SpiLedPinBase(int num_leds,
                int pin,
                int clock_pin,
                Color8::Byteorder byteorder,
                int frequency) :
    pin_(pin),
    clock_pin_(clock_pin),
    num_leds_(num_leds),
    frequency_(frequency),
    byteorder_(byteorder) {
      pinMode(clock_pin_, OUTPUT);
      digitalWrite(clock_pin_, HIGH);
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
  }

  bool IsReadyForBeginFrame() { return true; }
  bool IsReadyForEndFrame() { return true; }

  static inline void delay_nanos(uint32_t nanos) {
#ifdef TEENSYDUINO
    uint32_t scale = F_CPU / 1000000;
#else    
    uint32_t scale = SystemCoreClock / 1000000;
#endif    
    uint32_t n = (nanos * scale + 2) / 3000;
  
    __asm__ __volatile__(
      "1: subs %0, #1 \n"
      "   bne  1b     \n"
      : "+r" (n));
  }

  void OutByte(uint8_t output) {
    for (int bit = 0; bit < 8; bit++) {
      digitalWrite(clock_pin_, LOW);
      digitalWrite(pin_, output & 0x80);
      delay_nanos(500000000 / frequency_);
      output <<= 1;
      digitalWrite(clock_pin_, HIGH);
      delay_nanos(500000000 / frequency_);
    }
  }
  
  void OutColor(Color16 color) {
    int MAX = std::max(color.r, std::max(color.g, color.b));
    int output_level = (MAX + 2113) / 2117;
    int mult = output_level == 0 ? 0 : 7930 / output_level;
    OutByte(0xE0 | output_level);
    for (int i = Color8::num_bytes(byteorder_) - 1; i >= 0; i--) {
      OutByte(color.getShort(byteorder_, i) * mult >> 16);
    }
  }


  void BeginFrame() {
    for (int i = Color8::num_bytes(byteorder_); i >= 0; i--) OutByte(0);
  }

  void EndFrame() {
    for (int i = 0; i < num_leds_; i++) {
      OutColor(color_buffer[i]);
      if ((i & 31) == 31) Looper::DoHFLoop();
    }
    for (int i = Color8::num_bytes(byteorder_); i >= 0; i--) OutByte(0xff);
  }

  int num_leds() const override { return num_leds_; }
  Color8::Byteorder get_byteorder() const override { return byteorder_; }
  void Enable(bool on) override {
    pinMode(pin_, on ? OUTPUT : INPUT_ANALOG);
    pinMode(clock_pin_, on ? OUTPUT : INPUT_ANALOG);
  }

private:
  uint8_t pin_;
  uint8_t clock_pin_;
  int num_leds_;
  uint32_t frequency_;
  Color8::Byteorder byteorder_;
};

template<int LEDS, int DATA_PIN, int CLOCK_PIN, Color8::Byteorder byteorder, int max_frequency>
class SpiLedPin : public SpiLedPinBase {
public:
  SpiLedPin() : SpiLedPinBase(LEDS, DATA_PIN, CLOCK_PIN, byteorder, max_frequency) {}
};

#endif
