#ifndef BLADES_SPILEDPIN_H
#define BLADES_SPILEDPIN_H

template<int CLOCK_PIN>
class SpiLedPin {
public:
  SpiLedPin(int pin,
            int num_leds,
            Color8::Byteorder byteorder,
            int frequency,
            int reset_us,
            int t0h_us = 294,
            int t1h_us = 862) :
    pin_(pin),
    num_leds_(num_leds),
    frequency_(frequency),
    byteorder_(byteorder) {
      pinMode(CLOCK_PIN, OUTPUT);
      digitalWrite(CLOCK_PIN, HIGH);
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
      digitalWrite(CLOCK_PIN, LOW);
      digitalWrite(pin_, output & 0x80);
      delay_nanos(500000000 / frequency_);
      output <<= 1;
      digitalWrite(CLOCK_PIN, HIGH);
      delay_nanos(500000000 / frequency_);
    }
  }
  
  void BeginFrame() {
    for (int i = Color8::num_bytes(byteorder_); i >= 0; i--) OutByte(0);
  }
  void EndFrame() {
    for (int i = Color8::num_bytes(byteorder_); i >= 0; i--) OutByte(0xff);
  }

  void Set(int led, Color16 color) {
    int MAX = std::max(color.r, std::max(color.g, color.b));
    int output_level = (MAX + 2113) / 2117;
    int mult = output_level == 0 ? 0 : 7930 / output_level;
    OutByte(0xE0 | output_level);
    for (int i = Color8::num_bytes(byteorder_) - 1; i >= 0; i--) {
      OutByte(color.getShort(byteorder_, i) * mult >> 16);
    }
  }

  void Set(int led, Color8 color) {
    Set(led, Color16(color));
  }

  int num_leds() const { return num_leds_; }
  Color8::Byteorder get_byteorder() const { return byteorder_; }
  uint8_t pin() const { return pin_; }

private:
  uint8_t pin_;
  uint32_t num_leds_;
  uint32_t frequency_;
  Color8::Byteorder byteorder_;
};

#endif
