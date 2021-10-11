#ifndef BLADES_WS2811_SERIAL_H
#define BLADES_WS2811_SERIAL_H

/*  WS2812Serial - Non-blocking WS2812 LED Display Library
    https://github.com/PaulStoffregen/WS2812Serial
    Copyright (c) 2017 Paul Stoffregen, PJRC.COM, LLC
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

class WS2811SerialEngine {
public:
  static uint32_t show(int pin, int numbits, int frequency) {
    uint32_t divisor, portconfig, hwtrigger;
    KINETISK_UART_t *uart;

    frequency *= 5;

    if (pin != last_pin && last_pin != -1) {
      digitalWrite(last_pin, LOW);
      pinMode(last_pin, OUTPUT);
      last_pin = -1;
    }
    
    switch (pin) {
#if defined(KINETISK) // Teensy 3.x
      case 1: // Serial1
      case 5:
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
      case 26:
#endif
	uart = &KINETISK_UART0;
	divisor = BAUD2DIV(frequency);
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3);
	hwtrigger = DMAMUX_SOURCE_UART0_TX;
	SIM_SCGC4 |= SIM_SCGC4_UART0;
	break;
      
      case 10: // Serial2
#if defined(__MK20DX128__) || defined(__MK20DX256__)
      case 31:
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
      case 58:
#endif
	uart = &KINETISK_UART1;
	divisor = BAUD2DIV2(frequency);
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3);
	hwtrigger = DMAMUX_SOURCE_UART1_TX;
	SIM_SCGC4 |= SIM_SCGC4_UART1;
	break;
      
      case 8: // Serial3
	uart = &KINETISK_UART2;
	divisor = BAUD2DIV3(frequency);
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3);
	hwtrigger = DMAMUX_SOURCE_UART2_TX;
	SIM_SCGC4 |= SIM_SCGC4_UART2;
	break;
      
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
      case 32: // Serial4
      case 62:
	uart = &KINETISK_UART3;
	divisor = BAUD2DIV3(frequency);
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3);
	hwtrigger = DMAMUX_SOURCE_UART3_TX;
	SIM_SCGC4 |= SIM_SCGC4_UART3;
	break;
      
      case 33: // Serial5
	uart = &KINETISK_UART4;
	divisor = BAUD2DIV3(frequency);
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3);
	hwtrigger = DMAMUX_SOURCE_UART4_RXTX;
	SIM_SCGC1 |= SIM_SCGC1_UART4;
	break;
#endif
#if defined(__MK64FX512__)
      case 48: // Serial6
	uart = &KINETISK_UART5;
	divisor = BAUD2DIV3(frequency);
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3);
	hwtrigger = DMAMUX_SOURCE_UART5_RXTX;
	SIM_SCGC1 |= SIM_SCGC1_UART5;
	break;
#endif
      
#elif defined(KINETISL)  // Teensy LC
      case 1: // Serial1
      case 5:
	uart = &KINETISK_UART0;
	divisor = 1;
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3);
	hwtrigger = DMAMUX_SOURCE_UART0_TX;
	SIM_SCGC4 |= SIM_SCGC4_UART0;
	break;
      case 4:
	uart = &KINETISK_UART0;
	divisor = 1;
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(2);
	hwtrigger = DMAMUX_SOURCE_UART0_TX;
	SIM_SCGC4 |= SIM_SCGC4_UART0;
	break;
      case 24:
	uart = &KINETISK_UART0;
	divisor = 1;
	portconfig = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(4);
	hwtrigger = DMAMUX_SOURCE_UART0_TX;
	SIM_SCGC4 |= SIM_SCGC4_UART0;
	break;
#endif
      default:
	return false; // pin not supported
    }
    uint32_t target_divisor = divisor;
#if defined(KINETISK)
    if (divisor < 32) divisor = 32;
    uart->BDH = (divisor >> 13) & 0x1F;
    uart->BDL = (divisor >> 5) & 0xFF;
    uart->C4 = divisor & 0x1F;
#elif defined(KINETISL)
    uart->BDH = (divisor >> 8) & 0x1F;
    uart->BDL = divisor & 0xFF;
    uart->C4 = 11;
#endif
    uart->C1 = 0;
    uart->C2 = UART_C2_TE | UART_C2_TIE;
    uart->C3 = UART_C3_TXINV;
    uart->C5 = UART_C5_TDMAS;
#if defined(KINETISK)
    uart->PFIFO = 0; // TODO: is this ok for Serial3-6?
#endif
    *(portConfigRegister(pin)) = portconfig;
    dma.destination(uart->D);
    dma.triggerAtHardwareEvent(hwtrigger);

    // start DMA transfer to update LEDs  :-)
#if defined(KINETISK)
    dma.sourceBuffer(displayMemory, numbits / 2);
    dma.transferSize(1);
    dma.transferCount(numbits / 2);
    dma.disableOnCompletion();
    dma.enable();
#elif defined(KINETISL)
    dma.CFG->SAR = displayMemory;
    dma.CFG->DSR_BCR = 0x01000000;
    dma.CFG->DSR_BCR = numbits / 2
    dma.CFG->DCR = DMA_DCR_ERQ | DMA_DCR_CS | DMA_DCR_SSIZE(1) |
      DMA_DCR_SINC | DMA_DCR_DSIZE(1) | DMA_DCR_D_REQ;
#endif
    return numbits * 5000000.0 / frequency  * divisor / target_divisor;
  }


  static bool busy() {
#if defined(KINETISK)
    return (DMA_ERQ & (1 << dma.channel));
#elif defined(KINETISL)
    return (dma.CFG->DCR & DMA_DCR_ERQ);
#endif
  }

  static int last_pin;
  static DMAChannel dma;
};

int WS2811SerialEngine::last_pin = -1;
DMAChannel WS2811SerialEngine::dma;

const uint32_t serial_lookup_table[256] = {
  0xefefefef, 0xfefefef, 0xe8efefef, 0x8efefef, 0xef0fefef, 0xf0fefef, 0xe80fefef, 0x80fefef,
  0xefe8efef, 0xfe8efef, 0xe8e8efef, 0x8e8efef, 0xef08efef, 0xf08efef, 0xe808efef, 0x808efef,
  0xefef0fef, 0xfef0fef, 0xe8ef0fef, 0x8ef0fef, 0xef0f0fef, 0xf0f0fef, 0xe80f0fef, 0x80f0fef,
  0xefe80fef, 0xfe80fef, 0xe8e80fef, 0x8e80fef, 0xef080fef, 0xf080fef, 0xe8080fef, 0x8080fef,
  0xefefe8ef, 0xfefe8ef, 0xe8efe8ef, 0x8efe8ef, 0xef0fe8ef, 0xf0fe8ef, 0xe80fe8ef, 0x80fe8ef,
  0xefe8e8ef, 0xfe8e8ef, 0xe8e8e8ef, 0x8e8e8ef, 0xef08e8ef, 0xf08e8ef, 0xe808e8ef, 0x808e8ef,
  0xefef08ef, 0xfef08ef, 0xe8ef08ef, 0x8ef08ef, 0xef0f08ef, 0xf0f08ef, 0xe80f08ef, 0x80f08ef,
  0xefe808ef, 0xfe808ef, 0xe8e808ef, 0x8e808ef, 0xef0808ef, 0xf0808ef, 0xe80808ef, 0x80808ef,
  0xefefef0f, 0xfefef0f, 0xe8efef0f, 0x8efef0f, 0xef0fef0f, 0xf0fef0f, 0xe80fef0f, 0x80fef0f,
  0xefe8ef0f, 0xfe8ef0f, 0xe8e8ef0f, 0x8e8ef0f, 0xef08ef0f, 0xf08ef0f, 0xe808ef0f, 0x808ef0f,
  0xefef0f0f, 0xfef0f0f, 0xe8ef0f0f, 0x8ef0f0f, 0xef0f0f0f, 0xf0f0f0f, 0xe80f0f0f, 0x80f0f0f,
  0xefe80f0f, 0xfe80f0f, 0xe8e80f0f, 0x8e80f0f, 0xef080f0f, 0xf080f0f, 0xe8080f0f, 0x8080f0f,
  0xefefe80f, 0xfefe80f, 0xe8efe80f, 0x8efe80f, 0xef0fe80f, 0xf0fe80f, 0xe80fe80f, 0x80fe80f,
  0xefe8e80f, 0xfe8e80f, 0xe8e8e80f, 0x8e8e80f, 0xef08e80f, 0xf08e80f, 0xe808e80f, 0x808e80f,
  0xefef080f, 0xfef080f, 0xe8ef080f, 0x8ef080f, 0xef0f080f, 0xf0f080f, 0xe80f080f, 0x80f080f,
  0xefe8080f, 0xfe8080f, 0xe8e8080f, 0x8e8080f, 0xef08080f, 0xf08080f, 0xe808080f, 0x808080f,
  0xefefefe8, 0xfefefe8, 0xe8efefe8, 0x8efefe8, 0xef0fefe8, 0xf0fefe8, 0xe80fefe8, 0x80fefe8,
  0xefe8efe8, 0xfe8efe8, 0xe8e8efe8, 0x8e8efe8, 0xef08efe8, 0xf08efe8, 0xe808efe8, 0x808efe8,
  0xefef0fe8, 0xfef0fe8, 0xe8ef0fe8, 0x8ef0fe8, 0xef0f0fe8, 0xf0f0fe8, 0xe80f0fe8, 0x80f0fe8,
  0xefe80fe8, 0xfe80fe8, 0xe8e80fe8, 0x8e80fe8, 0xef080fe8, 0xf080fe8, 0xe8080fe8, 0x8080fe8,
  0xefefe8e8, 0xfefe8e8, 0xe8efe8e8, 0x8efe8e8, 0xef0fe8e8, 0xf0fe8e8, 0xe80fe8e8, 0x80fe8e8,
  0xefe8e8e8, 0xfe8e8e8, 0xe8e8e8e8, 0x8e8e8e8, 0xef08e8e8, 0xf08e8e8, 0xe808e8e8, 0x808e8e8,
  0xefef08e8, 0xfef08e8, 0xe8ef08e8, 0x8ef08e8, 0xef0f08e8, 0xf0f08e8, 0xe80f08e8, 0x80f08e8,
  0xefe808e8, 0xfe808e8, 0xe8e808e8, 0x8e808e8, 0xef0808e8, 0xf0808e8, 0xe80808e8, 0x80808e8,
  0xefefef08, 0xfefef08, 0xe8efef08, 0x8efef08, 0xef0fef08, 0xf0fef08, 0xe80fef08, 0x80fef08,
  0xefe8ef08, 0xfe8ef08, 0xe8e8ef08, 0x8e8ef08, 0xef08ef08, 0xf08ef08, 0xe808ef08, 0x808ef08,
  0xefef0f08, 0xfef0f08, 0xe8ef0f08, 0x8ef0f08, 0xef0f0f08, 0xf0f0f08, 0xe80f0f08, 0x80f0f08,
  0xefe80f08, 0xfe80f08, 0xe8e80f08, 0x8e80f08, 0xef080f08, 0xf080f08, 0xe8080f08, 0x8080f08,
  0xefefe808, 0xfefe808, 0xe8efe808, 0x8efe808, 0xef0fe808, 0xf0fe808, 0xe80fe808, 0x80fe808,
  0xefe8e808, 0xfe8e808, 0xe8e8e808, 0x8e8e808, 0xef08e808, 0xf08e808, 0xe808e808, 0x808e808,
  0xefef0808, 0xfef0808, 0xe8ef0808, 0x8ef0808, 0xef0f0808, 0xf0f0808, 0xe80f0808, 0x80f0808,
  0xefe80808, 0xfe80808, 0xe8e80808, 0x8e80808, 0xef080808, 0xf080808, 0xe8080808, 0x8080808,
};


class WS2811SerialPin {
public:
  WS2811SerialPin(int pin,
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
  }

  bool IsReadyForBeginFrame() {
    return !WS2811SerialEngine::busy();
  }
  bool IsReadyForEndFrame() {
    return micros() - start_micros_ > required_micros_ + 301;
  }
  Color16* BeginFrame() {
    while (Color8::num_bytes(byteorder_) * num_leds_ * 8 + 1 > (int)sizeof(displayMemory)) {
      STDOUT.print("Display memory is not big enough, increase maxLedsPerStrip!");
      num_leds_ /= 2;
    }
    while (!IsReadyForBeginFrame()) yield();
    frame_num_++;
    return color_buffer;
  }
  void EndFrame() {
    while (!IsReadyForEndFrame()) yield();
    required_micros_ = WS2811SerialEngine::show(pin_, num_leds_ * Color8::num_bytes(byteorder_) * 8, frequency_);
    start_micros_ = micros();
  }

  void Set(int led, Color8 color) {
    uint32_t *output = ((uint32_t *)displayMemory) + led * Color8::num_bytes(byteorder_);
    for (int i = Color8::num_bytes(byteorder_) - 1; i >= 0; i--) {
      *(output++) = serial_lookup_table[color.getByte(byteorder_, i)];
    }
  }
  void Set(int led, Color16 color) {
    Set(led, color.dither(frame_num_, led));
  }

  int num_leds() const { return num_leds_; }
  Color8::Byteorder get_byteorder() const { return byteorder_; }
  uint8_t pin() const { return pin_; }

  uint8_t pin_;
  uint32_t num_leds_;
  uint32_t frame_num_;
  int frequency_;
  Color8::Byteorder byteorder_;
  uint32_t start_micros_;
  uint32_t required_micros_ = 0;
};

#endif
