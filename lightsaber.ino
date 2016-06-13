// Lightsaber Firmware
// TRY: https://forum.pjrc.com/threads/31717-OctoWS2811-and-CAN-bus?p=89689&amp%3Bviewfull=1
//
// Full orientation sensing using NXP's advanced sensor fusion algorithm.
// You *must* perform a magnetic calibration before this code will work.

// Feature defines, these let you turn off large blocks of code, meant mostly for debugging.

#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_SNOOZE

// Use the optimized SD_t3 sd library.
#define USE_TEENSY3_OPTIMIZED_CODE

#ifdef ENABLE_AUDIO
#include <Audio.h>
#include <AudioStream.h>
#endif

#include <EEPROM.h>
#include <SerialFlash.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <math.h>

// Teensy 3.2 pin map:
// 0 - FREE (reserve for serial?)
// 1 - FREE (reserve for serial?)
// 2 - input, motion sensor interrupts
// 3 - used by octo library
// 4 - output, SD card chip select (optional)
// 5 - output, enables amplifier
// 6 - output, serial flash memory chip select
// 7 - output, spi led chip select (optional)
// 8 - AUX button
// 9 - AUX2 button
// 10 - used by octo library (used to be pin 4)
// 11 - SPI data out (memory, spi led, sd card)
// 12 - SPI data in (memory, sd card)
// 13 - SPI clock (memory, spi led, sd card)
// 14 - output, WS2811 led (blade)
// 15 - used by octo library (connected to 16)
// 16 - used by octo library (connected to 15)
// 18 - I2C (motion sensors)
// 19 - I2C (motion sensors)
// 20 - input, Battery level
// 21 - input, Blade identify
// 22 - output, blade power BEC control
// 23 - power button


#ifdef ENABLE_MOTION
#include <NXPMotionSense.h>

NXPMotionSense imu;
NXPSensorFusion filter;
#endif


#ifdef ENABLE_SNOOZE
#include <Snooze.h>

SnoozeBlock snooze_config;
#endif

const int amplifierPin = 5;
const int batteryLevelPin = 20;
const int batteryLevelAnalogPin = 6;
const int bladeIdentifyPin = 21;
const int bladeIdentifyAnalogPin = 7;
const int auxPin = 8;
const int aux2Pin = 9;
const int powerButtonPin = 23;

// Helper class for classses that needs to be called back from the Loop() function.
class Looper;
Looper* loopers = NULL;
class Looper {
public:
  Looper() : next_looper_(loopers) {
    loopers = this;
  }
  static void DoLoop() {
    for (Looper *l = loopers; l; l = l->next_looper_) {
      l->Loop();
    }
  }
  static void DoSetup() {
    for (Looper *l = loopers; l; l = l->next_looper_) {
      l->Setup();
    }
  }
protected:
  virtual void Loop() = 0;
  virtual void Setup() {}
private:
  Looper* next_looper_;
};

// Command parsing linked list base class.
class CommandParser;
CommandParser* parsers = NULL;

class CommandParser {
public:
  CommandParser() : next_parser_(parsers) {
    parsers = this;
  }
  static bool DoParse(const char* cmd, const char* arg) {
    for (CommandParser *p = parsers; p; p = p->next_parser_) {
      if (p->Parse(cmd, arg))
        return true;
    }
    return false;
  }
protected:
  virtual bool Parse(const char* cmd, const char* arg) = 0;
private:
  CommandParser* next_parser_;
};


// Debug printout helper class
class Monitoring : Looper, CommandParser {
public:
  Monitoring() : Looper(), CommandParser() {}
  enum MonitorBit {
    MonitorSwings = 1,
    MonitorSamples = 2,
  };

  bool ShouldPrint(MonitorBit bit) {
    return monitor_this_loop_ && (bit & active_monitors_);
  }
protected:
  void Loop() override {
    monitor_this_loop_ = millis() - last_monitor_loop_ > monitor_frequency_ms_;
  }
  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "monitor") || !strcmp(cmd, "mon")) {
      if (!strcmp(arg, "swings")) {
	active_monitors_ ^= MonitorSwings;
	return true;
      }
      if (!strcmp(arg, "samples")) {
	active_monitors_ ^= MonitorSamples;
	return true;
      }
    }
    return false;
  }
private:
  uint32_t monitor_frequency_ms_ = 250;
  int last_monitor_loop_ = 0;
  bool monitor_this_loop_ = false;
  uint32_t active_monitors_ = 0;
};

Monitoring monitor;

// Helper functions & classes
float fract(float x) { return x - floor(x); }
float clamp(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

#ifdef ENABLE_AUDIO

#if 0
class ClickAvoiderExp {
public:
  ClickAvoiderExp(float speed) : speed_(32768 * speed) {}
  void set(uint16_t target) { target_ = target; }
  uint16_t value() const {
    return current_;
  }
  void advance() {
    current_ = (current_ * (32768-speed_) + target_ * speed_) >> 15;
  }

  const uint16_t speed_;
  uint16_t current_;
  uint16_t target_;
};
#endif

class ClickAvoiderLin {
public:
  ClickAvoiderLin(uint32_t speed) : speed_(speed) { }
  void set_target(uint32_t target) { target_ = target; }
  void set(uint16_t v) { current_ = v; }
  uint32_t value() const {return current_; }
  void advance() {
    uint32_t target = target_;
    if (current_ > target) {
      current_ -= min(speed_, current_ - target);
      return;
    }
    if (current_ < target) {
      current_ += min(speed_, target - current_);
      return;
    }
  }

  const uint32_t speed_;
  uint32_t current_;
  uint32_t target_;
};

struct WaveForm {
  int16_t table_[1024];
};

struct WaveFormSampler {
  WaveFormSampler(const WaveForm& waveform) : waveform_(waveform), pos_(0), delta_(0) {}
  const WaveForm& waveform_;
  int pos_;
  volatile int delta_;
  int16_t next() {
    pos_ += delta_;
    if (pos_ > 1024 * 65536) pos_ -= 1024 * 65536;
    // Bilinear lookup here?
    return waveform_.table_[pos_ >> 16];
  }
};

#if 0
class AudioEffectUpsample : public AudioStream {
public:
  AudioEffectUpsample() : AudioStream(1, inputQueueArray) {
    
  }
  void Emit(int16_t sample) {
    output_block_->data[output_pos_++] = sample;
    if (output_pos_ == AUDIO_BLOCK_SAMPLES) {
      transmit(output_block_);
      release(output_block_);
      output_block_ = allocate();
      output_pos_ = 0;
    }
  }
  void Ingest(int16_t sample) {
    buffer_[pos_] = sample;
    pos_++;
    if (pos_ == sizeof(buffer_) / sizeof(buffer_[0])) {
      memcpy(buffer_, buffer_ + 60, 8);
      pos_ -= 60;
    }
    Emit(sample);
    Emit((sample * C1 + buf_[pos_-1] * C2 + buf_[pos-2] * C2 + buf_[pos-3] * C1) >> 15);
  }
  virtual void update() {
    audio_block_t *block;

      block = receiveReadOnly();
      if (!block) return;
      for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++)
	Ingest(block->data[i]);
      release(block);
    }
  }
private:
  size_t out_pos_;
  audio_block_t* output_block_;
  audio_block_t* inputQueueArray[1];
  int16_t buffer_[64];
  size_t pos_;
};
#endif

class AudioSynthLightSaber : public AudioStream, Looper {
public:
  WaveForm sin_a_;
  WaveForm sin_b_;
  WaveForm buzz_;
  WaveForm humm_;
  WaveFormSampler sin_sampler_a_hi_;
  WaveFormSampler sin_sampler_a_lo_;
  WaveFormSampler sin_sampler_b_;
  WaveFormSampler buzz_sampler_;
  WaveFormSampler humm_sampler_hi_;
  WaveFormSampler humm_sampler_lo_;
  ClickAvoiderLin volume_;

  // For debug monitoring..
  int32_t last_value = 0;
  int32_t last_prevolume_value = 0;
  
  volatile bool on_ = false;

  float si(float x) { return sin(fract(x) * M_PI * 2.0); }
//  float sc(float x) { return clamp(si(x), -0.707, 0.707); }
  float sc(float x) { return clamp(si(x), -0.6, 0.6); }
  float buzz(float x) {
    x = fract(x) * 10.0;
    return sin(exp(2.5 - x) - 0.2);
  }
  float humm(float x) {
    return sc(x)*0.75 + si(x * 2.0)*0.75/2;
  }

  AudioSynthLightSaber() : AudioStream(0, NULL),
  sin_sampler_a_hi_(sin_a_),
  sin_sampler_a_lo_(sin_a_),
  sin_sampler_b_(sin_b_),
  buzz_sampler_(buzz_),
  humm_sampler_hi_(humm_),
  humm_sampler_lo_(humm_),
  volume_(32768 / 100) {
    sin_sampler_a_hi_.delta_ = 137 * 65536 / AUDIO_SAMPLE_RATE;
    sin_sampler_a_lo_.delta_ = 1024 * 65536 / AUDIO_SAMPLE_RATE;
    sin_sampler_b_.delta_ = 300 * 65536 / AUDIO_SAMPLE_RATE;
    AdjustDelta(0.0);
    for (int i = 0; i < 1024; i++) {
      float f = i/1024.0;
      sin_a_.table_[i] = 32768 / (3 + si(f));
      sin_b_.table_[i] = 32766 / (3 + si(f));
      buzz_.table_[i] = 32766 * buzz(f);
      humm_.table_[i] = 32766 * humm(f);
    }
  }

  void AdjustDelta(float speed) {
    float cents = 1.0 - 0.5 * clamp(speed/200.0, -1.0, 1.0);
    float hz_to_delta = cents * 1024 * 65536 / AUDIO_SAMPLE_RATE;
    buzz_sampler_.delta_ = 35 * hz_to_delta;
    humm_sampler_lo_.delta_ = 90 * hz_to_delta;
    humm_sampler_hi_.delta_ = 98 * hz_to_delta;
  }

  virtual void update(void) {
    audio_block_t *block ;
    if (!on_) return;
    block = allocate();
    if (block == NULL) return;
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      int32_t tmp;
      tmp  = humm_sampler_lo_.next() * sin_sampler_a_lo_.next();
      tmp += humm_sampler_hi_.next() * sin_sampler_a_hi_.next();
      tmp += buzz_sampler_.next() * sin_sampler_b_.next();
      tmp >>= 15;
//      tmp = humm_sampler_lo_.next();
      last_prevolume_value = tmp;
      tmp = (tmp * (int32_t)volume_.value()) >> 15;
      volume_.advance();
      last_value = tmp;
      tmp = clampi32(tmp, -32768, 32767);
      block->data[i] = tmp;
    }
    transmit(block);
    release(block);
  }
protected:
  void Loop() override {
    if (monitor.ShouldPrint(Monitoring::MonitorSamples)) {
      Serial.print("Last sample: ");
      Serial.print(last_value);
      Serial.print(" prevol: ");
      Serial.print(last_prevolume_value);
      Serial.print(" vol: ");
      Serial.println(volume_.value());
    }
  }
};

AudioSynthLightSaber saber_synth;

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=225,392
AudioPlaySerialflashRaw  playFlashRaw1;  //xy=234,486
AudioMixer4              mixer1;         //xy=476,396
AudioOutputAnalog        dac1;           //xy=685,405
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 1);
AudioConnection          patchCord2(playFlashRaw1, 0, mixer1, 2);
AudioConnection          patchCord3(saber_synth, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, dac1);
//AudioConnection          patchCord(saber, dac1);
// GUItool: end automatically generated code

class Effect;
Effect* all_effects = NULL;

class Effect {
  public:
  Effect(const char* name) : name_(name) {
    next_ = all_effects;
    all_effects = this;
    reset();
  }

  void reset() { files_found_ = 0; }
  void Parse(const char *filename) {
    size_t len = strlen(name_);
    if (memcmp(filename, name_, len)) {
      return;
    }

    int n = -1;
    if (filename[len] == '.') {
      n = 1;
    } else {
      n = strtol(filename + len, NULL, 0);
      if (n <= 0) return;
    }
    files_found_ = max(files_found_, n);
  }

  void Show() {
    Serial.print("Found ");
    Serial.print(files_found_);
    Serial.print(" '");
    Serial.print(name_);
    Serial.println("' sounds.");
  }

  static void ShowAll() {
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Show();
    }
  }

  bool Play() {
    if (files_found_ < 0) return false;
    int n = 0;
    if (files_found_ > 0) {
      n = rand() % files_found_;
    }
    char filename[150];
    strcpy(filename, name_);
    if (n > 0) {
      char buf[12];
      strcat(filename, itoa(n + 1, buf, 10));
    }
    strcat(filename, ".raw");
    Serial.print("Playing ");
    Serial.println(filename);
    playFlashRaw1.play(filename);
    return true;
  }

  static void ScanSerialFlash() {
    SerialFlashChip::opendir();
    uint32_t size;
    char filename[100];
    while (SerialFlashChip::readdir(filename, sizeof(filename), size)) {
      for (Effect* e = all_effects; e; e = e->next_) {
	e->Parse(filename);
      }
    }
  };

private:
  Effect* next_;
  int files_found_;
  const char* name_;
};

#define EFFECT(X) Effect X(#X)

EFFECT(boot);
EFFECT(poweron);  // Ignition?
EFFECT(poweroff);
EFFECT(clash);
EFFECT(force);
EFFECT(stab);
EFFECT(blaster);

#endif  // ENABLE_AUDIO

#if 1
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

#ifndef MonopodWS2811_h
#define MonopodWS2811_h

#include <Arduino.h>
#include "DMAChannel.h"

#if TEENSYDUINO < 121
#error "Teensyduino version 1.21 or later is required to compile this library."
#endif
#ifdef __AVR__
#error "MonopodWS2811 does not work with Teensy 2.0 or Teensy++ 2.0."
#endif

#define WS2811_RGB	0	// The WS2811 datasheet documents this way
#define WS2811_RBG	1
#define WS2811_GRB	2	// Most LED strips are wired this way
#define WS2811_GBR	3

#define WS2811_800kHz 0x00	// Nearly all WS2811 are 800 kHz
#define WS2811_400kHz 0x10	// Adafruit's Flora Pixels


class MonopodWS2811 {
public:
	MonopodWS2811(uint32_t numPerStrip, void *frameBuf, void *drawBuf, uint8_t config = WS2811_GRB);
	void begin(void);

	void setPixel(uint32_t num, int color);
	void setPixel(uint32_t num, uint8_t red, uint8_t green, uint8_t blue) {
		setPixel(num, color(red, green, blue));
	}
	int getPixel(uint32_t num);

	void show(void);
	int busy(void);

	int numPixels(void) {
		return stripLen;
	}
	int color(uint8_t red, uint8_t green, uint8_t blue) {
		return (red << 16) | (green << 8) | blue;
	}
	

private:
	static uint16_t stripLen;
	static void *frameBuffer;
	static void *drawBuffer;
	static uint8_t params;
	static DMAChannel dma1, dma2, dma3;
	static void isr(void);
};

#endif

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



uint16_t MonopodWS2811::stripLen;
void * MonopodWS2811::frameBuffer;
void * MonopodWS2811::drawBuffer;
uint8_t MonopodWS2811::params;
DMAChannel MonopodWS2811::dma1;
DMAChannel MonopodWS2811::dma2;
DMAChannel MonopodWS2811::dma3;

static uint8_t ones = 0x02;  // pin 14
static volatile uint8_t update_in_progress = 0;
static uint32_t update_completed_at = 0;


MonopodWS2811::MonopodWS2811(uint32_t numPerStrip, void *frameBuf, void *drawBuf, uint8_t config)
{
	stripLen = numPerStrip;
	frameBuffer = frameBuf;
	drawBuffer = drawBuf;
	params = config;
}

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


void MonopodWS2811::begin(void)
{
	uint32_t bufsize, frequency = 400000;

	bufsize = stripLen*24;

	// set up the buffers
	memset(frameBuffer, ones, bufsize);
	if (drawBuffer) {
		memset(drawBuffer, ones, bufsize);
	} else {
		drawBuffer = frameBuffer;
	}
	
	// configure the 8 output pins
	GPIOD_PCOR = ones;
//	pinMode(2, OUTPUT);	// strip #1
	pinMode(14, OUTPUT);	// strip #2
//	pinMode(7, OUTPUT);	// strip #3
//	pinMode(8, OUTPUT);	// strip #4
//	pinMode(6, OUTPUT);	// strip #5
//	pinMode(20, OUTPUT);	// strip #6
//	pinMode(21, OUTPUT);	// strip #7
//	pinMode(5, OUTPUT);	// strip #8

	// create the two waveforms for WS2811 low and high bits
	// FOOPIN Must be 4 or 17, and it can't be 4....
	// Need help changing 4 to 17...
#define FOOPIN 4
#define BARPIN 3
#define FOOCAT32(X,Y,Z) X##Y##Z
#define FOOCAT3(X,Y,Z) FOOCAT32(X,Y,Z)

	int t0h = WS2811_TIMING_T0H;
	int t1h = WS2811_TIMING_T1H;
	switch (params & 0xF0) {
          case WS2811_400kHz:
	   frequency = 400000;
	    break;

          case WS2811_800kHz:
	   frequency = 800000;
	   break;
        }
	frequency = 400000;
	analogWriteResolution(8);
	analogWriteFrequency(BARPIN, frequency);
	analogWriteFrequency(FOOPIN, frequency);
	analogWrite(BARPIN, t0h);
	analogWrite(FOOPIN, t1h);

#if defined(KINETISK)
	// pin 16 triggers DMA(port B) on rising edge (configure for pin 3's waveform)
	CORE_PIN16_CONFIG = PORT_PCR_IRQC(1)|PORT_PCR_MUX(3);
	pinMode(BARPIN, INPUT_PULLUP); // pin 3 no longer needed

	// pin 15 triggers DMA(port C) on falling edge of low duty waveform
	// pin 15 and 16 must be connected by the user: 16 is output, 15 is input
	pinMode(15, INPUT);
	CORE_PIN15_CONFIG = PORT_PCR_IRQC(2)|PORT_PCR_MUX(1);

	// pin FOOPIN triggers DMA(port A) on falling edge of high duty waveform
	FOOCAT3(CORE_PIN,FOOPIN,_CONFIG) = PORT_PCR_IRQC(2)|PORT_PCR_MUX(3);

#elif defined(KINETISL)
	// on Teensy-LC, use timer DMA, not pin DMA
	//Serial1.println(FTM2_C0SC, HEX);
	//FTM2_C0SC = 0xA9;
	//FTM2_C0SC = 0xA9;
	//uint32_t t = FTM2_C0SC;
	//FTM2_C0SC = 0xA9;
	//Serial1.println(t, HEX);
	CORE_PIN3_CONFIG = 0;
	FOOCAT3(CORE_PIN,FOOPIN,_CONFIG) = 0;
	//FTM2_C0SC = 0;
	//FTM2_C1SC = 0;
	//while (FTM2_C0SC) ;
	//while (FTM2_C1SC) ;
	//FTM2_C0SC = 0x99;
	//FTM2_C1SC = 0x99;

	//MCM_PLACR |= MCM_PLACR_ARB;

#endif

	// DMA channel #1 sets WS2811 high at the beginning of each cycle
	dma1.source(ones);
	dma1.destination(GPIOD_PSOR);
	dma1.transferSize(1);
	dma1.transferCount(bufsize);
	dma1.disableOnCompletion();

	// DMA channel #2 writes the pixel data at 20% of the cycle
	dma2.sourceBuffer((uint8_t *)frameBuffer, bufsize);
	dma2.destination(GPIOD_PCOR);
	dma2.transferSize(1);
	dma2.transferCount(bufsize);
	dma2.disableOnCompletion();

	// DMA channel #3 clear all the pins low at 48% of the cycle
	dma3.source(ones);
	dma3.destination(GPIOD_PCOR);
	dma3.transferSize(1);
	dma3.transferCount(bufsize);
	dma3.disableOnCompletion();
	dma3.interruptAtCompletion();

#ifdef __MK20DX256__
	MCM_CR = MCM_CR_SRAMLAP(1) | MCM_CR_SRAMUAP(0);
	AXBS_PRS0 = 0x1032;
#endif

#if defined(KINETISK)
	// route the edge detect interrupts to trigger the 3 channels
	dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
	dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTC);
	dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTA);
#elif defined(KINETISL)
	// route the timer interrupts to trigger the 3 channels
	dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_OV);
	dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
	dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH1);
#endif

	// enable a done interrupts when channel #3 completes
	dma3.attachInterrupt(isr);
	//pinMode(9, OUTPUT); // testing: oscilloscope trigger
}

void MonopodWS2811::isr(void)
{
	//Serial1.print(".");
	//Serial1.println(dma3.CFG->DCR, HEX);
	//Serial1.print(dma3.CFG->DSR_BCR > 24, HEX);
	dma3.clearInterrupt();
	//Serial1.print("*");
	update_completed_at = micros();
	update_in_progress = 0;
}

int MonopodWS2811::busy(void)
{
	if (update_in_progress) return 1;
	// busy for 50 us after the done interrupt, for WS2811 reset
	if (micros() - update_completed_at < 50) return 1;
	return 0;
}

void MonopodWS2811::show(void)
{
	// wait for any prior DMA operation
	//Serial1.print("1");
	while (update_in_progress) ; 
	//Serial1.print("2");
	// it's ok to copy the drawing buffer to the frame buffer
	// during the 50us WS2811 reset time
	if (drawBuffer != frameBuffer) {
		// TODO: this could be faster with DMA, especially if the
		// buffers are 32 bit aligned... but does it matter?
		memcpy(frameBuffer, drawBuffer, stripLen * 24);
	}
	// wait for WS2811 reset
	while (micros() - update_completed_at < 50) ;

#if defined(KINETISK)
	// ok to start, but we must be very careful to begin
	// without any prior 3 x 800kHz DMA requests pending
	uint32_t sc = FTM1_SC;
	uint32_t cv = FTM1_C1V;
	noInterrupts();
	// CAUTION: this code is timing critical.  Any editing should be
	// tested by verifying the oscilloscope trigger pulse at the end
	// always occurs while both waveforms are still low.  Simply
	// counting CPU cycles does not take into account other complex
	// factors, like flash cache misses and bus arbitration from USB
	// or other DMA.  Testing should be done with the oscilloscope
	// display set at infinite persistence and a variety of other I/O
	// performed to create realistic bus usage.  Even then, you really
	// should not mess with this timing critical code!
	update_in_progress = 1;
	while (FTM1_CNT <= cv) ; 
	while (FTM1_CNT > cv) ; // wait for beginning of an 800 kHz cycle
	while (FTM1_CNT < cv) ;
	FTM1_SC = sc & 0xE7;	// stop FTM1 timer (hopefully before it rolls over)
	//digitalWriteFast(9, HIGH); // oscilloscope trigger
	PORTB_ISFR = (1<<0);    // clear any prior rising edge
	PORTC_ISFR = (1<<0);	// clear any prior low duty falling edge
	PORTA_ISFR = (1<<13);	// clear any prior high duty falling edge
	dma1.enable();
	dma2.enable();		// enable all 3 DMA channels
	dma3.enable();
	FTM1_SC = sc;		// restart FTM1 timer
	//digitalWriteFast(9, LOW);
#elif defined(KINETISL)
	uint32_t sc = FTM2_SC;
	uint32_t cv = FTM2_C1V;
	noInterrupts();
	update_in_progress = 1;
	while (FTM2_CNT <= cv) ;
	while (FTM2_CNT > cv) ; // wait for beginning of an 800 kHz cycle
	while (FTM2_CNT < cv) ;
	FTM2_SC = 0;		// stop FTM2 timer (hopefully before it rolls over)
	//digitalWriteFast(9, HIGH); // oscilloscope trigger


	dma1.clearComplete();
	dma2.clearComplete();
	dma3.clearComplete();
	uint32_t bufsize = stripLen*24;
	dma1.transferCount(bufsize);
	dma2.transferCount(bufsize);
	dma3.transferCount(bufsize);
	dma2.sourceBuffer((uint8_t *)frameBuffer, bufsize);

	// clear any pending event flags
	FTM2_SC = 0x80;
	FTM2_C0SC = 0xA9;	// clear any previous pending DMA requests
	FTM2_C1SC = 0xA9;
	// clear any prior pending DMA requests
	dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_OV);
	dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
	dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH1);
	//GPIOD_PTOR = 0xFF;
	//GPIOD_PTOR = 0xFF;
	dma1.enable();
	dma2.enable();		// enable all 3 DMA channels
	dma3.enable();
	FTM2_SC = 0x188;
	//digitalWriteFast(9, LOW);
#endif
	//Serial1.print("3");
	interrupts();
	//Serial1.print("4");
}

void MonopodWS2811::setPixel(uint32_t num, int color)
{
	uint32_t strip, offset, mask;
	uint8_t bit, *p;
	
	switch (params & 7) {
	  case WS2811_RBG:
		color = (color&0xFF0000) | ((color<<8)&0x00FF00) | ((color>>8)&0x0000FF);
		break;
	  case WS2811_GRB:
		color = ((color<<8)&0xFF0000) | ((color>>8)&0x00FF00) | (color&0x0000FF);
		break;
	  case WS2811_GBR:
		color = ((color<<8)&0xFFFF00) | ((color>>16)&0x0000FF);
		break;
	  default:
		break;
	}
        color = color ^ 0xFFFFFF;
//	strip = num / stripLen;  // Cortex-M4 has 2 cycle unsigned divide :-)
//	offset = num % stripLen;

        strip = 1;
        offset = num;
	bit = (1<<strip);
	p = ((uint8_t *)drawBuffer) + offset * 24;
	for (mask = (1<<23) ; mask ; mask >>= 1) {
		if (color & mask) {
			*p++ |= bit;
		} else {
			*p++ &= ~bit;
		}
	}
}

int MonopodWS2811::getPixel(uint32_t num)
{
	uint32_t strip, offset, mask;
	uint8_t bit, *p;
	int color=0;

//	strip = num / stripLen;
//	offset = num % stripLen;
        strip = 2;
        offset = num;
	bit = (1<<strip);
	p = ((uint8_t *)drawBuffer) + offset * 24;
	for (mask = (1<<23) ; mask ; mask >>= 1) {
		if (*p++ & bit) color |= mask;
	}
	switch (params & 7) {
	  case WS2811_RBG:
		color = (color&0xFF0000) | ((color<<8)&0x00FF00) | ((color>>8)&0x0000FF);
		break;
	  case WS2811_GRB:
		color = ((color<<8)&0xFF0000) | ((color>>8)&0x00FF00) | (color&0x0000FF);
		break;
	  case WS2811_GBR:
		color = ((color<<8)&0xFFFF00) | ((color>>16)&0x0000FF);
		break;
	  default:
		break;
	}
	return color ^ 0xFFFFFF;
}

const unsigned int ledsPerStrip = 100;
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_800kHz;
MonopodWS2811 monopodws(ledsPerStrip, displayMemory, drawingMemory, config);

#endif

struct Color {
  Color() : r(0), g(0), b(0) {}
  Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
  // x = 0..256
  Color mix(const Color& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color( ((256-x) * r + x * other.r) >> 8,
                  ((256-x) * g + x * other.g) >> 8,
                  ((256-x) * b + x * other.b) >> 8);
  }
  uint8_t r, g, b;
};

struct BladeColor {
  Color base;
  Color tip;
  Color get(int led) { return base.mix(tip, led * 256 / ledsPerStrip); }
  void set(Color c) { base = tip = c; }
  void set(uint8_t r, uint8_t g, uint8_t b) { base = tip = Color(r, g, b); }
};

struct BladeSettings {
  BladeColor base;
  BladeColor shimmer;
  BladeColor clash;

  void setAll(uint8_t r, uint8_t g, uint8_t b) {
    base.set(r, g, b);
    shimmer = clash = base;
  }
};

// TODO: Support for alternate blade types
// TODO: Support changing number of LEDs at runtime
class Blade : CommandParser, Looper {
public:
  enum State {
    BLADE_OFF,
    BLADE_TURNING_ON,
    BLADE_ON,
    BLADE_TURNING_OFF,
    BLADE_TEST,
  };
  Blade() : CommandParser(), Looper(), state_(BLADE_OFF), clash_(false) {}
  bool IsON() { return state_ != BLADE_OFF; }
  void On() {
    state_ = BLADE_TURNING_ON;
    event_millis_ = millis();
    event_length_ = 500; // Guess
  }
  void Off() {
    state_ = BLADE_TURNING_OFF;
    event_millis_ = millis();
    event_length_ = 500; // Guess
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, "blade")) {
      if (!strcmp(arg, "on")) {
        On();
        return true;
      }
      if (!strcmp(arg, "off")) {
        Off();
        return true;
      }
      if (!strcmp(arg, "test")) {
	state_ = BLADE_TEST;
        return true;
      }
    }
    return false;
  }

  void Clash() { clash_ = true; }
  void Lockup() {  }

  BladeSettings settings_;

protected:
  void Loop() override {
    if (monopodws.busy()) return;
    monopodws.show();
    if (state_ == BLADE_OFF) return;
    int m = millis();
    int thres = ledsPerStrip * 256 * (m - event_millis_) / event_length_;
    if (thres > (int)((ledsPerStrip + 2) * 256)) {
      switch (state_) {
	case BLADE_TURNING_ON:
	  state_ = BLADE_ON;
	  break;
	case BLADE_TURNING_OFF:
	  state_ = BLADE_OFF;
	  break;
        default: break;
      }
    }
    for (unsigned int i = 0; i < ledsPerStrip; i++) {
      Color c;
      if (clash_) {
        c = settings_.clash.get(i);
        clash_ = false;
      } else {
        c = settings_.base.get(i);
      }
      // TODO: shimmer more when moving.
      // TODO: Other shimmer styles
      //      c = c.mix(settings_.shimmer.get(i), rand() & 0xFF);

      switch (state_) {
        case BLADE_TEST: {
          switch(((m / 100 + i)/10) % 3) {
            case 0: c = Color(255,0,0); break;
            case 1: c = Color(0,255,0); break;
            case 2: c = Color(0,0,255); break;
          }
          break;
        }
        case BLADE_OFF: c = Color();
	case BLADE_TURNING_ON: {
	  int x = i * 256 - thres;
	  if (x > 256) c = Color();
	  else if (x > -256) {
            c = c.mix(Color(), (512 - x) >> 1);
	  }
	  break;
	}
	case BLADE_ON: break;
	case BLADE_TURNING_OFF: {
	  int x = i * 256 - ledsPerStrip * 256 + thres;
          if (x > 256) c = Color();
	  else if (x > -256) { 
            c = c.mix(Color(), (512 - x) >> 1);
	  }
	  break;
	}
      }
      monopodws.setPixel(i, monopodws.color(c.r, c.g, c.b));
    }
  }
  
private:
  State state_;
  bool clash_;
  int event_millis_;
  int event_length_;
};

// Global settings:
// Volume, Blade Style

Blade blade;

struct BladeID {
  enum BladeType {
    WS2811,   // Also called neopixels, usually in the form of "strips"
    PL9823,   // Same as WS2811, but turns blue when you turn them on, which means we need to
                 // avoid turning the BEC off when a blade of this type is connected. Bad for battery life.
#if 0
    // The following are not yet implemented
    STRING,   // Single LED or string
    FLASH,    // One LED or string, with another led or string for flash-on-clash.

    // These would need to be hooked up through the prop shield, not pin 14.
    APA102,   // Similar to WS2811, but support much higher data rates, and uses two wires.
                 // BT_APA102 uses SPI to talk to the string.
#endif
  };

  // Blade identify resistor value.
  // Good values range from 1k to 100k
  // Remember that precision is typically +/- 5%, so don't
  // choose values that are too close to each other.
  int ohms;

  BladeType blade_type;
  int num_leds;
  // Default color settings
  // Default sound font
};

BladeID blades[] = {
  // ohms, blade type, leds
  {   2600, BladeID::PL9823, 100 },
  {  10000, BladeID::WS2811,  60 },
};

// Support for uploading files in TAR format.
class Tar {
  public:
    // Header description:
    //
    // Fieldno	Offset	len	Description
    // 
    // 0	0	100	Filename
    // 1	100	8	Mode (octal)
    // 2	108	8	uid (octal)
    // 3	116	8	gid (octal)
    // 4	124	12	size (octal)
    // 5	136	12	mtime (octal)
    // 6	148	8	chksum (octal)
    // 7	156	1	linkflag
    // 8	157	100	linkname
    // 9	257	8	magic
    // 10	265	32	(USTAR) uname
    // 11	297	32	(USTAR)	gname
    // 12	329	8	devmajor (octal)
    // 13	337	8	devminor (octal)
    // 14	345	167	(USTAR) Long path
    //
    // magic can be any of:
    //   "ustar\0""00"	POSIX ustar (Version 0?).
    //   "ustar  \0"	GNU tar (POSIX draft)

  void begin() {
    file_length_ = 0;
    bytes_ = 0;
  }
  bool write(char* data, size_t bytes) {
    while (bytes) {
      size_t to_copy = min(sizeof(block_) - bytes_, bytes);
      memcpy(block_ + bytes_, data, to_copy);
      bytes -= to_copy;
      data += to_copy;
      bytes_ += to_copy;
//      Serial.print("Have: ");
//      Serial.println(bytes_);

      if (bytes_ == sizeof(block_)) {
        if (file_length_) {
          size_t to_write = min(sizeof(block_), file_length_);
          flash_file_.write(block_, to_write);
          file_length_ -= to_write;
        } else {
          if (memcmp("ustar", block_ + 257, 5)) {
            Serial.println("NOT USTAR!");
          } else {
            // Start new file here
            file_length_ = strtol(block_ + 124, NULL, 8);
            Serial.print("Receiving ");
	    Serial.print(block_);
	    Serial.print(" length = ");
	    Serial.println(file_length_);
            if (!SerialFlashChip::create(block_, file_length_)) {
              Serial.println("Create file failed.");
              return false;
            }
	    flash_file_ = SerialFlashChip::open(block_);
          }
        }
	bytes_ = 0;
      }
    }
    return true;
  }
  private:
    SerialFlashFile flash_file_;
    size_t file_length_;
    size_t bytes_;
    char block_[512];
};

class Scheduler : Looper {
public:
  enum Action {
    NO_ACTION,
    TURN_ON,
  };
  Scheduler() : Looper(), next_action_(NO_ACTION) {}

  unsigned long delay_;
  unsigned long base_;
  Action next_action_;

  void CheckNext() {
    if (millis() - base_ > delay_) {
      switch (next_action_) {
	case TURN_ON:
	  // Will fade in based on swing volume set elsewhere.
	  saber_synth.volume_.set(0);
	  saber_synth.on_ = true;
	  break;

         case NO_ACTION:
	  // Do nothing
          break;
      }
      next_action_ = NO_ACTION;
    }
  }

  void SetNextAction(Action action, unsigned long delay) {
    delay_ = delay;
    base_ = millis();
    next_action_ = action;
  }
protected:
  void Loop() override { CheckNext(); }
};

Scheduler scheduler;

class Button : Looper, CommandParser {
public:
  Button(int pin, const char* name)
    : Looper(),
      CommandParser(),
      pin_(pin),
      name_(name),
      pushed_(false),
      clicked_(false),
      push_millis_(0) {
    pinMode(pin, INPUT_PULLUP);
    snooze_config.pinMode(pin, INPUT_PULLUP, RISING);
  }
  int pushed_millis() {
    if (pushed_) return millis() - push_millis_;
    return 0;
  }
  bool clicked() {
    bool ret = clicked_;
    clicked_ = false;
    return ret;
  }
protected:
  void Loop() {
    if (digitalRead(pin_) == HIGH) {
      if (!pushed_) {
        push_millis_ = millis();
        pushed_ = true;
      }
    } else {
      if (pushed_) {
        int t = millis() - push_millis_;
        if (t < 5) return; // De-bounce
        pushed_ = false;
        if (t < 500) clicked_ = true;
      }
    }
  }
  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      clicked_ = true;
      return true;
    }
    return false;
  }
private:
  uint8_t pin_;
  const char* name_;
  bool pushed_;
  bool clicked_;
  int push_millis_;
};

class Saber : CommandParser, Looper {
public:
  Saber() : CommandParser(),
	    power_(powerButtonPin, "pow"),
	    aux_(auxPin, "aux"),
	    aux2_(aux2Pin, "aux2") {}

  void On() {
    if (saber_synth.on_) return;
    Serial.println("Ignition.");
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    delay(10);             // allow time to wake up
    
    blade.On();
    poweron.Play();
    uint32_t delay = 0;
#ifdef ENABLE_AUDIO
    if (playFlashRaw1.isPlaying()) {
      delay = playFlashRaw1.lengthMillis() - 10;
    }
#endif
  
    scheduler.SetNextAction(Scheduler::TURN_ON, delay);
  }

  void Off() {
    // TODO: FADE OUT!
    saber_synth.on_ = false;
    if (scheduler.next_action_ == Scheduler::TURN_ON) {
      scheduler.SetNextAction(Scheduler::NO_ACTION, 0);
    }
    poweroff.Play();
    blade.Off();
  }

  unsigned long last_clash = 0;
  void Clash() {
    // TODO: Pick clash randomly and/or based on strength of clash.
    if (!saber_synth.on_) return;
    unsigned long t = millis();
    if (t - last_clash < 100) return;
    last_clash = t;
    clash.Play();
    blade.Clash();
  }

protected:
  void Loop() override {
    if (power_.clicked()) {
      if (saber_synth.on_) {
	Off();
      } else {
	On();
      }
    }
  }
  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "on")) {
      On();
      return true;
    }
    if (!strcmp(cmd, "off")) {
      Off();
      return true;
    }
    if (!strcmp(cmd, "clash")) {
      Clash();
      return true;
    }
    return false;
  }
private:
  Button power_;
  Button aux_;
  Button aux2_;
};

Saber saber;

class Parser : Looper {
public:
  enum Mode {
    ModeCommand,
    ModeDecode
  };
  Parser() : Looper(), len_(0), mode_(ModeCommand), do_print_welcome_(true) {}

  void Loop() override {
    if (!Serial) {
      do_print_welcome_ = true;
      return;
    }
    if (do_print_welcome_) {
      Serial.println("Welcome to TeensySaber, type 'help' for more info.");
      do_print_welcome_ = false;
    }

    while (Serial.available() > 0) {
      int c = Serial.read();
      if (c < 0) { len_ = 0; break; }
      if (c == '\n') { Parse(); len_ = 0; continue; }
      cmd_[len_] = c;
      cmd_[len_ + 1] = 0;
      if (len_ + 1 < (int)sizeof(cmd_)) len_++;
    }
  }

  void Parse() {
    switch (mode_) {
      case ModeCommand:
	ParseCmd();
	break;
      case ModeDecode:
        UUDecode();
	break;
    }
  }

  int32_t DecodeChar(char c) {
    return (c - 32) & 0x3f;
  }

  void UUDecode() {
    if (cmd_[0] == '`') {
      Serial.println("Done");
      mode_ = ModeCommand;
      return;
    }
    int to_decode = DecodeChar(cmd_[0]);
    if (to_decode > 0 || to_decode < 80) {
      char *in = cmd_+ 1;
      char *out = cmd_;
      int decoded = 0;
      while (decoded < to_decode) {
        int32_t bits = (DecodeChar(in[0]) << 18) | (DecodeChar(in[1]) << 12) | (DecodeChar(in[2]) << 6) | DecodeChar(in[3]);
        out[0] = bits >> 16;
        out[1] = bits >> 8;
        out[2] = bits;
#if 0
	Serial.print(bits);
        Serial.print("<");
        Serial.print((int)out[0]);
        Serial.print(",");
        Serial.print((int)out[1]);
        Serial.print(",");
        Serial.print((int)out[2]);
        Serial.print(">");
        Serial.println("");
#endif
        in += 4;
        out += 3;
        decoded += 3;
      }
      if (tar_.write(cmd_, to_decode))
        return;
    }

    Serial.println("decode failed");
    mode_ = ModeCommand;
  }

  void ParseCmd() {
    if (len_ == 0 || len_ == (int)sizeof(cmd_)) return;
    char *cmd = cmd_;
    while (*cmd == ' ') cmd++;
    char *e = cmd;
    while (*e != ' ' && *e) e++;
    if (*e) {
      *e = 0;
      e++;  // e is now argument (if any)
    }
    if (!strcmp(cmd, "help")) {
      Serial.println("General commands:");
      Serial.println("  on, off, blade on, blade off, clash, pow, aux, aux2");
      Serial.println("Serial Flash memory management:");
      Serial.println("   ls, rm <file>, format, play <file>, effects");
      Serial.println("To upload files: tar cf - files | uuencode >/dev/ttyACM0");
      Serial.println("Debugging commands:");
      Serial.println("   monitor swings, monitor samples, top");
      return;
    }

    if (!strcmp(cmd, "end")) {
      // End command ignored.
      return;
    }
    if (!strcmp(cmd, "begin")) {
      // Filename is ignored.
      mode_ = ModeDecode;
      tar_.begin();
      return;
    }
    if (!strcmp(cmd, "ls")) {
      SerialFlashChip::opendir();
      uint32_t size;
      while (SerialFlashChip::readdir(cmd_, sizeof(cmd_), size)) {
	Serial.print(cmd_);
	Serial.print(" ");
	Serial.println(size);
      }
      Serial.println("Done listing files.");
      return;
    }
    if (!strcmp(cmd, "effects")) {
      Effect::ShowAll();
      return;
    }
#if 0
    if (!strcmp(cmd, "df")) {
      Serial.print(SerialFlashChip::capacity());
      Serial.println(" bytes available.");
      return;
    }
#endif
    if (!strcmp(cmd, "rm")) {
      if (SerialFlashChip::remove(e)) {
        Serial.println("Removed.\n");
      } else {
        Serial.println("No such file.\n");
      }
      return;
    }
    if (!strcmp(cmd, "play")) {
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      delay(10);             // allow time to wake up
      playFlashRaw1.play(e);
      return;
    }
    if (!strcmp(cmd, "format")) {
      Serial.print("Erasing ... ");
      SerialFlashChip::eraseAll();
      while (!SerialFlashChip::ready());
      Serial.println("Done");
      return;
    }
    if (!strcmp(cmd, "top")) {
      Serial.print("Audio usage: ");
      Serial.println(AudioProcessorUsage());
      Serial.print("Audio usage max: ");
      Serial.println(AudioProcessorUsageMax());
      AudioProcessorUsageMaxReset();
      // TODO: List blade update speed
      return;
    }
    if (CommandParser::DoParse(cmd, e)) {
      return;
    }
    Serial.println("Whut?");
  }
private:
  Tar tar_;
  int len_;
  Mode mode_;
  char cmd_[256];
  bool do_print_welcome_;
};

Parser parser;

#ifdef ENABLE_MOTION
class Vec3 {
public:
  Vec3(){}
  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
  Vec3 operator-(const Vec3& o) const {
    return Vec3(x - o.x, y - o.y, z - o.z);
  }
  float len2() const { return x*x + y*y + z*z; }
  float x, y, z;
};

class Orientation : Looper {
  public:
  Orientation() : Looper(), accel_(0.0f, 0.0f, 0.0f) {
  }

  void Loop() override {
    if (imu.available()) {
      Vec3 accel, gyro, magnetic;

      // Read the motion sensors
      imu.readMotionSensor(accel.x, accel.y, accel.z,
			   gyro.x, gyro.y, gyro.z,
			   magnetic.x, magnetic.y, magnetic.z);
      // Clash detection
      //Serial.print("ACCEL2: ");
      //Serial.println((accel_ - accel).len2());
      if ( (accel_ - accel).len2() > 1.0) {
	// Needs de-bouncing
	saber.Clash();
      }
      accel_ = accel;

      // static float last_speed = 0.0;
      float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);

      if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
	Serial.print("Speed: ");
	Serial.print(speed);
	Serial.print(" VOL: ");
	Serial.print(saber_synth.volume_.value());
	Serial.print(" TARGET: ");
	Serial.println(saber_synth.volume_.target_);
      }

      saber_synth.volume_.set_target(32768 * (0.5 + clamp(speed/200.0, 0.0, 0.5)));
      // TODO: speed delta?
      saber_synth.AdjustDelta(speed);
      
      // last_speed = speed;
#if 0
      // Update the SensorFusion filter
      filter.update(accel.x, accel.y, accel.z,
		    gyro.x, gyro.y, gyro.z,
		    magnetic.x, magnetic.y, magnetic.z);

      // print the heading, pitch and roll
      float roll = filter.getRoll();
      float pitch = filter.getPitch();
      float heading = filter.getYaw();
      Serial.print("Orientation: ");
      Serial.print(heading);
      Serial.print(" ");
      Serial.print(pitch);
      Serial.print(" ");
      Serial.println(roll);
#endif
    }
  }
private:
  Vec3 accel_;
};

Orientation orientation;
#endif   // ENABLE_MOTION


#ifdef ENABLE_AUDIO
// Maybe name this IdleHelper or something instead??
class Amplifier : Looper {
public:
  Amplifier() : Looper(), do_print_amp_off_(false) {}
protected:
  void Loop() override{
    if (!saber_synth.on_ && !playFlashRaw1.isPlaying() && !playSdWav1.isPlaying()) {
    // Make sure audio has actually settled.
    if (do_print_amp_off_) {
      Serial.println("Amplifier off.");
      do_print_amp_off_ = false;
    }
    delay(20);
    digitalWrite(amplifierPin, LOW); // turn the amplifier off
    // Make sure amplifier settles.
    delay(20);
    } else {
      do_print_amp_off_ = true;
    }
  }
private:
  bool do_print_amp_off_;
};

Amplifier amplifier;
#endif

void setup() {
  pinMode(bladeIdentifyPin, INPUT_PULLUP);
  pinMode(batteryLevelPin, INPUT_PULLUP);

  delayMicroseconds(10);

  // Time to identify the blade.
  int blade_id = analogRead(bladeIdentifyAnalogPin);
  float v1 = blade_id * 3.3 / 1024.0;
  float amps = (3.3 - v1) / 33000; // Pull-up is 33k
  float r = v1 / amps;

  size_t best_config = 0;
  float best_err = 1000000.0;
  for (size_t i = 0; i < sizeof(blades) / sizeof(blades)[0]; i++) {
    float err = fabs(r - blades[i].ohms);
    if (err < best_err) {
      best_config = i;
      best_err = err;
    }
  }
  Serial.print("ID: ");
  Serial.print(blade_id);
  Serial.print(" resistance= ");
  Serial.print(r);
  Serial.print(" blade= ");
  Serial.println(best_config);

  // TODO only activate monopodws if we have a WS2811-type blade
  blade.settings_.base.set(0, 0, 255);
  blade.settings_.shimmer.set(0, 0, 255);
  blade.settings_.clash.set(255, 255, 255);
  monopodws.begin();
  monopodws.show();
  
  SerialFlashChip::begin(6);
    
  Serial.begin(9600);
  imu.begin();
  filter.begin(100);

  // Audio setup
  // dac1.analogReference(EXTERNAL); // much louder!
  delay(50);             // time for DAC voltage stable

  pinMode(amplifierPin, OUTPUT);
  AudioMemory(20);

  digitalWrite(amplifierPin, HIGH); // turn on the amplifier
  Effect::ScanSerialFlash();
  delay(10);             // allow time to wake up
  boot.Play();  
  monopodws.show();

  Looper::DoSetup();
}

int last_activity = millis();

void loop() {
  Looper::DoLoop();

  if (!saber_synth.on_ &&
      !playFlashRaw1.isPlaying() &&
      !playSdWav1.isPlaying() &&
      digitalRead(amplifierPin) == LOW &&
      !Serial && !blade.IsON()) {
    if (millis() - last_activity > 1000) {
      Serial.println("Snoozing...");
      Snooze.sleep(snooze_config);
    }
  } else {
    last_activity = millis();
  }
}

