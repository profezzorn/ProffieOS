#ifndef BLADES_MONOPODWS_H
#define BLADES_MONOPODWS_H

#ifdef ENABLE_WS2811

// What follows is a copy of the OctoWS2811 library. It's been modified in 
// the following ways:
//
//   1) It now only outputs data to one pin. (Which pin can be selected by
//      changing the "ones" bitfield below.
//   2) It has been modified to use the FTM timer to drive the DMA. This
//      frees up a few pins, but locks PWM frequencies for many pins to 800kHz
//   3) Number of LEDs and configuration is determined when you call begin()
//      instead of in the constructor to make different blade configurations possible.


/*  OctoWS2811 - High Performance WS2811 LED Display Library
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#if TEENSYDUINO < 121
#error "Teensyduino version 1.21 or later is required to compile this library."
#endif
#ifdef __AVR__
#error "MonopodWS2811 does not work with Teensy 2.0 or Teensy++ 2.0."
#endif

#define WS2811_RGB      0       // The WS2811 datasheet documents this way
#define WS2811_RBG      1
#define WS2811_GRB      2       // Most LED strips are wired this way
#define WS2811_GBR      3

#define WS2811_800kHz 0x00      // Nearly all WS2811 are 800 kHz
#define WS2811_400kHz 0x10      // Adafruit's Flora Pixels
#define WS2813_800kHz 0x20      // WS2813 are close to 800 kHz but has 300 us frame set delay
#define WS2811_580kHz 0x30      // PL9823
#define WS2811_ACTUALLY_800kHz 0x40      // Normally we use 740kHz instead of 800, this uses 800.



// Waveform timing: these set the high time for a 0 and 1 bit, as a fraction of
// the total 800 kHz or 400 kHz clock cycle.  The scale is 0 to 255.  The Worldsemi
// datasheet seems T1H should be 600 ns of a 1250 ns cycle, or 48%.  That may
// erroneous information?  Other sources reason the chip actually samples the
// line close to the center of each bit time, so T1H should be 80% if TOH is 20%.
// The chips appear to work based on a simple one-shot delay triggered by the
// rising edge.  At least 1 chip tested retransmits 0 as a 330 ns pulse (26%) and
// a 1 as a 660 ns pulse (53%).  Perhaps it's actually sampling near 500 ns?
// There doesn't seem to be any advantage to making T1H less, as long as there
// is sufficient low time before the end of the cycle, so the next rising edge
// can be detected.  T0H has been lengthened slightly, because the pulse can
// narrow if the DMA controller has extra latency during bus arbitration.  If you
// have an insight about tuning these parameters AND you have actually tested on
// real LED strips, please contact paul@pjrc.com.  Please do not email based only
// on reading the datasheets and purely theoretical analysis.
#define WS2811_TIMING_T0H  60
#define WS2811_TIMING_T1H  176

// Discussion about timing and flicker & color shift issues:
// http://forum.pjrc.com/threads/23877-WS2812B-compatible-with-OctoWS2811-library?p=38190&viewfull=1#post38190

#define WS2811_PORT_CLEAR GPIOD_PCOR
#define WS2811_PORT_SET   GPIOD_PSOR

class MonopodWS2811 {
public:
  static void show(int pin, uint8_t ones, int bufsize, uint32_t frequency) {
    ones_ = ones;
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    int t0h = WS2811_TIMING_T0H;
    int t1h = WS2811_TIMING_T1H;
    
#if defined(__MK20DX128__)
    FTM1_SC = 0;
    FTM1_CNT = 0;
    uint32_t mod = (F_BUS + frequency / 2) / frequency;
    FTM1_MOD = mod - 1;
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);
    FTM1_C0SC = 0x69;
    FTM1_C1SC = 0x69;
    FTM1_C0V = (mod * t0h) >> 8;
    FTM1_C1V = (mod * t1h) >> 8;
    // pin 16 triggers DMA(port B) on rising edge
    CORE_PIN16_CONFIG = PORT_PCR_IRQC(1)|PORT_PCR_MUX(3);
    //CORE_PIN4_CONFIG = PORT_PCR_MUX(3); // testing only
    
#elif defined(__MK20DX256__)
    FTM2_SC = 0;
    FTM2_CNT = 0;
    uint32_t mod = (F_BUS + frequency / 2) / frequency;
    FTM2_MOD = mod - 1;
    FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);
    FTM2_C0SC = 0x69;
    FTM2_C1SC = 0x69;
    FTM2_C0V = (mod * t0h) >> 8;
    FTM2_C1V = (mod * t1h) >> 8;
    // pin 32 is FTM2_CH0, PTB18, triggers DMA(port B) on rising edge
    // pin 25 is FTM2_CH1, PTB19
    CORE_PIN32_CONFIG = PORT_PCR_IRQC(1)|PORT_PCR_MUX(3);
    //CORE_PIN25_CONFIG = PORT_PCR_MUX(3); // testing only
    
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
    FTM2_SC = 0;
    FTM2_CNT = 0;
    uint32_t mod = (F_BUS + frequency / 2) / frequency;
    FTM2_MOD = mod - 1;
    FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);
    FTM2_C0SC = 0x69;
    FTM2_C1SC = 0x69;
    FTM2_C0V = (mod * t0h) >> 8;
    FTM2_C1V = (mod * t1h) >> 8;
    // FTM2_CH0, PTA10 (not connected), triggers DMA(port A) on rising edge
    PORTA_PCR10 = PORT_PCR_IRQC(1)|PORT_PCR_MUX(3);
    
#elif defined(__MKL26Z64__)
    analogWriteResolution(8);
    analogWriteFrequency(3, frequency);
    analogWriteFrequency(4, frequency);
    analogWrite(3, t0h);
    analogWrite(4, t1h);
    // on Teensy-LC, use timer DMA, not pin DMA
    //Serial1.println(FTM2_C0SC, HEX);
    //FTM2_C0SC = 0xA9;
    //FTM2_C0SC = 0xA9;
    //uint32_t t = FTM2_C0SC;
    //FTM2_C0SC = 0xA9;
    //Serial1.println(t, HEX);
    CORE_PIN3_CONFIG = 0;
    CORE_PIN4_CONFIG = 0;
    //FTM2_C0SC = 0;
    //FTM2_C1SC = 0;
    //while (FTM2_C0SC) ;
    //while (FTM2_C1SC) ;
    //FTM2_C0SC = 0x99;
    //FTM2_C1SC = 0x99;
    
    //MCM_PLACR |= MCM_PLACR_ARB;
    
#endif
    
    // DMA channel #1 sets WS2811 high at the beginning of each cycle
    dma1.source(ones_);
    dma1.destination(WS2811_PORT_SET);
    dma1.transferSize(1);
    dma1.transferCount(bufsize);
    dma1.disableOnCompletion();
    
    // DMA channel #2 writes the pixel data at 20% of the cycle
    dma2.sourceBuffer((uint8_t *)displayMemory, bufsize);
    dma2.destination(WS2811_PORT_CLEAR);
    dma2.transferSize(1);
    dma2.transferCount(bufsize);
    dma2.disableOnCompletion();
    
    // DMA channel #3 clear all the pins low at 48% of the cycle
    dma3.source(ones_);
    dma3.destination(WS2811_PORT_CLEAR);
    dma3.transferSize(1);
    dma3.transferCount(bufsize + 1);
    dma3.disableOnCompletion();
    dma3.interruptAtCompletion();

#if defined(__MK20DX128__)
    // route the edge detect interrupts to trigger the 3 channels
    dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
    dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM1_CH0);
    dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM1_CH1);
    DMAPriorityOrder(dma3, dma2, dma1);
#elif defined(__MK20DX256__)
    // route the edge detect interrupts to trigger the 3 channels
    dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
    dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
    dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH1);
    DMAPriorityOrder(dma3, dma2, dma1);

    // Try some stuff to reduce jitter
    DMA_CR |= DMA_CR_ERCA;
    MCM_CR = MCM_CR_SRAMLAP(1) | MCM_CR_SRAMUAP(0);
    AXBS_PRS0 = 0x1032;

#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
    // route the edge detect interrupts to trigger the 3 channels
    dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTA);
    dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
    dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH1);
    DMAPriorityOrder(dma3, dma2, dma1);
#elif defined(__MKL26Z64__)
    // route the timer interrupts to trigger the 3 channels
    dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_OV);
    dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
    dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH1);
#endif
    
    // enable a done interrupts when channel #3 completes
    dma3.attachInterrupt(isr);
    
#if defined(__MK20DX128__)
    uint32_t cv = FTM1_C1V;
    noInterrupts();
    // CAUTION: this code is timing critical.
    while (FTM1_CNT <= cv) ;
    while (FTM1_CNT > cv) ; // wait for beginning of an 800 kHz cycle
    while (FTM1_CNT < cv) ;
    FTM1_SC = 0;            // stop FTM1 timer (hopefully before it rolls over)
    update_in_progress_ = 1;
    PORTB_ISFR = (1<<0);    // clear any prior rising edge
    FTM1_C0SC = 0x28;
    // Clear any prior timer DMA triggers
    asm volatile ("" : "=m" (FTM1_C0SC) : "r" (FTM1_C0SC));
    FTM1_C0SC = 0x69;
    FTM1_C1SC = 0x28;
    // Clear any prior timer DMA triggers
    asm volatile ("" : "=m" (FTM1_C1SC) : "r" (FTM1_C1SC));
    FTM1_C1SC = 0x69;
    dma1.enable();
    dma2.enable();          // enable all 3 DMA channels
    dma3.enable();
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0); // restart FTM1 timer
    
#elif defined(__MK20DX256__)
    FTM2_C0SC = 0x28;
    FTM2_C1SC = 0x28;
    delay(1);
    uint32_t cv = FTM2_C1V;
    noInterrupts();
    // CAUTION: this code is timing critical.
    while (FTM2_CNT <= cv) ;
    while (FTM2_CNT > cv) ; // wait for beginning of an 800 kHz cycle
    while (FTM2_CNT < cv) ;
    FTM2_SC = 0;             // stop FTM2 timer (hopefully before it rolls over)
    update_in_progress_ = 1;
    //digitalWriteFast(9, HIGH); // oscilloscope trigger
    PORTB_ISFR = (1<<18);    // clear any prior rising edge
    FTM2_C0SC = 0x28;
    // Clear any prior timer DMA triggers
    asm volatile ("" : "=m" (FTM2_C0SC) : "r" (FTM2_C0SC));
    FTM2_C0SC = 0x69;
    FTM2_C1SC = 0x28;
    // Clear any prior timer DMA triggers
    asm volatile ("" : "=m" (FTM2_C1SC) : "r" (FTM2_C1SC));
    FTM2_C1SC = 0x69;
    dma1.enable();
    dma2.enable();           // enable all 3 DMA channels
    dma3.enable();
    FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0); // restart FTM2 timer
    
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
    FTM2_C0SC = 0x28;
    FTM2_C1SC = 0x28;
    delay(1);
    uint32_t cv = FTM2_C1V;
    noInterrupts();
    // CAUTION: this code is timing critical.
    while (FTM2_CNT <= cv) ;
    while (FTM2_CNT > cv) ; // wait for beginning of an 800 kHz cycle
    while (FTM2_CNT < cv) ;
    FTM2_SC = 0;             // stop FTM2 timer (hopefully before it rolls over)
    FTM2_CNT = 0;
    update_in_progress_ = 1;
    PORTA_ISFR = (1<<10);    // clear any prior rising edge
    FTM2_C0SC = 0x28;
    // Clear any prior timer DMA triggers
    asm volatile ("" : "=m" (FTM2_C0SC) : "r" (FTM2_C0SC));
    FTM2_C0SC = 0x69;
    FTM2_C1SC = 0x28;
    // Clear any prior timer DMA triggers
    asm volatile ("" : "=m" (FTM2_C1SC) : "r" (FTM2_C1SC));
    FTM2_C1SC = 0x69;
    dma1.enable();
    dma2.enable();           // enable all 3 DMA channels
    dma3.enable();
    FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0); // restart FTM2 timer
    
#elif defined(__MKL26Z64__)
    uint32_t sc = FTM2_SC;
    uint32_t cv = FTM2_C1V;
    noInterrupts();
    update_in_progress_ = 1;
    while (FTM2_CNT <= cv) ;
    while (FTM2_CNT > cv) ; // wait for beginning of an 800 kHz cycle
    while (FTM2_CNT < cv) ;
    FTM2_SC = 0;          // stop FTM2 timer (hopefully before it rolls over)
    dma1.clearComplete();
    dma2.clearComplete();
    dma3.clearComplete();
    dma1.transferCount(bufsize);
    dma2.transferCount(bufsize);
    dma3.transferCount(bufsize);
    dma2.sourceBuffer((uint8_t *)displayMemory, bufsize);
    // clear any pending event flags
    FTM2_SC = 0x80;
    FTM2_C0SC = 0xA9;     // clear any previous pending DMA requests
    FTM2_C1SC = 0xA9;
    // clear any prior pending DMA requests
    dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_OV);
    dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
    dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH1);
    //GPIOD_PTOR = 0xFF;
    //GPIOD_PTOR = 0xFF;
    dma1.enable();
    dma2.enable();                // enable all 3 DMA channels
    dma3.enable();
    FTM2_SC = 0x188;
#endif
    //Serial1.print("3");
    interrupts();
    //Serial1.print("4");
  }

  static bool busy() {
    return update_in_progress_;
  }

private:
  static void isr(void) {
    dma3.clearInterrupt();
    update_in_progress_ = false;
  }
  static volatile bool update_in_progress_;
  static DMAChannel dma1, dma2, dma3;
  static volatile uint8_t ones_;
};

DMAChannel MonopodWS2811::dma1;
DMAChannel MonopodWS2811::dma2;
DMAChannel MonopodWS2811::dma3;
volatile bool MonopodWS2811::update_in_progress_ = false;
volatile uint8_t MonopodWS2811::ones_;

template<Color8::Byteorder BYTEORDER>
class MonopodWSPinBase : public WS2811PIN {
public:
MonopodWSPinBase(int num_leds, int pin, uint32_t frequency,
	       uint32_t reset_us, uint32_t t0h_us, uint32_t t1h_us) :
    pin_(pin),
    num_leds_(num_leds),
    frequency_(frequency) {
    // These are for PORTB
    // TODO: Update it to work on any port
    switch (pin) {
      case 2:  ones_ = 1; break;
      case 14: ones_ = 2; break;
      case 7:  ones_ = 4; break;
      case 8:  ones_ = 8; break;
      case 6:  ones_ = 16; break;
      case 20: ones_ = 32; break;
      case 21: ones_ = 64; break;
      case 5:  ones_ = 128; break;
      default:
	STDOUT.println("Invalid monopodws pin!");
    }
    zero4X_ = ones_ * 0x01010101U;
  }
  bool IsReadyForBeginFrame() {
    return !MonopodWS2811::busy();
  }
  bool IsReadyForEndFrame() {
    return micros() - start_micros_ > num_leds_ * 24000000.0 / frequency_ + 300;
  }
  void BeginFrame() {
    while (Color8::num_bytes(BYTEORDER) * num_leds_ * 8 + 1 > (int)sizeof(displayMemory)) {
      STDOUT.print("Display memory is not big enough, increase maxLedsPerStrip!");
      num_leds_ /= 2;
    }
    while (!IsReadyForBeginFrame());
    frame_num_ ++;
  }
  void EndFrame() {
    while (!IsReadyForEndFrame());
    uint32_t *output = (uint32_t *)displayMemory;
    for (int j = 0; j < num_leds_; j++) {
      Color8 color = color_buffer[j].dither(frame_num_, j);
      for (int i = Color8::num_bytes(BYTEORDER) - 1; i >= 0; i--) {
	uint32_t tmp = color.getByte(BYTEORDER, i) * 0x8040201U;
	*(output++) = zero4X_ - ((tmp >> 7) & 0x01010101U) * ones_;
	*(output++) = zero4X_ - ((tmp >> 3) & 0x01010101U) * ones_;
      }
      if ((j & 31) == 31) Looper::DoHFLoop();
    }
    MonopodWS2811::show(pin_, ones_, num_leds_ * Color8::num_bytes(BYTEORDER) * 8, frequency_);
    start_micros_ = micros();
  }

  int num_leds() const override { return num_leds_; }
  Color8::Byteorder get_byteorder() const override { return BYTEORDER; }
  void Enable(bool on) override {
    pinMode(pin_, on ? OUTPUT : INPUT_ANALOG);
  }

  uint8_t pin_;
  uint32_t zero4X_;
  uint8_t ones_;
  int num_leds_;
  uint32_t start_micros_;
  uint32_t frequency_;
  uint32_t frame_num_;
};

template<int LEDS, int PIN, Color8::Byteorder BYTEORDER, int frequency=800000, int reset_us=300, int t1h=294, int t0h=892>
class MonopodWSPin : public MonopodWSPinBase<BYTEORDER> {
public:
  MonopodWSPin() : MonopodWSPinBase<BYTEORDER>(LEDS, PIN, frequency, reset_us, t0h, t1h) {
  }
};

#endif
#endif
