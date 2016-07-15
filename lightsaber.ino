/*  Teensy Lightsaber Firmware
    http://fredrik.hubbe.net/lightsaber/teensy_saber.html
    Copyright (c) 2016 Fredrik Hubinette
    Additional copyright holders listed inline below.

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


// TODO LIST:
// sdcard support
// Audio work items:
//   Make swing sounds!
//   Background music mode.
//   Support regular sound fonts.
//   select clash from force
// Support multiple sound fonts.
// Implement menues:
//    select sound font
//    select color
//    adjust volume
// POV-writer mode

/*
 * User configurable options below
 * These options relate to alternate, but supported ways to hook everything up.
 * For unsupported configurations, you're going to need to read the code below.
 */

// This struct defines the configuration for the available blades.
struct BladeID {
  enum BladeType {
    WS2811,   // Also called neopixels, usually in the form of "strips"
    PL9823,   // Same as WS2811, but turns blue when you turn them on, which means we need to
                 // avoid turning the BEC off when a blade of this type is connected. Bad for battery life.
    SIMPLE,   // string or LED, with optional FoC
#if 0
    // Not yet supported
    // These would need to be hooked up through the prop shield, not pin 20.
    APA102,   // Similar to WS2811, but support much higher data rates, and uses two wires.
                 // BT_APA102 uses SPI to talk to the string.
#endif
  };

  // Blade identify resistor value.
  // Good values range from 1k to 100k
  // Remember that precision can be +/- 10%, so don't
  // choose values that are too close to each other.
  int ohms;

  BladeType blade_type;
  int num_leds;
  const char* sound_font_directory;
  // Default color settings
  // Default sound font
};


// List of blades.
// If you only have one blade, the resistor value is irrelevant as it will always be selected.
BladeID blades[] = {
  // ohms, blade type, leds, default font directory
  {  69000, BladeID::PL9823,  97, "font01" },
//  { 10000, BladeID::WS2811,  120 },
};


// If you use an N-channel FET instead of a level shifter to drive
// neopixels, the high becomes low and vice versa, so the driving
// logic needs to be inverted.
// If you're using a FET or PEX, on pin 20, leave this defined.
// If not, comment it out.
// #define INVERT_WS2811


// Teensy 3.2 pin map:
// A lot of these can be changed, but be careful, because:
//   o The pins used by the prop shield cannot be easily changed.
//   o Pins that are going to control normal LEDs (not neopoxels) need PWM capability,
//     and not all teensy pins can do PWM.
//   o Touch input is not available on all pins.
//   o Sdcard chip select pin depends on what shield you use.
//   o Battery level and blade identification needs analog input, which is not possible
//     on all pins.
//
// Note that while WS2811 leds/strips are being controlled, analogWriteFrequency() can only
// be used on pins 3, 4, 25 & 32, on all others the analogWriteFrequency is locked to the
// frequency of the WS2811 (usually 740kHz) which is not an approperiate frequency for
// driving a FET. This limitation may change in future versions of this software, but for
// now, if you want to drive a crystal chamber or other internal LEDs, I recommend using
// the pins listed above to get a reasonable PWM frequency.
//
// See the teensy 3.2 pinout diagram for more info: https://www.pjrc.com/teensy/pinout.html
enum SaberPins {
  // Bottom edge (in pin-out diagram)
  sdCardSelectPin = 0,            // SD card chip (sd card adapter)
  freePin1 = 1,                   // FREE
  motionSensorInterruptPin = 2,   // motion sensor interrupt (prop shield)
  freePin3 = 3,                   // FREE
  freePin4 = 4,                   // FREE (make this sdCardSelectPin if you're using a Wiz820+SD shield)
  amplifierPin = 5,               // Amplifier enable pin (prop shield)
  serialFlashSelectPin = 6,       // serial flash chip select (prop shield)
  spiLedSelect = 7,               // APA102/dotstar chip select (prop shield)
  freePin8 = 8,                   // FREE
  freePin9 = 9,                   // FREE
  freePin10 = 10,                 // FREE
  spiDataOut = 11,                // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card

  // Top edge
  spiClock = 13,                  // spi clock, flash, spi led & sd card
  batteryLevelPin = 14,           // battery level input
  powerButtonPin = 15,            // power button
  auxPin = 16,                    // AUX button
  aux2Pin = 17,                   // AUX2 button
  i2cDataPin = 18,                // Used by motion sensors (prop shield)
  i2cClockPin = 19,               // Used by motion sensors (prop shield)
  bladePin = 20,                  // blade control, either WS2811 or PWM
  bladeIdentifyPin = 20,          // blade identify input / FoC
  bladePowerPin1 = 21,             // blade power control
  bladePowerPin2 = 22,             // blade power control
  bladePowerPin3 = 23,             // blade power control
};

/*
 * USER CONFIGURABLE OPTIONS END HERE
 */

// Feature defines, these let you turn off large blocks of code
// used for debugging.
#define ENABLE_AUDIO
#define ENABLE_MOTION
//#define ENABLE_SNOOZE
#define ENABLE_WS2811

// Use FTM timer for monopodws timing.
#define USE_FTM_TIMER

// For better playback go enable this define.
// (Not here, but in SD.h)
// #define USE_TEENSY3_OPTIMIZED_CODE

#include <Arduino.h>
#include <EEPROM.h>
#include <SerialFlash.h>
#include <DMAChannel.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <math.h>

#ifdef ENABLE_MOTION
#include <NXPMotionSense.h>

NXPMotionSense imu;
NXPSensorFusion filter;
#endif


#ifdef ENABLE_SNOOZE
#include <Snooze.h>

SnoozeBlock snooze_config;
#endif

const char version[] = "$Id$";

// Note, you cannot have two YIELD() on the same line.
#define YIELD() do { next_state_ = __LINE__; return; case __LINE__: break; } while(0)
#define SLEEP(MILLIS) do { sleep_until_ = millis() + (MILLIS); while (millis() < sleep_until_) YIELD(); } while(0)
#define STATE_MACHINE_BEGIN() switch(next_state_) { case -1:
#define STATE_MACHINE_END() }

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))

class StateMachine {
protected:
  int next_state_ = -1;
  uint32_t sleep_until_ = 0;
  void reset_state_machine() {
    next_state_ = -1;
  }
};

enum NoLink { NOLINK = 17 };

// Helper class for classses that needs to be called back from the Loop() function.
class Looper;
Looper* loopers = NULL;
class Looper {
public:
  void Link() {
    next_looper_ = loopers;
    loopers = this;
  }
  void Unlink() {
    for (Looper** i = &loopers; *i; i = &(*i)->next_looper_) {
      if (*i == this) {
        *i = next_looper_;
        return;
      }
    }
  }
  Looper() { Link(); }
  explicit Looper(NoLink _) { }
  ~Looper() { Unlink(); }
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
  void Link() {
    next_parser_ = parsers;
    parsers = this;
  }
  void Unlink() {
    for (CommandParser** i = &parsers; *i; i = &(*i)->next_parser_) {
      if (*i == this) {
        *i = next_parser_;
        return;
      }
    }
  }

  CommandParser() { Link(); }
  explicit CommandParser(NoLink _) {}
  ~CommandParser() { Unlink(); }
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

class SaberBase;
SaberBase* saberbases = NULL;

class SaberBase {
public:
  void Link() {
    next_saber_ = saberbases;
    saberbases = this;
  }
  void Unlink() {
    for (SaberBase** i = &saberbases; *i; i = &(*i)->next_saber_) {
      if (*i == this) {
        *i = next_saber_;
        return;
      }
    }
  }

  SaberBase() { Link(); }
  explicit SaberBase(NoLink _) {}
  ~SaberBase() { Unlink(); }


#define SABERFUN(NAME, TYPED_ARGS, ARGS)			\
public:								\
  static void Do##NAME TYPED_ARGS {				\
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {	\
      p->NAME ARGS;						\
    }								\
  }								\
protected:							\
  virtual void NAME TYPED_ARGS {}

  SABERFUN(Clash, (), ());
  SABERFUN(Stab, (), ());
  SABERFUN(On, (), ());
  SABERFUN(Off, (), ());
  SABERFUN(Lockup, (), ());
  SABERFUN(Force, (), ());
  SABERFUN(Swing, (), ());
  SABERFUN(Top, (), ());
  SABERFUN(IsOn, (bool* on), (on));

private:
  SaberBase* next_saber_;
};


// Debug printout helper class
class Monitoring : Looper, CommandParser {
public:
  Monitoring() : Looper(), CommandParser() {}
  enum MonitorBit {
    MonitorSwings = 1,
    MonitorSamples = 2,
    MonitorTouch = 4,
    MonitorBattery = 8,
  };

  bool ShouldPrint(MonitorBit bit) {
    return monitor_this_loop_ && (bit & active_monitors_);
  }
protected:
  void Loop() override {
    monitor_this_loop_ = millis() - last_monitor_loop_ > monitor_frequency_ms_;
    if (monitor_this_loop_) {
      last_monitor_loop_ = millis();
    }
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
      if (!strcmp(arg, "touch")) {
        active_monitors_ ^= MonitorTouch;
        return true;
      }
      if (!strcmp(arg, "battery")) {
        active_monitors_ ^= MonitorBattery;
        return true;
      }
    }
    return false;
  }
private:
  uint32_t monitor_frequency_ms_ = 500;
  int last_monitor_loop_ = 0;
  bool monitor_this_loop_ = false;
  uint32_t active_monitors_ = 0;
};

Monitoring monitor;

// Returns the decimals of a number, ie 12.2134 -> 0.2134
float fract(float x) { return x - floor(x); }

// clamp(x, a, b) makes sure that x is between a and b.
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

class ClickAvoiderLin {
public:
  ClickAvoiderLin() : speed_(0) { }
  ClickAvoiderLin(uint32_t speed) : speed_(speed) { }
  void set_target(uint32_t target) { target_ = target; }
  void set_speed(uint32_t speed) { speed_ = speed; }
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

  uint32_t speed_;
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


template<class T>
class DataStream {
public:
  virtual int read(T* data, int elements) = 0;
};

#define AUDIO_BUFFER_SIZE 440
#define AUDIO_RATE 44100

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_CONT | PDB_SC_PDBIE | PDB_SC_DMAEN)

class DAC {
public:
  DAC() {
    dma.begin(true); // Allocate the DMA channel first
    SIM_SCGC2 |= SIM_SCGC2_DAC0;
    DAC0_C0 = DAC_C0_DACEN;                   // 1.2V VDDA is DACREF_2
//    DAC0_C0 |= DAC_C0_DACRFS;  // 3.3V
    // slowly ramp up to DC voltage, approx 1/4 second
    // TODO: Use this for volume control?
    for (int16_t i=0; i<2048; i+=8) {
     *(int16_t *)&(DAC0_DAT0L) = i;
      delay(1);
    }

    // set the programmable delay block to trigger DMA requests
    SIM_SCGC6 |= SIM_SCGC6_PDB;
    PDB0_IDLY = 1;
    PDB0_MOD = F_BUS / AUDIO_RATE;
    PDB0_SC = PDB_CONFIG | PDB_SC_LDOK;
    PDB0_SC = PDB_CONFIG | PDB_SC_SWTRIG;
    PDB0_CH0C1 = 0x0101;

    dma.TCD->SADDR = dac_dma_buffer;
    dma.TCD->SOFF = 2;
    dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
    dma.TCD->NBYTES_MLNO = 2;
    dma.TCD->SLAST = -sizeof(dac_dma_buffer);
    dma.TCD->DADDR = &DAC0_DAT0L;
    dma.TCD->DOFF = 0;
    dma.TCD->CITER_ELINKNO = NELEM(dac_dma_buffer);
    dma.TCD->DLASTSGA = 0;
    dma.TCD->BITER_ELINKNO = NELEM(dac_dma_buffer);
    dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
    dma.triggerAtHardwareEvent(DMAMUX_SOURCE_PDB);
    dma.enable();
    dma.attachInterrupt(isr);
  }

  void SetStream(DataStream<int16_t>* stream) {
    stream_ = stream;
  }

private:
  static void isr(void) {
    int16_t *dest, *end;
    uint32_t saddr;

    saddr = (uint32_t)(dma.TCD->SADDR);
    dma.clearInterrupt();
    if (saddr < (uint32_t)dac_dma_buffer + sizeof(dac_dma_buffer) / 2) {
      // DMA is transmitting the first half of the buffer
      // so we must fill the second half
      dest = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE];
      end = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*2];
    } else {
      // DMA is transmitting the second half of the buffer
      // so we must fill the first half
      dest = (int16_t *)dac_dma_buffer;
      end = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE];
    }
    DataStream<int16_t> *stream = stream_;
    if (stream) {
      int n = stream->read(dest, end-dest);
      while (n--) {
	*dest = ((*(int16_t*)dest) + 32767) >> 4;
        dest++;
      }
    }
    while (dest < end) { *dest++ = 2047; }
  }

public: // FIXME remove 'public'

  DMAMEM static uint16_t dac_dma_buffer[AUDIO_BUFFER_SIZE*2];
  static DataStream<int16_t> * volatile stream_;
  static DMAChannel dma;
};

DMAChannel DAC::dma(false);
DataStream<int16_t> * volatile DAC::stream_ = nullptr;
DMAMEM uint16_t DAC::dac_dma_buffer[AUDIO_BUFFER_SIZE*2];

DAC dac;

template<int N> class AudioDynamicMixer : public DataStream<int16_t> {
public:
  AudioDynamicMixer() {
    for (int i = 0; i < N; i++) {
      streams_[i] = nullptr;
    }
  }

  int read(int16_t* data, int elements) override {
    int32_t sum[32];
    int ret = elements;
    while (elements) {
      int to_do = min(elements, (int)NELEM(sum));
      for (int i = 0; i < to_do; i++) sum[i] = 0;
      for (int i = 0; i < N; i++) {
	int e = streams_[i] ? streams_[i]->read(data, to_do) : 0;
	for (int j = 0; j < e; j++) {
	  sum[j] += data[j];
	}
      }
      
      // FIXME, actually do dynamic compression!
      for (int i = 0; i < to_do; i++) {
	data[i] = clampi32(sum[i] / N, -32768, 32767);
      }
      data += to_do;
      elements -= to_do;
    }
    return ret;
  }
  
  DataStream<int16_t>* streams_[N];
//  int32_t sum_;
//  ClickAvoiderLin volume_;
};


class LightSaberSynth : public DataStream<int16_t>, Looper {
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
  int32_t last_elements = 0;
  
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

  LightSaberSynth() :
    sin_sampler_a_hi_(sin_a_),
    sin_sampler_a_lo_(sin_a_),
    sin_sampler_b_(sin_b_),
    buzz_sampler_(buzz_),
    humm_sampler_hi_(humm_),
    humm_sampler_lo_(humm_),
    volume_(32768 / 100) {
    sin_sampler_a_hi_.delta_ = 137 * 65536 / AUDIO_RATE;
    sin_sampler_a_lo_.delta_ = 1024 * 65536 / AUDIO_RATE;
    sin_sampler_b_.delta_ = 300 * 65536 / AUDIO_RATE;
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
    float hz_to_delta = cents * 1024 * 65536 / AUDIO_RATE;
    buzz_sampler_.delta_ = 35 * hz_to_delta;
    humm_sampler_lo_.delta_ = 90 * hz_to_delta;
    humm_sampler_hi_.delta_ = 98 * hz_to_delta;
  }

  int read(int16_t *data, int elements) override {
    last_elements = elements;
    for (int i = 0; i < elements; i++) {
      int32_t tmp;
      tmp  = humm_sampler_lo_.next() * sin_sampler_a_lo_.next();
      tmp += humm_sampler_hi_.next() * sin_sampler_a_hi_.next();
      tmp += buzz_sampler_.next() * sin_sampler_b_.next();
      tmp >>= 15;
//      tmp = humm_sampler_lo_.next();
      last_prevolume_value = tmp;
      tmp = (tmp * (int32_t)volume_.value()) >> 15;
      volume_.advance();
      tmp = clampi32(tmp, -32768, 32767);
      last_value = tmp;
      data[i] = tmp;
    }
    return elements;
  }
protected:
  void Loop() override {
    if (monitor.ShouldPrint(Monitoring::MonitorSamples)) {
      Serial.print("Last elements: ");
      Serial.print(last_elements);
      Serial.print("Last sample: ");
      Serial.print(last_value);
      Serial.print(" prevol: ");
      Serial.print(last_prevolume_value);
      Serial.print(" vol: ");
      Serial.println(volume_.value());
    }
  }
};

LightSaberSynth saber_synth;

#endif  // ENABLE_AUDIO

#if 1

// Wanted audio capabilities:
// Background music (with independent volume)
// Options:
// 3 x EFFECT + synthesized HUM&SWING  (teensy style)
// 3 x EFFECT (wav) + { wav HUM or synthesized hum }  (nec style)
// HUM/EFFECTS spliced or x-faded (monophonic style)

// This means:
// 5 x WAV player (read from sdcard or serial flash) + HUM/SWING synthesizer
// WAV player needs gapless playback and find-cut support.

#define IRQ_WAV 55

class DataStreamWork;
DataStreamWork* data_streams;

class DataStreamWork {
public:
  DataStreamWork() {
    next_ = data_streams;
    data_streams = this;
    NVIC_SET_PRIORITY(IRQ_WAV, 240);
    _VectorsRam[IRQ_WAV + 16] = &ProcessDataStreams;
    NVIC_ENABLE_IRQ(IRQ_WAV);
  }
  ~DataStreamWork() {
    for (DataStreamWork** d = &data_streams; *d; d = &(*d)->next_) {
      if (*d == this) {
        *d = next_;
      }
    }
  }
  static void ProcessDataStreams() {
    for (DataStreamWork *d = data_streams; d; d=d->next_) {
      d->Loop();
    }
  }
protected:
  void fill() {
    if (!NVIC_IS_ACTIVE(IRQ_WAV))
      NVIC_TRIGGER_IRQ(IRQ_WAV);
  }

  virtual void Loop() = 0;
private:
  DataStreamWork* next_;
};

// N needs to be power of 2
template<class T, int N>
class BufferedDataStream : public DataStream<T>, DataStreamWork {
public:
  BufferedDataStream() : DataStreamWork() {
  }
  int read(T* buf, int bufsize) override {
    int copied = 0;
    while (bufsize) {
      size_t to_copy = buffered();
      if (!to_copy) break;
      to_copy = min(to_copy, bufsize);
      size_t start_pos = buf_start_ & (N-1);
      to_copy = min(to_copy, N - start_pos);
      memcpy(buf, buffer_ + start_pos, sizeof(T) * to_copy);
      copied += to_copy;
      buf_start_ += to_copy;
      buf += to_copy;
      bufsize -= to_copy;
    }
    fill();
    return copied;
  }
  size_t buffered() const {
    return buf_end_ - buf_start_;
  }
  size_t space_available() const {
    return N - buffered();
  }
private:
  void Loop() override {
    while (stream_) {
      size_t space = space_available();
      if (!space) break;
      size_t end_pos = buf_end_ & (N-1);
      size_t to_read = min(space, N - end_pos);
      int got = stream_->read(buffer_ + end_pos, sizeof(T) * to_read);
      if (!got) break;
      buf_end_ += got;
    }
  }
  DataStream<int16_t>* stream_ = 0;
  // Note, these are assumed to be atomic, 8-bit processors won't work.
  volatile size_t buf_start_ = 0;
  volatile size_t buf_end_ = 0;
  T buffer_[N];
};
#endif

class Effect;
Effect* all_effects = NULL;

char toLower(char x) {
  if (x >= 'A' && x <= 'Z') return x - 'A' + 'a';
  return x;
}

const char *startswith(const char *prefix, const char* x) {
  while (*prefix) {
    if (toLower(*x) != toLower(*prefix)) return nullptr;
    prefix++;
    x++;
  }
  return x;
}

int parse1hex(const char* x) {
  int ret = toLower(*x);
  if (ret > 'a') return ret - 'a' + 10;
  return ret - '0';
}

int parse2hex(const char* x) {
  return (parse1hex(x) << 4) | parse1hex(x+1);
}

bool endswith(const char *postfix, const char* x) {
  size_t l = strlen(x);
  if (l < strlen(postfix)) return false;
  x = x + l - strlen(postfix);
  while (*postfix) {
    if (toLower(*x) != toLower(*postfix)) return false;
    postfix++;
    x++;
  }
  return true;
}

char current_directory[128];

class Effect {
  public:
  Effect(const char* name) : name_(name) {
    next_ = all_effects;
    all_effects = this;
    reset();
  }

  void reset() {
    min_file_ = 20000;
    max_file_ = -1;
    digits_ = 0;
    unnumbered_file_found_ = false;
    subdirs_ = false;
  }

  void Scan(const char *filename) {
    size_t len = strlen(name_);
    const char *rest = startswith(name_, filename);
    if (!rest) return;
    if (*rest == '/') {
      subdirs_ = true;
      const char *tmp = startswith(name_, rest + 1);
      if (!tmp) return;
      rest = tmp;
    }

    int n = -1;
    if (*rest == '.') {
      unnumbered_file_found_ = true;
    } else {
      char *end;
      n = strtol(rest, &end, 0);
      if (n <= 0) return;
      max_file_ = max(max_file_, n);
      min_file_ = min(min_file_, n);
      if (*rest == '0') {
        digits_ = end - filename + len;
      }
    }
  }

  void Show() {
    if (files_found()) {
      Serial.print("Found ");
      Serial.print(name_);
      Serial.print(" files: ");
      if (min_file_ <= max_file_) {
	Serial.print(min_file_);
	Serial.print("-");
	Serial.print(max_file_);
	if (digits_) {
	  Serial.print(" using ");
	  Serial.print(digits_);
	  Serial.print(" digits");
	}
	if (unnumbered_file_found_) {
	  Serial.print(" + ");
	}
      }
      if (unnumbered_file_found_) {
	Serial.print("one unnumbered file");
      }
      if (subdirs_) {
	Serial.print(" in subdirs");
      }
      Serial.println("");
    }
  }

  static void ShowAll() {
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Show();
    }
    Serial.println("Done listing effects.");
  }

  size_t files_found() const {
    size_t ret = 0;
    if (min_file_ <= max_file_) {
      ret += max_file_ - min_file_ + 1;
    }
    if (unnumbered_file_found_) {
      ret ++;
    }
    return ret;
  }


  bool Play(char *filename) {
#ifdef ENABLE_AUDIO
    int num_files = files_found();
    if (num_files < 1) return false;
    int n = rand() % num_files;
    strcpy(filename, current_directory);
    if (current_directory[0])
      strcat(filename, "/");
    strcat(filename, name_);
    if (subdirs_) {
      strcat(filename, "/");
      strcat(filename, name_);
    }
    n += min_file_;
    // n can be max_file_ + 1, which means pick the file without digits.
    if (n <= max_file_) {
      char buf[12];
      itoa(n, buf, 10);
      char *j = filename + strlen(filename);
      int num_digits = strlen(buf);
      while (num_digits < digits_) {
        *j = '0';
        ++j;
        num_digits++;
      }
      memcpy(j, buf, strlen(buf) + 1);
    }

    strcat(filename, ".wav");
    Serial.print("Playing ");
    Serial.println(filename);
    return true;
#else
    return false;
#endif
  }

  static void ScanAll(const char* filename) {
    if (!endswith(".wav", filename))
      return;
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Scan(filename);
    }
  }

  static void ScanDirectory(const char *directory) {
    Serial.print("Scanning sound font: ");
    Serial.println(directory);
    for (Effect* e = all_effects; e; e = e->next_) {
      e->reset();
    }

    // Scan serial flash.
    SerialFlashChip::opendir();
    uint32_t size;
    char filename[128];
    while (SerialFlashChip::readdir(filename, sizeof(filename), size)) {
      const char* f = startswith(directory, filename);
      if (f) ScanAll(f);
    }

    File dir = SD.open(directory);
    if (dir) {
      while (File f = dir.openNextFile()) {
	if (f.isDirectory()) {
	  char fname[128];
	  strcpy(fname, f.name());
	  strcat(fname, "/");
	  char* fend = fname + strlen(fname);
	  while (File f2 = dir.openNextFile()) {
	    strcpy(fend, f2.name());
	    ScanAll(fname);
	    f2.close();
	  }
	} else {
	  ScanAll(f.name());
	}
	f.close();
      }
    }
  };

private:
  Effect* next_;

  // Minimum file number.
  int min_file_;

  // Maximum file number.
  int max_file_;

  // Leading zeroes are used to make it this many digits.
  int digits_;

  // If true. there is an un-numbered file as well.
  bool unnumbered_file_found_;

  // Files are in subdirectories, like "lock/lockNN.wav"
  bool subdirs_;

  // All files must start with this prefix.
  const char* name_;
};

#define EFFECT(X) Effect X(#X)

// Monophonic fonts
EFFECT(boot);
EFFECT(swing);
EFFECT(hum);
EFFECT(poweron);
EFFECT(poweroff);
EFFECT(pwroff);
EFFECT(clash);
EFFECT(force);
EFFECT(stab);
EFFECT(blaster);
EFFECT(lockup);
EFFECT(poweronf);
EFFECT(font);

// Polyphonic fonts
EFFECT(blst);
EFFECT(clsh);
EFFECT(in);
EFFECT(out);
EFFECT(lock);
EFFECT(swng);
EFFECT(slsh);

#ifdef ENABLE_AUDIO

// Simple upsampler code, doubles the number of samples with
// 2-lobe lanczos upsampling.
#define C1 24757
#define C2 -8191

#define UPSAMPLE_FUNC(NAME, EMIT)                               \
  void NAME(int16_t sample) {                                   \
    upsample_buf_##NAME##_a_ = upsample_buf_##NAME##_b_;        \
    upsample_buf_##NAME##_b_ = upsample_buf_##NAME##_c_;        \
    upsample_buf_##NAME##_c_ = upsample_buf_##NAME##_d_;        \
    upsample_buf_##NAME##_d_ = sample;                          \
    EMIT((upsample_buf_##NAME##_a_ * C2 +                       \
          upsample_buf_##NAME##_b_ * C1 +                       \
          upsample_buf_##NAME##_c_ * C1 +                       \
          upsample_buf_##NAME##_d_ * C2) >> 15);                \
    EMIT(upsample_buf_##NAME##_c_);                             \
  }                                                             \
  void clear_##NAME() {                                         \
    upsample_buf_##NAME##_a_ = 0;                               \
    upsample_buf_##NAME##_b_ = 0;                               \
    upsample_buf_##NAME##_c_ = 0;                               \
    upsample_buf_##NAME##_d_ = 0;                               \
  }                                                             \
  int16_t upsample_buf_##NAME##_a_ = 0;                         \
  int16_t upsample_buf_##NAME##_b_ = 0;                         \
  int16_t upsample_buf_##NAME##_c_ = 0;                         \
  int16_t upsample_buf_##NAME##_d_ = 0

#define DOWNSAMPLE_FUNC(NAME, EMIT)                     \
  void NAME(int16_t sample) {                           \
    if (downsample_flag_##NAME##_) {                    \
      EMIT((downsample_buf_##NAME##_ + sample) >> 1);   \
      downsample_flag_##NAME##_ = false;                \
    } else {                                            \
      downsample_buf_##NAME##_ = sample;                \
      downsample_flag_##NAME##_ = true;                 \
    }                                                   \
  }                                                     \
  void clear_##NAME() {                                 \
    downsample_buf_##NAME##_ = 0;                       \
    downsample_flag_##NAME##_ = false;                  \
  }                                                     \
  int16_t downsample_buf_##NAME##_ = 0;                 \
  bool downsample_flag_##NAME##_ = false


class PlayWav : StateMachine, public DataStream<int16_t> {
public:
  void Play(const char* filename) {
    strcpy(filename_, filename);
    run_ = true;
  }

  void PlayOnce(Effect* effect) {
    effect->Play(filename_);
    run_ = true;
  }
  void PlayLoop(Effect* effect) {
    effect_ = effect;
  }

  void Stop() {
    reset_state_machine();
    effect_ = nullptr;
    run_ = false;
  }

  bool isPlaying() const {
    return run_;
  }

private:
  void Emit1(uint16_t sample) {
    samples_[num_samples_++] = sample;
  }
  UPSAMPLE_FUNC(Emit2, Emit1);
  UPSAMPLE_FUNC(Emit4, Emit2);
  DOWNSAMPLE_FUNC(Emit05, Emit1);

  uint32_t header(int n) const {
    return ((uint32_t *)buffer)[n];
  }

  template<int bits> int16_t read2() {
    if (bits == 8) return *(ptr_++) << 8;
    return *((*((int16_t**)&ptr_))++);
  }

  template<int bits, int channels, int rate>
  void DecodeBytes4(int samples) {
    samples = samples * rate / AUDIO_RATE;
    for (int i = 0; i < samples; i++) {
      int v = 0;
      if (channels == 1) {
	v = read2<bits>();
      } else {
	int v = read2<bits>();
	v += read2<bits>();
	v >>= 1;
      }
      if (rate == AUDIO_RATE) {
        Emit1(v);
      } else if (rate == AUDIO_RATE / 2) {
        Emit2(v);
      } else if (rate == AUDIO_RATE / 4) {
        Emit4(v);
      } else if (rate == AUDIO_RATE * 2) {
        Emit05(v);
      } else {
	Serial.println("Unsupported rate.");
	Stop();
      }
    }
  }

  template<int bits, int channels>
  void DecodeBytes3(int samples) {
    if (rate_ == 44100)
      DecodeBytes4<bits, channels, 44100>(samples);
    else if (rate_ == 22050)
      DecodeBytes4<bits, channels, 22050>(samples);
    else if (rate_ == 11025)
      DecodeBytes4<bits, channels, 11025>(samples);
  }

  template<int bits>
  void DecodeBytes2(int samples) {
    if (channels_ == 1) DecodeBytes3<bits, 1>(samples);
    else DecodeBytes3<bits, 2>(samples >> 1);
  }

  void DecodeBytes(int bytes) {
    if (bits_ == 8) DecodeBytes2<8>(bytes);
    else DecodeBytes2<16>(bytes >> 1);
  }

  int ReadFile(int n) {
    if (sf_file_) {
      return sf_file_.read(buffer, n);
    } else {
      return sd_file_.read(buffer, n);
    }
  }

  void Skip(int n) {
    if (sf_file_) {
      sf_file_.seek(sf_file_.position() + n);
    } else {
      sd_file_.seek(sd_file_.position() + n);
    }
  }

  int AlignRead(int n) {
    if (sf_file_) return n;
    int next_block = (sd_file_.position() + 512u) & ~511u;
    int bytes_to_end_of_block = next_block - sd_file_.position();
    return min(n, bytes_to_end_of_block);
  }

  void loop() {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!run_ && !effect_) YIELD();
      if (!run_) {
	if (!effect_->Play(filename_)) {
	  goto fail;
	}
        run_ = true;
      }
      sf_file_ = SerialFlashChip::open(filename_);
      if (!sf_file_) {
        sd_file_ = SD.open(filename_);
        if (!sd_file_) {
	  Serial.print("File ");
	  Serial.print(filename_);
	  Serial.println(" not found.");
	  goto fail;
        }
      }
      if (ReadFile(20) != 20) {
	Serial.println("Failed to read 20 bytes.");
	goto fail;
      }
      if (header(0) != 0x46464952 &&
          header(2) != 0x45564157 &&
          header(3) != 0x20746D66 &&
          header(4) < 16) {
	Serial.println("Headers don't match.");
	YIELD();
	goto fail;
      }
      tmp_ = header(4);
      if (tmp_ != ReadFile(tmp_)) {
	Serial.println("Read failed.");
	goto fail;
      }
      if ((header(0) & 0xffff) != 1) {
	Serial.println("Wrong format.");
	goto fail;
      }
      channels_ = header(0) >> 16;
      rate_ = header(1);
      bits_ = header(3) >> 16;
      Serial.print("chan: ");
      Serial.print(channels_);
      Serial.print(" rate: ");
      Serial.print(rate_);
      Serial.print(" bits: ");
      Serial.println(bits_);

      bytes_to_fill_samples_ =
	NELEM(samples_) * channels_ * (bits_ >> 3) * rate_ / AUDIO_RATE;

      while (ReadFile(8) == 8) {
        len_ = header(1);
        if (header(0) != 0x61746164) {
          Skip(len_);
          continue;
        }
        while (len_) {
	  bytes_to_decode_ =
	    ReadFile(AlignRead(min(len_, sizeof(buffer))));
	  len_ -= bytes_to_decode_;
	  ptr_ = buffer;
          while (bytes_to_decode_) {
	    {
	      int to_decode = min(bytes_to_decode_, bytes_to_fill_samples_);
	      DecodeBytes(to_decode);
	      bytes_to_decode_ -= to_decode;
	    }

            while (written_ < num_samples_) {
              // Preload should go to here...
              while (to_read_ == 0) YIELD();

              int n = min(num_samples_ - written_, to_read_);
              memcpy(dest_, samples_ + written_, n * 2);
              dest_ += n;
              written_ += n;
              to_read_ -= n;
            }
            written_ = num_samples_ = 0;
          }
        }
      }

      // EOF;
      run_ = false;
      continue;

  fail:
      run_ = false;
      YIELD();
    }

    STATE_MACHINE_END();
  }

public:
  // Called from interrupt handler.
  int read(int16_t* dest, int to_read) override {
    dest_ = dest;
    to_read_ = to_read;
    loop();
    return dest_ - dest;
  }

private:
  volatile bool run_ = false;
  Effect* volatile effect_ = nullptr;
  char filename_[128];
  File sd_file_;
  SerialFlashFile sf_file_;
  int16_t* dest_ = nullptr;
  int to_read_ = 0;
  int tmp_;

  int rate_;
  int channels_;
  int bits_;
  int bytes_to_fill_samples_;

  int bytes_to_decode_ = 0;
  size_t len_ = 0;
  char* ptr_;
  char buffer[512]  __attribute__((aligned(4)));

  // Number of samples_ in samples that has been
  // sent out already.
  int written_ = 0;
  
  // Number of samples in samples_
  int num_samples_ = 0;
  int16_t samples_[32];
};

PlayWav wav_players[5];
#endif

class MonophonicFont : public DataStream<int16_t>, SaberBase {
public:
  MonophonicFont() : SaberBase(NOLINK) {
  }
  void Activate() {
    SaberBase::Link();
    dac.SetStream(this);
  }
  void Deactivate() {
    dac.SetStream(nullptr);
    SaberBase::Unlink();
  }
  int read(int16_t* data, int elements) override {
    int16_t *p = data;
    int to_read = elements;
    if (current_ < 0) return 0;
    while (to_read) {
      int num = wav_players[current_].read(data, to_read);
      to_read -= num;
      data += elements;
      if (num < to_read) {
	// end of file.
	current_ = -1;
	while (num < to_read) {
	  *(data++) = 0;
	  to_read --;
	}
      }
    }
    if (fadeto_ >= 0) {
      p = data;
      to_read = elements;
      while (to_read) {
        int16_t tmp[32];
        int n = min(to_read, (int)NELEM(tmp));
        int num = wav_players[fadeto_].read(tmp, n);
        while (num < n) tmp[num++] = 0;
        for (int i = 0; i < num; i++) {
          p[i] = (p[i] * fade_ + tmp[i] * (256 - fade_)) >> 8;
          if (fade_) fade_ -= 2;
        }
        to_read -= n;
	p += n;
      }
      if (!fade_) {
	wav_players[current_].Stop();
        current_ = fadeto_;
        fadeto_ = -1;
      }
    }
    return elements;
  }

  void Preload() {
    // Implement preloading.
    // NVIC_TRIGGER_IRQ(IRQ_WAV);
  }

  virtual void Play(Effect* f) {
    if (current_ == -1) return;
    // find free unit
    if (fadeto_ != -1) {
      // Need to finish fading to the previous unit first. (~2.5ms)
      return;
    }
    int unit = 1 - current_;
    wav_players[unit].PlayOnce(f);
    wav_players[unit].PlayLoop(&hum);
    Preload();
    fadeto_ = unit;
    fade_ = 254;
  }

  void On() override {
    if (current_ == -1) {
      wav_players[0].PlayOnce(&poweron);
      wav_players[0].PlayLoop(&hum);
      Preload();
      current_ = 0;
    }
  }
  void Off() override {
    while (fadeto_ != -1) delay(1);
    int unit = 1 - current_;
    wav_players[unit].PlayOnce(&poweroff);
    Preload();
    fadeto_ = unit;
    fade_ = 254;
  }
  void Clash() override { Play(&clash); }
  void Swing() override { Play(&swing); }
  void Stab() override { Play(&stab); }
  void Force() override { Play(&force); }

protected:
  volatile int current_= -1;
  volatile int fadeto_ = -1;
  int fade_;
};

MonophonicFont monophonic_font;

class PolyphonicFont : public MonophonicFont {

  void On() override {
    if (current_ == -1) {
      wav_players[0].PlayOnce(&out);
      wav_players[0].PlayLoop(&hum);
      Preload();
      current_ = 0;
    }
  }

  void Off() override {
    while (fadeto_ != -1) delay(1);
    int unit = 1 - current_;
    wav_players[unit].PlayOnce(&in);
    Preload();
    fadeto_ = unit;
    fade_ = 254;
  }

  void Play(Effect* f) override {
    // Find a free wave playback unit.
    for (size_t unit = 2; unit < NELEM(wav_players); unit++) {
      if (!wav_players[unit].isPlaying()) {
	wav_players[unit].PlayOnce(f);
	Preload();
	return;
      }
    }
    // Implement preloading.
    // NVIC_TRIGGER_IRQ(IRQ_WAV);
  }
  void Clash() override { Play(&clsh); }
  void Swing() override { Play(&swng); }
  void Stab() override { Play(&stab); }
  void Force() override { Play(&force); }
};

PolyphonicFont polyphonic_font;

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

#include <Arduino.h>
#include "DMAChannel.h"

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
#define WS2811_580kHz 0x20      // PL9823


class MonopodWS2811 {
public:
  void begin(uint32_t numPerStrip,
             void *frameBuf,
             void *drawBuf,
             uint8_t config = WS2811_GRB);
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

static uint8_t ones = 0x20;  // pin 20
static volatile uint8_t update_in_progress = 0;
static uint32_t update_completed_at = 0;


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

static uint8_t analog_write_res = 8;

void setFTM_Timer(uint8_t ch1, uint8_t ch2, float frequency)
{
  uint32_t prescale, mod, ftmClock, ftmClockSource;
  float minfreq;

  if (frequency < (float)(F_BUS >> 7) / 65536.0f) {     //If frequency is too low for working with F_TIMER:
    ftmClockSource = 2;                 //Use alternative 31250Hz clock source
    ftmClock = 31250;                   //Set variable for the actual timer clock frequency
  } else {                                                //Else do as before:
    ftmClockSource = 1;                 //Use default F_Timer clock source
    ftmClock = F_BUS;                    //Set variable for the actual timer clock frequency
  }

  for (prescale = 0; prescale < 7; prescale++) {
    minfreq = (float)(ftmClock >> prescale) / 65536.0f;    //Use ftmClock instead of F_TIMER
    if (frequency >= minfreq) break;
  }

  mod = (float)(ftmClock >> prescale) / frequency - 0.5f;    //Use ftmClock instead of F_TIMER
  if (mod > 65535) mod = 65535;

  FTM0_SC = 0; // stop FTM until setting of registers are ready
  FTM0_CNTIN = 0; // initial value for counter. CNT will be set to this value, if any value is written to FTMx_CNT
  FTM0_CNT = 0;
  FTM0_MOD = mod;

  // I don't know why, but the following code leads to a very short first pulse. Shifting the compare values to the end looks much better
  // uint32_t cval;
  // FTM0_C0V = 1;  // 0 is not working -> add 1 to every compare value.
  // cval = ((uint32_t)ch1 * (uint32_t)(mod + 1)) >> analog_write_res;
  // FTM0_C1V = cval +1;
  // cval = ((uint32_t)ch2 * (uint32_t)(mod + 1)) >> analog_write_res;
  // FTM0_C2V = cval +1;

  // Shifting the compare values to the end leads to a perfect first (and last) pulse:
  uint32_t cval1 = ((uint32_t)ch1 * (uint32_t)(mod + 1)) >> analog_write_res;
  uint32_t cval2 = ((uint32_t)ch2 * (uint32_t)(mod + 1)) >> analog_write_res;
  FTM0_C0V = mod - (cval2 - 0);
  FTM0_C1V = mod - (cval2 - cval1);
  FTM0_C2V = mod;

  FTM0_C0SC = FTM_CSC_DMA | FTM_CSC_CHIE | 0x28;
  FTM0_C1SC = FTM_CSC_DMA | FTM_CSC_CHIE | 0x28;
  FTM0_C2SC = FTM_CSC_DMA | FTM_CSC_CHIE | 0x28;

  FTM0_SC = FTM_SC_CLKS(ftmClockSource) | FTM_SC_PS(prescale);    //Use ftmClockSource instead of 1. Start FTM-Timer.
  //with 96MHz Teensy: prescale 0, mod 59, ftmClockSource 1, cval1 14, cval2 41
}

#ifdef INVERT_WS2811
#define WS2811_PORT_CLEAR GPIOD_PSOR
#define WS2811_PORT_SET   GPIOD_PCOR
#else
#define WS2811_PORT_CLEAR GPIOD_PCOR
#define WS2811_PORT_SET   GPIOD_PSOR
#endif

void MonopodWS2811::begin(uint32_t numPerStrip,
                          void *frameBuf,
                          void *drawBuf,
                          uint8_t config)
{
  stripLen = numPerStrip;
  frameBuffer = frameBuf;
  drawBuffer = drawBuf;
  params = config;

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
  WS2811_PORT_CLEAR = ones;
  if (ones & 1)   pinMode(2, OUTPUT);   // strip #1
  if (ones & 2)   pinMode(14, OUTPUT);  // strip #2
  if (ones & 4)   pinMode(7, OUTPUT);   // strip #3
  if (ones & 8)   pinMode(8, OUTPUT);   // strip #4
  if (ones & 16)  pinMode(6, OUTPUT);   // strip #5
  if (ones & 32)  pinMode(20, OUTPUT);  // strip #6
  if (ones & 64)  pinMode(21, OUTPUT);  // strip #7
  if (ones & 128) pinMode(5, OUTPUT);   // strip #8

  int t0h = WS2811_TIMING_T0H;
  int t1h = WS2811_TIMING_T1H;
  switch (params & 0xF0) {
    case WS2811_400kHz:
      frequency = 400000;
      break;

    case WS2811_800kHz:
      frequency = 740000;
      break;

    case WS2811_580kHz:
      frequency = 580000;
      break;
  }

#ifdef USE_FTM_TIMER
  setFTM_Timer(t0h, t1h, frequency);
#else  // USE_FTM_TIMER
  // create the two waveforms for WS2811 low and high bits
  // FOOPIN Must be 4 or 17, and it can't be 4....
  // Need help changing 4 to 17...
#define FOOPIN 4
#define BARPIN 3
#define FOOCAT32(X,Y,Z) X##Y##Z
#define FOOCAT3(X,Y,Z) FOOCAT32(X,Y,Z)

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

#endif  // USE_FTM_TIMER

  // DMA channel #1 sets WS2811 high at the beginning of each cycle
  dma1.source(ones);
  dma1.destination(WS2811_PORT_SET);
  dma1.transferSize(1);
  dma1.transferCount(bufsize);
  dma1.disableOnCompletion();

  // DMA channel #2 writes the pixel data at 20% of the cycle
  dma2.sourceBuffer((uint8_t *)frameBuffer, bufsize);
  dma2.destination(WS2811_PORT_CLEAR);
  dma2.transferSize(1);
  dma2.transferCount(bufsize);
  dma2.disableOnCompletion();

  // DMA channel #3 clear all the pins low at 48% of the cycle
  dma3.source(ones);
  dma3.destination(WS2811_PORT_CLEAR);
  dma3.transferSize(1);
  dma3.transferCount(bufsize);
  dma3.disableOnCompletion();
  dma3.interruptAtCompletion();

#ifdef __MK20DX256__
  MCM_CR = MCM_CR_SRAMLAP(1) | MCM_CR_SRAMUAP(0);
  AXBS_PRS0 = 0x1032;
#endif

#ifdef USE_FTM_TIMER
  dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH0);
  dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH1);
  dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM0_CH2);
#elif defined(KINETISK)
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

#ifdef USE_FTM_TIMER
        uint32_t sc = FTM0_SC;
        //digitalWriteFast(1, HIGH); // oscilloscope trigger
  
        //noInterrupts(); // This code is not time critical anymore. IRQs can stay on. 
        // We disable the FTM Timer, reset it to its initial counter value and clear all irq-flags. 
        // Clearing irqs is a bit tricky, because with DMA enabled, only the DMA can clear them. 
        // We have to disable DMA, reset the irq-flags and enable DMA once again.
        update_in_progress = 1;
        FTM0_SC = sc & 0xE7;    // stop FTM timer
        
        FTM0_CNT = 0; // writing any value to CNT-register will load the CNTIN value!
        
        FTM0_C0SC = 0; // disable DMA transfer. It has to be done, because we can't reset the CHnF bit while DMA is enabled
        FTM0_C1SC = 0;
        FTM0_C2SC = 0;
        
        FTM0_STATUS; // read status and write 0x00 to it, clears all pending IRQs
        FTM0_STATUS = 0x00;
        
        FTM0_C0SC = FTM_CSC_DMA | FTM_CSC_CHIE | 0x28; 
        FTM0_C1SC = FTM_CSC_DMA | FTM_CSC_CHIE | 0x28;
        FTM0_C2SC = FTM_CSC_DMA | FTM_CSC_CHIE | 0x28;
        
        dma1.enable();
        dma2.enable();        // enable all 3 DMA channels
        dma3.enable();
        //interrupts();
        //digitalWriteFast(1, LOW);
        // wait for WS2811 reset
        while (micros() - update_completed_at < 50) ; // moved to the end, because everything else can be done before.
        FTM0_SC = sc;        // restart FTM timer

#elif defined(KINETISK)
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
        FTM1_SC = sc & 0xE7;    // stop FTM1 timer (hopefully before it rolls over)
        //digitalWriteFast(9, HIGH); // oscilloscope trigger
        PORTB_ISFR = (1<<0);    // clear any prior rising edge
        PORTC_ISFR = (1<<0);    // clear any prior low duty falling edge
        PORTA_ISFR = (1<<13);   // clear any prior high duty falling edge
        dma1.enable();
        dma2.enable();          // enable all 3 DMA channels
        dma3.enable();
        FTM1_SC = sc;           // restart FTM1 timer
        //digitalWriteFast(9, LOW);
#elif defined(KINETISL)
        uint32_t sc = FTM2_SC;
        uint32_t cv = FTM2_C1V;
        noInterrupts();
        update_in_progress = 1;
        while (FTM2_CNT <= cv) ;
        while (FTM2_CNT > cv) ; // wait for beginning of an 800 kHz cycle
        while (FTM2_CNT < cv) ;
        FTM2_SC = 0;            // stop FTM2 timer (hopefully before it rolls over)
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
        FTM2_C0SC = 0xA9;       // clear any previous pending DMA requests
        FTM2_C1SC = 0xA9;
        // clear any prior pending DMA requests
        dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_OV);
        dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH0);
        dma3.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM2_CH1);
        //GPIOD_PTOR = 0xFF;
        //GPIOD_PTOR = 0xFF;
        dma1.enable();
        dma2.enable();          // enable all 3 DMA channels
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
        uint32_t offset, mask;
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
//      strip = num / stripLen;  // Cortex-M4 has 2 cycle unsigned divide :-)
//      offset = num % stripLen;

        // strip = 5;
        offset = num;
        // bit = (1<<strip);
        bit = ones;
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
        uint32_t offset, mask;
        uint8_t bit, *p;
        int color=0;

//      strip = num / stripLen;
//      offset = num % stripLen;
        // strip = 5;
        offset = num;
        // bit = (1<<strip);
        bit = ones;
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

// TODO: rename to maxledsPerStrip
const unsigned int ledsPerStrip = 200;
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
MonopodWS2811 monopodws;

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
  void set(uint8_t r, uint8_t g, uint8_t b) {
    base.set(r, g, b);
    shimmer = base;
  }
};

// TODO: Support changing number of LEDs at runtime

// WS2811-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class WS2811_Blade : public SaberBase, CommandParser, Looper {
public:
  enum State {
    BLADE_OFF,
    BLADE_TURNING_ON,
    BLADE_ON,
    BLADE_TURNING_OFF,
    BLADE_ALMOST_OFF,
    BLADE_TEST,
    BLADE_SHOWOFF,
  };
  WS2811_Blade() : SaberBase(NOLINK),
		   CommandParser(NOLINK),
		   Looper(NOLINK),
		   state_(BLADE_OFF) {
    // TODO: colors should be set in in blade identify code.
    settings_.set(0,0,255);
    settings_.clash.set(255, 255, 255);
  }

  void Power(bool on) {
    pinMode(bladePowerPin1, OUTPUT);
    pinMode(bladePowerPin2, OUTPUT);
    pinMode(bladePowerPin3, OUTPUT);
    digitalWrite(bladePowerPin1, on?HIGH:LOW);
    digitalWrite(bladePowerPin2, on?HIGH:LOW);
    digitalWrite(bladePowerPin3, on?HIGH:LOW);
//    pinMode(bladePin, on ? OUTPUT : INPUT);
  }
  void Activate(int num_leds, uint8_t config) {
    Power(true);
    delay(10);
    monopodws.begin(num_leds, displayMemory, drawingMemory, config);
    monopodws.show();  // Make it black
    monopodws.show();  // Make it black
    monopodws.show();  // Make it black
    while (monopodws.busy());
    Power(false);
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link();
  }

  void IsOn(bool* on) override {
    if (state_ != BLADE_OFF) *on = true;
  }
  void On() override {
    Power(true);
    delay(10);
    state_ = BLADE_TURNING_ON;
    event_millis_ = millis();
    event_length_ = 200; // Guess
  }
  void Off() override {
    state_ = BLADE_TURNING_OFF;
    event_millis_ = millis();
    event_length_ = 200; // Guess
  }

  void Clash() override { clash_=true; }
  void Lockup() override {  }

  void Top() override {
    if (!millis_sum_) return;
    Serial.print("blade fps: ");
    Serial.println(updates_ * 1000.0 / millis_sum_);
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
      if (!strcmp(arg, "poweroff")) {
	Power(false);
	state_ = BLADE_OFF;
        return true;
      }
      if (!strcmp(arg, "float")) {
	pinMode(bladePowerPin1, INPUT);
	pinMode(bladePowerPin2, INPUT);
	pinMode(bladePowerPin3, INPUT);
	state_ = BLADE_OFF;
        return true;
      }
      if (!strcmp(arg, "test")) {
        state_ = BLADE_TEST;
        return true;
      }
      if (!strcmp(arg, "showoff")) {
        state_ = BLADE_SHOWOFF;
        return true;
      }
    }
    if (!strcmp(cmd, "red")) {
      settings_.set(255,0,0);
      return true;
    }
    if (!strcmp(cmd, "green")) {
      settings_.set(0,255,0);
      return true;
    }
    if (!strcmp(cmd, "blue")) {
      settings_.set(0,0,255);
      return true;
    }
    if (!strcmp(cmd, "yellow")) {
      settings_.set(255,255,0);
      return true;
    }
    if (!strcmp(cmd, "cyan")) {
      settings_.set(0,255,255);
      return true;
    }
    if (!strcmp(cmd, "magenta") || !strcmp(cmd, "purple")) {
      settings_.set(255,0,255);
      return true;
    }
    if (!strcmp(cmd, "white")) {
      settings_.set(255,255,255);
      return true;
    }
    if (!strcmp(cmd, "color") && arg) {
      switch (strlen(arg)) {
        case 3:
           settings_.set(parse1hex(arg) * 0x11,
			 parse1hex(arg + 1) * 0x11,
			 parse1hex(arg + 2) * 0x11);
	   return true;
	case 6:
	  settings_.set(parse2hex(arg),
			parse2hex(arg + 2),
			parse2hex(arg + 4));
	  return true;
      }
    }
    return false;
  }

  BladeSettings settings_;

protected:
  void Loop() override {
    if (monopodws.busy()) return;
    if (state_ == BLADE_OFF) {
      Power(false);
      last_millis_ = 0;
      return;
    }
    monopodws.show();
    int m = millis();
    if (last_millis_) {
      millis_sum_ += m - last_millis_;
      updates_ ++;
      if (updates_ > 1000) {
        updates_ /= 2;
        millis_sum_ /= 2;
      }
    }
    last_millis_ = m;
    int thres = ledsPerStrip * 256 * (m - event_millis_) / event_length_;
    if (thres > (int)((ledsPerStrip + 2) * 256)) {
      switch (state_) {
        case BLADE_TURNING_ON:
          state_ = BLADE_ON;
          break;
        case BLADE_TURNING_OFF:
          state_ = BLADE_ALMOST_OFF;
          break;
        case BLADE_ALMOST_OFF:
          state_ = BLADE_OFF;
          break;
        default: break;
      }
    }
    for (unsigned int i = 0; i < ledsPerStrip; i++) {
      Color c;
      if (clash_) {
        c = settings_.clash.get(i);
      } else {
        c = settings_.base.get(i);
      }
      // TODO: shimmer more when moving.
      // TODO: Other shimmer styles
      c = c.mix(settings_.shimmer.get(i), rand() & 0xFF);

      switch (state_) {
        case BLADE_SHOWOFF:
          c = Color(sin(i*0.23 - m*0.013) * 127 + 127,
                    sin(i*0.17 - m*0.005) * 127 + 127,
                    sin(i*0.37 - m*0.007) * 127 + 127);
          break;
        case BLADE_TEST: {
          switch((((m >> 5) + i)/10) % 3) {
//          switch((i/10) % 3) {
            case 0: c = Color(255,0,0); break;
            case 1: c = Color(0,255,0); break;
            case 2: c = Color(0,0,255); break;
          }
          break;
        }
        case BLADE_OFF:
        case BLADE_ALMOST_OFF:
          c = Color();
          break;
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
    clash_ = false;
  }
  
private:
  State state_;
  bool clash_ = false;
  int event_millis_;
  int event_length_;
  int last_millis_;
  int updates_ = 0;
  int millis_sum_ = 0;
};

// Simple blade, LED string or LED star with optional flash on clash.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class Simple_Blade : public SaberBase, CommandParser, Looper {
public:
  enum State {
    BLADE_OFF,
    BLADE_TURNING_ON,
    BLADE_ON,
    BLADE_TURNING_OFF,
  };
  Simple_Blade() : SaberBase(NOLINK),
		   CommandParser(NOLINK),
		   Looper(NOLINK),
		   state_(BLADE_OFF) {
  }
  void Activate() {
    analogWriteFrequency(bladePowerPin1, 1000);
    analogWriteFrequency(bladePowerPin2, 1000);
    analogWriteFrequency(bladePowerPin3, 1000);
    analogWrite(bladePowerPin1, 0);  // make it black
    analogWrite(bladePowerPin2, 0);  // make it black
    analogWrite(bladePowerPin3, 0);  // make it black
    analogWrite(bladePin, 0);   // Turn off FoC
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link();
  }
  void IsOn(bool *on) override {
    if (state_ != BLADE_OFF) *on = true;
  }
  void On() override {
    state_ = BLADE_TURNING_ON;
    event_millis_ = millis();
    event_length_ = 100;
  }
  void Off() override {
    state_ = BLADE_TURNING_OFF;
    event_millis_ = millis();
    event_length_ = 100; // Guess
  }

  void Clash() override {
    analogWrite(bladePin, 255);
    clash_millis_ = millis();
  }
  void Lockup() override {  }

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
    }
    return false;
  }

protected:
  void Loop() override {
    if (state_ == BLADE_OFF) return;
    int thres = 256 * (millis() - event_millis_) / event_length_;
    if (thres > 254) {
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
    
    int level = 0;
    switch (state_) {
      case BLADE_OFF: level = 0;
      case BLADE_TURNING_ON:
        level = thres;
        break;
      case BLADE_TURNING_OFF:
        level = 255 - thres;
      case BLADE_ON:
        level = 255;
    }
    analogWrite(bladePowerPin1, level);
    analogWrite(bladePowerPin2, level);
    analogWrite(bladePowerPin3, level);

    // TODO: support multiple flashes.
    if (millis() - clash_millis_ > 10) {
      analogWrite(bladePin, 0);
    }
  }
  
private:
  State state_;
  int clash_millis_;
  int event_millis_;
  int event_length_;
};

// Global settings:
// Volume, Blade Style

WS2811_Blade ws2811_blade;
Simple_Blade simple_blade;

// Support for uploading files in TAR format.
class Tar {
  public:
    // Header description:
    //
    // Fieldno  Offset  len     Description
    // 
    // 0        0       100     Filename
    // 1        100     8       Mode (octal)
    // 2        108     8       uid (octal)
    // 3        116     8       gid (octal)
    // 4        124     12      size (octal)
    // 5        136     12      mtime (octal)
    // 6        148     8       chksum (octal)
    // 7        156     1       linkflag
    // 8        157     100     linkname
    // 9        257     8       magic
    // 10       265     32      (USTAR) uname
    // 11       297     32      (USTAR) gname
    // 12       329     8       devmajor (octal)
    // 13       337     8       devminor (octal)
    // 14       345     167     (USTAR) Long path
    //
    // magic can be any of:
    //   "ustar\0""00"  POSIX ustar (Version 0?).
    //   "ustar  \0"    GNU tar (POSIX draft)

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

// Scheduler class.
// It can currently only keep track of one outstanding event.
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
#ifdef ENABLE_AUDIO
          saber_synth.volume_.set(0);
          saber_synth.on_ = true;
#endif
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
  void Loop() override {
    CheckNext();
  }
};

Scheduler scheduler;

#if 1
// Simple button handler. Keeps track of clicks and lengths of pushes.
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
#ifdef ENABLE_SNOOZE
    snooze_config.pinMode(pin, INPUT_PULLUP, RISING);
#endif
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

  uint8_t pin_;
  const char* name_;
  bool pushed_;
  bool clicked_;
  int push_millis_;
};
#else

class Button {
public:
  virtual bool Read() = 0;
};

class DebouncedButton : Button, StateMachine {
public:
  void Update() {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!Button::Read()) YIELD();
      pushed_ = true;
      SLEEP(10);
      while (Button::Read()) YIELD();
      pushed_ = false;
      SLEEP(10);
    }
    STATE_MACHINE_END();
  }
  bool DebouncedRead() {
    Update();
    return pushed_;
  }

private:
  bool pushed = false;
};

// Simple button handler. Keeps track of clicks and lengths of pushes.
class Button : Looper, CommandParser, DebouncedButton, StateMachine {
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
#ifdef ENABLE_SNOOZE
    snooze_config.pinMode(pin, INPUT_PULLUP, RISING);
#endif
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
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!DebouncedRead()) YIELD();
      pushed_ = true;
      push_millis_ = millis();
      while (DebouncedRead()) YIELD();
      pushed_ = false;
      if (millis() - push_millis_ < 500) {
        clicked_ = true;
      }
    }
    STATE_MACHINE_END();
  }
  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      clicked_ = true;
      return true;
    }
    return false;
  }

  uint8_t pin_;
  const char* name_;
  bool pushed_;
  bool clicked_;
  int push_millis_;
};

#endif

// What follows is a copy of the touch.c code from the TensyDuino core library.
// That code originally implements the touchRead() function, I have modified it
// to become a class instead. That way reading the touch sensor can be
// initiated and polled without waiting around for it.

/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows 
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if defined(__MK20DX128__) || defined(__MK20DX256__)
// These settings give approx 0.02 pF sensitivity and 1200 pF range
// Lower current, higher number of scans, and higher prescaler
// increase sensitivity, but the trade-off is longer measurement
// time and decreased range.
#define CURRENT   2 // 0 to 15 - current to use, value is 2*(current+1)
#define NSCAN     9 // number of times to scan, 0 to 31, value is nscan+1
#define PRESCALE  2 // prescaler, 0 to 7 - value is 2^(prescaler+1)
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
255, 255,  11,   5
};

#elif defined(__MK66FX1M0__)
#define CURRENT   2
#define NSCAN     9
#define PRESCALE  2
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255, 255, 255,  11,
 12, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

#elif defined(__MKL26Z64__)
#define NSCAN     9
#define PRESCALE  2
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255
};

#endif

// Note, there can currently only be one of these.
// If we need more, we need a time-sharing system.
class TouchButton : Looper, CommandParser {
public:
  TouchButton(int pin, int threshold, const char* name)
    : Looper(),
      CommandParser(),
      pin_(pin),
      name_(name),
      threshold_(threshold),
      pushed_(false),
      clicked_(false),
      push_millis_(0) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_config.pinMode(pin, TSI, threshold);
#endif
#if defined(__MK64FX512__)
    Serial.println("Touch sensor not supported!\n");
#endif
    if (pin >= NUM_DIGITAL_PINS) {
      Serial.println("touch pin out of range");
      return;
    }
    if (pin2tsi[pin_] == 255) {
      Serial.println("Not a touch-capable pin!");
    } 
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
  void Update(int value) {
    if (print_next_) {
      Serial.print("Touch ");
      Serial.print(name_);
      Serial.print(" = ");
      Serial.println(value);
      print_next_ = false;
    }
    if (value > threshold_) {
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

  void BeginRead() {
    // Copied from touch.c
    int32_t ch = pin2tsi[pin_];
    *portConfigRegister(pin_) = PORT_PCR_MUX(0);
    SIM_SCGC5 |= SIM_SCGC5_TSI;
#if defined(KINETISK)
    TSI0_GENCS = 0;
    TSI0_PEN = (1 << ch);
    TSI0_SCANC = TSI_SCANC_REFCHRG(3) | TSI_SCANC_EXTCHRG(CURRENT);
    TSI0_GENCS = TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_PS(PRESCALE) | TSI_GENCS_TSIEN | TSI_GENCS_SWTS;
#elif defined(KINETISL)
    TSI0_GENCS = TSI_GENCS_REFCHRG(4) | TSI_GENCS_EXTCHRG(3) | TSI_GENCS_PS(PRESCALE)
      | TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_TSIEN | TSI_GENCS_EOSF;
    TSI0_DATA = TSI_DATA_TSICH(ch) | TSI_DATA_SWTS;
#endif
    begin_read_micros_ = micros();
  }

  void Setup() override {
    BeginRead();
  }

  void Loop() override {
    if (monitor.ShouldPrint(Monitoring::MonitorTouch)) {
      print_next_ = true;
    }
    if (micros() - begin_read_micros_ <= 10) return;
    if (TSI0_GENCS & TSI_GENCS_SCNIP) return;
    int32_t ch = pin2tsi[pin_];
    delayMicroseconds(1);
#if defined(KINETISK)
    Update(*((volatile uint16_t *)(&TSI0_CNTR1) + ch));
#elif defined(KINETISL)
    Update(TSI0_DATA & 0xFFFF);
#endif
    BeginRead();
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      clicked_ = true;
      return true;
    }
    return false;
  }

  bool print_next_ = false;
  int begin_read_micros_ = 0;
  uint8_t pin_;
  const char* name_;
  int threshold_;
  bool pushed_;
  bool clicked_;
  int push_millis_;
};

// Menu system

// STATE_OFF:
// Power click  -> STATE_ON
// Power double click
// Power long click
// AUX click  -> STATE_ON
// AUX double click -> MUSIC_ON
// AUX long click -> SILENT_ON

// STATE_RUN:
// 

// The Saber class implements the basic states and actions
// for the saber.
class Saber : CommandParser, Looper {
public:
  Saber() : CommandParser(),
            power_(powerButtonPin, 1300, "pow"),
            aux_(auxPin, "aux"),
            aux2_(aux2Pin, "aux2") {}

  void On() {
    if (on_) return;
    Serial.println("Ignition.");
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    delay(10);             // allow time to wake up

    on_ = true;
    SaberBase::DoOn();
#if 0
    // FIXME: Let the audio implementation turn on the hum
    uint32_t delay = 0;
#ifdef ENABLE_AUDIO
    if (playFlashRaw1.isPlaying()) {
      delay = playFlashRaw1.lengthMillis() - 10;
    }
#endif
    scheduler.SetNextAction(Scheduler::TURN_ON, delay);
#endif
  }

  void Off() {
#if 0
    // TODO: FADE OUT!
#ifdef ENABLE_AUDIO
    saber_synth.on_ = false;
#endif
    on_ = false;
    if (scheduler.next_action_ == Scheduler::TURN_ON) {
      scheduler.SetNextAction(Scheduler::NO_ACTION, 0);
    }
#endif
    SaberBase::DoOff();
  }

  unsigned long last_clash = 0;
  void Clash() {
    // TODO: Pick clash randomly and/or based on strength of clash.
    if (!on_) return;
    unsigned long t = millis();
    if (t - last_clash < 100) return;
    last_clash = t;
    SaberBase::DoClash();
  }

  bool chdir(const char* dir) {
    monophonic_font.Deactivate();
    polyphonic_font.Deactivate();

    strcpy(current_directory, dir);
    if (current_directory[strlen(current_directory)-1] != '/') {
      strcat(current_directory, "/");
    }
    
#ifdef ENABLE_AUDIO
    Effect::ScanDirectory(current_directory);
#endif
    if (clash.files_found()) {
      monophonic_font.Activate();
      return true;
    }
    if (clsh.files_found()) {
      polyphonic_font.Activate();
      return true;
    }
    return false;
  }

  float id() {
    int blade_id = analogRead(bladeIdentifyPin);
    float volts = blade_id * 3.3 / 1024.0;  // Volts at bladeIdentifyPin
    float amps = (3.3 - volts) / 33000;     // Pull-up is 33k
    float resistor = volts / amps;
    Serial.print("ID: ");
    Serial.print(blade_id);
    Serial.print(" volts ");
    Serial.print(volts);
    Serial.print(" resistance= ");
    Serial.println(resistor);
    return resistor;
  }

  void next_directory(int sign = 1) {
    int tries = 0;
    int dirs = 0;
    do {
      dirs = 0;
      File dir = SD.open("/");
      File best, first;
      while (File f = dir.openNextFile()) {
	if (!f.isDirectory()) continue;
	dirs++;
	if (!first) {
	  first = f;
	} else {
	  if (strcmp(f.name(), first.name())*sign < 0) continue;
	}
	if (strcmp(f.name(), current_directory)*sign < 0) continue;
	if (best && strcmp(f.name(), best.name())*sign > 0) continue;
	best = f;
      }
      if (best) {
	if (chdir(best.name()))
	  return;
      } else if (first) {
	if (chdir(first.name()))
	  return;
      }
    } while (++tries <= dirs);
  }

protected:
  void Loop() override {
    if (power_.clicked()) {
      if (on_) {
        Off();
      } else {
        On();
      }
    }
  }
  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "id")) {
      id();
      return true;
    }
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
    if (!strcmp(cmd, "cd")) {
      chdir(arg);
      return true;
    }
    if (!strcmp(cmd, "pwd")) {
      Serial.println(current_directory);
      return true;
    }
    if (!strcmp(cmd, "next") && arg && !strcmp(arg, "font")) {
      next_directory(1);
      return true;
    }
    if (!strcmp(cmd, "prev") && arg && !strcmp(arg, "font")) {
      next_directory(-1);
      return true;
    }
    return false;
  }
private:
  bool on_;
  TouchButton power_;
  Button aux_;
  Button aux2_;
};

Saber saber;

class BatteryMonitor : Looper, CommandParser {
protected:
  void Setup() override {
    pinMode(batteryLevelPin, INPUT_PULLDOWN);
  }
  float battery() {
    float volts = 3.3 * analogRead(batteryLevelPin) / 1024.0;
    float pulldown = 33000;  // Internal pulldown is 33kOhm
    float external = 23000;  // External pullup
    float battery_volts = volts * (1.0 + external / pulldown);
    return battery_volts;
  }
  void Loop() override {
    float battery_volts = battery();
    if (monitor.ShouldPrint(Monitoring::MonitorBattery) ||
	millis() - last_print_millis_ > 20000) {
      Serial.print("Battery voltage: ");
      Serial.println(battery_volts);
      last_print_millis_ = millis();
    }
    if (battery_volts < 3.0) {
      // Battery voltage is low. Turn stuff off, beep, etc.
    }
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, "batt") || !strcmp(cmd, "battery")) {
      Serial.print("Battery voltage: ");
      Serial.println(battery());
      return true;
    }
    return false;
  }
private:
  uint32_t last_print_millis_;
};

BatteryMonitor battery_monitor;

// Command-line parser. Easiest way to use it is to start the arduino
// serial monitor.
class Parser : Looper, StateMachine {
public:
  enum Mode {
    ModeCommand,
    ModeDecode
  };
  Parser() : Looper(), len_(0), mode_(ModeCommand) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!Serial) YIELD();
      Serial.println("Welcome to TeensySaber, type 'help' for more info.");
      
      while (Serial) {
        while (!Serial.available()) YIELD();
        int c = Serial.read();
        if (c < 0) { len_ = 0; break; }
        if (c == '\n') { Parse(); len_ = 0; continue; }
        cmd_[len_] = c;
        cmd_[len_ + 1] = 0;
        if (len_ + 1 < (int)sizeof(cmd_)) len_++;
      }
    }
    STATE_MACHINE_END();
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
      Serial.println("  red, green, blue, yellow, cyan, magenta, white");
      Serial.println("Serial Flash memory management:");
      Serial.println("   ls, rm <file>, format, play <file>, effects");
      Serial.println("To upload files: tar cf - files | uuencode >/dev/ttyACM0");
      Serial.println("Debugging commands:");
      Serial.println("   monitor swings, monitor samples, top, version");
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
    if (!strcmp(cmd, "dir")) {
      File dir = SD.open(e ? e : current_directory);
      while (File f = dir.openNextFile()) {
        Serial.print(f.name());
        Serial.print(" ");
        Serial.println(f.size());
        f.close();
      }
      Serial.println("Done listing files.");
      return;
    }
    if (!strcmp(cmd, "cache")) {
      File f = SD.open(e);
      if (!f) {
	Serial.println("File not found.");
	return;
      }
      int bytes = f.size();
      if (!SerialFlashChip::create(e, bytes)) {
	Serial.println("Not enough space on serial flash chip.");
	return;
      }
      SerialFlashFile o = SerialFlashChip::open(e);
      while (bytes) {
	char tmp[256];
	int b = f.read(tmp, min(bytes, (int)NELEM(tmp)));
	o.write(tmp, b);
	bytes -= b;
      }
      Serial.println("Cached!");
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
#ifdef ENABLE_AUDIO
    if (!strcmp(cmd, "ton")) {
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      dac.SetStream(&saber_synth);
      saber_synth.on_ = true;
      return;
    }
    if (!strcmp(cmd, "tof")) {
      saber_synth.on_ = false;
      return;
    }
    // TODO: Move to DAC
    if (!strcmp(cmd, "dacbuf")) {
      for (size_t i = 0; i < NELEM(DAC::dac_dma_buffer); i++) {
	Serial.print(DAC::dac_dma_buffer[i]);
	if ((i & 0xf) == 0xf)
	  Serial.println("");
	else
	  Serial.print(" ");
      }
      Serial.println("");
      return;
    }
    if (!strcmp(cmd, "dumpwav")) {
      int16_t tmp[32];
      wav_players[0].Stop();
      wav_players[0].read(tmp, NELEM(tmp));
      wav_players[0].Play(e);
      for (int j = 0; j < 32; j++) {
	int k = wav_players[0].read(tmp, NELEM(tmp));
	for (int i = 0; i < k; i++) {
	  Serial.print(tmp[i]);
	  Serial.print(" ");
	}
	Serial.println("");
      }
      wav_players[0].Stop();
      return;
    }
    if (!strcmp(cmd, "play")) {
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      wav_players[0].Play(e);
      dac.SetStream(wav_players);
      return;
    }
#endif
    if (!strcmp(cmd, "format")) {
      Serial.print("Erasing ... ");
      SerialFlashChip::eraseAll();
      while (!SerialFlashChip::ready());
      Serial.println("Done");
      return;
    }
    if (!strcmp(cmd, "top")) {
#ifdef OLD_AUDIO_CODE
//      Serial.print("Audio usage: ");
//      Serial.println(AudioProcessorUsage());
      Serial.print("Audio usage max: ");
      Serial.println(AudioProcessorUsageMax());
      AudioProcessorUsageMaxReset();
#endif
      SaberBase::DoTop();
      return;
    }
    if (!strcmp(cmd, "version")) {
      Serial.println(version);
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
};

Parser parser;

#ifdef ENABLE_MOTION

// Simple 3D vector.
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

// Motion tracking. The NXPmotionsense library can supposedly do
// full absolute motion tracking, but currently we're only using
// the raw values from accelerometers and gyroscopes.
class Orientation : Looper {
  public:
  Orientation() : Looper(), accel_(0.0f, 0.0f, 0.0f) {
  }

  void Setup() override {
    imu.begin();
    filter.begin(100);
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
#ifdef ENABLE_AUDIO
        Serial.print(" VOL: ");
        Serial.print(saber_synth.volume_.value());
        Serial.print(" TARGET: ");
        Serial.println(saber_synth.volume_.target_);
#endif
      }

#ifdef ENABLE_AUDIO
      saber_synth.volume_.set_target(32768 * (0.5 + clamp(speed/200.0, 0.0, 0.5)));

      // TODO: speed delta?
      saber_synth.AdjustDelta(speed);
#endif
      
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
// Turns off amplifier when no audio is played.
// Maybe name this IdleHelper or something instead??
class Amplifier : Looper, StateMachine {
public:
  Amplifier() : Looper() {}
protected:
  void Setup() override {
    // Audio setup
    delay(50);             // time for DAC voltage stable
    pinMode(amplifierPin, OUTPUT);
    delay(10);
  }

  bool Active() {
    if (saber_synth.on_) return true;
    for (size_t i = 0; i < NELEM(wav_players); i++)
      if (wav_players[i].isPlaying())
	return true;
    return false;
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (Active()) YIELD();
      SLEEP(20);
      if (Active()) continue;
      Serial.println("Amplifier off.");
      digitalWrite(amplifierPin, LOW); // turn the amplifier off
      while (!Active()) YIELD();
    }
    STATE_MACHINE_END();
  }
};

Amplifier amplifier;
#endif

void setup() {
  Serial.begin(9600);
  pinMode(bladeIdentifyPin, INPUT_PULLUP);
  delay(40);

  // Time to identify the blade.
  float resistor = saber.id();

  size_t best_config = 0;
  float best_err = 1000000.0;
  for (size_t i = 0; i < sizeof(blades) / sizeof(blades)[0]; i++) {
    float err = fabs(resistor - blades[i].ohms);
    if (err < best_err) {
      best_config = i;
      best_err = err;
    }
  }
  Serial.print(" blade= ");
  Serial.println(best_config);

  // TODO only activate monopodws if we have a WS2811-type blade
  switch (blades[best_config].blade_type) {
    case BladeID::PL9823:
      ws2811_blade.Activate(blades[best_config].num_leds, WS2811_580kHz);
      break;
    case BladeID::WS2811:
      ws2811_blade.Activate(blades[best_config].num_leds,
          WS2811_800kHz | WS2811_GRB);
      break;
    case BladeID::SIMPLE:
      simple_blade.Activate();
  }

  
#ifdef ENABLE_AUDIO
  // AudioMemory(10);
  SerialFlashChip::begin(6);
  if (!SD.begin(sdCardSelectPin)) {
    Serial.println("No sdcard found.");
  } else {
    Serial.println("Sdcard found..");
  }

  // Check sound font type.
#endif
  saber.chdir(blades[best_config].sound_font_directory);

  Serial.println("DoSetup()");
  Looper::DoSetup();

#ifdef OLD_AUDIO_CODE
  Serial.println("Play()");
  boot.Play();
#endif
}

int last_activity = millis();

void loop() {
  Looper::DoLoop();

#ifdef ENABLE_SNOOZE
  bool on = false;
  BladeBase::IsOn(&on);
  if (
#ifdef ENABLE_AUDIO
    !saber_synth.on_ &&
      !playFlashRaw1.isPlaying() &&
      !playSdWav1.isPlaying() &&
      digitalRead(amplifierPin) == LOW &&
#endif
    !Serial && !on) {
    if (millis() - last_activity > 1000) {
      Serial.println("Snoozing...");
      Snooze.sleep(snooze_config);
    }
  } else {
    last_activity = millis();
  }
#endif
}

