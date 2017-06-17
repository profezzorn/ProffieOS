/*
 Teensy Lightsaber Firmware
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

// You can have multiple configuration files, and specify which one
// to use here.

// #define CONFIG_FILE "toy_saber_config.h"
// #define CONFIG_FILE "graflex_v1_config.h"
// #define CONFIG_FILE "owk_v2_config.h"
#define CONFIG_FILE "test_bench_config.h"

// Search for CONFIGURABLE in this file to find all the places which
// might need to be modified for your saber.

#ifdef CONFIG_FILE

#define CONFIG_TOP
#include CONFIG_FILE
#undef CONFIG_TOP

#if VERSION_MAJOR >= 2
#define V2
#endif

#else

// Board version
#define VERSION_MAJOR 2
#define VERSION_MINOR 3

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 1

// If you have two 144 LED/m strips in your blade, connect
// both of them to bladePin and drive them in parallel.
const unsigned int maxLedsPerStrip = 144;

#if VERSION_MAJOR >= 2
#define V2
#else
#define POWER_TOUCHBUTTON
#endif

//
// OVERVIEW
//
// Here explain some general code concepts to make it easier
// to understand the code below.
//
// Most things start with the Saber class. It is responsible
// for the overall state of the saber and handles button clicks.
// Once an event is registered, such as "on" or "clash", the
// event is sent to all registered SaberBase classes.
//
// Generally speaking, thre are usually two registered SaberBase
// classes listening for events. One for sound and one for 
// the blade. Sound and blade effects are generally executed
// separately by separate clases.
//
// Blades are generally handled by one of the child-classes of
// BladeBase. These classes know how many LEDs the current
// blade has, and how to set those LEDs to a given color, but
// they don't actually decide what the blade should look like.
// Instead they just call the current BladeStyle class and
// asks it to set the colors. The BladeStyle classes don't
// need to know what kind of blade is attached, although
// some combinations of BladeBases and BladeStyles just don't
// make any sense.
//
// Sounds are also abstracted. It starts with scanning a directory
// on the SD card for files that match known patterns of file names.
// The Effect class is responsible for keeping track of all numbered
// files that for a particular filename prefix.
//
// Once the directory has been scanned, we'll either initiate
// a MonophonicFont or a PolyphonicFont based on the names of the
// files we find. MonophonicFont and PolyphonicFont inherit from
// SaberBase and listen on on/off/clash/etc. events, just like
// BladeBase classes do.
// 
// MonophonicFont and PolyphonicFont tells the audio subsystem
// to trigger and mix sounds as aproperiate. The sound subsystem
// starts with an DMA channel which feeds data to a digital-to-analog
// converter. Once the data buffer is half-gone, and interrupt is
// triggered in the DAC class, which tries to fill it up by
// reading data from a int16_t AudioStream. Generally, that data
// stream is hooked up to the AudioDynamicMixer class. This
// class is responsible for taking multiple audio inputs,
// summing them up and then adjusting the volume to minimize
// clipping.  Generally, one of the inputs are hooked up to
// the AudioSplicer class, and the others are hooked up to
// BufferedWavPlayers.  The AudioSplicer is able to do
// smooth cutovers between sounds, and it's inputs are also
// BufferedWavPlayers.


// TODO LIST:
//   Drag effect
//   Blast effect
//   stab detect/effect
//   Test MTP with beta teensyduino.
// Make sure that sound is off before doing file command
// make "charging style" prevent you from turning the saber "on"
// 
// Audio work items:
//   Tune swings better
//   select clash from force
//   stab effect
// Blade stuff
//    better clash
//    blast effect
// Implement menues:
//    select sound font
//    select color
//    adjust volume
// Disable motion when off to save power.
// Allow several blades to share power pins.

// If your electonics inverts the bladePin for some reason, define this.
// #define INVERT_WS2811

// Feature defines, these let you turn off large blocks of code
// used for debugging.
#define ENABLE_AUDIO
#define ENABLE_MOTION
// #define ENABLE_SNOOZE
#define ENABLE_WS2811

// FASTLED is experimental and untested right now
#define ENABLE_FASTLED

// #define ENABLE_WATCHDOG
#define ENABLE_SD
#if VERSION_MAJOR == 1
#define ENABLE_SERIALFLASH
#endif

// #define ENABLE_DEBUG

#endif  // CONFIG_FILE

// If defined, DAC vref will be 3 volts, resulting in louder sound.
#define LOUD

// If defined all sound samples will be divided by 8, resulting in
// very quit sound.
// #define QUIET

// This doesn't seem to work.
// #define TAR_UPLOADS_TO_SDCARD

// You can get better SD card performance by
// activating the  USE_TEENSY3_OPTIMIZED_CODE define
// in SD.h in the teensy library, however, my sd card
// did not work with that define.

#include <Arduino.h>
#include <EEPROM.h>
#ifdef ENABLE_SERIALFLASH
#include <SerialFlash.h>
#endif

#include <DMAChannel.h>

#include <SD.h>

#include <SPI.h>
#include <math.h>
#include <usb_dev.h>
#include <i2c_t3.h>
// #include <Wire.h>
#include <kinetis.h>
#include <malloc.h>

#ifdef ENABLE_SNOOZE

#define startup_early_hook DISABLE_startup_early_hook
#include <Snooze.h>
#undef startup_early_hook

SnoozeTimer snooze_timer;
SnoozeDigital snooze_digital;
SnoozeTouch snooze_touch;
SnoozeBlock snooze_config(snooze_touch, snooze_digital, snooze_timer);
#endif


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
// See the teensy 3.2 pinout diagram for more info: https://www.pjrc.com/teensy/pinout.html
enum SaberPins {
  // Bottom edge (in pin-out diagram)
  sdCardSelectPin = 0,            // SD card chip (sd card adapter)
                                  // Set to BUILTIN_SDCARD for Teensy 3.5/3.6
                                  // (See File->Example->SD->ReadWrite for
                                  // other possible values.)
#ifdef V2
  amplifierPin = 1,               // Amplifier enable pin (TeensySaber V2)
  motionSensorInterruptPin = 2,   // motion sensor interrupt (TeensySaber V2)
  bladePowerPin4 = 3,             // Optional power control (TeensySaber V2)
  bladePowerPin5 = 4,             // Optional power control (TeensySaber V2)
  bladePowerPin6 = 5,             // Optional power control (TeensySaber V2)
  freePin6 = 6,
  spiLedSelect = -1,               // APA102/dotstar chip select
  spiLedDataOut = 7,
  spiLedClock = 8,
#else
  freePin1 = 1,                   // FREE
  motionSensorInterruptPin = 2,   // motion sensor interrupt (prop shield)
  freePin3 = 3,                   // FREE
  freePin4 = 4,                   // FREE (make this sdCardSelectPin if you're using a Wiz820+SD shield)
  amplifierPin = 5,               // Amplifier enable pin (prop shield)
  serialFlashSelectPin = 6,       // serial flash chip select (prop shield)
  spiLedSelect = 7,               // APA102/dotstar chip select (prop shield)
  freePin8 = 8,                   // FREE
  spiLedDataOut = 11,
  spiLedClock = 13,
#endif
  freePin9 = 9,                   // FREE
  freePin10 = 10,                 // FREE
  spiDataOut = 11,                // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card

  // Top edge
  spiClock = 13,                  // spi clock, flash, spi led & sd card
  batteryLevelPin = 14,           // battery level input
  auxPin = 15,                    // AUX button
  powerButtonPin = 16,            // power button
  aux2Pin = 17,                   // AUX2 button
  i2cDataPin = 18,                // I2C bus, Used by motion sensors
  i2cClockPin = 19,               // I2C bus, Used by motion sensors
  bladePin = 20,                  // blade control, either WS2811 or PWM
  bladeIdentifyPin = 20,          // blade identify input / FoC
#ifdef V2
  bladePowerPin3 = 21,            // blade power control
  bladePowerPin2 = 22,            // blade power control
  bladePowerPin1 = 23,            // blade power control
#else
  bladePowerPin1 = 21,            // blade power control
  bladePowerPin2 = 22,            // blade power control
  bladePowerPin3 = 23,            // blade power control
#endif
};


const char version[] = "$Id$";

/*
 * State machine code.
 * This code uses the fact that switch can jump into code to create
 * lightweight pseudo-thread. These threads have a lot of limitations,
 * but they can make code much easier to read compared to manually
 * written state machines. Lots of examples below.
 */
// Note, you cannot have two YIELD() on the same line.
#define YIELD() do { state_machine_.next_state_ = __LINE__; return; case __LINE__: break; } while(0)
#define SLEEP(MILLIS) do { state_machine_.sleep_until_ = millis() + (MILLIS); while (millis() < state_machine_.sleep_until_) YIELD(); } while(0)
#define SLEEP_MICROS(MICROS) do { state_machine_.sleep_until_ = micros() + (MICROS); while (micros() < state_machine_.sleep_until_) YIELD(); } while(0)
#define STATE_MACHINE_BEGIN() switch(state_machine_.next_state_) { case -1:
#define STATE_MACHINE_END() state_machine_.next_state_ = -2; case -2: break; } 

#define CALL(SM, FUN) do {                  \
  state_machine_.next_state_ = __LINE__;    \
  case __LINE__:                            \
  FUN();                                    \
  if (SM.next_state == -2) return;          \
  SM.reset_state_machine();                 \
} while(0)

struct StateMachineState {
  int next_state_ = -1;
  uint32_t sleep_until_ = 0;
  void reset_state_machine() {
    next_state_ = -1;
  }
};

class StateMachine {
protected:
  StateMachineState state_machine_;
};

#ifdef ENABLE_DEBUG

// This class is really useful for finding crashes
// basically, the pin you give it will be held high
// while this function is running. After that it will
// be set to low. If a crash occurs in this function
// it will stay high.
class ScopedPinTracer {
public:
  explicit ScopedPinTracer(int pin) : pin_(pin) {
    pinMode(pin_, OUTPUT);
    digitalWriteFast(pin, HIGH);
  }
  ~ScopedPinTracer() {
    digitalWriteFast(pin_, LOW);
  }
private:
  int pin_;
};

class ScopedTracer3 {
public:
  explicit ScopedTracer3(int code) {
    pinMode(bladePowerPin1, OUTPUT);
    pinMode(bladePowerPin2, OUTPUT);
    pinMode(bladePowerPin3, OUTPUT);
    digitalWriteFast(bladePowerPin1, !!(code & 1));
    digitalWriteFast(bladePowerPin2, !!(code & 2));
    digitalWriteFast(bladePowerPin3, !!(code & 4));
  }
  ~ScopedTracer3() {
    digitalWriteFast(bladePowerPin1, LOW);
    digitalWriteFast(bladePowerPin2, LOW);
    digitalWriteFast(bladePowerPin3, LOW);
  }
};

#define CHECK_LL(T, START, NEXT) do {                                   \
  int len = 0;                                                          \
  for (T* i = START; i; i = i->NEXT) {                                  \
    if (abs(((long)i) - (long)&START) > 65536) {                        \
      Serial.print("Linked list " #START " has invalid pointer @ ");    \
      Serial.print(__LINE__);                                           \
      Serial.print(" pointer: ");                                       \
      Serial.println((long)i, 16);                                      \
      START = NULL;                                                     \
      break;                                                            \
    }                                                                   \
    if (++len > 1000) {                                                 \
      Serial.print("Linked list " #START " has become infinite @ ");    \
      Serial.println(__LINE__);                                         \
      i->NEXT = NULL;                                                   \
      break;                                                            \
    }                                                                   \
  }                                                                     \
} while(0)

#else
#define CHECK_LL(T, START, NEXT)

#endif

class ScopedCycleCounter {
public:
  ScopedCycleCounter(uint64_t& dest) :
    dest_(dest) {
    ARM_DEMCR |= ARM_DEMCR_TRCENA;
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
    cycles_ = ARM_DWT_CYCCNT;
  }
  ~ScopedCycleCounter() {
    cycles_ = ARM_DWT_CYCCNT - cycles_;
    dest_ += cycles_;
  }
private:
  uint32_t cycles_;
  uint64_t& dest_;
};

uint64_t audio_dma_interrupt_cycles = 0;
uint64_t wav_interrupt_cycles = 0;
uint64_t loop_cycles = 0;

class LoopCounter {
public:
  void Print() {
    if (millis_sum_)
      Serial.print(updates_ * 1000.0 / millis_sum_);
  }
  void Reset() {
    updates_ = 0;
    millis_sum_ = 0;
    last_millis_ = 0;
  }
  void Update() {
    uint32_t m = millis();
    if (last_millis_) {
      millis_sum_ += m - last_millis_;
      updates_++;
      if (updates_ > 1000) {
         updates_ /= 2;
         millis_sum_ /= 2;
      }
    }
    last_millis_ = m;
  }
private:
  int updates_ = 0;
  int millis_sum_ = 0;
  uint32_t last_millis_ = 0;
};

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))

// Magic type used to prevent linked-list types from automatically linking.
enum NoLink { NOLINK = 17 };

// Helper class for classses that needs to be called back from the Loop()
// function. Also provides a Setup() function.
class Looper;
Looper* loopers = NULL;
LoopCounter global_loop_counter;
class Looper {
public:
  void Link() {
    CHECK_LL(Looper, loopers, next_looper_);
    next_looper_ = loopers;
    loopers = this;
    CHECK_LL(Looper, loopers, next_looper_);
  }
  void Unlink() {
    CHECK_LL(Looper, loopers, next_looper_);
    for (Looper** i = &loopers; *i; i = &(*i)->next_looper_) {
      if (*i == this) {
        *i = next_looper_;
        CHECK_LL(Looper, loopers, next_looper_);
        return;
      }
    }
    CHECK_LL(Looper, loopers, next_looper_);
  }
  Looper() { Link(); }
  explicit Looper(NoLink _) { }
  ~Looper() { Unlink(); }
  static void DoLoop() {
    ScopedCycleCounter cc(loop_cycles);
    CHECK_LL(Looper, loopers, next_looper_);
    for (Looper *l = loopers; l; l = l->next_looper_) {
      l->Loop();
    }
    global_loop_counter.Update();
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
    CHECK_LL(CommandParser, parsers, next_parser_);
    next_parser_ = parsers;
    parsers = this;
    CHECK_LL(CommandParser, parsers, next_parser_);
  }
  void Unlink() {
    CHECK_LL(CommandParser, parsers, next_parser_);
    for (CommandParser** i = &parsers; *i; i = &(*i)->next_parser_) {
      if (*i == this) {
        *i = next_parser_;
    CHECK_LL(CommandParser, parsers, next_parser_);
        return;
      }
    }
    CHECK_LL(CommandParser, parsers, next_parser_);
  }

  CommandParser() { Link(); }
  explicit CommandParser(NoLink _) {}
  ~CommandParser() { Unlink(); }
  static bool DoParse(const char* cmd, const char* arg) {
    CHECK_LL(CommandParser, parsers, next_parser_);
    for (CommandParser *p = parsers; p; p = p->next_parser_) {
      if (p->Parse(cmd, arg))
        return true;
    }
    return false;
  }
  static void DoHelp() {
    CHECK_LL(CommandParser, parsers, next_parser_);
    for (CommandParser *p = parsers; p; p = p->next_parser_) {
      p->Help();
    }
  }
protected:
  virtual bool Parse(const char* cmd, const char* arg) = 0;
  virtual void Help() = 0;
private:
  CommandParser* next_parser_;
};

// Simple 3D vector.
class Vec3 {
public:
  Vec3(){}
  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
  static Vec3 MSB(const unsigned char* msb_data, float mul) {
    Vec3 ret;
    ret.x = mul * (int16_t)((msb_data[0] << 8) | (msb_data[1]));
    ret.y = mul * (int16_t)((msb_data[2] << 8) | (msb_data[3]));
    ret.z = mul * (int16_t)((msb_data[4] << 8) | (msb_data[5]));
    return ret;
  }
  static Vec3 LSB(const unsigned char* lsb_data, float mul) {
    Vec3 ret;
    ret.x = mul * (int16_t)((lsb_data[1] << 8) | (lsb_data[0]));
    ret.y = mul * (int16_t)((lsb_data[3] << 8) | (lsb_data[2]));
    ret.z = mul * (int16_t)((lsb_data[5] << 8) | (lsb_data[4]));
    return ret;
  }
  Vec3 operator-(const Vec3& o) const {
    return Vec3(x - o.x, y - o.y, z - o.z);
  }
  Vec3 operator+(const Vec3& o) const {
    return Vec3(x + o.x, y + o.y, z + o.z);
  }
  void operator+=(const Vec3& o)  {
    x += o.x;
    y += o.y;
    z += o.z;
  }
  Vec3 operator*(float f) const {
    return Vec3(x * f, y * f, z * f);
  }
  Vec3 dot(const Vec3& o) const {
    return Vec3(x * o.x, y * o.y, z * o.z);
  }
  float len2() const { return x*x + y*y + z*z; }
  float x, y, z;
};


// SaberBase is our main class for distributing saber-related events, such
// as on/off/clash/etc. to where they need to go. Each SABERFUN below
// has a corresponding SaberBase::Do* function which invokes that function
// on all active SaberBases.
class SaberBase;
SaberBase* saberbases = NULL;

class SaberBase {
protected:
  void Link(SaberBase* x) {
    CHECK_LL(SaberBase, saberbases, next_saber_);
    noInterrupts();
    x->next_saber_ = saberbases;
    saberbases = x;
    interrupts();
    CHECK_LL(SaberBase, saberbases, next_saber_);
  }
  void Unlink(const SaberBase* x) {
    CHECK_LL(SaberBase, saberbases, next_saber_);
    for (SaberBase** i = &saberbases; *i; i = &(*i)->next_saber_) {
      if (*i == x) {
        *i = x->next_saber_;
        CHECK_LL(SaberBase, saberbases, next_saber_);
        return;
      }
    }
    CHECK_LL(SaberBase, saberbases, next_saber_);
  }

  SaberBase() { Link(this); }
  explicit SaberBase(NoLink _) {}
  ~SaberBase() { Unlink(this); }

public:
  static bool Lockup() { return lockup_; }
  static void SetLockup(bool lockup) { lockup_ = lockup; }
  
  // 1.0 = kDefaultVolume
  virtual void SetHumVolume(float volume) {}

#define SABERFUN(NAME, TYPED_ARGS, ARGS)                        \
public:                                                         \
  static void Do##NAME TYPED_ARGS {                             \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {    \
      p->SB_##NAME ARGS;                                        \
    }                                                           \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
  }                                                             \
                                                                \
  virtual void SB_##NAME TYPED_ARGS {}

#define SABERBASEFUNCTIONS()                    \
  SABERFUN(Clash, (), ());                      \
  SABERFUN(Stab, (), ());                       \
  SABERFUN(On, (), ());                         \
  SABERFUN(Off, (), ());                        \
  SABERFUN(Force, (), ());                      \
  SABERFUN(Blast, (), ());                      \
  SABERFUN(Boot, (), ());                       \
  SABERFUN(NewFont, (), ());                    \
  SABERFUN(BeginLockup, (), ());                \
  SABERFUN(EndLockup, (), ());                  \
                                                \
  /* Swing rotation speed degrees per second */ \
  SABERFUN(Motion, (const Vec3& gyro), (gyro)); \
  /* Accelertation in g */                      \
  SABERFUN(Accel, (const Vec3& accel), (accel));\
                                                \
  SABERFUN(Top, (), ());                        \
  SABERFUN(IsOn, (bool* on), (on));

  SABERBASEFUNCTIONS();
#undef SABERFUN

private:
  static bool lockup_;
  SaberBase* next_saber_;
};

bool SaberBase::lockup_ = false;

class SaberBasePassThrough : public SaberBase {
public:
  SaberBasePassThrough() : SaberBase(NOLINK) {}
protected:
  void SetDelegate(SaberBase* delegate) {
    Unlink(this);
    if (delegate_) {
      SaberBase::Link(delegate_);
    }
    delegate_ = delegate;
    if (delegate_) {
      SaberBase::Unlink(delegate_);
      SaberBase::Link(this);
    }
  }
#define SABERFUN(NAME, TYPED_ARGS, ARGS)        \
  void SB_##NAME TYPED_ARGS override {          \
    delegate_->SB_##NAME ARGS;                  \
  }

  SABERBASEFUNCTIONS();
#undef SABERFUN

  SaberBase* delegate_ = NULL;
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
    MonitorPWM = 16,
    MonitorClash = 32,
    MonitorTemp = 64,
    MonitorGyro = 128,
    MonitorStrokes = 256,
  };

  bool ShouldPrint(MonitorBit bit) {
    if (bit & monitor_soon_) {
      monitor_soon_ &= ~bit;
      return true;
    }
    return false;
  }

  bool IsMonitoring(MonitorBit bit) const {
    return (active_monitors_ & bit) != 0;
  }
  
protected:
  void Loop() override {
    if (millis() - last_monitor_loop_ > monitor_frequency_ms_) {
      monitor_soon_ = active_monitors_;
      last_monitor_loop_ = millis();
    }
  }
  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "monitor") || !strcmp(cmd, "mon")) {
      if (!strcmp(arg, "swings")) {
        active_monitors_ ^= MonitorSwings;
        return true;
      }
      if (!strcmp(arg, "gyro")) {
        active_monitors_ ^= MonitorGyro;
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
      if (!strcmp(arg, "pwm")) {
        active_monitors_ ^= MonitorPWM;
        return true;
      }
      if (!strcmp(arg, "clash")) {
        active_monitors_ ^= MonitorClash;
        return true;
      }
      if (!strcmp(arg, "temp")) {
        active_monitors_ ^= MonitorTemp;
        return true;
      }
      if (!strcmp(arg, "strokes")) {
        active_monitors_ ^= MonitorStrokes;
        return true;
      }
    }
    return false;
  }
  void Help() {
    Serial.println(" mon[itor] swings/samples/touch/battery/pwm/clash/temp - toggle monitoring");
  }
private:
  uint32_t monitor_frequency_ms_ = 500;
  int last_monitor_loop_ = 0;
  uint32_t monitor_soon_ = 0;
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
int16_t clamptoi16(int32_t x) {
  return clampi32(x, -32768, 32767);
}

const int16_t sin_table[1024] = {
  0,100,201,301,402,502,603,703,803,904,1004,1104,1205,1305,1405,
  1505,1605,1705,1805,1905,2005,2105,2204,2304,2403,2503,2602,2701,
  2800,2899,2998,3097,3196,3294,3393,3491,3589,3687,3785,3883,3980,
  4078,4175,4272,4369,4466,4562,4659,4755,4851,4947,5043,5139,5234,
  5329,5424,5519,5613,5708,5802,5896,5989,6083,6176,6269,6362,6454,
  6547,6639,6730,6822,6913,7004,7095,7185,7276,7365,7455,7545,7634,
  7722,7811,7899,7987,8075,8162,8249,8336,8422,8508,8594,8679,8764,
  8849,8934,9018,9101,9185,9268,9351,9433,9515,9597,9678,9759,9839,
  9920,9999,10079,10158,10237,10315,10393,10470,10547,10624,10700,
  10776,10852,10927,11002,11076,11150,11223,11296,11369,11441,11513,
  11584,11655,11725,11795,11865,11934,12003,12071,12139,12206,12273,
  12339,12405,12470,12535,12600,12664,12727,12790,12853,12915,12977,
  13038,13098,13158,13218,13277,13336,13394,13452,13509,13565,13621,
  13677,13732,13787,13841,13894,13947,14000,14052,14103,14154,14204,
  14254,14303,14352,14400,14448,14495,14542,14588,14633,14678,14722,
  14766,14810,14852,14894,14936,14977,15017,15057,15097,15135,15174,
  15211,15248,15285,15321,15356,15391,15425,15458,15491,15524,15556,
  15587,15618,15648,15677,15706,15734,15762,15789,15816,15841,15867,
  15892,15916,15939,15962,15984,16006,16027,16048,16068,16087,16106,
  16124,16141,16158,16174,16190,16205,16220,16233,16247,16259,16271,
  16283,16293,16304,16313,16322,16330,16338,16345,16352,16358,16363,
  16367,16371,16375,16378,16380,16381,16382,16383,16382,16381,16380,
  16378,16375,16371,16367,16363,16358,16352,16345,16338,16330,16322,
  16313,16304,16293,16283,16271,16259,16247,16233,16220,16205,16190,
  16174,16158,16141,16124,16106,16087,16068,16048,16027,16006,15984,
  15962,15939,15916,15892,15867,15841,15816,15789,15762,15734,15706,
  15677,15648,15618,15587,15556,15524,15491,15458,15425,15391,15356,
  15321,15285,15248,15211,15174,15135,15097,15057,15017,14977,14936,
  14894,14852,14810,14766,14722,14678,14633,14588,14542,14495,14448,
  14400,14352,14303,14254,14204,14154,14103,14052,14000,13947,13894,
  13841,13787,13732,13677,13621,13565,13509,13452,13394,13336,13277,
  13218,13158,13098,13038,12977,12915,12853,12790,12727,12664,12600,
  12535,12470,12405,12339,12273,12206,12139,12071,12003,11934,11865,
  11795,11725,11655,11584,11513,11441,11369,11296,11223,11150,11076,
  11002,10927,10852,10776,10700,10624,10547,10470,10393,10315,10237,
  10158,10079,9999,9920,9839,9759,9678,9597,9515,9433,9351,9268,9185,
  9101,9018,8934,8849,8764,8679,8594,8508,8422,8336,8249,8162,8075,
  7987,7899,7811,7722,7634,7545,7455,7365,7276,7185,7095,7004,6913,
  6822,6730,6639,6547,6454,6362,6269,6176,6083,5989,5896,5802,5708,
  5613,5519,5424,5329,5234,5139,5043,4947,4851,4755,4659,4562,4466,
  4369,4272,4175,4078,3980,3883,3785,3687,3589,3491,3393,3294,3196,
  3097,2998,2899,2800,2701,2602,2503,2403,2304,2204,2105,2005,1905,
  1805,1705,1605,1505,1405,1305,1205,1104,1004,904,803,703,603,502,
  402,301,201,100,0,-100,-201,-301,-402,-502,-603,-703,-803,-904,
  -1004,-1104,-1205,-1305,-1405,-1505,-1605,-1705,-1805,-1905,-2005,
  -2105,-2204,-2304,-2403,-2503,-2602,-2701,-2800,-2899,-2998,-3097,
  -3196,-3294,-3393,-3491,-3589,-3687,-3785,-3883,-3980,-4078,-4175,
  -4272,-4369,-4466,-4562,-4659,-4755,-4851,-4947,-5043,-5139,-5234,
  -5329,-5424,-5519,-5613,-5708,-5802,-5896,-5989,-6083,-6176,-6269,
  -6362,-6454,-6547,-6639,-6730,-6822,-6913,-7004,-7095,-7185,-7276,
  -7365,-7455,-7545,-7634,-7722,-7811,-7899,-7987,-8075,-8162,-8249,
  -8336,-8422,-8508,-8594,-8679,-8764,-8849,-8934,-9018,-9101,-9185,
  -9268,-9351,-9433,-9515,-9597,-9678,-9759,-9839,-9920,-9999,-10079,
  -10158,-10237,-10315,-10393,-10470,-10547,-10624,-10700,-10776,-10852,
  -10927,-11002,-11076,-11150,-11223,-11296,-11369,-11441,-11513,-11584,
  -11655,-11725,-11795,-11865,-11934,-12003,-12071,-12139,-12206,-12273,
  -12339,-12405,-12470,-12535,-12600,-12664,-12727,-12790,-12853,-12915,
  -12977,-13038,-13098,-13158,-13218,-13277,-13336,-13394,-13452,-13509,
  -13565,-13621,-13677,-13732,-13787,-13841,-13894,-13947,-14000,-14052,
  -14103,-14154,-14204,-14254,-14303,-14352,-14400,-14448,-14495,-14542,
  -14588,-14633,-14678,-14722,-14766,-14810,-14852,-14894,-14936,-14977,
  -15017,-15057,-15097,-15135,-15174,-15211,-15248,-15285,-15321,-15356,
  -15391,-15425,-15458,-15491,-15524,-15556,-15587,-15618,-15648,-15677,
  -15706,-15734,-15762,-15789,-15816,-15841,-15867,-15892,-15916,-15939,
  -15962,-15984,-16006,-16027,-16048,-16068,-16087,-16106,-16124,-16141,
  -16158,-16174,-16190,-16205,-16220,-16233,-16247,-16259,-16271,-16283,
  -16293,-16304,-16313,-16322,-16330,-16338,-16345,-16352,-16358,-16363,
  -16367,-16371,-16375,-16378,-16380,-16381,-16382,-16383,-16382,-16381,
  -16380,-16378,-16375,-16371,-16367,-16363,-16358,-16352,-16345,-16338,
  -16330,-16322,-16313,-16304,-16293,-16283,-16271,-16259,-16247,-16233,
  -16220,-16205,-16190,-16174,-16158,-16141,-16124,-16106,-16087,-16068,
  -16048,-16027,-16006,-15984,-15962,-15939,-15916,-15892,-15867,-15841,
  -15816,-15789,-15762,-15734,-15706,-15677,-15648,-15618,-15587,-15556,
  -15524,-15491,-15458,-15425,-15391,-15356,-15321,-15285,-15248,-15211,
  -15174,-15135,-15097,-15057,-15017,-14977,-14936,-14894,-14852,-14810,
  -14766,-14722,-14678,-14633,-14588,-14542,-14495,-14448,-14400,-14352,
  -14303,-14254,-14204,-14154,-14103,-14052,-14000,-13947,-13894,-13841,
  -13787,-13732,-13677,-13621,-13565,-13509,-13452,-13394,-13336,-13277,
  -13218,-13158,-13098,-13038,-12977,-12915,-12853,-12790,-12727,-12664,
  -12600,-12535,-12470,-12405,-12339,-12273,-12206,-12139,-12071,-12003,
  -11934,-11865,-11795,-11725,-11655,-11584,-11513,-11441,-11369,-11296,
  -11223,-11150,-11076,-11002,-10927,-10852,-10776,-10700,-10624,-10547,
  -10470,-10393,-10315,-10237,-10158,-10079,-9999,-9920,-9839,-9759,-9678,
  -9597,-9515,-9433,-9351,-9268,-9185,-9101,-9018,-8934,-8849,-8764,-8679,
  -8594,-8508,-8422,-8336,-8249,-8162,-8075,-7987,-7899,-7811,-7722,-7634,
  -7545,-7455,-7365,-7276,-7185,-7095,-7004,-6913,-6822,-6730,-6639,-6547,
  -6454,-6362,-6269,-6176,-6083,-5989,-5896,-5802,-5708,-5613,-5519,-5424,
  -5329,-5234,-5139,-5043,-4947,-4851,-4755,-4659,-4562,-4466,-4369,-4272,
  -4175,-4078,-3980,-3883,-3785,-3687,-3589,-3491,-3393,-3294,-3196,-3097,
  -2998,-2899,-2800,-2701,-2602,-2503,-2403,-2304,-2204,-2105,-2005,-1905,
  -1805,-1705,-1605,-1505,-1405,-1305,-1205,-1104,-1004,-904,-803,-703,
  -603,-502,-402,-301,-201,-100
};

#ifdef ENABLE_AUDIO

// This class is used to control volumes without causing
// clicking noises. Everytime you call advance(), the value
// goes closer towards the target at a preset speed.
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
  bool isConstant() const {
    return current_ == target_;
  }

  uint32_t speed_;
  uint32_t current_;
  uint32_t target_;
};

struct WaveForm {
  int16_t table_[1024];
};

struct WaveFormSampler {
  WaveFormSampler(const WaveForm& waveform) : waveform_(waveform.table_), pos_(0), delta_(0) {}
  WaveFormSampler(const int16_t* waveform) : waveform_(waveform), pos_(0), delta_(0) {}
  const int16_t *waveform_;
  int pos_;
  volatile int delta_;
  int16_t next() {
    pos_ += delta_;
    if (pos_ > 1024 * 65536) pos_ -= 1024 * 65536;
    // Bilinear lookup here?
    return waveform_[pos_ >> 16];
  }
};

class AudioStream {
public:
  virtual int read(int16_t* data, int elements) = 0;
  // There is no need to call eof() unless read() returns zero elements.
  virtual bool eof() const { return false; }
  // Stop
  virtual void Stop() {}
};


// DMA-driven audio output.
// Based on the Teensy Audio library code.
#define AUDIO_BUFFER_SIZE 88
#define AUDIO_RATE 44100

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_CONT | PDB_SC_PDBIE | PDB_SC_DMAEN)

class DAC : CommandParser {
public:
  DAC() {
    dma.begin(true); // Allocate the DMA channel first
    SIM_SCGC2 |= SIM_SCGC2_DAC0;
    DAC0_C0 = DAC_C0_DACEN;
#ifdef LOUD
    DAC0_C0 |= DAC_C0_DACRFS;  // 3.3V, much louder
#endif
    // This would cause a click, but the amp is not on yet...
    *(int16_t *)&(DAC0_DAT0L) = 2048;

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

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, "dacbuf")) {
      for (size_t i = 0; i < NELEM(dac_dma_buffer); i++) {
        Serial.print(dac_dma_buffer[i] - 2048);
        if ((i & 0xf) == 0xf)
          Serial.println("");
        else
          Serial.print(" ");
      }
      Serial.println("");
      return true;
    }
    return false;
  }

  bool isSilent() {
     for (size_t i = 0; i < NELEM(dac_dma_buffer); i++)
       if (dac_dma_buffer[i] != dac_dma_buffer[0])
         return false;
     return true;
  }

  void Help() override {
    Serial.println(" dacbuf - print the current contents of the dac buffer");
  }

  void SetStream(AudioStream* stream) {
    stream_ = stream;
  }

private:
  // Interrupt handler.
  // Fills the dma buffer with new sample data.
  static void isr(void) {
    ScopedCycleCounter cc(audio_dma_interrupt_cycles);
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
    AudioStream *stream = stream_;
    if (stream) {
      int n = stream->read(dest, end-dest);
      while (n--) {
        *dest = ((*(uint16_t*)dest) + 32768) >> 4;
        dest++;
      }
    }
    while (dest < end) { *dest++ = 2048; }
  }

  DMAMEM static uint16_t dac_dma_buffer[AUDIO_BUFFER_SIZE*2];
  static AudioStream * volatile stream_;
  static DMAChannel dma;
};

DMAChannel DAC::dma(false);
AudioStream * volatile DAC::stream_ = nullptr;
DMAMEM uint16_t DAC::dac_dma_buffer[AUDIO_BUFFER_SIZE*2];

DAC dac;

// Audio compressor, takes N input channels, sums them and divides the
// result by the square root of the average volume.
template<int N> class AudioDynamicMixer : public AudioStream, Looper {
public:
  AudioDynamicMixer() {
    for (int i = 0; i < N; i++) {
      streams_[i] = nullptr;
    }
  }

  // Calculate square root of |x|, using the previous square
  // root as a guess.
  int my_sqrt(int x) {
    if (x <= 0) return 0;
    int over, under, step = 1;
    if (last_square_ * last_square_ > x) {
      over = last_square_;
      under = over - 1;
      while (under * under > x) {
        over = under;
        under -= step;
        step += step;
        if (under <= 0) { under = 0; break; }
      }
    } else {
      under = last_square_;
      over = under + 1;
      while (over * over <= x) {
        under = over;
        over += step;
        step += step;
        if (over < 0) { over = x; break; }
      }
    }
    while (under + 1 < over) {
      int mid = (over + under) >> 1;
      if (mid * mid > x) {
        over = mid;
      } else {
        under = mid;
      }
    }
    return last_square_ = under;
  }
  int last_square_ = 0;

  int read(int16_t* data, int elements) override {
    int32_t sum[AUDIO_BUFFER_SIZE / 2];
    int ret = elements;
    int v = 0, v2 = 0;
    num_samples_ += elements;
    while (elements) {
      int to_do = min(elements, (int)NELEM(sum));
      for (int i = 0; i < to_do; i++) sum[i] = 0;
      for (int i = 0; i < N; i++) {
        int e = streams_[i] ? streams_[i]->read(data, to_do) : 0;
        for (int j = 0; j < e; j++) {
          sum[j] += data[j];
        }
      }

      for (int i = 0; i < to_do; i++) {
        v = sum[i];
        vol_ = ((vol_ + abs(v)) * 255) >> 8;
#if 1
        v2 = (v << 11) / (my_sqrt(vol_) + 100);
#else
        v2 = v;
#endif
#ifdef QUIET
        v2 >>= 3;
#endif
        data[i] = clamptoi16(v2);
        peak_sum_ = max(abs(v), peak_sum_);
        peak_ = max(abs(v2), peak_);
      }
      data += to_do;
      elements -= to_do;
    }
    last_sample_ = v2;
    last_sum_ = v;
    
//    Serial.println(vol_);
    return ret;
  }

  void Loop() override {
    if (monitor.ShouldPrint(Monitoring::MonitorSamples)) {
      Serial.print("Samples: ");
      Serial.print(num_samples_);
      Serial.print(" AVG volume: ");
      Serial.print(vol_);
      Serial.print(" last input sample: ");
      Serial.print(last_sum_);
      Serial.print(" last output sample:");
      Serial.print(last_sample_);
      Serial.print(" peak sum: ");
      Serial.print(peak_sum_);
      Serial.print(" peak: ");
      Serial.println(peak_);
      peak_sum_ = peak_ = 0;
    }
  }

  // TODO: Make levels monitorable

  int32_t last_sample() const {
    return last_sample_;
  }

  int32_t last_sum() const {
    return last_sum_;
  }

  AudioStream* streams_[N];
  int32_t vol_ = 0;
  int32_t last_sample_ = 0;
  int32_t last_sum_ = 0;
  int32_t peak_sum_ = 0;
  int32_t peak_ = 0;
  int32_t num_samples_ = 0;
//  int32_t sum_;
//  ClickAvoiderLin volume_;
};

AudioDynamicMixer<8> dynamic_mixer;

// Beeper class, used for warning beeps and such.
class Beeper : public AudioStream {
public:
  int read(int16_t *data, int elements) override {
    int e = elements;
    while (true) {
      int s = min(elements, samples_);
      s = min(s, x_);
      if (s <= 0) return e - elements;
      if (up_) {
        for (int i = 0; i < s; i++) data[i] = 200;
      } else {
        for (int i = 0; i < s; i++) data[i] = -200;
      }
      data += s;
      elements -= s;
      x_ -= s;
      samples_ -= s;
      if (x_ == 0) {
        x_ = f_;
        up_ = !up_;
      }
    }
  }

  void Beep(float length, float freq) {
    x_ = f_ = AUDIO_RATE / freq / 2.0;
    samples_ = AUDIO_RATE * length;
  }

  bool isPlaying() {
    return samples_ > 0;
  }

private:  
  volatile int samples_ = 0;
  volatile int f_ = 0;
  volatile int x_ = 0;
  volatile bool up_ = false;
};

Beeper beeper;

class LightSaberSynth : public AudioStream, Looper {
public:
//  WaveForm sin_a_;
//  WaveForm sin_b_;
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
    sin_sampler_a_hi_(sin_table),
    sin_sampler_a_lo_(sin_table),
    sin_sampler_b_(sin_table),
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
//      sin_a_.table_[i] = 32768 / (3 + si(f));
//      sin_b_.table_[i] = 32766 / (3 + si(f));
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
      tmp  = humm_sampler_lo_.next() *
        (32768 * 16383 / (3 * 16383 + sin_sampler_a_lo_.next()));
      tmp += humm_sampler_hi_.next() *
        (32768 * 16383 / (3 * 16383 + sin_sampler_a_hi_.next()));
      tmp += buzz_sampler_.next() *
        (32768 * 16383 / (3 * 16383 + sin_sampler_b_.next()));
      tmp >>= 15;
//      tmp = humm_sampler_lo_.next();
      last_prevolume_value = tmp;
      tmp = (tmp * (int32_t)volume_.value()) >> 15;
      volume_.advance();
      tmp = clamptoi16(tmp);
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

// LightSaberSynth saber_synth;

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

// Style #1
//  DAC
//   +-Dynamic Mixer
//   |  +-MonophonicFont (on/hum/off/swing/clash/etc.)
//   |     +-Buffer
//   |     |  +-WavPlayer
//   |     +-Buffer
//   |        +-WavPlayer
//   +-Buffer (for tracks)
//      +-WavPlayer
//
// Style #2 (superset of style #1)
//  DAC
//   +-Dynamic Mixer
//   |  +-MonophonicFont (on/hum/off)
//   |     +-Buffer
//   |     |  +-WavPlayer
//   |     +-Buffer
//   |        +-WavPlayer
//   +-Buffer (for tracks)
//   |   +-WavPlayer
//   +-Buffer (other sound fx)
//   |   +-WavPlayer
//   +-Buffer (other sound fx)
//   |   +-WavPlayer
//   +-Buffer (other sound fx)
//       +-WavPlayer
//

// AudioStreamWork is a linked list of classes that would like to
// do some work in a software-triggered interrupt. This is used to
// let audio processing preempt less important tasks.
#define IRQ_WAV 55

class AudioStreamWork;
AudioStreamWork* data_streams;

class AudioStreamWork {
public:
  AudioStreamWork() {
    next_ = data_streams;
    data_streams = this;
    NVIC_SET_PRIORITY(IRQ_WAV, 240);
    _VectorsRam[IRQ_WAV + 16] = &ProcessAudioStreams;
    NVIC_ENABLE_IRQ(IRQ_WAV);
  }
  ~AudioStreamWork() {
    for (AudioStreamWork** d = &data_streams; *d; d = &(*d)->next_) {
      if (*d == this) {
        *d = next_;
      }
    }
  }

  static void scheduleFillBuffer() {
    if (!NVIC_IS_ACTIVE(IRQ_WAV))
      NVIC_TRIGGER_IRQ(IRQ_WAV);
  }

  static void LockSD(bool locked) {
//    scheduleFillBuffer();
    sd_locked = locked;
  }

protected:
  virtual bool FillBuffer() = 0;
  virtual size_t space_available() const = 0;

private:
  static void ProcessAudioStreams() {
    ScopedCycleCounter cc(wav_interrupt_cycles);
    if (sd_locked) return;
#if 1
    // Yes, it's a selection sort, luckily there's not a lot of
    // AudioStreamWork instances.
    for (int i = 0; i < 50; i++) {
      size_t max_space = 0;
      for (AudioStreamWork *d = data_streams; d; d=d->next_)
        max_space = max(max_space, d->space_available());
      if (max_space == 0) break;
      for (AudioStreamWork *d = data_streams; d; d=d->next_) {
        if (d->space_available() >= max_space)
          d->FillBuffer();
      }
    }
#else
    for (int i = 0; i < 10; i++) {
      for (AudioStreamWork *d = data_streams; d; d=d->next_) {
        d->FillBuffer();
      }
    }
#endif
  }

  static volatile bool sd_locked;

  AudioStreamWork* next_;
};

volatile bool AudioStreamWork::sd_locked = false;
#define LOCK_SD(X) AudioStreamWork::LockSD(X)

// BufferedAudioStream is meant to be read from the main autdio interrupt.
// Every time some space is freed up, Schedulefillbuffer() is called
// to handle filling up the buffer at a lower interrupt level. Since
// filling up the buffer can mean reading from SD, there can potentially
// be more reads from the buffer while we're working on filling it up.
// To make this work, we need make sure that the end pointer for the buffer
// is only modified in the FillBuffer() function and the begin pointer is
// only modified in read();
// N needs to be power of 2
template<int N>
class BufferedAudioStream : public AudioStream, public AudioStreamWork {
public:
  BufferedAudioStream() : AudioStreamWork() {
  }
  int read(int16_t* buf, int bufsize) override {
#if 0
    // Disable buffer
    return stream_ ? stream_->read(buf, bufsize) : 0;
#else
    int copied = 0;
    while (bufsize) {
      int to_copy = buffered();
      if (!to_copy) break;
      to_copy = min(to_copy, bufsize);
      int start_pos = buf_start_ & (N-1);
      to_copy = min(to_copy, N - start_pos);
      memcpy(buf, buffer_ + start_pos, sizeof(buf[0]) * to_copy);
      copied += to_copy;
      buf_start_ += to_copy;
      buf += to_copy;
      bufsize -= to_copy;
    }
    scheduleFillBuffer();
    return copied;
#endif
  }
  bool eof() const override {
    return !buffered() && eof_;
  }
  void Stop() override { if (!stream_) stream_->Stop(); }
  void clear() {
    eof_ = false;
    buf_start_ = buf_end_;
    stream_ = NULL;
  }
  int buffered() const {
    return buf_end_ - buf_start_;
  }
  size_t space_available() const override {
    if (eof_ || !stream_) return 0;
    return N - buffered();
  }
  void SetStream(AudioStream* stream) {
    eof_ = false;
    stream_ = stream;
  }
private:
  bool FillBuffer() override {
    if (stream_) {
      size_t space = space_available();
      if (space) {
        size_t end_pos = buf_end_ & (N-1);
        size_t to_read = min(space, N - end_pos);
        int got = stream_->read(buffer_ + end_pos, to_read);
        if (got) {
          eof_ = false;
        } else {
          eof_ = stream_->eof();
        }
        buf_end_ += got;
      }
    }
    return stream_ && space_available() > 0 && !eof_;
  }
  AudioStream* volatile stream_ = 0;
  // Note, these are assumed to be atomic, 8-bit processors won't work.
  volatile size_t buf_start_ = 0;
  volatile size_t buf_end_ = 0;
  volatile bool eof_ = false;
  int16_t buffer_[N];
};

#endif

#else  // ENABLE_AUDIO

#define LOCK_SD(X) do { } while(0)

#endif  // ENABLE_AUDIO

class Effect;
Effect* all_effects = NULL;

int constexpr toLower(char x) {
  return (x >= 'A' && x <= 'Z') ? x - 'A' + 'a' : x;
}

const char *startswith(const char *prefix, const char* x) {
  while (*prefix) {
    if (toLower(*x) != toLower(*prefix)) return nullptr;
    prefix++;
    x++;
  }
  return x;
}

int cmpdir(const char *a, const char *b) {
  while (toLower(*a) == toLower(*b)) {
    if (!*a) return 0;
    a++;
    b++;
  }
  if (*a == '/' && *b == 0) return 0;
  if (*b == '/' && *a == 0) return 0;
  return toLower(*a) - toLower(*b);
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

// Effect represents a set of sound files.
// We keep track of the minimum number found, the maximum number found, weather
// there is a file with no number, and if there are leading zeroes or not.
// Basically, it means that files can be numbered any which way, as long as it
// is consistent and there are no gaps in the numbering.
//
// Note that *ALL* sounds use this Effect class to keep track of the sounds.
// This means that you can for instance have hum1.wav, hum2.wav, hum3.wav, and
// every time the hum loops, it will randomly pick one of them.
class Effect {
  public:

  enum Extension {
    WAV,
    RAW,
    USL,
    UNKNOWN,
  };

  static Extension IdentifyExtension(const char* filename) {
    if (endswith(".wav", filename)) return WAV;
    if (endswith(".raw", filename)) return RAW;
    if (endswith(".usl", filename)) return USL;
    return UNKNOWN;
  }

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
    ext_ = UNKNOWN;
  }

  void Scan(const char *filename) {
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
        digits_ = end - rest;
      }
    }

    if (ext_ == UNKNOWN)
      ext_ = IdentifyExtension(filename);
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
    int num_files = files_found();
    if (num_files < 1) return false;
    int n = rand() % num_files;
    strcpy(filename, current_directory);
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

    switch (ext_) {
      case WAV: strcat(filename, ".wav"); break;
      case RAW: strcat(filename, ".raw"); break;
      case USL: strcat(filename, ".usl"); break;
      default: break;
    }
    
    Serial.print("Playing ");
    Serial.println(filename);
    return true;
  }

  static void ScanAll(const char* filename) {
    if (Effect::IdentifyExtension(filename) == Effect::UNKNOWN) {
      return;
    }

#if 0
    // TODO: "monitor scan" command?
    Serial.print("SCAN ");
    Serial.println(filename);
#endif
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Scan(filename);
    }
  }

  static void ScanDirectory(const char *directory) {
    Serial.print("Scanning sound font: ");
    Serial.print(directory);
    for (Effect* e = all_effects; e; e = e->next_) {
      e->reset();
    }

#ifdef ENABLE_SERIALFLASH
    // Scan serial flash.
    SerialFlashChip::opendir();
    uint32_t size;
    char filename[128];
    while (SerialFlashChip::readdir(filename, sizeof(filename), size)) {
      const char* f = startswith(directory, filename);
      if (f) ScanAll(f);
    }
#endif

#ifdef ENABLE_SD
    if (SD.exists(directory)) {
      File dir = SD.open(directory);
      if (dir) {
        while (File f = dir.openNextFile()) {
          if (f.isDirectory()) {
            char fname[128];
            strcpy(fname, f.name());
            strcat(fname, "/");
            char* fend = fname + strlen(fname);
            while (File f2 = f.openNextFile()) {
              strcpy(fend, f2.name());
              ScanAll(fname);
              f2.close();
            }
          } else {
            ScanAll(f.name());
          }
          f.close();
        }
	dir.close();
      }
    }
#endif
    Serial.println(" done");
  }

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

  // All files must end with this extension.
  Extension ext_;
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

// Looped swing fonts.
EFFECT(swingl);  // Looped swing, LOW
EFFECT(swingh);  // Looped swing, HIGH

#ifdef ENABLE_AUDIO

// Simple upsampler code, doubles the number of samples with
// 2-lobe lanczos upsampling.
#define C1 24757
#define C2 -8191

#if 1
#define UPSAMPLE_FUNC(NAME, EMIT)                               \
  void NAME(int16_t sample) {                                   \
    upsample_buf_##NAME##_a_ = upsample_buf_##NAME##_b_;        \
    upsample_buf_##NAME##_b_ = upsample_buf_##NAME##_c_;        \
    upsample_buf_##NAME##_c_ = upsample_buf_##NAME##_d_;        \
    upsample_buf_##NAME##_d_ = sample;                          \
    EMIT(clamptoi16((upsample_buf_##NAME##_a_ * C2 +            \
          upsample_buf_##NAME##_b_ * C1 +                       \
          upsample_buf_##NAME##_c_ * C1 +                       \
                   upsample_buf_##NAME##_d_ * C2) >> 15));      \
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
#else
#define UPSAMPLE_FUNC(NAME, EMIT)               \
  void NAME(int16_t sample) {                   \
      EMIT(sample);      EMIT(sample);          \
  }                                             \
  void clear_##NAME() {                         \
  }
#endif

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

// PlayWav reads a file from serialflash or SD and converts
// it into a stream of samples. Note that because it can
// spend some time reading data between samples, the
// reader must have enough buffers to provide smooth playback.
class PlayWav : StateMachine, public AudioStream {
public:
  void Play(const char* filename) {
    if (!*filename) return;
    strcpy(filename_, filename);
    run_ = true;
  }

  void PlayOnce(Effect* effect) {
    if (effect->Play(filename_)) {
      effect_ = nullptr;
      run_ = true;
    }
  }
  void PlayLoop(Effect* effect) {
    effect_ = effect;
  }

  void Stop() override {
    state_machine_.reset_state_machine();
    effect_ = nullptr;
    run_ = false;
    written_ = num_samples_ = 0;
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
  void DecodeBytes4() {
    while (ptr_ < end_ && num_samples_ < (int)NELEM(samples_)) {
      int v = 0;
      if (channels == 1) {
        v = read2<bits>();
      } else {
        v = read2<bits>();
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
  void DecodeBytes3() {
    if (rate_ == 44100)
      DecodeBytes4<bits, channels, 44100>();
    else if (rate_ == 22050)
      DecodeBytes4<bits, channels, 22050>();
    else if (rate_ == 11025)
      DecodeBytes4<bits, channels, 11025>();
  }

  template<int bits>
  void DecodeBytes2() {
    if (channels_ == 1) DecodeBytes3<bits, 1>();
    else DecodeBytes3<bits, 2>();
  }

  void DecodeBytes() {
    if (bits_ == 8) DecodeBytes2<8>();
    else DecodeBytes2<16>();
  }

  int ReadFile(int n) {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) {
      return sf_file_.read(buffer, n);
    }
#endif
#ifdef ENABLE_SD
    return sd_file_.read(buffer, n);
#else
    return 0;
#endif
  }

  void Skip(int n) {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) {
      sf_file_.seek(sf_file_.position() + n);
      return;
    }
#endif
#ifdef ENABLE_SD
    sd_file_.seek(sd_file_.position() + n);
#endif
  }

  uint32_t Tell() {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) return sf_file_.position();
#endif
#ifdef ENABLE_SD
    return sd_file_.position();
#endif
    return 0;
  }

  uint32_t FileSize() {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) return sf_file_.size();
#endif
#ifdef ENABLE_SD
    return sd_file_.size();
#endif
    return 0;
  }

  int AlignRead(int n) {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) return n;
#endif
#ifdef ENABLE_SD
    int next_block = (sd_file_.position() + 512u) & ~511u;
    int bytes_to_end_of_block = next_block - sd_file_.position();
    return min(n, bytes_to_end_of_block);
#else
    return n;
#endif
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
#ifdef ENABLE_SERIALFLASH
      sf_file_ = SerialFlashChip::open(filename_);
      if (!sf_file_)
#endif
      {
#ifdef ENABLE_SD
        sd_file_.close();
        sd_file_ = SD.open(filename_);
        YIELD();
        if (!sd_file_)
#endif
        {
          Serial.print("File ");
          Serial.print(filename_);
          Serial.println(" not found.");
          goto fail;
        }
      }
      wav_ = endswith(".wav", filename_);
      if (wav_) {
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
      } else {
         channels_ = 1;
         rate_ = 44100;
         bits_ = 16;
      }
      Serial.print("channels: ");
      Serial.print(channels_);
      Serial.print(" rate: ");
      Serial.print(rate_);
      Serial.print(" bits: ");
      Serial.println(bits_);

      while (true) {
        if (wav_) {
          if (ReadFile(8) != 8) break;
          len_ = header(1);
          if (header(0) != 0x61746164) {
            Skip(len_);
            continue;
          }
        } else {
          if (Tell() >= FileSize()) break;
          len_ = FileSize() - Tell();
        }
        sample_bytes_ = len_;
        while (len_) {
          bytes_to_decode_ =
            ReadFile(AlignRead(min(len_, sizeof(buffer))));
          len_ -= bytes_to_decode_;
          ptr_ = buffer;
          end_ = buffer + bytes_to_decode_;
          while (ptr_ < end_) {
            DecodeBytes();

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
        YIELD();
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

  bool eof() const override {
    return !run_;
  }

  // Length, seconds.
  float length() const {
    return (float)(sample_bytes_) * 8 / (bits_ * rate_);
  }

private:
  volatile bool run_ = false;
  Effect* volatile effect_ = nullptr;
  char filename_[128];
#ifdef ENABLE_SD
  File sd_file_;
#endif
#ifdef ENABLE_SERIALFLASH
  SerialFlashFile sf_file_;
#endif
  int16_t* dest_ = nullptr;
  int to_read_ = 0;
  int tmp_;

  int rate_;
  uint8_t channels_;
  uint8_t bits_;

  bool wav_;

  int bytes_to_decode_ = 0;
  size_t len_ = 0;
  volatile size_t sample_bytes_ = 0;
  char* ptr_;
  char* end_;
  char buffer[512]  __attribute__((aligned(4)));

  // Number of samples_ in samples that has been
  // sent out already.
  int written_ = 0;
  
  // Number of samples in samples_
  int num_samples_ = 0;
  int16_t samples_[32];
};

const uint32_t kVolumeShift = 14;
const uint32_t kMaxVolume = 1 << kVolumeShift;
const uint32_t kDefaultVolume = kMaxVolume / 2;

template<class T>
class VolumeOverlay : public T {
public:
  
  VolumeOverlay() : volume_(kMaxVolume / 100) {
    volume_.set(kDefaultVolume);
    volume_.set_target(kDefaultVolume);
  }
  int read(int16_t* data, int elements) override {
    elements = T::read(data, elements);
    if (volume_.isConstant()) {
      int32_t mult = volume_.value();
      if (mult == kMaxVolume) {
        // Do nothing
      } else if (mult == 0) {
        if (stop_when_zero_) {
          this->Stop();
          stop_when_zero_ = false;
        }
        for (int i = 0; i < elements; i++) data[i] = 0;
      } else {
        for (int i = 0; i < elements; i++) {
          data[i] = clamptoi16((data[i] * mult) >> kVolumeShift);
        }
      }
    } else {
      for (int i = 0; i < elements; i++) {
        int32_t v = (data[i] * (int32_t)volume_.value()) >> kVolumeShift;
        data[i] = clamptoi16(v);
        volume_.advance();
      }
    }
    return elements;
  }
  float volume() {
    return volume_.value() * (1.0f / (1 << kVolumeShift));
  }
  void set_volume(int vol) {
    volume_.set_target(vol);
  }
  void set_volume_now(int vol) {
    volume_.set(vol);
    volume_.set_target(vol);
  }
  void reset_volume() {
    set_volume_now((int)kDefaultVolume);
  }
  void set_volume(float vol) {
    set_volume((int)(kDefaultVolume * vol));
  }
  void set_volume_now(float vol) {
    set_volume_now((int)(kDefaultVolume * vol));
  }
  void set_speed(int speed) {
    volume_.set_speed(speed);
  }
  void set_fade_time(float t) {
    set_speed(max(1, (int)(kMaxVolume / t / AUDIO_RATE)));
  }
  bool isOff() const {
    return volume_.isConstant() && volume_.value() == 0;
  }
  void FadeAndStop() {
    volume_.set_target(0);
    stop_when_zero_ = true;
  }

private:
  volatile bool stop_when_zero_ = false;
  ClickAvoiderLin volume_;
};

size_t WhatUnit(class BufferedWavPlayer* player);


// Combines a WavPlayer and a BufferedAudioStream into a
// buffered wav player. When we start a new sample, we
// make sure to fill up the buffer before we start playing it.
// This minimizes latency while making sure to avoid any gaps.
class BufferedWavPlayer : public VolumeOverlay<BufferedAudioStream<512> > {
public:
  void Play(const char* filename) {
    pause_ = true;
    clear();
    wav.Play(filename);
    SetStream(&wav);
    scheduleFillBuffer();
    pause_ = false;
  }

  void PlayOnce(Effect* effect) {
    Serial.print("unit = ");
    Serial.print(WhatUnit(this));
    Serial.print(" vol = ");
    Serial.print(volume());
    Serial.print(", ");

    pause_ = true;
    clear();
    wav.PlayOnce(effect);
    SetStream(&wav);
    scheduleFillBuffer();
    pause_ = false;
  }
  void PlayLoop(Effect* effect) { wav.PlayLoop(effect); }

  void Stop() override {
    pause_ = true;
    wav.Stop();
    clear();
  }

  bool isPlaying() const {
    return !pause_ && (wav.isPlaying() || buffered());
  }

  BufferedWavPlayer() {
    SetStream(&wav);
  }

  int read(int16_t* dest, int to_read) override {
    if (pause_) return 0;
    return VolumeOverlay<BufferedAudioStream<512> >::read(dest, to_read);
  }

  float length() const { return wav.length(); }

private:
  PlayWav wav;
  volatile bool pause_;
};

BufferedWavPlayer wav_players[6];
size_t reserved_wav_players = 0;

class BufferedWavPlayer* GetFreeWavPlayer()  {
  // Find a free wave playback unit.
  for (size_t unit = reserved_wav_players;
       unit < NELEM(wav_players); unit++) {
    if (!wav_players[unit].isPlaying()) {
      wav_players[unit].reset_volume();
      return wav_players + unit;
    }
  }
  return NULL;
}

size_t WhatUnit(class BufferedWavPlayer* player) {
  if (!player) return -1;
  return player - wav_players;
}


// This class is used to cut from one sound to another with
// no gap. It does a short (2.5ms) crossfade to accomplish this.
// It's currently hard-coded to use wav_players[0] and wav_playes[1].
class AudioSplicer : public AudioStream, Looper {
public:
  AudioSplicer() : Looper(NOLINK) {}

  int read(int16_t* data, int elements) override {
    int16_t *p = data;
    int to_read = elements;
    if (current_ < 0 && fadeto_ < 0) {
      return 0;
    }
    if (current_ >= 0) {
      int num = wav_players[current_].read(p, to_read);
      to_read -= num;
      p += num;
      if (to_read > 0) {
        // end of file?
        if (wav_players[current_].eof()) {
          // Serial.println("AudioSPlicer::EOF!!");
          current_ = -1;
        }
      }
    }
    while (to_read > 0) {
      *(p++) = 0;
      to_read --;
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
          p[i] = (p[i] * fade_ + tmp[i] * (32768 - fade_)) >> 15;
          if (fade_) {
            fade_ -= fade_speed_;
            if (fade_ < 0) fade_ = 0;
          }
        }
        to_read -= n;
        p += n;
      }
      if (!fade_) {
        if (current_ != -1)
          wav_players[current_].Stop();
        current_ = fadeto_;
        fadeto_ = -1;
      }
    }
    return elements;
  }

  bool eof() const override {
    return current_ == -1 && fadeto_ == -1;
  }

  void set_crossover_time(float t) {
    fade_speed_ = max(1, (int)(32768 / t / AUDIO_RATE));
#if 0
    Serial.print("FADE SPEED: ");
    Serial.println(fade_speed_);
#endif
  }

  Effect* next_effect_ = NULL;
  Effect* next_loop_ = NULL;

  void Loop() override {
    if (fadeto_ == -1) {
      Effect *f = next_effect_;
      Effect *l = next_loop_;
      next_effect_ = next_loop_ = NULL;
      if (f) Play(f, l);
      Looper::Unlink();
    }
  }
          
  bool Play(Effect* f, Effect* loop) {
    if (fadeto_ != -1) {
      if (!next_effect_) Looper::Link();
      next_effect_ = f;
      next_loop_ = loop;
      return false;
    }
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    int unit = current_ == 0 ? 1 : 0;
    wav_players[unit].PlayOnce(f);
    if (loop) {
      wav_players[unit].PlayLoop(loop);
    }
    if (current_ == -1) {
      current_ = unit;
    } else {
      fadeto_ = unit;
      fade_ = 32768;
    }
    return true;
  }

  bool isPlaying() const {
    return current_ != -1 || fadeto_ != -1;
  }

protected:
  volatile int current_= -1;
  volatile int fadeto_ = -1;
  volatile int fade_speed_ = 128;
  volatile int fade_;
};

VolumeOverlay<AudioSplicer> audio_splicer;

void SetupStandardAudioLow() {
    for (size_t i = 0; i < NELEM(wav_players); i++) {
    dynamic_mixer.streams_[i] = wav_players + i;
    wav_players[i].reset_volume();
  }
  reserved_wav_players = 0;
  dynamic_mixer.streams_[NELEM(wav_players)] = &beeper;
}

void SetupStandardAudio() {
  dac.SetStream(NULL);
  SetupStandardAudioLow();
  dac.SetStream(&dynamic_mixer);
}

void ActivateAudioSplicer() {
  dac.SetStream(NULL);
  SetupStandardAudioLow();
  dynamic_mixer.streams_[0] = &audio_splicer;
  dynamic_mixer.streams_[1] = NULL;
  reserved_wav_players = 2;
  audio_splicer.set_crossover_time(0.003);
  dac.SetStream(&dynamic_mixer);
}

// Monophonic sound fonts are the most common.
// These fonts are fairly simple, as generally only one sound is
// played at a time. It starts with the "poweron" sound and when
// that runs out, we gaplessly transition to the "hum" sound.
//
// When an effect happens, like "clash", we do a short cross-fade
// to transition to the new sound, then we play that sound until
// it ends and gaplessly transition back to the hum sound.
class MonophonicFont : public SaberBase {
public:
  MonophonicFont() : SaberBase(NOLINK) { }
  void Activate() {
    Serial.println("Activating monophonic font.");
    ActivateAudioSplicer();
    SaberBase::Link(this);
    SetHumVolume(1.0);
    on_ = false;
  }

  void Deactivate() { SaberBase::Unlink(this); }

  void SB_On() override {
    on_ = true;
    audio_splicer.Play(&poweron, &hum);
  }

  void SB_Off() override {
    on_ = false;
    audio_splicer.Play(&poweroff, NULL);
  }
  void SB_Clash() override { audio_splicer.Play(&clash, &hum); }
  void SB_Stab() override { audio_splicer.Play(&stab, &hum); }
  void SB_Force() override { audio_splicer.Play(&force, &hum); }
  void SB_Blast() override { audio_splicer.Play(&blaster, &hum); }
  void SB_Boot() override { audio_splicer.Play(&boot,  NULL); }
  void SB_NewFont() override { audio_splicer.Play(&font,  NULL); }

  void SB_BeginLockup() override {
    if (lockup.files_found()) {
      audio_splicer.Play(&lockup, &lockup);
    }
  }

  void SB_EndLockup() override {
    if (lockup.files_found()) {
      audio_splicer.Play(&clash, &hum);
    }
  }

  void SetHumVolume(float vol) override {
    audio_splicer.set_volume(vol);
  }

  bool on_ = false;
  bool swinging_ = false;
  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    if (speed > 250.0) {
      if (!swinging_ && on_ && !SaberBase::Lockup()) {
        swinging_ = true;
        audio_splicer.Play(&swing, &hum);
      }
    } else {
      swinging_ = false;
    }
    float vol = 1.0f;
    if (!swinging_) {
      vol = vol * (0.99 + clamp(speed/200.0, 0.0, 2.3));
    }
    SetHumVolume(vol);
  }
};

MonophonicFont monophonic_font;

// Reads an igniter config file, looking for the humstart value.
struct ConfigFile {
#ifdef ENABLE_SD
  void skipwhite(File* f) {
    while (f->peek() == ' ' || f->peek() == '\t')
      f->read();
  }
  void skipline(File* f) {
    while (f->available() && f->read() != '\n');
  }

  int64_t readValue(File* f) {
    int64_t ret = 0;
    int64_t sign = 1;
    if (f->peek() == '-') {
      sign = -1;
      f->read();
    }
    while (f->available()) {
      int c = toLower(f->peek());
      if (c >= '0' && c <= '9') {
        ret = (c - '0') + 10 * ret;
        f->read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  void Read(File* f) {
    for (; f->available(); skipline(f)) {
      char variable[33];
      variable[0] = 0;
      skipwhite(f);
      if (f->peek() == '#') continue;
      for (int i = 0; i < 32; i++) {
        int c = toLower(f->peek());
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
          f->read();
          variable[i] = c;
          variable[i+1] = 0;
        } else {
          break;
        }
      }
      skipwhite(f);
      if (f->peek() != '=') continue;
      f->read();
      skipwhite(f);
      int64_t v = readValue(f);
#if 0
      Serial.print(variable);
      Serial.print(" = ");
      Serial.println((int)v);
#endif
      if (!strcmp(variable, "humstart")) {
        humStart = v;
      } else if (!strcmp(variable, "volhum")) {
        volHum = v;
      } else if (!strcmp(variable, "voleff")) {
        volEff = v;
      }
    }
  }
#endif

  void Read(const char *filename) {
#ifdef ENABLE_SD
    File f = SD.open(filename);
    Read(&f);
    f.close();
#endif
  }

  int humStart = 100;
  int volHum = 15;
  int volEff = 16;
};

// With polyphonic fonts, sounds are played more or less
// independently. Hum is faded in/out by changing the volume
// and all other sound effects are just played in parallel
// when needed.
class PolyphonicFont : public SaberBase {
public:
  PolyphonicFont() : SaberBase(NOLINK) { }
  void Activate() {
    Serial.println("Activating polyphonic font.");
    SetupStandardAudio();
    wav_players[0].set_volume_now(0);
    char config_filename[128];
    strcpy(config_filename, current_directory);
    strcat(config_filename, "config.ini");
    config_.Read(config_filename);
    SaberBase::Link(this);
    state_ = STATE_OFF;
    lock_player_ = NULL;
  }
  enum State {
    STATE_OFF,
    STATE_OUT,
    STATE_HUM_FADE_IN,
    STATE_HUM_ON,
    STATE_HUM_FADE_OUT,
  };
  void Deactivate() { SaberBase::Unlink(this); }

  void SB_On() override {
    state_ = STATE_OUT;
    wav_players[0].PlayOnce(&hum);
    wav_players[0].PlayLoop(&hum);
    hum_start_ = millis();
    if (config_.humStart) {
      BufferedWavPlayer* tmp = Play(&out);
      if (tmp) {
        int delay_ms = 1000 * tmp->length() - config_.humStart;
        hum_start_ += delay_ms;
      }
    }
  }

  void SB_Off() override {
    state_ = STATE_HUM_FADE_OUT;
    Play(&in);
  }

  BufferedWavPlayer* Play(Effect* f)  {
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    BufferedWavPlayer* player = GetFreeWavPlayer();
    if (player) {
      player->set_volume_now(config_.volEff / 16.0);
      player->PlayOnce(f);
    }
    return player;
  }
  void SB_Clash() override { Play(&clsh); }
  void SB_Stab() override { Play(&stab); }
  void SB_Force() override { Play(&force); }
  void SB_Blast() override { Play(&blst); }
  void SB_Boot() override { Play(&boot); }
  void SB_NewFont() override { Play(&font); }

  BufferedWavPlayer* lock_player_ = NULL;
  void SB_BeginLockup() override {
    if (!lock_player_) {
      lock_player_ = Play(&lock);
      if (lock_player_) {
        lock_player_->PlayLoop(&lock);
      }
    }
  }

  void SB_EndLockup() override {
    if (lock_player_) {
      lock_player_->set_fade_time(0.3);
      lock_player_->FadeAndStop();
      lock_player_ = NULL;
    }
  }

  bool swinging_ = false;
  uint32_t hum_start_;
  uint32_t last_micros_;

  void SetHumVolume(float vol) override {
    switch (state_) {
      case STATE_OFF:
        volume_ = 0.0f;
        return;
      case STATE_OUT:
        volume_ = 0.0f;
        if (millis() - hum_start_ < 0x7fffffffUL) {
          state_ = STATE_HUM_FADE_IN;
          last_micros_ = micros();
        }
        break;
      case STATE_HUM_FADE_IN: {
        uint32_t m = micros();
        uint32_t delta = m - last_micros_;
        volume_ += (delta / 1000000.0) * 0.3; // 0.3 seconds
        if (volume_ >= 1.0f) {
          volume_ = 1.0f;
          state_ = STATE_HUM_ON;
        }
        break;
      }
      case STATE_HUM_ON:
        last_micros_ = micros();
        break;
      case STATE_HUM_FADE_OUT: {
        uint32_t m = micros();
        uint32_t delta = m - last_micros_;
        volume_ -= (delta / 1000000.0) * 0.3; // 0.3 seconds
        if (volume_ <= 0.0f) {
          volume_ = 0.0f;
          state_ = STATE_OFF;
        }
        break;
      }
    }
    wav_players[0].set_volume(vol * volume_);
  }
  
  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    if (speed > 250.0) {
      if (!swinging_ && state_ != STATE_OFF) {
        swinging_ = true;
        Play(&swng);
      }
    } else {
      swinging_ = false;
    }
    float vol = 1.0f;
    if (!swinging_) {
      vol = vol * (0.99 + clamp(speed/200.0, 0.0, 2.3));
    }
    SetHumVolume(vol);
  }

  ConfigFile config_;
  State state_;
  float volume_;
};

PolyphonicFont polyphonic_font;

class SyntheticFont : PolyphonicFont {
public:
  void SB_On() override {}
  void SB_Off() override {}

  void SB_Motion(const Vec3& speed) override {
    // Adjust hum volume based on motion speed
  }
};


// Looped swing sounds is a new way to play swing sounds.
// Basically, two swing sounds (swingl and swingh) are always
// playing in the background, but with zero volume. When
// the saber is swung, we fade out the hum and fade in one of
// the swingl/swingh sounds. This type of swing sounds can
// be added to any font by just adding swingl.wav and swingh.wav.
class LoopedSwingWrapper : public SaberBasePassThrough {
public:
  LoopedSwingWrapper() : SaberBasePassThrough() {}

  void Activate(SaberBase* base_font) {
    Serial.println("Activating looped swing sounds");
    SetDelegate(base_font);
  }

  void Deactivate() {
    SetDelegate(NULL);
  }

  BufferedWavPlayer* low_;
  BufferedWavPlayer* high_;
  void SB_On() override {
    // Starts hum, etc.
    delegate_->SB_On();
    low_ = GetFreeWavPlayer();
    if (low_) {
      low_->set_volume_now(0);
      low_->PlayOnce(&swingl);
      low_->PlayLoop(&swingl);
    } else {
      Serial.println("Looped swings cannot allocate wav player.");
    }
    high_ = GetFreeWavPlayer();
    if (high_) {
      high_->set_volume_now(0);
      high_->PlayOnce(&swingh);
      high_->PlayLoop(&swingh);
    } else {
      Serial.println("Looped swings cannot allocate wav player.");
    }
#if 0
    Serial.print("UNITS: ");
    Serial.print(WhatUnit(low_));
    Serial.print(",");
    Serial.println(WhatUnit(high_));
#endif
  }
  void SB_Off() override {
    if (low_) {
      low_->set_fade_time(0.3);
      low_->FadeAndStop();
      low_ = NULL;
    }
    if (high_) {
      high_->set_fade_time(0.3);
      high_->FadeAndStop();
      high_ = NULL;
    }
    delegate_->SB_Off();
  }

  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    uint32_t t = (millis() >> 2);
    float s = sin_table[t & 1023] * (1.0/16383);
    float c = sin_table[(t + 256) & 1023] * (1.0/16383);
    float blend = c * gyro.z + s * gyro.y;
    blend = clamp(blend / 150.0, -1.0, 1.0);
    float vol = 0.99 + clamp(speed/200.0, 0.0, 2.3);
    float low = max(0, blend);
    float high = max(0, -blend);
    float hum = 1.0 - abs(blend);

    delegate_->SetHumVolume(vol * hum);
    if (low_) low_->set_volume(vol * low);
    if (high_) high_->set_volume(vol * high);
    if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
      Serial.print("S:");
      Serial.print(speed);
      Serial.print(" T:");
      Serial.print(t);
      Serial.print(" s:");
      Serial.print(s);
      Serial.print(" c:");
      Serial.print(c);
      Serial.print(" blend:");
      Serial.print(blend);
      Serial.print(" vol:");
      Serial.print(vol);
      Serial.print(" hi:");
      Serial.print(high);
      Serial.print(" lo:");
      Serial.print(low);
      Serial.print(" hum:");
      Serial.println(hum);
    }
  }
};

LoopedSwingWrapper looped_swing_wrapper;

#endif  // ENABLE_AUDIO

class BatteryMonitor : Looper, CommandParser {
public:
  float battery_now() {
    // This is the volts on the battery monitor pin.
    float volts = 3.3 * analogRead(batteryLevelPin) / 1024.0;
#if VERSION_MAJOR >= 2
    float pulldown = 220000;  // External pulldown
    float pullup = 2000000;  // External pullup
#else
    float pulldown = 33000;  // Internal pulldown is 33kOhm
    float pullup = 23000;  // External pullup
#endif
    return volts * (1.0 + pullup / pulldown);
  }
  float battery() const {
    return last_voltage_;
  }
  void SetLoad(bool on) {
    loaded_ = on;
  }
  bool low() const {
    return battery() < (loaded_ ? 2.6 : 3.0);
  }
protected:
  void Setup() override {
    really_old_voltage_ = old_voltage_ = last_voltage_ = battery_now();
#if VERSION_MAJOR >= 2
    pinMode(batteryLevelPin, INPUT);
#else
    pinMode(batteryLevelPin, INPUT_PULLDOWN);
#endif
  }
  void Loop() override {
    float v = battery_now();
    last_voltage_ = last_voltage_ * 0.999 + v * 0.001;
    if (monitor.ShouldPrint(Monitoring::MonitorBattery) ||
        millis() - last_print_millis_ > 20000) {
      Serial.print("Battery voltage: ");
      Serial.println(battery());
      last_print_millis_ = millis();
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
  void Help() override {
    Serial.println(" batt[ery] - show battery voltage");
  }
private:
  bool loaded_ = false;
  float last_voltage_ = 0.0;
  float old_voltage_ = 0.0;
  float really_old_voltage_ = 0.0;
  uint32_t last_print_millis_;
  uint32_t last_beep_ = 0;
};

BatteryMonitor battery_monitor;

// Used to represent a color. Uses normal 8-bit-per channel RGB.
// Note that these colors are in linear space and their interpretation
// depends on the blade.
class Color {
  public:
  Color() : r(0), g(0), b(0) {}
  Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
  // x = 0..256
  Color mix(const Color& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color( ((256-x) * r + x * other.r) >> 8,
                  ((256-x) * g + x * other.g) >> 8,
                  ((256-x) * b + x * other.b) >> 8);
  }
  uint8_t select(const Color& other) const {
    uint8_t ret = 255;
    if (other.r) ret = min(ret, r * 255 / other.r);
    if (other.g) ret = min(ret, g * 255 / other.g);
    if (other.b) ret = min(ret, b * 255 / other.b);
    return ret;
  }
  uint8_t r, g, b;
};

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
#define WS2813_800kHz 0x20      // WS2813 are close to 800 kHz but has 300 us frame set delay
#define WS2811_580kHz 0x30      // PL9823

class MonopodWS2811 {
public:
  void begin(uint32_t numPerStrip,
             void *frameBuf,
             uint8_t config,
             int pin);
  void setPixel(uint32_t num, Color color) {
    drawBuffer[num] = color;
  }
  Color getPixel(uint32_t num) {
    return drawBuffer[num];
  }

  void show();
  int busy(void);
  int in_progress(void);

  int numPixels(void) {
    return stripLen;
  }
  
  static Color drawBuffer[maxLedsPerStrip];
private:
  static uint16_t stripLen;
  static void *frameBuffer;
  static uint8_t params;
  static DMAChannel dma1, dma2, dma3;
  static void isr(void);
  static uint16_t frameSetDelay;
};

uint16_t MonopodWS2811::stripLen;
void * MonopodWS2811::frameBuffer;
uint8_t MonopodWS2811::params;
DMAChannel MonopodWS2811::dma1;
DMAChannel MonopodWS2811::dma2;
DMAChannel MonopodWS2811::dma3;
uint16_t MonopodWS2811::frameSetDelay = 0;

Color MonopodWS2811::drawBuffer[maxLedsPerStrip];
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

#ifdef INVERT_WS2811
#define WS2811_PORT_CLEAR GPIOD_PSOR
#define WS2811_PORT_SET   GPIOD_PCOR
#else
#define WS2811_PORT_CLEAR GPIOD_PCOR
#define WS2811_PORT_SET   GPIOD_PSOR
#endif

void MonopodWS2811::begin(uint32_t numPerStrip,
                          void *frameBuf,
                          uint8_t config,
                          int pin)
{
  stripLen = numPerStrip;
  frameBuffer = frameBuf;
  params = config;

  uint32_t bufsize, frequency = 400000;

  bufsize = stripLen*24;

  switch (pin) {
    case 2:  ones = 1; break;
    case 14: ones = 2; break;
    case 7:  ones = 4; break;
    case 8:  ones = 8; break;
    case 6:  ones = 16; break;
    case 20: ones = 32; break;
    case 21: ones = 64; break;
    case 5:  ones = 128; break;
    default:
      Serial.println("Invalid monopodws pin!");
  }

  // set up the buffers
  memset(frameBuffer, ones, bufsize);

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
  
  frameSetDelay = 50;
  switch (params & 0xF0) {
    case WS2811_400kHz:
      frequency = 400000;
      break;

    case WS2811_800kHz:
      frequency = 740000;
      break;

    case WS2813_800kHz:
      frequency = 740000;
      frameSetDelay = 300;
      break;

    case WS2811_580kHz:
      frequency = 580000;
      break;
  }

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
  if (micros() - update_completed_at < frameSetDelay) return 1;
  return 0;
}

int MonopodWS2811::in_progress(void)
{
  return update_in_progress;
}

static inline void Out2DMA(uint8_t *& o, uint8_t v) {
  *(o++) = (v & 128) ? 0 : ones;
  *(o++) = (v & 64) ? 0 : ones;
  *(o++) = (v & 32) ? 0 : ones;
  *(o++) = (v & 16) ? 0 : ones;
  *(o++) = (v & 8) ? 0 : ones;
  *(o++) = (v & 4) ? 0 : ones;
  *(o++) = (v & 2) ? 0 : ones;
  *(o++) = (v & 1) ? 0 : ones;
}

void CopyOut(int params, struct Color* inbuf, void* frameBuffer, int num) {
  uint8_t *o = (uint8_t*)frameBuffer;
  switch (params & 7) {
    case WS2811_RBG:
        for (int j = 0; j < num; j++)  {
          Out2DMA(o, inbuf[j].r);
          Out2DMA(o, inbuf[j].b);
          Out2DMA(o, inbuf[j].g);
        }
        break;
    case WS2811_GRB:
        for (int j = 0; j < num; j++)  {
          Out2DMA(o, inbuf[j].g);
          Out2DMA(o, inbuf[j].r);
          Out2DMA(o, inbuf[j].b);
        }
        break;
    case WS2811_GBR:
        for (int j = 0; j < num; j++)  {
          Out2DMA(o, inbuf[j].g);
          Out2DMA(o, inbuf[j].b);
          Out2DMA(o, inbuf[j].r);
        }
        break;
    default:
        for (int j = 0; j < num; j++)  {
          Out2DMA(o, inbuf[j].r);
          Out2DMA(o, inbuf[j].g);
          Out2DMA(o, inbuf[j].b);
        }
        break;
   }
}


void MonopodWS2811::show(void)
{
  // wait for any prior DMA operation
  //Serial1.print("1");
  while (update_in_progress) ; 
  //Serial1.print("2");

  CopyOut(params, drawBuffer, frameBuffer, stripLen);

  // wait for WS2811 reset
  while (micros() - update_completed_at < frameSetDelay) ;

#if defined(__MK20DX128__)
  uint32_t cv = FTM1_C1V;
  noInterrupts();
  // CAUTION: this code is timing critical.
  while (FTM1_CNT <= cv) ;
  while (FTM1_CNT > cv) ; // wait for beginning of an 800 kHz cycle
  while (FTM1_CNT < cv) ;
  FTM1_SC = 0;            // stop FTM1 timer (hopefully before it rolls over)
  update_in_progress = 1;
  //digitalWriteFast(9, HIGH); // oscilloscope trigger
  PORTB_ISFR = (1<<0);    // clear any prior rising edge
  uint32_t tmp __attribute__((unused));
  FTM1_C0SC = 0x28;
  tmp = FTM1_C0SC;        // clear any prior timer DMA triggers
  FTM1_C0SC = 0x69;
  FTM1_C1SC = 0x28;
  tmp = FTM1_C1SC;
  FTM1_C1SC = 0x69;
  dma1.enable();
  dma2.enable();          // enable all 3 DMA channels
  dma3.enable();
  FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0); // restart FTM1 timer
  //digitalWriteFast(9, LOW);

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
  update_in_progress = 1;
  //digitalWriteFast(9, HIGH); // oscilloscope trigger
  PORTB_ISFR = (1<<18);    // clear any prior rising edge
  uint32_t tmp __attribute__((unused));
  FTM2_C0SC = 0x28;
  tmp = FTM2_C0SC;         // clear any prior timer DMA triggers
  FTM2_C0SC = 0x69;
  FTM2_C1SC = 0x28;
  tmp = FTM2_C1SC;
  FTM2_C1SC = 0x69;
  dma1.enable();
  dma2.enable();           // enable all 3 DMA channels
  dma3.enable();
  FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0); // restart FTM2 timer
  //digitalWriteFast(9, LOW);

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
  update_in_progress = 1;
  //digitalWriteFast(9, HIGH); // oscilloscope trigger
  PORTA_ISFR = (1<<10);    // clear any prior rising edge
  uint32_t tmp __attribute__((unused));
  FTM2_C0SC = 0x28;
  tmp = FTM2_C0SC;         // clear any prior timer DMA triggers
  FTM2_C0SC = 0x69;
  FTM2_C1SC = 0x28;
  tmp = FTM2_C1SC;
  FTM2_C1SC = 0x69;
  dma1.enable();
  dma2.enable();           // enable all 3 DMA channels
  dma3.enable();
  FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0); // restart FTM2 timer
  //digitalWriteFast(9, LOW);

#elif defined(__MKL26Z64__)
  uint32_t sc = FTM2_SC;
  uint32_t cv = FTM2_C1V;
  noInterrupts();
  update_in_progress = 1;
  while (FTM2_CNT <= cv) ;
  while (FTM2_CNT > cv) ; // wait for beginning of an 800 kHz cycle
  while (FTM2_CNT < cv) ;
  FTM2_SC = 0;          // stop FTM2 timer (hopefully before it rolls over)
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
  //digitalWriteFast(9, LOW);
#endif
  //Serial1.print("3");
  interrupts();
  //Serial1.print("4");
}

MonopodWS2811 monopodws;
#endif

#if defined(ENABLE_WS2811)
DMAMEM int displayMemory[maxLedsPerStrip * 24 / 4];
#elif defined(ENABLE_FASTLED)
DMAMEM int displayMemory[maxLedsPerStrip * 3 / 4];
#endif

class BladeBase;

// Base class for blade styles.
// Blade styles are responsible for the colors and patterns
// of the colors of the blade. Each time run() is called, the
// BladeStyle shouldl call blade->set() to update the color
// of all the LEDs in the blade.
class BladeStyle {
public:
  virtual void activate() {}
  virtual void deactivate() {}
  virtual void run(BladeBase* blade) = 0;
  virtual bool NoOnOff() { return false; }
};

class BladeBase {
public:
  // Returns number of LEDs in this blade.
  virtual int num_leds() const = 0;

  // Returns true if the blade is supposed to be on.
  // false while "turning off".
  virtual bool is_on() const = 0;

  // Set led 'led' to color 'c'.
  virtual void set(int led, Color c) = 0;

  // Bypasses battery voltage based PWM, intended to be used for
  // brief flashes only.
  virtual void set_overdrive(int led, Color c) { set(led, c); }

  // Returns true when a clash occurs.
  // Returns true only once per clash.
  virtual bool clash() = 0;

  // Clear blade colors.
  virtual void clear() {
    for (int i = 0; i < num_leds(); i++) set(i, Color());
  }

  // Called to let the blade know that it's ok to
  // disable power now. (Usually called after is_on()
  // has returned false for some period of time.)
  virtual void allow_disable() = 0;

  virtual void Activate() = 0;

  virtual void SetStyle(BladeStyle* style) {
    if (current_style_) current_style_->deactivate();
    current_style_ = style;
    current_style_->activate();
  }

 protected:
  BladeStyle *current_style_ = NULL;
};


// Charging blade style.
// Slowly pulsating battery indicator.
class StyleCharging : public BladeStyle {
public:
  void activate() override {
    Serial.println("Charging Style");
  }
  void run(BladeBase *blade) override {
    int black_mix = 128 + 100 * sin(millis() / 500.0);
    float volts = battery_monitor.battery();
    Color colors[] = {
      Color(0,255,0),   // Green > 4.0
      Color(0,0,255),
      Color(255,128,0),
      Color(255,0,0),
      Color(255,0,0) 
    };
    float x = (4.0 - volts) * 2.0;
    int i = floor(x);
    i = clampi32(i, 0, NELEM(colors) - 2);
    // Blend colors over 0.1 volts.
    int blend = (x - i) * 10 * 255;
    blend = clampi32(blend, 0, 255);
    Color c = colors[i].mix(colors[i + 1], blend);
    c = c.mix(Color(), black_mix);
    int num_leds = blade->num_leds();

    float min_volts = 2.7;
    float max_volts = 4.2;
    float pos = (volts - min_volts) * num_leds / (max_volts - min_volts);
    int p = pos * 32;
    for (int i = 0; i < num_leds; i++) {
      blade->set(i, Color().mix(c, max(0, 256 - abs(p - i * 32))));
    }
  };

  bool NoOnOff() override { return false; }
};

// No need to templetize this one, as there are no arguments.
StyleCharging style_charging;

// Fire-style
template<int BLADE_NUM>
class StyleFire : public BladeStyle {
public:
  StyleFire(Color c1, Color c2) : c1_(c1), c2_(c2) {}

  void activate() override {
    Serial.println("Fire Style");
    for (size_t i = 0; i < NELEM(heat_); i++) heat_[i] = 0;
  }
  void run(BladeBase* blade) override {
    uint32_t m = millis();
    int num_leds = blade->num_leds();
    if (m - last_update_ >= 10) {
      last_update_ = m;

      // Note heat_[0] is tip of blade
      if (blade->clash()) {
        heat_[num_leds - 1] += 3000;
      } else if (blade->is_on()) {
        heat_[num_leds - 1] += random(random(random(1000)));
      }
      for (int i = 0; i < num_leds; i++) {
        int x = (heat_[i] * 5  + heat_[i+1] * 7 + heat_[i+2] * 4) >> 4;
        heat_[i] = max(x - random(0, 1), 0);
      }
    }
    bool zero = true;
    for (int i = 0; i < num_leds; i++) {
      int h = heat_[num_leds - 1 - i];
      Color c;
      if (h < 256) {
        c = Color().mix(c1_, h);
      } else if (h < 512) {
        c = c1_.mix(c2_, h - 256);
      } else if (h < 768) {
        c = c2_.mix(Color(255,255,255), h - 512);
      } else {
        c = Color(255,255,255);
      }
      if (h) zero = false;
      blade->set(i, c);
    }
    if (zero) blade->allow_disable();
  }

private:
  static uint32_t last_update_;
  static unsigned short heat_[maxLedsPerStrip + 2];
  Color c1_, c2_;
};

template<int BLADE_NUM>
uint32_t StyleFire<BLADE_NUM>::last_update_ = 0;

template<int BLADE_NUM>
unsigned short StyleFire<BLADE_NUM>::heat_[maxLedsPerStrip + 2];

// If you have multiple blades, make sure to use a different BLADE_NUM
// for each blade.
template<class COLOR1, class COLOR2, int BLADE_NUM=0>
class BladeStyle *StyleFirePtr() {
  static StyleFire<BLADE_NUM> style(COLOR1::color(), COLOR2::color());
  return &style;
}

class MicroEventTime {
  void SetToNow() { micros_ = micros(); millis_ = millis(); }
  uint32_t millis_since() { return millis() - millis_; }
  uint32_t micros_since() {
    if (millis_since() > (0xFFFF0000UL / 1000)) return 0xFFFFFFFFUL;
    return micros() - micros_;
  }
private:
  uint32_t millis_;
  uint32_t micros_;
};

struct OverDriveColor {
  Color c;
  bool overdrive;
};
// Simple solid color.
template<int R, int G, int B>
class Rgb {
public:
  static Color color() { return Color(R,G,B); }
  void run(BladeBase* base) {}
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.c = Color(R, G, B);
    ret.overdrive = false;
    return ret;
  }
};

// Gradient, color A at base, B at tip.
template<class A, class B>
class Gradient {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    num_leds_ = blade->num_leds();
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, led * 255 / num_leds_);
    return a;
  }
private:
  A a_;
  B b_;
  int num_leds_;
};

// Mixes randomly between A and B.
// mix is even over entire blade.
template<class A, class B>
class RandomFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    mix_ = random(255);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, mix_);
    return a;
  }
private:
  A a_;
  B b_;
  int mix_;
};

// Mixes randomly between A and B.
// mix is chosen individually for every LED.
template<class A, class B>
class RandomPerLEDFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, random(255));
    return a;
  }
private:
  A a_;
  B b_;
};

// Mixes between A and B based
// on audio. Quiet audio means more A, loud audio means more B.
// Based on a single sample instead of an average to make it flicker.
template<class A, class B>
class AudioFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    mix_ = clampi32(dynamic_mixer.last_sum() >> 4, 0, 255);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, mix_);
    return a;
  }
private:
  A a_;
  B b_;
  int mix_;
};

// Basic RGB rainbow.
class Rainbow {
public:
  void run(BladeBase* base) {
    m = millis();
  }
  OverDriveColor getColor(int led) {
    Color c(max(0, (sin_table[((m * 3 + led * 50)) & 0x3ff] >> 7)),
            max(0, (sin_table[((m * 3 + led * 50 + 1024 / 3)) & 0x3ff] >> 7)),
            max(0, (sin_table[((m * 3 + led * 50 + 1024 * 2 / 3)) & 0x3ff] >> 7)));
    OverDriveColor ret;
    ret.c = c;
    ret.overdrive = false;
    return ret;
  }
private:
  uint32_t m;
};

// Let's us use a different color right in the beginning.
template<class T, class SPARK_COLOR = Rgb<255,255,255>, int MILLIS = 200>
class OnSpark {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    spark_color_.run(blade);
    uint32_t m = millis();
    if (on_ != blade->is_on()) {
      on_ = blade->is_on();
      if (on_) on_millis_ = m;
    }
    uint32_t t = millis() - on_millis_;
    if (t < MILLIS) {
      mix_ = 255 - 255 * t / MILLIS;
    } else {
      mix_ = 0;
    }
  }
  OverDriveColor getColor(int led) {
    OverDriveColor ret = base_.getColor(led);
    OverDriveColor spark = spark_color_.getColor(led);
    ret.c = ret.c.mix(spark.c, mix_);
    return ret;
  }
private:
  bool on_;
  int mix_;
  T base_;
  SPARK_COLOR spark_color_;
  uint32_t on_millis_;
};

template<class T, class CLASH_COLOR = Rgb<255,255,255>, int CLASH_MILLIS = 40>
class SimpleClash {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    clash_color_.run(blade);
    uint32_t m = millis();
    if (blade->clash()) clash_millis_ = m;
    clash_ = m - clash_millis_ < CLASH_MILLIS;
  }
  OverDriveColor getColor(int led) {
    if (clash_) {
      return clash_color_.getColor(led);
    } else {
      return base_.getColor(led);
    }
  }
private:
  bool clash_;
  T base_;
  CLASH_COLOR clash_color_;
  uint32_t clash_millis_;
};

// Shows LOCKUP if the lockup state is true, otherwise BASE.
template<class BASE, class LOCKUP>
class Lockup {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    lockup_.run(blade);
  }
  OverDriveColor getColor(int led) {
    if (SaberBase::Lockup()) {
      return lockup_.getColor(led);
    } else {
      return base_.getColor(led);
    }
  }
private:
  BASE base_;
  LOCKUP lockup_;
};

template<class T, class STROBE_COLOR, int STROBE_FREQUENCY, int STROBE_MILLIS = 1>
class Strobe {
public:
   void run(BladeBase* blade) {
     base_.run(blade);
     strobe_color_.run(blade);
     uint32_t m = millis();
     uint32_t timeout = strobe_ ? STROBE_MILLIS : (1000/STROBE_FREQUENCY);
     if (m - strobe_millis_ > timeout) {
       strobe_millis_ += timeout;
       if (m - strobe_millis_ > STROBE_MILLIS + (1000/STROBE_FREQUENCY))
         strobe_millis_ = m;
       strobe_ = !strobe_;
     }
   }
   OverDriveColor getColor(int led) {
     if (strobe_) {
       return strobe_color_.getColor(led);
     } else {
       return base_.getColor(led);
     }
   }
private:
  bool strobe_;
  T base_;
  STROBE_COLOR strobe_color_;
  uint32_t strobe_millis_;
};

template<class T, int OUT_MILLIS, int IN_MILLIS>
class InOutHelper {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if (blade->is_on()) {
      if (extension == 0.0) {
        // We might have been off for a while, so delta might
        // be insanely high.
        extension = 0.00001;
      } else {
        extension += delta / (OUT_MILLIS * 1000.0);
        extension = min(extension, 1.0f);
      }
    } else {
      if (extension == 0.0) blade->allow_disable();
      extension -= delta / (IN_MILLIS * 1000.0);
      extension = max(extension, 0.0f);
    }
    thres = extension * blade->num_leds() * 256;
  }
  OverDriveColor getColor(int led) {
    int black_mix = clampi32(thres - led * 256, 0, 255);
    OverDriveColor ret = base_.getColor(led);
    ret.c = Color().mix(ret.c, black_mix);
    return ret;
  }
private:
  T base_;
  int thres = 0;
  float extension = 0.0;
  uint32_t last_micros_;
};

template<class T, int OUT_MILLIS, int IN_MILLIS, class SPARK_COLOR = Rgb<255,255,255> >
class InOutSparkTip {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    if ((on_ = blade->is_on())) {
      if (extension == 0.0) {
        // We might have been off for a while, so delta might
        // be insanely high.
        extension = 0.00001;
      } else {
        extension += delta / (OUT_MILLIS * 1000.0);
        extension = min(extension, 1.0f);
      }
    } else {
      if (extension == 0.0) blade->allow_disable();
      extension -= delta / (IN_MILLIS * 1000.0);
      extension = max(extension, 0.0f);
    }
    thres = extension * (blade->num_leds() + 4) * 256;
  }
  
  OverDriveColor getColor(int led) {
    OverDriveColor ret = base_.getColor(led);
    if (on_) {
      OverDriveColor spark = spark_color_.getColor(led);
      int spark_mix = clampi32(thres - 1024 - led * 256, 0, 255);
      ret.c = spark.c.mix(ret.c, spark_mix);
    }
    int black_mix = clampi32(thres - led * 256, 0, 255);
    ret.c = Color().mix(ret.c, black_mix);
    return ret;
  }
private:
  T base_;
  bool on_;
  int thres = 0;
  float extension = 0.0;
  uint32_t last_micros_;
  SPARK_COLOR spark_color_;
};

template<class T>
class Style : public BladeStyle {
public:
  void activate() override { }
  void run(BladeBase* blade) override {
    base_.run(blade);
    int num_leds = blade->num_leds();
    for (int i = 0; i < num_leds; i++) {
      OverDriveColor c = base_.getColor(i);
      if (c.overdrive) {
        blade->set_overdrive(i, c.c);
      } else {
        blade->set(i, c.c);
      }
    }
  }
private:
  T base_;
};

// Get a pointer to class.
template<class STYLE>
BladeStyle* StylePtr() {
  static Style<STYLE> style;
  return &style;
};


typedef Rgb<255,0,0> RED;
typedef Rgb<0,255,0> GREEN;
typedef Rgb<0,0,255> BLUE;
typedef Rgb<255,255,0> YELLOW;
typedef Rgb<0,255,255> CYAN;
typedef Rgb<255,0,255> MAGENTA;
typedef Rgb<255,255,255> WHITE;
typedef Rgb<0,0,0> BLACK;

typedef Rgb<223, 239, 255> AliceBlue;
typedef Rgb<0, 255, 255> Aqua;
typedef Rgb<55, 255, 169> Aquamarine;
typedef Rgb<223, 255, 255> Azure;
typedef Rgb<255, 199, 142> Bisque;
typedef Rgb<0, 0, 0> Black;
typedef Rgb<255, 213, 157> BlanchedAlmond;
typedef Rgb<0, 0, 255> Blue;
typedef Rgb<55, 255, 0> Chartreuse;
typedef Rgb<255, 55, 19> Coral;
typedef Rgb<255, 239, 184> Cornsilk;
typedef Rgb<0, 255, 255> Cyan;
typedef Rgb<255, 68, 0> DarkOrange;
typedef Rgb<255, 0, 75> DeepPink;
typedef Rgb<0, 135, 255> DeepSkyBlue;
typedef Rgb<2, 72, 255> DodgerBlue;
typedef Rgb<255, 244, 223> FloralWhite;
typedef Rgb<255, 0, 255> Fuchsia;
typedef Rgb<239, 239, 255> GhostWhite;
typedef Rgb<0, 255, 0> Green;
typedef Rgb<108, 255, 6> GreenYellow;
typedef Rgb<223, 255, 223> HoneyDew;
typedef Rgb<255, 36, 118> HotPink;
typedef Rgb<255, 255, 223> Ivory;
typedef Rgb<255, 223, 233> LavenderBlush;
typedef Rgb<255, 244, 157> LemonChiffon;
typedef Rgb<191, 255, 255> LightCyan;
typedef Rgb<255, 121, 138> LightPink;
typedef Rgb<255, 91, 50> LightSalmon;
typedef Rgb<255, 255, 191> LightYellow;
typedef Rgb<0, 255, 0> Lime;
typedef Rgb<255, 0, 255> Magenta;
typedef Rgb<233, 255, 244> MintCream;
typedef Rgb<255, 199, 193> MistyRose;
typedef Rgb<255, 199, 119> Moccasin;
typedef Rgb<255, 187, 108> NavajoWhite;
typedef Rgb<255, 97, 0> Orange;
typedef Rgb<255, 14, 0> OrangeRed;
typedef Rgb<255, 221, 171> PapayaWhip;
typedef Rgb<255, 180, 125> PeachPuff;
typedef Rgb<255, 136, 154> Pink;
typedef Rgb<255, 0, 0> Red;
typedef Rgb<255, 233, 219> SeaShell;
typedef Rgb<255, 244, 244> Snow;
typedef Rgb<0, 255, 55> SpringGreen;
typedef Rgb<14, 57, 118> SteelBlue;
typedef Rgb<255, 31, 15> Tomato;
typedef Rgb<255, 255, 255> White;
typedef Rgb<255, 255, 0> Yellow;

#define EASYBLADE(COLOR, CLASH_COLOR) \
  SimpleClash<Lockup<COLOR, AudioFlicker<COLOR, WHITE> >, CLASH_COLOR>

// The following functions are mostly for illustration.
// The templates above gives you more power and functionality.

// Arguments: color, clash color, turn-on/off time
template<class base_color,
         class clash_color,
         int out_millis,
         int in_millis,
         class lockup_flicker_color = WHITE>
BladeStyle *StyleNormalPtr() {
  typedef AudioFlicker<base_color, lockup_flicker_color> AddFlicker;
  typedef Lockup<base_color, AddFlicker> AddLockup;
  typedef SimpleClash<AddLockup, clash_color> AddClash;
  return StylePtr<InOutHelper<AddClash, out_millis, in_millis> >();
}

// Rainbow blade.
// Arguments: color, clash color, turn-on/off time
template<int out_millis,
         int in_millis,
         class clash_color = WHITE,
         class lockup_flicker_color = WHITE>
BladeStyle *StyleRainbowPtr() {
  typedef AudioFlicker<Rainbow, lockup_flicker_color> AddFlicker;
  typedef Lockup<Rainbow, AddFlicker> AddLockup;
  typedef SimpleClash<AddLockup, clash_color> AddClash;
  return StylePtr<InOutHelper<AddClash, out_millis, in_millis> >();
}

// Stroboscope, flickers the blade at the desired frequency.
// Arguments: color, clash color, turn-on/off time
template<class strobe_color,
         class clash_color,
         int frequency,
         int out_millis,
         int in_millis>
BladeStyle *StyleStrobePtr() {
  typedef Strobe<BLACK, strobe_color, frequency, 1> strobe;
  typedef Strobe<BLACK, strobe_color, 3* frequency, 1> fast_strobe;
  typedef Lockup<strobe, fast_strobe> AddLockup;
  typedef SimpleClash<AddLockup, clash_color> clash;
  return StylePtr<InOutHelper<clash, out_millis, in_millis> >();
}

#if defined(ENABLE_WS2811) || defined(ENABLE_FASTLED)
void rle_decode(const unsigned char *input,
                unsigned char *output,
                int output_length) {
  int olen = 0;
  while (olen < output_length) {
    if (*input == 255) {
      int i;
      int offset = input[1]+1;
      int len = input[2];
      input += 3;
      for (i = 0; i < len; i++) {
        *output = output[-offset];
        output++;
        olen++;
      }
    }
    else if (*input < 128) {
      memcpy(output, input+1, *input + 1);
      output += *input + 1;
      olen += *input + 1;
      input += *input + 2;
    } else {
      memset(output, input[1], *input - 128 + 2);
      output += *input - 128 + 2;
      olen += *input - 128 + 2;
      input += 2;
    }
  }
}

#include "star_wars_logo_pov_data.h"

// POV writer.
class StylePOV : public BladeStyle, public SaberBase {
public:
  StylePOV() : SaberBase(NOLINK) {
  }
  void activate() override {
    SaberBase::Link(this);
    Serial.println("POV Style");
  }
  void deactivate() override {
    SaberBase::Unlink(this);
  }
  Vec3 extrapolate_accel() {
    uint32_t now = micros();
    Vec3 sum(0.0, 0.0, 0.0);
    float sum_t = 0.0;
    for (size_t i = 0; i < NELEM(accel_entries_); i++) {
      float t = now - accel_entries_[i].t;
      sum_t += t;
      sum += accel_entries_[i].accel;
    }
    Vec3 avg = sum * (1.0 / NELEM(accel_entries_));
    float avg_t = sum_t / NELEM(accel_entries_);

    Vec3 dot_sum(0.0,0.0,0.0);
    float t_square_sum = 0.0;
    for (size_t i = 0; i < NELEM(accel_entries_); i++) {
      float t = (now - accel_entries_[i].t) - avg_t;
      Vec3 v = accel_entries_[i].accel - avg;
      t_square_sum += t * t;
      dot_sum += v * t;
    }
    Vec3 slope = dot_sum * (1.0 / t_square_sum);
#if 0
    Serial.print("SLOPE: ");
    Serial.print(slope.x * 100.0);
    Serial.print(", ");
    Serial.print(slope.y * 100.0);
    Serial.print(", ");
    Serial.println(slope.z * 100.0);
#endif
    return sum - slope * avg_t;
  }

  void SB_Accel(const Vec3& accel) override {
    entry_++;
    if (entry_ >= NELEM(accel_entries_)) entry_ = 0;
    accel_entries_[entry_].accel = accel;
    accel_entries_[entry_].t = micros();
  }

  void run(BladeBase* blade) override {
    Vec3 v = extrapolate_accel();
    float fraction = 0.5 - atan2f(v.y, v.x) * 2.0 / M_PI;
    // Serial.print("F:");
    // Serial.println(fraction);
    if (fraction < 0 || fraction > 1.0) {
      memset((unsigned char *)&MonopodWS2811::drawBuffer,
             0,
             maxLedsPerStrip * 3);
      return;
    }
    int col = fraction * NELEM(imageoffsets);
    rle_decode(imagedata + imageoffsets[col],
               (unsigned char *)&MonopodWS2811::drawBuffer,
               maxLedsPerStrip * 3);
    size_t num_leds = blade->num_leds();
    if (num_leds < maxLedsPerStrip) {
      for (size_t i = 0; i < maxLedsPerStrip; i++) {
        MonopodWS2811::drawBuffer[i] =
          MonopodWS2811::drawBuffer[i * maxLedsPerStrip / num_leds];
      }
    }
    blade->allow_disable();
  }
private:
  struct { uint32_t t; Vec3 accel; } accel_entries_[10];
  size_t entry_;
  Color* data_;
  int width_;
  int height_;
};

StylePOV style_pov;
#endif

class PowerPinInterface {
public:
  virtual void Init() = 0;
  virtual void Power(bool on) = 0;
};

template<int...>
class PowerPINS {};

template<>
class PowerPINS<> : public PowerPinInterface {
public:
  void Init() override {}
  void Power(bool power) override {
    battery_monitor.SetLoad(power);
  }
};

template<int PIN, int... PINS>
class PowerPINS<PIN, PINS...> : public PowerPinInterface {
public:
  void Init() override {
    pinMode(PIN, OUTPUT);
    rest_.Init();
  }
  void Power(bool power) override { 
    digitalWrite(PIN, power);
    rest_.Power(power);
  }
private:
  PowerPINS<PINS...> rest_;
};


#ifdef ENABLE_WS2811
// WS2811-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class WS2811_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  WS2811_Blade(int num_leds, uint8_t config, int pin, PowerPinInterface* power) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK),
    num_leds_(num_leds),
    config_(config),
    pin_(pin),
    power_(power) {
  }

  void Power(bool on) {
    power_->Power(on);
//    pinMode(bladePin, on ? OUTPUT : INPUT);
    powered_ = on;
    allow_disable_ = false;
  }

  // No need for a "deactivate", the blade stays active until
  // you take it out, which also cuts the power.
  void Activate() override {
    Serial.print("WS2811 Blade with ");
    Serial.print(num_leds_);
    Serial.println(" leds");
    power_->Init();
    Power(true);
    delay(10);
    while (monopodws.busy());
    monopodws.begin(num_leds_, displayMemory, config_, pin_);
    monopodws.show();  // Make it black
    monopodws.show();  // Make it black
    monopodws.show();  // Make it black
    while (monopodws.busy());
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return num_leds_;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color c) override {
    monopodws.setPixel(led, c);
  }
  bool clash() override {
    bool ret = clash_;
    clash_ = false;
    return ret;
  }
  void allow_disable() override {
    if (!on_) allow_disable_ = true;
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

  void SB_Clash() override { clash_=true; }

  void SB_Top() override {
    Serial.print("blade fps: ");
    loop_counter_.Print();
    Serial.println("");
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
    Serial.println(" blade on/off - turn ws2811 blade on off");
  }

protected:
  void Loop() override {
    STATE_MACHINE_BEGIN() 
    while (true) {
       while (!powered_) {
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
       while (monopodws.busy()) YIELD();
//       monopodws.begin(num_leds_, displayMemory, config_, pin_);
       monopodws.show();
       loop_counter_.Update();
       current_blade = NULL;
       YIELD();
    }
    STATE_MACHINE_END();
  }
  
private:
  int num_leds_;
  uint8_t config_;
  uint8_t pin_;
  bool on_ = false;
  bool powered_ = false;
  bool clash_ = false;
  bool allow_disable_ = false;
  LoopCounter loop_counter_;
  
  static WS2811_Blade* current_blade;
  StateMachineState state_machine_;
  PowerPinInterface* power_;
};

WS2811_Blade* WS2811_Blade::current_blade = NULL;

template<int LEDS, int CONFIG, int DATA_PIN = bladePin, class POWER_PINS = PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3> >
class WS2811_Blade *WS2811BladePtr() {
  static_assert(LEDS <= maxLedsPerStrip, "update maxLedsPerStrip");
  static POWER_PINS power_pins;
  static WS2811_Blade blade(LEDS, CONFIG, DATA_PIN, &power_pins);
  return &blade;
}
#endif

#ifdef ENABLE_FASTLED

#include <FastLED.h>

// FASTLED-type blade implementation.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
template<ESPIChipsets CHIPSET, EOrder RGB_ORDER, uint8_t SPI_DATA_RATE>
class FASTLED_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  FASTLED_Blade(int num_leds, PowerPinInterface* power) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK),
    num_leds_(num_leds),
    power_(power) {
  }

  void Power(bool on) {
    power_->Power(on);
    powered_ = on;
    allow_disable_ = false;
  }

  void Show() {
    if (spiLedSelect != -1 || spiDataOut != spiLedDataOut)
      Serial.println("SPI data conflict!");
    if (spiLedSelect != -1 || spiClock != spiLedClock)
      Serial.println("SPI clock conflict!");
    if (spiLedSelect != -1){
      SPI.beginTransaction(SPISettings(SPI_DATA_RATE, MSBFIRST, SPI_MODE0));
      digitalWrite(spiLedSelect, HIGH);  // enable access to LEDs
      FastLED.show();
      digitalWrite(spiLedSelect, LOW);
      SPI.endTransaction();   // allow other libs to use SPI again
    } else {
      // Bitbang on separate pins, need to lock anything.
      FastLED.show();
    }
  }

  // No need for a "deactivate", the blade stays active until
  // you take it out, which also cuts the power.
  void Activate() override {
    Serial.print("FASTLED Blade with ");
    Serial.print(num_leds_);
    Serial.println(" leds");
    FastLED.addLeds<CHIPSET, spiLedDataOut, spiLedClock, EOrder, SPI_DATA_RATE>((struct CRGB*)displayMemory, num_leds_);
    power_->Init();
    Power(true);
    delay(10);

    clear();
    Show();
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return num_leds_;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color c) override {
    ((Color*)displayMemory)[led] = c;
  }
  bool clash() override {
    bool ret = clash_;
    clash_ = false;
    return ret;
  }
  void allow_disable() override {
    if (!on_) allow_disable_ = true;
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

  void SB_Clash() override { clash_=true; }

  void SB_Top() override {
    Serial.print("blade fps: ");
    loop_counter_.Print();
    Serial.println("");
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
    Serial.println(" blade on/off - turn apa102 blade on off");
  }

protected:
  void Loop() override {
    if (!powered_) {
      loop_counter_.Reset();
      return;
    }
    int m = millis();
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
  bool clash_ = false;
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
#endif

// LED interface.
class LEDInterface {
public:
  // Given a color, return a the right PWM level (0-255).
  virtual int PWM(Color c) = 0;

  // Same as PWM(), but ignores battery voltage.
  virtual int PWM_overdrive(Color c) = 0;
};

// This code turns down the PWM duty cycle when the battery voltage
// is too high for the LED. We use a simple affine model to approximate
// the amp/volt curve of the LED. This model requires that we know
// two amp/volt points to draw a line through. The higher of those two
// points also doubles as the maximum amps that the LED will
// tolerate. So we use the model to calculate what the amps would be
// given the current battery voltage, if the amps is higher than
// the maximum then we adjust the PWM rate so that the average amps
// equals the maximum amps.
template<class LED>
class DriveLogic : public LEDInterface {
public:
  float PWMMultiplier() {
    float V = battery_monitor.battery();
    float dv = LED::MaxVolts - LED::P2Volts;
    float di = LED::MaxAmps - LED::P2Amps;
    float delta = dv / di;
    float amps = (V - LED::MaxVolts + LED::MaxAmps * delta) / (delta + LED::R);
    if (monitor.ShouldPrint(Monitoring::MonitorPWM)) {
      Serial.print("AMPS = ");
      Serial.print(amps);
      Serial.print(" / ");
      Serial.print(LED::MaxAmps);
      Serial.print(" PWM = ");
      Serial.println(100.0 * LED::MaxAmps / amps);
    }
    if (amps <= LED::MaxAmps) {
      return 1.0f;
    }

    return LED::MaxAmps / amps;
  }
  int PWM(Color c) override {
    return c.select(Color(LED::Red, LED::Green, LED::Blue)) * PWMMultiplier();
  }
  int PWM_overdrive(Color c) override {
    return c.select(Color(LED::Red, LED::Green, LED::Blue));
  }
};

template<class LED>
class LEDInterface* LEDPtr() {
  static DriveLogic<LED> led;
  return &led;
}

class PWMPin {
public:
  void Init(int pin, LEDInterface*c) {
    pin_ = pin;
    c_ = c;
  }
  void Activate() {
    if (pin_ == -1) return;
    analogWriteFrequency(pin_, 1000);
    analogWrite(pin_, 0);  // make it black
  }
  void set(const Color& c) {
    if (pin_ == -1) return;
    analogWrite(pin_, c_->PWM(c));
  }
  void set_overdrive(const Color& c) {
    if (pin_ == -1) return;
    analogWrite(pin_, c_->PWM_overdrive(c));
  }
  
  int pin_;
  LEDInterface* c_;
};

// Simple blade, LED string or LED star with optional flash on clash.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
class Simple_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  Simple_Blade(LEDInterface* c1,
               LEDInterface* c2,
               LEDInterface* c3,
               LEDInterface* c4,
               int pin1,
               int pin2,
               int pin3,
               int pin4) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK) {
    pins_[0].Init(pin1, c1);
    pins_[1].Init(pin2, c2);
    pins_[2].Init(pin3, c3);
    pins_[3].Init(pin4, c4);
  }

  void Activate() override {
    Serial.println("Simple Blade");
    analogWriteResolution(8);
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i].Activate();
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return 1;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color c) override {
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i].set(c);
  }

  void set_overdrive(int led, Color c) override {
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i].set_overdrive(c);
  }

  bool clash() override {
    bool ret = clash_;
    clash_ = false;
    return ret;
  }
  void allow_disable() override {
    power_ = false;
  }
  
  // SaberBase implementation
  void SB_IsOn(bool *on) override {
    if (on_) *on = true;
  }
  void SB_On() override {
    battery_monitor.SetLoad(true);
    power_ = on_ = true;
  }
  void SB_Off() override {
    battery_monitor.SetLoad(false);
    on_ = false;
  }
  void SB_Clash() override {
    clash_ = true;
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
    Serial.println(" blade on/off - turn simple blade on off");
  }

protected:
  void Loop() override {
    if (!power_) return;
    current_style_->run(this);
  }
  
private:
  PWMPin pins_[4];
  static bool on_;
  static bool power_;
  bool clash_ = false;
};

bool Simple_Blade::on_ = false;
bool Simple_Blade::power_ = false;

template<class LED1, class LED2, class LED3, class LED4,
         int pin1 = bladePowerPin1,
         int pin2 = bladePowerPin2,
         int pin3 = bladePowerPin3,
         int pin4 = bladePin>
class Simple_Blade *SimpleBladePtr() {
  static Simple_Blade blade(LEDPtr<LED1>(), LEDPtr<LED2>(), LEDPtr<LED3>(), LEDPtr<LED4>(), pin1, pin2, pin3, pin4);
  return &blade;
}


#ifdef V2
// String blade, Segment LED string. All segments assumed to be the same color.
// Note that this class does nothing when first constructed. It only starts
// interacting with pins and timers after Activate() is called.
#define STRING_SEGMENTS 6
class String_Blade : public SaberBase, CommandParser, Looper, public BladeBase {
public:
  String_Blade(LEDInterface* c, int clash_pin, LEDInterface* clash_led) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK),
    c_(c) {
    clash_pin_.Init(clash_pin, clash_led);
  }

  void Activate() override {
    Serial.println("String Blade");
    analogWriteResolution(8);
    for (int i = 0; i < STRING_SEGMENTS; i++) {
      analogWriteFrequency(pin_[i], 1000);
      analogWrite(pin_[i], 0);  // make it black
    }
    clash_pin_.Activate();
    CommandParser::Link();
    Looper::Link();
    SaberBase::Link(this);
  }

  // BladeBase implementation
  int num_leds() const override {
    return STRING_SEGMENTS;
  }
  bool is_on() const override {
    return on_;
  }
  void set(int led, Color c) override {
    analogWrite(pin_[led], c_->PWM(c));
    if (led == 0) clash_pin_.set(c);
  }

  void set_overdrive(int led, Color c) override {
    analogWrite(pin_[led], c_->PWM_overdrive(c));
    if (led == 0) clash_pin_.set_overdrive(c);
  }

  bool clash() override {
    bool ret = clash_;
    clash_ = false;
    return ret;
  }
  void allow_disable() override {
    power_ = false;
  }
  
  // SaberBase implementation
  void SB_IsOn(bool *on) override {
    if (on_) *on = true;
  }
  void SB_On() override {
    battery_monitor.SetLoad(false);
    power_ = on_ = true;
  }
  void SB_Off() override {
    battery_monitor.SetLoad(true);
    on_ = false;
  }
  void SB_Clash() override {
    clash_ = true;
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
    Serial.println(" blade on/off - turn string blade on off");
  }

protected:
  void Loop() override {
    if (!power_) return;
    current_style_->run(this);
  }
  
private:
  LEDInterface *c_;
  PWMPin clash_pin_;
  static int pin_[STRING_SEGMENTS];
  static bool on_;
  static bool power_;
  static bool clash_;
};

int String_Blade::pin_[STRING_SEGMENTS] = {
    bladePowerPin1,
    bladePowerPin2,
    bladePowerPin3,
    bladePowerPin4,
    bladePowerPin5,
    bladePowerPin6,
};
bool String_Blade::on_ = false;
bool String_Blade::power_ = false;
bool String_Blade::clash_ = true;

class NoLED;

// Possibly one LED driver per channel...
template<class LED, int CLASH_PIN = -1, class CLASH_LED = NoLED>
class String_Blade *StringBladePtr() {
  static String_Blade blade(LEDPtr<LED>(), CLASH_PIN, LEDPtr<CLASH_LED>());
  return &blade;
}
#endif

// CONFIGURABLE
// These structs below describe the properties of the LED circuit
// so that we know how to drive it properly.
// I've added structs for all the Cree XP-E2 LEDs, you can modify
// or copy-and-add as many other ones as you like.
// See DriveLogic above for more explanations.
struct CreeXPE2White {
  // These four values come from the datasheet.

  // Maximum (average) amperes for the LED.
  // If you have multiple LEDs hooked up in parallel, multiply
  // the amps from the datasheet by the number of LEDs.
  static constexpr float MaxAmps = 1.0;
  // LED voltage at MaxAmps.
  static constexpr float MaxVolts = 3.15;

  // P2Amps should be less than MaxAmps.
  // If you have multiple LEDs hooked up in parallel, multiply
  // the amps from the datasheet by the number of LEDs.
  static constexpr float P2Amps = 0.7;
  // LED voltage at P2Amps.
  static constexpr float P2Volts = 3.05;

  // Value of the actual resistor you hooked up to the LED.
  static constexpr float R = 0.55;

  // LED color
  static const int Red = 255;
  static const int Green = 255;
  static const int Blue = 255;
};

struct CreeXPE2Blue {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 3.4;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 3.1;
  static constexpr float R = 0.24;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

struct CreeXPE2Green {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 3.7;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 3.2;
  static constexpr float R = 0.0;
  static const int Red = 0;
  static const int Green = 255;
  static const int Blue = 0;
};

struct CreeXPE2PCAmber {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 3.28;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 3.05;
  static constexpr float R = 0.0;
  // TODO(hubbe): Find correct values for PC Amber
  static const int Red = 255;
  static const int Green = 128;
  static const int Blue = 0;
};

struct CreeXPE2Red {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 2.65;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 2.2;
  static constexpr float R = 0.0;
  static const int Red = 255;
  static const int Green = 0;
  static const int Blue = 0;
};

struct CreeXPE2RedOrange {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 2.65;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 2.2;
  static constexpr float R = 0.0;
  // TODO(hubbe): Find correct values for red-orange
  static const int Red = 255;
  static const int Green = 196;
  static const int Blue = 0;
};

struct CreeXPE2Amber {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 2.65;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 2.2;
  static constexpr float R = 0.0;
  // TODO(hubbe): Find correct values for Amber
  static const int Red = 255;
  static const int Green = 100;
  static const int Blue = 0;
};

struct CreeXPL {
  static constexpr float MaxAmps = 3.0;
  static constexpr float MaxVolts = 3.32;
  static constexpr float P2Amps = 1.05;
  static constexpr float P2Volts = 2.95;
  static constexpr float R = 0.15;
  static const int Red = 255;
  static const int Green = 255;
  static const int Blue = 255;
};

// This is a "superbright 3mm blue led" that I found on ebay.
// I used this to build an LED string with ~150 LEDs.
// Since I don't have a proper datasheet, I measured these values.
struct Blue3mmLED {
  static constexpr float MaxAmps = 0.03 * 150;
  static constexpr float MaxVolts = 3.4;
  static constexpr float P2Amps = 0.016 * 150;
  static constexpr float P2Volts = 3.2;
  static constexpr float R = 0.05; // assumed wire resistance
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

// This blade has 100 x 0.5W red 8mm straw-hat LEDs.
// Since I don't have a proper datasheet, I measured these values.
struct Red8mmLED100 {
  static constexpr float MaxAmps = 11.74;
  static constexpr float MaxVolts = 2.97;
  static constexpr float P2Amps = 7.35;
  static constexpr float P2Volts = 2.5;
  static constexpr float R = 0.001;
  static const int Red = 255;
  static const int Green = 0;
  static const int Blue = 0;
};

// This blade has 100 x 0.5W blue 8mm straw-hat LEDs.
// Since I don't have a proper datasheet, I measured these values.
struct Blue8mmLED100 {
  static constexpr float MaxAmps = 8.9;
  static constexpr float MaxVolts = 3.8;
  static constexpr float P2Amps = 6.05;
  static constexpr float P2Volts = 3.5;
  static constexpr float R = 0.001;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

// For when there is no LED hooked up to a channel.
struct NoLED {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 1000.0;
  static constexpr float P2Amps = 0.0;
  static constexpr float P2Volts = 0.0;
  static constexpr float R = 0.0;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 0;
};

#define CONFIGARRAY(X) X, NELEM(X)

struct Preset {
  // Sound font.
  const char* font;

  // Sound track
  const char* track;

  // Blade config.
  BladeStyle* style;
#if NUM_BLADES >= 2
  BladeStyle* style2;
#endif
#if NUM_BLADES >= 3
  BladeStyle* style3;
#endif
#if NUM_BLADES >= 4
  BladeStyle* style4;
#endif
};

struct BladeConfig {
  // Blade identifier resistor.
  int ohm;

  // Blade driver.
  BladeBase* blade;
#if NUM_BLADES >= 2
  BladeBase* blade2;
#endif
#if NUM_BLADES >= 3
  BladeBase* blade3;
#endif
#if NUM_BLADES >= 4
  BladeBase* blade4;
#endif

  // Blade presets
  Preset* presets;
  size_t num_presets;
};

#ifdef CONFIG_FILE

#define CONFIG_PRESETS
#include CONFIG_FILE
#undef CONFIG_PRESETS
#else

// CONFIGURABLE
// Each preset line consists of:
// { "font directory", "sound track", Style },
// Where Style is one of:
//   StyleNormalPtr<BaseColor, FlashColor, out millis, in millis>()
//   StyleFirePtr<LowHeatColor, HighHeatColor>()
//   StyleRainBowPtr<out millis, in millis>()
//   StyleStrobePtr<BaseColor, FlashColor, out millis, in millis>()
//   &style_charging
// All colors can be specied as three numbers or using one the handy macros above.
// If you wish to have different presets for different blades, copy this array and
// name it something other than "preset", then use the new name inside the blade
// configuration array below. See "simple_presets" and "charging_presets"
// below for examples.
Preset presets[] = {
  { "font03", "tracks/title.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>() },
  { "graflex7", "tracks/cantina.wav",
    StylePtr<InOutSparkTip<EASYBLADE(BLUE, WHITE), 300, 800> >() },
  { "caliban", "tracks/duel.wav", StyleFirePtr<RED, YELLOW>() },
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 300, 800>() },
  { "font02", "tracks/title.wav", StyleFirePtr<BLUE, CYAN>() },
  { "igniter/font4", "tracks/duel.wav",
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >() },
  { "font01", "tracks/duel.wav",
    StyleNormalPtr<WHITE, RED, 300, 800, RED>() },
  { "font01", "tracks/walls.wav",
      StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>() },
  { "font01", "tracks/title.wav", 
    StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >() },
  { "font02", "tracks/cantina.wav", StyleNormalPtr<
    Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>() },
  { "font02", "tracks/cantina.wav", StyleRainbowPtr<300, 800>() },
  { "font02", "tracks/cantina.wav", StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>() },
  { "font02", "tracks/cantina.wav", &style_pov },

  { "charging", "tracks/duel.wav", &style_charging },
};

Preset red_presets[] = {
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 100, 200>()},
  { "caliban", "tracks/duel.wav", StyleNormalPtr<RED, WHITE, 100, 200>()},
  { "font02", "tracks/cantina.wav", StyleStrobePtr<RED, WHITE, 15, 100, 200>()},
};

Preset simple_presets[] = {
  { "font01", "tracks/title.wav", StyleNormalPtr<BLUE, WHITE, 100, 200>() },
  { "font02", "tracks/duel.wav", StyleNormalPtr<BLUE, WHITE, 100, 200>() },
  { "font02", "tracks/cantina.wav", StyleStrobePtr<BLUE, WHITE, 15, 100, 200>() },
};

Preset white_presets[] = {
  { "font01", "tracks/title.wav", StyleNormalPtr<WHITE, WHITE, 100, 200>() },
  { "font02", "tracks/duel.wav", StyleNormalPtr<WHITE, WHITE, 100, 200>() },
  { "font02", "tracks/cantina.wav", StyleStrobePtr<WHITE, WHITE, 15, 100, 200>() },
};

Preset charging_presets[] = {
  { "charging", "", &style_charging },
};

Preset testing_presets[] = {
  { "font02", "tracks/cantina.wav", StyleRainbowPtr<300, 800>() },
  { "graflex4", "tracks/title.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>() },
  { "graflex4", "tracks/cantina.wav", StyleNormalPtr<BLUE, RED, 300, 800>() },
  { "caliban", "tracks/duel.wav", StyleFirePtr<RED, YELLOW>() },
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 300, 800>() },
  { "graflex5", "tracks/title.wav", StyleFirePtr<BLUE, CYAN>() },
  { "igniter/font4", "tracks/duel.wav", StyleNormalPtr<GREEN, WHITE, 300, 800>() },
  { "graflex4", "tracks/duel.wav", StyleNormalPtr<WHITE, RED, 300, 800>() },
  { "graflex4", "tracks/walls.wav", StyleNormalPtr<YELLOW, BLUE, 300, 800>() },
  { "graflex4", "tracks/title.wav", StyleNormalPtr<MAGENTA, WHITE, 300, 800>() },
  { "graflex5", "tracks/cantina.wav", StyleRainbowPtr<300, 800>() },
  { "graflex5", "tracks/cantina.wav", StyleStrobePtr<WHITE, RED, 15, 300, 800>() },
  { "graflex5", "tracks/cantina.wav", &style_pov },

  { "charging", "tracks/duel.wav", &style_charging },
};

// CONFIGURABLE
// Each line of configuration should be:
//     { blade id resistor ohms, blade, CONFIGARRAY(array of presets) },
// Where "blade", can be one of the following:
//     WS2811BladePtr<number of leds, WS2811 configuration flags>()
//     SimpleBladePTR<Color of channel 1, color of channel 2, color of channel 3, color of channel 4>()
// All colors can be specied as three numbers or using one the handy macros above.

BladeConfig blades[] = {
#ifdef ENABLE_WS2811
  // PL9823 blade, 97 LEDs
  {   2600, WS2811BladePtr<97, WS2811_580kHz>(), CONFIGARRAY(presets) },

  // Charging adapter, single PL9823 LED.
  {  15000, WS2811BladePtr<1, WS2811_580kHz>(), CONFIGARRAY(charging_presets) },
  // For testing (makes the charging blade behave like a normal blade.)
  //  {  15000, WS2811BladePtr<1, WS2811_580kHz>(), CONFIGARRAY(presets) },

  // WS2811 string blade 144 LEDs
  {   7800, WS2811BladePtr<144, WS2811_800kHz | WS2811_GRB>(), CONFIGARRAY(presets) },
#endif

  // Simple blue string blade.
  {   5200, SimpleBladePtr<Blue3mmLED,Blue3mmLED,Blue3mmLED,NoLED>(), CONFIGARRAY(simple_presets) },

  // Blue-Blue-White LED star
  { 20000, SimpleBladePtr<CreeXPE2White, CreeXPE2Blue, CreeXPE2Blue, NoLED>(), CONFIGARRAY(simple_presets) },

  // Blue LED string (with green flourescent tube)
  { 27000, SimpleBladePtr<Blue8mmLED100, Blue8mmLED100, Blue8mmLED100, NoLED>(), CONFIGARRAY(simple_presets) },

  // Red LED string
//  { 33000, SimpleBladePtr<Red8mmLED100, Red8mmLED100, Red8mmLED100, NoLED>(), CONFIGARRAY(red_presets) },

  // 3 x Cree XL-L LED star
  { 100000, SimpleBladePtr<CreeXPL, CreeXPL, CreeXPL, NoLED>(), CONFIGARRAY(white_presets) },

  // Testing configuration. 
  { 130000, SimpleBladePtr<CreeXPE2Red, CreeXPE2Green, Blue3mmLED, NoLED>(), CONFIGARRAY(testing_presets) },
};

#endif // CONFIG_FILE

class DebouncedButton {
public:
  void Update() {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!Read()) YIELD();
      pushed_ = true;
      do {
        if (Read()) last_on_ = millis();
        YIELD();
      } while (millis() - last_on_ < timeout());
      pushed_ = false;
    }
    STATE_MACHINE_END();
  }
  bool DebouncedRead() {
    Update();
    return pushed_;
  }

protected:
  virtual uint32_t timeout() { return 10; }
  virtual bool Read() = 0;

private:
  uint32_t last_on_;
  bool pushed_ = false;
  StateMachineState state_machine_;
};

// Simple button handler. Keeps track of clicks and lengths of pushes.
class ButtonBase : public Looper,
                   public CommandParser,
                   public DebouncedButton {
public:
  enum ClickType {
    CLICK_NONE,
    CLICK_SHORT,
    CLICK_LONG
  };

  ButtonBase(const char* name)
    : Looper(),
      CommandParser(),
      name_(name),
      pushed_(false),
      click_(CLICK_NONE),
      push_millis_(0) {
  }
  int pushed_millis() {
    if (pushed_) return millis() - push_millis_;
    return 0;
  }

  ClickType GetClick() {
    ClickType ret = click_;
    click_ = CLICK_NONE;
    return ret;
  }

  void EatClick() {
    eat_click_ = true;
  }

protected:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!DebouncedRead()) YIELD();
      pushed_ = true;
      push_millis_ = millis();
      while (DebouncedRead()) YIELD();
      pushed_ = false;
      if (eat_click_) {
        eat_click_ = false;
      } else {
        if (millis() - push_millis_ < 500) {
          click_ = CLICK_SHORT;
        } else {
          click_ = CLICK_LONG;
        }
      }
    }
    STATE_MACHINE_END();
  }
  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      click_ = CLICK_SHORT;
      return true;
    }
    return false;
  }

  void Help() override {
    Serial.print(" ");
    Serial.print(name_);
    Serial.print(" - clicks the ");
    Serial.print(name_);
    Serial.println(" button");
  }

  int next_state_ = -1;
  uint32_t sleep_until_ = 0;
  const char* name_;
  bool pushed_;
  bool eat_click_ = false;
  ClickType click_;
  int push_millis_;

  StateMachineState state_machine_;
};

class Button : public ButtonBase {
public:
  Button(int pin, const char* name) : ButtonBase(name), pin_(pin) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_digital.pinMode(pin, INPUT_PULLUP, RISING);
#endif
  }
protected:
  bool Read() override {
    return digitalRead(pin_) == LOW;
  }
  uint8_t pin_;
};

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

class TouchButton : public ButtonBase {
public:
  TouchButton(int pin, int threshold, const char* name)
    : ButtonBase(name),
      pin_(pin),
      threshold_(threshold) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_touch.pinMode(pin, threshold);
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
protected:

  bool Read() override {
    return is_pushed_;
  }

  virtual uint32_t timeout() {
    return 50;
  }

  void Update(int value) {
    if (print_next_) {
      Serial.print("Touch ");
      Serial.print(name_);
      Serial.print(" = ");
      Serial.print(value);
      Serial.print(" (");
      Serial.print(min_);
      Serial.print(" - ");
      Serial.print(max_);
      Serial.println(")");
      
      print_next_ = false;
      min_ = 10000000;
      max_ = 0;
    } else {
      min_ = min(value, min_);
      max_ = max(value, max_);
    }
    is_pushed_ = value > threshold_;
  }

  void Loop() override {
    ButtonBase::Loop();
    if (monitor.ShouldPrint(Monitoring::MonitorTouch)) {
      print_next_ = true;
    }
    STATE_MACHINE_BEGIN();
    while (true) {
      // Wait until it's our turn.
      while (current_button) YIELD();
      current_button = this;

      // Initiate touch read.
      {
        int32_t ch = pin2tsi[pin_];
        *portConfigRegister(pin_) = PORT_PCR_MUX(0);
        SIM_SCGC5 |= SIM_SCGC5_TSI;
      
#if defined(KINETISK) && !defined(HAS_KINETIS_TSI_LITE)
        TSI0_GENCS = 0;
        TSI0_PEN = (1 << ch);
        TSI0_SCANC = TSI_SCANC_REFCHRG(3) | TSI_SCANC_EXTCHRG(CURRENT);
        TSI0_GENCS = TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_PS(PRESCALE) | TSI_GENCS_TSIEN | TSI_GENCS_SWTS;
#elif defined(KINETISL) || defined(HAS_KINETIS_TSI_LITE)
        TSI0_GENCS = TSI_GENCS_REFCHRG(4) | TSI_GENCS_EXTCHRG(3) | TSI_GENCS_PS(PRESCALE)
          | TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_TSIEN | TSI_GENCS_EOSF;
        TSI0_DATA = TSI_DATA_TSICH(ch) | TSI_DATA_SWTS;
#endif
      }
      // Wait for result to be available.
      SLEEP_MICROS(10);
      while (TSI0_GENCS & TSI_GENCS_SCNIP) YIELD();
      SLEEP_MICROS(1);

      // Read resuilt.
#if defined(KINETISK) && !defined(HAS_KINETIS_TSI_LITE)
      Update(*((volatile uint16_t *)(&TSI0_CNTR1) + (pin2tsi[pin_])));
#elif defined(KINETISL) || defined(HAS_KINETIS_TSI_LITE)
      Update(TSI0_DATA & 0xFFFF);
#endif
      // Let someone else have a turn.
      current_button = NULL;
      YIELD();
    }
    STATE_MACHINE_END();
  }

  static TouchButton *current_button;
  bool print_next_ = false;
  uint8_t pin_;
  int threshold_;
  int min_ = 100000000;
  int max_ = 0;
  bool is_pushed_ = false;

  StateMachineState state_machine_;
};

TouchButton* TouchButton::current_button = NULL;

// Menu system

// Configuration system:
// switch track
// + BLADE
// +-+ effect
// +-+ color
// +-+ sound font

// STATE_OFF:
// Power click  -> STATE_ON
// Power double click
// Power long click
// AUX click  -> STATE_ON
// AUX double click -> MUSIC_ON
// AUX long click -> SILENT_ON

// STATE_RUN:
// 


#if 0
class Script : Looper, StateMachine {
public:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    SLEEP(2000);
    CommandParser::DoParse("on", NULL);
    SLEEP(2000);
    while (true) {
      if (dac.isSilent()) {
        SLEEP(2000);
      } else {
        CommandParser::DoParse("clash", NULL);
        Serial.print("alloced: ");
        Serial.println(mallinfo().uordblks);
        SLEEP(100);
      }
    }
    STATE_MACHINE_END();
  }
  void Run() {
    state_machine_.reset_state_machine();
    run_ = true;
  }
  bool run_ = false;
};

Script script;
#endif


// The Saber class implements the basic states and actions
// for the saber.
class Saber : CommandParser, Looper, SaberBase {
public:
  Saber() : CommandParser(),
  // CONFIGURABLE, use "monitor touch" to see the range of
  // values from the touch sensor, then select a value that is
  // big enough to not trigger the touch sensor randomly.
#ifdef POWER_TOUCHBUTTON
            power_(powerButtonPin, 1700, "pow"),
#else
            power_(powerButtonPin, "pow"),
#endif
            aux_(auxPin, "aux"),
            aux2_(aux2Pin, "aux2") {}

  bool IsOn() const {
    return on_;
  }

  bool NeedsPower() {
    return on_ || current_preset_->style->NoOnOff();
  }

  void On() {
    if (on_) return;
    if (current_preset_->style->NoOnOff()) return;
    Serial.println("Ignition.");
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    delay(10);             // allow time to wake up

    on_ = true;
    SaberBase::DoOn();
  }

  void Off() {
    on_ = false;
    SaberBase::DoOff();
  }

  uint32_t last_clash_ = 0;
  void Clash() {
    // No clashes in lockup mode.
    if (SaberBase::Lockup()) return;
    // TODO: Pick clash randomly and/or based on strength of clash.
    uint32_t t = millis();
    if (t - last_clash_ < 100) return;
    if (on_) {
      ButtonBase *a;
      if (aux_on_) {
        a = &aux_;
      } else {
        a = &power_;
      }

      if (a->pushed_millis()) {
        SaberBase::SetLockup(true);
        SaberBase::DoBeginLockup();
        a->EatClick();
      } else{
        SaberBase::DoClash();
      }
    } else {
      if (power_.pushed_millis()) {
        // Avoid skipping lots of steps.
        if (t - last_clash_ < 400) return;
        power_.EatClick();
        Serial.println("Previous preset");
        previous_preset();
      }
    }
    last_clash_ = t;
  }

  bool chdir(const char* dir) {
    if (strlen(dir) > 1 && dir[strlen(dir)-1] == '/') {
      Serial.println("Directory must not end with slash.");
      return false;
    }
#ifdef ENABLE_AUDIO
    looped_swing_wrapper.Deactivate();
    monophonic_font.Deactivate();
    polyphonic_font.Deactivate();

    // Stop all sound!
    // TODO: Move scanning to wav-playing interrupt level so we can
    // interleave things without worry about memory corruption.
    for (size_t i = 0; i < NELEM(wav_players); i++) {
      wav_players[i].Stop();
    }
#endif

    strcpy(current_directory, dir);
    if (strlen(current_directory) && 
        current_directory[strlen(current_directory)-1] != '/') {
      strcat(current_directory, "/");
    }

#ifdef ENABLE_AUDIO
    Effect::ScanDirectory(dir);
    SaberBase* font = NULL;
    if (clsh.files_found()) {
      polyphonic_font.Activate();
      font = &polyphonic_font;
    } else if (clash.files_found()) {
      monophonic_font.Activate();
      font = &monophonic_font;
    } else if (boot.files_found()) {
      monophonic_font.Activate();
      font = &monophonic_font;
    }
    if (font) {
      if (swingl.files_found()) {
        looped_swing_wrapper.Activate(font);
      }
    }
#endif
    return false;
  }
  
  // Select preset (font/style)
  void SetPreset(Preset* preset) {
    current_preset_ = preset;
    current_config_->blade->SetStyle(preset->style);
#if NUM_BLADES >= 2
    current_config_->blade2->SetStyle(preset->style2);
#endif
#if NUM_BLADES >= 3
    current_config_->blade3->SetStyle(preset->style3);
#endif
#if NUM_BLADES >= 4
    current_config_->blade4->SetStyle(preset->style4);
#endif
    chdir(preset->font);
  }

  // Go to the next Preset.
  void next_preset() {
    if (current_config_->num_presets == 1)
      return;
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
#ifdef ENABLE_AUDIO
    beeper.Beep(0.05, 2000.0);
#endif
    Preset* tmp = current_preset_ + 1;
    if (tmp == current_config_->presets + current_config_->num_presets) {
      tmp = current_config_->presets;
    }
    SetPreset(tmp);
    SaberBase::DoNewFont();
  }

  // Go to the previous Preset.
  void previous_preset() {
    if (current_config_->num_presets == 1)
      return;
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
#ifdef ENABLE_AUDIO
    beeper.Beep(0.05, 2000.0);
#endif
    Preset* tmp = current_preset_ - 1;
    if (tmp == current_config_->presets - 1) {
      tmp = current_config_->presets + current_config_->num_presets - 1;
    }
    SetPreset(tmp);
    SaberBase::DoNewFont();
  }

  // Measure and return the blade identifier resistor.
  float id() {
    pinMode(bladeIdentifyPin, INPUT_PULLUP);
    delay(100);
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

  // Called from setup to identify the blade and select the right
  // Blade driver, style and sound font.
  void FindBlade() {
    float resistor = id();
    
    size_t best_config = 0;
    float best_err = 1000000.0;
    for (size_t i = 0; i < sizeof(blades) / sizeof(blades)[0]; i++) {
      float err = fabs(resistor - blades[i].ohm);
      if (err < best_err) {
        best_config = i;
        best_err = err;
      }
    }
    Serial.print("blade= ");
    Serial.println(best_config);
    current_config_ = blades + best_config;
    current_config_->blade->Activate();
#if NUM_BLADES >= 2
    current_config_->blade2->Activate();
#endif
#if NUM_BLADES >= 3
    current_config_->blade3->Activate();
#endif
#if NUM_BLADES >= 4
    current_config_->blade4->Activate();
#endif
    SetPreset(current_config_->presets);
  }

  // Select next sound font (in alphabetic order)
  // Set sign to -1 to get the previous sound font instead.
  void next_directory(int sign = 1) {
#ifdef ENABLE_SD
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
	  first.close();
          first = f;
        } else {
          if (cmpdir(f.name(), first.name())*sign < 0) {
	    first.close();
	    first = f;
	  }
        }
        if (cmpdir(f.name(), current_directory)*sign <= 0) continue;
        if (best && cmpdir(f.name(), best.name())*sign > 0) continue;
	best.close();
        best = f;
      }
      if (best) {
        if (chdir(best.name())) {
          SaberBase::DoNewFont();
          return;
        }
      } else if (first) {
        if (chdir(first.name())) {
          SaberBase::DoNewFont();
          return;
        }
      }
      dir.close();
      first.close();
      best.close();
    } while (++tries <= dirs);
#endif
  }

  void SB_Accel(const Vec3& accel) override {
    if ( (accel_ - accel).len2() > 1.0) {
      // Needs de-bouncing
      Clash();
    }
    accel_ = accel;
    if (monitor.ShouldPrint(Monitoring::MonitorClash)) {
      Serial.print("ACCEL: ");
      Serial.print(accel.x);
      Serial.print(", ");
      Serial.print(accel.y);
      Serial.print(", ");
      Serial.println(accel.z);
    }
  }

  enum StrokeType {
    NO_STROKE,
    TWIST_LEFT,
    TWIST_RIGHT,
  };
  struct Stroke {
    StrokeType type;
    uint32_t start_millis;
    uint32_t end_millis;
    uint32_t length() const { return end_millis - start_millis; }
  };

  Stroke strokes[5];

  void ProcessStrokes() {
    if (monitor.IsMonitoring(Monitoring::MonitorStrokes)) {
      Serial.print("Stroke: ");
      switch (strokes[NELEM(strokes)-1].type) {
	case TWIST_LEFT:
	  Serial.print("TwistLeft");
	  break;
	case TWIST_RIGHT:
	  Serial.print("TwistRight");
	  break;
	default: break;
      }
      Serial.print(" len=");
      Serial.print(strokes[NELEM(strokes)-1].length());
      Serial.print(" separation=");
	uint32_t separation =
	  strokes[NELEM(strokes)-1].start_millis -
	  strokes[NELEM(strokes)-2].end_millis;
      Serial.println(separation);
    }
    if ((strokes[NELEM(strokes)-1].type == TWIST_LEFT &&
	 strokes[NELEM(strokes)-2].type == TWIST_RIGHT) ||
	(strokes[NELEM(strokes)-1].type == TWIST_RIGHT &&
	 strokes[NELEM(strokes)-2].type == TWIST_LEFT)) {
      if (strokes[NELEM(strokes) -1].length() > 90UL &&
	  strokes[NELEM(strokes) -1].length() < 300UL &&
	  strokes[NELEM(strokes) -2].length() > 90UL &&
	  strokes[NELEM(strokes) -2].length() < 300UL) {
	uint32_t separation =
	  strokes[NELEM(strokes)-1].start_millis -
	  strokes[NELEM(strokes)-2].end_millis;
	if (separation < 200UL) {
	  Serial.println("TWIST");
	  // We have a twisting gesture.
#if NUM_BUTTONS == 0
	  if (on_) Off(); else On();
#endif
	}
      }
    }
  }

  void DoGesture(StrokeType gesture) {
    if (gesture == NO_STROKE) {
      if (strokes[NELEM(strokes) - 1].end_millis == 0) {
	strokes[NELEM(strokes) - 1].end_millis = millis();
	ProcessStrokes();
      }
      return;
    }
    if (gesture == strokes[NELEM(strokes)-1].type &&
	strokes[NELEM(strokes)-1].end_millis == 0) {
      // Stroke not done, wait.
      return;
    }
    for (size_t i = 0; i < NELEM(strokes) - 1; i++) {
      strokes[i] = strokes[i+1];
    }
    strokes[NELEM(strokes)-1].type = gesture;
    strokes[NELEM(strokes)-1].start_millis = millis();
    strokes[NELEM(strokes)-1].end_millis = 0;
  }

  void SB_Motion(const Vec3& gyro) override {
    if (monitor.ShouldPrint(Monitoring::MonitorGyro)) {
      // Got gyro data
      Serial.print("GYRO: ");
      Serial.print(gyro.x);
      Serial.print(", ");
      Serial.print(gyro.y);
      Serial.print(", ");
      Serial.println(gyro.z);
    }
    if (abs(gyro.x) > 200.0 &&
	abs(gyro.x) > 3.0 * abs(gyro.y) &&
	abs(gyro.x) > 3.0 * abs(gyro.z)) {
      DoGesture(gyro.x > 0 ? TWIST_LEFT : TWIST_RIGHT);
    } else {
      DoGesture(NO_STROKE);
    }
  }
protected:
  Vec3 accel_;
#ifdef ENABLE_AUDIO
  BufferedWavPlayer* track_player_ = NULL;
#endif

  void StartOrStopTrack() {
#ifdef ENABLE_AUDIO
    if (track_player_) {
      track_player_->Stop();
      track_player_ = NULL;
    } else {
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      track_player_ = GetFreeWavPlayer();
      if (track_player_) {
        track_player_->Play(current_preset_->track);
      } else {
        Serial.println("No available WAV players.");
      }
    }
#else
    Serial.println("Audio disabled.");
#endif
  }

  bool aux_on_ = true;
  uint32_t last_beep_;

  void Loop() override {
    if (battery_monitor.low()) {
      if (current_preset_->style != &style_charging) {
        if (on_) {
          Serial.print("Battery low, turning off: ");
          Serial.println(battery_monitor.battery());
          Off();
        } else if (millis() - last_beep_ > 1000) {
          Serial.println("Battery low beep");
#ifdef ENABLE_AUDIO
          beeper.Beep(0.5, 440.0);
#endif
        }
      }
      last_beep_ = millis();
    }
#ifdef ENABLE_AUDIO
    if (track_player_ && !track_player_->isPlaying()) {
      track_player_ = NULL;
    }
#endif
    if (!on_) {
      switch (power_.GetClick()) {
         case ButtonBase::CLICK_NONE:
           break;
         case ButtonBase::CLICK_SHORT:
           if (aux_.pushed_millis()) {
             aux_.EatClick();
             next_preset();
             Serial.println("Next preset");
           } else {
             Serial.println("On (power)");
             On();
             // script.Run();
             aux_on_ = false;
           }
           break;
        case ButtonBase::CLICK_LONG:
          // TODO: Change this to something else...
          if (aux_.pushed_millis()) {
            aux_.EatClick();
            next_directory();
            Serial.println("next directory");
          } else {
            StartOrStopTrack();
          }
          break;
      }

      switch (aux_.GetClick()) {
        case ButtonBase::CLICK_SHORT:
          aux_on_ = true;
          On();
          Serial.println("On (aux)");
          break;
          
        case ButtonBase::CLICK_LONG: // reserved
        case ButtonBase::CLICK_NONE:
          break;
      }
    } else {
      ButtonBase *a, *b;
      if (aux_on_) {
        a = &aux_;
        b = &power_;
      } else {
        b = &aux_;
        a = &power_;
      }
      if (SaberBase::Lockup() && !a->pushed_millis()) {
        SaberBase::SetLockup(false);
        SaberBase::DoEndLockup();
      }
      switch (a->GetClick()) {
        case ButtonBase::CLICK_SHORT:
          Serial.println("Off");
          Off();
          break;
        case ButtonBase::CLICK_LONG:
          Serial.println("Force");
          SaberBase::DoForce();
          break;
        case ButtonBase::CLICK_NONE:
          break;
      }

      switch (b->GetClick()) {
        case ButtonBase::CLICK_SHORT:
          SaberBase::DoBlast();
          Serial.println("Blast");
          break;
        case ButtonBase::CLICK_LONG: // reserved
        case ButtonBase::CLICK_NONE:
          break;
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
#ifdef ENABLE_AUDIO
    if (!strcmp(cmd, "beep")) {
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      beeper.Beep(1.0, 3000.0);
      return true;
    }
    if (!strcmp(cmd, "play")) {
      if (!arg) {
        StartOrStopTrack();
        return true;
      }
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      BufferedWavPlayer* player = GetFreeWavPlayer();
      if (player) {
        Serial.print("Playing ");
        Serial.println(arg);
        player->Play(arg);
      } else {
        Serial.println("No available WAV players.");
      }
      return true;
    }
    if (!strcmp(cmd, "volumes")) {
      for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
        Serial.print(" Unit ");
        Serial.print(unit);
        Serial.print(" Volume ");
        Serial.println(wav_players[unit].volume());
      }
      Serial.print("Splicer Volume ");
      Serial.println(audio_splicer.volume());
      return true;
    }
    if (!strcmp(cmd, "buffered")) {
      for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
        Serial.print(" Unit ");
        Serial.print(unit);
        Serial.print(" Buffered: ");
        Serial.println(wav_players[unit].buffered());
      }
      return true;
    }
#endif
    if (!strcmp(cmd, "cd")) {
      chdir(arg);
      SaberBase::DoNewFont();
      return true;
    }
    if (!strcmp(cmd, "mkdir")) {
      SD.mkdir(arg);
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
    if (!strcmp(cmd, "n") || (!strcmp(cmd, "next") && arg && (!strcmp(arg, "preset") || !strcmp(arg, "pre")))) {
      next_preset();
      return true;
    }
    if (!strcmp(cmd, "prev") && arg && (!strcmp(arg, "preset") || !strcmp(arg, "pre"))) {
      previous_preset();
      return true;
    }
    return false;
  }
  void Help() override {
    Serial.println(" clash - trigger a clash");
    Serial.println(" on/off - turn saber on/off");
#ifdef ENABLE_AUDIO
    Serial.println(" pwd - print current directory");
    Serial.println(" cd directory - change directory, and sound font");
    Serial.println(" play filename - play file");
    Serial.println(" next/prev font - walk through directories in alphabetical order");
    Serial.println(" next/prev pre[set] - walk through presets.");
    Serial.println(" beep - play a beep");
#endif
  }
  
private:
  BladeConfig* current_config_ = NULL;
  Preset* current_preset_ = NULL;

  bool on_;  // <- move to SaberBase
#ifdef POWER_TOUCHBUTTON
  TouchButton power_;
#else
  Button power_;
#endif
  Button aux_;
  Button aux2_;
};

Saber saber;

// Command-line parser. Easiest way to use it is to start the arduino
// serial monitor.
class Parser : Looper, StateMachine {
public:
  Parser() : Looper(), len_(0) {}

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
      // Serial.println("  red, green, blue, yellow, cyan, magenta, white");
#ifdef ENABLE_SERIALFLASH
      Serial.println("Serial Flash memory management:");
      Serial.println("   ls, rm <file>, format, play <file>, effects");
      Serial.println("To upload files: tar cf - files | uuencode x >/dev/ttyACM0");
#endif
      Serial.println(" version - show software version");
      Serial.println(" reset - restart software");
      Serial.println(" effects - list current effects");
      CommandParser::DoHelp();
      return;
    }

    if (!strcmp(cmd, "end")) {
      // End command ignored.
      return;
    }
#ifdef ENABLE_SERIALFLASH
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
    if (!strcmp(cmd, "rm")) {
      if (SerialFlashChip::remove(e)) {
        Serial.println("Removed.\n");
      } else {
        Serial.println("No such file.\n");
      }
      return;
    }
    if (!strcmp(cmd, "format")) {
      Serial.print("Erasing ... ");
      SerialFlashChip::eraseAll();
      while (!SerialFlashChip::ready());
      Serial.println("Done");
      return;
    }
#endif
#ifdef ENABLE_SD
    if (!strcmp(cmd, "dir")) {
      LOCK_SD(true);
      File dir = SD.open(e ? e : current_directory);
      while (File f = dir.openNextFile()) {
        Serial.print(f.name());
        Serial.print(" ");
        Serial.println(f.size());
        f.close();
      }
      LOCK_SD(false);
      Serial.println("Done listing files.");
      return;
    }
    if (!strcmp(cmd, "readalot")) {
      char tmp[10];
      LOCK_SD(true);
      File f = SD.open(e);
      for (int i = 0; i < 10000; i++) {
        f.seek(0);
        f.read(tmp, 10);
        f.seek(1000);
        f.read(tmp, 10);
      }
      LOCK_SD(true);
      Serial.println("Done");
      return;
    }
#endif
#if defined(ENABLE_SD) && defined(ENABLE_SERIALFLASH)
    if (!strcmp(cmd, "cache")) {
      LOCK_SD(true);
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
      LOCK_SD(false);
      Serial.println("Cached!");
      return;
    }
#endif
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
#ifdef ENABLE_AUDIO
#if 0
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
#endif
    if (!strcmp(cmd, "dumpwav")) {
      int16_t tmp[32];
      wav_players[0].Stop();
      wav_players[0].read(tmp, NELEM(tmp));
      wav_players[0].Play(e);
      for (int j = 0; j < 64; j++) {
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
#endif
    if (!strcmp(cmd, "twiddle")) {
      int pin = strtol(e, NULL, 0);
      Serial.print("twiddling ");
      Serial.println(pin);
      pinMode(pin, OUTPUT);
      for (int i = 0; i < 1000; i++) {
         digitalWrite(pin, HIGH);
         delay(10);
         digitalWrite(pin, LOW);
         delay(10);
      }
      Serial.println("done");
      return;
    }
    if (!strcmp(cmd, "twiddle2")) {
      int pin = strtol(e, NULL, 0);
      Serial.print("twiddling ");
      Serial.println(pin);
      pinMode(pin, OUTPUT);
      for (int i = 0; i < 1000; i++) {
         for (int i = 0; i < 500; i++) {
           digitalWrite(pin, HIGH);
           delayMicroseconds(1);
           digitalWrite(pin, LOW);
           delayMicroseconds(1);
         }
         delay(10);
      }
      Serial.println("done");
      return;
    }
    if (!strcmp(cmd, "malloc")) {
      Serial.print("alloced: ");
      Serial.println(mallinfo().uordblks);
      Serial.print("Free: ");
      Serial.println(mallinfo().fordblks);
      return;
    }
    if (!strcmp(cmd, "top")) {
      // TODO: list cpu usage for various objects.
      double total_cycles =
        (double)(audio_dma_interrupt_cycles +
         wav_interrupt_cycles +
         loop_cycles);
      Serial.print("Audio DMA: ");
      Serial.print(audio_dma_interrupt_cycles * 100.0 / total_cycles);
      Serial.println("%");
      Serial.print("Wav reading: ");
      Serial.print(wav_interrupt_cycles * 100.0 / total_cycles);
      Serial.println("%");
      Serial.print("LOOP: ");
      Serial.print(loop_cycles * 100.0 / total_cycles);
      Serial.println("%");
      Serial.print("Global loops / second: ");
      global_loop_counter.Print();
      Serial.println("");
      SaberBase::DoTop();
      noInterrupts();
      audio_dma_interrupt_cycles = 0;
      wav_interrupt_cycles = 0;
      loop_cycles = 0;
      interrupts();
      return;
    }
    if (!strcmp(cmd, "version")) {
      Serial.println(version);
      return;
    }
    if (!strcmp(cmd, "reset")) {
      SCB_AIRCR = 0x05FA0004;
      Serial.println("Reset failed.");
      return;
    }
    if (CommandParser::DoParse(cmd, e)) {
      return;
    }
    Serial.print("Whut? :");
    Serial.println(cmd);
  }

private:
  int len_;
  char cmd_[256];
};

Parser parser;

#ifdef ENABLE_MOTION

#define I2C_TIMEOUT_MILLIS 300

class I2CBus : Looper, StateMachine {
public:
  void Loop() {
    STATE_MACHINE_BEGIN();
    SLEEP(1000);

    // Check that we have pullups.
    while (true) {
      pinMode(i2cDataPin, INPUT_PULLDOWN);
      pinMode(i2cClockPin, INPUT_PULLDOWN);
      delayMicroseconds(10);
      data_detected = analogRead(i2cDataPin) > 800;
      clock_detected = analogRead(i2cClockPin) > 800;
      pinMode(i2cDataPin, INPUT);
      pinMode(i2cClockPin, INPUT);
      if (data_detected && clock_detected) {
        // All good, proceed.
        break;
      }
      if (clock_detected && !data_detected) {
        Serial.println("I2C pending data detected, trying to clear...");
        pinMode(i2cClockPin, OUTPUT);
        for (i = 0; i < 100; i++) {
          SLEEP_MICROS(1);
          digitalWrite(i2cClockPin, HIGH);
          SLEEP_MICROS(1);
          digitalWrite(i2cClockPin, LOW);
        }
        pinMode(i2cClockPin, INPUT);
        SLEEP(100); // Try again soon
      } else {
        SLEEP(1000); // Try again later
      }
    }

    Serial.println("I2C pullups found, initializing...");

    Wire.begin();
    Wire.setClock(400000);
    Wire.setDefaultTimeout(I2C_TIMEOUT_MILLIS * 1000);
    i2c_detected_ = true;
    Looper::Unlink();
    STATE_MACHINE_END();
  }

  bool inited() const { return i2c_detected_; };

private:
  int i;
  bool clock_detected, data_detected;
  bool i2c_detected_ = false;
};

I2CBus i2cbus;

class I2CDevice {
public:
  explicit I2CDevice(uint8_t address) : address_(address) {}
  void writeByte(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(address_);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
  }
  int readByte(uint8_t reg) {
    Wire.beginTransmission(address_);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(address_, (uint8_t) 1);
    if (Wire.available() < 1) {
      uint32_t start = millis();
      while (Wire.available() < 1) {
        if (millis() - start > I2C_TIMEOUT_MILLIS) return -1;
      }
    }
    return Wire.read();
  }
  int readBytes(uint8_t reg, uint8_t* data, int bytes) {
    Wire.beginTransmission(address_);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(address_, (uint8_t) bytes);
    if (Wire.available() < bytes) {
      uint32_t start = millis();
      while (Wire.available() < bytes) {
        if (millis() - start > I2C_TIMEOUT_MILLIS) return -1;
      }
    }
    for (int i = 0; i < bytes; i++) {
      data[i] = Wire.read();
    }           
    return bytes;
  }
private:
  uint8_t address_;
};

#ifdef V2
class LSM6DS3H : public I2CDevice, Looper, StateMachine {
public:
  enum Registers {
    FUNC_CFG_ACCESS = 0x1,
    SENSOR_SYNC_TIME_FRAME = 0x4,
    FIFO_CONTROL1 = 0x6,
    FIFO_CONTROL2 = 0x7,
    FIFO_CONTROL3 = 0x8,
    FIFO_CONTROL4 = 0x9,
    FIFO_CONTROL5 = 0xA,
    ORIENT_CFG_G = 0xB,
    INT1_CTRL = 0xD,
    INT2_CTRL = 0xE,
    WHO_AM_I = 0xF,
    CTRL1_XL = 0x10,
    CTRL2_G = 0x11,
    CTRL3_C = 0x12,
    CTRL4_C = 0x13,
    CTRL5_C = 0x14,
    CTRL6_C = 0x15,
    CTRL7_G = 0x16,
    CTRL8_XL = 0x17,
    CTRL9_XL = 0x18,
    CTRL10_C = 0x19,
    MASTER_CONFIG = 0x1A,
    WAKE_UP_SRC = 0x1B,
    TAP_SRC = 0x1C,
    D6D_SRC = 0x1D,
    STATUS_REG = 0x1E,
    STATUS_SPIAux = 0x1E,
    OUT_TEMP_L = 0x20,
    OUT_TEMP_H = 0x21,
    OUTX_L_G = 0x22,
    OUTX_H_G = 0x23,
    OUTY_L_G = 0x24,
    OUTY_H_G = 0x25,
    OUTZ_L_G = 0x26,
    OUTZ_H_G = 0x27,
    OUTX_L_XL = 0x28,
    OUTX_H_XL = 0x29,
    OUTY_L_XL = 0x2A,
    OUTY_H_XL = 0x2B,
    OUTZ_L_XL = 0x2C,
    OUTZ_H_XL = 0x2D,
    SENSORHUB1_REG = 0x2E,
    SENSORHUB2_REG = 0x2F,
    SENSORHUB3_REG = 0x30,
    SENSORHUB4_REG = 0x31,
    SENSORHUB5_REG = 0x32,
    SENSORHUB6_REG = 0x33,
    SENSORHUB7_REG = 0x34,
    SENSORHUB8_REG = 0x35,
    SENSORHUB9_REG = 0x36,
    SENSORHUB10_REG = 0x37,
    SENSORHUB11_REG = 0x38,
    SENSORHUB12_REG = 0x39,
    FIFO_STATUS1 = 0x3A,
    FIFO_STATUS2 = 0x3B,
    FIFO_STATUS3 = 0x3C,
    FIFO_STATUS4 = 0x3D,
    FIFO_DATA_OUT_L = 0x3E,
    FIFO_DATA_OUT_H = 0x3F,
    TIMESTAMP0_REG = 0x40,
    TIMESTAMP1_REG = 0x40,
    TIMESTAMP2_REG = 0x41,
    STEP_TIMESTAMP_L = 0x49,
    STEP_TIMESTAMP_H = 0x4A,
    STEP_COUNTER_L = 0x4B,
    STEP_COUNTER_H = 0x4C,
    SENSORHUB13_REG = 0x4D,
    SENSORHUB14_REG = 0x4E,
    SENSORHUB15_REG = 0x4F,
    SENSORHUB16_REG = 0x50,
    SENSORHUB17_REG = 0x51,
    SENSORHUB18_REG = 0x52,
    FUNC_SRC = 0x53,
    TAP_CFG = 0x58,
    TAP_THS_6D = 0x59,
    INT_DUR2 = 0x5A,
    WAKE_UP_THS = 0x5B,
    WAKE_UP_DUR = 0x5C,
    FREE_FALL = 0x5D,
    MD1_CFG = 0x5E,
    MD2_CFG = 0x5F,
    OUT_MAG_RAW_X_L = 0x66,
    OUT_MAG_RAW_X_H = 0x67,
    OUT_MAG_RAW_Y_L = 0x68,
    OUT_MAG_RAW_Y_H = 0x69,
    OUT_MAG_RAW_Z_L = 0x6A,
    OUT_MAG_RAW_Z_H = 0x6B,
    CTRL_SPIAux = 0x70
  };

  LSM6DS3H() : I2CDevice(106) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();

    while (1) {
      unsigned char databuffer[6];

      Serial.print("Motion setup ... ");

      writeByte(CTRL1_XL, 0x88);  // 1.66kHz accel, 4G range
      writeByte(CTRL2_G, 0x8C);   // 1.66kHz gyro, 2000 dps
      writeByte(CTRL3_C, 0x44);   // ?
      writeByte(CTRL4_C, 0x00);
      writeByte(CTRL5_C, 0x00);
      writeByte(CTRL6_C, 0x00);
      writeByte(CTRL7_G, 0x00);
      writeByte(CTRL8_XL, 0x00);
      writeByte(CTRL9_XL, 0x38);  // accel xyz enable
      writeByte(CTRL10_C, 0x38);  // gyro xyz enable
      if (readByte(WHO_AM_I) == 105) {
        Serial.println("done.");
      } else {
        Serial.println("failed.");
      }

      while (1) {
        YIELD();
        int status_reg = readByte(STATUS_REG);
        if (status_reg == -1) {
          // motion fail, reboot motion chip.
          Serial.println("Motion chip timeout, reboot motion chip!");
          writeByte(CTRL3_C, 1);
          delay(20);
          break;
        }
        if (status_reg & 0x1) {
          // Temp data available
          int16_t temp_data;
          if (readBytes(OUT_TEMP_L, (uint8_t*)&temp_data, 2) == 2) {
            float temp = 25.0f + temp_data * (1.0f / 16.0f);
            if (monitor.ShouldPrint(Monitoring::MonitorTemp)) {
              Serial.print("TEMP: ");
              Serial.println(temp);
            }
          }
        }
        if (status_reg & 0x2) {
          // gyroscope data available
          if (readBytes(OUTX_L_G, databuffer, 6) == 6) {
            SaberBase::DoMotion(
              Vec3::LSB(databuffer, 2000.0 / 32768.0)); // 2000 dps
          }
        }
        if (status_reg & 0x4) {
          // accel data available
          if (readBytes(OUTX_L_XL, databuffer, 6) == 6) {
            SaberBase::DoAccel(
              Vec3::LSB(databuffer, 4.0 / 32768.0));  // 4 g range
          }
        }
      }
    }
    STATE_MACHINE_END();
  }
};

LSM6DS3H motion;

#else  // V2

class FXOS8700 : public I2CDevice, Looper, StateMachine {
public:
  enum Registers {
    STATUS              = 0x00, // Real-time data-ready status or FIFO status
    OUT_X_MSB           = 0x01, // 8 MSBs of 14-bit sample / Root pointer to XYZ FIFO
    OUT_X_LSB           = 0x02, // 6 LSBs of 14-bit real-time sample
    OUT_Y_MSB           = 0x03, // 8 MSBs of 14-bit real-time sample
    OUT_Y_LSB           = 0x04, // 6 LSBs of 14-bit real-time sample
    OUT_Z_MSB           = 0x05, // 8 MSBs of 14-bit real-time sample
    OUT_Z_LSB           = 0x06, // 6 LSBs of 14-bit real-time sample
    F_SETUP             = 0x09, // FIFO setup
    TRIG_CFG            = 0x0A, // FIFO event trigger configuration register
    SYSMOD              = 0x0B, // Current system mode
    INT_SOURCE          = 0x0C, // Interrupt status
    WHO_AM_I            = 0x0D, // Device ID
    XYZ_DATA_CFG        = 0x0E, // Acceleration dynamic range and filter enable settings
    P_FILTER_CUTOFF     = 0x0F, // Pulse detection high- pass and low-pass filter enable bits. High-pass filter cutoff frequency selection
    PL_STATUS           = 0x10, // Landscape/Portrait orientation status
    PL_CFG              = 0x11, // Landscape/Portrait configuration
    PL_COUNT            = 0x12, // Landscape/Portrait debounce counter
    PL_BF_ZCOMP         = 0x13, // Back/Front Trip angle threshold
    PL_THS_REG          = 0x14, // Portrait to Landscape Trip Threshold angle and hysteresis settings
    A_FFMT_CFG          = 0x15, // Freefall/Motion function configuration
    A_FFMT_SRC          = 0x16, // Freefall/Motion event source register
    A_FFMT_THS          = 0x17, // Freefall/Motion threshold register
    A_FFMT_COUNT        = 0x18, // Freefall/Motion debounce counter
    TRANSIENT_CFG       = 0x1D, // FIFO setup
    TRANSIENT_SRC       = 0x1E, // Transient event status register
    TRANSIENT_THS       = 0x1F, // Transient event threshold
    TRANSIENT_COUNT     = 0x20, // Transient debounce counter
    PULSE_CFG           = 0x21, // Pulse function configuration
    PULSE_SRC           = 0x22, // Pulse function source register
    PULSE_THSX          = 0x23, // X-axis pulse threshold
    PULSE_THSY          = 0x24, // Y-axis pulse threshold
    PULSE_THSZ          = 0x25, // Z-axis pulse threshold
    PULSE_TMLT          = 0x26, // Time limit for pulse detection
    PULSE_LTCY          = 0x27, // Latency time for second pulse detection
    PULSE_WIND          = 0x28, // Window time for second pulse detection
    ASLP_COUNT          = 0x29, // In activity counter setting for Auto-Sleep
    CTRL_REG1           = 0x2A, // System ODR, accelerometer OSR, operating mode
    CTRL_REG2           = 0x2B, // Self-Test, Reset, accelerometer OSR and Sleep mode settings
    CTRL_REG3           = 0x2C, // Sleep mode interrupt wake enable, interrupt polarity, push-pull/open-drain configuration
    CTRL_REG4           = 0x2D, // Interrupt enable register
    CTRL_REG5           = 0x2E, // Interrupt pin (INT1/INT2) map
    OFF_X               = 0x2F, // X-axis accelerometer offset adjust
    OFF_Y               = 0x30, // Y-axis accelerometer offset adjust
    OFF_Z               = 0x31, // Z-axis accelerometer offset adjust
    M_DR_STATUS         = 0x32, // Magnetic data ready
    M_OUT_X_MSB         = 0x33, // MSB of 16-bit magnetic data for X-axis
    M_OUT_X_LSB         = 0x34, // LSB of 16-bit magnetic data for X-axis
    M_OUT_Y_MSB         = 0x35, // MSB of 16-bit magnetic data for Y-axis
    M_OUT_Y_LSB         = 0x36, // LSB of 16-bit magnetic data for Y-axis
    M_OUT_Z_MSB         = 0x37, // MSB of 16-bit magnetic data for Z-axis
    M_OUT_Z_LSB         = 0x38, // LSB of 16-bit magnetic data for Z-axis
    CMP_X_MSB           = 0x39, // Bits [13:8] of integrated X-axis accerleration data
    CMP_X_LSB           = 0x3A, // Bits [7:0] of integrated X-axis accerleration data
    CMP_Y_MSB           = 0x3B, // Bits [13:8] of integrated Y-axis accerleration data
    CMP_Y_LSB           = 0x3C, // Bits [7:0] of integrated Y-axis accerleration data
    CMP_Z_MSB           = 0x3D, // Bits [13:8] of integrated Z-axis accerleration data
    CMP_Z_LSB           = 0x3E, // Bits [7:0] of integrated Z-axis accerleration data
    M_OFF_X_MSB         = 0x3F, // MSB of magnetometer of X-axis offset
    M_OFF_X_LSB         = 0x40, // LSB of magnetometer of X-axis offset
    M_OFF_Y_MSB         = 0x41, // MSB of magnetometer of Y-axis offset
    M_OFF_Y_LSB         = 0x42, // LSB of magnetometer of Y-axis offset
    M_OFF_Z_MSB         = 0x43, // MSB of magnetometer of Z-axis offset
    M_OFF_Z_LSB         = 0x44, // LSB of magnetometer of Z-axis offset
    MAX_X_MSB           = 0x45, // Magnetometer X-axis maximum value MSB
    MAX_X_LSB           = 0x46, // Magnetometer X-axis maximum value LSB
    MAX_Y_MSB           = 0x47, // Magnetometer Y-axis maximum value MSB
    MAX_Y_LSB           = 0x48, // Magnetometer Y-axis maximum value LSB
    MAX_Z_MSB           = 0x49, // Magnetometer Z-axis maximum value MSB
    MAX_Z_LSB           = 0x4A, // Magnetometer Z-axis maximum value LSB
    MIN_X_MSB           = 0x4B, // Magnetometer X-axis minimum value MSB
    MIN_X_LSB           = 0x4C, // Magnetometer X-axis minimum value LSB
    MIN_Y_MSB           = 0x4D, // Magnetometer Y-axis minimum value MSB
    MIN_Y_LSB           = 0x4E, // Magnetometer Y-axis minimum value LSB
    MIN_Z_MSB           = 0x4F, // Magnetometer Z-axis minimum value MSB
    MIN_Z_LSB           = 0x50, // Magnetometer Z-axis minimum value LSB
    TEMP                = 0x51, // Device temperature, valid range of -128 to 127 C when M_CTRL1[m_hms] > 0b00
    M_THS_CFG           = 0x52, // Magnetic threshold detection function configuration
    M_THS_SRC           = 0x53, // Magnetic threshold event source register
    M_THS_X_MSB         = 0x54, // X-axis magnetic threshold MSB
    M_THS_X_LSB         = 0x55, // X-axis magnetic threshold LSB
    M_THS_Y_MSB         = 0x56, // Y-axis magnetic threshold MSB
    M_THS_Y_LSB         = 0x57, // Y-axis magnetic threshold LSB
    M_THS_Z_MSB         = 0x58, // Z-axis magnetic threshold MSB
    M_THS_Z_LSB         = 0x59, // Z-axis magnetic threshold LSB
    M_THS_COUNT         = 0x5A, // Magnetic threshold debounce counter
    M_CTRL_REG1         = 0x5B, // Control for magnetic sensor functions
    M_CTRL_REG2         = 0x5C, // Control for magnetic sensor functions
    M_CTRL_REG3         = 0x5D, // Control for magnetic sensor functions
    M_INT_SRC           = 0x5E, // Magnetic interrupt source
    A_VECM_CFG          = 0x5F, // Acceleration vector-magnitude configuration register
    A_VECM_THS_MSB      = 0x60, // Acceleration vector-magnitude threshold MSB
    A_VECM_THS_LSB      = 0x61, // Acceleration vector-magnitude threshold LSB
    A_VECM_CNT          = 0x62, // Acceleration vector-magnitude debounce count
    A_VECM_INITX_MSB    = 0x63, // Acceleration vector-magnitude X-axis reference value MSB
    A_VECM_INITX_LSB    = 0x64, // Acceleration vector-magnitude X-axis reference value LSB
    A_VECM_INITY_MSB    = 0x65, // Acceleration vector-magnitude Y-axis reference value MSB
    A_VECM_INITY_LSB    = 0x66, // Acceleration vector-magnitude Y-axis reference value LSB
    A_VECM_INITZ_MSB    = 0x67, // Acceleration vector-magnitude Z-axis reference value MSB
    A_VECM_INITZ_LSB    = 0x68, // Acceleration vector-magnitude Z-axis reference value LSB
    M_VECM_CFG          = 0x69, // Magnetic vector-magnitude configuration register
    M_VECM_THS_MSB      = 0x6A, // Magnetic vector-magnitude threshold MSB
    M_VECM_THS_LSB      = 0x6B, // Magnetic vector-magnitude threshold LSB
    M_VECM_CNT          = 0x6C, // Magnetic vector-magnitude debounce count
    M_VECM_INITX_MSB    = 0x6D, // Magnetic vector-magnitude reference value X-axis MSB
    M_VECM_INITX_LSB    = 0x6E, // Magnetic vector-magnitude reference value X-axis LSB
    M_VECM_INITY_MSB    = 0x6F, // Magnetic vector-magnitude reference value Y-axis MSB
    M_VECM_INITY_LSB    = 0x70, // Magnetic vector-magnitude reference value Y-axis LSB
    M_VECM_INITZ_MSB    = 0x71, // Magnetic vector-magnitude reference value Z-axis MSB
    M_VECM_INITZ_LSB    = 0x72, // Magnetic vector-magnitude reference value Z-axis LSB
    A_FFMT_THS_X_MSB    = 0x73, // X-axis FMT threshold MSB
    A_FFMT_THS_X_LSB    = 0x74, // X-axis FFMT threshold LSB
    A_FFMT_THS_Y_MSB    = 0x75, // Y-axis FFMT threshold MSB
    A_FFMT_THS_Y_LSB    = 0x76, // Y-axis FFMT threshold LSB
    A_FFMT_THS_Z_MSB    = 0x77, // Z-axis FFMT threshold MSB
    A_FFMT_THS_Z_LSB    = 0x78, // Z-axis FFMT threshold LSB
  };

  FXOS8700() : I2CDevice(0x1E) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();

    while (1) {
      unsigned char databuffer[6];
      Serial.print("Accel setup ... ");

      if (readByte(WHO_AM_I) != 0xC7) {
        Serial.print("Failed.");
        SLEEP(1000);
        continue;
      }

      // Standby
      writeByte(CTRL_REG1, 0);
      // configure magnetometer
      // writeByte(M_CTRL_REG1, 0x1F);
      // writeByte(M_CTRL_REG2, 0x20);
      writeByte(M_CTRL_REG2, 0x00); // Disable magnetometer
      // configure accelerometer
      writeByte(XYZ_DATA_CFG, 0x01);  // 4G range
      writeByte(CTRL_REG2, 0x02);  // hires
      // writeByte(CTRL_REG1, 0x15);  // 100Hz A+M 
      writeByte(CTRL_REG1, 0x01);  // 800Hz Accel only

      Serial.println(" Done");

      while (1) {
        YIELD();
        int status = readByte(STATUS);
        if (status == -1) {
          // motion fail, reboot gyro chip.
          Serial.println("Motion chip timeout, reboot motion chip!");
          // writeByte(CTRL3_C, 1);
          delay(20);
          break;
        }
        if (status) {
          // gyroscope data available
          if (readBytes(OUT_X_MSB, databuffer, 6) == 6) {
            SaberBase::DoAccel(
              Vec3::MSB(databuffer, 4.0 / 32768.0)); // 4 g range
          }
        }
      }
    }
    STATE_MACHINE_END();
  }
};

class FXAS21002 : public I2CDevice, Looper, StateMachine {
public:
  enum Registers {
    I2C_ADDR0         = 0x20, // SA0 = Gnd
    I2C_ADDR1         = 0x21, // SA0 = Vcc
    STATUS            = 0x00, // Alias for DR_STATUS or F_STATUS
    OUT_X_MSB         = 0x01, // MSB of 16 bit X-axis data sample
    OUT_X_LSB         = 0x02, // LSB of 16 bit X-axis data sample
    OUT_Y_MSB         =0x03 ,// MSB of 16 bit Y-axis data sample
    OUT_Y_LSB         = 0x04, // LSB of 16 bit Y-axis data sample
    OUT_Z_MSB         = 0x05, // MSB of 16 bit Z-axis data sample
    OUT_Z_LSB         = 0x06, // LSB of 16 bit Z-axis data sample
    DR_STATUS         = 0x07, // Data-ready status information
    F_STATUS          = 0x08, // FIFO Status
    F_SETUP           = 0x09, // FIFO setup
    F_EVENT           = 0x0A, // FIFO event
    INT_SRC_FLAG      = 0x0B, // Interrupt event source status flags
    WHO_AM_I          = 0x0C, // Device ID
    CTRL_REG0         = 0x0D, // Full-scale range selection, high-pass filter setting, SPI mode selection
    RT_CFG            = 0x0E, // Rate threshold function configuration
    RT_SRC            = 0x0F, // Rate threshold event flags status register
    RT_THS            = 0x10, // Rate threshold function threshold register
    RT_COUNT          = 0x11, // Rate threshold function debounce counter
    TEMP              = 0x12, // Device temperature in C
    CTRL_REG1         = 0x13, // Operating mode, ODR, self-test and soft reset
    CTRL_REG2         = 0x14, // Interrupt configuration
    CTRL_REG3         = 0x15, // Auto-increment address configuration, external power control, FSR expansion
  };

  FXAS21002() : I2CDevice(0x20) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();
    while (1) {
      unsigned char databuffer[6];

      Serial.print("Gyro setup ... ");

      if (readByte(WHO_AM_I) != 0xD7) {
        Serial.println("Failed.");
        SLEEP(1000);
        return;
      }

      // Standby
      writeByte(CTRL_REG1, 0);
      // switch to active mode, 800 Hz output rate
      writeByte(CTRL_REG0, 0x00);
      writeByte(CTRL_REG1, 0x02);

      Serial.println(" Done");

      while (1) {
        YIELD();
        int status = readByte(STATUS);
        if (status == -1) {
          // motion fail, reboot gyro chip.
          Serial.println("Motion chip timeout, reboot motion chip!");
          // writeByte(CTRL3_C, 1);
          delay(20);
          break;
        }
        if (status) {
          // gyroscope data available
          if (readBytes(OUT_X_MSB, databuffer, 6) == 6) {
            SaberBase::DoMotion(Vec3::MSB(databuffer, 2000.0 / 32768.0));
          }
        }
      }
    }
    STATE_MACHINE_END();
  }
};

FXOS8700 fxos8700;
FXAS21002 fxas21002;

#endif   // V2
#endif   // ENABLE_MOTION


#ifdef ENABLE_AUDIO
// Turns off amplifier when no audio is played.
// Maybe name this IdleHelper or something instead??
class Amplifier : Looper, StateMachine, CommandParser {
public:
  Amplifier() : Looper(), CommandParser() {}

  bool Active() {
//    if (saber_synth.on_) return true;
    if (audio_splicer.isPlaying()) return true;
    if (beeper.isPlaying()) return true;
    for (size_t i = 0; i < NELEM(wav_players); i++)
      if (wav_players[i].isPlaying())
        return true;
    return false;
  }

protected:
  void Setup() override {
    // Audio setup
    delay(50);             // time for DAC voltage stable
    pinMode(amplifierPin, OUTPUT);
    delay(10);
    SetupStandardAudio();
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

  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "amp")) {
      if (!strcmp(arg, "on")) {
        digitalWrite(amplifierPin, HIGH); // turn the amplifier off
        return true;
      }
      if (!strcmp(arg, "off")) {
        digitalWrite(amplifierPin, LOW); // turn the amplifier off
        return true;
      }
    }
    if (!strcmp(cmd, "whatison")) {
      bool on = false;
      SaberBase::DoIsOn(&on);
      Serial.print("Saber bases: ");
      Serial.println(on ? "On" : "Off");
      Serial.print("Audio splicer: ");
      Serial.println(audio_splicer.isPlaying() ? "On" : "Off");
      Serial.print("Beeper: ");
      Serial.println(beeper.isPlaying() ? "On" : "Off");
      for (size_t i = 0; i < NELEM(wav_players); i++) {
        Serial.print("Wav player ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(wav_players[i].isPlaying() ? "On" : "Off");
        Serial.print(" (eof =  ");
        Serial.print(wav_players[i].eof());
        Serial.println(")");
      }
      return true;
    }
    return false;
  }

  void Help() {
    Serial.println(" amp on/off - turn amplifier on or off");
  }
};

Amplifier amplifier;
#endif

void setup() {

  // Enable cycle counting
#if 0
//  pinMode(bladePin, OUTPUT);
  pinMode(bladePowerPin1, OUTPUT);
  pinMode(bladePowerPin2, OUTPUT);
  pinMode(bladePowerPin3, OUTPUT);
//  digitalWrite(bladePin, LOW);
  digitalWrite(bladePowerPin1, LOW);
  digitalWrite(bladePowerPin2, LOW);
  digitalWrite(bladePowerPin3, LOW);
#ifdef V2
  pinMode(bladePowerPin4, OUTPUT);
  pinMode(bladePowerPin5, OUTPUT);
  pinMode(bladePowerPin6, OUTPUT);
  digitalWrite(bladePowerPin4, LOW);
  digitalWrite(bladePowerPin5, LOW);
  digitalWrite(bladePowerPin6, LOW);
#endif
#endif

  // Wait for all voltages to settle.
  // Accumulate some entrypy while we wait.
  uint32_t now = millis();
  while (millis() - now < 1000) {
    srand((rand() * 917823) ^ analogRead(batteryLevelPin));
  }

  Serial.begin(9600);
#ifdef ENABLE_SERIALFLASH
  SerialFlashChip::begin(serialFlashSelectPin);
#endif
#ifdef ENABLE_SD
  if (!SD.begin(sdCardSelectPin)) {
    Serial.println("No sdcard found.");
  } else {
    Serial.println("Sdcard found..");
  }
#endif
  // Time to identify the blade.
  Looper::DoSetup();
  saber.FindBlade();
  SaberBase::DoBoot();
}

#if 0
extern "C" void startup_early_hook(void) {
#ifdef ENABLE_WATCHDOG
  // The next 2 lines sets the time-out value. This is the value that the watchdog timer compares itself to
  WDOG_TOVALL = 1000;
  WDOG_TOVALH = 0;
  WDOG_STCTRLH = (WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_WDOGEN |
                  WDOG_STCTRLH_STOPEN |
                  WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN); // Enable WDG
  WDOG_PRESC = 0; // prescaler
#elif defined(KINETISK)
        WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;
#elif defined(KINETISL)
        SIM_COPC = 0;  // disable the watchdog
#endif
        // enable clocks to always-used peripherals
#if defined(__MK20DX128__)
        SIM_SCGC5 = 0x00043F82;         // clocks active to all GPIO
        SIM_SCGC6 = SIM_SCGC6_RTC | SIM_SCGC6_FTM0 | SIM_SCGC6_FTM1 | SIM_SCGC6_ADC0 | SIM_SCGC6_FTFL;
#elif defined(__MK20DX256__)
        SIM_SCGC3 = SIM_SCGC3_ADC1 | SIM_SCGC3_FTM2;
        SIM_SCGC5 = 0x00043F82;         // clocks active to all GPIO
        SIM_SCGC6 = SIM_SCGC6_RTC | SIM_SCGC6_FTM0 | SIM_SCGC6_FTM1 | SIM_SCGC6_ADC0 | SIM_SCGC6_FTFL;
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
        SIM_SCGC3 = SIM_SCGC3_ADC1 | SIM_SCGC3_FTM2 | SIM_SCGC3_FTM3;
        SIM_SCGC5 = 0x00043F82;         // clocks active to all GPIO
        SIM_SCGC6 = SIM_SCGC6_RTC | SIM_SCGC6_FTM0 | SIM_SCGC6_FTM1 | SIM_SCGC6_ADC0 | SIM_SCGC6_FTFL;
        //PORTC_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_DSE | PORT_PCR_SRE;
        //GPIOC_PDDR |= (1<<5);
        //GPIOC_PSOR = (1<<5);
        //while (1);
#elif defined(__MKL26Z64__)
        SIM_SCGC4 = SIM_SCGC4_USBOTG | 0xF0000030;
        SIM_SCGC5 = 0x00003F82;         // clocks active to all GPIO
        SIM_SCGC6 = SIM_SCGC6_ADC0 | SIM_SCGC6_TPM0 | SIM_SCGC6_TPM1 | SIM_SCGC6_TPM2 | SIM_SCGC6_FTFL;
#endif
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
        SCB_CPACR = 0x00F00000;
#endif
#if defined(__MK66FX1M0__)
        LMEM_PCCCR = 0x85000003;
#endif
        
#define SETUP_PIN(X) do {                                               \
 CORE_PIN##X##_PORTREG &=~ CORE_PIN##X##_BITMASK;                       \
 CORE_PIN##X##_CONFIG = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);  \
 CORE_PIN##X##_DDRREG |= CORE_PIN##X##_BITMASK;                         \
} while (0)

  SETUP_PIN(20);
  SETUP_PIN(21);
  SETUP_PIN(22);
  SETUP_PIN(23);
#ifdef V2
  SETUP_PIN(3);
  SETUP_PIN(4);
  SETUP_PIN(5);
#endif
}
#endif

#ifdef ENABLE_WATCHDOG
class WatchDog : Looper {
  void Loop() override {
    if (watchdogTimer_ > 5) {
      watchdogTimer_ = 0;
      
      noInterrupts();
      WDOG_REFRESH = 0xA602;
      WDOG_REFRESH = 0xB480;
      interrupts();
    }
  };

  elapsedMillis watchdogTimer_;
};

WatchDog dog;
#endif

#ifdef MTP_RX_ENDPOINT

void mtp_yield() { Looper::DoLoop(); }
void mtp_lock_storage(bool lock) {
  AudioStreamWork::LockSD(lock);
}

// This interface lets the MTP responder interface any storage.
// We'll need to give the MTP responder a pointer to one of these.
class MTPStorageInterface {
public:
  // Return true if this storage is read-only
  virtual bool readonly() = 0;
 
  // Does it have directories?
  virtual bool has_directories() = 0;

  // Return size of storage in bytes.
  virtual uint64_t size() = 0;

  // Return free space in bytes.
  virtual uint64_t free() = 0;

  // parent = 0 means get all handles.
  // parent = 0xFFFFFFFF means get root folder.
  virtual void StartGetObjectHandles(uint32_t parent) = 0;
  virtual uint32_t GetNextObjectHandle() = 0;

  // Size should be 0xFFFFFFFF if it's a directory.
  virtual void GetObjectInfo(uint32_t handle,
                             char* name,
                             uint32_t* size,
                             uint32_t* parent) = 0;
  virtual uint32_t GetSize(uint32_t handle) = 0;
  virtual void read(uint32_t handle,
                    uint32_t pos,
                    char* buffer,
                    uint32_t bytes) = 0;
  virtual uint32_t Create(uint32_t parent,
                          bool folder,
                          const char* filename) = 0;
  virtual void write(const char* data, uint32_t size);
  virtual void close();
  virtual bool DeleteObject(uint32_t object) = 0;
};

// Storage implementation for SD. SD needs to be already initialized.
class MTPStorage_SD : public MTPStorageInterface {
private:
  File index_;

  uint8_t mode_ = 0;
  uint32_t open_file_ = 0xFFFFFFFEUL;
  File f_;
  uint32_t index_entries_ = 0;

  struct Record {
    uint32_t parent;
    uint32_t child;  // size stored here for files
    uint32_t sibling;
    uint8_t isdir;
    uint8_t scanned;
    char name[14];
  };

  bool readonly() { return false; }
  bool has_directories() { return true; }
  uint64_t size() { return 1 << 30; }
  uint64_t free() { return 1 << 29; }

  void OpenIndex() {
    if (index_) return;
    mtp_lock_storage(true);
    index_ = SD.open("mtpindex.dat", FILE_WRITE);
    mtp_lock_storage(false);
  }

  void WriteIndexRecord(uint32_t i, const Record& r) {
    OpenIndex();
    mtp_lock_storage(true);
    index_.seek(sizeof(r) * i);
    index_.write((char*)&r, sizeof(r));
    mtp_lock_storage(false);
  }
  
  uint32_t AppendIndexRecord(const Record& r) {
    uint32_t new_record = index_entries_++;
    WriteIndexRecord(new_record, r);
    return new_record;
  }

  // TODO(hubbe): Cache a few records for speed.
  Record ReadIndexRecord(uint32_t i) {
    Record ret;
    if (i > index_entries_) {
      memset(&ret, 0, sizeof(ret));
      return ret;
    }
    OpenIndex();
    mtp_lock_storage(true);
    index_.seek(sizeof(ret) * i);
    index_.read(&ret, sizeof(ret));
    mtp_lock_storage(false);
    return ret;
  }

  void ConstructFilename(int i, char* out) {
    if (i == 0) {
      strcpy(out, "/");
    } else {
      Record tmp = ReadIndexRecord(i);
      ConstructFilename(tmp.parent, out);
      if (out[strlen(out)-1] != '/')
        strcat(out, "/");
      strcat(out, tmp.name);
    }
  }

  void OpenFileByIndex(uint32_t i, uint8_t mode = O_RDONLY) {
    if (open_file_ == i && mode_ == mode)
      return;
    char filename[256];
    ConstructFilename(i, filename);
    mtp_lock_storage(true);
    f_.close();
    f_ = SD.open(filename, mode);
    open_file_ = i;
    mode_ = mode;
    mtp_lock_storage(false);
  }

  // MTP object handles should not change or be re-used during a session.
  // This would be easy if we could just have a list of all files in memory.
  // Since our RAM is limited, we'll keep the index in a file instead.
  bool index_generated = false;
  void GenerateIndex() {
    if (index_generated) return;
    index_generated = true;

    mtp_lock_storage(true);
    SD.remove("mtpindex.dat");
    mtp_lock_storage(false);
    index_entries_ = 0;

    Record r;
    r.parent = 0;
    r.sibling = 0;
    r.child = 0;
    r.isdir = true;
    r.scanned = false;
    strcpy(r.name, "/");
    AppendIndexRecord(r);
  }

  void ScanDir(uint32_t i) {
    Record record = ReadIndexRecord(i);
    if (record.isdir && !record.scanned) {
      OpenFileByIndex(i);
      if (!f_) return;
      int sibling = 0;
      while (true) {
        mtp_lock_storage(true);
        File child = f_.openNextFile();
        mtp_lock_storage(false);
        if (!child) break;
        Record r;
        r.parent = i;
        r.sibling = sibling;
        r.isdir = child.isDirectory();
        r.child = r.isdir ? 0 : child.size();
        r.scanned = false;
        strcpy(r.name, child.name());
        sibling = AppendIndexRecord(r);
        child.close();
      }
      record.scanned = true;
      record.child = sibling;
      WriteIndexRecord(i, record);
    }
  }

  bool all_scanned_ = false;
  void ScanAll() {
    if (all_scanned_) return;
    all_scanned_ = true;

    GenerateIndex();
    for (uint32_t i = 0; i < index_entries_; i++) {
      ScanDir(i);
    }
  }

  uint32_t next_;
  bool follow_sibling_;
  void StartGetObjectHandles(uint32_t parent) override {
    GenerateIndex();
    if (parent) {
      if (parent == 0xFFFFFFFF) parent = 0;
      ScanDir(parent);
      follow_sibling_ = true;
      // Root folder?
      next_ = ReadIndexRecord(parent).child;
    } else {
      ScanAll();
      follow_sibling_ = false;
      next_ = 1;
    }
  }

  uint32_t GetNextObjectHandle() override {
    while (true) {
      if (next_ == 0)
        return 0;
      int ret = next_;
      Record r = ReadIndexRecord(ret);
      if (follow_sibling_) {
        next_ = r.sibling;
      } else {
        next_++;
        if (next_ >= index_entries_)
          next_ = 0;
      }
      if (r.name[0]) return ret;
    }
  }

  void GetObjectInfo(uint32_t handle,
                     char* name,
                     uint32_t* size,
                     uint32_t* parent) override {
    Record r = ReadIndexRecord(handle);
    strcpy(name, r.name);
    *parent = r.parent;
    *size = r.isdir ? 0xFFFFFFFFUL : r.child;
  }

  uint32_t GetSize(uint32_t handle) {
    return ReadIndexRecord(handle).child;
  }

  void read(uint32_t handle,
            uint32_t pos,
            char* out,
            uint32_t bytes) override {
    OpenFileByIndex(handle);
    mtp_lock_storage(true);
    f_.seek(pos);
    f_.read(out, bytes);
    mtp_lock_storage(false);
  }

  bool DeleteObject(uint32_t object) override {
    char filename[256];
    Record r;
    while (true) {
      r = ReadIndexRecord(object == 0xFFFFFFFFUL ? 0 : object);
      if (!r.isdir) break;
      if (!r.child) break;
      if (!DeleteObject(r.child))
        return false;
    }

    // We can't actually delete the root folder,
    // but if we deleted everything else, return true.
    if (object == 0xFFFFFFFFUL) return true;

    ConstructFilename(object, filename);
    bool success;
    mtp_lock_storage(true);
    if (r.isdir) {
      success = SD.rmdir(filename);
    } else {
      success = SD.remove(filename);
    }
    mtp_lock_storage(false);
    if (!success) return false;
    r.name[0] = 0;
    int p = r.parent;
    WriteIndexRecord(object, r);
    Record tmp = ReadIndexRecord(p);
    if (tmp.child == object) {
      tmp.child = r.sibling;
      WriteIndexRecord(p, tmp);
    } else {
      int c = tmp.child;
      while (c) {
        tmp = ReadIndexRecord(c);
        if (tmp.sibling == object) {
          tmp.sibling = r.sibling;
          WriteIndexRecord(c, tmp);
          break;
        } else {
          c = tmp.sibling;
        }
      }
    }
    return true;
  }

  uint32_t Create(uint32_t parent,
                  bool folder,
                  const char* filename) override {
    uint32_t ret;
    if (parent == 0xFFFFFFFFUL) parent = 0;
    Record p = ReadIndexRecord(parent);
    Record r;
    if (strlen(filename) > 12) return 0;
    strcpy(r.name, filename);
    r.parent = parent;
    r.child = 0;
    r.sibling = p.child;
    r.isdir = folder;
    // New folder is empty, scanned = true.
    r.scanned = 1;
    ret = p.child = AppendIndexRecord(r);
    WriteIndexRecord(parent, p);
    if (folder) {
      char filename[256];
      ConstructFilename(ret, filename);
      mtp_lock_storage(true);
      SD.mkdir(filename);
      mtp_lock_storage(false);
    } else {
      OpenFileByIndex(ret, FILE_WRITE);
    }
    return ret;
  }

  void write(const char* data, uint32_t bytes) override {
    mtp_lock_storage(true);
    f_.write(data, bytes);
    mtp_lock_storage(false);
  }

  void close() override {
    mtp_lock_storage(true);
    uint32_t size = f_.size();
    f_.close();
    mtp_lock_storage(false);
    Record r = ReadIndexRecord(open_file_);
    r.child = size;
    WriteIndexRecord(open_file_, r);
    open_file_ = 0xFFFFFFFEUL;
  }
};

// MTP Responder.
class MTPD {
public:
  explicit MTPD(MTPStorageInterface* storage) : storage_(storage) {}

private:
  MTPStorageInterface* storage_;

  struct MTPHeader {
    uint32_t len;  // 0
    uint16_t type; // 4
    uint16_t op;   // 6
    uint32_t transaction_id; // 8
  } __attribute__((__may_alias__));

  struct MTPContainer {
    uint32_t len;  // 0
    uint16_t type; // 4
    uint16_t op;   // 6
    uint32_t transaction_id; // 8
    uint32_t params[5];    // 12
  } __attribute__((__may_alias__));

  void PrintPacket(const usb_packet_t *x) {
#if 0
    for (int i = 0; i < x->len; i++) {
      Serial1.print("0123456789ABCDEF"[x->buf[i] >> 4]);
      Serial1.print("0123456789ABCDEF"[x->buf[i] & 0xf]);
      if ((i & 3) == 3) Serial1.print(" ");
    } 
    Serial1.println("");
#endif
#if 0
    MTPContainer *tmp = (struct MTPContainer*)(x->buf);
    Serial1.print(" len = ");
    Serial1.print(tmp->len, HEX);
    Serial1.print(" type = ");
    Serial1.print(tmp->type, HEX);
    Serial1.print(" op = ");
    Serial1.print(tmp->op, HEX);
    Serial1.print(" transaction_id = ");
    Serial1.print(tmp->transaction_id, HEX);
    for (int i = 0; i * 4 < x->len - 12; i ++) {
      Serial1.print(" p");
      Serial1.print(i);
      Serial1.print(" = ");
      Serial1.print(tmp->params[i], HEX);
    }
    Serial1.println("");
#endif
  }

  usb_packet_t *data_buffer_ = NULL;
  void get_buffer() {
    while (!data_buffer_) {
      data_buffer_ = usb_malloc();
      if (!data_buffer_) mtp_yield();
    }
  }

  void receive_buffer() {
    while (!data_buffer_) {
      data_buffer_ = usb_rx(MTP_RX_ENDPOINT);
      if (!data_buffer_) mtp_yield();
    }
  }

  bool write_get_length_ = false;
  uint32_t write_length_ = 0;
  void write(const char *data, int len) {
    if (write_get_length_) {
      write_length_ += len;
    } else {
      int pos = 0;
      while (pos < len) {
        get_buffer();
        int avail = sizeof(data_buffer_->buf) - data_buffer_->len;
        int to_copy = min(len - pos, avail);
        memcpy(data_buffer_->buf + data_buffer_->len,
               data + pos,
               to_copy);
        data_buffer_->len += to_copy;
        pos += to_copy;
        if (data_buffer_->len == sizeof(data_buffer_->buf)) {
          usb_tx(MTP_TX_ENDPOINT, data_buffer_);
          data_buffer_ = NULL;
          // Serial1.println("SENT...");
        }
      }
    }
  }
  void write8(uint8_t x) { write((char*)&x, sizeof(x)); }
  void write16(uint16_t x) { write((char*)&x, sizeof(x)); }
  void write32(uint32_t x) { write((char*)&x, sizeof(x)); }
  void write64(uint64_t x) { write((char*)&x, sizeof(x)); }
  void writestring(const char* str) {
    if (*str) {
      write8(strlen(str) + 1);
      while (*str) {
        write16(*str);
        ++str;
      }
      write16(0);
    } else {
      write8(0);
    }
  }

  void WriteDescriptor() {
    write16(100);  // MTP version
    write32(6);    // MTP extension
//    write32(0xFFFFFFFFUL);    // MTP extension
    write16(100);  // MTP version
    writestring("microsoft.com: 1.0;");
    write16(0);    // functional mode

    // Supported operations (array of uint16)
    write32(14);
    write16(0x1001);  // GetDeviceInfo
    write16(0x1002);  // OpenSession
    write16(0x1003);  // CloseSession
    write16(0x1004);  // GetStorageIDs

    write16(0x1005);  // GetStorageInfo
    write16(0x1006);  // GetNumObjects
    write16(0x1007);  // GetObjectHandles
    write16(0x1008);  // GetObjectInfo

    write16(0x1009);  // GetObject
    write16(0x100B);  // DeleteObject
    write16(0x100C);  // SendObjectInfo
    write16(0x100D);  // SendObject

    write16(0x1014);  // GetDevicePropDesc
    write16(0x1015);  // GetDevicePropValue

//    write16(0x1010);  // Reset
//    write16(0x1019);  // MoveObject
//    write16(0x101A);  // CopyObject

    write32(0);       // Events (array of uint16)

    write32(1);       // Device properties (array of uint16)
    write16(0xd402);  // Device friendly name

    write32(0);       // Capture formats (array of uint16)

    write32(3);       // Playback formats (array of uint16)
    write16(0x3000);  // Undefined format
    write16(0x3001);  // Folders (associations)
    write16(0x3008);  // WAV

    writestring("PJRC");     // Manufacturer
    writestring("Teensy");   // Model
    writestring("1.0");      // version
    writestring("???");      // serial
  }

  void WriteStorageIDs() {
    write32(1); // 1 entry
    write32(1); // 1 storage
  }

  void GetStorageInfo(uint32_t storage) {
    write16(storage_->readonly() ? 0x0001 : 0x0004);   // storage type (removable RAM)
    write16(storage_->has_directories() ? 0x0002: 0x0001);   // filesystem type (generic hierarchical)
    write16(0x0000);   // access capability (read-write)
    write64(storage_->size());  // max capacity
    write64(storage_->free());  // free space (100M)
    write32(0xFFFFFFFFUL);  // free space (objects)
    writestring("SD Card");  // storage descriptor
    writestring("");  // volume identifier
  }

  uint32_t GetNumObjects(uint32_t storage,
                         uint32_t parent) {
    storage_->StartGetObjectHandles(parent);
    int num = 0;
    while (storage_->GetNextObjectHandle()) num++;
    return num;
  }

  void GetObjectHandles(uint32_t storage,
                        uint32_t parent) {
    uint32_t num = 0;
    if (!write_get_length_) {
      num = GetNumObjects(storage, parent);
    }
    write32(num);
    int handle;
    storage_->StartGetObjectHandles(parent);
    while ((handle = storage_->GetNextObjectHandle()))
      write32(handle);
  }

  void GetObjectInfo(uint32_t handle) {
    char filename[256];
    uint32_t size, parent;
    storage_->GetObjectInfo(handle, filename, &size, &parent);

    write32(1); // storage
    write16(size == 0xFFFFFFFFUL ? 0x3001 : 0x0000); // format
    write16(0);  // protection
    write32(size); // size
    write16(0); // thumb format
    write32(0); // thumb size
    write32(0); // thumb width
    write32(0); // thumb height
    write32(0); // pix width
    write32(0); // pix height
    write32(0); // bit depth
    write32(parent); // parent
    write16(size == 0xFFFFFFFFUL ? 1 : 0); // association type
    write32(0); // association description
    write32(0);  // sequence number
    writestring(filename);
    writestring("");  // date created
    writestring("");  // date modified
    writestring("");  // keywords
  }

  void GetObject(uint32_t object_id) {
    uint32_t size = storage_->GetSize(object_id);
    if (write_get_length_) {
      write_length_ += size;
    } else {
      uint32_t pos = 0;
      while (pos < size) {
        get_buffer();
        uint32_t avail = sizeof(data_buffer_->buf) - data_buffer_->len;
        uint32_t to_copy = min(pos - size, avail);
        // Read directly from storage into usb buffer.
        storage_->read(object_id,
                       pos,
                       (char*)(data_buffer_->buf + data_buffer_->len),
                       to_copy);
        pos += to_copy;
        data_buffer_->len += to_copy;
        if (data_buffer_->len == sizeof(data_buffer_->buf)) {
          usb_tx(MTP_TX_ENDPOINT, data_buffer_);
          data_buffer_ = NULL;
        }
      }
    }
  }

#define CONTAINER ((struct MTPContainer*)(receive_buffer->buf))

#define TRANSMIT(FUN) do {                              \
    write_length_ = 0;                                  \
    write_get_length_ = true;                           \
    FUN;                                                \
    write_get_length_ = false;                          \
    MTPHeader header;                                   \
    header.len = write_length_ + 12;                    \
    header.type = 2;                                    \
    header.op = CONTAINER->op;                          \
    header.transaction_id = CONTAINER->transaction_id;  \
    write((char *)&header, sizeof(header));             \
    FUN;                                                \
    get_buffer();                                       \
    usb_tx(MTP_TX_ENDPOINT, data_buffer_);              \
    data_buffer_ = NULL;                                \
  } while(0)
  
  void read(char* data, uint32_t size) {
    while (size) {
      receive_buffer();
      uint32_t to_copy = data_buffer_->len - data_buffer_->index;
      to_copy = min(to_copy, size);
      if (data) {
        memcpy(data,
               data_buffer_->buf + data_buffer_->index,
               to_copy);
        data += to_copy;
      }
      size -= to_copy;
      data_buffer_->index += to_copy;
      if (data_buffer_->index == data_buffer_->len) {
        usb_free(data_buffer_);
        data_buffer_ = NULL;
      }
    }
  }

  uint32_t ReadMTPHeader() {
    MTPHeader header;
    read((char *)&header, sizeof(MTPHeader));
    // check that the type is data
    return header.len - 12;
  }

  uint8_t read8() {
    uint8_t ret;
    read((char*)&ret, sizeof(ret));
    return ret;
  }
  uint16_t read16() {
    uint16_t ret;
    read((char*)&ret, sizeof(ret));
    return ret;
  }
  uint32_t read32() {
    uint32_t ret;
    read((char*)&ret, sizeof(ret));
    return ret;
  }

  void readstring(char* buffer) {
    int len = read8();
    if (!buffer) {
      read(NULL, len * 2);
    } else {
      for (int i = 0; i < len; i++) {
        *(buffer++) = read16();
      }
    }
  }

  void read_until_short_packet() {
    bool done = false;
    while (!done) {
      receive_buffer();
      done = data_buffer_->len != sizeof(data_buffer_->buf);
      usb_free(data_buffer_);
      data_buffer_ = NULL;
    }
  }

  uint32_t SendObjectInfo(uint32_t storage, uint32_t parent) {
    ReadMTPHeader();
    char filename[256];

    read32(); // storage
    bool dir = read16() == 0x3001; // format
    read16();  // protection
    read32(); // size
    read16(); // thumb format
    read32(); // thumb size
    read32(); // thumb width
    read32(); // thumb height
    read32(); // pix width
    read32(); // pix height
    read32(); // bit depth
    read32(); // parent
    read16(); // association type
    read32(); // association description
    read32();  // sequence number

    readstring(filename);
    read_until_short_packet();  // ignores dates & keywords
    return storage_->Create(parent, dir, filename);
  }

  void SendObject() {
    uint32_t len = ReadMTPHeader();
    while (len) {
      receive_buffer();
      uint32_t to_copy = data_buffer_->len - data_buffer_->index;
      to_copy = min(to_copy, len);
      storage_->write((char*)(data_buffer_->buf + data_buffer_->index),
                      to_copy);
      data_buffer_->index += to_copy;
      len -= to_copy;
      if (data_buffer_->index == data_buffer_->len) {
        usb_free(data_buffer_);
        data_buffer_ = NULL;
      }
    }
    storage_->close();
  }

  void GetDevicePropValue(uint32_t prop) {
    switch (prop) {
      case 0xd402: // friendly name
        // This is the name we'll actually see in the windows explorer.
        // Should probably be configurable.
        writestring("TeensySaber");
        break;
    }
  }
  void GetDevicePropDesc(uint32_t prop) {
    switch (prop) {
      case 0xd402: // friendly name
        write16(prop);
        write16(0xFFFF); // string type
        write8(0);       // read-only
        GetDevicePropValue(prop);
        GetDevicePropValue(prop);
        write8(0);       // no form
    }
  }

public:
  void loop() {
    usb_packet_t *receive_buffer;
    if ((receive_buffer = usb_rx(MTP_RX_ENDPOINT))) {
      PrintPacket(receive_buffer);
      uint32_t return_code = 0;
      uint32_t p1 = 0;
      if (receive_buffer->len >= 12) {
        return_code = 0x2001;  // Ok
        receive_buffer->len = 16;
        if (CONTAINER->type == 1) { // command
          switch (CONTAINER->op) {
            case 0x1001: // GetDescription
              TRANSMIT(WriteDescriptor());
              break;
            case 0x1002:  // OpenSession
              break;
            case 0x1003:  // CloseSession
              break;
            case 0x1004:  // GetStorageIDs
              TRANSMIT(WriteStorageIDs());
              break;
            case 0x1005:  // GetStorageInfo
              TRANSMIT(GetStorageInfo(CONTAINER->params[0]));
              break;
            case 0x1006:  // GetNumObjects
              if (CONTAINER->params[1]) {
                return_code = 0x2014; // spec by format unsupported
              } else {
                p1 = GetNumObjects(CONTAINER->params[0],
                                   CONTAINER->params[2]);
              }
              break;
            case 0x1007:  // GetObjectHandles
              if (CONTAINER->params[1]) {
                return_code = 0x2014; // spec by format unsupported
              } else {
                TRANSMIT(GetObjectHandles(CONTAINER->params[0],
                                          CONTAINER->params[2]));
              }
              break;
            case 0x1008:  // GetObjectInfo
              TRANSMIT(GetObjectInfo(CONTAINER->params[0]));
              break;
            case 0x1009:  // GetObject
              TRANSMIT(GetObject(CONTAINER->params[0]));
              break;
            case 0x100B:  // DeleteObject
              if (CONTAINER->params[1]) {
                return_code = 0x2014; // spec by format unsupported
              } else {
                if (!storage_->DeleteObject(CONTAINER->params[0])) {
                  return_code = 0x2012; // partial deletion
                }
              }
              break;
            case 0x100C:  // SendObjectInfo
              CONTAINER->params[2] =
                SendObjectInfo(CONTAINER->params[0], // storage
                               CONTAINER->params[1]); // parent
              p1 = CONTAINER->params[0];
              if (!p1) p1 = 1;
              CONTAINER->len = receive_buffer->len = 12 + 3 * 4;
              break;
            case 0x100D:  // SendObject
              SendObject();
              break;
            case 0x1014:  // GetDevicePropDesc
              TRANSMIT(GetDevicePropDesc(CONTAINER->params[0]));
              break;
            case 0x1015:  // GetDevicePropvalue
              TRANSMIT(GetDevicePropValue(CONTAINER->params[0]));
              break;
            default:
              return_code = 0x2005;  // operation not supported
              break;
          }
        } else {
          return_code = 0x2000;  // undefined
        }
      }
      if (return_code) {
        CONTAINER->type = 3;
        CONTAINER->op = return_code;
        CONTAINER->params[0] = p1;
        PrintPacket(receive_buffer);
        usb_tx(MTP_TX_ENDPOINT, receive_buffer);
        receive_buffer = 0;
      } else {
        usb_free(receive_buffer);
      }
    }
    // Maybe put event handling inside mtp_yield()?
    if ((receive_buffer = usb_rx(MTP_EVENT_ENDPOINT))) {
      usb_free(receive_buffer);
    }
  }
};

MTPStorage_SD storage;
MTPD mtpd(&storage);

#endif

int last_activity = millis();

void loop() {
#ifdef MTP_RX_ENDPOINT
  mtpd.loop();
#endif
  Looper::DoLoop();

#if defined(ENABLE_SNOOZE)
  bool on = false;
  SaberBase::DoIsOn(&on);
  if (!on && !Serial && !saber.NeedsPower()
#ifdef ENABLE_AUDIO
       && !amplifier.Active()
#endif
    ) {
    if (millis() - last_activity > 1000) {
      snooze_timer.setTimer(500);
      Snooze.sleep(snooze_config);
      Serial.begin(9600);
    }
  } else {
    last_activity = millis();
  }
#endif
}
