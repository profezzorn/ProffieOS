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

// Search for CONFIGURABLE in this file to find all the places which
// might need to be modified for your saber.


// Board version
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

#define NUM_BLADES 1

// If you have two 144 LED/m strips in your blade, connect
// both of them to bladePin and drive them in parallel.
const unsigned int maxLedsPerStrip = 144;


#if VERSION_MAJOR >= 2
#define V2
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
// reading data from a int16_t DataStream. Generally, that data
// stream is hooked up to the AudioDynamicMixer class. This
// class is responsible for taking multiple audio inputs,
// summing them up and then adjusting the volume to minimize
// clipping. Generally, one of the inputs are hooked up to
// the AudioSplicer class, and the others are hooked up to
// BufferedWavPlayers.  The AudioSplicer is able to do
// smooth cutovers between sounds, and it's inputs are also
// BufferedWavPlayers.
//

// TODO LIST:
// Make sure that sound is off before doing file command
// make "chargint style" prevent you from turning the saber "on"
// Audio work items:
//   Tune swings better
//   select clash from force
//   stab effect
//   synthesize swings
// Blade stuff
//    better clash
// Implement menues:
//    select sound font
//    select color
//    adjust volume
// Disable motion when off to save power.

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

// If defined, DAC vref will be 3 volts, resulting in louder sound.
#define LOUD

// If defined all sound samples will be divided by 2, resulting in
// quieter sound.
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
  bladePowerPin1 = 21,            // blade power control
  bladePowerPin2 = 22,            // blade power control
  bladePowerPin3 = 23,            // blade power control
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

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))

// Magic type used to prevent linked-list types from automatically linking.
enum NoLink { NOLINK = 17 };

// Helper class for classses that needs to be called back from the Loop() function.
// Also provides a Setup() function.
class Looper;
Looper* loopers = NULL;
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
    CHECK_LL(Looper, loopers, next_looper_);
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
  SABERFUN(Lockup, (), ());                     \
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
  SaberBase* next_saber_;
};

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
  };

  bool ShouldPrint(MonitorBit bit) {
    if (bit & monitor_soon_) {
      monitor_soon_ &= ~bit;
      return true;
    }
    return false;
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
  bool isOff() const {
    return current_ == 0 && target_ == 0;
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

template<class T>
class DataStream {
public:
  virtual int read(T* data, int elements) = 0;
  // There is no need to call eof() unless read() returns zero elements.
  virtual bool eof() { return false; }
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
        Serial.print(dac_dma_buffer[i] - 2047);
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

  void Help() override {
    Serial.println(" dacbuf - print the current contents of the dac buffer");
  }

  void SetStream(DataStream<int16_t>* stream) {
    stream_ = stream;
  }

private:
  // Interrupt handler.
  // Fills the dma buffer with new sample data.
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

  DMAMEM static uint16_t dac_dma_buffer[AUDIO_BUFFER_SIZE*2];
  static DataStream<int16_t> * volatile stream_;
  static DMAChannel dma;
};

DMAChannel DAC::dma(false);
DataStream<int16_t> * volatile DAC::stream_ = nullptr;
DMAMEM uint16_t DAC::dac_dma_buffer[AUDIO_BUFFER_SIZE*2];

DAC dac;

// Audio compressor, takes N input channels, sums them and divides the
// result by the square root of the average volume.
template<int N> class AudioDynamicMixer : public DataStream<int16_t>, Looper {
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
    int32_t sum[32];
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
        v2 = (v << 10) / (my_sqrt(vol_) + 100);
#else
        v2 = v;
#endif
#ifdef QUIET
        v2 >>= 8;
#endif
        data[i] = clampi32(v2, -32768, 32767);
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
  
  DataStream<int16_t>* streams_[N];
  int32_t vol_ = 0;
  int32_t last_sample_ = 0;
  int32_t last_sum_ = 0;
  int32_t peak_sum_ = 0;
  int32_t peak_ = 0;
  int32_t num_samples_ = 0;
//  int32_t sum_;
//  ClickAvoiderLin volume_;
};

AudioDynamicMixer<7> dynamic_mixer;

// Beeper class, used for warning beeps and such.
class Beeper : public DataStream<int16_t> {
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

class LightSaberSynth : public DataStream<int16_t>, Looper {
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

// DataStreamWork is a linked list of classes that would like to
// do some work in a software-triggered interrupt. This is used to
// let audio processing preempt less important tasks.
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
  static void ProcessDataStreams() {
    if (sd_locked) return;
#if 1
    // Yes, it's a selection sort, luckily there's not a lot of
    // DataStreamWork instances.
    for (int i = 0; i < 50; i++) {
      size_t max_space = 0;
      for (DataStreamWork *d = data_streams; d; d=d->next_)
        max_space = max(max_space, d->space_available());
      if (max_space == 0) break;
      for (DataStreamWork *d = data_streams; d; d=d->next_) {
        if (d->space_available() >= max_space)
          d->FillBuffer();
      }
    }
#else
    for (int i = 0; i < 10; i++) {
      for (DataStreamWork *d = data_streams; d; d=d->next_) {
        d->FillBuffer();
      }
    }
#endif
  }

  static volatile bool sd_locked;

  DataStreamWork* next_;
};

volatile bool DataStreamWork::sd_locked = false;
#define LOCK_SD(X) DataStreamWork::LockSD(X)

// BufferedDataStream is meant to be read from the main autdio interrupt.
// Every time some space is freed up, Schedulefillbuffer() is called
// to handle filling up the buffer at a lower interrupt level. Since
// filling up the buffer can mean reading from SD, there can potentially
// be more reads from the buffer while we're working on filling it up.
// To make this work, we need make sure that the end pointer for the buffer
// is only modified in the FillBuffer() function and the begin pointer is
// only modified in read();
// N needs to be power of 2
template<class T, int N>
class BufferedDataStream : public DataStream<T>, public DataStreamWork {
public:
  BufferedDataStream() : DataStreamWork() {
  }
  int read(T* buf, int bufsize) override {
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
      memcpy(buf, buffer_ + start_pos, sizeof(T) * to_copy);
      copied += to_copy;
      buf_start_ += to_copy;
      buf += to_copy;
      bufsize -= to_copy;
    }
    scheduleFillBuffer();
    return copied;
#endif
  }
  bool eof() override {
    return !buffered() && eof_;
  }
  void clear() {
    eof_ = false;
    buf_start_ = buf_end_;
  }
  int buffered() const {
    return buf_end_ - buf_start_;
  }
  size_t space_available() const override {
    if (eof_) return 0;
    return N - buffered();
  }
  void SetStream(DataStream<T>* stream) {
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
        if (!got) eof_ = stream_->eof();
        buf_end_ += got;
      }
    }
    return stream_ && space_available() > 0 && !eof_;
  }
  DataStream<T> * volatile stream_ = 0;
  // Note, these are assumed to be atomic, 8-bit processors won't work.
  volatile size_t buf_start_ = 0;
  volatile size_t buf_end_ = 0;
  volatile bool eof_ = false;
  T buffer_[N];
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
    EMIT(clampi32((upsample_buf_##NAME##_a_ * C2 +              \
          upsample_buf_##NAME##_b_ * C1 +                       \
          upsample_buf_##NAME##_c_ * C1 +                       \
                   upsample_buf_##NAME##_d_ * C2) >> 15,        \
                  -32768, 32767));                              \
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
class PlayWav : StateMachine, public DataStream<int16_t> {
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

  void Stop() {
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

  bool eof() override {
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


// Combines a WavPlayer and a BufferedDataStream into a
// buffered wav player. When we start a new sample, we
// make sure to fill up the buffer before we start playing it.
// This minimizes latency while making sure to avoid any gaps.
class BufferedWavPlayer : public DataStream<int16_t> {
public:
  void Play(const char* filename) {
    pause_ = true;
    buffer.clear();
    wav.Play(filename);
    buffer.scheduleFillBuffer();
    pause_ = false;
  }

  void PlayOnce(Effect* effect) {
    pause_ = true;
    buffer.clear();
    wav.PlayOnce(effect);
    buffer.scheduleFillBuffer();
    pause_ = false;
  }
  void PlayLoop(Effect* effect) { wav.PlayLoop(effect); }
  void Stop() {
    wav.Stop();
    pause_ = true;
    buffer.clear();
  }

  bool isPlaying() const {
    return wav.isPlaying();
  }

  BufferedWavPlayer() {
    buffer.SetStream(&wav);
  }

  int read(int16_t* dest, int to_read) override {
    if (pause_) return 0;
    return buffer.read(dest, to_read);
  }

  bool eof() override { return buffer.eof(); }

  float length() const { return wav.length(); }

private:
  BufferedDataStream<int16_t, 512> buffer;
  PlayWav wav;
  volatile bool pause_;
};

BufferedWavPlayer wav_players[6];
size_t allocated_wav_players = 2;

class BufferedWavPlayer* GetFreeWavPlayer()  {
  // Find a free wave playback unit.
  for (size_t unit = allocated_wav_players; unit < NELEM(wav_players); unit++) {
    if (!wav_players[unit].isPlaying()) {
      return wav_players + unit;
    }
  }
  return NULL;
}

class VolumeStream : public DataStream<int16_t> {
public:
  VolumeStream() : volume_(16384 / 100) {
    volume_.set(32768);
    volume_.set_target(32768);
  }
  int read(int16_t* data, int elements) override {
    elements = source_->read(data, elements);
    if (volume_.isOff()) {
      for (int i = 0; i < elements; i++) {
        data[i] = 0;
      }
    } else {
      for (int i = 0; i < elements; i++) {
        int32_t v = (data[i] * (int32_t)volume_.value()) >> 15;
        data[i] = clampi32(v, -32768, 32767);
        volume_.advance();
      }
    }
    return elements;
  }
  bool eof() override { return source_->eof(); }
  void set_volume(int vol) {
    volume_.set_target(vol);
  }
  void set_volume(float vol) {
    set_volume((int)(32768 * vol));
  }
  void set_speed(int speed) {
    volume_.set_speed(speed);
  }
  void set_fade_time(float t) {
    set_speed(max(1, (int)(32768 / t / AUDIO_RATE)));
  }
  void set_source(DataStream<int16_t>* source) {
    source_ = source;
  }
  bool isOff() const {
    return volume_.isOff() || source_->eof();
  }

private:
  DataStream<int16_t>* source_ = NULL;
  ClickAvoiderLin volume_;
};

VolumeStream volume_streams[3];

// This class is used to cut from one sound to another with
// no gap. It does a short (2.5ms) crossfade to accomplish this.
// It's currently hard-coded to use wav_players[0] and wav_playes[1].
class AudioSplicer : public DataStream<int16_t> {
public:
  AudioSplicer() {}

  int read(int16_t* data, int elements) override {
    int16_t *p = data;
    int to_read = elements;
    if (start_after_) {
      start_after_ -= elements;
      if (start_after_ < 0) {
        start_after_ = 0;
        current_ = fadeto_;
        fadeto_ = -1;
      }
      return 0;
    }
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

  bool eof() override {
    return current_ == -1 && fadeto_ == -1;
  }

  void set_fade_time(float t) {
    fade_speed_ = max(1, (int)(32768 / t / AUDIO_RATE));
#if 0
    Serial.print("FADE SPEED: ");
    Serial.println(fade_speed_);
#endif
  }

  bool Play(Effect* f, Effect* loop, int delay_ms = 0) {
    if (fadeto_ != -1) {
      Serial.print("cutover unit busy fade_ =");
      Serial.print(fade_);
      Serial.print(" fadeto_ = ");
      Serial.print(fadeto_);
      Serial.print(" current_ = ");
      Serial.println(current_);
      // Need to finish fading to the previous unit first. (~2.5ms)
      // Perhaps we should just wait for it?
      return false;
    }
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    int unit = current_ == 0 ? 1 : 0;
    wav_players[unit].PlayOnce(f);
    if (loop) {
      wav_players[unit].PlayLoop(loop);
    }
    if (delay_ms) {
      fadeto_ = unit;
      start_after_ = delay_ms * (AUDIO_RATE/100) / 10;
#if 0
      Serial.print("START AFTER: ");
      Serial.println(start_after_);
#endif
    } else {
      if (current_ == -1) {
        current_ = unit;
      } else {
        fadeto_ = unit;
        fade_ = 32768;
      }
    }
    return true;
  }

  bool isPlaying() const {
    return current_ != -1 || fadeto_ != -1;
  }

  void set_volume(int vol) {
    volume_streams[0].set_volume(vol);
  }

protected:
  volatile int current_= -1;
  volatile int fadeto_ = -1;
  volatile int fade_speed_ = 128;
  volatile int start_after_ = 0;
  volatile int fade_;
};

AudioSplicer audio_splicer;

// Configure links between audio units, filters, sources.
void SetupStandardAudio() {
  dac.SetStream(NULL);
  allocated_wav_players = 2;
  dynamic_mixer.streams_[0] = volume_streams + 0;
  volume_streams[0].set_source(&audio_splicer);
  dynamic_mixer.streams_[1] = &beeper;
  for (size_t i = 2; i < NELEM(wav_players); i++) {
    dynamic_mixer.streams_[i] = wav_players + i;
  }
  volume_streams[0].set_volume(10000);
  volume_streams[0].set_speed(16384 / 100);
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
    SetupStandardAudio();
    audio_splicer.set_fade_time(0.003);
    SaberBase::Link(this);
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

  bool on_ = false;
  bool swinging_ = false;
  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    if (speed > 250.0) {
      if (!swinging_ && on_) {
        swinging_ = true;
        audio_splicer.Play(&swing, &hum);
      }
    } else {
      swinging_ = false;
    }
    int vol = 10000;
    if (!swinging_) {
      vol = vol * (0.99 + clamp(speed/200.0, 0.0, 1.0));
    }
    audio_splicer.set_volume(vol);
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
      }
    }
  }
#endif

  void Read(const char *filename) {
#ifdef ENABLE_SD
    File f = SD.open(filename);
    Read(&f);
#endif
  }

  int humStart = 100;
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
    // TODO: while we need to fade out the hum,
    // maybe we don't need to fade in the in sound?
    SetupStandardAudio();
    audio_splicer.set_fade_time(0.3);
    char config_filename[128];
    strcpy(config_filename, current_directory);
    strcat(config_filename, "config.ini");
    config_.Read(config_filename);
    SaberBase::Link(this);
    on_ = false;
  }
  void Deactivate() { SaberBase::Unlink(this); }

  void SB_On() override {
    on_ = true;
    if (config_.humStart) {
      BufferedWavPlayer* tmp = Play(&out);
      if (tmp) {
        int delay_ms = 1000 * tmp->length() - config_.humStart;
#if 1
        Serial.print(" LEN = ");
        Serial.print(tmp->length());
        Serial.print(" humstart = ");
        Serial.print(config_.humStart);
        Serial.print(" delay_ms = ");
        Serial.println(delay_ms);
#endif
        audio_splicer.Play(&hum, &hum, delay_ms);
        return;
      }
    }
    audio_splicer.Play(&out, &hum);
  }

  void SB_Off() override {
    on_ = false;
    audio_splicer.Play(&in, NULL);
  }

  BufferedWavPlayer* Play(Effect* f)  {
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    BufferedWavPlayer* player = GetFreeWavPlayer();
    if (player) player->PlayOnce(f);
    return player;
  }
  void SB_Clash() override { Play(&clsh); }
  void SB_Stab() override { Play(&stab); }
  void SB_Force() override { Play(&force); }
  void SB_Blast() override { Play(&blst); }
  void SB_Boot() override { audio_splicer.Play(&boot,  NULL); }
  void SB_NewFont() override { audio_splicer.Play(&font,  NULL); }

  bool swinging_ = false;
  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    if (speed > 250.0) {
      if (!swinging_ && on_) {
        swinging_ = true;
        Play(&swng);
      }
    } else {
      swinging_ = false;
    }
    int vol = 10000;
    if (!swinging_) {
      vol = vol * (0.99 + clamp(speed/200.0, 0.0, 1.0));
    }
    audio_splicer.set_volume(vol);
  }

  ConfigFile config_;
  bool on_ = false;
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
class LoopedSwingWrapper : public SaberBasePassThrough, public Looper{
public:
  LoopedSwingWrapper() : SaberBasePassThrough(), Looper(NOLINK) {}

  void Activate(SaberBase* base_font) {
    Serial.println("Activating looped swing sounds");
    SetDelegate(base_font);
    allocated_wav_players = 4;
    // Inject volume controls for wave player 2,3.
    for (int i = 2; i < 4; i++) {
      VolumeStream *v = volume_streams + i - 1;
      dynamic_mixer.streams_[i] = v;
      v->set_source(wav_players + i);
      v->set_volume(0);
      v->set_speed(16384 / 100);
    }
  }

  void Deactivate() {
    SetDelegate(NULL);
    Looper::Unlink();
  }

  void SB_On() override {
    // Starts hum, etc.
    delegate_->SB_On();
    wav_players[2].PlayOnce(&swingl);
    wav_players[2].PlayLoop(&swingl);
    wav_players[3].PlayOnce(&swingh);
    wav_players[3].PlayLoop(&swingh);
  }
  void SB_Off() override {
    volume_streams[1].set_fade_time(0.3);
    volume_streams[2].set_fade_time(0.3);
    volume_streams[1].set_volume(0);
    volume_streams[2].set_volume(0);
    delegate_->SB_Off();
    Looper::Link();
  }

  void Loop() override {
    if (volume_streams[1].isOff()) {
      wav_players[2].PlayLoop(NULL);
    }
    if (volume_streams[2].isOff()) {
      wav_players[3].PlayLoop(NULL);
    }
    if (volume_streams[1].isOff() &&
        volume_streams[2].isOff()) {
      Looper::Unlink();
    }
  }

  void SB_Motion(const Vec3& gyro) override {
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    uint32_t t = (millis() >> 2);
    float s = sin_table[t & 1023] * (1.0/16383);
    float c = sin_table[(t + 256) & 1023] * (1.0/16383);
    float blend = c * gyro.z + s * gyro.y;
    blend = clamp(blend / 150.0, -1.0, 1.0);
    float vol = 0.299 + clamp(speed / 150.0, 0.0, 2.3);
    float low = max(0, blend);
    float high = max(0, -blend);
    float hum = 1.0 - abs(blend);
    volume_streams[0].set_volume(vol * hum);
    volume_streams[1].set_volume(vol * low);
    volume_streams[2].set_volume(vol * high);
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
  // Returns true only once.
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
    uint32_t num_leds = blade->num_leds();
    // Note we use "/ 4" instead of "* 256 / 1024".
    // It's close enough and avoids overflows.
    uint32_t delta = (now - last_micros_) * num_leds / 4;
    last_micros_ = now;
    if (blade->is_on()) {
      if (thres == 0) {
	// We might have been off for a while, so delta might
	// be insanely high.
	thres = 1;
      } else {
	thres += delta / OUT_MILLIS;
      }
    } else {
      if (thres == 0) blade->allow_disable();
      thres -= delta / IN_MILLIS;
    }
    thres = clampi32(thres, 0, num_leds * 256);
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
  uint32_t last_micros_;
};

template<class T, int OUT_MILLIS, int IN_MILLIS, class SPARK_COLOR = Rgb<255,255,255> >
class InOutSparkTip {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    uint32_t now = micros();
    uint32_t num_leds = blade->num_leds();
    // Note we use "/ 4" instead of "* 256 / 1024".
    // It's close enough and avoids overflows.
    uint32_t delta = (now - last_micros_) * num_leds / 4;
    last_micros_ = now;
    if ((on_ = blade->is_on())) {
      if (thres == 0) {
	// We might have been off for a while, so delta might
	// be insanely high.
	thres = 1;
      } else {
	thres += delta / OUT_MILLIS;
      }
    } else {
      if (thres == 0) blade->allow_disable();
      thres -= delta / IN_MILLIS;
    }
    thres = clampi32(thres, 0, num_leds * 256 + 1024);
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

// The following functions are mostly for illustration.
// The templates above gives you more power and functionality.

// Arguments: color, clash color, turn-on/off time
template<class base_color, class clash_color, int out_millis, int in_millis>
BladeStyle *StyleNormalPtr() {
  return StylePtr<InOutHelper<SimpleClash<base_color, clash_color>, out_millis, in_millis> >();
}

// Rainbow blade.
// Arguments: color, clash color, turn-on/off time
template<int out_millis, int in_millis>
BladeStyle *StyleRainbowPtr() {
  return StylePtr<InOutHelper<SimpleClash<Rainbow, Rgb<255,255,255> >, out_millis, in_millis> >();
}

// Stroboscope, flickers the blade at the desired frequency.
// Arguments: color, clash color, turn-on/off time
template<class strobe_color, class clash_color, int frequency, int out_millis, int in_millis>
BladeStyle *StyleStrobePtr() {
  typedef Rgb<0, 0, 0> black;
  typedef Strobe<black, strobe_color, frequency, 1> strobe;
  typedef SimpleClash<strobe, clash_color> clash;
  return StylePtr<InOutHelper<clash, out_millis, in_millis> >();
}


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

const unsigned char imagedata[] = {
 253, 0, 203, 0, 1, 41, 33, 253, 0, 225, 0, 253, 0, 200, 0,
 7, 3, 2, 0, 213, 173, 2, 5, 4, 253, 0, 222, 0, 253, 0,
 200, 0, 8, 31, 25, 0, 255, 207, 2, 104, 85, 1, 253, 0, 221,
 0, 253, 0, 200, 0, 10, 99, 81, 0, 255, 207, 2, 246, 199, 2,
 16, 13, 253, 0, 219, 0, 253, 0, 200, 0, 11, 209, 169, 1, 255,
 207, 2, 255, 207, 2, 161, 130, 1, 253, 0, 218, 0, 253, 0, 197,
 0, 7, 8, 6, 0, 248, 201, 2, 255, 207, 255, 2, 4, 255, 8,
 3, 1, 43, 34, 253, 0, 216, 0, 253, 0, 197, 0, 5, 45, 36,
 0, 255, 207, 2, 255, 2, 9, 4, 199, 161, 1, 3, 2, 253, 0,
 213, 0, 253, 0, 197, 0, 5, 128, 104, 1, 255, 207, 2, 255, 2,
 12, 1, 87, 71, 253, 0, 213, 0, 253, 0, 197, 0, 4, 223, 182,
 2, 255, 207, 255, 2, 13, 4, 227, 186, 2, 14, 11, 253, 0, 210,
 0, 253, 0, 194, 0, 7, 13, 10, 0, 250, 203, 2, 255, 207, 255,
 2, 16, 2, 151, 121, 1, 253, 0, 209, 0, 253, 0, 194, 0, 5,
 55, 45, 0, 255, 207, 2, 255, 2, 18, 4, 252, 205, 2, 44, 36,
 253, 0, 207, 0, 253, 0, 194, 0, 5, 141, 115, 1, 255, 207, 2,
 255, 2, 21, 4, 203, 164, 1, 5, 4, 253, 0, 204, 0, 253, 0,
 194, 0, 4, 233, 191, 2, 255, 207, 255, 2, 4, 4, 246, 199, 2,
 241, 197, 255, 11, 7, 255, 2, 9, 2, 107, 87, 1, 253, 0, 203,
 0, 253, 0, 191, 0, 7, 15, 12, 0, 252, 205, 2, 255, 207, 255,
 2, 4, 5, 151, 122, 1, 103, 84, 0, 255, 11, 6, 255, 2, 9,
 4, 246, 199, 2, 22, 18, 253, 0, 201, 0, 253, 0, 191, 0, 5,
 63, 52, 0, 255, 207, 2, 255, 2, 6, 7, 55, 44, 0, 5, 4,
 0, 215, 175, 255, 14, 7, 255, 2, 9, 1, 173, 139, 129, 1, 253,
 0, 198, 0, 253, 0, 191, 0, 13, 146, 118, 1, 255, 207, 2, 255,
 207, 2, 252, 205, 2, 12, 10, 130, 0, 4, 53, 43, 0, 250, 203,
 255, 17, 4, 255, 2, 12, 1, 84, 68, 253, 0, 198, 0, 253, 0,
 191, 0, 4, 235, 191, 2, 255, 207, 255, 2, 4, 2, 227, 184, 2,
 135, 0, 2, 154, 125, 1, 255, 20, 6, 255, 2, 9, 4, 231, 187,
 2, 18, 14, 253, 0, 195, 0, 253, 0, 188, 0, 7, 15, 12, 0,
 252, 205, 2, 255, 207, 255, 2, 4, 2, 125, 103, 1, 135, 0, 4,
 13, 10, 0, 241, 197, 255, 23, 7, 255, 2, 9, 1, 157, 128, 129,
 1, 253, 0, 192, 0, 253, 0, 188, 0, 5, 65, 52, 0, 255, 207,
 2, 255, 2, 6, 1, 50, 40, 139, 0, 4, 79, 65, 0, 252, 205,
 255, 26, 7, 255, 2, 9, 1, 73, 58, 253, 0, 192, 0, 253, 0,
 188, 0, 13, 144, 118, 1, 255, 207, 2, 255, 207, 2, 250, 203, 2,
 10, 8, 139, 0, 128, 1, 2, 0, 180, 147, 255, 29, 7, 255, 2,
 9, 4, 225, 182, 2, 14, 11, 253, 0, 189, 0, 253, 0, 188, 0,
 4, 233, 191, 2, 255, 207, 255, 2, 4, 2, 223, 180, 2, 144, 0,
 4, 25, 21, 0, 244, 197, 255, 32, 7, 255, 2, 9, 3, 168, 136,
 1, 1, 253, 0, 187, 0, 253, 0, 185, 0, 7, 15, 12, 0, 252,
 205, 2, 255, 207, 255, 2, 4, 2, 115, 93, 1, 147, 0, 2, 100,
 81, 0, 255, 32, 6, 255, 2, 12, 1, 79, 63, 253, 0, 186, 0,
 253, 0, 185, 0, 5, 56, 46, 0, 255, 207, 2, 255, 2, 6, 1,
 45, 36, 148, 0, 5, 3, 2, 0, 195, 159, 1, 255, 38, 9, 255,
 2, 6, 4, 233, 189, 2, 23, 18, 253, 0, 183, 0, 253, 0, 185,
 0, 13, 136, 111, 1, 255, 207, 2, 255, 207, 2, 250, 203, 2, 8,
 7, 151, 0, 4, 30, 25, 0, 246, 199, 255, 38, 4, 255, 2, 12,
 4, 189, 154, 1, 2, 1, 253, 0, 180, 0, 253, 0, 185, 0, 4,
 227, 186, 2, 255, 207, 255, 2, 4, 2, 227, 184, 2, 156, 0, 2,
 103, 84, 1, 255, 41, 6, 255, 2, 9, 4, 252, 207, 2, 100, 80,
 253, 0, 180, 0, 253, 0, 182, 0, 7, 11, 9, 0, 250, 203, 2,
 255, 207, 255, 2, 4, 2, 125, 103, 1, 156, 0, 128, 2, 3, 0,
 193, 157, 1, 255, 44, 6, 255, 2, 9, 4, 246, 201, 2, 33, 27,
 253, 0, 177, 0, 253, 0, 182, 0, 5, 47, 38, 0, 255, 207, 2,
 255, 2, 6, 1, 50, 40, 160, 0, 4, 25, 21, 0, 235, 191, 255,
 47, 7, 255, 2, 9, 4, 203, 164, 1, 6, 5, 253, 0, 174, 0,
 253, 0, 182, 0, 13, 118, 96, 1, 255, 207, 2, 255, 207, 2, 252,
 205, 2, 10, 8, 163, 0, 2, 87, 71, 0, 255, 50, 6, 255, 2,
 12, 2, 141, 114, 1, 253, 0, 173, 0, 253, 0, 182, 0, 4, 211,
 171, 2, 255, 207, 255, 2, 4, 2, 233, 189, 2, 165, 0, 128, 1,
 3, 0, 175, 141, 1, 255, 53, 6, 255, 2, 9, 4, 252, 205, 2,
 65, 52, 253, 0, 171, 0, 253, 0, 179, 0, 7, 6, 5, 0, 246,
 199, 2, 255, 207, 255, 2, 4, 2, 138, 112, 1, 168, 0, 4, 17,
 13, 0, 229, 186, 255, 56, 7, 255, 2, 9, 4, 233, 189, 2, 21,
 16, 253, 0, 168, 0, 253, 0, 179, 0, 5, 34, 28, 0, 255, 207,
 2, 255, 2, 6, 1, 57, 46, 172, 0, 4, 68, 55, 0, 250, 203,
 255, 59, 7, 255, 2, 9, 4, 199, 161, 1, 3, 2, 253, 0, 165,
 0, 253, 0, 179, 0, 13, 93, 75, 0, 255, 207, 2, 255, 207, 2,
 252, 205, 2, 15, 12, 172, 0, 5, 1, 0, 0, 131, 107, 1, 255,
 62, 6, 255, 2, 12, 2, 124, 101, 1, 253, 0, 164, 0, 253, 0,
 179, 0, 11, 182, 147, 1, 255, 207, 2, 255, 207, 2, 246, 199, 2,
 177, 0, 4, 6, 4, 0, 213, 173, 255, 62, 4, 255, 2, 12, 4,
 250, 203, 2, 66, 53, 253, 0, 162, 0, 253, 0, 176, 0, 7, 2,
 1, 0, 239, 195, 2, 255, 207, 255, 2, 4, 2, 156, 125, 1, 180,
 0, 4, 33, 27, 0, 237, 193, 255, 68, 7, 255, 2, 9, 4, 235,
 189, 2, 24, 19, 253, 0, 159, 0, 253, 0, 176, 0, 7, 3, 2,
 0, 248, 201, 2, 255, 207, 255, 2, 4, 1, 77, 61, 184, 0, 2,
 87, 71, 0, 255, 68, 6, 255, 2, 12, 4, 201, 162, 1, 6, 5,
 253, 0, 156, 0, 253, 0, 179, 0, 4, 237, 193, 2, 255, 207, 255,
 2, 4, 1, 33, 27, 184, 0, 5, 1, 0, 0, 151, 122, 1, 255,
 71, 6, 255, 2, 12, 1, 149, 121, 129, 1, 253, 0, 153, 0, 253,
 0, 179, 0, 10, 171, 139, 1, 255, 207, 2, 255, 207, 2, 59, 48,
 187, 0, 4, 8, 7, 0, 211, 171, 255, 71, 4, 255, 2, 12, 4,
 252, 205, 2, 97, 79, 253, 0, 153, 0, 253, 0, 179, 0, 10, 127,
 104, 1, 255, 207, 2, 255, 207, 2, 90, 73, 190, 0, 4, 31, 25,
 0, 244, 199, 255, 74, 4, 255, 2, 12, 4, 248, 201, 2, 45, 36,
 253, 0, 150, 0, 253, 0, 179, 0, 11, 77, 61, 0, 255, 207, 2,
 255, 207, 2, 157, 127, 1, 192, 0, 4, 72, 58, 0, 252, 205, 255,
 77, 4, 255, 2, 12, 4, 231, 187, 2, 21, 17, 253, 0, 147, 0,
 253, 0, 179, 0, 11, 36, 29, 0, 255, 207, 2, 255, 207, 2, 199,
 162, 1, 195, 0, 2, 138, 111, 1, 255, 80, 6, 255, 2, 12, 4,
 205, 166, 1, 6, 5, 253, 0, 144, 0, 253, 0, 179, 0, 13, 18,
 15, 0, 255, 207, 2, 255, 207, 2, 252, 205, 2, 1, 1, 193, 0,
 5, 5, 4, 0, 184, 149, 1, 255, 83, 6, 255, 2, 12, 1, 159,
 128, 129, 1, 253, 0, 141, 0, 253, 0, 179, 0, 128, 2, 5, 0,
 250, 203, 2, 255, 207, 255, 2, 4, 1, 11, 9, 196, 0, 4, 16,
 13, 0, 229, 186, 255, 83, 7, 255, 2, 12, 2, 117, 95, 1, 253,
 0, 140, 0, 253, 0, 182, 0, 4, 223, 182, 2, 255, 207, 255, 2,
 4, 1, 31, 25, 199, 0, 4, 42, 34, 0, 239, 193, 255, 86, 7,
 255, 2, 9, 4, 248, 201, 2, 81, 66, 253, 0, 138, 0, 253, 0,
 182, 0, 10, 169, 138, 1, 255, 207, 2, 255, 207, 2, 70, 56, 202,
 0, 4, 82, 67, 0, 250, 203, 255, 86, 4, 255, 2, 12, 4, 244,
 199, 2, 44, 36, 253, 0, 135, 0, 253, 0, 182, 0, 11, 107, 86,
 1, 255, 207, 2, 255, 207, 2, 105, 86, 1, 204, 0, 1, 127, 103,
 255, 89, 7, 255, 2, 12, 4, 229, 187, 2, 25, 21, 253, 0, 132,
 0, 253, 0, 182, 0, 11, 69, 55, 0, 255, 207, 2, 255, 207, 2,
 187, 152, 1, 204, 0, 5, 2, 1, 0, 169, 138, 1, 255, 92, 6,
 255, 2, 12, 4, 211, 171, 1, 13, 11, 253, 0, 129, 0, 253, 0,
 182, 0, 11, 25, 21, 0, 255, 207, 2, 255, 207, 2, 223, 180, 2,
 207, 0, 5, 9, 8, 0, 199, 161, 1, 255, 95, 6, 255, 2, 12,
 4, 191, 154, 1, 6, 4, 253, 0, 182, 0, 5, 12, 10, 0, 255,
 207, 2, 255, 2, 6, 1, 5, 4, 208, 0, 4, 20, 16, 0, 229,
 186, 255, 95, 7, 255, 2, 12, 4, 162, 131, 1, 3, 3, 250, 0,
 253, 0, 185, 0, 4, 246, 201, 2, 255, 207, 255, 2, 4, 1, 23,
 19, 211, 0, 4, 39, 31, 0, 237, 193, 255, 98, 7, 255, 2, 12,
 3, 142, 115, 1, 1, 248, 0, 253, 0, 185, 0, 10, 199, 161, 1,
 255, 207, 2, 255, 207, 2, 49, 40, 214, 0, 4, 66, 54, 0, 250,
 203, 255, 98, 4, 255, 2, 15, 2, 117, 95, 1, 246, 0, 253, 0,
 185, 0, 10, 139, 112, 1, 255, 207, 2, 255, 207, 2, 103, 84, 217,
 0, 4, 92, 75, 0, 252, 205, 255, 101, 4, 255, 2, 15, 1, 97,
 79, 244, 0, 253, 0, 185, 0, 11, 86, 70, 0, 255, 207, 2, 255,
 207, 2, 154, 125, 1, 219, 0, 4, 124, 100, 1, 252, 205, 255, 104,
 4, 255, 2, 12, 4, 248, 201, 2, 80, 66, 241, 0, 253, 0, 185,
 0, 11, 50, 40, 0, 255, 207, 2, 255, 207, 2, 231, 187, 2, 219,
 0, 128, 1, 3, 0, 149, 121, 1, 255, 107, 6, 255, 2, 12, 4,
 250, 203, 2, 65, 52, 238, 0, 253, 0, 185, 0, 13, 13, 10, 0,
 255, 207, 2, 255, 207, 2, 244, 199, 2, 3, 2, 220, 0, 128, 3,
 3, 0, 168, 136, 1, 255, 110, 6, 255, 2, 12, 4, 246, 201, 2,
 60, 49, 235, 0, 253, 0, 185, 0, 7, 4, 3, 0, 250, 203, 2,
 255, 207, 255, 2, 4, 1, 18, 15, 223, 0, 5, 5, 4, 0, 191,
 154, 1, 255, 110, 6, 255, 2, 12, 4, 241, 195, 2, 52, 42, 232,
 0, 253, 0, 188, 0, 4, 217, 176, 2, 255, 207, 255, 2, 4, 1,
 45, 36, 226, 0, 5, 10, 9, 0, 193, 156, 1, 255, 113, 6, 255,
 2, 12, 4, 246, 199, 2, 45, 37, 229, 0, 253, 0, 188, 0, 10,
 157, 128, 1, 255, 207, 2, 255, 207, 2, 84, 68, 229, 0, 4, 13,
 11, 0, 211, 171, 255, 113, 4, 255, 2, 15, 4, 235, 191, 2, 50,
 40, 226, 0, 253, 0, 188, 0, 11, 97, 79, 0, 255, 207, 2, 255,
 207, 2, 156, 127, 1, 231, 0, 4, 19, 15, 0, 217, 175, 255, 116,
 4, 255, 2, 15, 4, 241, 195, 2, 43, 34, 223, 0, 253, 0, 188,
 0, 11, 50, 40, 0, 255, 207, 2, 255, 207, 2, 211, 171, 1, 234,
 0, 4, 25, 20, 0, 215, 175, 255, 119, 4, 255, 2, 15, 4, 233,
 189, 2, 48, 39, 220, 0, 253, 0, 188, 0, 13, 23, 18, 0, 255,
 207, 2, 255, 207, 2, 250, 205, 2, 4, 3, 235, 0, 4, 25, 20,
 0, 229, 186, 255, 122, 4, 255, 2, 15, 4, 235, 191, 2, 53, 43,
 217, 0, 253, 0, 188, 0, 128, 3, 5, 0, 250, 203, 2, 255, 207,
 255, 2, 4, 1, 14, 11, 238, 0, 4, 30, 24, 0, 219, 178, 255,
 125, 7, 255, 2, 12, 4, 248, 203, 2, 55, 45, 214, 0, 253, 0,
 191, 0, 4, 233, 187, 2, 255, 207, 255, 2, 4, 1, 51, 41, 241,
 0, 4, 27, 22, 0, 225, 182, 255, 128, 7, 255, 2, 12, 3, 219,
 176, 2, 1, 212, 0, 253, 0, 191, 0, 10, 157, 127, 1, 255, 207,
 2, 255, 207, 2, 86, 71, 244, 0, 4, 25, 19, 0, 227, 184, 255,
 128, 4, 255, 2, 12, 2, 151, 122, 1, 213, 0, 253, 0, 191, 0,
 11, 103, 82, 1, 255, 207, 2, 255, 207, 2, 162, 131, 1, 246, 0,
 4, 27, 21, 0, 215, 175, 255, 131, 4, 255, 2, 9, 1, 57, 47,
 214, 0, 253, 0, 191, 0, 11, 52, 41, 0, 255, 207, 2, 255, 207,
 2, 217, 175, 2, 249, 0, 4, 21, 17, 0, 227, 186, 255, 134, 4,
 255, 2, 3, 4, 252, 205, 2, 10, 8, 133, 0, 4, 29, 24, 0,
 1, 1, 202, 0, 253, 0, 191, 0, 5, 20, 16, 0, 255, 207, 2,
 255, 2, 6, 1, 3, 2, 250, 0, 2, 151, 122, 1, 255, 134, 6,
 2, 197, 161, 1, 135, 0, 4, 118, 96, 1, 59, 48, 202, 0, 253,
 0, 191, 0, 5, 4, 3, 0, 255, 207, 2, 255, 2, 6, 1, 25,
 20, 247, 0, 4, 1, 0, 0, 231, 187, 255, 134, 7, 2, 100, 81,
 1, 135, 0, 4, 223, 180, 2, 211, 171, 255, 14, 10, 192, 0, 253,
 0, 194, 0, 10, 213, 173, 1, 255, 207, 2, 255, 207, 2, 51, 41,
 247, 0, 2, 19, 15, 0, 255, 131, 6, 255, 2, 3, 1, 30, 25,
 133, 0, 4, 15, 12, 0, 252, 205, 255, 17, 4, 1, 55, 45, 199,
 0, 253, 0, 194, 0, 11, 166, 135, 1, 255, 207, 2, 255, 207, 2,
 115, 93, 1, 246, 0, 2, 77, 62, 0, 255, 131, 6, 4, 233, 189,
 2, 3, 2, 133, 0, 1, 72, 58, 255, 20, 7, 1, 211, 171, 255,
 152, 73, 253, 0, 194, 0, 11, 86, 70, 0, 255, 207, 2, 255, 207,
 2, 168, 136, 1, 246, 0, 2, 169, 138, 1, 255, 131, 6, 1, 162,
 131, 255, 131, 10, 1, 176, 144, 255, 20, 7, 255, 2, 3, 1, 55,
 45, 196, 0, 253, 0, 194, 0, 12, 49, 40, 0, 255, 207, 2, 255,
 207, 2, 241, 195, 2, 1, 242, 0, 4, 3, 2, 0, 246, 201, 255,
 128, 4, 255, 2, 3, 1, 70, 56, 133, 0, 4, 4, 3, 0, 241,
 195, 255, 20, 7, 255, 2, 3, 1, 211, 171, 255, 154, 70, 253, 0,
 194, 0, 5, 14, 12, 0, 255, 207, 2, 255, 2, 6, 1, 7, 5,
 241, 0, 1, 31, 25, 255, 128, 10, 255, 140, 3, 132, 0, 1, 35,
 29, 255, 20, 10, 255, 2, 6, 1, 55, 45, 193, 0, 253, 0, 194,
 0, 7, 3, 2, 0, 241, 197, 2, 255, 207, 255, 2, 4, 1, 36,
 29, 241, 0, 2, 103, 84, 0, 255, 125, 6, 3, 215, 175, 2, 1,
 134, 0, 2, 108, 87, 1, 255, 20, 6, 255, 2, 9, 1, 211, 171,
 255, 28, 9, 184, 0, 253, 0, 197, 0, 10, 213, 173, 1, 255, 207,
 2, 255, 207, 2, 71, 57, 241, 0, 1, 193, 157, 255, 125, 7, 2,
 131, 107, 1, 135, 0, 1, 215, 173, 255, 17, 4, 255, 2, 15, 1,
 55, 45, 190, 0, 253, 0, 197, 0, 11, 131, 107, 1, 255, 207, 2,
 255, 207, 2, 136, 111, 1, 237, 0, 128, 5, 0, 0, 255, 122, 6,
 255, 2, 3, 1, 47, 38, 133, 0, 4, 11, 9, 0, 250, 203, 255,
 20, 7, 255, 8, 9, 255, 2, 3, 1, 211, 171, 129, 1, 187, 0,
 253, 0, 197, 0, 11, 79, 65, 0, 255, 207, 2, 255, 207, 2, 209,
 169, 1, 237, 0, 1, 44, 36, 255, 122, 7, 4, 250, 203, 2, 8,
 6, 133, 0, 1, 56, 45, 255, 20, 7, 255, 2, 3, 4, 59, 48,
 0, 252, 205, 255, 11, 7, 255, 2, 3, 1, 67, 55, 187, 0, 253,
 0, 197, 0, 10, 28, 23, 0, 255, 207, 2, 255, 207, 2, 250, 203,
 129, 2, 166, 0, 3, 18, 14, 0, 1, 191, 0, 2, 115, 95, 1,
 255, 122, 6, 1, 201, 162, 255, 76, 10, 1, 142, 115, 255, 20, 7,
 1, 187, 152, 255, 20, 4, 1, 180, 147, 255, 14, 7, 255, 2, 3,
 3, 225, 182, 2, 1, 255, 117, 59, 253, 0, 149, 0, 4, 8, 6,
 0, 2, 2, 169, 0, 6, 10, 8, 0, 252, 207, 2, 255, 255, 2,
 5, 1, 21, 17, 166, 0, 4, 46, 37, 0, 105, 86, 129, 1, 187,
 0, 2, 209, 169, 1, 255, 119, 6, 1, 104, 84, 255, 72, 10, 1,
 233, 189, 255, 140, 7, 1, 82, 67, 130, 0, 4, 29, 24, 0, 252,
 205, 255, 17, 7, 255, 2, 3, 1, 80, 66, 184, 0, 253, 0, 149,
 0, 4, 60, 49, 0, 90, 73, 172, 0, 4, 227, 182, 2, 255, 207,
 255, 2, 4, 1, 48, 40, 166, 0, 10, 12, 9, 0, 248, 201, 2,
 133, 108, 1, 2, 2, 181, 0, 2, 9, 8, 0, 255, 116, 6, 255,
 2, 3, 1, 36, 29, 133, 0, 4, 20, 16, 0, 252, 205, 255, 20,
 7, 255, 11, 3, 132, 0, 2, 178, 146, 1, 255, 41, 9, 3, 231,
 187, 2, 3, 255, 113, 56, 253, 0, 149, 0, 7, 119, 96, 1, 233,
 189, 2, 50, 40, 169, 0, 11, 184, 149, 1, 255, 207, 2, 255, 207,
 2, 124, 100, 1, 165, 0, 255, 39, 3, 1, 229, 186, 255, 50, 4,
 4, 146, 118, 1, 4, 3, 178, 0, 255, 161, 3, 255, 116, 6, 4,
 244, 199, 2, 5, 4, 133, 0, 1, 73, 59, 255, 20, 7, 2, 248,
 203, 2, 135, 0, 4, 23, 19, 0, 252, 205, 255, 20, 4, 255, 2,
 6, 1, 96, 78, 181, 0, 253, 0, 149, 0, 10, 180, 146, 1, 255,
 207, 2, 225, 182, 2, 25, 20, 166, 0, 11, 95, 77, 0, 255, 207,
 2, 255, 207, 2, 176, 142, 1, 168, 0, 1, 164, 133, 255, 104, 4,
 7, 252, 205, 2, 166, 135, 1, 6, 5, 175, 0, 1, 125, 103, 255,
 62, 4, 255, 2, 3, 1, 182, 147, 255, 116, 10, 1, 173, 141, 255,
 20, 7, 1, 147, 119, 255, 137, 13, 1, 161, 131, 255, 23, 7, 255,
 2, 3, 3, 239, 195, 2, 7, 255, 110, 50, 129, 0, 253, 0, 149,
 0, 4, 241, 195, 2, 255, 207, 255, 2, 4, 4, 186, 151, 1, 9,
 8, 163, 0, 2, 54, 43, 0, 255, 50, 6, 4, 241, 197, 2, 2,
 1, 166, 0, 1, 86, 70, 255, 53, 7, 255, 2, 3, 4, 176, 142,
 1, 12, 9, 172, 0, 1, 215, 175, 255, 59, 7, 1, 93, 75, 133,
 0, 4, 3, 2, 0, 237, 193, 255, 20, 7, 1, 57, 46, 139, 0,
 4, 19, 15, 0, 250, 203, 255, 26, 7, 255, 2, 3, 1, 118, 96,
 255, 165, 41, 137, 0, 253, 0, 146, 0, 5, 9, 7, 0, 255, 207,
 2, 255, 2, 9, 4, 152, 124, 1, 2, 2, 160, 0, 1, 15, 12,
 255, 53, 10, 255, 65, 3, 165, 0, 1, 32, 26, 255, 107, 13, 4,
 195, 159, 1, 15, 12, 166, 0, 1, 10, 8, 255, 59, 10, 1, 28,
 23, 133, 0, 1, 27, 22, 255, 20, 10, 1, 12, 9, 142, 0, 2,
 144, 117, 1, 255, 29, 9, 4, 244, 199, 2, 14, 11, 175, 0, 253,
 0, 146, 0, 5, 33, 27, 0, 255, 207, 2, 255, 2, 9, 4, 250,
 203, 2, 109, 90, 129, 1, 157, 0, 128, 2, 2, 0, 244, 199, 255,
 50, 7, 1, 51, 41, 166, 0, 4, 6, 5, 0, 252, 205, 255, 107,
 10, 255, 2, 3, 4, 215, 175, 2, 27, 22, 163, 0, 1, 52, 42,
 255, 167, 7, 4, 239, 193, 2, 3, 2, 133, 0, 1, 88, 72, 255,
 20, 7, 1, 233, 189, 255, 18, 8, 137, 0, 4, 11, 9, 0, 248,
 201, 255, 107, 10, 1, 151, 122, 255, 213, 32, 143, 0, 253, 0, 146,
 0, 5, 72, 58, 0, 255, 207, 2, 255, 2, 12, 4, 248, 201, 2,
 74, 60, 160, 0, 2, 203, 164, 1, 255, 47, 6, 1, 88, 72, 169,
 0, 1, 217, 175, 255, 107, 13, 255, 2, 3, 4, 225, 182, 2, 48,
 40, 160, 0, 1, 128, 104, 255, 110, 7, 2, 175, 142, 1, 135, 0,
 1, 191, 156, 255, 20, 7, 1, 122, 100, 255, 20, 10, 138, 0, 1,
 117, 95, 255, 32, 7, 255, 215, 4, 3, 203, 2, 27, 22, 172, 0,
 253, 0, 146, 0, 5, 130, 105, 1, 255, 207, 2, 255, 2, 15, 4,
 231, 187, 2, 52, 42, 157, 0, 1, 121, 99, 255, 56, 7, 2, 180,
 146, 1, 255, 110, 63, 4, 244, 199, 2, 66, 54, 157, 0, 1, 215,
 175, 255, 44, 7, 1, 86, 70, 133, 0, 4, 4, 3, 0, 244, 197,
 255, 20, 7, 1, 44, 36, 148, 0, 4, 6, 5, 0, 239, 195, 255,
 104, 10, 1, 182, 149, 255, 170, 43, 129, 0, 253, 0, 146, 0, 5,
 191, 156, 1, 255, 207, 2, 255, 2, 18, 4, 227, 184, 2, 31, 25,
 154, 0, 2, 69, 56, 0, 255, 41, 9, 168, 0, 1, 58, 47, 255,
 53, 7, 255, 2, 15, 7, 248, 203, 2, 100, 81, 0, 1, 1, 148,
 0, 1, 9, 8, 255, 53, 10, 1, 27, 22, 133, 0, 255, 158, 3,
 255, 20, 9, 1, 7, 5, 151, 0, 1, 90, 73, 255, 38, 10, 4,
 252, 205, 2, 47, 39, 169, 0, 253, 0, 143, 0, 7, 1, 0, 0,
 250, 203, 2, 255, 207, 255, 2, 22, 4, 201, 162, 1, 21, 17, 151,
 0, 255, 26, 3, 255, 41, 9, 1, 10, 8, 166, 0, 1, 19, 15,
 255, 53, 10, 255, 2, 15, 255, 137, 3, 4, 135, 109, 1, 5, 4,
 145, 0, 1, 50, 40, 255, 53, 7, 4, 239, 193, 2, 3, 2, 133,
 0, 1, 95, 78, 255, 20, 7, 1, 211, 173, 255, 18, 8, 146, 0,
 4, 2, 1, 0, 233, 187, 255, 101, 10, 1, 211, 171, 129, 1, 166,
 0, 253, 0, 143, 0, 5, 10, 8, 0, 255, 207, 2, 255, 2, 27,
 4, 191, 156, 1, 11, 9, 148, 0, 4, 5, 4, 0, 252, 205, 255,
 38, 7, 1, 36, 29, 166, 0, 128, 1, 2, 0, 244, 197, 255, 113,
 28, 4, 171, 139, 1, 13, 11, 142, 0, 2, 122, 100, 1, 255, 29,
 6, 2, 175, 142, 1, 135, 0, 1, 197, 161, 255, 20, 7, 1, 108,
 88, 255, 20, 10, 147, 0, 1, 60, 50, 255, 230, 13, 1, 78, 63,
 166, 0, 253, 0, 143, 0, 5, 35, 29, 0, 255, 207, 2, 255, 2,
 30, 4, 175, 141, 1, 7, 5, 148, 0, 2, 205, 166, 1, 255, 35,
 6, 1, 91, 74, 169, 0, 1, 175, 142, 255, 53, 7, 255, 2, 24,
 4, 215, 173, 2, 30, 24, 139, 0, 1, 209, 169, 255, 50, 7, 1,
 86, 70, 133, 0, 4, 5, 4, 0, 244, 197, 255, 47, 7, 1, 37,
 31, 160, 0, 255, 170, 9, 255, 2, 3, 3, 239, 193, 2, 6, 255,
 209, 23, 141, 0, 253, 0, 143, 0, 13, 77, 62, 0, 255, 207, 2,
 255, 207, 2, 250, 203, 2, 252, 205, 255, 8, 4, 255, 2, 21, 4,
 151, 121, 1, 5, 4, 145, 0, 2, 151, 122, 1, 255, 32, 6, 2,
 161, 131, 1, 168, 0, 1, 87, 71, 255, 116, 7, 255, 2, 27, 4,
 235, 191, 2, 65, 52, 133, 0, 1, 7, 6, 255, 47, 10, 1, 27,
 22, 133, 0, 1, 36, 29, 255, 20, 10, 255, 155, 21, 141, 0, 2,
 35, 28, 0, 255, 224, 12, 1, 119, 97, 255, 176, 37, 253, 0, 143,
 0, 13, 127, 103, 1, 255, 207, 2, 255, 207, 2, 182, 149, 1, 157,
 127, 255, 11, 7, 255, 2, 18, 7, 252, 205, 2, 144, 117, 1, 2,
 2, 142, 0, 2, 63, 52, 0, 255, 32, 6, 1, 235, 191, 255, 27,
 17, 152, 0, 1, 36, 30, 255, 53, 7, 255, 2, 3, 1, 248, 201,
 255, 113, 22, 255, 2, 3, 4, 241, 197, 2, 112, 91, 255, 116, 4,
 1, 44, 35, 255, 47, 7, 255, 17, 3, 128, 3, 133, 0, 1, 100,
 81, 255, 20, 7, 1, 211, 173, 255, 122, 40, 1, 166, 135, 255, 224,
 10, 2, 149, 121, 1, 162, 0, 253, 0, 143, 0, 16, 187, 152, 1,
 255, 207, 2, 255, 207, 2, 103, 84, 1, 9, 8, 0, 176, 142, 255,
 14, 7, 255, 2, 18, 7, 250, 203, 2, 130, 105, 1, 2, 2, 139,
 0, 2, 33, 27, 0, 255, 29, 6, 4, 252, 205, 2, 10, 8, 166,
 0, 4, 4, 3, 0, 252, 205, 255, 86, 7, 2, 125, 101, 1, 255,
 131, 9, 255, 2, 21, 4, 156, 125, 1, 152, 124, 255, 32, 7, 2,
 182, 147, 1, 135, 0, 1, 193, 157, 255, 20, 7, 1, 99, 80, 166,
 0, 3, 19, 15, 0, 252, 255, 86, 8, 1, 62, 51, 163, 0, 253,
 0, 143, 0, 4, 246, 199, 2, 255, 207, 255, 2, 4, 1, 55, 45,
 130, 0, 5, 20, 16, 0, 197, 161, 1, 255, 17, 6, 255, 2, 18,
 7, 252, 205, 2, 119, 97, 1, 2, 1, 136, 0, 4, 4, 3, 0,
 252, 205, 255, 29, 7, 1, 40, 31, 169, 0, 1, 209, 169, 255, 53,
 7, 10, 115, 93, 1, 8, 7, 0, 152, 124, 1, 250, 203, 255, 116,
 22, 255, 119, 6, 255, 8, 6, 1, 95, 77, 255, 119, 10, 1, 244,
 197, 255, 20, 7, 1, 33, 27, 166, 0, 2, 17, 14, 0, 255, 83,
 9, 1, 13, 10, 163, 0, 253, 0, 140, 0, 5, 9, 7, 0, 255,
 207, 2, 255, 2, 6, 1, 22, 18, 133, 0, 4, 25, 21, 0, 221,
 178, 255, 20, 7, 255, 2, 18, 4, 248, 201, 2, 122, 99, 129, 1,
 136, 0, 1, 219, 176, 255, 26, 7, 1, 95, 77, 169, 0, 2, 117,
 95, 1, 255, 53, 6, 1, 199, 161, 255, 75, 4, 128, 1, 255, 17,
 4, 1, 241, 197, 255, 119, 25, 255, 2, 6, 1, 33, 27, 133, 0,
 1, 31, 25, 255, 185, 10, 128, 3, 166, 0, 1, 75, 61, 255, 53,
 7, 255, 224, 3, 165, 0, 253, 0, 140, 0, 13, 31, 25, 0, 255,
 207, 2, 255, 207, 2, 248, 201, 2, 5, 4, 136, 0, 4, 40, 32,
 0, 221, 178, 255, 23, 4, 255, 2, 21, 7, 250, 203, 2, 114, 92,
 1, 2, 2, 133, 0, 2, 138, 112, 1, 255, 23, 6, 2, 166, 135,
 1, 168, 0, 1, 56, 46, 255, 122, 7, 1, 246, 199, 255, 122, 10,
 4, 56, 45, 0, 237, 191, 255, 119, 25, 0, 246, 255, 164, 11, 1,
 95, 78, 255, 62, 7, 1, 207, 168, 255, 116, 43, 1, 168, 136, 255,
 170, 7, 1, 119, 96, 255, 53, 40, 253, 0, 140, 0, 11, 66, 53,
 0, 255, 207, 2, 255, 207, 2, 217, 175, 2, 141, 0, 4, 46, 37,
 0, 237, 191, 255, 26, 4, 255, 2, 24, 1, 119, 97, 129, 1, 130,
 0, 1, 81, 67, 255, 68, 7, 2, 233, 189, 2, 255, 17, 6, 162,
 0, 1, 9, 7, 255, 53, 7, 255, 2, 3, 1, 33, 27, 136, 0,
 5, 30, 24, 0, 201, 162, 1, 255, 113, 21, 1, 195, 159, 255, 93,
 10, 1, 187, 152, 255, 35, 7, 1, 108, 88, 255, 114, 40, 4, 3,
 2, 0, 237, 193, 255, 74, 7, 1, 41, 33, 166, 0, 253, 0, 140,
 0, 11, 115, 93, 1, 255, 207, 2, 255, 207, 2, 169, 138, 1, 144,
 0, 4, 53, 42, 0, 237, 193, 255, 29, 4, 255, 2, 21, 11, 248,
 203, 2, 121, 99, 1, 2, 1, 0, 25, 21, 0, 255, 20, 9, 1,
 11, 9, 166, 0, 255, 111, 3, 1, 235, 191, 255, 53, 7, 1, 90,
 73, 139, 0, 7, 9, 7, 0, 138, 112, 1, 252, 205, 255, 113, 16,
 1, 108, 88, 255, 164, 7, 128, 3, 2, 0, 241, 195, 255, 20, 7,
 1, 37, 31, 166, 0, 1, 28, 23, 255, 167, 10, 1, 5, 4, 166,
 0, 253, 0, 140, 0, 11, 173, 141, 1, 255, 207, 2, 255, 207, 2,
 108, 88, 1, 147, 0, 4, 71, 57, 0, 235, 191, 255, 32, 4, 255,
 2, 21, 10, 252, 205, 2, 127, 103, 1, 25, 20, 0, 246, 201, 255,
 17, 7, 1, 49, 40, 169, 0, 1, 135, 108, 255, 125, 7, 1, 187,
 152, 255, 125, 16, 128, 1, 5, 0, 95, 77, 0, 239, 195, 255, 110,
 13, 1, 44, 36, 133, 0, 2, 27, 22, 0, 255, 20, 9, 1, 5,
 4, 166, 0, 1, 91, 74, 255, 53, 7, 1, 193, 156, 255, 236, 22,
 147, 0, 253, 0, 140, 0, 4, 233, 187, 2, 255, 207, 255, 2, 4,
 1, 59, 48, 151, 0, 4, 68, 55, 0, 244, 197, 255, 38, 7, 255,
 2, 21, 4, 151, 121, 1, 252, 205, 255, 11, 7, 1, 99, 80, 169,
 0, 2, 74, 60, 0, 255, 53, 6, 4, 241, 197, 2, 3, 2, 145,
 0, 4, 45, 36, 0, 219, 178, 255, 104, 10, 1, 9, 7, 133, 0,
 1, 79, 65, 255, 56, 7, 1, 211, 173, 255, 54, 20, 149, 0, 2,
 186, 151, 1, 255, 53, 6, 1, 95, 77, 169, 0, 253, 0, 137, 0,
 5, 5, 4, 0, 255, 207, 2, 255, 2, 6, 1, 23, 19, 154, 0,
 4, 69, 55, 0, 237, 193, 255, 41, 7, 255, 2, 18, 1, 252, 205,
 255, 2, 4, 255, 11, 6, 2, 189, 154, 1, 168, 0, 1, 15, 12,
 255, 128, 10, 1, 27, 22, 148, 0, 11, 10, 8, 0, 157, 127, 1,
 252, 205, 2, 209, 169, 2, 135, 0, 2, 169, 138, 1, 255, 53, 6,
 1, 115, 93, 255, 110, 40, 255, 236, 3, 1, 246, 199, 255, 107, 7,
 1, 28, 23, 169, 0, 253, 0, 137, 0, 13, 21, 16, 0, 255, 207,
 2, 255, 207, 2, 252, 205, 2, 7, 6, 157, 0, 4, 78, 62, 0,
 237, 193, 255, 44, 4, 255, 2, 30, 4, 241, 195, 2, 1, 1, 166,
 0, 4, 2, 1, 0, 246, 199, 255, 56, 7, 1, 75, 61, 151, 0,
 128, 1, 5, 0, 78, 63, 0, 128, 104, 255, 87, 7, 128, 1, 2,
 0, 235, 191, 255, 53, 7, 1, 44, 36, 166, 0, 1, 33, 27, 255,
 236, 7, 3, 250, 203, 2, 2, 255, 161, 41, 129, 0, 253, 0, 137,
 0, 13, 53, 43, 0, 255, 207, 2, 255, 207, 2, 223, 180, 2, 1,
 1, 160, 0, 4, 62, 51, 0, 246, 199, 255, 47, 4, 255, 2, 30,
 1, 22, 17, 169, 0, 2, 156, 127, 1, 255, 53, 6, 1, 178, 146,
 255, 129, 31, 1, 5, 4, 133, 0, 1, 18, 14, 255, 182, 7, 255,
 2, 3, 1, 9, 7, 166, 0, 1, 103, 84, 255, 53, 7, 1, 169,
 138, 255, 234, 35, 137, 0, 253, 0, 137, 0, 11, 91, 74, 0, 255,
 207, 2, 255, 207, 2, 186, 151, 1, 165, 0, 4, 63, 52, 0, 227,
 184, 255, 50, 4, 255, 2, 27, 1, 55, 45, 169, 0, 1, 82, 67,
 255, 131, 7, 3, 237, 193, 2, 1, 255, 133, 38, 1, 66, 53, 255,
 50, 7, 2, 233, 189, 2, 168, 0, 2, 199, 161, 1, 255, 53, 6,
 1, 77, 62, 172, 0, 253, 0, 137, 0, 11, 152, 124, 1, 255, 207,
 2, 255, 207, 2, 122, 100, 1, 168, 0, 4, 55, 45, 0, 225, 182,
 255, 53, 4, 255, 2, 24, 1, 138, 112, 255, 77, 43, 2, 25, 20,
 0, 255, 56, 9, 1, 29, 23, 163, 0, 1, 139, 114, 255, 182, 7,
 1, 131, 107, 255, 104, 40, 4, 6, 5, 0, 248, 201, 255, 101, 7,
 1, 23, 19, 172, 0, 253, 0, 137, 0, 10, 201, 162, 1, 255, 207,
 2, 255, 207, 2, 69, 55, 172, 0, 4, 40, 32, 0, 229, 186, 255,
 56, 4, 255, 2, 21, 2, 209, 169, 1, 168, 0, 4, 2, 1, 0,
 246, 199, 255, 56, 7, 1, 79, 65, 163, 0, 255, 122, 9, 1, 55,
 45, 166, 0, 2, 39, 32, 0, 255, 50, 6, 3, 241, 195, 2, 1,
 255, 157, 43, 130, 0, 253, 0, 134, 0, 128, 1, 3, 0, 255, 207,
 2, 255, 2, 6, 1, 36, 29, 175, 0, 5, 33, 27, 0, 205, 168,
 1, 255, 65, 9, 255, 2, 12, 4, 250, 203, 2, 6, 5, 169, 0,
 1, 171, 139, 255, 71, 7, 2, 175, 142, 1, 159, 0, 4, 9, 8,
 0, 250, 203, 255, 104, 7, 1, 15, 12, 166, 0, 1, 108, 88, 255,
 98, 7, 1, 156, 125, 255, 98, 34, 141, 0, 253, 0, 134, 0, 5,
 9, 8, 0, 255, 207, 2, 255, 2, 6, 1, 11, 9, 178, 0, 6,
 26, 21, 0, 187, 152, 1, 252, 255, 68, 8, 255, 2, 12, 1, 28,
 22, 169, 0, 1, 75, 61, 255, 137, 7, 4, 246, 201, 2, 2, 1,
 157, 0, 1, 44, 35, 255, 44, 7, 2, 248, 203, 2, 168, 0, 2,
 199, 161, 1, 255, 53, 6, 1, 71, 57, 175, 0, 253, 0, 134, 0,
 13, 33, 27, 0, 255, 207, 2, 255, 207, 2, 239, 195, 2, 3, 2,
 181, 0, 7, 14, 11, 0, 175, 141, 1, 252, 205, 255, 71, 4, 255,
 2, 12, 1, 92, 74, 169, 0, 1, 30, 25, 255, 137, 7, 255, 2,
 3, 1, 26, 21, 157, 0, 2, 105, 86, 1, 255, 41, 6, 2, 159,
 130, 1, 165, 0, 4, 6, 5, 0, 248, 201, 255, 95, 7, 1, 19,
 15, 175, 0, 253, 0, 134, 0, 11, 62, 51, 0, 255, 207, 2, 255,
 207, 2, 205, 164, 1, 186, 0, 5, 7, 6, 0, 141, 115, 1, 255,
 74, 6, 255, 2, 9, 1, 162, 133, 255, 83, 43, 4, 2, 1, 0,
 241, 195, 255, 56, 7, 1, 90, 73, 157, 0, 1, 203, 164, 255, 107,
 7, 1, 77, 61, 166, 0, 1, 39, 32, 255, 233, 7, 255, 101, 3,
 255, 234, 51, 253, 0, 134, 0, 11, 115, 93, 1, 255, 207, 2, 255,
 207, 2, 152, 124, 1, 189, 0, 7, 4, 3, 0, 108, 87, 1, 246,
 201, 255, 77, 4, 255, 2, 6, 3, 235, 191, 2, 1, 255, 85, 44,
 1, 187, 152, 255, 140, 7, 1, 169, 138, 255, 54, 28, 255, 104, 3,
 1, 241, 197, 255, 98, 7, 1, 27, 22, 166, 0, 1, 108, 88, 255,
 92, 7, 1, 156, 125, 255, 233, 52, 253, 0, 134, 0, 10, 162, 131,
 1, 255, 207, 2, 255, 207, 2, 95, 77, 193, 0, 7, 1, 0, 0,
 82, 67, 0, 235, 191, 255, 80, 4, 255, 2, 6, 1, 13, 10, 169,
 0, 2, 77, 61, 0, 255, 53, 6, 4, 250, 203, 2, 3, 3, 151,
 0, 1, 25, 20, 255, 38, 7, 255, 2, 3, 1, 2, 1, 166, 0,
 1, 193, 157, 255, 233, 7, 1, 71, 57, 178, 0, 253, 0, 134, 0,
 4, 225, 182, 2, 255, 207, 255, 2, 4, 1, 54, 43, 199, 0, 4,
 48, 39, 0, 219, 178, 255, 86, 7, 1, 59, 48, 169, 0, 2, 29,
 23, 0, 255, 53, 6, 255, 2, 3, 1, 27, 22, 151, 0, 1, 73,
 59, 255, 38, 7, 2, 201, 164, 1, 165, 0, 4, 5, 4, 0, 246,
 199, 255, 89, 7, 1, 19, 15, 178, 0, 253, 0, 131, 0, 5, 2,
 1, 0, 255, 207, 2, 255, 2, 6, 1, 23, 19, 202, 0, 255, 77,
 3, 8, 197, 159, 1, 255, 207, 2, 124, 100, 1, 168, 0, 128, 1,
 2, 0, 235, 191, 255, 143, 7, 1, 107, 86, 255, 56, 25, 1, 152,
 124, 255, 89, 4, 255, 2, 3, 1, 108, 88, 255, 92, 40, 1, 33,
 27, 255, 233, 7, 2, 241, 195, 2, 255, 144, 43, 137, 0, 253, 0,
 131, 0, 13, 14, 11, 0, 255, 207, 2, 255, 207, 2, 252, 205, 2,
 7, 5, 205, 0, 8, 11, 9, 0, 156, 125, 1, 213, 173, 2, 171,
 0, 2, 173, 141, 1, 255, 146, 6, 2, 189, 154, 1, 150, 0, 1,
 231, 187, 255, 32, 4, 255, 2, 3, 1, 45, 37, 166, 0, 1, 96,
 79, 255, 233, 7, 1, 159, 128, 255, 86, 25, 156, 0, 253, 0, 131,
 0, 13, 35, 29, 0, 255, 207, 2, 255, 207, 2, 225, 182, 2, 1,
 1, 208, 0, 7, 4, 3, 0, 109, 88, 1, 5, 4, 169, 0, 1,
 71, 57, 255, 146, 7, 4, 246, 199, 2, 7, 5, 145, 0, 4, 10,
 8, 0, 250, 203, 255, 32, 4, 255, 2, 3, 1, 9, 8, 166, 0,
 2, 184, 149, 1, 255, 50, 6, 1, 75, 61, 181, 0, 253, 0, 131,
 0, 11, 68, 55, 0, 255, 207, 2, 255, 207, 2, 195, 157, 1, 216,
 0, 0, 2, 255, 91, 44, 1, 18, 15, 255, 146, 7, 255, 2, 3,
 1, 42, 34, 145, 0, 1, 42, 33, 255, 32, 7, 2, 244, 197, 2,
 165, 0, 4, 3, 2, 0, 241, 195, 255, 83, 7, 1, 23, 19, 181,
 0, 253, 0, 131, 0, 11, 115, 93, 1, 255, 207, 2, 255, 207, 2,
 133, 108, 1, 253, 0, 134, 0, 128, 1, 2, 0, 235, 191, 255, 146,
 4, 255, 2, 3, 1, 112, 91, 255, 149, 19, 2, 99, 80, 0, 255,
 29, 6, 1, 151, 122, 255, 179, 40, 1, 27, 21, 255, 50, 7, 3,
 250, 203, 2, 1, 255, 232, 56, 253, 0, 131, 0, 10, 162, 131, 1,
 255, 207, 2, 255, 207, 2, 87, 70, 253, 0, 138, 0, 1, 144, 117,
 255, 149, 7, 2, 221, 178, 2, 144, 0, 1, 187, 152, 255, 29, 7,
 1, 73, 59, 166, 0, 2, 78, 63, 0, 255, 50, 6, 2, 173, 141,
 1, 183, 0, 253, 0, 131, 0, 4, 225, 182, 2, 255, 207, 255, 2,
 4, 1, 45, 36, 253, 0, 138, 0, 2, 66, 54, 0, 255, 149, 6,
 4, 250, 205, 2, 10, 8, 139, 0, 128, 1, 2, 0, 237, 193, 255,
 179, 7, 1, 27, 22, 166, 0, 2, 166, 135, 1, 255, 50, 6, 1,
 88, 71, 184, 0, 253, 0, 128, 0, 5, 2, 1, 0, 255, 207, 2,
 255, 2, 6, 1, 27, 22, 253, 0, 138, 0, 4, 11, 9, 0, 250,
 203, 255, 152, 7, 1, 65, 53, 139, 0, 1, 18, 14, 255, 179, 10,
 1, 3, 2, 163, 0, 128, 1, 2, 0, 229, 186, 255, 50, 7, 1,
 29, 24, 184, 0, 253, 0, 128, 0, 5, 9, 7, 0, 255, 207, 2,
 255, 2, 6, 1, 47, 38, 253, 0, 141, 0, 1, 225, 182, 255, 152,
 7, 2, 147, 119, 1, 138, 0, 1, 57, 46, 255, 179, 7, 2, 217,
 176, 2, 165, 0, 1, 15, 13, 255, 230, 10, 128, 3, 184, 0, 253,
 0, 128, 0, 5, 7, 6, 0, 255, 207, 2, 255, 2, 6, 1, 85,
 69, 253, 0, 141, 0, 2, 118, 96, 1, 255, 152, 6, 4, 235, 191,
 2, 1, 1, 136, 0, 1, 121, 99, 255, 23, 7, 1, 119, 97, 255,
 21, 11, 155, 0, 1, 58, 47, 255, 230, 7, 1, 193, 156, 255, 50,
 40, 147, 0, 253, 0, 131, 0, 4, 219, 176, 2, 255, 207, 255, 2,
 4, 2, 180, 146, 1, 253, 0, 140, 0, 2, 45, 36, 0, 255, 152,
 6, 255, 2, 3, 1, 25, 20, 136, 0, 2, 209, 169, 1, 255, 20,
 6, 1, 55, 45, 166, 0, 1, 135, 109, 255, 50, 7, 1, 108, 87,
 255, 227, 61, 253, 0, 131, 0, 11, 156, 125, 1, 255, 207, 2, 255,
 207, 2, 225, 182, 2, 253, 0, 140, 0, 4, 7, 6, 0, 248, 201,
 255, 152, 4, 255, 2, 3, 1, 78, 63, 133, 0, 4, 4, 3, 0,
 244, 197, 255, 20, 7, 1, 16, 13, 166, 0, 1, 213, 171, 255, 50,
 7, 1, 43, 35, 187, 0, 253, 0, 131, 0, 5, 92, 74, 0, 255,
 207, 2, 255, 2, 6, 1, 9, 7, 253, 0, 141, 0, 2, 211, 171,
 1, 255, 152, 6, 1, 79, 65, 133, 0, 1, 22, 18, 255, 173, 10,
 165, 0, 4, 7, 6, 0, 250, 203, 255, 224, 70, 253, 0, 131, 0,
 5, 29, 23, 0, 255, 207, 2, 255, 2, 6, 1, 44, 36, 253, 0,
 141, 0, 1, 237, 193, 255, 152, 7, 1, 31, 25, 133, 0, 1, 66,
 54, 255, 173, 7, 2, 187, 152, 1, 165, 0, 1, 36, 30, 255, 50,
 7, 2, 225, 182, 2, 189, 0, 253, 0, 131, 0, 5, 12, 10, 0,
 255, 207, 2, 255, 2, 6, 1, 82, 67, 253, 0, 138, 0, 1, 8,
 6, 255, 152, 7, 4, 250, 203, 2, 7, 5, 133, 0, 2, 135, 109,
 1, 255, 20, 6, 2, 105, 85, 1, 165, 0, 1, 100, 81, 255, 71,
 7, 1, 136, 111, 255, 50, 40, 150, 0, 253, 0, 134, 0, 4, 231,
 187, 2, 255, 207, 255, 2, 4, 2, 184, 149, 1, 253, 0, 137, 0,
 2, 35, 29, 0, 255, 149, 6, 2, 219, 176, 2, 135, 0, 1, 215,
 175, 255, 170, 7, 1, 44, 36, 166, 0, 255, 212, 3, 255, 50, 6,
 1, 65, 53, 190, 0, 253, 0, 134, 0, 11, 162, 131, 1, 255, 207,
 2, 255, 207, 2, 219, 178, 2, 253, 0, 137, 0, 2, 84, 68, 0,
 255, 149, 6, 2, 154, 125, 1, 132, 0, 4, 5, 4, 0, 244, 199,
 255, 17, 4, 255, 2, 3, 1, 12, 9, 163, 0, 4, 2, 1, 0,
 235, 191, 255, 50, 7, 1, 20, 16, 190, 0, 253, 0, 134, 0, 5,
 100, 81, 0, 255, 207, 2, 255, 2, 6, 1, 12, 10, 253, 0, 135,
 0, 2, 151, 122, 1, 255, 146, 6, 1, 81, 67, 133, 0, 1, 25,
 20, 255, 167, 10, 165, 0, 1, 18, 14, 255, 50, 7, 4, 250, 203,
 2, 1, 1, 190, 0, 253, 0, 134, 0, 5, 31, 25, 0, 255, 207,
 2, 255, 2, 6, 1, 45, 36, 253, 0, 135, 0, 1, 225, 182, 255,
 146, 7, 1, 34, 28, 133, 0, 1, 68, 55, 255, 167, 7, 2, 178,
 146, 1, 165, 0, 1, 61, 50, 255, 50, 7, 1, 176, 144, 255, 50,
 40, 153, 0, 253, 0, 134, 0, 7, 14, 11, 0, 252, 205, 2, 255,
 207, 255, 2, 4, 1, 91, 73, 253, 0, 132, 0, 2, 6, 5, 0,
 255, 143, 6, 255, 2, 3, 1, 8, 7, 133, 0, 2, 135, 109, 1,
 255, 17, 6, 2, 99, 80, 1, 165, 0, 1, 133, 108, 255, 50, 7,
 1, 97, 79, 193, 0, 253, 0, 137, 0, 4, 231, 187, 2, 255, 207,
 255, 2, 4, 2, 186, 151, 1, 253, 0, 131, 0, 2, 28, 22, 0,
 255, 143, 6, 2, 223, 180, 2, 135, 0, 1, 215, 175, 255, 164, 7,
 1, 44, 36, 166, 0, 1, 211, 169, 255, 50, 7, 1, 39, 31, 193,
 0, 253, 0, 137, 0, 12, 157, 128, 1, 255, 207, 2, 255, 207, 2,
 227, 184, 2, 1, 253, 0, 130, 0, 2, 70, 56, 0, 255, 143, 6,
 1, 166, 135, 255, 142, 7, 4, 4, 3, 0, 244, 199, 255, 17, 4,
 255, 2, 3, 1, 12, 9, 163, 0, 4, 6, 5, 0, 250, 203, 255,
 50, 7, 1, 8, 6, 193, 0, 253, 0, 137, 0, 5, 96, 78, 0,
 255, 207, 2, 255, 2, 6, 1, 18, 15, 253, 0, 129, 0, 2, 133,
 107, 1, 255, 140, 6, 1, 93, 75, 133, 0, 1, 22, 18, 255, 161,
 10, 165, 0, 1, 31, 25, 255, 50, 7, 1, 225, 182, 255, 50, 40,
 156, 0, 253, 0, 137, 0, 5, 31, 25, 0, 255, 207, 2, 255, 2,
 6, 1, 52, 42, 253, 0, 129, 0, 2, 201, 162, 1, 255, 140, 6,
 1, 41, 33, 133, 0, 1, 62, 51, 255, 161, 7, 2, 178, 146, 1,
 165, 0, 1, 81, 67, 255, 50, 7, 1, 139, 114, 255, 50, 40, 156,
 0, 253, 0, 137, 0, 7, 12, 9, 0, 250, 205, 2, 255, 207, 255,
 2, 4, 2, 115, 93, 1, 252, 0, 128, 1, 0, 0, 255, 137, 6,
 255, 2, 3, 1, 12, 10, 133, 0, 2, 121, 99, 1, 255, 17, 6,
 1, 99, 80, 255, 158, 40, 1, 161, 131, 255, 50, 7, 1, 70, 57,
 196, 0, 253, 0, 140, 0, 4, 223, 180, 2, 255, 207, 255, 2, 4,
 2, 201, 164, 1, 252, 0, 2, 18, 15, 0, 255, 137, 6, 3, 231,
 187, 2, 1, 255, 139, 8, 2, 205, 166, 1, 255, 20, 6, 1, 44,
 36, 255, 209, 49, 1, 24, 19, 196, 0, 253, 0, 140, 0, 13, 147,
 121, 1, 255, 207, 2, 255, 207, 2, 244, 197, 2, 2, 1, 250, 0,
 2, 52, 42, 0, 255, 137, 6, 1, 186, 151, 255, 135, 7, 255, 143,
 3, 1, 239, 195, 255, 17, 4, 255, 2, 3, 1, 13, 10, 163, 0,
 1, 10, 8, 255, 68, 7, 255, 2, 3, 128, 2, 196, 0, 253, 0,
 140, 0, 5, 84, 68, 0, 255, 207, 2, 255, 2, 6, 1, 31, 25,
 250, 0, 1, 104, 84, 255, 137, 7, 2, 115, 93, 1, 132, 0, 1,
 16, 13, 255, 155, 10, 165, 0, 1, 41, 33, 255, 50, 7, 1, 201,
 162, 255, 68, 7, 192, 0, 253, 0, 140, 0, 5, 23, 19, 0, 255,
 207, 2, 255, 2, 6, 1, 66, 53, 250, 0, 2, 169, 138, 1, 255,
 134, 6, 1, 58, 47, 133, 0, 1, 49, 40, 255, 155, 7, 2, 187,
 152, 1, 165, 0, 1, 96, 79, 255, 50, 7, 1, 118, 96, 255, 50,
 40, 159, 0, 253, 0, 140, 0, 7, 8, 7, 0, 250, 205, 2, 255,
 207, 255, 2, 4, 2, 157, 127, 1, 249, 0, 1, 237, 191, 255, 134,
 7, 1, 21, 17, 133, 0, 2, 103, 84, 1, 255, 17, 6, 1, 112,
 91, 255, 152, 40, 1, 176, 144, 255, 50, 7, 1, 56, 46, 199, 0,
 253, 0, 143, 0, 11, 203, 164, 1, 255, 207, 2, 255, 207, 2, 219,
 178, 2, 246, 0, 2, 8, 6, 0, 255, 131, 6, 4, 246, 199, 2,
 4, 3, 133, 0, 1, 180, 146, 255, 152, 7, 1, 55, 45, 163, 0,
 128, 1, 2, 0, 231, 187, 255, 47, 4, 255, 2, 3, 1, 16, 13,
 199, 0, 253, 0, 143, 0, 5, 135, 109, 1, 255, 207, 2, 255, 2,
 6, 1, 8, 7, 244, 0, 2, 31, 25, 0, 255, 128, 6, 2, 211,
 169, 2, 135, 0, 1, 233, 189, 255, 149, 7, 1, 19, 15, 163, 0,
 1, 13, 10, 255, 68, 7, 4, 250, 203, 2, 1, 1, 199, 0, 253,
 0, 143, 0, 5, 56, 46, 0, 255, 207, 2, 255, 2, 6, 1, 51,
 41, 244, 0, 1, 70, 56, 255, 131, 7, 2, 146, 119, 1, 132, 0,
 4, 9, 7, 0, 250, 203, 255, 149, 7, 128, 2, 163, 0, 1, 45,
 36, 255, 68, 7, 1, 184, 149, 255, 68, 7, 195, 0, 253, 0, 143,
 0, 5, 15, 13, 0, 255, 207, 2, 255, 2, 6, 1, 95, 77, 244,
 0, 2, 130, 105, 1, 255, 128, 6, 1, 85, 69, 133, 0, 1, 32,
 26, 255, 149, 7, 2, 221, 180, 2, 165, 0, 1, 101, 82, 255, 50,
 7, 2, 108, 87, 1, 201, 0, 253, 0, 143, 0, 128, 2, 5, 0,
 250, 205, 2, 255, 207, 255, 2, 4, 2, 205, 166, 1, 243, 0, 2,
 191, 156, 1, 255, 128, 6, 1, 38, 31, 133, 0, 2, 74, 60, 0,
 255, 17, 6, 1, 135, 109, 255, 146, 40, 1, 178, 144, 255, 68, 7,
 1, 47, 39, 202, 0, 253, 0, 146, 0, 13, 162, 131, 1, 255, 207,
 2, 255, 207, 2, 237, 193, 2, 3, 2, 238, 0, 128, 1, 2, 0,
 250, 203, 255, 125, 4, 255, 2, 3, 1, 11, 9, 133, 0, 1, 133,
 108, 255, 146, 7, 1, 71, 57, 255, 68, 40, 1, 231, 187, 255, 68,
 7, 1, 13, 11, 202, 0, 253, 0, 146, 0, 5, 107, 87, 1, 255,
 207, 2, 255, 2, 6, 1, 28, 22, 238, 0, 2, 12, 10, 0, 255,
 122, 6, 4, 233, 189, 2, 1, 1, 133, 0, 1, 207, 168, 255, 146,
 7, 1, 30, 24, 163, 0, 0, 13, 255, 68, 8, 2, 246, 199, 2,
 255, 67, 8, 196, 0, 253, 0, 146, 0, 5, 33, 27, 0, 255, 207,
 2, 255, 2, 6, 1, 77, 62, 238, 0, 1, 36, 30, 255, 125, 7,
 2, 191, 156, 1, 132, 0, 128, 2, 2, 0, 239, 195, 255, 143, 7,
 1, 7, 5, 163, 0, 1, 45, 36, 255, 68, 7, 1, 176, 144, 255,
 68, 7, 198, 0, 253, 0, 146, 0, 7, 9, 7, 0, 248, 201, 2,
 255, 207, 255, 2, 4, 2, 159, 128, 1, 237, 0, 2, 84, 68, 0,
 255, 122, 6, 1, 125, 101, 255, 122, 7, 1, 14, 12, 255, 17, 7,
 2, 250, 203, 2, 165, 0, 1, 95, 77, 255, 50, 7, 1, 104, 84,
 205, 0, 253, 0, 149, 0, 4, 225, 182, 2, 255, 207, 255, 2, 4,
 4, 229, 186, 2, 2, 1, 235, 0, 2, 144, 117, 1, 255, 122, 6,
 1, 68, 55, 133, 0, 2, 46, 37, 0, 255, 17, 6, 1, 182, 149,
 255, 138, 40, 1, 173, 139, 255, 68, 7, 1, 52, 42, 205, 0, 253,
 0, 149, 0, 5, 122, 100, 1, 255, 207, 2, 255, 2, 6, 1, 11,
 9, 172, 0, 4, 18, 14, 0, 17, 14, 184, 0, 1, 205, 164, 255,
 122, 7, 1, 28, 22, 133, 0, 2, 91, 74, 0, 255, 17, 6, 2,
 108, 88, 1, 162, 0, 128, 1, 2, 0, 225, 182, 255, 188, 7, 1,
 16, 13, 205, 0, 253, 0, 149, 0, 5, 69, 56, 0, 255, 207, 2,
 255, 2, 6, 1, 70, 56, 172, 0, 13, 80, 65, 0, 213, 173, 2,
 141, 115, 1, 49, 39, 0, 3, 2, 172, 0, 128, 1, 255, 119, 10,
 1, 8, 6, 133, 0, 2, 157, 128, 1, 255, 17, 6, 1, 55, 44,
 163, 0, 1, 8, 7, 255, 68, 7, 4, 250, 203, 2, 1, 1, 205,
 0, 253, 0, 149, 0, 5, 13, 10, 0, 255, 207, 2, 255, 2, 6,
 2, 118, 96, 1, 171, 0, 1, 19, 16, 255, 59, 7, 10, 237, 191,
 2, 184, 149, 1, 61, 50, 0, 8, 7, 166, 0, 1, 14, 12, 255,
 59, 7, 3, 225, 182, 2, 1, 255, 118, 8, 1, 221, 180, 255, 137,
 7, 1, 22, 17, 163, 0, 1, 36, 30, 255, 68, 7, 255, 125, 3,
 207, 0, 253, 0, 149, 0, 128, 2, 5, 0, 237, 195, 2, 255, 207,
 255, 2, 4, 2, 231, 187, 2, 171, 0, 4, 3, 2, 0, 246, 201,
 255, 59, 7, 255, 2, 3, 10, 250, 203, 2, 195, 159, 1, 73, 59,
 0, 9, 8, 160, 0, 2, 40, 33, 0, 255, 53, 6, 2, 180, 146,
 1, 132, 0, 1, 5, 4, 255, 77, 10, 1, 4, 3, 163, 0, 1,
 81, 67, 255, 68, 7, 1, 118, 96, 255, 68, 7, 201, 0, 253, 0,
 152, 0, 13, 154, 125, 1, 255, 207, 2, 255, 207, 2, 250, 205, 2,
 7, 6, 172, 0, 1, 149, 121, 255, 59, 7, 255, 2, 9, 10, 250,
 203, 2, 191, 156, 1, 90, 73, 0, 9, 7, 154, 0, 2, 85, 69,
 0, 255, 47, 6, 2, 114, 93, 1, 132, 0, 1, 21, 17, 255, 17,
 7, 2, 229, 186, 2, 165, 0, 1, 152, 124, 255, 125, 7, 1, 56,
 46, 208, 0, 243, 0, 128, 3, 2, 0, 5, 4, 157, 0, 5, 91,
 74, 0, 255, 207, 2, 255, 2, 6, 1, 54, 44, 172, 0, 1, 85,
 69, 255, 59, 10, 255, 2, 12, 10, 250, 203, 2, 209, 169, 1, 85,
 69, 0, 13, 10, 148, 0, 2, 144, 117, 1, 255, 41, 6, 1, 62,
 50, 133, 0, 1, 52, 42, 255, 74, 7, 2, 154, 125, 1, 165, 0,
 1, 215, 173, 255, 110, 7, 1, 20, 16, 208, 0, 243, 0, 4, 12,
 10, 0, 18, 15, 157, 0, 5, 26, 21, 0, 255, 207, 2, 255, 2,
 6, 2, 107, 87, 1, 171, 0, 1, 15, 12, 255, 59, 10, 255, 2,
 21, 7, 207, 169, 1, 100, 81, 0, 13, 10, 142, 0, 2, 201, 162,
 1, 255, 32, 6, 255, 125, 3, 132, 0, 1, 103, 84, 255, 74, 7,
 1, 95, 77, 163, 0, 4, 5, 4, 0, 250, 203, 255, 101, 7, 128,
 2, 208, 0, 243, 0, 4, 29, 23, 0, 78, 63, 157, 0, 128, 3,
 5, 0, 241, 195, 2, 255, 207, 255, 2, 4, 2, 209, 169, 1, 171,
 0, 128, 1, 2, 0, 235, 191, 255, 59, 7, 255, 2, 24, 10, 252,
 205, 2, 217, 176, 2, 115, 93, 1, 19, 16, 255, 53, 10, 255, 29,
 9, 1, 7, 5, 133, 0, 2, 169, 136, 1, 255, 17, 6, 1, 45,
 36, 163, 0, 1, 25, 20, 255, 68, 7, 1, 209, 168, 255, 179, 46,
 165, 0, 243, 0, 5, 51, 41, 0, 142, 117, 1, 159, 0, 13, 199,
 161, 1, 255, 207, 2, 255, 207, 2, 246, 201, 2, 8, 6, 172, 0,
 1, 136, 111, 255, 59, 7, 255, 2, 30, 14, 252, 205, 2, 237, 191,
 2, 131, 107, 1, 33, 27, 0, 22, 17, 0, 255, 20, 6, 3, 225,
 182, 2, 1, 255, 157, 8, 1, 225, 184, 255, 41, 7, 1, 15, 12,
 163, 0, 1, 63, 52, 255, 68, 7, 1, 136, 109, 255, 224, 34, 177,
 0, 243, 0, 5, 82, 67, 0, 219, 178, 2, 159, 0, 5, 90, 73,
 0, 255, 207, 2, 255, 2, 6, 1, 37, 30, 172, 0, 1, 63, 52,
 255, 59, 10, 255, 2, 36, 5, 237, 191, 2, 209, 169, 1, 255, 11,
 6, 2, 184, 149, 1, 132, 0, 4, 5, 4, 0, 248, 203, 255, 32,
 7, 0, 2, 255, 19, 7, 157, 0, 1, 122, 100, 255, 68, 7, 1,
 72, 58, 211, 0, 243, 0, 7, 122, 100, 1, 250, 203, 2, 13, 10,
 157, 0, 5, 46, 37, 0, 255, 207, 2, 255, 2, 6, 2, 115, 93,
 1, 171, 0, 4, 10, 8, 0, 250, 205, 255, 59, 7, 255, 2, 48,
 1, 118, 96, 255, 107, 7, 1, 21, 17, 255, 128, 7, 2, 225, 182,
 2, 165, 0, 2, 191, 156, 1, 255, 50, 6, 1, 31, 25, 211, 0,
 243, 0, 7, 173, 139, 1, 255, 207, 2, 29, 23, 157, 0, 128, 4,
 5, 0, 246, 199, 2, 255, 207, 255, 2, 4, 2, 191, 156, 1, 174,
 0, 1, 211, 171, 255, 59, 7, 255, 2, 48, 1, 65, 53, 133, 0,
 2, 48, 39, 0, 255, 17, 6, 1, 146, 118, 255, 125, 37, 4, 2,
 1, 0, 235, 191, 255, 68, 7, 1, 7, 6, 211, 0, 243, 0, 8,
 213, 173, 2, 255, 207, 2, 108, 88, 1, 159, 0, 4, 211, 171, 1,
 255, 207, 255, 44, 4, 4, 252, 205, 2, 8, 6, 172, 0, 1, 108,
 87, 255, 59, 7, 255, 62, 6, 255, 11, 9, 255, 2, 33, 1, 31,
 25, 133, 0, 2, 96, 78, 0, 255, 17, 6, 1, 87, 71, 163, 0,
 1, 12, 9, 255, 47, 7, 2, 237, 191, 2, 213, 0, 240, 0, 128,
 1, 9, 0, 231, 187, 2, 255, 207, 2, 168, 136, 1, 159, 0, 4,
 99, 80, 0, 255, 207, 255, 44, 4, 255, 2, 3, 1, 35, 28, 172,
 0, 255, 47, 3, 255, 59, 9, 5, 203, 166, 1, 149, 121, 1, 255,
 119, 6, 255, 2, 33, 1, 9, 7, 133, 0, 2, 154, 124, 1, 255,
 17, 6, 1, 45, 36, 163, 0, 1, 40, 32, 255, 113, 7, 1, 166,
 135, 255, 218, 34, 180, 0, 240, 0, 128, 4, 11, 0, 250, 203, 2,
 255, 207, 2, 241, 197, 2, 1, 1, 157, 0, 4, 46, 38, 0, 255,
 207, 255, 44, 4, 255, 2, 3, 1, 127, 103, 255, 45, 32, 140, 0,
 4, 4, 3, 0, 241, 195, 255, 59, 7, 14, 227, 184, 2, 5, 4,
 0, 28, 23, 0, 107, 86, 1, 215, 175, 2, 255, 128, 6, 255, 2,
 21, 1, 233, 189, 255, 149, 10, 2, 209, 168, 1, 255, 20, 6, 1,
 15, 12, 163, 0, 1, 87, 72, 255, 173, 7, 1, 100, 81, 214, 0,
 240, 0, 5, 14, 11, 0, 255, 207, 2, 255, 2, 6, 1, 24, 19,
 157, 0, 4, 5, 4, 0, 250, 203, 255, 44, 7, 2, 197, 159, 1,
 174, 0, 2, 161, 131, 1, 255, 59, 6, 4, 252, 205, 2, 11, 9,
 133, 0, 10, 10, 8, 0, 78, 63, 0, 180, 146, 1, 248, 201, 255,
 89, 7, 255, 2, 12, 255, 101, 9, 128, 2, 2, 0, 241, 197, 255,
 20, 7, 1, 3, 2, 163, 0, 1, 152, 124, 255, 110, 7, 1, 49,
 40, 214, 0, 240, 0, 5, 31, 25, 0, 255, 207, 2, 255, 2, 6,
 1, 51, 41, 160, 0, 2, 205, 164, 1, 255, 44, 6, 4, 252, 205,
 2, 10, 9, 172, 0, 1, 79, 65, 255, 107, 10, 1, 80, 65, 139,
 0, 10, 4, 3, 0, 39, 31, 0, 142, 117, 1, 221, 178, 255, 32,
 7, 255, 2, 6, 2, 135, 109, 1, 132, 0, 1, 11, 9, 255, 59,
 7, 2, 227, 184, 2, 165, 0, 1, 213, 171, 255, 68, 7, 1, 17,
 13, 157, 0, 10, 10, 8, 0, 38, 31, 0, 40, 33, 0, 13, 10,
 172, 0, 240, 0, 5, 55, 44, 0, 255, 207, 2, 255, 2, 6, 2,
 154, 125, 1, 159, 0, 2, 115, 93, 1, 255, 47, 9, 1, 36, 30,
 172, 0, 4, 11, 9, 0, 252, 205, 255, 59, 7, 1, 171, 139, 255,
 107, 22, 10, 17, 14, 0, 78, 63, 0, 191, 156, 1, 244, 199, 255,
 38, 4, 1, 81, 66, 133, 0, 1, 35, 29, 255, 167, 7, 1, 162,
 131, 255, 164, 34, 129, 0, 3, 4, 3, 0, 246, 255, 65, 5, 255,
 2, 3, 0, 2, 255, 49, 26, 10, 16, 13, 0, 138, 111, 1, 231,
 187, 2, 252, 205, 255, 2, 4, 6, 225, 184, 2, 96, 78, 0, 1,
 255, 97, 37, 130, 0, 240, 0, 5, 81, 67, 0, 255, 207, 2, 255,
 2, 6, 2, 207, 169, 1, 159, 0, 1, 40, 33, 255, 47, 10, 1,
 135, 109, 255, 47, 34, 141, 0, 1, 223, 180, 255, 59, 7, 4, 244,
 197, 2, 6, 5, 151, 0, 128, 2, 8, 0, 39, 31, 0, 125, 103,
 1, 36, 29, 133, 0, 1, 70, 56, 255, 119, 7, 1, 107, 86, 255,
 116, 37, 1, 19, 15, 255, 47, 7, 1, 205, 166, 255, 47, 22, 7,
 4, 3, 0, 91, 73, 0, 239, 193, 255, 143, 7, 255, 2, 12, 1,
 127, 103, 255, 215, 40, 240, 0, 5, 118, 96, 1, 255, 207, 2, 255,
 2, 9, 1, 10, 8, 157, 0, 2, 9, 7, 0, 255, 44, 9, 2,
 201, 164, 1, 174, 0, 1, 101, 82, 255, 110, 10, 1, 47, 38, 169,
 0, 1, 117, 95, 255, 56, 7, 1, 60, 49, 163, 0, 1, 52, 42,
 255, 167, 7, 1, 139, 114, 255, 164, 19, 7, 25, 20, 0, 191, 156,
 1, 252, 205, 255, 248, 10, 255, 2, 12, 255, 23, 3, 1, 104, 84,
 255, 218, 37, 240, 0, 5, 159, 128, 1, 255, 207, 2, 255, 2, 9,
 1, 45, 36, 160, 0, 1, 193, 157, 255, 50, 10, 1, 14, 11, 172,
 0, 2, 41, 33, 0, 255, 59, 9, 2, 128, 104, 1, 168, 0, 1,
 180, 147, 255, 116, 7, 1, 28, 23, 163, 0, 1, 99, 80, 255, 104,
 7, 1, 81, 66, 142, 0, 4, 70, 56, 0, 237, 193, 255, 242, 10,
 255, 2, 21, 4, 246, 199, 2, 47, 39, 160, 0, 240, 0, 5, 211,
 171, 1, 255, 207, 2, 255, 2, 9, 1, 96, 78, 160, 0, 1, 128,
 104, 255, 50, 10, 1, 48, 39, 172, 0, 4, 2, 1, 0, 233, 189,
 255, 59, 7, 4, 241, 195, 2, 2, 1, 166, 0, 1, 219, 176, 255,
 53, 7, 1, 9, 8, 163, 0, 1, 168, 136, 255, 164, 7, 1, 36,
 29, 136, 0, 4, 5, 4, 0, 138, 111, 255, 239, 13, 255, 2, 27,
 4, 217, 176, 2, 6, 4, 157, 0, 240, 0, 4, 233, 189, 2, 255,
 207, 255, 2, 10, 2, 203, 166, 1, 159, 0, 2, 31, 25, 0, 255,
 47, 9, 1, 147, 121, 255, 47, 34, 141, 0, 2, 166, 135, 1, 255,
 62, 9, 1, 22, 18, 163, 0, 1, 3, 2, 255, 113, 7, 3, 244,
 197, 2, 1, 255, 112, 38, 1, 219, 176, 255, 98, 7, 1, 11, 9,
 133, 0, 4, 20, 16, 0, 195, 159, 255, 122, 10, 255, 2, 36, 1,
 131, 107, 255, 108, 31, 237, 0, 7, 4, 3, 0, 244, 197, 2, 255,
 207, 255, 2, 10, 1, 239, 195, 129, 2, 157, 0, 4, 9, 7, 0,
 252, 205, 255, 47, 7, 3, 221, 178, 2, 1, 173, 0, 2, 59, 48,
 0, 255, 110, 9, 1, 117, 95, 255, 61, 37, 1, 13, 10, 255, 50,
 7, 1, 199, 162, 255, 109, 40, 255, 200, 3, 255, 47, 3, 1, 250,
 203, 255, 158, 7, 4, 55, 45, 0, 227, 184, 255, 233, 13, 255, 2,
 12, 255, 206, 3, 7, 233, 187, 2, 211, 171, 1, 250, 203, 255, 23,
 13, 4, 244, 199, 2, 18, 14, 154, 0, 237, 0, 5, 10, 8, 0,
 255, 207, 2, 255, 2, 15, 1, 38, 31, 160, 0, 2, 180, 146, 1,
 255, 47, 9, 1, 20, 16, 172, 0, 4, 9, 8, 0, 250, 205, 255,
 59, 7, 2, 207, 168, 1, 162, 0, 1, 34, 28, 255, 167, 7, 1,
 146, 118, 255, 47, 37, 129, 0, 1, 223, 180, 255, 47, 4, 10, 227,
 186, 2, 10, 8, 0, 117, 95, 1, 246, 201, 255, 230, 16, 255, 2,
 6, 19, 252, 205, 2, 182, 149, 1, 46, 37, 0, 5, 4, 0, 4,
 3, 0, 33, 27, 0, 205, 166, 255, 218, 10, 255, 2, 3, 1, 156,
 127, 255, 113, 28, 237, 0, 5, 23, 18, 0, 255, 207, 2, 255, 2,
 15, 1, 79, 63, 160, 0, 2, 115, 92, 1, 255, 47, 9, 1, 73,
 59, 175, 0, 1, 199, 162, 255, 62, 7, 4, 252, 205, 2, 19, 15,
 160, 0, 1, 63, 52, 255, 167, 7, 1, 95, 78, 166, 0, 0, 201,
 255, 98, 8, 1, 207, 168, 255, 170, 10, 255, 2, 15, 7, 233, 187,
 2, 69, 55, 0, 1, 1, 139, 0, 4, 8, 6, 0, 219, 178, 255,
 35, 10, 4, 248, 201, 2, 13, 10, 151, 0, 237, 0, 10, 40, 33,
 0, 255, 207, 2, 255, 207, 2, 244, 199, 255, 5, 4, 255, 2, 6,
 2, 186, 151, 1, 159, 0, 1, 25, 20, 255, 56, 7, 255, 2, 3,
 1, 175, 142, 255, 47, 34, 141, 0, 1, 81, 66, 255, 62, 10, 1,
 84, 68, 160, 0, 2, 105, 85, 1, 255, 44, 6, 1, 56, 45, 166,
 0, 1, 169, 138, 255, 50, 7, 255, 2, 21, 7, 252, 205, 2, 178,
 144, 1, 18, 15, 148, 0, 4, 16, 13, 0, 250, 203, 255, 44, 10,
 1, 109, 90, 255, 224, 25, 237, 0, 13, 63, 52, 0, 255, 207, 2,
 255, 207, 2, 187, 152, 1, 248, 203, 255, 8, 4, 255, 2, 3, 4,
 231, 187, 2, 3, 2, 157, 0, 4, 5, 4, 0, 244, 199, 255, 47,
 7, 1, 237, 191, 255, 47, 34, 141, 0, 1, 24, 20, 255, 119, 7,
 255, 2, 3, 2, 195, 159, 1, 159, 0, 2, 151, 122, 1, 255, 44,
 6, 1, 28, 22, 166, 0, 1, 128, 104, 255, 50, 7, 255, 2, 18,
 5, 246, 199, 2, 122, 99, 1, 255, 179, 30, 1, 62, 51, 255, 161,
 10, 3, 239, 193, 2, 1, 255, 163, 23, 237, 0, 14, 96, 79, 0,
 255, 207, 2, 255, 207, 2, 86, 70, 0, 199, 161, 1, 255, 11, 6,
 255, 2, 3, 1, 21, 17, 160, 0, 1, 164, 133, 255, 47, 10, 1,
 35, 29, 175, 0, 1, 215, 173, 255, 59, 7, 255, 2, 3, 1, 13,
 10, 157, 0, 1, 221, 178, 255, 41, 7, 1, 11, 9, 166, 0, 1,
 81, 66, 255, 218, 7, 255, 2, 15, 4, 237, 193, 2, 68, 55, 160,
 0, 4, 1, 0, 0, 182, 147, 255, 224, 10, 1, 45, 37, 148, 0,
 237, 0, 13, 136, 109, 1, 255, 207, 2, 255, 207, 2, 53, 43, 0,
 107, 87, 255, 11, 7, 255, 2, 3, 1, 86, 70, 160, 0, 2, 90,
 73, 0, 255, 47, 9, 1, 104, 84, 175, 0, 1, 127, 103, 255, 110,
 10, 1, 58, 47, 157, 0, 1, 244, 197, 255, 38, 4, 1, 241, 197,
 129, 2, 166, 0, 1, 33, 27, 255, 158, 10, 255, 2, 9, 4, 211,
 171, 1, 32, 26, 166, 0, 4, 22, 18, 0, 250, 203, 255, 56, 7,
 2, 151, 122, 1, 147, 0, 237, 0, 14, 175, 142, 1, 255, 207, 2,
 255, 207, 2, 31, 26, 0, 48, 39, 0, 255, 11, 6, 255, 2, 3,
 2, 141, 115, 1, 159, 0, 1, 16, 13, 255, 47, 10, 1, 209, 168,
 255, 47, 34, 141, 0, 4, 29, 23, 0, 252, 205, 255, 62, 7, 1,
 182, 147, 255, 62, 28, 4, 5, 4, 0, 250, 203, 255, 38, 4, 2,
 231, 187, 2, 168, 0, 4, 3, 2, 0, 248, 201, 255, 95, 7, 255,
 2, 6, 4, 176, 142, 1, 10, 9, 172, 0, 2, 173, 141, 1, 255,
 59, 6, 4, 246, 201, 2, 3, 3, 145, 0, 237, 0, 4, 225, 182,
 2, 255, 207, 255, 2, 4, 7, 16, 13, 0, 5, 4, 0, 244, 197,
 255, 14, 7, 4, 248, 201, 2, 2, 1, 157, 0, 128, 1, 2, 0,
 235, 191, 255, 47, 10, 1, 10, 8, 172, 0, 255, 95, 3, 1, 237,
 193, 255, 62, 10, 1, 6, 5, 151, 0, 2, 15, 12, 0, 255, 38,
 6, 1, 173, 141, 255, 147, 32, 140, 0, 2, 119, 97, 1, 255, 56,
 6, 7, 252, 205, 2, 131, 107, 1, 3, 3, 175, 0, 1, 41, 33,
 255, 122, 7, 255, 2, 3, 1, 40, 33, 145, 0, 237, 0, 4, 239,
 195, 2, 255, 207, 255, 2, 4, 1, 6, 5, 130, 0, 1, 225, 182,
 255, 14, 7, 4, 252, 205, 2, 15, 12, 160, 0, 2, 136, 111, 1,
 255, 47, 6, 255, 2, 3, 1, 58, 48, 175, 0, 1, 138, 112, 255,
 62, 10, 1, 60, 49, 151, 0, 2, 30, 24, 0, 255, 35, 6, 2,
 127, 103, 1, 171, 0, 10, 11, 9, 0, 241, 195, 2, 215, 175, 2,
 62, 51, 184, 0, 1, 227, 184, 255, 161, 7, 1, 125, 101, 255, 125,
 19, 234, 0, 128, 3, 11, 0, 248, 201, 2, 255, 207, 2, 252, 205,
 2, 1, 1, 130, 0, 4, 107, 86, 1, 255, 207, 255, 17, 4, 255,
 2, 3, 1, 81, 66, 160, 0, 2, 63, 51, 0, 255, 47, 9, 1,
 146, 118, 255, 66, 5, 170, 0, 1, 39, 31, 255, 62, 10, 1, 171,
 138, 255, 62, 25, 1, 48, 39, 255, 38, 7, 1, 84, 69, 175, 0,
 4, 10, 8, 0, 8, 6, 187, 0, 1, 99, 80, 255, 125, 7, 2,
 231, 187, 2, 144, 0, 234, 0, 11, 10, 8, 0, 255, 207, 2, 255,
 207, 2, 246, 199, 2, 132, 0, 1, 63, 52, 255, 17, 7, 255, 2,
 3, 2, 141, 114, 1, 159, 0, 4, 7, 6, 0, 252, 205, 255, 47,
 7, 3, 241, 195, 2, 2, 255, 49, 34, 139, 0, 4, 5, 4, 0,
 246, 201, 255, 62, 10, 255, 14, 3, 147, 0, 1, 71, 58, 255, 149,
 7, 1, 54, 43, 241, 0, 1, 19, 15, 255, 227, 10, 1, 13, 10,
 142, 0, 234, 0, 11, 22, 17, 0, 255, 207, 2, 255, 207, 2, 193,
 157, 1, 132, 0, 1, 9, 7, 255, 17, 7, 255, 2, 3, 3, 244,
 199, 2, 1, 255, 22, 7, 154, 0, 2, 205, 166, 1, 255, 50, 9,
 1, 24, 19, 175, 0, 1, 146, 118, 255, 62, 10, 1, 63, 52, 148,
 0, 1, 101, 81, 255, 35, 7, 1, 31, 25, 244, 0, 1, 217, 175,
 255, 161, 7, 1, 55, 45, 142, 0, 234, 0, 11, 40, 32, 0, 255,
 207, 2, 255, 207, 2, 149, 121, 1, 132, 0, 255, 6, 3, 1, 213,
 173, 255, 17, 4, 255, 2, 3, 4, 252, 205, 2, 19, 16, 160, 0,
 1, 99, 80, 255, 68, 7, 255, 2, 3, 1, 109, 88, 255, 71, 7,
 168, 0, 1, 59, 48, 255, 62, 10, 1, 152, 124, 255, 62, 22, 2,
 127, 104, 1, 255, 32, 6, 1, 18, 14, 244, 0, 1, 117, 95, 255,
 128, 7, 1, 130, 105, 255, 161, 16, 234, 0, 11, 63, 52, 0, 255,
 207, 2, 255, 207, 2, 109, 88, 1, 135, 0, 2, 149, 121, 1, 255,
 20, 6, 255, 2, 3, 1, 71, 57, 160, 0, 1, 35, 28, 255, 68,
 7, 255, 2, 3, 1, 191, 156, 255, 71, 10, 165, 0, 4, 5, 4,
 0, 241, 195, 255, 62, 7, 4, 250, 203, 2, 6, 5, 145, 0, 1,
 156, 127, 255, 146, 7, 1, 7, 6, 244, 0, 1, 40, 32, 255, 227,
 7, 1, 207, 168, 255, 224, 16, 234, 0, 10, 90, 72, 0, 255, 207,
 2, 255, 207, 2, 80, 66, 136, 0, 1, 52, 41, 255, 20, 7, 255,
 2, 3, 2, 169, 138, 1, 159, 0, 4, 2, 1, 0, 241, 195, 255,
 47, 7, 255, 2, 3, 1, 14, 11, 175, 0, 2, 161, 131, 1, 255,
 62, 9, 1, 55, 45, 145, 0, 1, 182, 149, 255, 32, 4, 4, 252,
 205, 2, 3, 2, 244, 0, 2, 5, 4, 0, 255, 158, 6, 1, 250,
 203, 255, 131, 16, 234, 0, 10, 119, 97, 1, 255, 207, 2, 255, 207,
 2, 53, 43, 136, 0, 2, 21, 17, 0, 255, 20, 6, 255, 2, 3,
 3, 233, 189, 2, 1, 161, 0, 1, 164, 133, 255, 71, 7, 255, 2,
 3, 1, 55, 45, 175, 0, 1, 61, 50, 255, 113, 10, 1, 156, 127,
 255, 112, 19, 1, 203, 166, 255, 95, 4, 2, 248, 201, 2, 249, 0,
 1, 203, 164, 255, 227, 7, 1, 21, 16, 139, 0, 234, 0, 10, 157,
 127, 1, 255, 207, 2, 255, 207, 2, 33, 27, 139, 0, 1, 213, 173,
 255, 20, 4, 255, 2, 6, 1, 31, 25, 160, 0, 2, 59, 48, 0,
 255, 47, 9, 2, 171, 138, 1, 174, 0, 4, 4, 3, 0, 241, 197,
 255, 62, 7, 4, 250, 203, 2, 9, 7, 142, 0, 1, 205, 166, 255,
 167, 4, 2, 227, 184, 2, 249, 0, 1, 124, 100, 255, 131, 4, 255,
 2, 3, 1, 57, 47, 139, 0, 234, 0, 10, 197, 161, 1, 255, 207,
 2, 255, 207, 2, 17, 14, 139, 0, 1, 157, 127, 255, 23, 7, 255,
 2, 3, 1, 69, 55, 160, 0, 4, 15, 12, 0, 252, 205, 255, 47,
 7, 4, 237, 191, 2, 2, 1, 175, 0, 1, 182, 147, 255, 113, 10,
 1, 55, 44, 142, 0, 1, 229, 186, 255, 23, 4, 2, 235, 189, 2,
 249, 0, 2, 59, 48, 0, 255, 158, 6, 1, 100, 81, 139, 0, 234,
 0, 10, 246, 199, 2, 255, 207, 2, 252, 205, 2, 7, 5, 139, 0,
 4, 49, 40, 0, 255, 207, 255, 26, 4, 255, 2, 3, 2, 187, 152,
 1, 162, 0, 2, 197, 161, 1, 255, 50, 9, 1, 45, 37, 175, 0,
 255, 113, 12, 1, 180, 146, 255, 113, 16, 1, 213, 173, 255, 23, 4,
 2, 217, 176, 2, 249, 0, 1, 23, 19, 255, 161, 7, 1, 141, 115,
 255, 158, 13, 231, 0, 128, 1, 11, 0, 252, 205, 2, 255, 207, 2,
 246, 199, 2, 2, 1, 139, 0, 3, 16, 13, 0, 250, 255, 26, 5,
 255, 2, 3, 1, 235, 191, 255, 29, 16, 147, 0, 2, 115, 93, 1,
 255, 47, 6, 255, 2, 3, 1, 107, 86, 255, 48, 35, 140, 0, 1,
 7, 5, 255, 140, 7, 255, 2, 3, 4, 250, 203, 2, 10, 8, 139,
 0, 1, 227, 184, 255, 23, 4, 2, 233, 189, 2, 249, 0, 2, 5,
 4, 0, 255, 158, 6, 1, 201, 164, 255, 221, 13, 231, 0, 11, 6,
 5, 0, 255, 207, 2, 255, 207, 2, 237, 193, 2, 144, 0, 1, 219,
 176, 255, 26, 4, 255, 2, 6, 1, 40, 33, 160, 0, 1, 25, 20,
 255, 77, 7, 255, 2, 3, 4, 239, 193, 2, 1, 1, 175, 0, 2,
 168, 136, 1, 255, 65, 9, 1, 43, 35, 139, 0, 1, 205, 166, 255,
 26, 4, 1, 227, 184, 255, 167, 19, 234, 0, 1, 231, 187, 255, 155,
 4, 1, 235, 191, 255, 134, 13, 231, 0, 11, 15, 12, 0, 255, 207,
 2, 255, 207, 2, 215, 175, 2, 144, 0, 2, 131, 107, 1, 255, 29,
 6, 255, 2, 3, 1, 87, 71, 160, 0, 128, 3, 2, 0, 237, 193,
 255, 47, 7, 4, 252, 205, 2, 17, 13, 175, 0, 1, 48, 39, 255,
 143, 7, 255, 2, 3, 1, 36, 29, 139, 0, 1, 201, 162, 255, 140,
 4, 1, 239, 195, 255, 167, 19, 234, 0, 1, 195, 159, 255, 134, 4,
 1, 250, 203, 129, 2, 136, 0, 231, 0, 11, 27, 22, 0, 255, 207,
 2, 255, 207, 2, 169, 138, 1, 144, 0, 1, 55, 45, 255, 29, 7,
 255, 2, 3, 2, 213, 173, 2, 162, 0, 2, 133, 108, 1, 255, 50,
 9, 1, 107, 87, 255, 83, 19, 156, 0, 4, 7, 5, 0, 250, 203,
 255, 62, 7, 1, 15, 13, 139, 0, 1, 178, 144, 255, 89, 4, 3,
 250, 205, 2, 1, 255, 85, 49, 202, 0, 1, 152, 124, 255, 224, 7,
 1, 9, 7, 136, 0, 231, 0, 11, 44, 35, 0, 255, 207, 2, 255,
 207, 2, 133, 107, 1, 144, 0, 4, 8, 7, 0, 250, 203, 255, 29,
 4, 255, 2, 3, 4, 241, 197, 2, 6, 5, 160, 0, 1, 60, 49,
 255, 80, 7, 255, 2, 3, 1, 199, 161, 255, 83, 19, 159, 0, 1,
 241, 195, 255, 59, 4, 255, 119, 3, 128, 3, 139, 0, 2, 159, 130,
 1, 255, 86, 6, 1, 3, 2, 250, 0, 1, 111, 90, 255, 134, 7,
 1, 17, 14, 136, 0, 231, 0, 10, 62, 51, 0, 255, 207, 2, 255,
 207, 2, 95, 78, 148, 0, 1, 221, 178, 255, 29, 4, 255, 2, 6,
 1, 55, 45, 160, 0, 4, 4, 3, 0, 244, 197, 255, 47, 7, 4,
 250, 203, 2, 15, 13, 172, 0, 255, 62, 3, 255, 53, 3, 255, 59,
 3, 2, 246, 199, 2, 141, 0, 2, 147, 119, 1, 255, 86, 6, 1,
 6, 5, 250, 0, 2, 87, 71, 0, 255, 134, 6, 1, 25, 21, 136,
 0, 231, 0, 10, 90, 73, 0, 255, 207, 2, 255, 207, 2, 68, 55,
 148, 0, 1, 101, 82, 255, 32, 7, 255, 2, 3, 2, 124, 101, 1,
 162, 0, 2, 195, 159, 1, 255, 50, 9, 0, 69, 255, 86, 23, 150,
 0, 1, 15, 12, 255, 110, 7, 1, 203, 164, 255, 107, 16, 1, 133,
 108, 255, 86, 7, 1, 14, 11, 250, 0, 1, 65, 52, 255, 161, 7,
 1, 33, 27, 136, 0, 231, 0, 10, 121, 99, 1, 255, 207, 2, 255,
 207, 2, 45, 36, 148, 0, 2, 51, 41, 0, 255, 32, 6, 255, 2,
 3, 3, 229, 186, 2, 1, 161, 0, 1, 62, 51, 255, 50, 10, 1,
 182, 149, 255, 49, 36, 136, 0, 1, 34, 28, 255, 59, 7, 1, 141,
 114, 255, 56, 16, 1, 104, 85, 255, 170, 7, 1, 24, 19, 250, 0,
 1, 55, 44, 255, 161, 7, 1, 42, 34, 136, 0, 231, 0, 10, 161,
 131, 1, 255, 207, 2, 255, 207, 2, 28, 23, 148, 0, 4, 3, 2,
 0, 241, 195, 255, 32, 4, 255, 2, 3, 4, 252, 205, 2, 17, 13,
 160, 0, 2, 19, 16, 0, 255, 47, 6, 255, 2, 3, 4, 250, 203,
 2, 9, 7, 169, 0, 1, 60, 49, 255, 59, 7, 1, 95, 78, 142,
 0, 1, 80, 65, 255, 26, 7, 1, 41, 33, 250, 0, 1, 46, 38,
 255, 134, 7, 1, 48, 39, 136, 0, 231, 0, 10, 195, 159, 1, 255,
 207, 2, 255, 207, 2, 15, 12, 151, 0, 1, 203, 164, 255, 35, 7,
 255, 2, 3, 1, 79, 63, 163, 0, 1, 195, 157, 255, 50, 10, 1,
 55, 45, 169, 0, 2, 93, 77, 0, 255, 53, 6, 1, 60, 49, 142,
 0, 1, 61, 50, 255, 26, 7, 1, 67, 54, 250, 0, 1, 41, 33,
 255, 134, 7, 1, 50, 40, 136, 0, 231, 0, 4, 233, 189, 2, 255,
 207, 255, 2, 4, 1, 7, 6, 151, 0, 2, 75, 61, 0, 255, 35,
 6, 255, 2, 3, 2, 169, 138, 1, 162, 0, 2, 111, 90, 1, 255,
 50, 9, 1, 178, 146, 255, 50, 37, 132, 0, 1, 141, 114, 255, 56,
 7, 1, 33, 27, 142, 0, 1, 42, 34, 255, 134, 7, 1, 101, 82,
 250, 0, 1, 40, 32, 255, 134, 7, 1, 50, 40, 136, 0, 231, 0,
 7, 255, 207, 2, 255, 207, 2, 244, 197, 129, 2, 151, 0, 1, 33,
 27, 255, 38, 7, 255, 2, 3, 4, 246, 201, 2, 4, 3, 160, 0,
 4, 19, 16, 0, 252, 205, 255, 50, 7, 3, 235, 191, 2, 3, 255,
 50, 35, 132, 0, 2, 199, 162, 1, 255, 53, 6, 1, 13, 10, 142,
 0, 1, 22, 17, 255, 134, 7, 2, 154, 124, 1, 249, 0, 1, 45,
 36, 255, 134, 7, 1, 51, 41, 136, 0, 228, 0, 11, 3, 2, 0,
 255, 207, 2, 255, 207, 2, 229, 186, 2, 153, 0, 4, 1, 0, 0,
 229, 186, 255, 38, 4, 255, 2, 6, 1, 40, 32, 160, 0, 128, 1,
 255, 50, 13, 1, 61, 50, 166, 0, 1, 237, 193, 255, 47, 4, 4,
 250, 203, 2, 3, 3, 142, 0, 1, 9, 7, 255, 173, 7, 2, 207,
 168, 1, 249, 0, 1, 55, 45, 255, 134, 7, 1, 40, 32, 136, 0,
 228, 0, 11, 9, 7, 0, 255, 207, 2, 255, 207, 2, 215, 173, 2,
 156, 0, 2, 164, 133, 1, 255, 41, 6, 255, 2, 3, 2, 118, 96,
 1, 162, 0, 1, 119, 96, 255, 50, 10, 1, 144, 117, 255, 50, 37,
 4, 3, 2, 0, 248, 201, 255, 47, 4, 1, 246, 199, 255, 143, 19,
 128, 2, 2, 0, 250, 203, 255, 29, 7, 0, 2, 255, 79, 37, 211,
 0, 2, 68, 55, 0, 255, 212, 6, 1, 36, 29, 136, 0, 228, 0,
 11, 21, 17, 0, 255, 207, 2, 255, 207, 2, 169, 136, 1, 156, 0,
 1, 53, 42, 255, 41, 7, 255, 2, 3, 2, 211, 171, 2, 255, 45,
 31, 131, 0, 1, 30, 24, 255, 50, 10, 4, 246, 199, 2, 6, 5,
 160, 0, 1, 11, 9, 255, 50, 7, 1, 203, 164, 255, 97, 22, 1,
 215, 173, 255, 80, 7, 1, 16, 13, 247, 0, 1, 86, 70, 255, 164,
 7, 1, 28, 23, 136, 0, 228, 0, 11, 36, 29, 0, 255, 207, 2,
 255, 207, 2, 135, 109, 1, 156, 0, 1, 15, 12, 255, 41, 7, 255,
 2, 6, 1, 16, 13, 160, 0, 4, 3, 2, 0, 235, 191, 255, 50,
 10, 1, 46, 38, 160, 0, 1, 27, 22, 255, 101, 7, 1, 149, 121,
 255, 143, 22, 2, 171, 139, 1, 255, 32, 6, 1, 51, 41, 247, 0,
 1, 115, 93, 255, 131, 7, 1, 18, 14, 136, 0, 228, 0, 11, 54,
 44, 0, 255, 207, 2, 255, 207, 2, 105, 86, 1, 159, 0, 2, 203,
 164, 1, 255, 44, 6, 255, 2, 3, 1, 77, 62, 163, 0, 1, 121,
 99, 255, 50, 10, 1, 149, 121, 255, 98, 34, 1, 51, 41, 255, 143,
 7, 1, 100, 81, 148, 0, 1, 104, 84, 255, 80, 7, 1, 112, 91,
 255, 77, 34, 213, 0, 1, 152, 124, 255, 131, 7, 1, 9, 7, 136,
 0, 228, 0, 10, 73, 58, 0, 255, 207, 2, 255, 207, 2, 79, 65,
 160, 0, 2, 115, 93, 1, 255, 44, 6, 255, 2, 3, 2, 180, 146,
 1, 162, 0, 1, 47, 38, 255, 95, 7, 255, 2, 3, 4, 246, 199,
 2, 5, 4, 157, 0, 1, 84, 68, 255, 47, 7, 1, 66, 54, 148,
 0, 1, 61, 50, 255, 32, 7, 1, 201, 162, 255, 128, 37, 210, 0,
 2, 197, 159, 1, 255, 131, 6, 1, 3, 2, 136, 0, 228, 0, 10,
 97, 80, 0, 255, 207, 2, 255, 207, 2, 56, 46, 160, 0, 1, 31,
 25, 255, 44, 7, 255, 2, 3, 4, 241, 197, 2, 6, 5, 160, 0,
 128, 1, 2, 0, 225, 184, 255, 50, 7, 255, 2, 3, 1, 41, 33,
 157, 0, 2, 121, 97, 1, 255, 41, 6, 1, 38, 31, 148, 0, 1,
 22, 18, 255, 131, 7, 4, 244, 197, 2, 6, 4, 193, 0, 4, 5,
 4, 0, 14, 11, 172, 0, 1, 239, 195, 255, 128, 4, 2, 235, 191,
 2, 138, 0, 228, 0, 10, 128, 105, 1, 255, 207, 2, 255, 207, 2,
 37, 30, 160, 0, 4, 4, 3, 0, 239, 195, 255, 44, 4, 255, 2,
 6, 1, 48, 39, 163, 0, 1, 159, 128, 255, 98, 7, 255, 2, 3,
 2, 171, 138, 1, 156, 0, 1, 166, 136, 255, 44, 7, 1, 19, 16,
 255, 131, 25, 1, 252, 205, 255, 77, 7, 255, 179, 36, 156, 0, 10,
 72, 58, 0, 205, 166, 1, 248, 201, 2, 63, 52, 166, 0, 2, 6,
 5, 0, 255, 125, 6, 1, 191, 156, 255, 203, 13, 228, 0, 10, 161,
 130, 1, 255, 207, 2, 255, 207, 2, 19, 16, 163, 0, 1, 156, 127,
 255, 47, 7, 255, 2, 3, 2, 131, 107, 1, 162, 0, 2, 33, 27,
 0, 255, 50, 9, 4, 237, 191, 2, 3, 3, 154, 0, 1, 225, 184,
 255, 38, 4, 4, 252, 205, 2, 7, 5, 151, 0, 1, 211, 171, 255,
 77, 7, 1, 122, 100, 255, 128, 37, 147, 0, 7, 8, 6, 0, 144,
 117, 1, 252, 205, 255, 74, 7, 4, 248, 201, 2, 8, 7, 163, 0,
 1, 22, 18, 255, 206, 7, 1, 146, 118, 255, 125, 13, 228, 0, 10,
 187, 152, 1, 255, 207, 2, 255, 207, 2, 11, 9, 163, 0, 2, 67,
 54, 0, 255, 47, 6, 255, 2, 3, 4, 233, 189, 2, 1, 1, 160,
 0, 4, 4, 3, 0, 239, 193, 255, 50, 7, 255, 2, 3, 1, 58,
 47, 154, 0, 1, 241, 197, 255, 35, 4, 2, 248, 201, 2, 153, 0,
 1, 127, 103, 255, 179, 7, 1, 235, 191, 255, 128, 37, 144, 0, 4,
 17, 14, 0, 201, 162, 255, 68, 7, 255, 2, 9, 1, 86, 70, 163,
 0, 2, 53, 43, 0, 255, 47, 6, 1, 92, 74, 139, 0, 228, 0,
 10, 223, 182, 2, 255, 207, 2, 252, 205, 2, 5, 4, 163, 0, 4,
 11, 9, 0, 252, 205, 255, 50, 4, 255, 2, 6, 1, 25, 21, 163,
 0, 2, 142, 115, 1, 255, 50, 9, 2, 166, 135, 1, 150, 0, 255,
 131, 3, 1, 250, 203, 255, 35, 4, 2, 229, 186, 2, 153, 0, 2,
 55, 45, 0, 255, 77, 9, 1, 33, 27, 175, 0, 4, 36, 30, 0,
 221, 178, 255, 194, 10, 255, 2, 9, 1, 191, 156, 255, 152, 25, 138,
 0, 1, 96, 78, 255, 125, 7, 1, 52, 42, 139, 0, 228, 0, 10,
 255, 207, 2, 255, 207, 2, 235, 191, 2, 2, 1, 166, 0, 2, 201,
 162, 1, 255, 53, 6, 255, 2, 3, 1, 111, 90, 255, 54, 37, 1,
 36, 29, 255, 104, 7, 255, 2, 3, 4, 244, 197, 2, 6, 5, 148,
 0, 1, 15, 13, 255, 38, 7, 1, 175, 141, 255, 86, 28, 1, 11,
 9, 255, 77, 10, 1, 125, 101, 255, 183, 41, 131, 0, 4, 68, 55,
 0, 239, 195, 255, 59, 7, 255, 2, 18, 1, 4, 3, 160, 0, 1,
 154, 125, 255, 254, 7, 1, 22, 17, 139, 0, 225, 0, 128, 3, 9,
 0, 255, 207, 2, 255, 207, 2, 223, 180, 2, 168, 0, 1, 95, 77,
 255, 53, 7, 255, 2, 3, 2, 199, 161, 1, 162, 0, 4, 4, 3,
 0, 239, 193, 255, 50, 7, 255, 2, 3, 1, 77, 61, 148, 0, 1,
 32, 27, 255, 89, 7, 1, 128, 104, 255, 86, 31, 1, 217, 175, 255,
 74, 7, 4, 235, 191, 2, 4, 3, 166, 0, 4, 95, 78, 0, 250,
 203, 255, 134, 10, 255, 2, 18, 1, 18, 14, 160, 0, 1, 209, 168,
 255, 41, 4, 4, 252, 205, 2, 5, 4, 139, 0, 225, 0, 11, 9,
 7, 0, 255, 207, 2, 255, 207, 2, 211, 171, 2, 168, 0, 4, 28,
 22, 0, 252, 205, 255, 53, 4, 255, 2, 6, 1, 21, 17, 163, 0,
 2, 124, 100, 1, 255, 50, 9, 2, 169, 138, 1, 147, 0, 1, 56,
 45, 255, 143, 7, 1, 86, 70, 157, 0, 1, 117, 95, 255, 77, 10,
 255, 44, 33, 129, 0, 128, 2, 5, 0, 119, 97, 1, 250, 203, 255,
 182, 10, 255, 2, 21, 1, 27, 22, 157, 0, 128, 1, 255, 161, 7,
 2, 225, 182, 2, 255, 195, 15, 225, 0, 11, 15, 13, 0, 255, 207,
 2, 255, 207, 2, 175, 142, 1, 168, 0, 255, 42, 3, 1, 229, 186,
 255, 53, 4, 255, 2, 6, 1, 70, 56, 163, 0, 1, 44, 36, 255,
 107, 7, 255, 2, 3, 4, 252, 205, 2, 16, 13, 145, 0, 1, 86,
 70, 255, 35, 7, 1, 55, 45, 157, 0, 1, 36, 30, 255, 77, 10,
 3, 209, 169, 1, 2, 255, 190, 29, 7, 5, 4, 0, 166, 135, 1,
 250, 205, 255, 179, 10, 255, 2, 9, 8, 246, 199, 2, 131, 107, 1,
 144, 118, 1, 255, 14, 6, 1, 31, 25, 157, 0, 1, 11, 9, 255,
 119, 7, 1, 187, 152, 255, 114, 16, 225, 0, 11, 26, 21, 0, 255,
 207, 2, 255, 207, 2, 141, 114, 1, 171, 0, 2, 139, 114, 1, 255,
 56, 6, 255, 2, 3, 1, 197, 161, 255, 59, 37, 128, 1, 2, 0,
 221, 178, 255, 50, 7, 255, 2, 3, 1, 84, 68, 145, 0, 1, 125,
 101, 255, 86, 7, 1, 32, 26, 157, 0, 128, 3, 2, 0, 239, 195,
 255, 77, 10, 1, 101, 82, 151, 0, 4, 14, 12, 0, 193, 157, 255,
 173, 10, 255, 2, 12, 4, 239, 193, 2, 57, 46, 130, 0, 2, 86,
 70, 0, 255, 17, 6, 1, 28, 23, 157, 0, 255, 152, 3, 255, 41,
 6, 1, 127, 103, 255, 245, 16, 225, 0, 11, 43, 35, 0, 255, 207,
 2, 255, 207, 2, 108, 88, 1, 171, 0, 1, 41, 33, 255, 56, 7,
 255, 2, 3, 4, 246, 199, 2, 6, 5, 163, 0, 2, 128, 104, 1,
 255, 53, 9, 1, 199, 162, 255, 113, 19, 1, 171, 139, 255, 32, 7,
 1, 14, 12, 160, 0, 1, 146, 118, 255, 77, 10, 3, 237, 191, 2,
 15, 255, 50, 20, 4, 37, 30, 0, 215, 175, 255, 35, 7, 255, 2,
 15, 4, 209, 168, 1, 23, 19, 133, 0, 1, 124, 101, 255, 74, 7,
 1, 20, 16, 157, 0, 1, 66, 54, 255, 248, 7, 1, 73, 59, 142,
 0, 225, 0, 10, 61, 49, 0, 255, 207, 2, 255, 207, 2, 81, 66,
 172, 0, 4, 6, 5, 0, 244, 199, 255, 56, 4, 255, 62, 45, 1,
 29, 24, 255, 110, 7, 255, 2, 6, 1, 28, 22, 142, 0, 1, 221,
 180, 255, 23, 4, 4, 252, 205, 2, 5, 4, 160, 0, 3, 31, 25,
 0, 252, 255, 77, 11, 2, 205, 166, 1, 255, 53, 15, 4, 68, 55,
 0, 241, 197, 255, 32, 10, 255, 2, 12, 4, 187, 152, 1, 14, 12,
 136, 0, 2, 178, 146, 1, 255, 23, 6, 1, 9, 7, 157, 0, 1,
 112, 91, 255, 41, 7, 1, 39, 31, 142, 0, 225, 0, 10, 81, 67,
 0, 255, 207, 2, 255, 207, 2, 60, 49, 175, 0, 2, 159, 130, 1,
 255, 59, 6, 255, 2, 3, 2, 157, 127, 1, 165, 0, 1, 213, 173,
 255, 53, 10, 1, 97, 79, 142, 0, 1, 241, 197, 255, 23, 4, 2,
 248, 201, 2, 162, 0, 255, 114, 3, 1, 215, 173, 255, 74, 7, 255,
 2, 6, 16, 207, 168, 1, 30, 25, 0, 2, 2, 0, 8, 7, 0,
 125, 101, 1, 246, 199, 255, 29, 13, 255, 2, 9, 4, 176, 142, 1,
 6, 5, 139, 0, 255, 41, 6, 255, 5, 3, 128, 2, 157, 0, 1,
 162, 131, 255, 191, 7, 1, 15, 12, 142, 0, 225, 0, 10, 107, 87,
 1, 255, 207, 2, 255, 207, 2, 44, 35, 175, 0, 2, 71, 56, 0,
 255, 59, 6, 255, 2, 3, 4, 250, 203, 2, 10, 8, 163, 0, 1,
 114, 92, 255, 113, 7, 255, 2, 3, 1, 233, 189, 129, 2, 136, 0,
 4, 4, 3, 0, 250, 203, 255, 23, 4, 1, 229, 186, 255, 21, 11,
 155, 0, 1, 93, 75, 255, 131, 10, 255, 2, 6, 7, 237, 193, 2,
 203, 164, 1, 221, 180, 255, 20, 13, 255, 2, 9, 6, 252, 205, 2,
 149, 121, 1, 3, 255, 94, 14, 255, 14, 3, 1, 250, 205, 255, 59,
 7, 159, 0, 1, 225, 182, 255, 41, 4, 4, 248, 201, 2, 3, 3,
 142, 0, 225, 0, 10, 138, 112, 1, 255, 207, 2, 255, 207, 2, 29,
 23, 175, 0, 4, 7, 6, 0, 246, 201, 255, 59, 4, 255, 2, 6,
 1, 45, 36, 163, 0, 4, 14, 11, 0, 252, 205, 255, 53, 10, 1,
 38, 31, 136, 0, 2, 12, 10, 0, 255, 20, 6, 2, 175, 142, 1,
 165, 0, 4, 5, 4, 0, 237, 193, 255, 77, 10, 255, 2, 30, 4,
 250, 205, 2, 124, 100, 129, 1, 142, 0, 255, 119, 9, 1, 171, 139,
 255, 119, 31, 1, 2, 1, 255, 41, 7, 2, 221, 178, 2, 144, 0,
 225, 0, 10, 162, 131, 1, 255, 207, 2, 255, 207, 2, 17, 14, 178,
 0, 1, 199, 161, 255, 62, 7, 255, 2, 3, 2, 175, 141, 1, 165,
 0, 1, 203, 164, 255, 53, 10, 1, 139, 114, 255, 53, 10, 2, 27,
 22, 0, 255, 20, 6, 1, 131, 107, 255, 74, 40, 129, 0, 1, 86,
 70, 255, 53, 7, 255, 2, 30, 4, 252, 205, 2, 111, 90, 255, 86,
 22, 1, 34, 27, 255, 65, 7, 1, 121, 97, 255, 119, 31, 1, 13,
 10, 255, 41, 7, 1, 176, 142, 255, 41, 19, 225, 0, 10, 184, 149,
 1, 255, 207, 2, 255, 207, 2, 14, 12, 178, 0, 2, 67, 54, 0,
 255, 62, 6, 255, 2, 3, 4, 241, 195, 2, 4, 3, 163, 0, 1,
 78, 63, 255, 53, 10, 4, 252, 205, 2, 9, 7, 133, 0, 1, 49,
 40, 255, 23, 7, 1, 92, 75, 169, 0, 4, 6, 5, 0, 233, 189,
 255, 77, 7, 255, 2, 24, 4, 246, 199, 2, 78, 63, 151, 0, 1,
 62, 52, 255, 119, 7, 255, 35, 27, 132, 0, 1, 33, 27, 255, 41,
 7, 2, 118, 96, 1, 144, 0, 225, 0, 4, 219, 176, 2, 255, 207,
 255, 2, 4, 1, 27, 22, 178, 0, 2, 21, 16, 0, 255, 62, 6,
 255, 2, 6, 1, 54, 44, 163, 0, 4, 8, 6, 0, 246, 199, 255,
 53, 10, 1, 59, 48, 133, 0, 1, 74, 60, 255, 77, 7, 1, 63,
 52, 172, 0, 4, 39, 31, 0, 252, 205, 255, 80, 10, 255, 2, 15,
 4, 237, 193, 2, 56, 46, 154, 0, 1, 99, 81, 255, 119, 7, 1,
 40, 33, 157, 0, 255, 152, 3, 255, 41, 6, 1, 71, 57, 145, 0,
 225, 0, 4, 244, 197, 2, 255, 207, 255, 2, 4, 2, 127, 103, 1,
 180, 0, 2, 187, 152, 1, 255, 65, 6, 255, 2, 3, 1, 162, 133,
 255, 68, 40, 1, 184, 151, 255, 53, 10, 1, 191, 156, 255, 53, 7,
 1, 107, 86, 255, 20, 7, 1, 38, 31, 175, 0, 1, 122, 100, 255,
 80, 10, 255, 2, 12, 4, 219, 178, 2, 34, 27, 157, 0, 1, 149,
 121, 255, 59, 7, 1, 19, 15, 157, 0, 1, 109, 88, 255, 161, 28,
 222, 0, 5, 2, 1, 0, 255, 207, 2, 255, 2, 6, 3, 197, 161,
 1, 1, 179, 0, 2, 111, 90, 1, 255, 68, 9, 4, 239, 193, 2,
 4, 3, 163, 0, 1, 42, 34, 255, 122, 10, 255, 2, 3, 1, 22,
 18, 130, 0, 1, 146, 118, 255, 74, 7, 1, 21, 17, 175, 0, 7,
 3, 2, 0, 157, 128, 1, 252, 205, 255, 83, 10, 255, 212, 4, 3,
 159, 1, 17, 14, 160, 0, 1, 213, 173, 255, 119, 4, 255, 59, 3,
 1, 6, 5, 157, 0, 1, 159, 128, 255, 161, 7, 1, 14, 12, 145,
 0, 222, 0, 5, 6, 5, 0, 255, 207, 2, 255, 2, 9, 1, 15,
 12, 178, 0, 4, 13, 11, 0, 250, 203, 255, 68, 10, 1, 63, 51,
 163, 0, 4, 4, 3, 0, 241, 197, 255, 53, 10, 1, 97, 79, 130,
 0, 10, 189, 154, 1, 255, 207, 2, 252, 205, 2, 8, 7, 178, 0,
 7, 3, 2, 0, 99, 80, 0, 239, 195, 255, 65, 4, 7, 241, 195,
 2, 127, 103, 1, 7, 5, 163, 0, 1, 248, 201, 255, 131, 4, 255,
 143, 3, 159, 0, 1, 213, 171, 255, 41, 4, 255, 47, 3, 255, 191,
 3, 144, 0, 222, 0, 5, 13, 10, 0, 255, 207, 2, 255, 2, 9,
 1, 97, 79, 178, 0, 128, 1, 2, 0, 221, 178, 255, 68, 10, 2,
 141, 114, 1, 165, 0, 2, 131, 107, 1, 255, 53, 9, 7, 237, 193,
 2, 3, 3, 0, 235, 191, 255, 11, 4, 3, 250, 203, 2, 2, 255,
 67, 40, 145, 0, 7, 12, 10, 0, 40, 33, 0, 28, 23, 166, 0,
 4, 5, 4, 0, 252, 205, 255, 119, 4, 2, 215, 175, 2, 255, 233,
 33, 255, 173, 6, 1, 223, 180, 255, 41, 22, 222, 0, 7, 4, 3,
 0, 241, 195, 2, 255, 207, 255, 2, 7, 2, 173, 141, 1, 180, 0,
 2, 88, 72, 0, 255, 68, 9, 4, 252, 205, 2, 9, 7, 163, 0,
 4, 22, 18, 0, 250, 205, 255, 125, 10, 4, 70, 57, 0, 244, 197,
 255, 8, 4, 2, 246, 199, 2, 234, 0, 1, 18, 14, 255, 188, 7,
 1, 152, 124, 255, 254, 31, 255, 215, 3, 255, 41, 6, 1, 182, 147,
 255, 41, 22, 225, 0, 5, 211, 171, 1, 255, 207, 2, 255, 2, 6,
 4, 250, 203, 2, 16, 13, 178, 0, 2, 24, 19, 0, 255, 68, 9,
 255, 2, 3, 1, 47, 38, 163, 0, 128, 1, 2, 0, 221, 178, 255,
 53, 10, 255, 137, 3, 1, 252, 205, 255, 8, 4, 2, 209, 169, 1,
 234, 0, 1, 38, 31, 255, 188, 7, 1, 104, 84, 255, 119, 31, 1,
 27, 21, 255, 41, 7, 1, 128, 104, 255, 41, 22, 225, 0, 5, 73,
 59, 0, 255, 207, 2, 255, 2, 9, 1, 47, 38, 181, 0, 1, 207,
 168, 255, 68, 10, 2, 175, 142, 1, 165, 0, 1, 78, 62, 255, 125,
 13, 255, 2, 6, 1, 161, 131, 255, 62, 40, 195, 0, 1, 66, 54,
 255, 131, 7, 1, 65, 53, 157, 0, 1, 53, 43, 255, 41, 7, 1,
 81, 66, 148, 0, 225, 0, 7, 26, 21, 0, 252, 205, 2, 255, 207,
 255, 2, 7, 2, 178, 146, 1, 180, 0, 2, 95, 77, 0, 255, 68,
 9, 4, 246, 199, 2, 8, 6, 163, 0, 4, 10, 8, 0, 246, 199,
 255, 125, 10, 255, 2, 6, 1, 121, 97, 255, 131, 55, 180, 0, 1,
 103, 84, 255, 185, 7, 1, 36, 29, 157, 0, 1, 90, 73, 255, 41,
 7, 1, 45, 37, 148, 0, 225, 0, 7, 1, 0, 0, 223, 180, 2,
 255, 207, 255, 2, 7, 4, 235, 191, 2, 4, 3, 178, 0, 4, 26,
 21, 0, 252, 205, 255, 71, 10, 1, 66, 53, 166, 0, 2, 175, 142,
 1, 255, 53, 9, 255, 2, 6, 1, 86, 70, 235, 0, 1, 152, 124,
 255, 128, 7, 1, 17, 13, 157, 0, 1, 133, 108, 255, 41, 7, 1,
 22, 18, 148, 0, 228, 0, 5, 109, 90, 1, 255, 207, 2, 255, 2,
 9, 1, 50, 40, 181, 0, 1, 201, 162, 255, 71, 10, 2, 193, 156,
 1, 165, 0, 2, 33, 27, 0, 255, 125, 12, 255, 2, 3, 1, 58,
 47, 235, 0, 1, 205, 166, 255, 182, 4, 4, 252, 205, 2, 6, 5,
 157, 0, 1, 184, 149, 255, 224, 7, 0, 7, 255, 41, 23, 228, 0,
 5, 45, 37, 0, 255, 207, 2, 255, 2, 9, 2, 152, 124, 1, 180,
 0, 2, 111, 90, 1, 255, 68, 9, 4, 244, 199, 2, 9, 7, 163,
 0, 128, 2, 2, 0, 229, 186, 255, 125, 10, 255, 2, 3, 1, 36,
 29, 235, 0, 1, 237, 193, 255, 116, 4, 2, 248, 201, 2, 159, 0,
 1, 235, 191, 255, 41, 4, 255, 5, 3, 128, 1, 148, 0, 228, 0,
 128, 1, 5, 0, 223, 182, 2, 255, 207, 255, 2, 7, 4, 231, 187,
 2, 2, 1, 178, 0, 4, 12, 10, 0, 248, 203, 255, 71, 10, 1,
 100, 81, 166, 0, 2, 109, 88, 1, 255, 53, 9, 255, 2, 3, 1,
 21, 17, 232, 0, 4, 3, 2, 0, 248, 201, 255, 116, 4, 2, 223,
 182, 2, 156, 0, 2, 2, 1, 0, 255, 158, 6, 1, 213, 173, 255,
 41, 25, 231, 0, 5, 168, 136, 1, 255, 207, 2, 255, 2, 9, 1,
 55, 45, 181, 0, 1, 211, 171, 255, 68, 10, 2, 195, 159, 1, 165,
 0, 4, 10, 8, 0, 244, 199, 255, 56, 10, 1, 9, 7, 232, 0,
 2, 11, 9, 0, 255, 116, 6, 1, 166, 135, 255, 173, 31, 1, 12,
 10, 255, 41, 7, 1, 168, 138, 255, 41, 25, 231, 0, 5, 40, 32,
 0, 255, 207, 2, 255, 2, 9, 2, 119, 97, 1, 180, 0, 1, 74,
 60, 255, 71, 10, 4, 252, 205, 2, 24, 19, 166, 0, 2, 173, 141,
 1, 255, 53, 6, 4, 250, 203, 2, 3, 2, 232, 0, 1, 27, 22,
 255, 176, 7, 255, 242, 33, 1, 29, 23, 255, 41, 7, 1, 112, 91,
 255, 41, 25, 231, 0, 7, 8, 6, 0, 246, 199, 2, 255, 207, 255,
 2, 7, 255, 11, 3, 1, 6, 5, 178, 0, 4, 13, 10, 0, 252,
 205, 255, 71, 10, 2, 119, 97, 1, 165, 0, 2, 48, 39, 0, 255,
 50, 6, 2, 248, 201, 2, 234, 0, 1, 52, 42, 255, 119, 7, 1,
 80, 66, 157, 0, 1, 54, 44, 255, 41, 7, 1, 72, 58, 151, 0,
 234, 0, 5, 168, 136, 1, 255, 207, 2, 255, 2, 6, 4, 252, 205,
 2, 29, 23, 181, 0, 1, 186, 151, 255, 71, 10, 4, 215, 175, 2,
 1, 1, 163, 0, 255, 37, 3, 1, 217, 175, 255, 50, 4, 2, 223,
 182, 2, 234, 0, 2, 84, 68, 0, 255, 170, 6, 1, 46, 37, 157,
 0, 1, 87, 71, 255, 41, 7, 1, 41, 33, 151, 0, 234, 0, 5,
 55, 45, 0, 255, 207, 2, 255, 2, 9, 1, 92, 74, 255, 71, 70,
 1, 49, 40, 166, 0, 8, 101, 82, 1, 255, 207, 2, 178, 146, 1,
 234, 0, 1, 121, 97, 255, 116, 4, 255, 2, 3, 1, 25, 21, 157,
 0, 1, 136, 111, 255, 41, 7, 1, 19, 16, 151, 0, 234, 0, 5,
 13, 10, 0, 255, 207, 2, 255, 2, 9, 1, 70, 56, 181, 0, 1,
 10, 8, 255, 71, 13, 2, 136, 111, 1, 255, 128, 42, 4, 250, 203,
 2, 139, 114, 255, 47, 40, 195, 0, 2, 168, 136, 1, 255, 167, 6,
 255, 182, 3, 156, 0, 1, 184, 149, 255, 41, 7, 1, 6, 5, 151,
 0, 237, 0, 5, 154, 125, 1, 255, 207, 2, 255, 2, 6, 1, 55,
 45, 184, 0, 1, 142, 115, 255, 71, 10, 4, 246, 199, 2, 7, 6,
 166, 0, 5, 141, 114, 1, 105, 86, 1, 234, 0, 1, 221, 180, 255,
 164, 4, 4, 250, 203, 2, 3, 2, 157, 0, 1, 235, 191, 255, 41,
 4, 255, 5, 3, 128, 1, 151, 0, 237, 0, 5, 90, 73, 0, 255,
 207, 2, 255, 2, 6, 1, 43, 35, 184, 0, 1, 55, 45, 255, 71,
 10, 255, 2, 3, 1, 77, 62, 166, 0, 255, 116, 3, 0, 78, 255,
 44, 41, 195, 0, 1, 241, 197, 255, 161, 4, 2, 246, 199, 2, 156,
 0, 1, 2, 1, 255, 209, 7, 1, 213, 175, 255, 41, 28, 237, 0,
 7, 6, 5, 0, 237, 193, 2, 255, 207, 255, 2, 4, 1, 27, 21,
 184, 0, 4, 3, 2, 0, 231, 187, 255, 71, 7, 255, 2, 3, 2,
 182, 147, 1, 168, 0, 1, 36, 29, 232, 0, 4, 5, 4, 0, 250,
 203, 255, 161, 4, 1, 197, 161, 255, 161, 31, 2, 10, 8, 0, 255,
 203, 6, 1, 168, 138, 255, 41, 28, 240, 0, 10, 209, 168, 1, 255,
 207, 2, 255, 207, 2, 18, 14, 187, 0, 1, 115, 92, 255, 71, 7,
 255, 2, 6, 1, 28, 22, 253, 0, 147, 0, 2, 15, 13, 0, 255,
 158, 6, 2, 144, 118, 1, 156, 0, 1, 25, 21, 255, 41, 7, 1,
 114, 92, 255, 41, 28, 240, 0, 10, 173, 139, 1, 255, 207, 2, 255,
 207, 2, 11, 9, 187, 0, 2, 33, 27, 0, 255, 71, 6, 255, 2,
 6, 2, 108, 87, 1, 174, 0, 1, 10, 8, 223, 0, 1, 31, 25,
 255, 164, 7, 1, 103, 84, 157, 0, 1, 48, 40, 255, 41, 7, 1,
 79, 63, 154, 0, 240, 0, 10, 207, 168, 1, 255, 207, 2, 255, 207,
 2, 4, 3, 190, 0, 1, 195, 159, 255, 74, 7, 255, 2, 3, 4,
 227, 184, 2, 2, 1, 172, 0, 4, 178, 144, 1, 41, 33, 220, 0,
 2, 54, 44, 0, 255, 158, 6, 1, 69, 56, 157, 0, 1, 79, 63,
 255, 41, 7, 1, 46, 38, 154, 0, 240, 0, 4, 219, 178, 2, 255,
 207, 255, 2, 4, 128, 1, 190, 0, 2, 99, 80, 0, 255, 74, 6,
 255, 2, 6, 1, 60, 48, 172, 0, 7, 227, 184, 2, 250, 203, 2,
 55, 45, 217, 0, 1, 84, 68, 255, 161, 7, 1, 43, 35, 157, 0,
 2, 118, 96, 1, 255, 41, 6, 1, 25, 20, 154, 0, 240, 0, 4,
 229, 186, 2, 255, 207, 255, 2, 4, 192, 0, 4, 10, 8, 0, 248,
 203, 255, 77, 7, 255, 2, 3, 2, 156, 127, 1, 171, 0, 1, 237,
 193, 255, 53, 4, 4, 250, 203, 2, 73, 59, 214, 0, 2, 122, 99,
 1, 255, 155, 6, 1, 27, 22, 157, 0, 1, 161, 130, 255, 41, 7,
 255, 212, 3, 153, 0, 237, 0, 128, 2, 9, 0, 237, 191, 2, 255,
 207, 2, 213, 173, 1, 195, 0, 4, 151, 122, 1, 255, 207, 255, 80,
 4, 255, 2, 3, 4, 250, 203, 2, 15, 12, 166, 0, 4, 4, 3,
 0, 246, 199, 255, 56, 10, 1, 97, 79, 211, 0, 1, 162, 131, 255,
 158, 7, 1, 12, 9, 157, 0, 1, 209, 169, 255, 41, 4, 4, 244,
 197, 2, 3, 2, 154, 0, 237, 0, 11, 4, 3, 0, 252, 205, 2,
 255, 207, 2, 195, 159, 1, 195, 0, 4, 59, 49, 0, 255, 207, 255,
 80, 4, 255, 2, 6, 1, 109, 90, 255, 86, 40, 1, 8, 7, 255,
 56, 13, 255, 149, 3, 1, 125, 101, 255, 146, 70, 138, 0, 1, 215,
 175, 255, 95, 7, 1, 3, 2, 157, 0, 255, 38, 6, 1, 223, 182,
 255, 39, 31, 237, 0, 11, 8, 6, 0, 255, 207, 2, 255, 207, 2,
 168, 136, 1, 195, 0, 128, 1, 2, 0, 217, 176, 255, 80, 4, 255,
 2, 6, 255, 11, 3, 128, 1, 163, 0, 1, 19, 15, 255, 137, 7,
 255, 2, 9, 4, 252, 205, 2, 161, 131, 129, 1, 202, 0, 1, 235,
 191, 255, 92, 7, 156, 0, 1, 4, 3, 255, 140, 7, 1, 195, 159,
 255, 126, 31, 237, 0, 11, 14, 12, 0, 255, 207, 2, 255, 207, 2,
 142, 115, 1, 198, 0, 2, 115, 93, 1, 255, 83, 6, 255, 2, 6,
 1, 57, 46, 163, 0, 1, 31, 26, 255, 137, 7, 255, 2, 15, 4,
 193, 157, 1, 6, 5, 196, 0, 4, 3, 2, 0, 244, 197, 255, 83,
 4, 2, 217, 176, 2, 156, 0, 1, 14, 12, 255, 140, 7, 2, 144,
 118, 1, 156, 0, 237, 0, 11, 21, 16, 0, 255, 207, 2, 255, 207,
 2, 124, 101, 1, 198, 0, 4, 21, 17, 0, 252, 205, 255, 83, 4,
 255, 2, 6, 1, 161, 131, 255, 89, 37, 1, 48, 39, 255, 137, 7,
 255, 2, 18, 4, 221, 178, 2, 17, 13, 193, 0, 1, 8, 7, 255,
 98, 7, 1, 164, 135, 255, 146, 31, 1, 31, 25, 255, 41, 7, 1,
 101, 82, 255, 41, 31, 237, 0, 10, 34, 27, 0, 255, 207, 2, 255,
 207, 2, 95, 77, 202, 0, 2, 173, 141, 1, 255, 86, 6, 255, 2,
 3, 4, 246, 201, 2, 10, 9, 160, 0, 1, 68, 55, 255, 137, 7,
 255, 2, 21, 4, 241, 197, 2, 37, 31, 190, 0, 1, 21, 16, 255,
 98, 7, 2, 122, 100, 1, 156, 0, 1, 54, 44, 255, 41, 7, 255,
 149, 3, 156, 0, 237, 0, 10, 46, 37, 0, 255, 207, 2, 255, 207,
 2, 80, 65, 202, 0, 1, 66, 54, 255, 86, 7, 255, 2, 6, 2,
 121, 99, 1, 159, 0, 1, 96, 79, 255, 50, 13, 255, 2, 18, 4,
 250, 203, 2, 79, 63, 187, 0, 1, 37, 31, 255, 98, 7, 1, 88,
 72, 157, 0, 1, 84, 68, 255, 41, 7, 1, 41, 33, 157, 0, 237,
 0, 10, 62, 51, 0, 255, 207, 2, 255, 207, 2, 63, 52, 202, 0,
 4, 3, 2, 0, 233, 189, 255, 86, 4, 255, 2, 6, 4, 221, 178,
 2, 5, 4, 157, 0, 2, 125, 103, 1, 255, 44, 6, 1, 237, 193,
 255, 56, 10, 255, 2, 15, 5, 252, 205, 2, 133, 108, 1, 183, 0,
 1, 60, 49, 255, 236, 7, 1, 59, 49, 157, 0, 1, 118, 96, 255,
 140, 7, 1, 22, 18, 157, 0, 237, 0, 10, 77, 62, 0, 255, 207,
 2, 255, 207, 2, 45, 37, 205, 0, 1, 100, 81, 255, 89, 7, 255,
 2, 6, 1, 55, 44, 157, 0, 2, 159, 128, 1, 255, 41, 6, 4,
 68, 55, 0, 203, 164, 255, 11, 7, 255, 2, 18, 7, 252, 205, 2,
 184, 151, 1, 6, 5, 178, 0, 1, 88, 72, 255, 146, 7, 1, 36,
 29, 157, 0, 1, 161, 130, 255, 128, 7, 1, 9, 7, 157, 0, 237,
 0, 10, 92, 75, 0, 255, 207, 2, 255, 207, 2, 37, 30, 205, 0,
 1, 27, 22, 255, 89, 7, 255, 2, 6, 2, 211, 171, 1, 156, 0,
 2, 197, 161, 1, 255, 41, 6, 10, 3, 2, 0, 11, 9, 0, 175,
 141, 1, 252, 205, 255, 62, 10, 255, 2, 15, 4, 231, 187, 2, 25,
 21, 175, 0, 1, 122, 100, 255, 98, 7, 1, 19, 15, 157, 0, 1,
 203, 164, 255, 41, 4, 1, 244, 197, 129, 2, 157, 0, 237, 0, 10,
 118, 96, 1, 255, 207, 2, 255, 207, 2, 21, 16, 208, 0, 1, 173,
 139, 255, 92, 7, 255, 2, 3, 4, 252, 205, 2, 27, 22, 154, 0,
 1, 229, 186, 255, 41, 7, 132, 0, 128, 3, 5, 0, 141, 114, 1,
 246, 199, 255, 20, 7, 255, 2, 18, 4, 250, 203, 2, 73, 59, 172,
 0, 1, 162, 133, 255, 143, 7, 1, 8, 6, 157, 0, 255, 92, 3,
 255, 38, 3, 1, 223, 182, 255, 140, 7, 153, 0, 237, 0, 10, 131,
 107, 1, 255, 207, 2, 255, 207, 2, 16, 13, 208, 0, 2, 49, 40,
 0, 255, 92, 6, 255, 2, 6, 2, 136, 111, 1, 153, 0, 1, 237,
 193, 255, 35, 4, 2, 248, 203, 2, 138, 0, 4, 100, 81, 1, 233,
 189, 255, 65, 10, 255, 2, 15, 6, 252, 205, 2, 141, 115, 1, 2,
 255, 85, 28, 139, 0, 1, 211, 171, 255, 236, 7, 128, 1, 154, 0,
 255, 80, 3, 255, 38, 6, 1, 207, 168, 255, 90, 34, 237, 0, 10,
 164, 133, 1, 255, 207, 2, 255, 207, 2, 8, 7, 208, 0, 4, 3,
 2, 0, 239, 193, 255, 92, 4, 255, 2, 6, 4, 252, 205, 2, 14,
 12, 148, 0, 4, 4, 3, 0, 246, 199, 255, 35, 4, 2, 201, 164,
 1, 141, 0, 4, 57, 46, 0, 219, 178, 255, 68, 10, 255, 2, 18,
 2, 205, 166, 1, 255, 86, 24, 141, 0, 1, 233, 189, 255, 50, 7,
 156, 0, 2, 8, 7, 0, 255, 38, 6, 1, 161, 130, 255, 140, 16,
 144, 0, 237, 0, 10, 184, 147, 1, 255, 207, 2, 255, 207, 2, 4,
 3, 211, 0, 2, 84, 68, 0, 255, 95, 6, 255, 2, 6, 2, 104,
 85, 1, 147, 0, 1, 8, 6, 255, 38, 7, 1, 161, 130, 255, 32,
 19, 7, 25, 20, 0, 193, 157, 1, 252, 205, 255, 71, 10, 255, 2,
 15, 4, 244, 197, 2, 59, 48, 157, 0, 128, 2, 2, 0, 244, 197,
 255, 44, 4, 2, 211, 173, 2, 156, 0, 1, 20, 16, 255, 140, 7,
 1, 118, 95, 255, 173, 22, 138, 0, 237, 0, 4, 215, 173, 2, 255,
 207, 255, 2, 4, 1, 2, 1, 211, 0, 4, 10, 9, 0, 237, 191,
 255, 98, 7, 255, 2, 3, 3, 223, 180, 2, 1, 255, 104, 20, 2,
 15, 12, 0, 255, 32, 6, 1, 130, 107, 255, 135, 22, 7, 6, 5,
 0, 149, 121, 1, 244, 199, 255, 74, 10, 255, 2, 15, 5, 252, 205,
 2, 141, 115, 1, 255, 197, 27, 1, 7, 5, 255, 98, 7, 1, 168,
 136, 255, 234, 31, 1, 39, 31, 255, 41, 7, 1, 84, 68, 160, 0,
 237, 0, 4, 231, 187, 2, 255, 207, 255, 2, 4, 216, 0, 2, 159,
 128, 1, 255, 98, 6, 255, 2, 6, 1, 72, 58, 145, 0, 2, 27,
 22, 0, 255, 29, 6, 2, 101, 82, 1, 153, 0, 1, 96, 79, 255,
 176, 10, 255, 2, 21, 4, 215, 175, 1, 22, 18, 148, 0, 1, 17,
 14, 255, 98, 7, 1, 135, 108, 255, 98, 28, 129, 0, 1, 61, 50,
 255, 41, 7, 1, 55, 45, 160, 0, 237, 0, 4, 235, 191, 2, 255,
 207, 255, 2, 4, 153, 0, 1, 13, 10, 187, 0, 4, 23, 19, 0,
 252, 205, 255, 101, 7, 255, 2, 3, 1, 205, 166, 129, 1, 142, 0,
 2, 42, 34, 0, 255, 29, 6, 1, 79, 65, 157, 0, 4, 41, 33,
 0, 207, 168, 255, 77, 10, 255, 2, 18, 4, 250, 203, 2, 85, 69,
 145, 0, 1, 31, 26, 255, 98, 7, 1, 97, 79, 157, 0, 1, 91,
 73, 255, 41, 7, 1, 33, 27, 160, 0, 234, 0, 11, 2, 1, 0,
 244, 197, 2, 255, 207, 2, 205, 166, 1, 153, 0, 4, 65, 52, 0,
 6, 5, 187, 0, 4, 180, 147, 1, 255, 207, 255, 104, 4, 255, 2,
 3, 4, 252, 205, 2, 32, 26, 142, 0, 2, 59, 48, 0, 255, 29,
 6, 1, 55, 45, 160, 0, 7, 11, 9, 0, 161, 130, 1, 248, 201,
 255, 80, 7, 255, 2, 18, 255, 98, 3, 4, 180, 146, 1, 9, 7,
 139, 0, 1, 50, 40, 255, 98, 7, 1, 70, 56, 157, 0, 1, 122,
 99, 255, 173, 7, 1, 18, 14, 160, 0, 234, 0, 11, 4, 3, 0,
 248, 201, 2, 255, 207, 2, 193, 157, 1, 153, 0, 4, 92, 75, 0,
 60, 49, 187, 0, 4, 69, 55, 0, 255, 207, 255, 104, 4, 255, 2,
 6, 1, 178, 144, 255, 110, 16, 1, 80, 66, 255, 32, 7, 1, 37,
 31, 163, 0, 128, 1, 5, 0, 96, 79, 0, 231, 187, 255, 83, 10,
 255, 2, 18, 4, 241, 197, 2, 59, 48, 136, 0, 1, 73, 59, 255,
 98, 7, 1, 47, 38, 157, 0, 2, 161, 131, 1, 255, 41, 6, 1,
 7, 6, 160, 0, 234, 0, 11, 8, 6, 0, 255, 207, 2, 255, 207,
 2, 156, 127, 1, 153, 0, 4, 105, 85, 1, 151, 122, 255, 32, 28,
 159, 0, 128, 1, 2, 0, 219, 178, 255, 104, 4, 255, 2, 9, 1,
 28, 23, 139, 0, 2, 103, 84, 1, 255, 23, 6, 1, 25, 21, 169,
 0, 7, 36, 29, 0, 201, 164, 1, 252, 205, 255, 89, 13, 255, 2,
 12, 255, 26, 3, 4, 154, 124, 1, 6, 5, 130, 0, 1, 101, 82,
 255, 98, 7, 1, 28, 22, 157, 0, 1, 205, 166, 255, 41, 4, 1,
 241, 197, 129, 2, 160, 0, 234, 0, 11, 12, 9, 0, 255, 207, 2,
 255, 207, 2, 142, 117, 1, 153, 0, 7, 139, 112, 1, 255, 207, 2,
 14, 11, 187, 0, 1, 81, 66, 255, 107, 7, 255, 2, 6, 1, 131,
 107, 255, 113, 13, 1, 133, 108, 255, 98, 4, 255, 2, 3, 1, 15,
 12, 172, 0, 7, 6, 5, 0, 141, 114, 1, 239, 195, 255, 89, 10,
 255, 2, 18, 7, 237, 193, 2, 53, 42, 0, 142, 117, 255, 98, 40,
 255, 38, 6, 2, 227, 186, 2, 162, 0, 234, 0, 11, 20, 16, 0,
 255, 207, 2, 255, 207, 2, 115, 93, 1, 153, 0, 7, 156, 125, 1,
 255, 207, 2, 55, 45, 187, 0, 4, 11, 9, 0, 250, 203, 255, 107,
 4, 255, 2, 6, 4, 241, 195, 2, 10, 8, 136, 0, 1, 162, 131,
 255, 98, 4, 255, 2, 3, 1, 7, 5, 178, 0, 4, 62, 51, 0,
 217, 176, 255, 92, 10, 255, 2, 18, 1, 252, 205, 255, 2, 4, 255,
 11, 6, 0, 6, 255, 98, 29, 2, 2, 1, 0, 255, 38, 6, 2,
 205, 164, 1, 162, 0, 234, 0, 11, 25, 21, 0, 255, 207, 2, 255,
 207, 2, 101, 82, 1, 153, 0, 7, 184, 151, 1, 255, 207, 2, 197,
 159, 255, 35, 28, 162, 0, 1, 128, 103, 255, 71, 4, 255, 2, 9,
 1, 131, 107, 255, 80, 10, 1, 193, 157, 255, 26, 7, 0, 2, 255,
 100, 56, 7, 14, 11, 0, 161, 130, 1, 246, 199, 255, 95, 10, 255,
 2, 24, 128, 1, 154, 0, 2, 7, 6, 0, 255, 38, 6, 255, 77,
 3, 162, 0, 234, 0, 10, 36, 30, 0, 255, 207, 2, 255, 207, 2,
 82, 67, 154, 0, 10, 211, 171, 1, 255, 207, 2, 250, 205, 2, 20,
 16, 187, 0, 4, 13, 11, 0, 241, 195, 255, 110, 4, 255, 2, 6,
 4, 239, 193, 2, 10, 8, 133, 0, 1, 229, 186, 255, 20, 7, 186,
 0, 7, 1, 0, 0, 80, 65, 0, 221, 178, 255, 98, 10, 255, 2,
 21, 156, 0, 2, 17, 14, 0, 255, 38, 6, 2, 121, 97, 1, 162,
 0, 234, 0, 10, 45, 36, 0, 255, 207, 2, 255, 207, 2, 68, 55,
 154, 0, 1, 235, 191, 255, 35, 4, 255, 2, 3, 1, 90, 72, 190,
 0, 2, 164, 133, 1, 255, 74, 6, 255, 2, 6, 1, 53, 42, 255,
 98, 13, 2, 248, 203, 2, 192, 0, 7, 18, 15, 0, 166, 135, 1,
 246, 199, 255, 101, 10, 255, 2, 12, 2, 205, 166, 1, 156, 0, 2,
 33, 27, 0, 255, 41, 6, 0, 88, 255, 236, 38, 234, 0, 10, 66,
 54, 0, 255, 207, 2, 255, 207, 2, 55, 45, 154, 0, 1, 239, 195,
 255, 35, 4, 255, 2, 3, 4, 239, 193, 2, 3, 2, 187, 0, 1,
 35, 29, 255, 113, 7, 255, 2, 6, 1, 59, 48, 130, 0, 4, 2,
 1, 0, 244, 197, 255, 14, 4, 2, 201, 164, 1, 195, 0, 255, 69,
 3, 4, 78, 62, 0, 221, 178, 255, 104, 10, 255, 2, 9, 1, 164,
 135, 255, 98, 31, 1, 52, 42, 255, 161, 7, 1, 62, 51, 163, 0,
 234, 0, 10, 73, 59, 0, 255, 207, 2, 255, 207, 2, 43, 35, 151,
 0, 4, 1, 0, 0, 246, 199, 255, 35, 4, 255, 2, 3, 4, 252,
 205, 2, 34, 27, 190, 0, 2, 175, 142, 1, 255, 77, 6, 255, 2,
 3, 1, 46, 38, 130, 0, 4, 5, 4, 0, 252, 205, 255, 14, 4,
 2, 169, 138, 1, 201, 0, 7, 14, 11, 0, 156, 127, 1, 241, 195,
 255, 107, 7, 255, 2, 6, 1, 135, 108, 255, 98, 31, 2, 75, 61,
 0, 255, 41, 6, 1, 40, 33, 163, 0, 234, 0, 10, 92, 75, 0,
 255, 207, 2, 255, 207, 2, 32, 26, 151, 0, 4, 4, 3, 0, 248,
 201, 255, 35, 4, 255, 2, 6, 2, 144, 117, 1, 189, 0, 4, 44,
 36, 0, 252, 205, 255, 77, 7, 1, 28, 22, 130, 0, 1, 11, 9,
 255, 134, 7, 0, 142, 255, 92, 65, 144, 0, 5, 57, 47, 0, 213,
 173, 2, 255, 113, 9, 1, 104, 85, 255, 98, 31, 1, 104, 84, 255,
 140, 7, 1, 24, 19, 163, 0, 234, 0, 10, 104, 85, 1, 255, 207,
 2, 255, 207, 2, 22, 18, 151, 0, 2, 5, 4, 0, 255, 35, 6,
 255, 2, 6, 4, 250, 203, 2, 10, 8, 187, 0, 128, 1, 2, 0,
 213, 173, 255, 119, 7, 1, 19, 15, 130, 0, 1, 20, 16, 255, 98,
 7, 2, 115, 93, 1, 210, 0, 7, 7, 5, 0, 125, 103, 1, 233,
 189, 255, 98, 4, 1, 75, 60, 157, 0, 1, 138, 112, 255, 155, 7,
 1, 12, 10, 163, 0, 234, 0, 10, 128, 104, 1, 255, 207, 2, 255,
 207, 2, 15, 13, 151, 0, 2, 13, 10, 0, 255, 35, 6, 255, 2,
 9, 1, 55, 45, 190, 0, 1, 146, 119, 255, 119, 7, 1, 12, 9,
 130, 0, 1, 29, 23, 255, 98, 7, 1, 88, 72, 217, 0, 7, 32,
 26, 0, 184, 149, 1, 51, 41, 157, 0, 1, 169, 138, 255, 155, 4,
 4, 252, 205, 2, 5, 4, 163, 0, 234, 0, 10, 141, 114, 1, 255,
 207, 2, 255, 207, 2, 10, 8, 151, 0, 2, 16, 13, 0, 255, 35,
 6, 255, 2, 9, 2, 205, 168, 1, 189, 0, 1, 166, 135, 255, 119,
 4, 4, 252, 205, 2, 5, 4, 130, 0, 1, 43, 35, 255, 98, 7,
 1, 71, 57, 220, 0, 128, 1, 0, 0, 255, 113, 6, 153, 0, 1,
 209, 169, 255, 155, 4, 3, 237, 193, 2, 1, 255, 229, 38, 234, 0,
 10, 162, 131, 1, 255, 207, 2, 252, 205, 2, 6, 5, 151, 0, 5,
 27, 22, 0, 255, 207, 2, 255, 2, 12, 255, 47, 3, 1, 28, 23,
 187, 0, 1, 193, 157, 255, 119, 4, 1, 248, 201, 129, 2, 130, 0,
 1, 59, 48, 255, 98, 7, 1, 55, 45, 253, 0, 129, 0, 255, 137,
 6, 1, 225, 184, 255, 153, 5, 161, 0, 234, 0, 10, 186, 151, 1,
 255, 207, 2, 250, 203, 2, 3, 3, 151, 0, 5, 31, 26, 0, 255,
 207, 2, 255, 2, 15, 2, 105, 85, 1, 186, 0, 1, 233, 191, 255,
 68, 4, 2, 241, 197, 2, 255, 69, 6, 1, 74, 60, 255, 98, 7,
 1, 36, 29, 253, 0, 128, 1, 255, 137, 7, 1, 201, 162, 255, 224,
 40, 234, 0, 10, 207, 168, 1, 255, 207, 2, 250, 201, 2, 1, 1,
 151, 0, 5, 46, 38, 0, 255, 207, 2, 255, 2, 15, 4, 244, 197,
 2, 6, 5, 184, 0, 1, 248, 201, 255, 68, 4, 2, 237, 193, 2,
 132, 0, 1, 97, 80, 255, 98, 7, 1, 27, 22, 253, 0, 1, 7,
 5, 255, 137, 7, 2, 161, 130, 1, 165, 0, 234, 0, 8, 239, 193,
 2, 255, 207, 2, 248, 201, 2, 153, 0, 4, 55, 45, 0, 255, 207,
 255, 38, 4, 255, 2, 15, 1, 66, 53, 181, 0, 128, 1, 2, 0,
 248, 203, 255, 65, 4, 1, 215, 175, 255, 119, 7, 2, 119, 96, 1,
 255, 83, 6, 1, 17, 14, 253, 0, 1, 14, 12, 255, 236, 7, 2,
 127, 103, 1, 165, 0, 234, 0, 8, 246, 201, 2, 255, 207, 2, 246,
 199, 2, 153, 0, 4, 72, 58, 0, 255, 207, 255, 38, 4, 1, 217,
 176, 255, 5, 4, 255, 2, 9, 2, 176, 144, 1, 180, 0, 128, 3,
 2, 0, 250, 205, 255, 65, 4, 1, 184, 149, 255, 65, 7, 2, 141,
 114, 1, 255, 83, 6, 1, 8, 6, 253, 0, 1, 26, 21, 255, 236,
 7, 1, 93, 75, 166, 0, 231, 0, 128, 1, 9, 0, 248, 201, 2,
 255, 207, 2, 237, 193, 2, 153, 0, 4, 82, 67, 0, 255, 207, 255,
 38, 4, 4, 101, 82, 1, 246, 199, 255, 8, 4, 255, 2, 9, 1,
 29, 23, 178, 0, 3, 6, 5, 0, 252, 255, 62, 5, 2, 147, 119,
 1, 132, 0, 2, 175, 142, 1, 255, 80, 6, 1, 4, 3, 253, 0,
 1, 40, 32, 255, 236, 7, 1, 69, 56, 166, 0, 231, 0, 128, 1,
 9, 0, 250, 205, 2, 255, 207, 2, 191, 156, 1, 153, 0, 4, 101,
 82, 1, 255, 207, 255, 38, 4, 4, 33, 27, 0, 162, 131, 255, 11,
 7, 255, 2, 6, 1, 121, 97, 255, 56, 28, 150, 0, 2, 15, 12,
 0, 255, 62, 6, 1, 133, 107, 255, 62, 7, 1, 201, 164, 255, 86,
 7, 128, 1, 253, 0, 1, 55, 45, 255, 155, 7, 1, 49, 40, 166,
 0, 231, 0, 11, 5, 4, 0, 250, 205, 2, 255, 207, 2, 182, 147,
 1, 153, 0, 4, 112, 91, 1, 255, 207, 255, 38, 4, 7, 20, 16,
 0, 29, 23, 0, 250, 203, 255, 11, 4, 255, 2, 6, 4, 227, 184,
 2, 3, 3, 175, 0, 2, 19, 16, 0, 255, 62, 6, 1, 104, 85,
 255, 119, 7, 1, 229, 186, 255, 80, 7, 253, 0, 128, 0, 1, 82,
 67, 255, 155, 7, 1, 33, 27, 166, 0, 231, 0, 11, 6, 5, 0,
 252, 207, 2, 255, 207, 2, 147, 119, 1, 153, 0, 3, 131, 107, 1,
 255, 255, 38, 5, 7, 14, 12, 0, 3, 2, 0, 225, 182, 255, 11,
 4, 255, 2, 9, 1, 81, 66, 175, 0, 2, 31, 25, 0, 255, 59,
 6, 1, 85, 69, 133, 0, 1, 233, 189, 255, 14, 4, 2, 244, 197,
 2, 253, 0, 128, 0, 1, 105, 85, 255, 236, 7, 1, 21, 17, 166,
 0, 231, 0, 11, 11, 9, 0, 255, 207, 2, 255, 207, 2, 139, 112,
 1, 153, 0, 2, 146, 118, 1, 255, 38, 6, 1, 8, 7, 130, 0,
 1, 92, 75, 255, 53, 7, 255, 2, 6, 1, 195, 157, 129, 1, 172,
 0, 1, 42, 34, 255, 65, 7, 1, 72, 58, 130, 0, 128, 1, 2,
 0, 244, 197, 255, 14, 4, 1, 201, 164, 255, 72, 47, 209, 0, 1,
 135, 109, 255, 236, 7, 1, 11, 9, 166, 0, 231, 0, 11, 15, 13,
 0, 255, 207, 2, 255, 207, 2, 117, 93, 1, 153, 0, 2, 173, 141,
 1, 255, 38, 6, 1, 6, 5, 130, 0, 4, 8, 6, 0, 235, 191,
 255, 14, 4, 255, 2, 6, 4, 252, 205, 2, 24, 19, 172, 0, 1,
 51, 42, 255, 119, 7, 1, 51, 41, 130, 0, 4, 4, 3, 0, 248,
 201, 255, 14, 4, 255, 104, 3, 253, 0, 128, 0, 1, 164, 135, 255,
 236, 4, 4, 250, 203, 2, 5, 4, 166, 0, 231, 0, 10, 19, 16,
 0, 255, 207, 2, 255, 207, 2, 104, 84, 154, 0, 2, 182, 149, 1,
 255, 38, 6, 1, 2, 1, 133, 0, 1, 164, 133, 255, 17, 7, 255,
 2, 6, 1, 171, 138, 255, 24, 8, 164, 0, 1, 74, 60, 255, 119,
 7, 1, 42, 34, 130, 0, 1, 8, 6, 255, 14, 7, 1, 156, 127,
 255, 71, 46, 210, 0, 1, 203, 164, 255, 218, 4, 3, 244, 197, 2,
 1, 255, 139, 41, 231, 0, 10, 30, 24, 0, 255, 207, 2, 255, 207,
 2, 92, 75, 154, 0, 1, 223, 180, 255, 35, 4, 255, 2, 3, 128,
 1, 133, 0, 1, 34, 28, 255, 56, 7, 255, 2, 6, 4, 244, 197,
 2, 19, 15, 169, 0, 1, 85, 68, 255, 62, 7, 1, 28, 22, 130,
 0, 1, 12, 10, 255, 14, 7, 1, 119, 96, 255, 96, 8, 248, 0,
 1, 248, 203, 255, 137, 4, 2, 233, 189, 2, 168, 0, 231, 0, 10,
 32, 26, 0, 255, 207, 2, 255, 207, 2, 73, 59, 154, 0, 1, 231,
 187, 255, 35, 4, 255, 2, 3, 138, 0, 2, 193, 156, 1, 255, 20,
 6, 255, 2, 6, 1, 96, 78, 169, 0, 1, 108, 88, 255, 59, 7,
 1, 20, 16, 130, 0, 1, 22, 18, 255, 134, 7, 2, 105, 85, 1,
 253, 0, 128, 0, 255, 137, 6, 1, 221, 178, 255, 236, 13, 156, 0,
 231, 0, 10, 50, 40, 0, 255, 207, 2, 255, 207, 2, 71, 57, 154,
 0, 1, 233, 189, 255, 35, 4, 2, 250, 203, 2, 138, 0, 1, 92,
 75, 255, 59, 7, 255, 2, 6, 255, 35, 3, 1, 4, 3, 166, 0,
 2, 125, 101, 1, 255, 53, 6, 1, 13, 10, 130, 0, 1, 29, 24,
 255, 74, 7, 1, 82, 67, 253, 0, 128, 2, 255, 137, 7, 2, 178,
 146, 1, 168, 0, 231, 0, 10, 55, 45, 0, 255, 207, 2, 255, 207,
 2, 49, 40, 154, 0, 1, 239, 195, 255, 35, 4, 2, 205, 166, 1,
 138, 0, 4, 6, 5, 0, 244, 199, 255, 23, 4, 255, 2, 9, 1,
 84, 68, 166, 0, 2, 149, 121, 1, 255, 50, 6, 1, 7, 5, 130,
 0, 1, 42, 34, 255, 134, 7, 1, 68, 55, 253, 0, 255, 143, 3,
 255, 137, 6, 1, 144, 117, 255, 236, 13, 156, 0, 231, 0, 10, 62,
 51, 0, 255, 207, 2, 255, 207, 2, 42, 34, 151, 0, 128, 2, 2,
 0, 244, 197, 255, 35, 4, 2, 205, 166, 1, 141, 0, 2, 111, 91,
 1, 255, 62, 6, 255, 2, 6, 1, 199, 161, 129, 1, 163, 0, 1,
 175, 142, 255, 56, 4, 4, 248, 201, 2, 5, 4, 130, 0, 1, 53,
 43, 255, 134, 7, 1, 87, 71, 253, 0, 1, 16, 13, 255, 137, 7,
 1, 117, 95, 255, 201, 38, 131, 0, 231, 0, 10, 82, 67, 0, 255,
 207, 2, 255, 207, 2, 33, 27, 151, 0, 4, 4, 3, 0, 248, 201,
 255, 35, 4, 2, 164, 135, 1, 141, 0, 4, 27, 22, 0, 250, 205,
 255, 26, 4, 255, 2, 9, 1, 37, 30, 163, 0, 9, 193, 157, 1,
 255, 207, 2, 248, 201, 2, 1, 255, 82, 5, 1, 68, 55, 255, 134,
 7, 7, 221, 180, 2, 101, 81, 0, 5, 4, 247, 0, 255, 209, 3,
 255, 137, 6, 1, 93, 77, 169, 0, 231, 0, 10, 82, 67, 0, 255,
 207, 2, 255, 207, 2, 26, 21, 151, 0, 1, 6, 5, 255, 35, 7,
 2, 161, 131, 1, 144, 0, 2, 187, 152, 1, 255, 29, 6, 255, 2,
 6, 1, 193, 157, 255, 35, 19, 144, 0, 1, 229, 186, 255, 44, 4,
 2, 239, 195, 2, 132, 0, 1, 85, 69, 255, 98, 7, 255, 2, 3,
 7, 246, 199, 2, 189, 154, 1, 55, 45, 244, 0, 1, 37, 31, 255,
 137, 7, 1, 69, 55, 169, 0, 231, 0, 10, 109, 87, 1, 255, 207,
 2, 255, 207, 2, 23, 19, 151, 0, 2, 10, 9, 0, 255, 35, 6,
 2, 138, 112, 1, 144, 0, 4, 34, 28, 0, 252, 205, 255, 29, 4,
 255, 2, 6, 4, 248, 201, 2, 31, 25, 160, 0, 1, 246, 201, 255,
 44, 4, 2, 237, 193, 2, 132, 0, 1, 99, 80, 255, 134, 7, 255,
 2, 9, 7, 233, 189, 2, 157, 127, 1, 27, 22, 238, 0, 1, 55,
 45, 255, 236, 7, 1, 51, 41, 169, 0, 231, 0, 10, 117, 93, 1,
 255, 207, 2, 255, 207, 2, 13, 10, 151, 0, 255, 26, 3, 255, 35,
 6, 2, 122, 99, 1, 144, 0, 4, 3, 2, 0, 205, 168, 255, 68,
 7, 255, 2, 6, 1, 130, 105, 255, 38, 19, 141, 0, 1, 248, 201,
 255, 41, 4, 2, 225, 182, 2, 132, 0, 1, 125, 101, 255, 65, 13,
 255, 2, 9, 5, 215, 175, 2, 118, 96, 1, 255, 155, 27, 207, 0,
 1, 73, 59, 255, 236, 7, 1, 36, 30, 169, 0, 231, 0, 10, 119,
 96, 1, 255, 207, 2, 255, 207, 2, 11, 9, 151, 0, 2, 23, 19,
 0, 255, 35, 6, 2, 111, 90, 1, 147, 0, 1, 93, 75, 255, 32,
 7, 255, 2, 6, 4, 252, 205, 2, 13, 11, 154, 0, 128, 1, 2,
 0, 250, 205, 255, 41, 4, 1, 187, 152, 255, 80, 7, 1, 138, 111,
 255, 134, 7, 255, 2, 18, 255, 77, 3, 7, 203, 164, 1, 88, 72,
 0, 5, 4, 226, 0, 1, 92, 74, 255, 203, 7, 1, 25, 20, 169,
 0, 231, 0, 10, 156, 127, 1, 255, 207, 2, 255, 207, 2, 8, 6,
 151, 0, 2, 25, 21, 0, 255, 35, 6, 1, 88, 72, 148, 0, 128,
 3, 2, 0, 227, 186, 255, 32, 4, 255, 2, 9, 2, 139, 112, 1,
 153, 0, 4, 5, 4, 0, 250, 205, 255, 38, 4, 1, 169, 138, 255,
 38, 7, 255, 8, 3, 255, 62, 12, 255, 2, 18, 10, 246, 199, 2,
 191, 156, 1, 63, 52, 0, 2, 2, 220, 0, 1, 117, 93, 255, 137,
 7, 1, 13, 10, 169, 0, 231, 0, 10, 157, 128, 1, 255, 207, 2,
 255, 207, 2, 4, 3, 151, 0, 2, 35, 29, 0, 255, 35, 6, 1,
 86, 70, 151, 0, 1, 168, 136, 255, 71, 7, 255, 2, 6, 4, 233,
 189, 2, 10, 8, 151, 0, 1, 6, 5, 255, 80, 7, 2, 142, 115,
 1, 132, 0, 1, 180, 147, 255, 62, 13, 255, 2, 24, 9, 239, 195,
 2, 180, 146, 1, 50, 40, 0, 1, 255, 55, 7, 208, 0, 1, 151,
 124, 255, 137, 7, 1, 7, 6, 169, 0, 231, 0, 10, 178, 144, 1,
 255, 207, 2, 244, 197, 2, 4, 3, 151, 0, 10, 42, 34, 0, 255,
 207, 2, 255, 207, 2, 60, 48, 145, 0, 7, 6, 5, 0, 81, 66,
 0, 219, 176, 255, 32, 4, 255, 2, 12, 1, 93, 75, 151, 0, 1,
 14, 11, 255, 80, 7, 2, 125, 101, 1, 132, 0, 1, 136, 109, 255,
 134, 4, 255, 2, 39, 7, 235, 191, 2, 173, 139, 1, 39, 31, 211,
 0, 1, 173, 141, 255, 137, 4, 4, 246, 199, 2, 4, 3, 169, 0,
 231, 0, 10, 205, 166, 1, 255, 207, 2, 241, 197, 2, 1, 1, 151,
 0, 10, 47, 39, 0, 255, 207, 2, 255, 207, 2, 59, 47, 142, 0,
 7, 37, 30, 0, 171, 139, 1, 248, 201, 255, 29, 4, 255, 2, 15,
 255, 83, 3, 1, 5, 4, 148, 0, 1, 16, 13, 255, 80, 7, 1,
 104, 85, 255, 117, 10, 4, 27, 22, 0, 193, 157, 255, 140, 4, 255,
 2, 39, 7, 231, 187, 2, 162, 131, 1, 39, 31, 205, 0, 1, 203,
 164, 255, 131, 4, 2, 241, 195, 2, 171, 0, 231, 0, 8, 205, 166,
 1, 255, 207, 2, 233, 191, 2, 153, 0, 10, 66, 53, 0, 255, 207,
 2, 255, 207, 2, 47, 38, 136, 0, 7, 8, 7, 0, 107, 86, 1,
 217, 175, 255, 23, 4, 255, 2, 24, 2, 105, 86, 1, 147, 0, 1,
 27, 22, 255, 80, 7, 1, 92, 75, 142, 0, 4, 49, 39, 0, 223,
 180, 255, 83, 28, 255, 2, 15, 7, 231, 187, 2, 162, 131, 1, 39,
 31, 199, 0, 1, 248, 203, 255, 86, 4, 2, 233, 189, 2, 171, 0,
 231, 0, 8, 233, 189, 2, 255, 207, 2, 225, 184, 2, 153, 0, 4,
 70, 56, 0, 255, 207, 255, 38, 4, 1, 37, 30, 133, 0, 7, 40,
 33, 0, 180, 146, 1, 252, 205, 255, 20, 4, 255, 2, 27, 4, 217,
 175, 2, 6, 5, 145, 0, 1, 31, 25, 255, 80, 7, 1, 73, 59,
 145, 0, 4, 1, 0, 0, 71, 57, 255, 152, 7, 255, 2, 39, 10,
 231, 187, 2, 164, 135, 1, 46, 37, 0, 1, 1, 190, 0, 255, 80,
 6, 2, 225, 182, 2, 171, 0, 231, 0, 8, 255, 207, 2, 255, 207,
 2, 225, 184, 2, 153, 0, 1, 82, 67, 255, 38, 7, 10, 41, 33,
 0, 8, 6, 0, 111, 88, 1, 221, 180, 255, 14, 4, 255, 2, 36,
 1, 72, 58, 145, 0, 1, 44, 36, 255, 80, 7, 1, 63, 52, 151,
 0, 7, 3, 2, 0, 85, 69, 0, 244, 199, 255, 104, 40, 255, 2,
 3, 9, 235, 191, 2, 176, 144, 1, 54, 44, 0, 2, 255, 205, 28,
 154, 0, 128, 1, 255, 152, 7, 2, 180, 147, 1, 171, 0, 231, 0,
 8, 255, 207, 2, 255, 207, 2, 217, 176, 2, 153, 0, 1, 96, 79,
 255, 38, 7, 7, 97, 79, 0, 173, 141, 1, 252, 205, 255, 11, 4,
 255, 2, 36, 255, 41, 3, 2, 168, 136, 1, 144, 0, 1, 51, 42,
 255, 80, 7, 1, 48, 39, 157, 0, 7, 4, 3, 0, 101, 82, 1,
 250, 203, 255, 113, 40, 255, 2, 3, 10, 239, 195, 2, 191, 156, 1,
 70, 56, 0, 6, 5, 175, 0, 1, 5, 4, 255, 152, 7, 1, 159,
 128, 255, 182, 19, 153, 0, 228, 0, 11, 1, 0, 0, 255, 207, 2,
 255, 207, 2, 209, 168, 2, 153, 0, 2, 101, 82, 1, 255, 38, 6,
 1, 248, 203, 255, 5, 4, 255, 2, 36, 10, 252, 205, 2, 227, 186,
 2, 100, 80, 0, 13, 10, 145, 0, 1, 66, 54, 255, 119, 7, 1,
 42, 34, 163, 0, 1, 5, 4, 255, 131, 4, 0, 250, 255, 125, 41,
 255, 170, 4, 6, 201, 2, 201, 162, 1, 97, 79, 255, 128, 22, 150,
 0, 1, 8, 6, 255, 152, 7, 2, 135, 108, 1, 171, 0, 228, 0,
 11, 2, 1, 0, 255, 207, 2, 255, 207, 2, 176, 142, 1, 153, 0,
 2, 128, 104, 1, 255, 38, 6, 255, 2, 39, 7, 248, 203, 2, 147,
 119, 1, 29, 23, 151, 0, 1, 78, 62, 255, 119, 7, 255, 35, 27,
 144, 0, 7, 5, 4, 0, 85, 69, 0, 244, 199, 255, 137, 43, 10,
 252, 205, 2, 213, 173, 1, 131, 107, 1, 27, 22, 163, 0, 1, 18,
 14, 255, 152, 7, 2, 107, 87, 1, 171, 0, 228, 0, 11, 2, 1,
 0, 255, 207, 2, 255, 207, 2, 168, 136, 1, 153, 0, 2, 135, 109,
 1, 255, 38, 6, 255, 2, 33, 10, 252, 205, 2, 217, 175, 2, 68,
 55, 0, 4, 3, 154, 0, 1, 90, 73, 255, 119, 7, 1, 27, 21,
 175, 0, 7, 3, 2, 0, 77, 62, 0, 233, 189, 255, 143, 37, 255,
 2, 9, 1, 229, 186, 255, 227, 4, 1, 50, 40, 157, 0, 1, 25,
 20, 255, 152, 7, 255, 161, 3, 171, 0, 228, 0, 11, 6, 5, 0,
 255, 207, 2, 255, 207, 2, 161, 130, 1, 153, 0, 2, 142, 115, 1,
 255, 38, 6, 255, 2, 30, 7, 250, 203, 2, 141, 114, 1, 19, 15,
 160, 0, 1, 108, 88, 255, 80, 7, 1, 14, 11, 181, 0, 128, 1,
 5, 0, 58, 47, 0, 213, 173, 255, 149, 34, 255, 2, 12, 2, 241,
 197, 2, 156, 0, 2, 33, 27, 0, 255, 41, 6, 1, 69, 55, 172,
 0, 228, 0, 11, 9, 8, 0, 255, 207, 2, 255, 207, 2, 133, 108,
 1, 153, 0, 2, 173, 141, 1, 255, 38, 6, 255, 2, 24, 10, 252,
 205, 2, 211, 171, 2, 72, 58, 0, 4, 3, 163, 0, 1, 114, 92,
 255, 80, 7, 1, 13, 10, 190, 0, 4, 38, 31, 0, 184, 149, 255,
 158, 31, 255, 2, 12, 156, 0, 2, 46, 38, 0, 255, 38, 6, 1,
 54, 44, 172, 0, 228, 0, 11, 10, 9, 0, 255, 207, 2, 255, 207,
 2, 124, 100, 1, 153, 0, 2, 178, 146, 1, 255, 38, 6, 255, 2,
 21, 7, 252, 205, 2, 151, 122, 1, 23, 19, 169, 0, 1, 147, 119,
 255, 80, 7, 1, 6, 5, 196, 0, 4, 21, 17, 0, 133, 108, 255,
 164, 28, 255, 2, 6, 1, 201, 164, 255, 230, 28, 129, 0, 2, 54,
 44, 0, 255, 41, 6, 1, 46, 38, 172, 0, 228, 0, 11, 15, 13,
 0, 255, 207, 2, 255, 207, 2, 117, 95, 1, 153, 0, 2, 193, 157,
 1, 255, 38, 6, 255, 2, 18, 7, 217, 176, 2, 92, 74, 0, 5,
 4, 172, 0, 1, 151, 122, 255, 80, 4, 2, 250, 203, 2, 255, 56,
 48, 156, 0, 7, 7, 5, 0, 88, 71, 0, 233, 189, 255, 170, 22,
 255, 2, 3, 1, 191, 156, 255, 230, 28, 129, 0, 2, 77, 62, 0,
 255, 41, 6, 1, 30, 25, 172, 0, 228, 0, 10, 21, 17, 0, 255,
 207, 2, 255, 207, 2, 92, 75, 154, 0, 1, 223, 180, 255, 35, 4,
 255, 2, 18, 4, 197, 159, 1, 40, 33, 178, 0, 10, 182, 149, 1,
 255, 207, 2, 248, 201, 2, 3, 2, 211, 0, 3, 45, 36, 0, 184,
 255, 98, 5, 255, 2, 15, 2, 156, 127, 1, 156, 0, 2, 87, 71,
 0, 255, 41, 6, 1, 25, 20, 172, 0, 228, 0, 10, 23, 19, 0,
 255, 207, 2, 255, 207, 2, 87, 71, 154, 0, 1, 231, 187, 255, 35,
 4, 255, 2, 12, 7, 246, 199, 2, 144, 117, 1, 11, 9, 181, 0,
 2, 193, 157, 1, 255, 68, 6, 0, 1, 218, 0, 7, 15, 12, 0,
 104, 85, 1, 244, 199, 255, 182, 10, 1, 142, 117, 255, 112, 31, 1,
 109, 88, 255, 152, 4, 255, 2, 3, 1, 14, 11, 172, 0, 228, 0,
 10, 25, 20, 0, 255, 207, 2, 255, 207, 2, 82, 67, 154, 0, 1,
 233, 189, 255, 35, 4, 255, 2, 9, 255, 14, 3, 4, 86, 69, 0,
 3, 2, 184, 0, 1, 209, 171, 255, 71, 4, 1, 239, 195, 255, 66,
 59, 167, 0, 128, 1, 2, 0, 75, 60, 255, 230, 7, 2, 115, 93,
 1, 156, 0, 2, 128, 104, 1, 255, 41, 6, 1, 10, 8, 172, 0,
 228, 0, 10, 39, 31, 0, 255, 207, 2, 255, 207, 2, 67, 55, 154,
 0, 1, 233, 189, 255, 35, 4, 255, 2, 6, 4, 197, 161, 1, 48,
 39, 190, 0, 1, 244, 199, 255, 74, 4, 2, 239, 195, 2, 228, 0,
 1, 19, 15, 255, 230, 7, 2, 101, 82, 1, 156, 0, 10, 149, 121,
 1, 255, 207, 2, 252, 205, 2, 5, 4, 172, 0, 228, 0, 10, 40,
 32, 0, 255, 207, 2, 255, 207, 2, 62, 51, 154, 0, 1, 241, 195,
 255, 35, 4, 7, 246, 201, 2, 169, 138, 1, 23, 18, 193, 0, 1,
 248, 201, 255, 80, 4, 2, 233, 189, 2, 228, 0, 1, 25, 21, 255,
 230, 7, 1, 82, 67, 157, 0, 7, 178, 146, 1, 255, 207, 2, 248,
 201, 129, 2, 172, 0, 228, 0, 10, 41, 33, 0, 255, 207, 2, 255,
 207, 2, 55, 45, 151, 0, 128, 1, 11, 0, 244, 197, 2, 250, 203,
 2, 130, 105, 1, 7, 6, 196, 0, 1, 248, 201, 255, 116, 4, 2,
 207, 168, 1, 228, 0, 1, 35, 29, 255, 230, 7, 1, 71, 57, 157,
 0, 8, 193, 157, 1, 255, 207, 2, 244, 197, 2, 255, 153, 48, 228,
 0, 10, 92, 74, 0, 255, 207, 2, 255, 207, 2, 48, 40, 151, 0,
 7, 4, 3, 0, 219, 176, 2, 91, 73, 255, 8, 4, 195, 0, 128,
 1, 2, 0, 248, 203, 255, 116, 4, 2, 187, 152, 1, 228, 0, 1,
 43, 35, 255, 230, 7, 1, 55, 45, 157, 0, 1, 233, 189, 255, 38,
 4, 2, 239, 195, 2, 174, 0, 225, 0, 13, 50, 40, 0, 201, 162,
 1, 255, 207, 2, 255, 207, 2, 40, 33, 151, 0, 4, 2, 1, 0,
 60, 49, 202, 0, 255, 80, 3, 1, 250, 205, 255, 116, 4, 2, 182,
 147, 1, 228, 0, 2, 55, 45, 0, 255, 230, 6, 1, 48, 39, 157,
 0, 1, 248, 201, 255, 38, 4, 2, 235, 191, 2, 174, 0, 219, 0,
 128, 3, 8, 0, 74, 60, 0, 211, 171, 2, 255, 207, 255, 2, 7,
 1, 34, 28, 232, 0, 4, 6, 5, 0, 250, 205, 255, 116, 4, 2,
 144, 117, 1, 228, 0, 2, 68, 55, 0, 255, 230, 6, 1, 33, 27,
 157, 0, 1, 248, 201, 255, 38, 4, 1, 203, 164, 255, 152, 49, 216,
 0, 10, 4, 3, 0, 82, 67, 0, 244, 197, 2, 255, 207, 255, 2,
 10, 1, 30, 24, 232, 0, 2, 6, 5, 0, 255, 116, 6, 2, 141,
 114, 1, 228, 0, 1, 78, 63, 255, 113, 7, 255, 230, 30, 128, 1,
 2, 0, 250, 203, 255, 38, 4, 1, 184, 149, 255, 152, 49, 213, 0,
 10, 5, 4, 0, 119, 96, 1, 244, 197, 2, 255, 207, 255, 2, 13,
 1, 22, 18, 232, 0, 2, 10, 8, 0, 255, 116, 6, 2, 128, 104,
 1, 228, 0, 1, 96, 79, 255, 113, 7, 1, 17, 14, 154, 0, 4,
 5, 4, 0, 250, 203, 255, 38, 4, 1, 156, 125, 255, 152, 49, 210,
 0, 10, 11, 9, 0, 138, 112, 1, 235, 189, 2, 255, 207, 255, 2,
 16, 1, 21, 16, 232, 0, 2, 15, 12, 0, 255, 116, 6, 2, 107,
 86, 1, 228, 0, 2, 104, 85, 1, 255, 113, 6, 1, 17, 14, 154,
 0, 1, 6, 5, 255, 152, 7, 1, 141, 114, 255, 152, 49, 210, 0,
 7, 138, 112, 1, 250, 203, 2, 255, 207, 255, 2, 19, 1, 15, 13,
 232, 0, 2, 16, 13, 0, 255, 116, 6, 1, 101, 82, 229, 0, 2,
 131, 107, 1, 255, 113, 6, 1, 7, 5, 154, 0, 1, 12, 9, 255,
 152, 7, 2, 117, 93, 1, 174, 0, 207, 0, 7, 2, 1, 0, 250,
 205, 2, 255, 207, 255, 2, 22, 1, 15, 12, 232, 0, 2, 23, 19,
 0, 255, 116, 6, 1, 87, 71, 229, 0, 2, 139, 112, 1, 255, 113,
 6, 1, 7, 5, 154, 0, 1, 15, 13, 255, 152, 7, 1, 104, 84,
 175, 0, 207, 0, 7, 6, 5, 0, 250, 205, 2, 255, 207, 255, 2,
 22, 1, 9, 7, 232, 0, 2, 30, 24, 0, 255, 116, 6, 1, 74,
 60, 229, 0, 2, 166, 135, 1, 255, 113, 6, 128, 2, 154, 0, 1,
 23, 19, 255, 152, 7, 1, 87, 71, 175, 0, 207, 0, 7, 6, 5,
 0, 250, 205, 2, 255, 207, 255, 2, 22, 1, 9, 7, 232, 0, 2,
 31, 25, 0, 255, 116, 6, 1, 71, 57, 229, 0, 2, 178, 146, 1,
 255, 113, 6, 128, 1, 255, 152, 37, 1, 73, 59, 175, 0, 207, 0,
 5, 6, 5, 0, 255, 207, 2, 255, 2, 24, 1, 5, 4, 232, 0,
 1, 42, 34, 255, 137, 7, 1, 57, 46, 229, 0, 2, 197, 161, 1,
 255, 113, 6, 156, 0, 1, 36, 30, 255, 152, 7, 1, 63, 52, 175,
 0, 207, 0, 5, 6, 5, 0, 255, 207, 2, 255, 2, 21, 4, 252,
 205, 2, 5, 4, 232, 0, 1, 50, 41, 255, 137, 7, 1, 49, 40,
 229, 0, 1, 223, 180, 255, 233, 7, 255, 152, 39, 1, 48, 39, 175,
 0, 207, 0, 5, 14, 11, 0, 255, 207, 2, 255, 2, 21, 3, 152,
 124, 1, 1, 233, 0, 1, 52, 42, 255, 137, 7, 1, 45, 37, 229,
 0, 1, 233, 189, 255, 233, 7, 255, 152, 90, 207, 0, 5, 15, 12,
 0, 255, 207, 2, 255, 2, 18, 4, 151, 122, 1, 17, 13, 235, 0,
 1, 63, 52, 255, 137, 7, 1, 36, 30, 229, 0, 1, 233, 189, 255,
 110, 4, 2, 207, 168, 1, 156, 0, 1, 77, 62, 255, 152, 7, 1,
 29, 23, 175, 0, 207, 0, 5, 15, 13, 0, 255, 207, 2, 255, 2,
 12, 7, 241, 195, 2, 147, 119, 1, 11, 9, 238, 0, 1, 77, 62,
 255, 137, 7, 1, 29, 23, 229, 0, 1, 239, 193, 255, 110, 4, 2,
 201, 164, 1, 156, 0, 1, 79, 63, 255, 152, 7, 1, 27, 22, 175,
 0, 207, 0, 5, 16, 13, 0, 255, 207, 2, 255, 2, 9, 7, 244,
 199, 2, 147, 119, 1, 5, 4, 241, 0, 1, 79, 63, 255, 137, 7,
 1, 27, 21, 226, 0, 128, 1, 2, 0, 244, 197, 255, 110, 4, 2,
 187, 152, 1, 156, 0, 1, 92, 75, 255, 152, 7, 1, 19, 15, 175,
 0, 207, 0, 5, 16, 13, 0, 255, 207, 2, 255, 2, 6, 7, 250,
 205, 2, 122, 99, 1, 7, 6, 244, 0, 1, 81, 66, 255, 137, 7,
 1, 24, 19, 226, 0, 4, 4, 3, 0, 244, 197, 255, 110, 4, 2,
 156, 127, 1, 156, 0, 2, 109, 88, 1, 255, 152, 6, 1, 13, 11,
 175, 0, 207, 0, 5, 29, 23, 0, 255, 207, 2, 255, 2, 6, 4,
 122, 99, 1, 10, 8, 247, 0, 2, 108, 88, 1, 255, 134, 6, 1,
 14, 11, 226, 0, 4, 4, 3, 0, 252, 205, 255, 110, 4, 2, 156,
 127, 1, 156, 0, 1, 111, 90, 255, 152, 7, 1, 12, 9, 175, 0,
 207, 0, 13, 27, 22, 0, 255, 207, 2, 255, 207, 2, 157, 127, 1,
 12, 9, 250, 0, 2, 109, 88, 1, 255, 137, 6, 1, 13, 11, 226,
 0, 1, 7, 5, 255, 248, 7, 2, 130, 107, 1, 156, 0, 1, 147,
 119, 255, 152, 7, 1, 5, 4, 175, 0, 207, 0, 10, 27, 22, 0,
 255, 207, 2, 255, 207, 2, 97, 79, 253, 0, 2, 111, 90, 1, 255,
 137, 6, 1, 13, 10, 226, 0, 1, 10, 9, 255, 248, 7, 2, 115,
 93, 1, 156, 0, 1, 146, 118, 255, 152, 4, 4, 250, 203, 2, 5,
 4, 175, 0, 207, 0, 10, 29, 23, 0, 255, 207, 2, 255, 207, 2,
 77, 62, 253, 0, 2, 128, 104, 1, 255, 137, 6, 1, 7, 5, 226,
 0, 1, 13, 10, 255, 248, 7, 2, 115, 93, 1, 156, 0, 1, 156,
 125, 255, 152, 4, 4, 250, 203, 2, 3, 3, 175, 0, 207, 0, 10,
 29, 23, 0, 255, 207, 2, 255, 207, 2, 77, 62, 253, 0, 2, 147,
 119, 1, 255, 137, 6, 1, 5, 4, 226, 0, 1, 17, 14, 255, 248,
 7, 1, 92, 74, 157, 0, 1, 182, 149, 255, 152, 4, 4, 250, 203,
 2, 1, 1, 175, 0, 207, 0, 10, 40, 33, 0, 255, 207, 2, 255,
 207, 2, 52, 42, 253, 0, 10, 149, 121, 1, 255, 207, 2, 250, 203,
 2, 5, 4, 226, 0, 1, 23, 19, 255, 248, 7, 1, 82, 67, 157,
 0, 1, 189, 154, 255, 152, 4, 4, 248, 201, 2, 1, 1, 175, 0,
 207, 0, 10, 45, 37, 0, 255, 207, 2, 255, 207, 2, 52, 42, 253,
 0, 10, 157, 128, 1, 255, 207, 2, 248, 201, 2, 5, 4, 226, 0,
 1, 27, 22, 255, 248, 7, 1, 82, 67, 157, 0, 1, 207, 168, 255,
 152, 7, 177, 0, 207, 0, 10, 45, 37, 0, 255, 207, 2, 255, 207,
 2, 52, 42, 247, 0, 128, 1, 2, 0, 84, 69, 255, 134, 7, 4,
 248, 201, 2, 1, 1, 226, 0, 1, 32, 26, 255, 113, 7, 1, 65,
 53, 157, 0, 1, 233, 189, 255, 152, 7, 177, 0, 207, 0, 10, 48,
 39, 0, 255, 207, 2, 255, 207, 2, 50, 40, 244, 0, 128, 1, 5,
 0, 101, 82, 0, 246, 199, 255, 131, 4, 255, 2, 3, 4, 250, 203,
 2, 1, 1, 226, 0, 1, 41, 33, 255, 248, 7, 1, 55, 45, 157,
 0, 1, 239, 195, 255, 38, 4, 2, 244, 197, 2, 177, 0, 207, 0,
 10, 49, 40, 0, 255, 207, 2, 255, 207, 2, 50, 40, 241, 0, 7,
 4, 3, 0, 122, 100, 1, 250, 203, 255, 128, 4, 255, 2, 6, 4,
 248, 201, 2, 1, 1, 130, 0, 2, 32, 26, 0, 255, 8, 6, 213,
 0, 1, 44, 36, 255, 248, 7, 1, 55, 45, 157, 0, 1, 239, 195,
 255, 38, 4, 1, 215, 173, 255, 141, 52, 207, 0, 10, 49, 40, 0,
 255, 207, 2, 255, 207, 2, 45, 37, 238, 0, 7, 4, 3, 0, 138,
 112, 1, 250, 205, 255, 125, 4, 255, 2, 9, 4, 248, 201, 2, 1,
 1, 130, 0, 4, 90, 74, 0, 57, 46, 217, 0, 255, 239, 3, 255,
 113, 6, 1, 48, 39, 154, 0, 255, 139, 3, 1, 239, 195, 255, 38,
 4, 1, 195, 159, 255, 150, 5, 173, 0, 207, 0, 10, 61, 50, 0,
 255, 207, 2, 255, 207, 2, 31, 25, 235, 0, 7, 8, 6, 0, 161,
 131, 1, 250, 203, 255, 122, 4, 255, 2, 12, 2, 248, 201, 2, 132,
 0, 7, 101, 82, 1, 213, 171, 1, 13, 10, 214, 0, 1, 66, 53,
 255, 248, 7, 1, 33, 27, 154, 0, 4, 1, 0, 0, 246, 199, 255,
 38, 4, 2, 191, 156, 1, 177, 0, 207, 0, 10, 71, 57, 0, 255,
 207, 2, 255, 207, 2, 31, 25, 232, 0, 5, 6, 5, 0, 147, 119,
 1, 255, 119, 6, 255, 2, 15, 2, 248, 201, 2, 132, 0, 1, 101,
 82, 255, 32, 4, 255, 38, 3, 128, 1, 211, 0, 1, 67, 54, 255,
 248, 7, 1, 33, 27, 154, 0, 255, 124, 3, 255, 146, 3, 255, 38,
 3, 1, 178, 146, 255, 135, 52, 207, 0, 10, 71, 57, 0, 255, 207,
 2, 255, 207, 2, 31, 25, 229, 0, 5, 9, 7, 0, 180, 146, 1,
 255, 116, 6, 255, 2, 18, 2, 246, 199, 2, 132, 0, 1, 125, 101,
 255, 35, 4, 4, 252, 205, 2, 69, 55, 211, 0, 255, 248, 9, 1,
 33, 27, 154, 0, 4, 5, 4, 0, 248, 201, 255, 38, 4, 2, 151,
 122, 1, 177, 0, 207, 0, 10, 72, 58, 0, 255, 207, 2, 255, 207,
 2, 30, 24, 226, 0, 5, 9, 8, 0, 166, 135, 1, 255, 113, 6,
 255, 2, 21, 2, 186, 149, 1, 132, 0, 1, 131, 107, 255, 38, 7,
 4, 229, 186, 2, 22, 18, 208, 0, 1, 78, 63, 255, 248, 7, 255,
 92, 30, 4, 5, 4, 0, 250, 203, 255, 38, 4, 1, 149, 121, 255,
 152, 7, 171, 0, 207, 0, 10, 74, 60, 0, 255, 207, 2, 255, 207,
 2, 30, 24, 223, 0, 5, 5, 4, 0, 159, 128, 1, 255, 110, 6,
 255, 2, 21, 4, 205, 168, 1, 18, 14, 133, 0, 1, 135, 109, 255,
 41, 10, 4, 168, 136, 1, 4, 3, 205, 0, 1, 95, 78, 255, 248,
 7, 0, 17, 255, 113, 8, 255, 179, 24, 255, 38, 6, 2, 147, 119,
 1, 177, 0, 207, 0, 10, 74, 60, 0, 255, 207, 2, 255, 207, 2,
 30, 24, 220, 0, 7, 8, 7, 0, 186, 151, 1, 252, 205, 255, 107,
 4, 255, 2, 21, 4, 169, 138, 1, 9, 8, 136, 0, 2, 139, 112,
 1, 255, 26, 9, 255, 53, 3, 2, 108, 87, 1, 204, 0, 1, 96,
 79, 255, 248, 7, 1, 17, 14, 154, 0, 1, 7, 5, 255, 38, 7,
 1, 118, 95, 255, 128, 52, 207, 0, 10, 74, 60, 0, 255, 207, 2,
 255, 207, 2, 25, 21, 217, 0, 5, 6, 5, 0, 151, 122, 1, 255,
 104, 6, 255, 2, 21, 4, 166, 136, 1, 11, 9, 139, 0, 1, 139,
 112, 255, 47, 13, 4, 239, 195, 2, 45, 36, 202, 0, 1, 101, 82,
 255, 95, 7, 1, 17, 14, 154, 0, 2, 13, 10, 0, 255, 38, 6,
 2, 111, 90, 1, 177, 0, 207, 0, 10, 82, 67, 0, 255, 207, 2,
 255, 207, 2, 16, 13, 214, 0, 128, 1, 3, 0, 138, 112, 1, 255,
 101, 6, 255, 2, 18, 7, 252, 205, 2, 173, 139, 1, 9, 8, 142,
 0, 1, 169, 138, 255, 50, 16, 4, 207, 169, 1, 15, 12, 199, 0,
 1, 104, 85, 255, 95, 7, 1, 10, 9, 154, 0, 2, 13, 10, 0,
 255, 38, 6, 2, 108, 88, 1, 177, 0, 207, 0, 10, 101, 82, 1,
 255, 207, 2, 255, 207, 2, 16, 13, 211, 0, 7, 2, 1, 0, 142,
 115, 1, 248, 203, 255, 98, 4, 255, 2, 21, 4, 157, 127, 1, 8,
 6, 145, 0, 1, 173, 141, 255, 152, 7, 255, 2, 12, 4, 151, 122,
 1, 2, 1, 196, 0, 1, 131, 107, 255, 95, 7, 1, 7, 5, 154,
 0, 2, 14, 11, 0, 255, 38, 6, 1, 108, 88, 255, 120, 52, 207,
 0, 10, 101, 82, 1, 255, 207, 2, 255, 207, 2, 16, 13, 208, 0,
 128, 2, 2, 0, 112, 92, 255, 95, 7, 255, 2, 21, 4, 171, 141,
 1, 8, 7, 148, 0, 1, 175, 142, 255, 56, 19, 4, 250, 203, 2,
 95, 77, 196, 0, 1, 131, 107, 255, 95, 7, 1, 7, 5, 154, 0,
 2, 18, 14, 0, 255, 38, 6, 1, 85, 68, 178, 0, 207, 0, 10,
 96, 79, 0, 255, 207, 2, 255, 207, 2, 16, 13, 208, 0, 4, 91,
 74, 0, 250, 203, 255, 92, 4, 255, 2, 21, 4, 168, 136, 1, 6,
 5, 151, 0, 2, 182, 149, 1, 255, 53, 21, 4, 237, 193, 2, 41,
 33, 193, 0, 1, 138, 112, 255, 95, 7, 0, 7, 255, 131, 26, 129,
 0, 2, 27, 22, 0, 255, 38, 6, 1, 79, 63, 178, 0, 207, 0,
 10, 95, 78, 0, 255, 207, 2, 255, 207, 2, 17, 14, 205, 0, 4,
 71, 57, 0, 235, 189, 255, 89, 4, 255, 2, 21, 4, 171, 139, 1,
 10, 8, 154, 0, 2, 182, 149, 1, 255, 59, 24, 4, 225, 182, 2,
 9, 7, 190, 0, 1, 139, 112, 255, 95, 7, 1, 7, 5, 154, 0,
 2, 27, 22, 0, 255, 38, 6, 1, 78, 62, 178, 0, 207, 0, 10,
 101, 82, 1, 255, 207, 2, 255, 207, 2, 17, 14, 202, 0, 4, 46,
 38, 0, 233, 189, 255, 86, 4, 255, 2, 21, 4, 201, 162, 1, 14,
 11, 157, 0, 1, 189, 156, 255, 152, 7, 1, 135, 108, 255, 8, 7,
 255, 2, 12, 2, 193, 157, 1, 189, 0, 1, 149, 121, 255, 86, 7,
 128, 1, 154, 0, 2, 29, 23, 0, 255, 38, 6, 1, 77, 62, 178,
 0, 207, 0, 10, 101, 82, 1, 255, 207, 2, 255, 207, 2, 17, 14,
 199, 0, 4, 31, 25, 0, 244, 197, 255, 83, 4, 255, 2, 21, 4,
 197, 161, 1, 12, 9, 160, 0, 1, 223, 180, 255, 47, 7, 4, 6,
 5, 0, 199, 161, 255, 164, 7, 255, 2, 12, 2, 125, 101, 1, 186,
 0, 1, 173, 141, 255, 83, 7, 128, 1, 154, 0, 2, 29, 23, 0,
 255, 38, 6, 1, 69, 55, 178, 0, 207, 0, 10, 101, 82, 1, 255,
 207, 2, 255, 207, 2, 17, 14, 196, 0, 4, 19, 16, 0, 205, 166,
 255, 83, 7, 255, 2, 18, 4, 193, 157, 1, 21, 17, 163, 0, 1,
 223, 180, 255, 50, 7, 129, 0, 4, 25, 20, 0, 246, 199, 255, 77,
 19, 1, 66, 54, 184, 0, 1, 173, 141, 255, 164, 7, 128, 1, 154,
 0, 2, 36, 30, 0, 255, 38, 6, 1, 52, 42, 178, 0, 207, 0,
 10, 101, 82, 1, 255, 207, 2, 255, 207, 2, 12, 9, 193, 0, 4,
 9, 7, 0, 164, 133, 255, 80, 7, 255, 2, 18, 4, 221, 180, 2,
 32, 26, 166, 0, 1, 223, 180, 255, 53, 7, 132, 0, 2, 74, 59,
 0, 255, 80, 18, 4, 235, 191, 2, 27, 22, 181, 0, 1, 178, 146,
 255, 167, 7, 128, 1, 154, 0, 1, 45, 37, 255, 119, 7, 1, 52,
 42, 178, 0, 207, 0, 10, 101, 82, 1, 255, 207, 2, 255, 207, 2,
 7, 5, 190, 0, 7, 2, 1, 0, 136, 111, 1, 252, 205, 255, 77,
 4, 255, 2, 18, 4, 248, 201, 2, 43, 35, 169, 0, 1, 231, 187,
 255, 56, 7, 135, 0, 1, 142, 115, 255, 170, 7, 255, 2, 12, 4,
 197, 159, 1, 12, 9, 178, 0, 1, 182, 149, 255, 77, 7, 128, 1,
 154, 0, 2, 45, 37, 0, 255, 38, 6, 1, 51, 41, 178, 0, 207,
 0, 10, 128, 104, 1, 255, 207, 2, 255, 207, 2, 7, 5, 190, 0,
 4, 103, 84, 0, 246, 199, 255, 74, 4, 255, 2, 18, 4, 237, 193,
 2, 67, 54, 172, 0, 1, 233, 189, 255, 59, 7, 135, 0, 128, 2,
 2, 0, 197, 161, 255, 173, 7, 255, 2, 12, 3, 159, 130, 1, 3,
 255, 37, 10, 166, 0, 1, 182, 149, 255, 74, 7, 156, 0, 2, 48,
 39, 0, 255, 38, 6, 1, 50, 40, 178, 0, 207, 0, 10, 131, 107,
 1, 255, 207, 2, 255, 207, 2, 7, 5, 187, 0, 4, 55, 45, 0,
 237, 191, 255, 71, 4, 255, 2, 18, 4, 244, 197, 2, 93, 75, 175,
 0, 1, 233, 189, 255, 59, 7, 138, 0, 4, 15, 12, 0, 221, 180,
 255, 95, 16, 5, 252, 205, 2, 112, 92, 1, 174, 0, 1, 201, 164,
 255, 248, 7, 2, 2, 1, 0, 255, 60, 27, 2, 49, 40, 0, 255,
 38, 6, 1, 50, 40, 178, 0, 207, 0, 10, 131, 107, 1, 255, 207,
 2, 255, 207, 2, 7, 5, 184, 0, 4, 21, 16, 0, 241, 195, 255,
 68, 4, 255, 2, 21, 1, 112, 91, 129, 1, 175, 0, 1, 233, 189,
 255, 59, 4, 1, 201, 164, 255, 57, 16, 4, 43, 34, 0, 237, 191,
 255, 98, 16, 4, 248, 201, 2, 81, 66, 172, 0, 1, 223, 180, 255,
 59, 7, 32, 166, 135, 1, 130, 105, 1, 103, 84, 0, 77, 62, 0,
 54, 44, 0, 31, 25, 0, 15, 13, 0, 5, 4, 0, 3, 3, 0,
 2, 1, 0, 61, 50, 0, 255, 38, 6, 1, 36, 29, 178, 0, 207,
 0, 10, 131, 107, 1, 255, 207, 2, 255, 207, 2, 7, 5, 181, 0,
 4, 9, 7, 0, 209, 169, 255, 68, 7, 255, 2, 18, 4, 146, 118,
 1, 4, 3, 178, 0, 1, 233, 189, 255, 62, 4, 2, 201, 164, 1,
 144, 0, 4, 90, 73, 0, 252, 205, 255, 104, 16, 255, 17, 3, 1,
 50, 40, 169, 0, 1, 223, 180, 255, 176, 22, 255, 2, 9, 7, 211,
 171, 1, 176, 142, 1, 195, 157, 255, 218, 7, 1, 33, 27, 178, 0,
 207, 0, 10, 131, 107, 1, 255, 207, 2, 255, 207, 2, 7, 5, 178,
 0, 128, 3, 2, 0, 144, 117, 255, 65, 7, 255, 2, 18, 4, 201,
 162, 1, 9, 7, 181, 0, 1, 233, 189, 255, 65, 5, 1, 164, 1,
 144, 0, 128, 1, 2, 0, 130, 105, 255, 116, 19, 4, 227, 186, 2,
 25, 20, 166, 0, 1, 223, 180, 255, 179, 22, 255, 2, 24, 1, 33,
 27, 178, 0, 207, 0, 10, 131, 107, 1, 255, 207, 2, 255, 207, 2,
 7, 5, 175, 0, 5, 1, 0, 0, 85, 69, 0, 255, 62, 6, 255,
 2, 18, 4, 241, 195, 2, 17, 14, 184, 0, 1, 233, 189, 255, 68,
 4, 2, 201, 164, 1, 147, 0, 4, 5, 4, 0, 178, 144, 255, 185,
 7, 255, 2, 12, 4, 197, 161, 1, 13, 10, 255, 95, 43, 255, 2,
 42, 1, 33, 27, 178, 0, 207, 0, 10, 135, 109, 1, 255, 207, 2,
 255, 207, 2, 7, 5, 175, 0, 4, 41, 33, 0, 239, 195, 255, 59,
 4, 255, 2, 18, 4, 231, 187, 2, 42, 34, 187, 0, 1, 233, 189,
 255, 71, 4, 2, 201, 164, 1, 150, 0, 4, 15, 12, 0, 229, 187,
 255, 122, 19, 4, 175, 142, 1, 3, 3, 255, 95, 40, 255, 2, 42,
 1, 33, 27, 178, 0, 207, 0, 10, 139, 112, 1, 255, 207, 2, 255,
 207, 2, 7, 5, 172, 0, 4, 15, 13, 0, 197, 161, 255, 59, 7,
 255, 2, 15, 4, 244, 199, 2, 86, 70, 190, 0, 1, 233, 189, 255,
 74, 4, 2, 201, 164, 1, 153, 0, 4, 33, 27, 0, 235, 191, 255,
 128, 19, 1, 156, 128, 129, 1, 255, 95, 37, 255, 2, 42, 255, 107,
 3, 177, 0, 207, 0, 10, 139, 112, 1, 255, 207, 2, 255, 207, 2,
 7, 5, 169, 0, 3, 4, 3, 0, 138, 255, 56, 8, 255, 2, 18,
 3, 130, 105, 1, 1, 191, 0, 1, 244, 197, 255, 74, 4, 1, 201,
 164, 255, 73, 31, 4, 53, 43, 0, 239, 193, 255, 131, 20, 0, 104,
 129, 1, 154, 0, 1, 233, 189, 255, 188, 22, 255, 2, 24, 1, 33,
 27, 178, 0, 207, 0, 10, 139, 112, 1, 255, 207, 2, 255, 207, 2,
 7, 5, 169, 0, 4, 77, 61, 0, 250, 205, 255, 53, 4, 255, 2,
 18, 2, 173, 141, 1, 255, 74, 45, 150, 0, 1, 244, 197, 255, 77,
 4, 2, 201, 164, 1, 159, 0, 3, 86, 70, 0, 252, 255, 140, 20,
 1, 96, 79, 154, 0, 19, 135, 109, 1, 162, 131, 1, 180, 147, 1,
 195, 157, 1, 209, 169, 1, 229, 186, 2, 248, 201, 255, 209, 22, 255,
 2, 6, 1, 33, 27, 178, 0, 207, 0, 10, 139, 112, 1, 255, 207,
 2, 255, 207, 2, 7, 5, 166, 0, 4, 32, 26, 0, 227, 186, 255,
 50, 4, 255, 2, 18, 4, 215, 175, 2, 23, 19, 196, 0, 1, 244,
 197, 255, 80, 4, 2, 201, 164, 1, 159, 0, 128, 1, 2, 0, 121,
 97, 255, 197, 7, 255, 2, 12, 4, 246, 199, 2, 68, 55, 255, 53,
 28, 46, 4, 3, 0, 7, 5, 0, 9, 8, 0, 14, 11, 0, 19,
 16, 0, 22, 18, 0, 31, 25, 0, 45, 37, 0, 58, 48, 0, 79,
 65, 0, 100, 81, 1, 119, 97, 1, 128, 104, 1, 142, 117, 1, 154,
 125, 1, 11, 9, 178, 0, 207, 0, 10, 139, 112, 1, 255, 207, 2,
 255, 207, 2, 7, 5, 163, 0, 4, 9, 7, 0, 175, 142, 255, 50,
 7, 255, 2, 15, 4, 246, 201, 2, 55, 45, 199, 0, 1, 244, 197,
 255, 83, 4, 2, 201, 164, 1, 162, 0, 4, 2, 1, 0, 173, 139,
 255, 149, 22, 1, 61, 49, 193, 0, 128, 1, 181, 0, 207, 0, 10,
 139, 112, 1, 255, 207, 2, 255, 207, 2, 7, 5, 160, 0, 128, 1,
 2, 0, 105, 85, 255, 47, 7, 255, 2, 18, 3, 103, 84, 0, 1,
 200, 0, 1, 244, 197, 255, 83, 4, 1, 173, 141, 255, 82, 40, 4,
 2, 1, 0, 182, 147, 255, 155, 19, 4, 250, 203, 2, 48, 40, 247,
 0, 207, 0, 7, 138, 111, 1, 255, 207, 2, 255, 207, 129, 2, 160,
 0, 4, 43, 34, 0, 241, 195, 255, 44, 4, 255, 2, 18, 4, 162,
 131, 1, 6, 5, 202, 0, 1, 244, 197, 255, 86, 4, 2, 156, 127,
 1, 168, 0, 4, 8, 6, 0, 195, 159, 255, 206, 7, 255, 2, 12,
 4, 231, 187, 2, 33, 26, 244, 0, 207, 0, 10, 135, 108, 1, 255,
 207, 2, 255, 207, 2, 2, 1, 157, 0, 4, 10, 9, 0, 201, 164,
 255, 44, 7, 255, 2, 15, 4, 225, 182, 2, 18, 15, 205, 0, 1,
 244, 197, 255, 89, 4, 1, 156, 127, 255, 150, 32, 140, 0, 4, 20,
 16, 0, 221, 178, 255, 161, 19, 4, 227, 184, 2, 28, 22, 241, 0,
 207, 0, 10, 133, 108, 1, 255, 207, 2, 255, 207, 2, 2, 1, 154,
 0, 255, 28, 3, 1, 118, 96, 255, 41, 7, 255, 2, 15, 4, 252,
 205, 2, 51, 41, 205, 0, 128, 1, 2, 0, 244, 197, 255, 92, 4,
 1, 156, 127, 255, 150, 29, 146, 0, 4, 29, 23, 0, 248, 203, 255,
 167, 19, 4, 227, 184, 2, 28, 22, 238, 0, 207, 0, 10, 133, 108,
 1, 255, 207, 2, 255, 207, 2, 2, 1, 154, 0, 4, 42, 33, 0,
 250, 203, 255, 38, 4, 255, 2, 18, 1, 118, 96, 129, 1, 205, 0,
 4, 4, 3, 0, 244, 197, 255, 92, 4, 1, 156, 127, 255, 90, 52,
 4, 32, 26, 0, 229, 187, 255, 170, 19, 4, 213, 171, 2, 23, 18,
 235, 0, 207, 0, 10, 133, 108, 1, 255, 207, 2, 255, 207, 2, 2,
 1, 151, 0, 4, 6, 5, 0, 217, 176, 255, 35, 4, 255, 2, 18,
 4, 191, 154, 1, 7, 5, 208, 0, 4, 4, 3, 0, 244, 197, 255,
 95, 4, 1, 156, 127, 255, 150, 26, 155, 0, 4, 44, 36, 0, 239,
 193, 255, 176, 19, 4, 215, 175, 2, 22, 18, 232, 0, 207, 0, 10,
 133, 108, 1, 255, 207, 2, 255, 207, 2, 2, 1, 151, 0, 1, 142,
 117, 255, 35, 7, 255, 2, 15, 4, 241, 197, 2, 28, 22, 211, 0,
 4, 4, 3, 0, 244, 197, 255, 98, 4, 1, 156, 127, 255, 150, 26,
 158, 0, 2, 63, 52, 0, 255, 182, 21, 4, 217, 176, 2, 22, 18,
 229, 0, 207, 0, 10, 133, 108, 1, 255, 207, 2, 255, 207, 2, 2,
 1, 148, 0, 4, 55, 44, 0, 241, 197, 255, 32, 4, 255, 2, 18,
 1, 85, 70, 214, 0, 3, 4, 3, 0, 244, 255, 116, 5, 1, 156,
 127, 255, 150, 23, 164, 0, 4, 63, 52, 0, 239, 195, 255, 185, 19,
 4, 211, 169, 2, 21, 17, 226, 0, 207, 0, 10, 133, 108, 1, 255,
 207, 2, 255, 207, 2, 2, 1, 145, 0, 4, 8, 6, 0, 195, 157,
 255, 32, 7, 255, 2, 15, 4, 162, 131, 1, 5, 4, 214, 0, 4,
 4, 3, 0, 244, 197, 255, 101, 4, 1, 156, 127, 255, 150, 20, 170,
 0, 4, 69, 55, 0, 248, 201, 255, 191, 19, 4, 231, 187, 2, 29,
 24, 223, 0, 207, 0, 10, 164, 133, 1, 255, 207, 2, 255, 207, 2,
 2, 1, 145, 0, 4, 114, 91, 1, 250, 203, 255, 29, 4, 255, 2,
 15, 4, 223, 180, 2, 23, 19, 217, 0, 4, 4, 3, 0, 244, 197,
 255, 104, 4, 1, 156, 127, 255, 150, 20, 173, 0, 2, 79, 63, 0,
 255, 194, 18, 255, 2, 3, 4, 215, 175, 2, 26, 21, 220, 0, 207,
 0, 10, 164, 133, 1, 255, 207, 2, 255, 207, 2, 2, 1, 142, 0,
 4, 29, 23, 0, 221, 178, 255, 26, 4, 255, 2, 15, 4, 252, 205,
 2, 78, 63, 220, 0, 4, 4, 3, 0, 244, 197, 255, 107, 4, 1,
 156, 127, 255, 150, 17, 179, 0, 4, 70, 56, 0, 239, 195, 255, 203,
 19, 4, 246, 201, 2, 40, 32, 217, 0, 207, 0, 10, 164, 133, 1,
 255, 207, 2, 255, 207, 2, 2, 1, 139, 0, 4, 4, 3, 0, 156,
 125, 255, 26, 7, 255, 2, 15, 4, 162, 131, 1, 4, 3, 220, 0,
 255, 95, 3, 1, 244, 197, 255, 107, 4, 1, 156, 127, 255, 150, 14,
 185, 0, 4, 72, 57, 0, 252, 205, 255, 206, 19, 4, 227, 184, 2,
 40, 32, 214, 0, 207, 0, 10, 164, 133, 1, 255, 207, 2, 255, 207,
 2, 2, 1, 139, 0, 4, 57, 47, 0, 252, 205, 255, 23, 4, 255,
 2, 15, 4, 223, 180, 2, 32, 25, 223, 0, 4, 4, 3, 0, 244,
 197, 255, 110, 4, 1, 156, 127, 255, 150, 14, 188, 0, 4, 60, 49,
 0, 235, 191, 255, 212, 19, 255, 20, 3, 1, 62, 51, 211, 0, 207,
 0, 10, 136, 111, 1, 255, 207, 2, 255, 207, 2, 2, 1, 136, 0,
 4, 3, 2, 0, 223, 182, 255, 20, 4, 255, 2, 15, 4, 250, 203,
 2, 108, 87, 255, 39, 11, 215, 0, 4, 4, 3, 0, 244, 197, 255,
 113, 4, 1, 156, 127, 255, 110, 79, 4, 57, 46, 0, 244, 199, 255,
 218, 19, 4, 246, 199, 2, 58, 48, 208, 0, 207, 0, 10, 133, 108,
 1, 255, 207, 2, 255, 207, 2, 2, 1, 136, 0, 1, 141, 114, 255,
 20, 7, 255, 2, 15, 4, 205, 166, 1, 5, 4, 226, 0, 4, 4,
 3, 0, 244, 197, 255, 113, 4, 1, 156, 127, 255, 150, 11, 197, 0,
 4, 59, 48, 0, 239, 195, 255, 221, 19, 4, 241, 197, 2, 84, 69,
 205, 0, 207, 0, 10, 133, 108, 1, 255, 207, 2, 255, 207, 2, 2,
 1, 133, 0, 4, 35, 28, 0, 229, 186, 255, 17, 4, 255, 2, 18,
 1, 40, 33, 229, 0, 4, 4, 3, 0, 244, 197, 255, 113, 4, 1,
 156, 127, 255, 150, 8, 203, 0, 4, 51, 41, 0, 237, 191, 255, 224,
 19, 4, 250, 203, 2, 93, 75, 202, 0, 207, 0, 10, 133, 108, 1,
 255, 207, 2, 255, 207, 2, 2, 1, 130, 0, 255, 5, 3, 1, 138,
 112, 255, 17, 7, 255, 2, 15, 1, 121, 97, 255, 33, 5, 227, 0,
 4, 4, 3, 0, 244, 197, 255, 116, 4, 1, 156, 127, 255, 116, 88,
 4, 40, 33, 0, 227, 186, 255, 230, 19, 255, 2, 3, 4, 146, 118,
 1, 5, 4, 196, 0, 207, 0, 10, 133, 108, 1, 255, 207, 2, 255,
 207, 2, 2, 1, 130, 0, 4, 40, 32, 0, 246, 201, 255, 14, 4,
 255, 2, 15, 4, 209, 169, 2, 9, 7, 232, 0, 4, 4, 3, 0,
 244, 197, 255, 119, 4, 1, 156, 127, 255, 150, 5, 212, 0, 4, 27,
 22, 0, 215, 173, 255, 236, 19, 255, 2, 3, 4, 154, 125, 1, 6,
 5, 193, 0, 207, 0, 10, 133, 108, 1, 255, 207, 2, 255, 207, 2,
 2, 1, 130, 0, 1, 191, 156, 255, 14, 7, 255, 2, 12, 4, 250,
 203, 2, 56, 45, 235, 0, 128, 1, 2, 0, 244, 197, 255, 122, 4,
 1, 156, 127, 255, 150, 5, 215, 0, 4, 19, 16, 0, 207, 168, 255,
 242, 19, 255, 2, 3, 4, 197, 159, 1, 19, 16, 190, 0, 207, 0,
 7, 133, 108, 1, 255, 207, 2, 255, 207, 129, 2, 2, 0, 103, 84,
 255, 11, 7, 255, 2, 15, 4, 147, 119, 1, 3, 3, 238, 0, 1,
 244, 197, 255, 122, 4, 2, 156, 127, 1, 222, 0, 4, 11, 9, 0,
 173, 139, 255, 248, 22, 4, 207, 168, 1, 33, 26, 187, 0, 207, 0,
 13, 133, 108, 1, 255, 207, 2, 255, 207, 2, 33, 27, 0, 213, 171,
 255, 8, 4, 255, 2, 15, 1, 227, 184, 255, 26, 4, 240, 0, 1,
 244, 197, 255, 125, 4, 2, 156, 127, 1, 225, 0, 7, 6, 5, 0,
 151, 122, 1, 252, 205, 255, 254, 19, 4, 235, 191, 2, 53, 43, 184,
 0, 207, 0, 13, 133, 108, 1, 255, 207, 2, 255, 207, 2, 146, 118,
 1, 252, 205, 255, 8, 4, 255, 2, 15, 2, 142, 114, 1, 243, 0,
 1, 244, 197, 255, 125, 4, 1, 168, 138, 255, 125, 103, 128, 2, 3,
 0, 131, 107, 1, 255, 254, 18, 255, 2, 6, 1, 100, 80, 181, 0,
 207, 0, 10, 130, 105, 1, 255, 207, 2, 255, 207, 2, 250, 203, 255,
 5, 4, 255, 2, 15, 4, 239, 193, 2, 7, 6, 244, 0, 1, 244,
 197, 255, 128, 4, 2, 201, 164, 1, 234, 0, 1, 84, 68, 255, 119,
 7, 255, 2, 15, 2, 211, 171, 2, 180, 0, 207, 0, 5, 128, 104,
 1, 255, 207, 2, 255, 2, 24, 1, 65, 53, 247, 0, 1, 244, 197,
 255, 128, 4, 2, 205, 166, 1, 237, 0, 4, 66, 54, 0, 244, 199,
 255, 254, 7, 255, 2, 9, 2, 217, 175, 2, 180, 0, 207, 0, 5,
 128, 104, 1, 255, 207, 2, 255, 2, 21, 4, 169, 138, 1, 5, 4,
 247, 0, 1, 244, 197, 255, 131, 4, 4, 241, 195, 2, 36, 29, 238,
 0, 4, 31, 25, 0, 215, 175, 255, 125, 4, 255, 2, 9, 2, 233,
 189, 2, 180, 0, 207, 0, 5, 128, 104, 1, 255, 207, 2, 255, 2,
 18, 4, 239, 195, 2, 53, 43, 250, 0, 1, 244, 197, 255, 137, 7,
 4, 217, 176, 2, 45, 36, 238, 0, 5, 15, 13, 0, 182, 149, 1,
 255, 128, 6, 255, 2, 3, 2, 233, 189, 2, 180, 0, 207, 0, 5,
 128, 104, 1, 255, 207, 2, 255, 2, 18, 3, 184, 149, 1, 1, 251,
 0, 1, 235, 191, 255, 140, 10, 4, 223, 180, 2, 59, 49, 238, 0,
 7, 5, 4, 0, 151, 122, 1, 250, 203, 255, 128, 4, 2, 233, 189,
 2, 180, 0, 207, 0, 5, 124, 100, 1, 255, 207, 2, 255, 2, 15,
 4, 248, 203, 2, 26, 21, 253, 0, 1, 233, 189, 255, 146, 13, 6,
 250, 203, 2, 75, 60, 0, 1, 239, 0, 8, 193, 157, 1, 255, 207,
 2, 233, 189, 2, 180, 0, 207, 0, 5, 117, 95, 1, 255, 207, 2,
 255, 2, 15, 255, 20, 3, 253, 0, 128, 0, 1, 233, 189, 255, 149,
 16, 4, 244, 199, 2, 96, 79, 238, 0, 10, 173, 141, 1, 255, 207,
 2, 233, 189, 2, 1, 1, 178, 0, 207, 0, 5, 117, 95, 1, 255,
 207, 2, 255, 2, 12, 4, 221, 178, 2, 22, 18, 253, 0, 129, 0,
 1, 233, 189, 255, 149, 13, 255, 2, 6, 5, 246, 201, 2, 136, 111,
 1, 234, 0, 9, 173, 141, 1, 255, 207, 2, 233, 189, 2, 2, 255,
 118, 53, 207, 0, 5, 117, 95, 1, 255, 207, 2, 255, 2, 9, 5,
 252, 205, 2, 128, 105, 1, 253, 0, 131, 0, 1, 233, 189, 255, 149,
 10, 255, 2, 12, 2, 235, 191, 2, 234, 0, 10, 173, 141, 1, 255,
 207, 2, 233, 189, 2, 2, 1, 178, 0, 207, 0, 5, 117, 95, 1,
 255, 207, 2, 255, 2, 9, 4, 239, 195, 2, 8, 7, 253, 0, 132,
 0, 1, 157, 128, 255, 152, 13, 255, 2, 9, 4, 235, 191, 2, 3,
 2, 232, 0, 1, 173, 141, 255, 134, 4, 4, 241, 197, 2, 2, 1,
 178, 0, 207, 0, 5, 117, 95, 1, 255, 207, 2, 255, 2, 9, 1,
 85, 68, 253, 0, 135, 0, 4, 9, 8, 0, 157, 128, 255, 155, 13,
 255, 2, 6, 4, 235, 191, 2, 3, 2, 232, 0, 1, 173, 141, 255,
 131, 7, 1, 2, 1, 178, 0, 207, 0, 5, 117, 95, 1, 255, 207,
 2, 255, 2, 6, 4, 209, 169, 1, 13, 10, 253, 0, 138, 0, 7,
 4, 3, 0, 136, 111, 1, 248, 201, 255, 158, 7, 255, 2, 6, 4,
 235, 191, 2, 3, 2, 232, 0, 2, 166, 135, 1, 255, 119, 6, 1,
 2, 1, 178, 0, 207, 0, 5, 93, 75, 0, 255, 207, 2, 255, 2,
 6, 2, 121, 97, 1, 253, 0, 143, 0, 128, 1, 5, 0, 100, 82,
 0, 250, 203, 255, 161, 7, 255, 2, 3, 4, 244, 197, 2, 3, 2,
 232, 0, 2, 139, 112, 1, 255, 119, 6, 255, 116, 54, 207, 0, 10,
 93, 75, 0, 255, 207, 2, 255, 207, 2, 227, 184, 129, 2, 253, 0,
 147, 0, 128, 1, 5, 0, 84, 68, 0, 235, 191, 255, 164, 4, 255,
 2, 6, 0, 3, 255, 167, 107, 2, 139, 112, 1, 255, 116, 6, 1,
 5, 4, 178, 0, 207, 0, 10, 93, 75, 0, 255, 207, 2, 255, 207,
 2, 75, 61, 253, 0, 156, 0, 4, 45, 36, 0, 235, 191, 255, 167,
 4, 255, 2, 3, 1, 3, 2, 232, 0, 2, 136, 109, 1, 255, 116,
 6, 1, 5, 4, 178, 0, 207, 0, 10, 93, 75, 0, 255, 207, 2,
 217, 176, 2, 13, 10, 253, 0, 159, 0, 10, 157, 127, 1, 255, 207,
 2, 255, 207, 2, 3, 2, 232, 0, 1, 131, 107, 255, 116, 7, 1,
 8, 6, 178, 0, 207, 0, 8, 93, 75, 0, 255, 207, 2, 109, 88,
 1, 253, 0, 161, 0, 10, 139, 112, 1, 255, 207, 2, 255, 207, 2,
 7, 5, 232, 0, 1, 131, 107, 255, 116, 7, 1, 9, 7, 178, 0,
 207, 0, 7, 85, 69, 0, 219, 176, 2, 4, 3, 253, 0, 162, 0,
 10, 139, 112, 1, 255, 207, 2, 255, 207, 2, 7, 5, 151, 0, 4,
 28, 22, 0, 1, 1, 202, 0, 2, 101, 82, 0, 255, 116, 6, 1,
 10, 8, 178, 0, 207, 0, 5, 82, 67, 0, 109, 88, 1, 253, 0,
 164, 0, 10, 131, 107, 1, 255, 207, 2, 255, 207, 2, 8, 6, 151,
 0, 7, 105, 85, 1, 133, 108, 1, 4, 3, 199, 0, 2, 101, 82,
 0, 255, 116, 6, 1, 15, 12, 178, 0, 207, 0, 4, 80, 65, 0,
 19, 15, 253, 0, 165, 0, 10, 131, 107, 1, 255, 207, 2, 255, 207,
 2, 9, 7, 151, 0, 10, 97, 79, 0, 255, 207, 2, 187, 152, 1,
 17, 14, 196, 0, 255, 80, 6, 255, 2, 3, 1, 16, 13, 178, 0,
 207, 0, 1, 12, 10, 253, 0, 168, 0, 10, 125, 101, 1, 255, 207,
 2, 255, 207, 2, 9, 7, 151, 0, 2, 96, 78, 0, 255, 35, 6,
 4, 203, 164, 1, 27, 22, 193, 0, 1, 95, 77, 255, 80, 7, 1,
 21, 16, 178, 0, 207, 0, 0, 1, 253, 0, 169, 0, 10, 101, 82,
 0, 255, 207, 2, 255, 207, 2, 12, 10, 151, 0, 1, 88, 72, 255,
 35, 7, 255, 2, 3, 4, 246, 199, 2, 55, 45, 190, 0, 1, 80,
 65, 255, 80, 7, 1, 21, 17, 178, 0, 253, 0, 251, 0, 10, 101,
 82, 0, 255, 207, 2, 255, 207, 2, 15, 12, 151, 0, 1, 88, 72,
 255, 35, 7, 255, 2, 6, 7, 237, 193, 2, 105, 86, 1, 2, 1,
 184, 0, 1, 71, 57, 255, 80, 7, 1, 30, 24, 178, 0, 253, 0,
 251, 0, 10, 96, 78, 0, 255, 207, 2, 255, 207, 2, 16, 13, 151,
 0, 1, 69, 56, 255, 35, 7, 255, 2, 9, 7, 252, 207, 2, 131,
 108, 1, 6, 4, 181, 0, 1, 68, 55, 255, 80, 7, 1, 31, 25,
 178, 0, 253, 0, 251, 0, 10, 95, 77, 0, 255, 207, 2, 255, 207,
 2, 21, 16, 151, 0, 1, 67, 55, 255, 35, 7, 255, 2, 15, 4,
 180, 147, 1, 17, 13, 178, 0, 1, 66, 53, 255, 80, 7, 1, 37,
 30, 178, 0, 253, 0, 251, 0, 10, 85, 69, 0, 255, 207, 2, 255,
 207, 2, 21, 16, 151, 0, 1, 62, 51, 255, 35, 7, 255, 2, 18,
 4, 237, 191, 2, 51, 41, 175, 0, 255, 50, 3, 255, 62, 6, 1,
 40, 33, 178, 0, 253, 0, 251, 0, 10, 71, 57, 0, 255, 207, 2,
 255, 207, 2, 29, 23, 151, 0, 1, 60, 49, 255, 35, 7, 255, 2,
 21, 7, 235, 191, 2, 91, 74, 0, 1, 1, 169, 0, 1, 45, 37,
 255, 80, 7, 1, 50, 40, 178, 0, 253, 0, 218, 0, 7, 5, 4,
 0, 32, 27, 0, 6, 5, 151, 0, 10, 70, 56, 0, 255, 207, 2,
 255, 207, 2, 30, 24, 151, 0, 1, 50, 40, 255, 35, 7, 255, 2,
 24, 7, 252, 205, 2, 142, 115, 1, 10, 8, 166, 0, 1, 41, 33,
 255, 80, 7, 1, 56, 45, 178, 0, 253, 0, 206, 0, 128, 1, 17,
 0, 19, 15, 0, 65, 53, 0, 138, 112, 1, 186, 151, 1, 215, 175,
 2, 25, 20, 151, 0, 10, 67, 54, 0, 255, 207, 2, 255, 207, 2,
 36, 29, 151, 0, 1, 43, 35, 255, 35, 7, 1, 223, 180, 255, 5,
 4, 255, 2, 24, 4, 189, 154, 1, 23, 18, 163, 0, 1, 38, 31,
 255, 80, 7, 1, 60, 49, 178, 0, 253, 0, 197, 0, 19, 6, 5,
 0, 37, 30, 0, 95, 77, 0, 164, 133, 1, 195, 159, 1, 231, 187,
 2, 255, 207, 255, 2, 7, 1, 30, 24, 151, 0, 2, 67, 54, 0,
 255, 35, 6, 255, 59, 3, 150, 0, 1, 40, 32, 255, 35, 7, 7,
 82, 68, 0, 84, 68, 0, 237, 193, 255, 89, 10, 255, 2, 15, 4,
 246, 199, 2, 69, 55, 160, 0, 1, 27, 21, 255, 80, 7, 1, 74,
 60, 178, 0, 253, 0, 188, 0, 19, 13, 10, 0, 52, 42, 0, 121,
 99, 1, 169, 138, 1, 207, 169, 1, 246, 199, 2, 255, 207, 255, 2,
 16, 1, 31, 25, 151, 0, 2, 50, 40, 0, 255, 35, 6, 1, 45,
 36, 151, 0, 1, 37, 30, 255, 35, 7, 1, 60, 48, 130, 0, 4,
 44, 36, 0, 213, 173, 255, 101, 19, 255, 2, 6, 7, 250, 203, 2,
 139, 114, 1, 4, 3, 154, 0, 1, 25, 20, 255, 80, 7, 1, 81,
 66, 178, 0, 253, 0, 179, 0, 19, 16, 13, 0, 59, 49, 0, 127,
 104, 1, 171, 139, 1, 217, 175, 2, 252, 205, 2, 255, 207, 255, 2,
 25, 1, 45, 36, 151, 0, 2, 49, 40, 0, 255, 35, 7, 0, 37,
 151, 0, 1, 27, 21, 255, 35, 7, 1, 72, 58, 133, 0, 7, 14,
 11, 0, 186, 151, 1, 250, 203, 255, 113, 25, 4, 211, 171, 1, 27,
 22, 151, 0, 1, 23, 19, 255, 80, 7, 1, 88, 72, 178, 0, 253,
 0, 170, 0, 16, 15, 12, 0, 57, 46, 0, 122, 99, 1, 169, 138,
 1, 219, 178, 2, 255, 207, 255, 2, 37, 1, 45, 36, 151, 0, 255,
 26, 3, 255, 35, 6, 1, 55, 45, 151, 0, 1, 24, 19, 255, 35,
 7, 1, 86, 70, 136, 0, 7, 2, 1, 0, 111, 90, 1, 250, 203,
 255, 116, 25, 4, 233, 189, 2, 80, 65, 148, 0, 1, 13, 10, 255,
 80, 7, 2, 104, 85, 1, 177, 0, 253, 0, 161, 0, 16, 12, 9,
 0, 53, 43, 0, 115, 93, 1, 164, 133, 1, 217, 175, 2, 255, 207,
 255, 2, 46, 1, 50, 40, 151, 0, 2, 41, 33, 0, 255, 35, 6,
 1, 56, 46, 151, 0, 1, 21, 16, 255, 35, 7, 1, 87, 71, 142,
 0, 4, 46, 37, 0, 227, 184, 255, 119, 25, 7, 248, 201, 2, 147,
 119, 1, 8, 6, 255, 206, 19, 255, 32, 6, 2, 112, 92, 1, 177,
 0, 253, 0, 152, 0, 19, 6, 5, 0, 40, 31, 0, 97, 79, 0,
 149, 121, 1, 209, 169, 1, 252, 205, 2, 255, 207, 255, 2, 52, 1,
 52, 42, 151, 0, 2, 30, 24, 0, 255, 35, 6, 1, 71, 57, 151,
 0, 1, 17, 14, 255, 35, 7, 1, 91, 74, 145, 0, 5, 13, 11,
 0, 168, 136, 1, 255, 125, 27, 1, 29, 23, 142, 0, 1, 10, 8,
 255, 80, 7, 2, 124, 100, 1, 177, 0, 253, 0, 143, 0, 128, 1,
 17, 0, 23, 19, 0, 73, 59, 0, 128, 104, 1, 189, 154, 1, 248,
 201, 2, 255, 207, 255, 2, 61, 1, 69, 55, 151, 0, 2, 28, 22,
 0, 255, 35, 6, 255, 110, 3, 150, 0, 1, 11, 9, 255, 35, 7,
 2, 121, 97, 1, 147, 0, 7, 4, 3, 0, 97, 79, 0, 252, 205,
 255, 119, 16, 4, 250, 203, 2, 71, 57, 255, 47, 22, 255, 32, 6,
 1, 144, 118, 255, 80, 22, 156, 0, 253, 0, 137, 0, 16, 9, 7,
 0, 46, 37, 0, 99, 80, 0, 161, 130, 1, 231, 187, 2, 255, 207,
 255, 2, 70, 1, 70, 56, 151, 0, 2, 25, 21, 0, 255, 35, 6,
 1, 85, 69, 151, 0, 1, 10, 8, 255, 35, 7, 2, 122, 99, 1,
 150, 0, 4, 36, 30, 0, 252, 205, 255, 116, 13, 255, 14, 3, 1,
 117, 95, 255, 47, 22, 128, 3, 255, 80, 7, 1, 156, 125, 255, 80,
 25, 153, 0, 253, 0, 131, 0, 16, 20, 16, 0, 67, 55, 0, 121,
 99, 1, 195, 157, 1, 252, 205, 2, 255, 207, 255, 2, 76, 1, 77,
 62, 151, 0, 2, 18, 14, 0, 255, 35, 6, 1, 92, 75, 151, 0,
 1, 8, 6, 255, 35, 7, 2, 124, 100, 1, 147, 0, 5, 6, 5,
 0, 205, 166, 1, 255, 116, 15, 4, 168, 136, 1, 3, 2, 148, 0,
 1, 2, 1, 255, 80, 7, 1, 166, 135, 255, 80, 22, 156, 0, 249,
 0, 16, 4, 3, 0, 33, 27, 0, 81, 66, 0, 146, 118, 1, 227,
 184, 2, 255, 207, 255, 2, 73, 5, 213, 173, 2, 138, 111, 1, 255,
 11, 6, 1, 87, 71, 151, 0, 2, 15, 12, 0, 255, 35, 6, 2,
 101, 82, 1, 150, 0, 128, 3, 255, 35, 7, 1, 157, 128, 255, 35,
 22, 1, 171, 138, 255, 104, 7, 255, 2, 9, 4, 219, 176, 2, 10,
 9, 154, 0, 255, 38, 6, 1, 191, 156, 255, 116, 25, 153, 0, 243,
 0, 16, 8, 6, 0, 43, 35, 0, 87, 71, 0, 166, 135, 1, 244,
 199, 2, 255, 207, 255, 2, 70, 7, 229, 186, 2, 104, 85, 1, 19,
 15, 130, 0, 2, 22, 17, 0, 255, 20, 6, 1, 100, 81, 151, 0,
 1, 12, 10, 255, 35, 7, 2, 117, 95, 1, 150, 0, 128, 3, 2,
 0, 248, 201, 255, 35, 4, 1, 161, 131, 255, 35, 19, 2, 109, 88,
 1, 255, 131, 15, 4, 250, 203, 2, 31, 25, 157, 0, 255, 41, 6,
 2, 209, 168, 2, 177, 0, 237, 0, 14, 10, 8, 0, 49, 40, 0,
 97, 79, 0, 182, 149, 1, 255, 207, 2, 255, 2, 69, 10, 250, 203,
 2, 138, 112, 1, 35, 29, 0, 1, 1, 136, 0, 2, 16, 13, 0,
 255, 29, 6, 1, 105, 86, 255, 21, 11, 140, 0, 1, 9, 7, 255,
 35, 7, 1, 124, 101, 255, 35, 25, 128, 3, 2, 0, 239, 195, 255,
 35, 4, 1, 161, 131, 255, 35, 16, 4, 46, 37, 0, 252, 205, 255,
 140, 16, 1, 74, 60, 160, 0, 1, 229, 186, 255, 41, 4, 2, 213,
 171, 2, 177, 0, 231, 0, 14, 12, 9, 0, 49, 40, 0, 101, 82,
 0, 199, 161, 1, 255, 207, 2, 255, 2, 69, 7, 197, 161, 1, 66,
 54, 0, 7, 5, 145, 0, 2, 14, 12, 0, 255, 35, 6, 2, 108,
 88, 1, 150, 0, 1, 5, 4, 255, 35, 7, 1, 138, 111, 255, 35,
 25, 129, 0, 1, 239, 195, 255, 35, 4, 1, 201, 162, 255, 35, 13,
 4, 13, 11, 0, 231, 187, 255, 143, 16, 0, 136, 255, 71, 29, 135,
 0, 1, 213, 171, 255, 44, 4, 2, 219, 176, 2, 177, 0, 225, 0,
 14, 11, 9, 0, 46, 38, 0, 99, 80, 0, 203, 164, 1, 255, 207,
 2, 255, 2, 66, 9, 246, 199, 2, 121, 99, 1, 25, 21, 0, 1,
 152, 0, 2, 9, 7, 0, 255, 44, 6, 1, 136, 111, 255, 37, 25,
 1, 4, 3, 255, 35, 7, 1, 147, 121, 255, 73, 27, 2, 0, 231,
 187, 255, 35, 4, 1, 205, 166, 255, 35, 10, 128, 2, 2, 0, 178,
 144, 255, 203, 16, 3, 187, 152, 1, 2, 255, 70, 28, 136, 0, 1,
 209, 168, 255, 47, 4, 2, 233, 189, 2, 177, 0, 219, 0, 14, 8,
 7, 0, 44, 35, 0, 95, 77, 0, 201, 162, 1, 255, 207, 2, 255,
 2, 66, 7, 209, 169, 1, 71, 57, 0, 8, 6, 160, 0, 2, 7,
 6, 0, 255, 50, 6, 2, 141, 114, 1, 150, 0, 128, 3, 255, 35,
 7, 1, 159, 128, 255, 35, 25, 129, 0, 1, 225, 184, 255, 122, 7,
 135, 0, 1, 103, 84, 255, 56, 7, 255, 2, 9, 4, 219, 178, 2,
 17, 13, 166, 0, 9, 173, 141, 1, 255, 207, 2, 233, 189, 2, 2,
 255, 118, 28, 148, 0, 213, 0, 14, 5, 4, 0, 36, 29, 0, 81,
 66, 0, 191, 156, 1, 255, 207, 2, 255, 2, 66, 7, 161, 131, 1,
 42, 34, 0, 3, 2, 255, 128, 43, 255, 56, 6, 2, 147, 119, 1,
 150, 0, 128, 1, 255, 35, 7, 1, 173, 141, 255, 35, 25, 129, 0,
 1, 225, 184, 255, 131, 7, 132, 0, 4, 43, 35, 0, 250, 203, 255,
 155, 13, 4, 246, 199, 2, 53, 43, 169, 0, 255, 110, 3, 255, 53,
 3, 3, 252, 205, 2, 2, 255, 118, 28, 148, 0, 207, 0, 128, 1,
 12, 0, 27, 22, 0, 66, 54, 0, 169, 138, 1, 255, 207, 2, 255,
 2, 63, 10, 250, 203, 2, 125, 101, 1, 23, 19, 0, 2, 1, 172,
 0, 2, 3, 2, 0, 255, 65, 6, 1, 173, 141, 255, 57, 28, 255,
 35, 6, 1, 193, 157, 255, 35, 28, 1, 215, 173, 255, 140, 7, 129,
 0, 4, 11, 9, 0, 217, 175, 255, 164, 16, 1, 108, 88, 255, 156,
 46, 2, 149, 121, 1, 255, 56, 6, 1, 4, 3, 175, 0, 207, 0,
 10, 12, 9, 0, 139, 114, 1, 248, 201, 2, 255, 207, 255, 2, 61,
 10, 237, 191, 2, 100, 81, 0, 13, 11, 0, 1, 1, 178, 0, 2,
 2, 1, 0, 255, 131, 6, 1, 180, 146, 255, 63, 28, 1, 239, 195,
 255, 35, 4, 1, 199, 161, 255, 35, 28, 2, 197, 159, 1, 255, 146,
 6, 4, 5, 4, 0, 154, 124, 255, 11, 7, 255, 2, 9, 4, 178,
 144, 1, 3, 2, 172, 0, 1, 135, 109, 255, 68, 7, 1, 7, 6,
 175, 0, 207, 0, 5, 35, 28, 0, 255, 207, 2, 255, 2, 60, 10,
 229, 186, 2, 85, 68, 0, 13, 10, 0, 1, 1, 184, 0, 4, 2,
 1, 0, 233, 189, 255, 77, 4, 1, 180, 146, 255, 69, 28, 1, 219,
 178, 255, 35, 4, 1, 199, 161, 255, 35, 28, 2, 175, 141, 1, 255,
 152, 6, 4, 112, 92, 1, 252, 205, 255, 170, 13, 4, 225, 182, 2,
 17, 13, 175, 0, 1, 119, 96, 255, 80, 7, 1, 11, 9, 175, 0,
 207, 0, 5, 31, 25, 0, 255, 207, 2, 255, 2, 54, 10, 229, 186,
 2, 84, 68, 0, 11, 9, 0, 1, 1, 193, 0, 1, 233, 189, 255,
 83, 4, 1, 207, 169, 255, 75, 28, 1, 219, 178, 255, 35, 4, 2,
 225, 184, 2, 153, 0, 2, 164, 133, 1, 255, 158, 6, 1, 246, 201,
 255, 173, 13, 4, 250, 203, 2, 50, 40, 178, 0, 2, 100, 81, 0,
 255, 65, 6, 1, 17, 14, 175, 0, 207, 0, 5, 31, 25, 0, 255,
 207, 2, 255, 2, 48, 10, 229, 186, 2, 86, 70, 0, 13, 10, 0,
 2, 1, 199, 0, 1, 227, 184, 255, 89, 4, 2, 215, 175, 2, 153,
 0, 7, 199, 161, 1, 255, 207, 2, 225, 184, 255, 35, 28, 1, 139,
 112, 255, 35, 4, 255, 2, 18, 1, 119, 97, 255, 171, 55, 2, 90,
 72, 0, 255, 65, 6, 1, 23, 18, 175, 0, 207, 0, 5, 30, 24,
 0, 255, 207, 2, 255, 2, 42, 10, 237, 193, 2, 101, 82, 1, 15,
 13, 0, 2, 2, 205, 0, 1, 213, 171, 255, 95, 4, 1, 215, 175,
 255, 87, 28, 10, 186, 151, 1, 255, 207, 2, 233, 191, 2, 1, 1,
 151, 0, 1, 127, 103, 255, 35, 4, 255, 2, 15, 3, 201, 162, 1,
 3, 255, 176, 56, 2, 70, 56, 0, 255, 68, 6, 1, 31, 25, 175,
 0, 207, 0, 5, 22, 18, 0, 255, 207, 2, 255, 2, 36, 10, 250,
 203, 2, 122, 99, 1, 23, 18, 0, 4, 3, 211, 0, 1, 213, 171,
 255, 101, 7, 153, 0, 2, 186, 151, 1, 255, 140, 6, 128, 1, 151,
 0, 1, 112, 91, 255, 35, 7, 255, 2, 9, 4, 246, 201, 2, 21,
 17, 184, 0, 2, 63, 51, 0, 255, 71, 6, 1, 40, 32, 175, 0,
 207, 0, 5, 16, 13, 0, 255, 207, 2, 255, 2, 33, 7, 161, 130,
 1, 33, 27, 0, 6, 5, 217, 0, 2, 205, 164, 1, 255, 107, 6,
 153, 0, 1, 159, 130, 255, 35, 7, 0, 3, 255, 37, 26, 1, 96,
 78, 255, 209, 16, 1, 77, 62, 187, 0, 1, 45, 37, 255, 80, 7,
 1, 52, 42, 175, 0, 207, 0, 5, 15, 13, 0, 255, 207, 2, 255,
 2, 27, 7, 205, 168, 1, 57, 47, 0, 10, 8, 223, 0, 2, 191,
 156, 1, 255, 113, 6, 128, 1, 151, 0, 1, 147, 119, 255, 35, 7,
 1, 5, 4, 151, 0, 1, 88, 72, 255, 209, 13, 1, 157, 128, 255,
 78, 26, 164, 0, 1, 40, 32, 255, 80, 7, 1, 63, 52, 175, 0,
 207, 0, 5, 15, 12, 0, 255, 207, 2, 255, 2, 21, 10, 239, 195,
 2, 96, 78, 0, 17, 14, 0, 2, 2, 226, 0, 2, 164, 133, 1,
 255, 119, 6, 1, 4, 3, 151, 0, 1, 131, 107, 255, 35, 7, 1,
 7, 5, 151, 0, 1, 67, 55, 255, 209, 10, 4, 229, 186, 2, 8,
 6, 190, 0, 1, 27, 21, 255, 80, 7, 1, 79, 63, 175, 0, 207,
 0, 5, 10, 8, 0, 255, 207, 2, 255, 2, 18, 7, 156, 127, 1,
 34, 27, 0, 6, 5, 232, 0, 2, 152, 124, 1, 255, 122, 6, 1,
 5, 4, 151, 0, 1, 115, 92, 255, 35, 7, 255, 182, 3, 150, 0,
 1, 61, 50, 255, 209, 7, 4, 252, 205, 2, 49, 40, 193, 0, 1,
 22, 18, 255, 80, 7, 1, 92, 75, 175, 0, 207, 0, 5, 6, 5,
 0, 255, 207, 2, 255, 2, 12, 9, 225, 184, 2, 70, 57, 0, 14,
 11, 0, 1, 236, 0, 2, 141, 114, 1, 255, 128, 6, 1, 8, 7,
 151, 0, 1, 107, 86, 255, 35, 7, 1, 15, 13, 151, 0, 1, 48,
 39, 255, 209, 7, 1, 133, 107, 255, 193, 70, 1, 13, 10, 255, 80,
 7, 1, 111, 91, 255, 80, 49, 207, 0, 7, 6, 5, 0, 252, 205,
 2, 255, 207, 255, 2, 7, 7, 136, 111, 1, 29, 23, 0, 5, 4,
 241, 0, 2, 122, 99, 1, 255, 131, 6, 1, 13, 10, 151, 0, 2,
 91, 73, 0, 255, 35, 6, 1, 21, 16, 151, 0, 1, 40, 32, 255,
 35, 4, 4, 219, 178, 2, 4, 3, 196, 0, 1, 17, 14, 255, 116,
 7, 2, 130, 105, 1, 174, 0, 207, 0, 16, 6, 5, 0, 250, 205,
 2, 255, 207, 2, 231, 187, 2, 66, 54, 0, 15, 13, 247, 0, 4,
 108, 88, 1, 255, 207, 255, 137, 4, 1, 15, 12, 151, 0, 2, 80,
 66, 0, 255, 35, 6, 1, 29, 23, 151, 0, 7, 31, 25, 0, 252,
 205, 2, 37, 30, 199, 0, 1, 125, 101, 255, 152, 7, 2, 154, 125,
 1, 174, 0, 207, 0, 13, 2, 1, 0, 250, 205, 2, 255, 207, 2,
 197, 161, 1, 6, 5, 250, 0, 4, 92, 75, 0, 255, 207, 255, 137,
 4, 1, 25, 21, 151, 0, 1, 67, 55, 255, 35, 7, 1, 36, 29,
 151, 0, 5, 23, 18, 0, 117, 93, 1, 198, 0, 4, 62, 50, 0,
 250, 203, 255, 113, 4, 255, 2, 3, 1, 175, 142, 255, 86, 49, 207,
 0, 128, 1, 9, 0, 252, 205, 2, 255, 207, 2, 175, 142, 1, 252,
 0, 4, 82, 68, 0, 255, 207, 255, 137, 4, 1, 29, 23, 151, 0,
 1, 56, 45, 255, 35, 7, 1, 45, 36, 151, 0, 4, 9, 7, 0,
 4, 3, 196, 0, 4, 21, 16, 0, 229, 186, 255, 110, 4, 255, 2,
 6, 2, 201, 162, 1, 174, 0, 207, 0, 128, 1, 9, 0, 252, 205,
 2, 255, 207, 2, 182, 149, 1, 252, 0, 4, 72, 58, 0, 255, 207,
 255, 137, 4, 1, 38, 31, 151, 0, 1, 46, 37, 255, 35, 7, 1,
 55, 45, 223, 0, 5, 3, 2, 0, 189, 154, 1, 255, 110, 6, 255,
 2, 6, 2, 203, 164, 1, 174, 0, 207, 0, 128, 1, 9, 0, 255,
 207, 2, 255, 207, 2, 207, 168, 1, 252, 0, 1, 59, 48, 255, 137,
 7, 1, 46, 37, 151, 0, 1, 36, 30, 255, 35, 7, 1, 67, 55,
 223, 0, 2, 124, 100, 1, 255, 107, 6, 255, 2, 6, 4, 252, 205,
 2, 79, 63, 175, 0, 210, 0, 8, 255, 207, 2, 255, 207, 2, 223,
 180, 2, 252, 0, 1, 50, 40, 255, 137, 7, 1, 54, 43, 151, 0,
 1, 29, 23, 255, 35, 7, 1, 81, 66, 220, 0, 1, 53, 43, 255,
 104, 7, 255, 2, 9, 1, 149, 121, 129, 1, 175, 0, 210, 0, 8,
 255, 207, 2, 255, 207, 2, 231, 187, 2, 252, 0, 1, 39, 31, 255,
 137, 7, 1, 70, 56, 151, 0, 1, 21, 16, 255, 35, 7, 1, 91,
 74, 217, 0, 4, 11, 9, 0, 244, 197, 255, 101, 4, 255, 2, 9,
 4, 219, 176, 2, 7, 6, 178, 0, 210, 0, 8, 255, 207, 2, 255,
 207, 2, 233, 189, 2, 252, 0, 1, 32, 26, 255, 137, 7, 1, 77,
 62, 151, 0, 1, 15, 13, 255, 35, 7, 2, 107, 86, 1, 216, 0,
 2, 191, 156, 1, 255, 101, 6, 255, 2, 6, 4, 252, 205, 2, 32,
 26, 181, 0, 210, 0, 8, 219, 176, 2, 255, 207, 2, 233, 189, 2,
 252, 0, 4, 24, 19, 0, 255, 207, 255, 137, 4, 1, 97, 79, 151,
 0, 1, 9, 7, 255, 35, 7, 2, 119, 97, 1, 213, 0, 1, 99,
 80, 255, 98, 7, 255, 2, 9, 1, 93, 77, 184, 0, 210, 0, 8,
 203, 164, 1, 255, 207, 2, 239, 193, 2, 252, 0, 11, 18, 14, 0,
 255, 207, 2, 255, 207, 2, 107, 87, 1, 150, 0, 1, 6, 5, 255,
 35, 7, 1, 144, 117, 255, 35, 25, 186, 0, 4, 29, 23, 0, 250,
 203, 255, 95, 4, 255, 2, 9, 1, 180, 146, 255, 104, 61, 210, 0,
 8, 203, 164, 1, 255, 207, 2, 244, 197, 2, 252, 0, 11, 13, 10,
 0, 255, 207, 2, 255, 207, 2, 127, 104, 1, 150, 0, 1, 3, 2,
 255, 35, 7, 1, 159, 128, 255, 35, 25, 183, 0, 128, 3, 3, 0,
 211, 171, 1, 255, 95, 6, 255, 2, 6, 4, 239, 193, 2, 8, 7,
 187, 0, 210, 0, 10, 184, 149, 1, 255, 207, 2, 244, 197, 2, 4,
 3, 250, 0, 11, 8, 6, 0, 255, 207, 2, 255, 207, 2, 144, 117,
 1, 150, 0, 128, 1, 255, 35, 7, 1, 186, 151, 255, 35, 25, 183,
 0, 2, 128, 104, 1, 255, 92, 6, 255, 2, 6, 4, 252, 205, 2,
 48, 39, 190, 0, 210, 0, 10, 156, 127, 1, 255, 207, 2, 248, 201,
 2, 4, 3, 250, 0, 11, 5, 4, 0, 255, 207, 2, 255, 207, 2,
 166, 135, 1, 150, 0, 4, 13, 11, 0, 248, 203, 255, 35, 4, 1,
 191, 156, 255, 35, 25, 180, 0, 1, 42, 34, 255, 125, 7, 255, 2,
 9, 1, 124, 101, 255, 98, 67, 210, 0, 10, 156, 127, 1, 255, 207,
 2, 255, 207, 2, 4, 3, 250, 0, 4, 2, 1, 0, 252, 205, 255,
 134, 4, 2, 180, 146, 1, 150, 0, 1, 169, 138, 255, 170, 7, 2,
 219, 176, 2, 201, 0, 4, 6, 5, 0, 217, 176, 255, 86, 4, 255,
 2, 9, 4, 199, 162, 1, 4, 3, 193, 0, 210, 0, 10, 144, 117,
 1, 255, 207, 2, 255, 207, 2, 10, 9, 250, 0, 4, 2, 1, 0,
 233, 189, 255, 134, 4, 2, 197, 159, 1, 147, 0, 2, 61, 50, 0,
 255, 167, 6, 255, 2, 3, 2, 225, 182, 2, 201, 0, 2, 138, 111,
 1, 255, 89, 9, 255, 2, 3, 4, 239, 195, 2, 30, 24, 196, 0,
 210, 0, 10, 117, 93, 1, 255, 207, 2, 255, 207, 2, 11, 9, 253,
 0, 1, 233, 189, 255, 134, 4, 2, 215, 175, 2, 144, 0, 4, 8,
 6, 0, 213, 173, 255, 29, 4, 255, 2, 9, 198, 0, 4, 50, 40,
 0, 250, 203, 255, 89, 13, 1, 95, 77, 199, 0, 210, 0, 10, 117,
 93, 1, 255, 207, 2, 255, 207, 2, 13, 10, 253, 0, 1, 213, 171,
 255, 134, 4, 2, 231, 187, 2, 144, 0, 1, 108, 88, 255, 164, 7,
 255, 2, 9, 1, 2, 1, 193, 0, 4, 6, 5, 0, 221, 178, 255,
 86, 13, 2, 182, 147, 1, 255, 89, 69, 132, 0, 210, 0, 10, 105,
 85, 1, 255, 207, 2, 255, 207, 2, 22, 17, 253, 0, 1, 213, 171,
 255, 134, 4, 255, 2, 3, 141, 0, 4, 20, 16, 0, 248, 201, 255,
 26, 7, 255, 2, 6, 4, 219, 176, 2, 1, 1, 193, 0, 1, 139,
 112, 255, 251, 7, 255, 2, 6, 4, 237, 191, 2, 21, 17, 202, 0,
 210, 0, 10, 82, 67, 0, 255, 207, 2, 255, 207, 2, 22, 18, 253,
 0, 2, 191, 156, 1, 255, 137, 6, 128, 1, 139, 0, 1, 193, 157,
 255, 23, 7, 255, 2, 6, 4, 252, 205, 2, 69, 55, 193, 0, 4,
 44, 36, 0, 250, 203, 255, 86, 13, 1, 82, 67, 205, 0, 210, 0,
 10, 85, 69, 0, 255, 207, 2, 255, 207, 2, 27, 21, 253, 0, 2,
 168, 135, 1, 255, 137, 6, 128, 4, 136, 0, 1, 71, 57, 255, 158,
 7, 255, 2, 9, 1, 182, 149, 129, 1, 190, 0, 4, 4, 3, 0,
 217, 176, 255, 86, 13, 1, 168, 136, 255, 86, 67, 141, 0, 210, 0,
 10, 74, 60, 0, 255, 207, 2, 255, 207, 2, 37, 30, 253, 0, 2,
 147, 119, 1, 255, 137, 6, 1, 8, 7, 133, 0, 4, 8, 6, 0,
 211, 171, 255, 20, 7, 255, 2, 9, 1, 23, 19, 193, 0, 1, 122,
 99, 255, 86, 13, 4, 233, 189, 2, 20, 16, 208, 0, 210, 0, 10,
 60, 49, 0, 255, 207, 2, 255, 207, 2, 41, 33, 253, 0, 2, 127,
 101, 1, 255, 137, 6, 1, 14, 11, 133, 0, 1, 99, 80, 255, 155,
 7, 255, 2, 9, 2, 111, 90, 1, 192, 0, 4, 30, 24, 0, 252,
 205, 255, 83, 10, 255, 11, 3, 1, 84, 68, 211, 0, 210, 0, 10,
 58, 47, 0, 255, 207, 2, 255, 207, 2, 50, 40, 253, 0, 2, 108,
 88, 1, 255, 137, 6, 1, 21, 17, 133, 0, 1, 237, 193, 255, 14,
 4, 255, 2, 9, 4, 207, 168, 1, 4, 3, 190, 0, 128, 1, 2,
 0, 209, 169, 255, 104, 7, 255, 2, 6, 4, 184, 149, 1, 2, 1,
 211, 0, 210, 0, 10, 45, 36, 0, 255, 207, 2, 255, 207, 2, 60,
 49, 253, 0, 1, 87, 71, 255, 137, 7, 1, 29, 23, 133, 0, 2,
 209, 169, 1, 255, 17, 6, 255, 2, 3, 4, 250, 203, 2, 59, 48,
 193, 0, 1, 96, 79, 255, 101, 7, 255, 2, 6, 4, 239, 193, 2,
 22, 18, 214, 0, 210, 0, 10, 37, 30, 0, 255, 207, 2, 255, 207,
 2, 67, 55, 253, 0, 1, 75, 61, 255, 137, 7, 1, 40, 32, 133,
 0, 2, 195, 159, 1, 255, 17, 6, 255, 2, 3, 1, 178, 146, 129,
 1, 190, 0, 4, 16, 13, 0, 248, 203, 255, 80, 7, 255, 2, 6,
 1, 95, 77, 217, 0, 210, 0, 10, 36, 29, 0, 255, 207, 2, 255,
 207, 2, 84, 69, 253, 0, 1, 58, 47, 255, 137, 7, 1, 49, 40,
 133, 0, 2, 168, 136, 1, 255, 17, 6, 255, 2, 3, 1, 23, 18,
 193, 0, 1, 186, 149, 255, 80, 10, 255, 2, 3, 4, 195, 159, 1,
 3, 2, 217, 0, 210, 0, 10, 22, 17, 0, 255, 207, 2, 255, 207,
 2, 92, 74, 253, 0, 1, 46, 37, 255, 137, 7, 1, 68, 55, 133,
 0, 2, 146, 118, 1, 255, 17, 6, 2, 115, 93, 1, 192, 0, 1,
 58, 48, 255, 95, 7, 255, 2, 6, 4, 250, 203, 2, 36, 29, 220,
 0, 210, 0, 10, 20, 16, 0, 255, 207, 2, 255, 207, 2, 103, 82,
 253, 0, 1, 34, 27, 255, 137, 7, 1, 79, 65, 133, 0, 2, 128,
 104, 1, 255, 17, 6, 1, 22, 18, 190, 0, 4, 5, 4, 0, 229,
 186, 255, 74, 4, 255, 2, 9, 2, 130, 105, 1, 222, 0, 210, 0,
 11, 17, 14, 0, 255, 207, 2, 255, 207, 2, 124, 100, 1, 252, 0,
 1, 25, 20, 255, 137, 7, 1, 103, 84, 133, 0, 2, 101, 82, 1,
 255, 17, 6, 255, 164, 3, 189, 0, 1, 121, 99, 255, 74, 7, 255,
 2, 6, 4, 235, 191, 2, 5, 4, 223, 0, 210, 0, 11, 9, 7,
 0, 255, 207, 2, 255, 207, 2, 131, 107, 1, 252, 0, 1, 17, 14,
 255, 137, 7, 1, 119, 97, 255, 137, 7, 1, 92, 74, 255, 17, 7,
 1, 27, 22, 187, 0, 4, 25, 21, 0, 244, 199, 255, 71, 4, 255,
 2, 9, 1, 61, 50, 226, 0, 210, 0, 11, 8, 6, 0, 255, 207,
 2, 255, 207, 2, 154, 125, 1, 252, 0, 1, 10, 8, 255, 137, 7,
 1, 142, 117, 255, 137, 7, 1, 70, 56, 255, 17, 7, 1, 37, 30,
 184, 0, 128, 1, 3, 0, 166, 135, 1, 255, 71, 6, 255, 2, 6,
 1, 182, 147, 255, 233, 103, 210, 0, 128, 3, 9, 0, 255, 207, 2,
 255, 207, 2, 169, 136, 1, 252, 0, 1, 6, 5, 255, 137, 7, 1,
 166, 135, 255, 137, 7, 1, 57, 46, 255, 17, 7, 1, 52, 42, 184,
 0, 4, 56, 45, 0, 250, 205, 255, 68, 4, 255, 2, 6, 4, 250,
 203, 2, 19, 15, 229, 0, 210, 0, 11, 2, 1, 0, 255, 207, 2,
 255, 207, 2, 182, 147, 1, 225, 0, 4, 29, 23, 0, 6, 5, 148,
 0, 128, 2, 2, 0, 252, 205, 255, 137, 4, 1, 180, 146, 255, 137,
 7, 1, 44, 36, 255, 155, 7, 1, 68, 55, 181, 0, 5, 4, 3,
 0, 211, 171, 1, 255, 68, 6, 255, 2, 6, 1, 111, 90, 255, 230,
 100, 132, 0, 210, 0, 128, 1, 9, 0, 255, 207, 2, 255, 207, 2,
 205, 166, 1, 222, 0, 7, 56, 45, 0, 184, 151, 1, 19, 15, 255,
 137, 25, 1, 233, 189, 255, 137, 4, 2, 211, 171, 2, 132, 0, 1,
 29, 23, 255, 155, 7, 1, 84, 68, 181, 0, 1, 100, 81, 255, 65,
 7, 255, 2, 6, 3, 221, 178, 2, 3, 255, 91, 7, 226, 0, 213,
 0, 8, 255, 207, 2, 255, 207, 2, 217, 176, 2, 216, 0, 7, 5,
 4, 0, 95, 77, 0, 219, 178, 255, 104, 4, 1, 28, 22, 151, 0,
 1, 225, 182, 255, 32, 4, 1, 227, 184, 255, 137, 7, 1, 22, 18,
 255, 155, 7, 2, 104, 85, 1, 177, 0, 4, 12, 9, 0, 248, 201,
 255, 62, 4, 255, 2, 9, 1, 50, 41, 235, 0, 213, 0, 8, 255,
 207, 2, 255, 207, 2, 219, 178, 2, 213, 0, 7, 18, 15, 0, 136,
 109, 1, 244, 197, 255, 101, 4, 255, 2, 3, 1, 37, 30, 151, 0,
 1, 213, 171, 255, 35, 7, 132, 0, 1, 12, 10, 255, 155, 7, 2,
 130, 105, 1, 177, 0, 2, 156, 125, 1, 255, 62, 6, 255, 2, 6,
 1, 164, 133, 255, 68, 52, 186, 0, 213, 0, 8, 223, 180, 2, 255,
 207, 2, 229, 186, 2, 210, 0, 7, 45, 36, 0, 171, 139, 1, 252,
 205, 255, 98, 4, 255, 2, 6, 1, 52, 42, 151, 0, 2, 195, 159,
 1, 255, 35, 6, 128, 2, 130, 0, 2, 7, 6, 0, 255, 14, 6,
 2, 154, 124, 1, 174, 0, 1, 35, 29, 255, 59, 7, 255, 2, 6,
 4, 244, 199, 2, 23, 18, 238, 0, 213, 0, 10, 209, 169, 1, 255,
 207, 2, 235, 191, 2, 1, 1, 202, 0, 255, 76, 3, 4, 80, 66,
 0, 205, 166, 255, 92, 4, 255, 2, 12, 1, 65, 53, 151, 0, 1,
 166, 135, 255, 44, 7, 1, 6, 5, 130, 0, 128, 3, 0, 0, 255,
 14, 6, 1, 175, 142, 255, 153, 46, 128, 1, 2, 0, 195, 157, 255,
 119, 13, 1, 121, 99, 255, 219, 77, 164, 0, 213, 0, 10, 191, 156,
 1, 255, 207, 2, 241, 195, 2, 3, 2, 199, 0, 10, 5, 4, 0,
 119, 96, 1, 227, 184, 2, 255, 207, 255, 2, 16, 1, 82, 67, 151,
 0, 1, 141, 114, 255, 137, 4, 255, 2, 3, 1, 13, 10, 133, 0,
 255, 14, 6, 2, 203, 164, 1, 171, 0, 1, 59, 48, 255, 56, 7,
 255, 2, 6, 4, 227, 186, 2, 9, 7, 241, 0, 213, 0, 10, 168,
 136, 1, 255, 207, 2, 255, 207, 2, 3, 3, 196, 0, 7, 12, 10,
 0, 156, 125, 1, 241, 197, 255, 83, 4, 255, 2, 18, 1, 100, 81,
 151, 0, 1, 117, 95, 255, 137, 7, 1, 19, 16, 133, 0, 1, 237,
 193, 255, 14, 4, 2, 215, 173, 2, 168, 0, 4, 4, 3, 0, 205,
 166, 255, 71, 7, 255, 2, 6, 1, 81, 66, 244, 0, 213, 0, 10,
 164, 133, 1, 255, 207, 2, 255, 207, 2, 7, 5, 196, 0, 4, 74,
 60, 0, 250, 203, 255, 80, 4, 255, 2, 21, 2, 121, 97, 1, 150,
 0, 2, 95, 77, 0, 255, 35, 6, 1, 29, 23, 133, 0, 1, 215,
 175, 255, 14, 4, 3, 229, 186, 2, 1, 255, 52, 25, 142, 0, 1,
 78, 62, 255, 68, 7, 255, 2, 6, 1, 221, 180, 255, 59, 43, 204,
 0, 213, 0, 10, 131, 107, 1, 255, 207, 2, 255, 207, 2, 10, 8,
 196, 0, 2, 92, 74, 0, 255, 80, 6, 255, 2, 21, 2, 146, 118,
 1, 150, 0, 1, 75, 61, 255, 56, 7, 1, 44, 35, 133, 0, 1,
 184, 149, 255, 155, 4, 3, 252, 205, 2, 2, 255, 52, 25, 139, 0,
 4, 6, 5, 0, 235, 191, 255, 110, 13, 0, 43, 255, 74, 8, 240,
 0, 213, 0, 10, 127, 104, 1, 255, 207, 2, 255, 207, 2, 17, 14,
 196, 0, 2, 86, 69, 0, 255, 80, 6, 255, 2, 21, 2, 169, 138,
 1, 150, 0, 1, 56, 45, 255, 56, 7, 1, 57, 46, 133, 0, 1,
 166, 135, 255, 155, 7, 1, 5, 4, 163, 0, 1, 122, 99, 255, 47,
 7, 255, 2, 6, 1, 171, 139, 255, 107, 25, 225, 0, 213, 0, 10,
 100, 82, 1, 255, 207, 2, 255, 207, 2, 21, 17, 196, 0, 2, 79,
 63, 0, 255, 80, 6, 255, 2, 21, 2, 191, 156, 1, 150, 0, 1,
 44, 35, 255, 56, 7, 1, 74, 60, 133, 0, 1, 141, 114, 255, 155,
 7, 1, 11, 9, 160, 0, 4, 16, 13, 0, 248, 201, 255, 104, 10,
 255, 11, 3, 0, 22, 255, 209, 71, 180, 0, 213, 0, 10, 92, 75,
 0, 255, 207, 2, 255, 207, 2, 31, 25, 196, 0, 1, 75, 60, 255,
 80, 7, 255, 2, 21, 2, 209, 169, 1, 150, 0, 255, 128, 3, 255,
 35, 6, 1, 99, 80, 133, 0, 2, 109, 90, 1, 255, 17, 6, 1,
 20, 16, 160, 0, 1, 135, 108, 255, 44, 7, 255, 2, 6, 1, 130,
 105, 255, 104, 25, 228, 0, 213, 0, 10, 75, 60, 0, 255, 207, 2,
 255, 207, 2, 37, 31, 196, 0, 1, 62, 50, 255, 80, 7, 255, 2,
 9, 7, 250, 203, 2, 71, 57, 0, 231, 187, 255, 11, 4, 2, 219,
 176, 2, 150, 0, 1, 19, 15, 255, 56, 7, 2, 119, 97, 1, 132,
 0, 1, 90, 73, 255, 17, 7, 1, 31, 25, 157, 0, 4, 21, 17,
 0, 244, 197, 255, 113, 10, 4, 239, 195, 2, 16, 13, 253, 0, 213,
 0, 10, 63, 52, 0, 255, 207, 2, 255, 207, 2, 50, 40, 196, 0,
 1, 59, 48, 255, 80, 7, 255, 2, 6, 11, 239, 195, 2, 36, 29,
 0, 1, 1, 0, 201, 162, 1, 255, 17, 6, 150, 0, 1, 12, 9,
 255, 56, 7, 2, 144, 117, 1, 132, 0, 1, 71, 57, 255, 17, 7,
 1, 44, 36, 157, 0, 1, 147, 119, 255, 92, 7, 255, 2, 6, 1,
 114, 92, 255, 65, 7, 249, 0, 213, 0, 10, 53, 43, 0, 255, 207,
 2, 255, 207, 2, 58, 48, 172, 0, 4, 25, 20, 0, 2, 1, 145,
 0, 1, 45, 36, 255, 80, 7, 255, 2, 3, 4, 217, 175, 2, 18,
 14, 133, 0, 2, 191, 154, 1, 255, 20, 6, 128, 2, 148, 0, 1,
 6, 5, 255, 56, 7, 1, 176, 142, 255, 87, 7, 1, 49, 40, 255,
 17, 7, 0, 59, 255, 155, 29, 4, 24, 19, 0, 250, 203, 255, 113,
 7, 255, 2, 3, 4, 241, 197, 2, 6, 5, 253, 0, 129, 0, 213,
 0, 10, 40, 33, 0, 255, 207, 2, 255, 207, 2, 73, 59, 169, 0,
 4, 29, 23, 0, 199, 161, 129, 1, 145, 0, 1, 39, 31, 255, 80,
 7, 4, 197, 159, 1, 9, 8, 136, 0, 2, 164, 135, 1, 255, 23,
 6, 1, 6, 5, 148, 0, 4, 2, 1, 0, 246, 199, 255, 56, 7,
 132, 0, 1, 36, 29, 255, 74, 7, 1, 77, 62, 154, 0, 1, 175,
 142, 255, 89, 7, 255, 2, 6, 1, 86, 70, 253, 0, 132, 0, 213,
 0, 10, 33, 27, 0, 255, 207, 2, 255, 207, 2, 81, 67, 166, 0,
 8, 32, 26, 0, 203, 164, 1, 231, 187, 2, 147, 0, 1, 31, 25,
 255, 80, 22, 2, 138, 112, 1, 255, 23, 6, 1, 13, 11, 148, 0,
 4, 1, 0, 0, 233, 189, 255, 32, 4, 1, 219, 178, 255, 89, 7,
 1, 27, 21, 255, 74, 7, 1, 100, 81, 151, 0, 1, 37, 30, 255,
 35, 7, 255, 2, 6, 3, 215, 175, 2, 3, 255, 151, 22, 238, 0,
 213, 0, 10, 23, 19, 0, 255, 207, 2, 255, 207, 2, 97, 80, 163,
 0, 11, 30, 24, 0, 205, 166, 1, 255, 207, 2, 175, 142, 1, 147,
 0, 0, 25, 255, 80, 8, 1, 95, 77, 139, 0, 1, 111, 91, 255,
 53, 4, 255, 2, 3, 1, 20, 16, 151, 0, 1, 215, 173, 255, 32,
 4, 255, 2, 3, 132, 0, 1, 16, 13, 255, 74, 7, 1, 125, 103,
 255, 107, 22, 129, 0, 1, 164, 133, 255, 86, 7, 255, 2, 6, 1,
 66, 54, 253, 0, 135, 0, 213, 0, 11, 19, 15, 0, 255, 207, 2,
 255, 207, 2, 114, 93, 1, 159, 0, 5, 31, 25, 0, 189, 154, 1,
 255, 47, 6, 1, 124, 101, 255, 47, 22, 0, 18, 255, 80, 8, 1,
 107, 87, 255, 32, 13, 1, 90, 73, 255, 23, 7, 255, 68, 3, 150,
 0, 1, 203, 164, 255, 92, 7, 128, 2, 130, 0, 1, 8, 6, 255,
 50, 7, 1, 156, 127, 255, 107, 22, 255, 74, 3, 1, 250, 203, 255,
 32, 4, 255, 2, 6, 3, 209, 169, 1, 3, 255, 56, 5, 253, 0,
 131, 0, 213, 0, 11, 12, 10, 0, 255, 207, 2, 255, 207, 2, 135,
 109, 1, 156, 0, 5, 28, 23, 0, 191, 156, 1, 255, 44, 6, 255,
 2, 3, 1, 81, 66, 148, 0, 0, 13, 255, 80, 8, 1, 125, 103,
 255, 80, 13, 1, 71, 57, 255, 23, 7, 1, 45, 36, 151, 0, 1,
 171, 138, 255, 95, 7, 1, 7, 6, 130, 0, 1, 3, 2, 255, 50,
 7, 1, 187, 152, 255, 155, 22, 1, 184, 149, 255, 143, 10, 255, 2,
 3, 1, 66, 54, 253, 0, 138, 0, 213, 0, 11, 10, 8, 0, 255,
 207, 2, 255, 207, 2, 154, 125, 1, 153, 0, 5, 27, 22, 0, 175,
 142, 1, 255, 41, 6, 255, 2, 6, 1, 49, 40, 148, 0, 1, 8,
 7, 255, 80, 7, 1, 147, 119, 255, 80, 13, 1, 55, 44, 255, 23,
 7, 1, 59, 48, 151, 0, 1, 141, 114, 255, 98, 7, 1, 14, 11,
 133, 0, 1, 250, 205, 255, 14, 4, 1, 207, 168, 255, 155, 19, 1,
 42, 34, 255, 80, 7, 255, 2, 6, 2, 215, 173, 1, 255, 192, 28,
 239, 0, 213, 0, 11, 4, 3, 0, 255, 207, 2, 255, 207, 2, 176,
 144, 1, 150, 0, 5, 23, 18, 0, 161, 131, 1, 255, 38, 6, 255,
 2, 9, 1, 25, 21, 148, 0, 1, 5, 4, 255, 80, 7, 1, 161,
 130, 255, 80, 13, 1, 40, 32, 255, 23, 7, 1, 78, 63, 151, 0,
 1, 112, 92, 255, 101, 7, 255, 92, 9, 1, 219, 178, 255, 14, 4,
 2, 225, 184, 2, 144, 0, 255, 176, 3, 255, 143, 12, 1, 48, 39,
 253, 0, 141, 0, 213, 0, 11, 3, 2, 0, 255, 207, 2, 255, 207,
 2, 199, 162, 1, 147, 0, 5, 18, 14, 0, 147, 119, 1, 255, 35,
 6, 255, 2, 12, 1, 11, 9, 148, 0, 128, 3, 2, 0, 241, 197,
 255, 32, 4, 1, 197, 161, 255, 80, 13, 1, 27, 22, 255, 104, 7,
 1, 101, 82, 255, 104, 22, 129, 0, 1, 87, 71, 255, 35, 7, 1,
 36, 29, 133, 0, 1, 195, 159, 255, 122, 7, 128, 1, 139, 0, 4,
 30, 24, 0, 250, 203, 255, 140, 10, 1, 191, 156, 129, 1, 253, 0,
 141, 0, 216, 0, 8, 255, 207, 2, 255, 207, 2, 213, 171, 2, 144,
 0, 5, 12, 10, 0, 128, 104, 1, 255, 32, 6, 255, 2, 12, 4,
 239, 195, 2, 2, 1, 151, 0, 255, 29, 3, 255, 35, 3, 1, 223,
 182, 255, 80, 13, 1, 18, 14, 255, 104, 7, 1, 125, 101, 255, 57,
 25, 1, 69, 55, 255, 35, 7, 1, 53, 43, 133, 0, 1, 173, 141,
 255, 125, 7, 1, 5, 4, 139, 0, 1, 168, 136, 255, 149, 13, 1,
 52, 42, 229, 0, 128, 1, 3, 0, 84, 68, 0, 255, 251, 27, 135,
 0, 216, 0, 8, 250, 203, 2, 255, 207, 2, 223, 180, 2, 141, 0,
 7, 6, 5, 0, 112, 91, 1, 252, 205, 255, 29, 4, 255, 2, 15,
 1, 227, 184, 255, 44, 16, 138, 0, 1, 235, 191, 255, 35, 4, 1,
 246, 199, 255, 35, 13, 2, 9, 8, 0, 255, 59, 6, 2, 147, 119,
 1, 150, 0, 1, 48, 39, 255, 35, 7, 1, 73, 59, 133, 0, 2,
 144, 117, 1, 255, 17, 6, 1, 11, 9, 136, 0, 1, 31, 25, 255,
 38, 7, 255, 2, 6, 3, 209, 169, 2, 1, 255, 82, 25, 202, 0,
 10, 28, 23, 0, 201, 164, 1, 252, 205, 2, 7, 5, 160, 0, 216,
 0, 10, 213, 171, 2, 255, 207, 2, 233, 189, 2, 1, 1, 136, 0,
 128, 2, 5, 0, 95, 77, 0, 246, 201, 255, 26, 4, 255, 2, 18,
 1, 178, 144, 255, 42, 11, 143, 0, 255, 74, 3, 255, 35, 3, 1,
 248, 201, 255, 80, 13, 2, 4, 3, 0, 255, 59, 6, 1, 176, 142,
 255, 59, 25, 1, 33, 27, 255, 35, 7, 1, 100, 81, 133, 0, 2,
 115, 92, 1, 255, 17, 6, 1, 20, 16, 136, 0, 1, 166, 135, 255,
 20, 7, 255, 2, 6, 1, 49, 40, 223, 0, 128, 2, 5, 0, 121,
 99, 1, 244, 199, 255, 113, 7, 1, 18, 15, 160, 0, 216, 0, 10,
 205, 164, 1, 255, 207, 2, 246, 199, 2, 2, 1, 136, 0, 7, 77,
 62, 0, 233, 189, 2, 255, 207, 255, 2, 22, 1, 124, 100, 255, 42,
 11, 143, 0, 1, 195, 157, 255, 80, 4, 4, 248, 201, 2, 3, 2,
 136, 0, 128, 1, 2, 0, 248, 203, 255, 59, 4, 1, 197, 159, 255,
 59, 25, 2, 21, 17, 0, 255, 95, 6, 1, 121, 99, 255, 35, 7,
 1, 91, 74, 255, 17, 7, 1, 34, 27, 133, 0, 4, 22, 17, 0,
 246, 199, 255, 137, 10, 255, 107, 3, 222, 0, 7, 33, 27, 0, 213,
 173, 2, 252, 205, 255, 116, 10, 1, 34, 28, 160, 0, 216, 0, 10,
 173, 141, 1, 255, 207, 2, 255, 207, 2, 5, 4, 133, 0, 4, 56,
 45, 0, 215, 175, 255, 17, 4, 255, 2, 24, 1, 80, 66, 154, 0,
 1, 178, 146, 255, 80, 4, 1, 252, 205, 255, 80, 10, 129, 0, 4,
 1, 0, 0, 229, 186, 255, 59, 4, 2, 219, 178, 2, 150, 0, 2,
 12, 9, 0, 255, 95, 6, 2, 154, 124, 1, 132, 0, 1, 67, 55,
 255, 17, 7, 1, 50, 40, 133, 0, 1, 141, 114, 255, 173, 7, 255,
 98, 6, 1, 52, 42, 217, 0, 128, 2, 5, 0, 121, 99, 1, 246,
 199, 255, 254, 16, 1, 57, 46, 160, 0, 216, 0, 10, 162, 131, 1,
 255, 207, 2, 255, 207, 2, 8, 6, 130, 0, 4, 36, 29, 0, 191,
 154, 255, 17, 7, 255, 2, 24, 1, 40, 32, 154, 0, 1, 149, 121,
 255, 62, 7, 1, 13, 10, 139, 0, 1, 209, 169, 255, 23, 7, 150,
 0, 2, 5, 4, 0, 255, 32, 6, 2, 180, 146, 1, 132, 0, 1,
 48, 39, 255, 17, 7, 1, 71, 57, 130, 0, 1, 16, 13, 255, 14,
 7, 255, 2, 6, 3, 213, 173, 1, 2, 255, 40, 7, 208, 0, 7,
 33, 27, 0, 213, 173, 2, 252, 205, 255, 254, 19, 1, 87, 71, 160,
 0, 216, 0, 16, 138, 111, 1, 255, 207, 2, 255, 207, 2, 14, 12,
 0, 17, 14, 0, 161, 130, 255, 14, 7, 255, 2, 27, 1, 21, 17,
 154, 0, 1, 117, 95, 255, 65, 7, 255, 38, 15, 1, 197, 161, 255,
 23, 7, 1, 2, 1, 148, 0, 255, 23, 3, 1, 241, 197, 255, 32,
 4, 2, 215, 175, 2, 132, 0, 2, 33, 27, 0, 255, 50, 6, 1,
 96, 79, 130, 0, 1, 133, 108, 255, 155, 13, 1, 63, 52, 211, 0,
 128, 1, 5, 0, 111, 90, 1, 244, 199, 255, 254, 25, 1, 124, 100,
 255, 192, 23, 137, 0, 216, 0, 16, 119, 96, 1, 255, 207, 2, 255,
 207, 2, 54, 43, 0, 135, 109, 1, 252, 205, 255, 11, 4, 255, 2,
 30, 1, 7, 6, 154, 0, 2, 96, 78, 0, 255, 38, 6, 1, 30,
 24, 139, 0, 1, 171, 139, 255, 104, 7, 1, 6, 5, 151, 0, 1,
 227, 184, 255, 32, 4, 2, 241, 197, 2, 132, 0, 1, 19, 15, 255,
 74, 7, 7, 127, 103, 1, 9, 7, 0, 235, 191, 255, 131, 10, 1,
 211, 171, 129, 1, 208, 0, 7, 21, 17, 0, 197, 159, 1, 252, 205,
 255, 254, 28, 1, 166, 135, 255, 120, 34, 216, 0, 13, 100, 81, 0,
 255, 207, 2, 255, 207, 2, 173, 141, 1, 241, 197, 255, 8, 4, 255,
 2, 30, 4, 246, 199, 2, 1, 1, 154, 0, 1, 78, 62, 255, 80,
 7, 1, 46, 38, 139, 0, 2, 138, 112, 1, 255, 23, 6, 1, 12,
 10, 151, 0, 1, 211, 169, 255, 101, 7, 255, 98, 6, 1, 9, 8,
 255, 74, 7, 4, 157, 127, 1, 95, 78, 255, 11, 7, 255, 2, 6,
 1, 67, 55, 208, 0, 4, 81, 67, 0, 241, 195, 255, 248, 25, 1,
 239, 193, 255, 8, 7, 2, 209, 168, 2, 159, 0, 216, 0, 10, 82,
 67, 0, 255, 207, 2, 255, 207, 2, 252, 205, 255, 5, 4, 255, 2,
 24, 1, 235, 191, 255, 8, 7, 2, 209, 168, 1, 156, 0, 1, 54,
 44, 255, 80, 7, 1, 63, 52, 139, 0, 2, 114, 93, 1, 255, 23,
 6, 1, 24, 19, 151, 0, 1, 184, 149, 255, 35, 7, 1, 6, 5,
 130, 0, 128, 3, 255, 74, 7, 4, 231, 189, 2, 244, 199, 255, 140,
 10, 4, 229, 186, 2, 5, 4, 202, 0, 7, 7, 6, 0, 166, 135,
 1, 250, 203, 255, 251, 22, 10, 252, 205, 2, 193, 157, 1, 31, 25,
 0, 229, 186, 255, 140, 5, 3, 187, 2, 1, 1, 157, 0, 216, 0,
 5, 69, 55, 0, 255, 207, 2, 255, 2, 30, 5, 176, 142, 1, 112,
 91, 1, 255, 11, 6, 2, 161, 131, 1, 156, 0, 1, 42, 34, 255,
 80, 7, 1, 81, 67, 139, 0, 1, 90, 73, 255, 23, 7, 1, 33,
 27, 151, 0, 1, 149, 119, 255, 101, 7, 1, 14, 11, 130, 0, 255,
 105, 3, 1, 241, 195, 255, 143, 19, 1, 96, 78, 202, 0, 4, 40,
 33, 0, 225, 182, 255, 251, 25, 4, 239, 195, 2, 109, 88, 129, 1,
 130, 0, 1, 187, 152, 255, 158, 4, 4, 252, 205, 2, 3, 3, 157,
 0, 216, 0, 5, 52, 42, 0, 255, 207, 2, 255, 2, 27, 7, 215,
 173, 2, 23, 19, 0, 91, 73, 255, 38, 7, 2, 122, 99, 1, 156,
 0, 1, 30, 24, 255, 41, 7, 1, 104, 84, 139, 0, 1, 70, 56,
 255, 23, 7, 1, 47, 38, 151, 0, 2, 114, 93, 1, 255, 35, 6,
 1, 27, 22, 133, 0, 1, 219, 176, 255, 143, 16, 4, 231, 187, 2,
 5, 4, 199, 0, 4, 107, 87, 1, 244, 199, 255, 251, 25, 4, 213,
 171, 2, 36, 29, 136, 0, 1, 147, 119, 255, 158, 7, 1, 13, 10,
 157, 0, 216, 0, 5, 45, 36, 0, 255, 207, 2, 255, 2, 24, 4,
 244, 197, 2, 39, 31, 130, 0, 2, 135, 109, 1, 255, 17, 6, 1,
 82, 67, 157, 0, 1, 15, 13, 255, 80, 7, 2, 136, 109, 1, 138,
 0, 1, 50, 40, 255, 23, 7, 1, 65, 52, 151, 0, 1, 88, 72,
 255, 35, 7, 1, 42, 34, 133, 0, 1, 197, 159, 255, 119, 7, 255,
 2, 9, 1, 107, 87, 255, 86, 13, 183, 0, 7, 9, 7, 0, 175,
 142, 1, 250, 205, 255, 248, 22, 7, 246, 199, 2, 142, 117, 1, 5,
 4, 139, 0, 1, 105, 85, 255, 140, 7, 1, 28, 23, 157, 0, 216,
 0, 5, 31, 25, 0, 255, 207, 2, 255, 2, 24, 4, 68, 55, 0,
 1, 1, 130, 0, 2, 171, 139, 1, 255, 17, 6, 1, 46, 37, 157,
 0, 1, 9, 7, 255, 80, 7, 1, 162, 131, 255, 57, 5, 134, 0,
 1, 36, 29, 255, 23, 7, 1, 92, 74, 151, 0, 1, 66, 54, 255,
 35, 7, 1, 58, 47, 133, 0, 1, 166, 136, 255, 119, 7, 255, 2,
 6, 4, 250, 203, 2, 15, 12, 193, 0, 4, 39, 31, 0, 229, 187,
 255, 245, 25, 1, 231, 187, 255, 248, 4, 144, 0, 1, 71, 57, 255,
 158, 7, 1, 50, 40, 157, 0, 216, 0, 5, 25, 20, 0, 255, 207,
 2, 255, 2, 21, 4, 114, 92, 1, 3, 2, 133, 0, 1, 215, 173,
 255, 20, 7, 1, 19, 15, 157, 0, 4, 5, 4, 0, 250, 205, 255,
 41, 4, 2, 191, 156, 1, 138, 0, 1, 23, 19, 255, 104, 7, 255,
 86, 3, 150, 0, 1, 45, 36, 255, 35, 7, 1, 86, 70, 133, 0,
 2, 133, 108, 1, 255, 146, 12, 1, 146, 118, 255, 59, 25, 168, 0,
 4, 93, 77, 0, 244, 199, 255, 242, 22, 1, 252, 205, 255, 182, 4,
 0, 18, 255, 227, 23, 1, 42, 34, 255, 158, 7, 1, 82, 67, 157,
 0, 216, 0, 5, 14, 12, 0, 255, 207, 2, 255, 2, 18, 4, 173,
 139, 1, 7, 5, 133, 0, 4, 2, 1, 0, 248, 201, 255, 23, 7,
 1, 8, 6, 157, 0, 128, 1, 2, 0, 248, 203, 255, 41, 4, 2,
 235, 191, 2, 255, 104, 21, 2, 138, 111, 1, 150, 0, 1, 29, 23,
 255, 35, 7, 1, 112, 92, 255, 35, 7, 1, 103, 82, 255, 158, 10,
 255, 131, 3, 1, 19, 15, 187, 0, 6, 4, 3, 0, 152, 124, 1,
 250, 255, 158, 5, 255, 2, 18, 6, 244, 199, 2, 124, 100, 1, 2,
 255, 157, 25, 2, 0, 22, 18, 255, 140, 7, 1, 122, 99, 255, 36,
 26, 131, 0, 216, 0, 5, 9, 7, 0, 255, 207, 2, 255, 2, 15,
 4, 225, 182, 2, 16, 13, 136, 0, 1, 9, 8, 255, 35, 7, 1,
 250, 203, 129, 2, 160, 0, 1, 244, 197, 255, 44, 4, 1, 239, 195,
 255, 42, 13, 1, 7, 5, 255, 68, 7, 2, 166, 135, 1, 150, 0,
 255, 116, 3, 255, 35, 6, 1, 144, 117, 255, 35, 7, 1, 73, 59,
 255, 158, 10, 1, 161, 131, 255, 56, 25, 162, 0, 255, 92, 3, 4,
 205, 168, 1, 252, 205, 255, 236, 16, 255, 2, 6, 4, 229, 187, 2,
 58, 47, 157, 0, 1, 9, 7, 255, 194, 34, 132, 0, 216, 0, 5,
 5, 4, 0, 255, 207, 2, 255, 2, 15, 1, 40, 33, 139, 0, 1,
 19, 15, 255, 35, 7, 2, 227, 184, 2, 162, 0, 1, 217, 176, 255,
 44, 4, 1, 248, 201, 129, 2, 136, 0, 4, 3, 2, 0, 244, 199,
 255, 23, 4, 2, 199, 162, 1, 150, 0, 1, 7, 6, 255, 104, 7,
 1, 180, 146, 255, 35, 7, 1, 51, 40, 255, 158, 10, 1, 45, 37,
 184, 0, 4, 46, 38, 0, 233, 189, 255, 230, 16, 255, 2, 6, 7,
 252, 205, 2, 193, 157, 1, 18, 15, 160, 0, 1, 2, 1, 255, 140,
 7, 1, 209, 168, 255, 216, 11, 146, 0, 216, 0, 128, 1, 3, 0,
 255, 207, 2, 255, 2, 12, 3, 92, 74, 0, 1, 140, 0, 1, 37,
 30, 255, 35, 7, 1, 187, 152, 255, 25, 15, 148, 0, 10, 178, 146,
 1, 255, 207, 2, 252, 205, 2, 5, 4, 139, 0, 1, 233, 191, 255,
 68, 4, 2, 241, 195, 2, 150, 0, 4, 3, 2, 0, 248, 201, 255,
 35, 4, 1, 207, 169, 255, 104, 7, 1, 33, 27, 255, 122, 7, 1,
 205, 166, 255, 122, 37, 147, 0, 4, 93, 77, 0, 244, 199, 255, 227,
 13, 255, 2, 9, 7, 246, 201, 2, 139, 114, 1, 3, 3, 166, 0,
 1, 229, 186, 255, 53, 4, 3, 235, 191, 2, 1, 255, 142, 29, 219,
 0, 2, 255, 207, 2, 255, 2, 9, 4, 169, 138, 1, 3, 3, 142,
 0, 1, 72, 58, 255, 35, 7, 2, 130, 105, 1, 162, 0, 2, 142,
 115, 1, 255, 47, 6, 1, 15, 12, 139, 0, 1, 227, 186, 255, 20,
 4, 2, 244, 197, 2, 150, 0, 255, 93, 3, 0, 229, 255, 35, 5,
 0, 241, 255, 35, 8, 1, 18, 15, 255, 122, 7, 1, 60, 49, 178,
 0, 128, 1, 5, 0, 139, 114, 1, 248, 203, 255, 224, 10, 255, 2,
 12, 4, 237, 193, 2, 84, 68, 172, 0, 1, 184, 149, 255, 218, 7,
 1, 5, 4, 154, 0, 219, 0, 4, 215, 175, 2, 255, 207, 255, 2,
 4, 4, 235, 191, 2, 13, 10, 145, 0, 2, 112, 91, 1, 255, 32,
 6, 1, 92, 75, 163, 0, 1, 115, 92, 255, 47, 7, 1, 27, 22,
 139, 0, 1, 211, 171, 255, 23, 4, 4, 248, 201, 2, 2, 1, 151,
 0, 1, 213, 171, 255, 140, 7, 128, 2, 130, 0, 10, 9, 7, 0,
 255, 207, 2, 239, 195, 2, 5, 4, 175, 0, 7, 6, 5, 0, 180,
 146, 1, 252, 205, 255, 80, 7, 255, 2, 15, 4, 221, 178, 2, 37,
 31, 175, 0, 1, 139, 112, 255, 218, 7, 1, 17, 14, 154, 0, 219,
 0, 10, 205, 166, 1, 255, 207, 2, 255, 207, 2, 47, 38, 148, 0,
 1, 147, 119, 255, 32, 7, 1, 61, 51, 163, 0, 2, 90, 73, 0,
 255, 47, 6, 1, 43, 35, 139, 0, 1, 178, 146, 255, 71, 4, 4,
 252, 205, 2, 5, 4, 151, 0, 1, 184, 149, 255, 107, 7, 1, 6,
 5, 130, 0, 128, 2, 255, 74, 4, 2, 109, 90, 1, 174, 0, 4,
 16, 13, 0, 211, 171, 255, 71, 4, 255, 2, 18, 255, 128, 3, 4,
 187, 152, 1, 13, 10, 178, 0, 1, 93, 75, 255, 218, 7, 1, 36,
 29, 154, 0, 219, 0, 8, 180, 146, 1, 255, 207, 2, 124, 101, 1,
 150, 0, 1, 187, 152, 255, 32, 4, 255, 2, 3, 1, 33, 27, 163,
 0, 2, 69, 55, 0, 255, 47, 6, 1, 62, 51, 139, 0, 1, 144,
 117, 255, 71, 7, 1, 11, 9, 151, 0, 255, 35, 9, 1, 13, 10,
 130, 0, 255, 147, 3, 4, 217, 176, 2, 9, 8, 172, 0, 255, 167,
 3, 1, 231, 187, 255, 68, 4, 255, 2, 18, 7, 248, 201, 2, 147,
 119, 1, 3, 2, 181, 0, 1, 60, 49, 255, 218, 7, 0, 63, 255,
 218, 14, 141, 0, 219, 0, 7, 161, 130, 1, 215, 175, 2, 4, 3,
 151, 0, 4, 231, 187, 2, 255, 207, 255, 2, 4, 1, 13, 10, 163,
 0, 2, 45, 36, 0, 255, 47, 6, 1, 87, 71, 139, 0, 2, 118,
 96, 1, 255, 23, 6, 1, 19, 15, 151, 0, 1, 111, 90, 255, 35,
 7, 1, 22, 17, 255, 17, 10, 171, 0, 4, 60, 49, 0, 241, 195,
 255, 176, 7, 255, 2, 15, 4, 241, 197, 2, 105, 85, 255, 77, 46,
 141, 0, 1, 34, 28, 255, 218, 7, 1, 100, 81, 154, 0, 219, 0,
 4, 139, 114, 1, 29, 23, 151, 0, 13, 4, 3, 0, 252, 205, 2,
 255, 207, 2, 252, 205, 2, 4, 3, 163, 0, 4, 27, 22, 0, 255,
 207, 255, 50, 4, 2, 128, 104, 1, 138, 0, 1, 95, 77, 255, 23,
 7, 1, 25, 20, 151, 0, 1, 87, 70, 255, 35, 7, 1, 34, 28,
 133, 0, 1, 14, 12, 169, 0, 4, 87, 71, 0, 244, 199, 255, 62,
 4, 255, 2, 18, 4, 233, 189, 2, 62, 51, 190, 0, 1, 16, 13,
 255, 158, 7, 1, 147, 119, 255, 218, 13, 141, 0, 219, 0, 1, 23,
 19, 154, 0, 11, 9, 7, 0, 255, 207, 2, 255, 207, 2, 239, 195,
 2, 165, 0, 1, 16, 13, 255, 50, 7, 2, 156, 127, 1, 138, 0,
 1, 78, 62, 255, 23, 7, 1, 32, 26, 151, 0, 1, 67, 55, 255,
 35, 7, 1, 47, 39, 175, 0, 4, 109, 90, 1, 248, 201, 255, 59,
 4, 255, 2, 18, 4, 221, 180, 2, 33, 27, 193, 0, 1, 5, 4,
 255, 158, 7, 1, 201, 162, 255, 218, 13, 141, 0, 249, 0, 11, 24,
 20, 0, 255, 207, 2, 255, 207, 2, 211, 171, 1, 165, 0, 1, 7,
 5, 255, 50, 7, 1, 193, 157, 255, 50, 13, 1, 63, 51, 255, 23,
 7, 1, 39, 31, 151, 0, 1, 48, 40, 255, 35, 7, 1, 60, 49,
 169, 0, 255, 102, 3, 4, 131, 107, 1, 250, 203, 255, 56, 4, 255,
 2, 18, 4, 203, 166, 1, 16, 13, 199, 0, 1, 246, 199, 255, 83,
 4, 2, 227, 184, 2, 153, 0, 249, 0, 11, 49, 40, 0, 255, 207,
 2, 255, 207, 2, 157, 127, 1, 165, 0, 4, 3, 2, 0, 241, 195,
 255, 50, 4, 2, 225, 182, 2, 138, 0, 1, 57, 45, 255, 74, 7,
 1, 42, 34, 151, 0, 1, 36, 29, 255, 35, 7, 1, 73, 59, 166,
 0, 128, 1, 5, 0, 157, 127, 1, 250, 203, 255, 53, 4, 255, 2,
 15, 7, 252, 205, 2, 180, 146, 1, 7, 5, 202, 0, 2, 197, 159,
 1, 255, 89, 6, 1, 4, 3, 151, 0, 249, 0, 11, 77, 62, 0,
 255, 207, 2, 255, 207, 2, 114, 93, 1, 168, 0, 1, 223, 180, 255,
 50, 4, 255, 2, 3, 128, 2, 136, 0, 1, 67, 55, 255, 74, 7,
 1, 40, 32, 151, 0, 1, 27, 22, 255, 35, 7, 1, 85, 69, 163,
 0, 7, 3, 2, 0, 169, 138, 1, 252, 205, 255, 110, 7, 255, 2,
 12, 5, 250, 203, 2, 151, 122, 1, 255, 128, 12, 195, 0, 2, 152,
 122, 1, 255, 95, 6, 1, 14, 11, 151, 0, 249, 0, 10, 104, 85,
 1, 255, 207, 2, 255, 207, 2, 82, 67, 169, 0, 1, 207, 166, 255,
 53, 7, 1, 12, 9, 136, 0, 2, 97, 79, 0, 255, 20, 6, 1,
 35, 29, 151, 0, 1, 19, 15, 255, 35, 7, 255, 44, 3, 159, 0,
 7, 3, 2, 0, 180, 146, 1, 252, 205, 255, 47, 4, 255, 2, 15,
 6, 248, 201, 2, 130, 105, 1, 1, 209, 0, 1, 105, 85, 255, 218,
 7, 0, 36, 255, 197, 26, 249, 0, 10, 156, 125, 1, 255, 207, 2,
 255, 207, 2, 45, 36, 169, 0, 1, 171, 139, 255, 53, 7, 1, 29,
 23, 136, 0, 1, 147, 119, 255, 20, 7, 1, 22, 18, 151, 0, 2,
 14, 12, 0, 255, 35, 6, 2, 112, 91, 1, 156, 0, 7, 7, 5,
 0, 180, 146, 1, 252, 205, 255, 44, 4, 255, 2, 15, 4, 244, 199,
 2, 108, 88, 255, 62, 31, 183, 0, 1, 63, 52, 255, 161, 7, 1,
 63, 51, 151, 0, 249, 0, 10, 203, 166, 1, 255, 207, 2, 255, 207,
 2, 25, 20, 169, 0, 1, 127, 104, 255, 53, 7, 1, 67, 54, 136,
 0, 0, 205, 255, 74, 8, 1, 14, 11, 151, 0, 2, 10, 8, 0,
 255, 35, 6, 2, 121, 99, 1, 153, 0, 7, 7, 5, 0, 199, 162,
 1, 252, 205, 255, 41, 4, 255, 2, 15, 4, 241, 197, 2, 95, 78,
 217, 0, 1, 36, 29, 255, 161, 7, 1, 100, 81, 151, 0, 249, 0,
 4, 229, 186, 2, 255, 207, 255, 2, 4, 1, 11, 9, 169, 0, 2,
 90, 74, 0, 255, 53, 6, 2, 149, 121, 1, 132, 0, 1, 6, 4,
 255, 17, 7, 4, 252, 205, 2, 5, 4, 151, 0, 1, 7, 6, 255,
 38, 7, 1, 125, 101, 255, 56, 7, 144, 0, 5, 7, 5, 0, 201,
 162, 1, 255, 38, 6, 255, 2, 15, 4, 239, 195, 2, 72, 58, 220,
 0, 1, 16, 13, 255, 161, 7, 1, 151, 122, 255, 161, 25, 246, 0,
 128, 1, 11, 0, 255, 207, 2, 255, 207, 2, 250, 203, 2, 4, 3,
 169, 0, 1, 56, 45, 255, 56, 7, 1, 233, 187, 129, 2, 130, 0,
 1, 50, 40, 255, 17, 7, 3, 246, 199, 2, 1, 152, 0, 1, 5,
 4, 255, 38, 7, 1, 135, 108, 255, 37, 22, 5, 7, 5, 0, 201,
 162, 1, 255, 35, 6, 255, 2, 15, 4, 235, 191, 2, 68, 55, 223,
 0, 1, 4, 3, 255, 161, 7, 1, 205, 164, 255, 199, 25, 246, 0,
 11, 9, 7, 0, 255, 207, 2, 255, 207, 2, 244, 199, 2, 171, 0,
 1, 31, 25, 255, 56, 7, 255, 2, 3, 8, 75, 61, 0, 7, 5,
 0, 168, 136, 1, 255, 14, 6, 2, 211, 171, 1, 153, 0, 1, 4,
 3, 255, 56, 7, 1, 144, 117, 255, 38, 19, 4, 6, 5, 0, 199,
 162, 255, 71, 7, 255, 2, 15, 4, 235, 191, 2, 51, 41, 229, 0,
 1, 241, 195, 255, 113, 4, 3, 233, 189, 2, 1, 255, 202, 23, 246,
 0, 11, 21, 16, 0, 255, 207, 2, 255, 207, 2, 199, 162, 1, 171,
 0, 1, 12, 10, 255, 56, 7, 255, 2, 3, 5, 231, 186, 2, 169,
 138, 1, 255, 14, 9, 1, 152, 124, 255, 74, 28, 4, 4, 3, 0,
 252, 205, 255, 38, 4, 1, 144, 117, 255, 38, 16, 5, 3, 2, 0,
 180, 146, 1, 255, 29, 6, 255, 2, 15, 4, 235, 191, 2, 51, 41,
 232, 0, 1, 193, 156, 255, 206, 7, 1, 6, 5, 148, 0, 246, 0,
 11, 34, 28, 0, 255, 207, 2, 255, 207, 2, 161, 131, 1, 171, 0,
 4, 3, 2, 0, 237, 193, 255, 56, 4, 255, 2, 18, 1, 103, 84,
 255, 56, 31, 255, 38, 6, 1, 146, 118, 255, 113, 13, 255, 23, 3,
 4, 180, 146, 1, 252, 205, 255, 80, 19, 4, 235, 191, 2, 51, 41,
 235, 0, 2, 144, 117, 1, 255, 122, 6, 1, 21, 17, 148, 0, 246,
 0, 11, 57, 47, 0, 255, 207, 2, 255, 207, 2, 125, 103, 1, 174,
 0, 2, 199, 161, 1, 255, 59, 6, 255, 2, 15, 1, 54, 43, 154,
 0, 1, 3, 2, 255, 113, 7, 1, 147, 119, 255, 113, 13, 4, 161,
 131, 1, 252, 205, 255, 77, 19, 4, 235, 191, 2, 51, 41, 238, 0,
 1, 90, 73, 255, 164, 7, 1, 45, 36, 148, 0, 246, 0, 10, 92,
 75, 0, 255, 207, 2, 255, 207, 2, 80, 65, 175, 0, 2, 133, 108,
 1, 255, 59, 6, 255, 2, 15, 1, 19, 16, 154, 0, 4, 4, 3,
 0, 252, 205, 255, 38, 4, 2, 141, 114, 1, 138, 0, 1, 235, 191,
 255, 74, 19, 255, 20, 3, 1, 51, 41, 241, 0, 2, 55, 45, 0,
 255, 128, 6, 1, 79, 65, 148, 0, 246, 0, 10, 122, 99, 1, 255,
 207, 2, 255, 207, 2, 51, 40, 175, 0, 2, 60, 49, 0, 255, 59,
 6, 255, 2, 12, 4, 233, 189, 2, 1, 1, 154, 0, 1, 4, 3,
 255, 53, 7, 1, 144, 118, 255, 39, 13, 1, 223, 180, 255, 74, 16,
 4, 235, 191, 2, 51, 41, 244, 0, 1, 29, 23, 255, 164, 7, 1,
 127, 103, 255, 204, 22, 246, 0, 10, 161, 130, 1, 255, 207, 2, 255,
 207, 2, 36, 29, 175, 0, 2, 12, 10, 0, 255, 59, 6, 255, 2,
 12, 2, 147, 119, 1, 156, 0, 1, 6, 5, 255, 53, 7, 1, 138,
 111, 255, 41, 13, 1, 173, 141, 255, 137, 7, 255, 2, 6, 4, 239,
 195, 2, 57, 46, 247, 0, 1, 9, 7, 255, 164, 7, 1, 182, 147,
 255, 206, 22, 246, 0, 4, 213, 171, 2, 255, 207, 255, 2, 4, 1,
 21, 17, 178, 0, 1, 223, 180, 255, 62, 7, 255, 2, 9, 1, 68,
 55, 157, 0, 2, 7, 6, 0, 255, 41, 6, 2, 125, 103, 1, 138,
 0, 2, 130, 105, 1, 255, 68, 9, 4, 239, 195, 2, 72, 58, 250,
 0, 128, 1, 255, 164, 7, 255, 224, 3, 147, 0, 246, 0, 4, 231,
 189, 2, 255, 207, 255, 2, 4, 1, 5, 4, 178, 0, 2, 88, 72,
 0, 255, 62, 6, 255, 2, 6, 3, 233, 189, 2, 4, 255, 71, 32,
 1, 10, 8, 255, 50, 7, 2, 117, 95, 1, 138, 0, 1, 91, 74,
 255, 23, 7, 4, 244, 199, 2, 84, 68, 253, 0, 129, 0, 1, 217,
 175, 255, 140, 4, 4, 250, 203, 2, 3, 3, 145, 0, 243, 0, 13,
 2, 1, 0, 246, 199, 2, 255, 207, 2, 248, 201, 2, 2, 1, 178,
 0, 4, 10, 8, 0, 239, 195, 255, 65, 4, 255, 2, 6, 1, 142,
 115, 255, 69, 34, 2, 13, 10, 0, 255, 44, 6, 1, 105, 86, 255,
 44, 13, 1, 59, 48, 255, 23, 7, 1, 136, 111, 255, 138, 53, 206,
 0, 2, 159, 128, 1, 255, 143, 6, 1, 16, 13, 145, 0, 243, 0,
 11, 7, 6, 0, 255, 207, 2, 255, 207, 2, 229, 186, 2, 183, 0,
 2, 103, 82, 1, 255, 68, 6, 4, 221, 180, 2, 8, 6, 160, 0,
 1, 19, 15, 255, 116, 7, 1, 93, 75, 139, 0, 1, 37, 30, 255,
 23, 7, 2, 108, 87, 1, 253, 0, 131, 0, 1, 108, 88, 255, 215,
 7, 1, 39, 31, 145, 0, 243, 0, 11, 19, 16, 0, 255, 207, 2,
 255, 207, 2, 201, 162, 1, 183, 0, 10, 3, 2, 0, 112, 91, 1,
 180, 146, 1, 36, 29, 163, 0, 1, 28, 22, 255, 116, 7, 1, 84,
 68, 139, 0, 1, 18, 15, 255, 23, 7, 1, 151, 124, 255, 140, 58,
 201, 0, 1, 63, 52, 255, 143, 7, 1, 72, 58, 145, 0, 243, 0,
 11, 33, 27, 0, 255, 207, 2, 255, 207, 2, 149, 121, 1, 186, 0,
 128, 1, 2, 0, 3, 2, 166, 0, 1, 35, 29, 255, 116, 7, 1,
 67, 55, 139, 0, 4, 7, 6, 0, 252, 205, 255, 23, 4, 1, 207,
 168, 255, 140, 61, 198, 0, 0, 36, 255, 167, 8, 1, 122, 100, 255,
 143, 19, 243, 0, 11, 49, 40, 0, 255, 207, 2, 255, 207, 2, 114,
 92, 1, 231, 0, 1, 48, 39, 255, 116, 7, 1, 58, 47, 139, 0,
 128, 1, 2, 0, 241, 195, 255, 23, 4, 255, 2, 3, 253, 0, 131,
 0, 1, 13, 10, 255, 167, 7, 2, 178, 144, 1, 144, 0, 243, 0,
 10, 75, 61, 0, 255, 207, 2, 255, 207, 2, 88, 72, 232, 0, 1,
 65, 52, 255, 116, 7, 1, 45, 36, 142, 0, 1, 231, 187, 255, 23,
 4, 255, 2, 3, 128, 2, 226, 0, 4, 5, 4, 0, 95, 77, 151,
 0, 128, 2, 255, 167, 7, 1, 223, 180, 255, 141, 19, 243, 0, 10,
 104, 85, 1, 255, 207, 2, 255, 207, 2, 52, 42, 232, 0, 2, 84,
 68, 0, 255, 116, 6, 1, 32, 26, 142, 0, 1, 186, 152, 255, 143,
 7, 1, 11, 9, 223, 0, 8, 7, 5, 0, 162, 131, 1, 223, 180,
 2, 153, 0, 1, 219, 178, 255, 140, 4, 4, 250, 203, 2, 4, 3,
 142, 0, 243, 0, 10, 128, 104, 1, 255, 207, 2, 255, 207, 2, 33,
 27, 232, 0, 1, 105, 86, 255, 116, 7, 1, 23, 19, 142, 0, 1,
 139, 112, 255, 26, 7, 1, 27, 22, 220, 0, 4, 10, 9, 0, 178,
 144, 255, 107, 4, 4, 250, 203, 2, 4, 3, 151, 0, 1, 166, 135,
 255, 143, 7, 1, 17, 14, 142, 0, 243, 0, 10, 168, 136, 1, 255,
 207, 2, 255, 207, 2, 25, 20, 232, 0, 1, 139, 114, 255, 116, 7,
 1, 13, 11, 142, 0, 2, 95, 77, 0, 255, 26, 6, 1, 47, 39,
 217, 0, 4, 16, 13, 0, 182, 147, 255, 131, 7, 255, 2, 3, 255,
 14, 3, 150, 0, 1, 114, 92, 255, 38, 7, 1, 42, 33, 142, 0,
 243, 0, 10, 213, 173, 1, 255, 207, 2, 255, 207, 2, 11, 9, 232,
 0, 1, 176, 144, 255, 116, 7, 1, 7, 6, 142, 0, 2, 62, 50,
 0, 255, 26, 6, 1, 77, 61, 214, 0, 4, 18, 15, 0, 199, 162,
 255, 128, 7, 255, 2, 6, 1, 40, 32, 151, 0, 1, 63, 52, 255,
 143, 7, 1, 81, 66, 142, 0, 243, 0, 7, 229, 186, 2, 255, 207,
 2, 252, 205, 129, 2, 232, 0, 1, 219, 178, 255, 116, 4, 3, 248,
 203, 2, 3, 255, 116, 17, 4, 37, 30, 0, 255, 207, 255, 29, 4,
 2, 119, 96, 1, 210, 0, 5, 21, 16, 0, 199, 162, 1, 255, 98,
 6, 255, 2, 9, 1, 74, 60, 151, 0, 1, 33, 27, 255, 143, 7,
 1, 131, 107, 255, 143, 16, 240, 0, 128, 1, 10, 0, 248, 201, 2,
 255, 207, 2, 239, 195, 2, 1, 233, 0, 1, 231, 187, 255, 116, 4,
 2, 244, 199, 2, 144, 0, 4, 17, 14, 0, 255, 207, 255, 29, 4,
 1, 168, 138, 255, 145, 82, 5, 29, 23, 0, 203, 164, 1, 255, 95,
 6, 255, 2, 12, 1, 124, 101, 255, 107, 25, 1, 11, 9, 255, 143,
 7, 1, 191, 154, 255, 35, 16, 240, 0, 11, 7, 5, 0, 255, 207,
 2, 255, 207, 2, 223, 180, 2, 231, 0, 4, 3, 2, 0, 252, 205,
 255, 116, 4, 1, 215, 175, 255, 116, 19, 1, 6, 5, 255, 29, 7,
 0, 221, 255, 146, 80, 4, 29, 23, 0, 215, 175, 255, 89, 4, 255,
 2, 18, 2, 182, 147, 1, 150, 0, 128, 1, 0, 0, 255, 35, 6,
 1, 227, 184, 255, 143, 16, 240, 0, 11, 13, 11, 0, 255, 207, 2,
 255, 207, 2, 186, 149, 1, 231, 0, 1, 10, 9, 255, 116, 7, 0,
 184, 255, 116, 20, 128, 1, 2, 0, 239, 193, 255, 29, 4, 255, 2,
 3, 201, 0, 4, 29, 23, 0, 215, 175, 255, 86, 7, 255, 2, 18,
 1, 223, 180, 255, 107, 28, 2, 201, 162, 1, 255, 38, 6, 1, 6,
 5, 139, 0, 240, 0, 11, 22, 18, 0, 255, 207, 2, 255, 207, 2,
 146, 118, 1, 231, 0, 1, 27, 22, 255, 116, 7, 1, 144, 117, 255,
 116, 22, 1, 227, 184, 255, 29, 4, 255, 2, 3, 1, 5, 4, 199,
 0, 2, 189, 154, 1, 255, 83, 6, 255, 2, 12, 4, 186, 151, 1,
 233, 189, 255, 8, 4, 4, 250, 203, 2, 4, 3, 151, 0, 1, 151,
 122, 255, 59, 7, 1, 22, 18, 139, 0, 240, 0, 11, 40, 33, 0,
 255, 207, 2, 255, 207, 2, 118, 96, 1, 231, 0, 1, 51, 41, 255,
 116, 7, 1, 105, 86, 255, 116, 22, 2, 169, 138, 1, 255, 32, 6,
 1, 17, 14, 199, 0, 1, 191, 156, 255, 83, 7, 255, 2, 9, 7,
 180, 146, 1, 9, 7, 0, 162, 131, 255, 23, 7, 0, 18, 255, 107,
 26, 1, 93, 77, 255, 176, 7, 1, 51, 40, 139, 0, 240, 0, 10,
 58, 47, 0, 255, 207, 2, 255, 207, 2, 85, 69, 232, 0, 1, 82,
 67, 255, 116, 7, 1, 73, 58, 148, 0, 2, 119, 96, 1, 255, 32,
 6, 1, 39, 32, 199, 0, 1, 133, 107, 255, 83, 7, 255, 2, 6,
 4, 180, 146, 1, 8, 6, 130, 0, 1, 112, 91, 255, 23, 7, 1,
 43, 34, 151, 0, 1, 50, 40, 255, 176, 7, 1, 93, 77, 139, 0,
 240, 0, 10, 72, 58, 0, 255, 207, 2, 255, 207, 2, 58, 47, 232,
 0, 2, 130, 105, 1, 255, 116, 6, 1, 43, 35, 148, 0, 1, 81,
 67, 255, 149, 7, 1, 65, 53, 199, 0, 1, 84, 68, 255, 83, 7,
 255, 2, 3, 4, 180, 146, 1, 8, 6, 133, 0, 1, 63, 52, 255,
 23, 7, 1, 82, 67, 151, 0, 1, 22, 18, 255, 35, 7, 2, 149,
 121, 1, 138, 0, 240, 0, 10, 93, 77, 0, 255, 207, 2, 255, 207,
 2, 44, 36, 232, 0, 2, 197, 161, 1, 255, 116, 6, 1, 23, 18,
 148, 0, 1, 49, 40, 255, 149, 7, 2, 101, 82, 1, 198, 0, 1,
 47, 39, 255, 83, 7, 4, 205, 166, 1, 8, 6, 136, 0, 1, 33,
 26, 255, 23, 7, 1, 133, 108, 255, 107, 25, 1, 6, 5, 255, 35,
 7, 1, 203, 166, 255, 35, 13, 240, 0, 10, 131, 107, 1, 255, 207,
 2, 255, 207, 2, 33, 26, 232, 0, 1, 237, 191, 255, 113, 4, 4,
 252, 205, 2, 8, 7, 148, 0, 2, 26, 21, 0, 255, 149, 6, 2,
 147, 119, 1, 198, 0, 1, 20, 16, 255, 83, 7, 1, 169, 138, 255,
 83, 13, 1, 11, 9, 255, 23, 7, 1, 197, 159, 255, 107, 28, 1,
 233, 189, 255, 35, 4, 3, 244, 199, 2, 1, 255, 37, 11, 240, 0,
 10, 157, 128, 1, 255, 207, 2, 255, 207, 2, 17, 14, 229, 0, 2,
 5, 4, 0, 255, 113, 6, 4, 248, 201, 2, 1, 1, 148, 0, 1,
 9, 8, 255, 35, 7, 2, 211, 173, 2, 198, 0, 1, 6, 5, 255,
 83, 8, 0, 171, 255, 83, 13, 128, 1, 255, 23, 7, 2, 231, 187,
 2, 255, 142, 23, 130, 0, 2, 184, 149, 1, 255, 38, 6, 1, 12,
 9, 136, 0, 240, 0, 10, 171, 139, 1, 255, 207, 2, 255, 207, 2,
 7, 5, 229, 0, 2, 24, 19, 0, 255, 113, 6, 2, 215, 175, 2,
 150, 0, 128, 2, 2, 0, 244, 197, 255, 35, 4, 255, 2, 3, 198,
 0, 4, 1, 0, 0, 227, 184, 255, 83, 7, 1, 2, 1, 139, 0,
 1, 209, 168, 255, 23, 7, 1, 6, 5, 151, 0, 2, 125, 101, 1,
 255, 35, 6, 1, 34, 28, 136, 0, 240, 0, 10, 227, 184, 2, 255,
 207, 2, 252, 205, 2, 4, 3, 229, 0, 4, 56, 45, 0, 255, 207,
 255, 116, 4, 2, 161, 130, 1, 153, 0, 1, 231, 187, 255, 35, 4,
 255, 2, 3, 255, 152, 48, 4, 21, 17, 0, 8, 6, 148, 0, 2,
 191, 156, 1, 255, 83, 6, 1, 13, 10, 139, 0, 1, 152, 122, 255,
 23, 7, 1, 23, 19, 151, 0, 1, 74, 60, 255, 182, 7, 1, 71,
 57, 136, 0, 240, 0, 4, 235, 191, 2, 255, 207, 255, 2, 4, 231,
 0, 2, 111, 90, 1, 255, 113, 6, 2, 103, 82, 1, 153, 0, 1,
 186, 151, 255, 38, 7, 1, 15, 12, 169, 0, 6, 12, 9, 0, 193,
 157, 1, 3, 255, 205, 23, 1, 130, 105, 255, 83, 7, 1, 36, 29,
 139, 0, 2, 96, 79, 0, 255, 23, 6, 1, 54, 44, 151, 0, 255,
 50, 3, 255, 35, 6, 1, 124, 100, 255, 182, 10, 237, 0, 11, 1,
 0, 0, 244, 199, 2, 255, 207, 2, 223, 180, 2, 231, 0, 4, 182,
 147, 1, 255, 207, 255, 116, 4, 1, 58, 47, 154, 0, 1, 127, 103,
 255, 38, 7, 1, 39, 32, 166, 0, 7, 5, 4, 0, 176, 142, 1,
 231, 187, 255, 203, 25, 2, 82, 67, 0, 255, 83, 6, 1, 71, 57,
 139, 0, 1, 57, 46, 255, 23, 7, 1, 97, 79, 151, 0, 1, 13,
 10, 255, 35, 7, 2, 180, 146, 1, 135, 0, 237, 0, 128, 3, 9,
 0, 255, 207, 2, 255, 207, 2, 201, 162, 1, 168, 0, 1, 5, 4,
 184, 0, 128, 1, 2, 0, 246, 201, 255, 113, 4, 255, 2, 3, 1,
 27, 22, 154, 0, 1, 85, 69, 255, 155, 7, 1, 68, 55, 163, 0,
 10, 2, 1, 0, 146, 118, 1, 255, 207, 2, 171, 139, 255, 203, 25,
 1, 42, 34, 255, 83, 7, 1, 119, 97, 255, 35, 13, 1, 24, 19,
 255, 23, 7, 1, 159, 130, 255, 59, 25, 255, 95, 3, 1, 241, 197,
 255, 35, 4, 2, 237, 193, 2, 135, 0, 237, 0, 11, 9, 7, 0,
 255, 207, 2, 255, 207, 2, 184, 151, 1, 168, 0, 1, 141, 114, 255,
 44, 43, 141, 0, 1, 13, 11, 255, 113, 7, 255, 2, 3, 1, 6,
 5, 154, 0, 1, 51, 41, 255, 41, 7, 1, 104, 85, 255, 110, 37,
 4, 115, 93, 1, 252, 205, 255, 47, 4, 1, 114, 92, 255, 47, 25,
 1, 17, 14, 255, 83, 7, 1, 176, 142, 255, 35, 13, 1, 7, 6,
 255, 23, 7, 2, 213, 171, 2, 153, 0, 2, 203, 164, 1, 255, 185,
 15, 237, 0, 11, 14, 12, 0, 255, 207, 2, 255, 207, 2, 151, 122,
 1, 168, 0, 4, 221, 180, 2, 21, 17, 181, 0, 1, 48, 39, 255,
 113, 7, 2, 231, 187, 2, 156, 0, 1, 26, 21, 255, 41, 7, 1,
 159, 130, 255, 155, 34, 4, 80, 66, 0, 248, 201, 255, 44, 4, 255,
 2, 3, 1, 70, 56, 151, 0, 128, 3, 2, 0, 252, 205, 255, 35,
 4, 1, 233, 189, 255, 125, 16, 1, 237, 193, 255, 23, 4, 3, 246,
 201, 2, 3, 255, 151, 26, 2, 147, 119, 1, 255, 98, 6, 1, 23,
 18, 133, 0, 237, 0, 11, 18, 14, 0, 255, 207, 2, 255, 207, 2,
 122, 99, 1, 165, 0, 128, 2, 5, 0, 246, 199, 2, 87, 71, 181,
 0, 2, 105, 85, 1, 255, 113, 6, 1, 182, 147, 255, 113, 31, 1,
 9, 7, 255, 155, 7, 2, 221, 180, 2, 156, 0, 4, 51, 41, 0,
 237, 193, 255, 41, 4, 255, 2, 6, 1, 37, 30, 154, 0, 1, 217,
 176, 255, 38, 7, 1, 5, 4, 139, 0, 1, 184, 149, 255, 152, 7,
 1, 16, 13, 151, 0, 1, 85, 69, 255, 146, 7, 1, 55, 45, 133,
 0, 237, 0, 10, 32, 26, 0, 255, 207, 2, 255, 207, 2, 101, 82,
 166, 0, 1, 14, 12, 255, 50, 4, 2, 180, 146, 1, 180, 0, 2,
 197, 159, 1, 255, 113, 6, 1, 111, 90, 255, 65, 31, 4, 2, 1,
 0, 241, 195, 255, 41, 4, 255, 2, 3, 153, 0, 4, 26, 21, 0,
 221, 178, 255, 38, 7, 255, 2, 6, 1, 15, 12, 154, 0, 1, 173,
 141, 255, 128, 7, 1, 21, 17, 139, 0, 1, 124, 101, 255, 23, 7,
 1, 36, 29, 151, 0, 1, 43, 35, 255, 251, 4, 255, 2, 3, 1,
 101, 82, 133, 0, 237, 0, 10, 40, 32, 0, 255, 207, 2, 255, 207,
 2, 85, 68, 166, 0, 1, 33, 27, 255, 50, 4, 4, 241, 195, 2,
 7, 5, 175, 0, 4, 5, 4, 0, 250, 203, 255, 110, 4, 255, 2,
 3, 1, 58, 47, 160, 0, 1, 229, 186, 255, 44, 7, 1, 8, 6,
 148, 0, 5, 10, 8, 0, 193, 157, 1, 255, 35, 6, 255, 2, 6,
 4, 252, 205, 2, 3, 3, 154, 0, 1, 114, 92, 255, 47, 7, 1,
 50, 40, 139, 0, 1, 71, 57, 255, 215, 4, 255, 2, 3, 255, 8,
 3, 150, 0, 1, 18, 14, 255, 35, 7, 2, 164, 133, 1, 132, 0,
 237, 0, 10, 45, 36, 0, 255, 207, 2, 255, 207, 2, 65, 53, 166,
 0, 1, 55, 45, 255, 50, 7, 1, 56, 45, 175, 0, 1, 31, 25,
 255, 59, 7, 255, 2, 3, 1, 17, 14, 160, 0, 2, 169, 138, 1,
 255, 44, 6, 1, 23, 19, 145, 0, 4, 3, 2, 0, 159, 128, 255,
 32, 7, 255, 2, 9, 2, 229, 186, 2, 156, 0, 1, 63, 52, 255,
 131, 7, 1, 93, 77, 139, 0, 1, 39, 31, 255, 23, 7, 2, 121,
 99, 1, 150, 0, 4, 4, 3, 0, 248, 203, 255, 35, 4, 1, 231,
 187, 255, 101, 7, 237, 0, 10, 66, 54, 0, 255, 207, 2, 255, 207,
 2, 45, 37, 166, 0, 1, 85, 69, 255, 50, 7, 2, 164, 133, 1,
 174, 0, 2, 104, 85, 1, 255, 59, 6, 3, 239, 195, 2, 2, 255,
 61, 35, 1, 115, 93, 255, 47, 7, 1, 47, 39, 145, 0, 1, 111,
 91, 255, 29, 7, 255, 2, 12, 1, 186, 151, 255, 87, 31, 1, 31,
 25, 255, 191, 7, 1, 151, 122, 255, 41, 13, 1, 14, 11, 255, 23,
 7, 1, 171, 139, 255, 65, 28, 1, 225, 184, 255, 35, 4, 4, 250,
 203, 2, 3, 2, 130, 0, 237, 0, 10, 86, 70, 0, 255, 207, 2,
 255, 207, 2, 35, 29, 166, 0, 2, 128, 104, 1, 255, 50, 6, 4,
 239, 195, 2, 7, 6, 172, 0, 1, 211, 171, 255, 59, 7, 2, 193,
 156, 1, 162, 0, 1, 75, 60, 255, 158, 7, 1, 85, 69, 142, 0,
 4, 68, 55, 0, 248, 201, 255, 26, 4, 255, 2, 15, 1, 130, 105,
 255, 89, 31, 1, 9, 7, 255, 83, 7, 1, 207, 169, 255, 41, 13,
 128, 2, 2, 0, 248, 203, 255, 23, 4, 2, 231, 187, 2, 153, 0,
 1, 168, 136, 255, 194, 7, 1, 19, 15, 130, 0, 237, 0, 10, 88,
 72, 0, 255, 207, 2, 255, 207, 2, 30, 24, 166, 0, 2, 169, 138,
 1, 255, 50, 6, 255, 2, 3, 1, 61, 50, 169, 0, 4, 8, 7,
 0, 250, 203, 255, 56, 7, 2, 112, 91, 1, 162, 0, 1, 40, 32,
 255, 158, 7, 1, 135, 109, 255, 47, 13, 4, 31, 25, 0, 229, 186,
 255, 74, 7, 255, 2, 15, 1, 81, 66, 157, 0, 128, 1, 2, 0,
 237, 191, 255, 44, 7, 128, 1, 139, 0, 1, 213, 173, 255, 23, 7,
 128, 2, 151, 0, 1, 108, 88, 255, 254, 7, 1, 47, 38, 130, 0,
 237, 0, 10, 101, 82, 0, 255, 207, 2, 255, 207, 2, 18, 14, 166,
 0, 2, 205, 168, 1, 255, 50, 6, 255, 2, 3, 2, 175, 141, 1,
 168, 0, 1, 78, 62, 255, 107, 7, 255, 2, 3, 1, 45, 36, 163,
 0, 1, 17, 14, 255, 50, 7, 1, 186, 152, 255, 104, 10, 4, 11,
 9, 0, 197, 161, 255, 131, 10, 255, 2, 15, 1, 47, 38, 160, 0,
 1, 195, 159, 255, 62, 7, 1, 14, 11, 139, 0, 1, 171, 139, 255,
 23, 7, 1, 12, 10, 151, 0, 1, 58, 48, 255, 146, 7, 1, 88,
 71, 130, 0, 237, 0, 10, 117, 95, 1, 255, 207, 2, 255, 207, 2,
 14, 11, 166, 0, 1, 231, 187, 255, 47, 4, 255, 2, 6, 4, 252,
 205, 2, 26, 21, 166, 0, 1, 169, 136, 255, 107, 7, 255, 53, 3,
 1, 7, 6, 163, 0, 4, 6, 5, 0, 250, 203, 255, 107, 7, 132,
 0, 4, 2, 1, 0, 154, 125, 255, 71, 7, 255, 2, 12, 255, 38,
 9, 1, 22, 18, 160, 0, 1, 135, 109, 255, 65, 7, 1, 40, 33,
 139, 0, 1, 119, 97, 255, 23, 7, 1, 24, 19, 151, 0, 2, 25,
 21, 0, 255, 35, 6, 2, 139, 112, 1, 129, 0, 237, 0, 10, 128,
 104, 1, 255, 207, 2, 255, 207, 2, 11, 9, 163, 0, 4, 2, 1,
 0, 250, 203, 255, 47, 4, 255, 2, 9, 255, 65, 3, 162, 0, 4,
 16, 13, 0, 252, 205, 255, 50, 7, 2, 215, 175, 2, 168, 0, 1,
 235, 191, 255, 53, 7, 1, 4, 3, 130, 0, 2, 95, 78, 0, 255,
 125, 12, 255, 2, 6, 4, 121, 97, 1, 223, 180, 255, 11, 7, 1,
 6, 5, 160, 0, 1, 81, 66, 255, 68, 7, 1, 82, 68, 139, 0,
 1, 79, 65, 255, 23, 7, 1, 39, 31, 151, 0, 1, 6, 4, 255,
 35, 7, 1, 197, 159, 255, 248, 4, 237, 0, 10, 142, 117, 1, 255,
 207, 2, 252, 205, 2, 7, 5, 163, 0, 5, 8, 6, 0, 255, 207,
 2, 255, 2, 12, 4, 235, 191, 2, 11, 9, 160, 0, 1, 75, 61,
 255, 56, 10, 2, 136, 111, 1, 168, 0, 1, 199, 162, 255, 161, 4,
 255, 2, 3, 7, 17, 14, 0, 46, 38, 0, 244, 197, 255, 125, 13,
 255, 2, 3, 7, 162, 131, 1, 5, 4, 0, 246, 199, 255, 11, 4,
 2, 244, 199, 2, 255, 90, 36, 1, 38, 31, 255, 140, 7, 255, 137,
 15, 1, 47, 38, 255, 23, 7, 1, 55, 45, 154, 0, 255, 107, 6,
 2, 241, 195, 2, 129, 0, 237, 0, 7, 157, 128, 1, 255, 207, 2,
 255, 207, 129, 2, 163, 0, 2, 15, 12, 0, 255, 47, 6, 255, 2,
 12, 2, 119, 97, 1, 156, 0, 4, 2, 1, 0, 231, 187, 255, 47,
 10, 1, 40, 32, 169, 0, 1, 138, 112, 255, 161, 7, 4, 105, 86,
 1, 213, 173, 255, 122, 16, 10, 215, 173, 2, 8, 6, 0, 5, 4,
 0, 252, 205, 255, 14, 4, 1, 205, 168, 255, 137, 31, 132, 0, 0,
 14, 255, 197, 8, 1, 199, 162, 255, 47, 13, 255, 152, 3, 255, 23,
 6, 1, 56, 46, 154, 0, 1, 186, 151, 255, 146, 7, 2, 6, 5,
 0, 237, 0, 10, 173, 141, 1, 255, 207, 2, 248, 201, 2, 2, 1,
 163, 0, 5, 22, 18, 0, 255, 207, 2, 255, 2, 15, 4, 244, 199,
 2, 21, 16, 154, 0, 4, 62, 51, 0, 252, 205, 255, 44, 7, 4,
 250, 203, 2, 8, 6, 169, 0, 1, 85, 69, 255, 113, 7, 1, 246,
 199, 255, 119, 16, 4, 246, 201, 2, 28, 22, 130, 0, 1, 17, 13,
 255, 35, 7, 1, 161, 130, 255, 198, 37, 128, 2, 1, 0, 244, 255,
 77, 5, 255, 140, 3, 255, 248, 12, 1, 37, 31, 255, 71, 7, 1,
 59, 49, 154, 0, 2, 128, 104, 1, 255, 38, 6, 2, 20, 16, 0,
 237, 0, 10, 176, 144, 1, 255, 207, 2, 250, 203, 2, 1, 1, 163,
 0, 5, 36, 29, 0, 255, 207, 2, 255, 2, 18, 3, 168, 136, 1,
 2, 255, 65, 23, 4, 4, 3, 0, 189, 154, 255, 101, 4, 255, 2,
 6, 1, 199, 161, 255, 105, 38, 134, 0, 1, 51, 41, 255, 113, 22,
 255, 2, 3, 1, 71, 57, 133, 0, 1, 40, 32, 255, 35, 7, 1,
 112, 92, 255, 92, 40, 1, 205, 166, 255, 146, 7, 1, 16, 13, 136,
 0, 1, 45, 36, 255, 71, 7, 1, 55, 45, 154, 0, 1, 85, 69,
 255, 38, 7, 2, 35, 29, 0, 237, 0, 9, 189, 154, 1, 255, 207,
 2, 255, 207, 2, 1, 164, 0, 2, 50, 40, 0, 255, 47, 6, 1,
 252, 205, 255, 5, 4, 255, 2, 12, 1, 84, 69, 148, 0, 1, 96,
 78, 255, 50, 7, 255, 2, 6, 1, 78, 63, 172, 0, 1, 23, 19,
 255, 62, 13, 255, 2, 9, 1, 131, 107, 255, 178, 10, 1, 68, 55,
 255, 35, 7, 1, 74, 60, 166, 0, 1, 142, 115, 255, 248, 7, 1,
 47, 38, 136, 0, 1, 66, 54, 255, 71, 7, 1, 42, 34, 154, 0,
 1, 54, 44, 255, 38, 7, 2, 51, 41, 0, 237, 0, 10, 176, 142,
 1, 255, 207, 2, 237, 193, 2, 1, 1, 163, 0, 10, 69, 55, 0,
 255, 207, 2, 255, 207, 2, 146, 118, 255, 56, 4, 255, 2, 12, 4,
 239, 195, 2, 52, 42, 139, 0, 128, 2, 5, 0, 66, 54, 0, 239,
 195, 255, 38, 13, 1, 22, 17, 172, 0, 4, 7, 6, 0, 252, 205,
 255, 65, 13, 255, 2, 3, 4, 191, 156, 1, 5, 4, 136, 0, 1,
 104, 84, 255, 149, 7, 1, 45, 36, 166, 0, 1, 79, 63, 255, 50,
 7, 1, 107, 86, 255, 249, 10, 1, 131, 105, 255, 212, 7, 1, 33,
 27, 154, 0, 1, 36, 29, 255, 59, 7, 2, 72, 58, 0, 237, 0,
 8, 193, 157, 1, 255, 207, 2, 252, 205, 2, 165, 0, 13, 82, 67,
 0, 255, 207, 2, 255, 207, 2, 52, 42, 0, 191, 156, 255, 59, 4,
 255, 2, 12, 19, 225, 184, 2, 81, 66, 0, 19, 15, 0, 22, 18,
 0, 53, 43, 0, 147, 119, 1, 248, 201, 255, 32, 13, 2, 195, 159,
 1, 174, 0, 128, 1, 2, 0, 237, 193, 255, 68, 13, 4, 239, 195,
 2, 23, 18, 139, 0, 1, 149, 121, 255, 137, 7, 255, 23, 15, 153,
 0, 1, 35, 28, 255, 200, 7, 1, 201, 162, 255, 146, 7, 4, 11,
 9, 0, 233, 189, 255, 98, 7, 1, 18, 14, 154, 0, 1, 24, 19,
 255, 59, 7, 2, 88, 71, 0, 237, 0, 8, 197, 161, 1, 255, 207,
 2, 248, 201, 2, 165, 0, 16, 85, 69, 0, 255, 207, 2, 255, 207,
 2, 29, 23, 0, 56, 45, 0, 252, 205, 255, 11, 4, 255, 2, 12,
 255, 74, 3, 7, 225, 182, 2, 227, 184, 2, 244, 199, 255, 26, 16,
 255, 2, 3, 1, 99, 80, 178, 0, 1, 205, 166, 255, 164, 4, 255,
 2, 9, 1, 63, 52, 142, 0, 1, 201, 162, 255, 32, 4, 255, 140,
 3, 1, 7, 6, 166, 0, 1, 10, 8, 255, 200, 7, 255, 53, 3,
 7, 84, 68, 0, 5, 4, 0, 182, 149, 255, 101, 7, 4, 250, 205,
 2, 4, 3, 154, 0, 1, 14, 11, 255, 59, 7, 2, 105, 85, 1,
 237, 0, 10, 180, 146, 1, 255, 207, 2, 239, 195, 2, 1, 1, 163,
 0, 16, 95, 78, 0, 255, 207, 2, 255, 207, 2, 19, 16, 0, 4,
 3, 0, 221, 180, 255, 11, 4, 255, 2, 39, 4, 248, 203, 2, 8,
 6, 178, 0, 1, 146, 118, 255, 164, 4, 255, 2, 6, 1, 136, 109,
 255, 169, 19, 1, 237, 193, 255, 95, 5, 1, 201, 2, 255, 196, 38,
 255, 53, 13, 255, 56, 6, 1, 229, 186, 255, 167, 10, 2, 219, 176,
 2, 156, 0, 2, 10, 8, 0, 255, 41, 6, 2, 118, 96, 1, 237,
 0, 9, 178, 144, 1, 255, 207, 2, 255, 207, 2, 1, 164, 0, 2,
 91, 73, 0, 255, 47, 6, 1, 28, 23, 130, 0, 1, 90, 72, 255,
 14, 7, 255, 2, 36, 1, 161, 130, 255, 100, 39, 142, 0, 0, 88,
 255, 101, 8, 4, 203, 164, 1, 7, 5, 142, 0, 1, 5, 4, 255,
 29, 7, 1, 215, 173, 255, 95, 46, 1, 168, 136, 255, 251, 7, 255,
 2, 15, 1, 133, 107, 255, 71, 31, 255, 113, 9, 2, 141, 115, 1,
 237, 0, 7, 173, 141, 1, 255, 207, 2, 255, 207, 129, 2, 163, 0,
 2, 78, 63, 0, 255, 47, 6, 1, 45, 36, 130, 0, 4, 7, 5,
 0, 223, 180, 255, 14, 4, 255, 2, 36, 1, 44, 35, 181, 0, 1,
 51, 41, 255, 116, 4, 4, 244, 199, 2, 31, 25, 145, 0, 1, 14,
 11, 255, 146, 7, 2, 164, 133, 1, 171, 0, 1, 86, 70, 255, 56,
 7, 255, 2, 15, 0, 43, 255, 167, 32, 1, 6, 5, 255, 56, 7,
 2, 139, 112, 1, 237, 0, 10, 159, 128, 1, 255, 207, 2, 252, 205,
 2, 7, 5, 163, 0, 10, 63, 52, 0, 255, 207, 2, 255, 207, 2,
 63, 51, 133, 0, 4, 87, 71, 0, 252, 205, 255, 17, 4, 255, 2,
 30, 2, 187, 152, 1, 183, 0, 1, 20, 16, 255, 116, 4, 1, 88,
 71, 148, 0, 1, 33, 27, 255, 146, 7, 1, 121, 97, 255, 98, 46,
 1, 23, 19, 255, 56, 7, 255, 2, 12, 3, 227, 184, 2, 1, 255,
 70, 32, 1, 4, 3, 255, 56, 7, 2, 146, 118, 1, 237, 0, 10,
 147, 119, 1, 255, 207, 2, 255, 207, 2, 13, 10, 163, 0, 2, 46,
 38, 0, 255, 47, 6, 1, 93, 75, 133, 0, 4, 2, 1, 0, 199,
 162, 255, 68, 7, 255, 2, 24, 4, 252, 205, 2, 40, 32, 184, 0,
 7, 6, 5, 0, 164, 133, 1, 2, 2, 148, 0, 1, 57, 46, 255,
 146, 7, 1, 82, 67, 175, 0, 1, 207, 168, 255, 185, 16, 1, 55,
 44, 160, 0, 4, 4, 3, 0, 252, 205, 255, 44, 4, 2, 139, 114,
 1, 237, 0, 10, 130, 105, 1, 255, 207, 2, 255, 207, 2, 22, 18,
 163, 0, 2, 32, 27, 0, 255, 47, 6, 2, 128, 104, 1, 135, 0,
 4, 37, 30, 0, 250, 203, 255, 20, 4, 255, 2, 24, 1, 146, 118,
 129, 1, 187, 0, 1, 14, 11, 151, 0, 1, 86, 70, 255, 146, 7,
 1, 52, 42, 175, 0, 4, 30, 25, 0, 244, 199, 255, 167, 10, 1,
 161, 130, 255, 167, 37, 4, 5, 4, 0, 252, 205, 255, 47, 4, 2,
 133, 108, 1, 237, 0, 10, 109, 88, 1, 255, 207, 2, 255, 207, 2,
 37, 30, 163, 0, 2, 19, 15, 0, 255, 47, 6, 2, 171, 138, 1,
 138, 0, 4, 107, 86, 1, 250, 203, 255, 23, 4, 255, 2, 18, 4,
 219, 178, 2, 13, 11, 217, 0, 1, 124, 100, 255, 194, 7, 1, 29,
 23, 178, 0, 13, 62, 51, 0, 233, 189, 2, 252, 205, 2, 191, 154,
 1, 9, 7, 163, 0, 255, 38, 3, 255, 113, 6, 2, 117, 95, 1,
 237, 0, 10, 87, 71, 0, 255, 207, 2, 255, 207, 2, 57, 47, 163,
 0, 1, 9, 7, 255, 47, 7, 2, 201, 162, 1, 138, 0, 128, 1,
 3, 0, 151, 122, 1, 255, 26, 6, 255, 2, 12, 4, 203, 166, 1,
 32, 26, 220, 0, 1, 171, 138, 255, 119, 7, 1, 13, 10, 181, 0,
 7, 19, 16, 0, 53, 43, 0, 4, 3, 166, 0, 2, 10, 9, 0,
 255, 113, 6, 2, 114, 92, 1, 237, 0, 10, 68, 55, 0, 255, 207,
 2, 255, 207, 2, 81, 66, 163, 0, 1, 2, 1, 255, 47, 7, 4,
 227, 184, 2, 1, 1, 139, 0, 22, 6, 5, 0, 125, 101, 1, 239,
 193, 2, 250, 203, 2, 246, 199, 2, 221, 178, 2, 127, 103, 1, 19,
 15, 223, 0, 10, 209, 169, 1, 255, 207, 2, 248, 203, 2, 4, 3,
 229, 0, 11, 21, 16, 0, 255, 207, 2, 255, 207, 2, 90, 73, 0,
 237, 0, 11, 49, 40, 0, 255, 207, 2, 255, 207, 2, 111, 91, 1,
 165, 0, 255, 47, 6, 255, 2, 3, 128, 3, 142, 0, 15, 2, 1,
 0, 34, 28, 0, 66, 54, 0, 54, 43, 0, 17, 14, 0, 1, 255,
 82, 40, 187, 0, 1, 244, 197, 255, 140, 4, 2, 237, 191, 2, 231,
 0, 4, 31, 25, 0, 255, 207, 255, 116, 4, 2, 79, 63, 0, 237,
 0, 11, 32, 26, 0, 255, 207, 2, 255, 207, 2, 147, 119, 1, 165,
 0, 2, 201, 164, 1, 255, 50, 6, 1, 14, 11, 253, 0, 129, 0,
 4, 4, 3, 0, 252, 205, 255, 140, 4, 1, 209, 171, 255, 191, 40,
 192, 0, 4, 47, 38, 0, 255, 207, 255, 116, 4, 2, 57, 46, 0,
 237, 0, 11, 18, 15, 0, 255, 207, 2, 255, 207, 2, 182, 147, 1,
 165, 0, 2, 161, 130, 1, 255, 50, 6, 1, 29, 24, 253, 0, 129,
 0, 1, 12, 9, 255, 191, 7, 1, 157, 128, 255, 191, 40, 192, 0,
 1, 79, 63, 255, 116, 7, 2, 36, 29, 0, 237, 0, 11, 9, 7,
 0, 255, 207, 2, 255, 207, 2, 211, 169, 1, 165, 0, 2, 122, 99,
 1, 255, 50, 6, 1, 51, 41, 253, 0, 129, 0, 1, 26, 21, 255,
 191, 7, 1, 114, 92, 255, 191, 40, 192, 0, 2, 112, 92, 1, 255,
 116, 6, 2, 21, 17, 0, 237, 0, 128, 2, 10, 0, 255, 207, 2,
 255, 207, 2, 244, 199, 2, 1, 164, 0, 1, 88, 72, 255, 50, 7,
 1, 85, 69, 253, 0, 129, 0, 1, 50, 40, 255, 140, 7, 1, 78,
 62, 232, 0, 2, 161, 131, 1, 255, 116, 6, 2, 8, 7, 0, 240,
 0, 4, 239, 195, 2, 255, 207, 255, 2, 4, 1, 4, 3, 163, 0,
 2, 56, 45, 0, 255, 47, 6, 2, 122, 99, 1, 253, 0, 128, 0,
 1, 75, 61, 255, 140, 7, 1, 50, 40, 232, 0, 1, 221, 178, 255,
 113, 4, 5, 248, 201, 2, 2, 1, 0, 240, 0, 10, 205, 168, 1,
 255, 207, 2, 255, 207, 2, 14, 11, 163, 0, 2, 33, 27, 0, 255,
 47, 6, 2, 168, 135, 1, 253, 0, 128, 0, 1, 104, 85, 255, 188,
 7, 1, 29, 23, 229, 0, 128, 2, 2, 0, 248, 201, 255, 113, 4,
 2, 231, 187, 2, 129, 0, 240, 0, 10, 176, 142, 1, 255, 207, 2,
 255, 207, 2, 27, 22, 163, 0, 2, 15, 12, 0, 255, 47, 6, 2,
 203, 166, 1, 253, 0, 128, 0, 1, 142, 115, 255, 188, 7, 1, 16,
 13, 229, 0, 1, 15, 13, 255, 254, 7, 1, 171, 139, 255, 254, 4,
 240, 0, 10, 139, 112, 1, 255, 207, 2, 255, 207, 2, 46, 37, 163,
 0, 2, 4, 3, 0, 255, 47, 6, 2, 246, 201, 2, 253, 0, 128,
 0, 1, 176, 142, 255, 188, 7, 1, 6, 5, 229, 0, 1, 46, 38,
 255, 254, 7, 2, 115, 93, 1, 129, 0, 240, 0, 10, 104, 84, 1,
 255, 207, 2, 255, 207, 2, 75, 60, 163, 0, 128, 1, 2, 0, 233,
 187, 255, 47, 4, 255, 2, 3, 1, 6, 5, 253, 0, 1, 223, 182,
 255, 134, 4, 4, 248, 201, 2, 1, 1, 229, 0, 2, 101, 82, 0,
 255, 251, 6, 1, 62, 51, 130, 0, 240, 0, 11, 71, 58, 0, 255,
 207, 2, 255, 207, 2, 107, 87, 1, 165, 0, 2, 201, 162, 1, 255,
 50, 6, 1, 17, 14, 253, 0, 1, 239, 195, 255, 134, 4, 2, 225,
 182, 2, 231, 0, 1, 182, 147, 255, 251, 7, 1, 27, 22, 130, 0,
 240, 0, 11, 49, 40, 0, 255, 207, 2, 255, 207, 2, 149, 121, 1,
 165, 0, 2, 161, 130, 1, 255, 50, 6, 1, 36, 29, 250, 0, 4,
 4, 3, 0, 252, 205, 255, 134, 4, 1, 187, 152, 255, 185, 40, 189,
 0, 128, 1, 2, 0, 246, 201, 255, 113, 4, 255, 119, 3, 1, 6,
 5, 130, 0, 240, 0, 11, 28, 22, 0, 255, 207, 2, 255, 207, 2,
 186, 151, 1, 165, 0, 2, 112, 91, 1, 255, 50, 6, 1, 67, 55,
 250, 0, 1, 10, 8, 255, 185, 7, 1, 152, 124, 255, 185, 40, 189,
 0, 1, 21, 17, 255, 113, 7, 2, 221, 180, 2, 132, 0, 240, 0,
 11, 13, 11, 0, 255, 207, 2, 255, 207, 2, 231, 187, 2, 165, 0,
 1, 74, 59, 255, 50, 7, 2, 109, 90, 1, 249, 0, 1, 22, 18,
 255, 134, 7, 1, 117, 95, 255, 134, 103, 1, 72, 58, 255, 113, 7,
 1, 144, 117, 255, 113, 7, 240, 0, 7, 4, 3, 0, 252, 205, 2,
 255, 207, 255, 2, 4, 128, 1, 163, 0, 2, 44, 35, 0, 255, 47,
 6, 1, 156, 125, 255, 48, 38, 212, 0, 1, 37, 30, 255, 134, 7,
 1, 87, 71, 229, 0, 2, 168, 136, 1, 255, 113, 6, 1, 73, 59,
 133, 0, 240, 0, 128, 1, 5, 0, 229, 186, 2, 255, 207, 255, 2,
 4, 1, 6, 5, 163, 0, 2, 22, 17, 0, 255, 47, 6, 2, 201,
 162, 1, 249, 0, 1, 51, 41, 255, 134, 7, 1, 62, 51, 226, 0,
 4, 4, 3, 0, 244, 197, 255, 110, 4, 255, 2, 3, 1, 24, 19,
 133, 0, 243, 0, 10, 209, 168, 1, 255, 207, 2, 255, 207, 2, 17,
 14, 163, 0, 2, 7, 6, 0, 255, 47, 6, 255, 2, 3, 249, 0,
 1, 73, 59, 255, 134, 7, 1, 41, 33, 166, 0, 4, 61, 50, 0,
 9, 7, 181, 0, 1, 56, 45, 255, 110, 7, 1, 248, 201, 129, 2,
 133, 0, 243, 0, 10, 176, 144, 1, 255, 207, 2, 255, 207, 2, 36,
 29, 163, 0, 4, 2, 1, 0, 237, 191, 255, 47, 4, 255, 2, 3,
 1, 7, 5, 247, 0, 2, 93, 75, 0, 255, 131, 6, 1, 25, 21,
 166, 0, 5, 138, 111, 1, 136, 109, 1, 180, 0, 2, 197, 159, 1,
 255, 110, 6, 1, 178, 146, 255, 65, 10, 243, 0, 10, 133, 108, 1,
 255, 207, 2, 255, 207, 2, 60, 49, 166, 0, 1, 209, 169, 255, 50,
 7, 1, 21, 17, 247, 0, 1, 124, 101, 255, 131, 7, 1, 16, 13,
 166, 0, 7, 201, 164, 1, 248, 201, 2, 21, 16, 175, 0, 2, 30,
 24, 0, 255, 107, 6, 255, 2, 3, 1, 74, 60, 136, 0, 243, 0,
 10, 91, 73, 0, 255, 207, 2, 255, 207, 2, 95, 77, 166, 0, 2,
 171, 138, 1, 255, 50, 6, 1, 46, 38, 199, 0, 128, 1, 5, 0,
 5, 4, 0, 2, 1, 166, 0, 1, 156, 125, 255, 131, 7, 1, 8,
 6, 255, 95, 40, 1, 244, 197, 255, 47, 4, 1, 176, 142, 255, 99,
 41, 134, 0, 1, 184, 149, 255, 107, 7, 4, 252, 205, 2, 14, 12,
 136, 0, 243, 0, 11, 60, 49, 0, 255, 207, 2, 255, 207, 2, 130,
 107, 1, 165, 0, 2, 121, 99, 1, 255, 50, 6, 1, 81, 67, 193,
 0, 128, 1, 17, 0, 58, 47, 0, 162, 131, 1, 203, 164, 1, 187,
 151, 1, 90, 73, 0, 6, 5, 160, 0, 1, 180, 146, 255, 131, 4,
 4, 252, 205, 2, 3, 2, 163, 0, 1, 8, 7, 255, 230, 7, 255,
 50, 3, 1, 57, 47, 169, 0, 1, 47, 38, 255, 56, 7, 255, 2,
 3, 2, 199, 162, 1, 138, 0, 243, 0, 11, 36, 29, 0, 255, 207,
 2, 255, 207, 2, 173, 141, 1, 165, 0, 1, 74, 60, 255, 50, 7,
 1, 125, 101, 255, 50, 40, 150, 0, 7, 5, 4, 0, 146, 118, 1,
 250, 203, 255, 77, 4, 255, 2, 6, 7, 250, 205, 2, 193, 157, 1,
 18, 14, 157, 0, 8, 207, 168, 1, 255, 207, 2, 244, 199, 2, 255,
 129, 39, 1, 25, 21, 255, 179, 7, 255, 2, 3, 4, 225, 182, 2,
 4, 3, 163, 0, 255, 38, 3, 1, 235, 191, 255, 155, 10, 1, 79,
 63, 139, 0, 243, 0, 11, 18, 15, 0, 255, 207, 2, 255, 207, 2,
 233, 189, 2, 165, 0, 1, 42, 34, 255, 50, 7, 2, 176, 144, 1,
 186, 0, 128, 1, 3, 0, 159, 128, 1, 255, 74, 6, 255, 2, 15,
 4, 211, 171, 1, 13, 10, 154, 0, 1, 227, 184, 255, 38, 4, 255,
 5, 3, 165, 0, 1, 47, 38, 255, 179, 7, 255, 2, 6, 1, 122,
 99, 255, 185, 37, 1, 127, 103, 255, 158, 10, 4, 244, 199, 2, 6,
 5, 139, 0, 243, 0, 13, 6, 5, 0, 252, 205, 2, 255, 207, 2,
 248, 201, 2, 1, 1, 163, 0, 4, 22, 18, 0, 255, 207, 255, 50,
 4, 2, 233, 187, 2, 186, 0, 1, 99, 80, 255, 122, 7, 255, 2,
 21, 3, 173, 141, 1, 2, 255, 143, 26, 1, 239, 195, 255, 35, 4,
 1, 209, 169, 255, 177, 38, 128, 0, 1, 77, 62, 255, 179, 7, 255,
 2, 6, 255, 119, 3, 1, 59, 48, 157, 0, 4, 52, 42, 0, 248,
 201, 255, 50, 10, 1, 114, 92, 255, 107, 16, 243, 0, 128, 1, 11,
 0, 237, 193, 2, 255, 207, 2, 252, 205, 2, 8, 7, 163, 0, 4,
 7, 6, 0, 252, 205, 255, 50, 4, 255, 2, 3, 128, 2, 181, 0,
 4, 17, 14, 0, 246, 199, 255, 68, 7, 255, 2, 24, 1, 92, 74,
 255, 179, 25, 255, 128, 6, 2, 187, 152, 1, 165, 0, 2, 108, 87,
 1, 255, 92, 15, 4, 241, 195, 2, 40, 32, 151, 0, 4, 27, 22,
 0, 227, 184, 255, 44, 10, 4, 248, 203, 2, 12, 9, 142, 0, 246,
 0, 4, 227, 184, 2, 255, 207, 255, 2, 4, 1, 21, 17, 163, 0,
 128, 1, 2, 0, 231, 187, 255, 50, 7, 1, 15, 13, 181, 0, 2,
 151, 122, 1, 255, 65, 6, 255, 2, 27, 4, 239, 195, 2, 11, 9,
 145, 0, 4, 3, 2, 0, 250, 205, 255, 32, 4, 2, 161, 131, 1,
 165, 0, 1, 147, 119, 255, 110, 19, 4, 237, 191, 2, 55, 45, 145,
 0, 4, 40, 32, 0, 237, 193, 255, 41, 13, 1, 101, 82, 255, 104,
 19, 246, 0, 10, 184, 151, 1, 255, 207, 2, 255, 207, 2, 41, 33,
 166, 0, 1, 209, 169, 255, 50, 7, 1, 35, 28, 178, 0, 2, 29,
 23, 0, 255, 62, 6, 255, 2, 33, 2, 136, 111, 1, 144, 0, 1,
 5, 4, 255, 62, 7, 1, 139, 112, 255, 29, 19, 147, 0, 0, 186,
 255, 227, 8, 255, 2, 15, 7, 248, 201, 2, 144, 117, 1, 26, 21,
 255, 83, 4, 10, 6, 5, 0, 22, 18, 0, 127, 104, 1, 237, 193,
 255, 35, 13, 4, 219, 178, 2, 5, 4, 145, 0, 246, 0, 10, 133,
 108, 1, 255, 207, 2, 255, 207, 2, 68, 55, 166, 0, 1, 152, 124,
 255, 50, 7, 1, 62, 50, 178, 0, 1, 173, 141, 255, 62, 7, 255,
 2, 33, 4, 244, 197, 2, 16, 13, 142, 0, 2, 8, 6, 0, 255,
 29, 6, 2, 127, 103, 1, 165, 0, 1, 213, 173, 255, 113, 4, 4,
 241, 195, 2, 171, 138, 255, 122, 19, 7, 250, 203, 2, 223, 182, 2,
 221, 180, 255, 8, 4, 255, 29, 18, 1, 62, 51, 148, 0, 246, 0,
 11, 90, 72, 0, 255, 207, 2, 255, 207, 2, 105, 86, 1, 165, 0,
 2, 100, 81, 1, 255, 50, 6, 1, 108, 87, 255, 50, 40, 135, 0,
 1, 23, 19, 255, 110, 7, 255, 2, 12, 7, 252, 205, 2, 246, 199,
 2, 246, 201, 255, 23, 16, 255, 2, 3, 1, 125, 103, 255, 98, 16,
 1, 11, 9, 255, 65, 7, 1, 114, 93, 255, 125, 40, 1, 239, 195,
 255, 47, 4, 7, 225, 182, 2, 12, 10, 0, 227, 184, 255, 101, 19,
 255, 2, 24, 1, 166, 135, 255, 98, 25, 246, 0, 11, 59, 48, 0,
 255, 207, 2, 255, 207, 2, 152, 122, 1, 165, 0, 1, 62, 50, 255,
 50, 7, 1, 156, 127, 255, 50, 40, 135, 0, 2, 139, 112, 1, 255,
 59, 6, 255, 2, 9, 16, 211, 171, 1, 97, 79, 0, 36, 29, 0,
 48, 40, 0, 135, 109, 1, 246, 199, 255, 29, 13, 4, 237, 193, 2,
 5, 4, 139, 0, 1, 12, 10, 255, 125, 7, 1, 105, 86, 255, 125,
 37, 128, 1, 2, 0, 246, 201, 255, 47, 4, 1, 199, 161, 255, 47,
 4, 4, 54, 43, 0, 252, 205, 255, 98, 13, 255, 2, 24, 4, 229,
 186, 2, 9, 7, 151, 0, 246, 0, 11, 34, 28, 0, 255, 207, 2,
 255, 207, 2, 209, 169, 1, 165, 0, 1, 29, 24, 255, 50, 7, 2,
 223, 180, 2, 171, 0, 4, 4, 3, 0, 231, 187, 255, 56, 4, 255,
 2, 9, 4, 180, 146, 1, 18, 15, 139, 0, 4, 61, 50, 0, 244,
 197, 255, 35, 13, 255, 17, 3, 138, 0, 1, 13, 10, 255, 125, 7,
 1, 117, 95, 255, 176, 37, 4, 3, 2, 0, 252, 205, 255, 47, 4,
 1, 191, 154, 255, 47, 7, 1, 97, 80, 255, 17, 7, 255, 2, 27,
 255, 32, 3, 1, 38, 31, 154, 0, 246, 0, 11, 16, 13, 0, 255,
 207, 2, 255, 207, 2, 239, 195, 2, 165, 0, 1, 11, 9, 255, 50,
 7, 255, 2, 3, 128, 1, 169, 0, 1, 46, 37, 255, 56, 10, 255,
 2, 3, 4, 207, 168, 1, 13, 10, 145, 0, 1, 66, 53, 255, 38,
 13, 1, 187, 152, 255, 99, 13, 255, 50, 3, 255, 23, 6, 1, 117,
 95, 255, 123, 37, 1, 7, 5, 255, 47, 7, 1, 227, 186, 255, 224,
 7, 255, 54, 3, 4, 115, 93, 1, 250, 205, 255, 98, 10, 255, 2,
 15, 4, 239, 195, 2, 56, 45, 157, 0, 246, 0, 13, 5, 4, 0,
 250, 203, 2, 255, 207, 2, 250, 203, 2, 5, 4, 163, 0, 128, 2,
 2, 0, 241, 195, 255, 50, 4, 255, 2, 3, 1, 11, 9, 169, 0,
 2, 152, 124, 1, 255, 53, 6, 255, 2, 3, 4, 252, 205, 2, 46,
 37, 151, 0, 1, 124, 100, 255, 41, 10, 255, 152, 3, 1, 13, 10,
 136, 0, 2, 10, 8, 0, 255, 23, 6, 2, 135, 109, 1, 162, 0,
 128, 1, 255, 47, 7, 255, 113, 3, 1, 9, 7, 133, 0, 255, 57,
 3, 4, 97, 79, 0, 241, 197, 255, 98, 7, 255, 2, 12, 4, 227,
 184, 2, 49, 40, 160, 0, 249, 0, 4, 246, 201, 2, 255, 207, 255,
 2, 4, 1, 15, 13, 166, 0, 1, 215, 175, 255, 50, 7, 1, 31,
 25, 169, 0, 1, 233, 189, 255, 53, 7, 255, 2, 3, 3, 159, 130,
 1, 1, 152, 0, 4, 7, 6, 0, 229, 186, 255, 44, 10, 1, 77,
 61, 136, 0, 128, 4, 0, 0, 255, 20, 6, 1, 173, 142, 255, 64,
 27, 139, 0, 2, 207, 168, 1, 255, 50, 6, 1, 35, 29, 139, 0,
 255, 188, 3, 19, 157, 128, 1, 231, 187, 2, 248, 201, 2, 246, 201,
 2, 221, 178, 2, 125, 101, 1, 11, 9, 163, 0, 249, 0, 4, 217,
 176, 2, 255, 207, 255, 2, 4, 1, 33, 27, 166, 0, 2, 171, 139,
 1, 255, 50, 6, 1, 62, 50, 166, 0, 2, 29, 23, 0, 255, 50,
 6, 255, 2, 3, 4, 246, 199, 2, 29, 23, 157, 0, 1, 87, 71,
 255, 47, 10, 2, 182, 147, 1, 135, 0, 128, 2, 2, 0, 248, 203,
 255, 20, 4, 2, 231, 187, 2, 165, 0, 1, 117, 93, 255, 173, 7,
 255, 83, 3, 144, 0, 10, 9, 7, 0, 25, 21, 0, 22, 18, 0,
 6, 4, 169, 0, 249, 0, 10, 161, 130, 1, 255, 207, 2, 255, 207,
 2, 59, 48, 166, 0, 1, 108, 87, 255, 50, 7, 255, 8, 3, 165,
 0, 2, 77, 62, 0, 255, 50, 6, 255, 2, 3, 1, 154, 125, 255,
 53, 34, 4, 6, 5, 0, 239, 195, 255, 47, 7, 255, 2, 3, 255,
 14, 3, 135, 0, 0, 241, 255, 23, 5, 4, 252, 205, 2, 3, 2,
 163, 0, 1, 53, 43, 255, 122, 7, 1, 161, 131, 255, 173, 40, 159,
 0, 249, 0, 11, 119, 97, 1, 255, 207, 2, 255, 207, 2, 103, 84,
 1, 165, 0, 2, 67, 54, 0, 255, 50, 6, 1, 156, 127, 255, 50,
 40, 1, 164, 133, 255, 101, 7, 4, 252, 205, 2, 36, 30, 163, 0,
 1, 114, 92, 255, 50, 7, 255, 2, 3, 1, 47, 39, 136, 0, 1,
 187, 152, 255, 23, 7, 1, 15, 12, 163, 0, 1, 16, 13, 255, 173,
 7, 2, 223, 180, 2, 198, 0, 249, 0, 11, 77, 61, 0, 255, 207,
 2, 255, 207, 2, 147, 121, 1, 165, 0, 1, 33, 27, 255, 50, 7,
 2, 227, 184, 2, 165, 0, 1, 235, 191, 255, 47, 4, 255, 2, 3,
 1, 223, 180, 255, 50, 40, 4, 20, 16, 0, 248, 203, 255, 53, 7,
 1, 131, 107, 255, 155, 10, 2, 142, 115, 1, 255, 20, 6, 1, 43,
 35, 163, 0, 128, 1, 255, 173, 7, 255, 2, 3, 1, 8, 7, 196,
 0, 249, 0, 11, 46, 37, 0, 255, 207, 2, 255, 207, 2, 203, 164,
 1, 165, 0, 1, 12, 10, 255, 50, 7, 255, 2, 3, 128, 1, 160,
 0, 4, 10, 9, 0, 252, 205, 255, 47, 7, 1, 77, 62, 169, 0,
 2, 193, 156, 1, 255, 53, 6, 1, 213, 171, 255, 102, 10, 1, 91,
 74, 255, 125, 7, 1, 84, 68, 166, 0, 0, 191, 255, 71, 8, 1,
 40, 32, 196, 0, 249, 0, 11, 23, 19, 0, 255, 207, 2, 255, 207,
 2, 235, 191, 2, 165, 0, 128, 2, 2, 0, 239, 193, 255, 50, 4,
 255, 2, 3, 1, 15, 12, 160, 0, 1, 48, 39, 255, 98, 7, 255,
 2, 3, 1, 24, 19, 169, 0, 1, 82, 67, 255, 56, 10, 1, 10,
 8, 133, 0, 1, 52, 42, 255, 20, 7, 2, 146, 118, 1, 165, 0,
 2, 114, 92, 1, 255, 50, 6, 1, 93, 75, 196, 0, 249, 0, 13,
 7, 6, 0, 255, 207, 2, 255, 207, 2, 246, 199, 2, 5, 4, 166,
 0, 1, 213, 173, 255, 50, 4, 255, 2, 3, 1, 35, 29, 160, 0,
 1, 88, 71, 255, 98, 7, 2, 221, 178, 2, 171, 0, 1, 15, 12,
 255, 56, 7, 255, 2, 3, 1, 50, 40, 133, 0, 1, 20, 16, 255,
 20, 7, 2, 207, 168, 1, 165, 0, 1, 52, 42, 255, 50, 7, 1,
 171, 138, 255, 50, 40, 156, 0, 249, 0, 5, 1, 0, 0, 255, 207,
 2, 255, 2, 6, 1, 15, 12, 166, 0, 2, 162, 133, 1, 255, 50,
 6, 1, 75, 61, 160, 0, 1, 149, 121, 255, 44, 7, 2, 151, 122,
 1, 174, 0, 1, 223, 180, 255, 155, 7, 1, 118, 95, 255, 59, 7,
 1, 4, 3, 255, 176, 7, 4, 252, 205, 2, 4, 3, 163, 0, 1,
 14, 11, 255, 50, 7, 3, 231, 187, 2, 1, 255, 130, 49, 145, 0,
 252, 0, 4, 229, 186, 2, 255, 207, 255, 2, 4, 1, 34, 28, 166,
 0, 2, 103, 82, 1, 255, 50, 6, 2, 119, 96, 1, 159, 0, 1,
 219, 178, 255, 95, 7, 1, 58, 47, 175, 0, 1, 115, 93, 255, 104,
 7, 2, 215, 173, 2, 135, 0, 1, 225, 182, 255, 80, 7, 1, 22,
 18, 163, 0, 255, 162, 3, 1, 250, 203, 255, 50, 7, 1, 13, 10,
 193, 0, 252, 0, 10, 173, 141, 1, 255, 207, 2, 255, 207, 2, 69,
 55, 166, 0, 2, 59, 48, 0, 255, 50, 6, 2, 184, 149, 1, 156,
 0, 4, 2, 1, 0, 248, 203, 255, 41, 4, 255, 2, 3, 1, 24,
 19, 175, 0, 1, 40, 32, 255, 104, 7, 255, 2, 3, 128, 3, 133,
 0, 1, 152, 124, 255, 176, 7, 1, 60, 49, 166, 0, 1, 176, 144,
 255, 50, 7, 1, 48, 39, 193, 0, 252, 0, 11, 117, 93, 1, 255,
 207, 2, 255, 207, 2, 107, 86, 1, 165, 0, 2, 27, 22, 0, 255,
 50, 6, 2, 239, 195, 2, 156, 0, 1, 11, 9, 255, 41, 7, 1,
 246, 199, 129, 2, 175, 0, 1, 7, 5, 255, 62, 7, 255, 2, 3,
 1, 25, 20, 133, 0, 1, 88, 73, 255, 20, 7, 1, 122, 100, 255,
 176, 40, 1, 97, 79, 255, 50, 7, 1, 111, 90, 255, 50, 40, 153,
 0, 252, 0, 11, 77, 62, 0, 255, 207, 2, 255, 207, 2, 162, 131,
 1, 165, 0, 1, 8, 7, 255, 50, 7, 255, 2, 3, 1, 5, 4,
 154, 0, 1, 36, 29, 255, 41, 7, 1, 199, 161, 255, 92, 40, 141,
 0, 1, 217, 176, 255, 104, 7, 1, 68, 55, 133, 0, 1, 40, 32,
 255, 83, 7, 1, 189, 154, 255, 83, 40, 1, 43, 34, 255, 50, 49,
 153, 0, 252, 0, 11, 44, 35, 0, 255, 207, 2, 255, 207, 2, 215,
 173, 2, 165, 0, 128, 1, 2, 0, 229, 186, 255, 50, 4, 255, 2,
 3, 1, 22, 17, 154, 0, 1, 70, 56, 255, 92, 7, 2, 144, 117,
 1, 180, 0, 2, 115, 93, 1, 255, 65, 6, 1, 133, 108, 255, 65,
 7, 1, 11, 9, 255, 83, 7, 3, 250, 203, 2, 1, 255, 85, 38,
 1, 10, 8, 255, 50, 7, 1, 248, 201, 129, 2, 190, 0, 252, 0,
 13, 19, 15, 0, 255, 207, 2, 255, 207, 2, 231, 187, 2, 1, 1,
 166, 0, 2, 205, 166, 1, 255, 53, 6, 1, 53, 42, 154, 0, 1,
 103, 82, 255, 38, 7, 1, 79, 65, 181, 0, 1, 51, 41, 255, 158,
 7, 1, 209, 168, 255, 156, 7, 128, 1, 2, 0, 233, 187, 255, 17,
 4, 255, 2, 3, 1, 16, 13, 166, 0, 1, 227, 186, 255, 50, 7,
 1, 21, 17, 190, 0, 252, 0, 13, 5, 4, 0, 255, 207, 2, 255,
 207, 2, 252, 205, 2, 8, 6, 166, 0, 2, 136, 111, 1, 255, 53,
 6, 1, 97, 79, 154, 0, 1, 144, 117, 255, 38, 7, 1, 40, 31,
 181, 0, 1, 15, 12, 255, 158, 7, 255, 2, 3, 128, 3, 133, 0,
 1, 176, 144, 255, 86, 7, 1, 53, 42, 166, 0, 1, 157, 128, 255,
 50, 7, 1, 68, 55, 190, 0, 253, 0, 128, 0, 2, 255, 207, 2,
 255, 2, 6, 1, 23, 19, 166, 0, 1, 85, 69, 255, 53, 7, 2,
 152, 124, 1, 153, 0, 2, 201, 162, 1, 255, 38, 6, 1, 18, 15,
 181, 0, 4, 2, 1, 0, 233, 189, 255, 158, 7, 0, 24, 255, 158,
 8, 1, 105, 86, 255, 86, 7, 1, 109, 88, 255, 125, 28, 138, 0,
 1, 81, 66, 255, 176, 7, 1, 139, 112, 255, 50, 40, 150, 0, 253,
 0, 128, 0, 10, 201, 162, 1, 255, 207, 2, 255, 207, 2, 48, 40,
 166, 0, 2, 45, 36, 0, 255, 50, 6, 2, 223, 180, 2, 153, 0,
 1, 235, 189, 255, 35, 4, 255, 2, 3, 1, 4, 3, 184, 0, 1,
 182, 147, 255, 158, 7, 1, 67, 55, 133, 0, 1, 50, 40, 255, 125,
 7, 2, 187, 152, 1, 165, 0, 1, 29, 24, 255, 50, 7, 1, 219,
 178, 255, 176, 28, 162, 0, 253, 0, 128, 0, 10, 146, 118, 1, 255,
 207, 2, 255, 207, 2, 90, 72, 166, 0, 2, 17, 13, 0, 255, 50,
 6, 255, 2, 3, 1, 2, 1, 148, 0, 4, 3, 2, 0, 252, 205,
 255, 32, 4, 2, 237, 193, 2, 186, 0, 1, 105, 86, 255, 158, 7,
 1, 133, 107, 255, 105, 7, 1, 15, 12, 255, 125, 10, 255, 124, 23,
 142, 0, 1, 4, 3, 255, 140, 7, 255, 2, 3, 1, 7, 5, 187,
 0, 253, 0, 128, 0, 11, 99, 80, 0, 255, 207, 2, 255, 207, 2,
 136, 111, 1, 165, 0, 4, 4, 3, 0, 246, 199, 255, 50, 4, 255,
 2, 3, 1, 16, 13, 148, 0, 1, 10, 8, 255, 86, 7, 1, 197,
 159, 255, 86, 40, 147, 0, 1, 48, 39, 255, 71, 7, 1, 209, 168,
 255, 71, 7, 4, 2, 1, 0, 237, 193, 255, 125, 31, 144, 0, 2,
 199, 162, 1, 255, 50, 6, 1, 39, 31, 187, 0, 253, 0, 128, 0,
 11, 57, 46, 0, 255, 207, 2, 255, 207, 2, 197, 161, 1, 168, 0,
 1, 217, 175, 255, 50, 4, 255, 2, 3, 1, 41, 33, 148, 0, 1,
 21, 17, 255, 86, 7, 1, 152, 124, 255, 86, 43, 144, 0, 1, 14,
 11, 255, 71, 7, 255, 2, 3, 128, 3, 133, 0, 2, 189, 154, 1,
 255, 17, 6, 1, 54, 43, 166, 0, 1, 127, 103, 255, 50, 7, 1,
 100, 81, 187, 0, 253, 0, 128, 0, 13, 28, 22, 0, 255, 207, 2,
 255, 207, 2, 227, 184, 2, 1, 1, 166, 0, 2, 168, 136, 1, 255,
 53, 6, 1, 82, 66, 148, 0, 1, 40, 33, 255, 86, 7, 1, 107,
 87, 255, 84, 41, 146, 0, 4, 2, 1, 0, 231, 187, 255, 71, 4,
 255, 2, 3, 1, 26, 21, 133, 0, 1, 114, 92, 255, 125, 7, 1,
 112, 91, 255, 92, 40, 1, 56, 45, 255, 143, 7, 1, 178, 144, 255,
 143, 61, 253, 0, 128, 0, 13, 10, 8, 0, 255, 207, 2, 255, 207,
 2, 250, 203, 2, 6, 4, 166, 0, 2, 103, 82, 1, 255, 53, 6,
 2, 144, 118, 1, 147, 0, 1, 61, 50, 255, 86, 7, 1, 72, 58,
 190, 0, 1, 182, 147, 255, 107, 7, 255, 74, 9, 1, 54, 43, 255,
 92, 7, 1, 207, 168, 255, 125, 22, 144, 0, 1, 14, 11, 255, 50,
 7, 255, 2, 3, 128, 1, 184, 0, 253, 0, 128, 0, 128, 1, 3,
 0, 255, 207, 2, 255, 2, 6, 1, 22, 18, 166, 0, 1, 57, 46,
 255, 53, 7, 2, 209, 169, 1, 147, 0, 1, 80, 65, 255, 32, 7,
 1, 48, 39, 190, 0, 2, 105, 86, 1, 255, 74, 6, 1, 141, 114,
 255, 107, 7, 1, 18, 14, 255, 179, 10, 0, 2, 255, 127, 22, 255,
 230, 19, 1, 229, 186, 255, 230, 49, 144, 0, 253, 0, 131, 0, 4,
 227, 184, 2, 255, 207, 255, 2, 4, 1, 46, 38, 166, 0, 2, 24,
 19, 0, 255, 50, 6, 255, 2, 3, 0, 1, 146, 0, 2, 111, 91,
 1, 255, 29, 6, 1, 29, 24, 190, 0, 255, 149, 3, 255, 74, 6,
 2, 221, 178, 2, 132, 0, 128, 2, 2, 0, 235, 191, 255, 125, 7,
 1, 22, 18, 166, 0, 1, 168, 136, 255, 146, 7, 1, 68, 55, 184,
 0, 253, 0, 131, 0, 10, 164, 133, 1, 255, 207, 2, 255, 207, 2,
 87, 71, 166, 0, 4, 6, 5, 0, 250, 203, 255, 50, 4, 255, 2,
 3, 1, 13, 10, 145, 0, 1, 151, 122, 255, 83, 7, 1, 16, 13,
 190, 0, 255, 95, 3, 255, 74, 6, 255, 2, 3, 255, 119, 3, 132,
 0, 1, 189, 152, 255, 95, 7, 1, 68, 55, 166, 0, 1, 80, 65,
 255, 71, 7, 2, 154, 124, 1, 183, 0, 253, 0, 131, 0, 11, 112,
 92, 1, 255, 207, 2, 255, 207, 2, 136, 111, 1, 168, 0, 1, 225,
 182, 255, 50, 4, 255, 2, 3, 1, 37, 30, 145, 0, 1, 180, 146,
 255, 83, 7, 1, 6, 5, 190, 0, 128, 1, 2, 0, 227, 184, 255,
 107, 7, 1, 31, 25, 133, 0, 2, 100, 81, 0, 255, 17, 6, 1,
 142, 117, 255, 179, 40, 1, 30, 24, 255, 50, 7, 2, 244, 199, 2,
 183, 0, 253, 0, 131, 0, 11, 68, 55, 0, 255, 207, 2, 255, 207,
 2, 197, 161, 1, 168, 0, 2, 180, 146, 1, 255, 53, 6, 1, 79,
 63, 145, 0, 1, 213, 173, 255, 26, 4, 3, 250, 203, 2, 2, 255,
 82, 43, 151, 0, 1, 175, 142, 255, 107, 7, 1, 85, 69, 133, 0,
 1, 42, 34, 255, 179, 7, 2, 237, 193, 2, 165, 0, 4, 4, 3,
 0, 246, 201, 255, 50, 4, 255, 2, 3, 1, 11, 9, 181, 0, 253,
 0, 131, 0, 13, 33, 27, 0, 255, 207, 2, 255, 207, 2, 227, 184,
 2, 1, 1, 166, 0, 2, 111, 91, 1, 255, 53, 6, 1, 144, 118,
 255, 51, 19, 1, 231, 187, 255, 26, 4, 2, 237, 193, 2, 195, 0,
 1, 93, 75, 255, 161, 7, 1, 162, 131, 255, 107, 7, 4, 10, 8,
 0, 252, 205, 255, 17, 4, 255, 2, 3, 1, 8, 6, 166, 0, 1,
 199, 162, 255, 179, 7, 1, 55, 45, 181, 0, 253, 0, 131, 0, 13,
 14, 11, 0, 255, 207, 2, 255, 207, 2, 250, 203, 2, 7, 5, 166,
 0, 1, 62, 50, 255, 53, 7, 2, 209, 169, 1, 141, 0, 128, 1,
 2, 0, 248, 201, 255, 26, 4, 2, 215, 175, 2, 195, 0, 1, 38,
 31, 255, 107, 7, 1, 237, 191, 255, 80, 10, 1, 225, 184, 255, 17,
 4, 255, 2, 3, 1, 39, 32, 166, 0, 2, 111, 90, 1, 255, 50,
 6, 1, 122, 99, 255, 179, 16, 165, 0, 253, 0, 131, 0, 128, 1,
 3, 0, 255, 207, 2, 255, 2, 6, 1, 23, 19, 166, 0, 1, 27,
 22, 255, 53, 10, 128, 1, 139, 0, 128, 3, 255, 26, 7, 1, 184,
 149, 255, 24, 14, 182, 0, 4, 8, 7, 0, 252, 205, 255, 107, 7,
 1, 12, 9, 133, 0, 2, 152, 124, 1, 255, 17, 6, 1, 101, 82,
 255, 101, 40, 1, 45, 36, 255, 152, 7, 2, 223, 180, 2, 180, 0,
 253, 0, 134, 0, 4, 227, 184, 2, 255, 207, 255, 2, 4, 1, 49,
 40, 166, 0, 4, 6, 5, 0, 250, 203, 255, 53, 7, 1, 15, 12,
 139, 0, 2, 9, 8, 0, 255, 23, 6, 2, 159, 130, 1, 198, 0,
 1, 219, 176, 255, 107, 7, 1, 47, 39, 133, 0, 1, 77, 62, 255,
 101, 7, 1, 191, 156, 255, 101, 40, 255, 152, 3, 255, 179, 6, 4,
 252, 205, 2, 7, 5, 178, 0, 253, 0, 134, 0, 10, 164, 133, 1,
 255, 207, 2, 255, 207, 2, 91, 74, 169, 0, 1, 227, 184, 255, 50,
 4, 255, 2, 3, 1, 43, 35, 139, 0, 2, 17, 14, 0, 255, 23,
 6, 2, 130, 105, 1, 198, 0, 1, 151, 122, 255, 161, 7, 1, 108,
 88, 255, 83, 7, 1, 27, 22, 255, 101, 7, 255, 2, 3, 128, 1,
 166, 0, 1, 219, 178, 255, 50, 7, 1, 41, 33, 178, 0, 253, 0,
 134, 0, 11, 112, 91, 1, 255, 207, 2, 255, 207, 2, 144, 118, 1,
 168, 0, 1, 180, 146, 255, 53, 7, 1, 88, 71, 139, 0, 2, 31,
 24, 0, 255, 23, 6, 1, 103, 84, 199, 0, 1, 74, 60, 255, 83,
 7, 1, 187, 152, 255, 161, 7, 4, 5, 4, 0, 246, 199, 255, 17,
 4, 255, 2, 3, 1, 22, 18, 166, 0, 1, 139, 112, 255, 179, 7,
 1, 108, 88, 255, 233, 43, 135, 0, 253, 0, 134, 0, 11, 62, 51,
 0, 255, 207, 2, 255, 207, 2, 201, 162, 1, 168, 0, 2, 109, 88,
 1, 255, 53, 6, 1, 149, 121, 255, 53, 13, 1, 45, 36, 255, 77,
 7, 1, 80, 65, 199, 0, 1, 27, 22, 255, 83, 7, 255, 2, 3,
 128, 2, 133, 0, 1, 209, 169, 255, 128, 7, 1, 70, 57, 166, 0,
 1, 60, 49, 255, 71, 7, 1, 205, 166, 255, 233, 43, 135, 0, 253,
 0, 134, 0, 13, 31, 25, 0, 255, 207, 2, 255, 207, 2, 235, 191,
 2, 2, 1, 166, 0, 1, 60, 49, 255, 53, 7, 2, 227, 184, 2,
 138, 0, 1, 68, 55, 255, 23, 7, 1, 61, 50, 199, 0, 4, 5,
 4, 0, 246, 199, 255, 83, 4, 255, 2, 3, 1, 25, 19, 133, 0,
 2, 117, 95, 1, 255, 17, 6, 1, 151, 122, 255, 180, 40, 4, 14,
 11, 0, 252, 205, 255, 50, 4, 255, 77, 3, 255, 83, 3, 174, 0,
 253, 0, 134, 0, 5, 10, 8, 0, 255, 207, 2, 255, 2, 6, 255,
 11, 3, 165, 0, 1, 25, 21, 255, 53, 10, 128, 3, 136, 0, 1,
 88, 72, 255, 23, 7, 1, 42, 34, 202, 0, 2, 197, 159, 1, 255,
 86, 6, 1, 73, 59, 133, 0, 1, 48, 39, 255, 104, 7, 2, 244,
 197, 2, 168, 0, 1, 239, 193, 255, 179, 7, 1, 36, 29, 175, 0,
 253, 0, 134, 0, 128, 1, 3, 0, 255, 207, 2, 255, 2, 6, 1,
 32, 26, 166, 0, 4, 5, 4, 0, 248, 201, 255, 53, 7, 1, 21,
 16, 136, 0, 2, 109, 88, 1, 255, 20, 6, 1, 25, 20, 202, 0,
 1, 117, 96, 255, 86, 7, 2, 147, 119, 1, 132, 0, 4, 11, 9,
 0, 252, 205, 255, 128, 7, 255, 11, 3, 165, 0, 1, 151, 122, 255,
 71, 7, 1, 95, 78, 175, 0, 253, 0, 137, 0, 10, 205, 166, 1,
 255, 207, 2, 255, 207, 2, 66, 53, 169, 0, 1, 219, 178, 255, 50,
 4, 255, 2, 3, 1, 55, 45, 136, 0, 1, 146, 118, 255, 74, 7,
 1, 13, 10, 202, 0, 2, 51, 41, 0, 255, 86, 6, 2, 233, 187,
 2, 135, 0, 1, 231, 187, 255, 128, 7, 1, 50, 40, 166, 0, 1,
 71, 57, 255, 71, 7, 2, 197, 159, 1, 174, 0, 253, 0, 137, 0,
 11, 154, 124, 1, 255, 207, 2, 255, 207, 2, 117, 95, 1, 168, 0,
 1, 166, 135, 255, 53, 7, 1, 112, 91, 255, 53, 10, 1, 178, 146,
 255, 20, 7, 1, 5, 4, 202, 0, 2, 13, 10, 0, 255, 86, 6,
 255, 2, 3, 1, 10, 8, 133, 0, 0, 152, 255, 182, 8, 1, 122,
 100, 255, 182, 40, 1, 18, 14, 255, 71, 7, 1, 244, 197, 255, 161,
 49, 253, 0, 137, 0, 11, 99, 80, 0, 255, 207, 2, 255, 207, 2,
 180, 146, 1, 255, 53, 44, 0, 1, 255, 53, 6, 1, 184, 149, 255,
 53, 10, 1, 215, 173, 255, 20, 7, 0, 2, 255, 73, 43, 160, 0,
 128, 1, 2, 0, 227, 184, 255, 86, 4, 255, 2, 3, 1, 48, 39,
 133, 0, 1, 71, 57, 255, 182, 7, 2, 221, 180, 2, 165, 0, 255,
 148, 3, 255, 50, 6, 255, 2, 3, 1, 33, 27, 172, 0, 253, 0,
 137, 0, 12, 55, 45, 0, 255, 207, 2, 255, 207, 2, 221, 178, 2,
 1, 167, 0, 1, 46, 38, 255, 53, 7, 1, 246, 199, 255, 53, 10,
 1, 235, 191, 255, 17, 4, 2, 241, 195, 2, 207, 0, 2, 159, 130,
 1, 255, 110, 6, 1, 112, 91, 255, 109, 7, 1, 21, 17, 255, 128,
 7, 255, 2, 3, 1, 7, 5, 166, 0, 1, 161, 131, 255, 71, 7,
 1, 100, 80, 172, 0, 253, 0, 137, 0, 13, 23, 19, 0, 255, 207,
 2, 255, 207, 2, 250, 203, 2, 6, 5, 166, 0, 1, 16, 13, 255,
 53, 7, 255, 2, 3, 1, 14, 11, 130, 0, 128, 1, 2, 0, 244,
 197, 255, 14, 4, 2, 205, 166, 1, 207, 0, 1, 79, 65, 255, 110,
 7, 1, 201, 162, 255, 92, 7, 4, 2, 1, 0, 239, 193, 255, 128,
 7, 1, 37, 31, 166, 0, 1, 74, 59, 255, 71, 7, 1, 189, 154,
 255, 164, 46, 253, 0, 137, 0, 5, 7, 5, 0, 255, 207, 2, 255,
 2, 6, 1, 22, 17, 166, 0, 4, 3, 2, 0, 241, 195, 255, 53,
 7, 1, 45, 36, 130, 0, 0, 6, 255, 71, 8, 2, 180, 146, 1,
 207, 0, 1, 27, 22, 255, 164, 10, 1, 4, 3, 133, 0, 2, 178,
 146, 1, 255, 17, 6, 1, 108, 88, 255, 113, 40, 1, 23, 18, 255,
 71, 7, 4, 246, 199, 2, 6, 4, 169, 0, 253, 0, 140, 0, 4,
 237, 191, 2, 255, 207, 255, 2, 4, 1, 55, 45, 169, 0, 2, 209,
 169, 1, 255, 53, 6, 1, 99, 80, 130, 0, 2, 13, 11, 0, 255,
 14, 6, 2, 152, 124, 1, 207, 0, 4, 4, 3, 0, 244, 199, 255,
 164, 7, 1, 31, 25, 133, 0, 1, 88, 72, 255, 113, 7, 1, 201,
 164, 255, 113, 40, 128, 1, 1, 0, 246, 255, 71, 8, 1, 36, 29,
 169, 0, 253, 0, 140, 0, 10, 180, 147, 1, 255, 207, 2, 255, 207,
 2, 101, 82, 169, 0, 1, 130, 105, 255, 53, 7, 2, 171, 139, 1,
 129, 0, 2, 23, 18, 0, 255, 14, 6, 1, 118, 96, 255, 14, 4,
 207, 0, 1, 189, 154, 255, 110, 7, 1, 92, 74, 133, 0, 1, 30,
 24, 255, 113, 7, 255, 2, 3, 128, 3, 166, 0, 1, 162, 131, 255,
 71, 7, 1, 108, 88, 255, 167, 43, 253, 0, 140, 0, 11, 119, 97,
 1, 255, 207, 2, 255, 207, 2, 164, 133, 1, 168, 0, 2, 68, 55,
 0, 255, 53, 6, 7, 231, 187, 2, 1, 1, 0, 37, 31, 255, 14,
 7, 1, 95, 77, 211, 0, 1, 108, 88, 255, 164, 7, 1, 173, 141,
 255, 164, 7, 4, 4, 3, 0, 244, 197, 255, 17, 4, 255, 2, 3,
 1, 33, 27, 166, 0, 1, 73, 59, 255, 167, 7, 1, 199, 161, 255,
 236, 43, 253, 0, 140, 0, 11, 71, 57, 0, 255, 207, 2, 255, 207,
 2, 213, 171, 2, 168, 0, 1, 28, 23, 255, 53, 7, 255, 2, 3,
 4, 12, 10, 0, 61, 50, 255, 14, 7, 1, 65, 53, 211, 0, 1,
 44, 35, 255, 95, 7, 4, 250, 203, 2, 1, 1, 133, 0, 2, 199,
 161, 1, 255, 20, 6, 1, 97, 80, 166, 0, 1, 19, 15, 255, 71,
 7, 4, 252, 205, 2, 7, 5, 166, 0, 253, 0, 140, 0, 13, 33,
 27, 0, 255, 207, 2, 255, 207, 2, 246, 199, 2, 4, 3, 166, 0,
 4, 7, 5, 0, 250, 203, 255, 53, 4, 255, 2, 3, 4, 51, 40,
 0, 86, 70, 255, 68, 7, 1, 45, 36, 169, 0, 1, 9, 8, 166,
 0, 1, 8, 7, 255, 110, 10, 1, 23, 19, 133, 0, 2, 101, 82,
 1, 255, 17, 6, 2, 197, 161, 1, 165, 0, 128, 1, 2, 0, 237,
 193, 255, 71, 7, 1, 43, 34, 166, 0, 253, 0, 140, 0, 5, 10,
 8, 0, 255, 207, 2, 255, 2, 6, 1, 20, 16, 169, 0, 1, 213,
 173, 255, 53, 7, 5, 111, 90, 1, 114, 92, 1, 255, 11, 6, 1,
 31, 25, 166, 0, 7, 70, 56, 0, 223, 182, 2, 57, 46, 166, 0,
 1, 213, 171, 255, 110, 7, 1, 77, 62, 133, 0, 1, 37, 30, 255,
 185, 10, 1, 2, 1, 166, 0, 1, 152, 124, 255, 170, 7, 1, 122,
 100, 255, 51, 40, 253, 0, 140, 0, 7, 1, 0, 0, 250, 203, 2,
 255, 207, 255, 2, 4, 1, 50, 40, 169, 0, 2, 151, 122, 1, 255,
 53, 6, 4, 199, 162, 1, 159, 128, 255, 11, 7, 1, 17, 13, 163,
 0, 4, 16, 13, 0, 235, 189, 255, 47, 4, 2, 164, 133, 1, 165,
 0, 1, 133, 108, 255, 98, 7, 1, 154, 125, 255, 50, 7, 4, 5,
 4, 0, 246, 199, 255, 185, 7, 1, 30, 24, 166, 0, 1, 117, 95,
 255, 71, 7, 1, 211, 171, 255, 122, 40, 253, 0, 143, 0, 10, 193,
 156, 1, 255, 207, 2, 255, 207, 2, 95, 77, 169, 0, 2, 78, 63,
 0, 255, 53, 6, 4, 241, 197, 2, 244, 197, 255, 8, 4, 4, 252,
 205, 2, 6, 5, 163, 0, 1, 125, 101, 255, 113, 7, 2, 231, 187,
 2, 165, 0, 1, 55, 45, 255, 110, 8, 2, 195, 2, 1, 134, 0,
 1, 205, 166, 255, 71, 7, 1, 88, 72, 163, 0, 4, 7, 5, 0,
 246, 199, 255, 47, 4, 255, 2, 6, 1, 10, 8, 163, 0, 253, 0,
 143, 0, 11, 135, 108, 1, 255, 207, 2, 255, 207, 2, 164, 133, 1,
 168, 0, 2, 29, 23, 0, 255, 53, 6, 255, 2, 3, 1, 252, 205,
 255, 5, 4, 3, 250, 203, 2, 2, 255, 64, 38, 1, 217, 176, 255,
 56, 10, 1, 12, 10, 163, 0, 1, 13, 11, 255, 110, 10, 1, 18,
 14, 133, 0, 1, 108, 87, 255, 185, 7, 1, 201, 164, 255, 120, 37,
 1, 117, 95, 255, 47, 7, 255, 2, 3, 4, 248, 201, 2, 12, 9,
 163, 0, 253, 0, 143, 0, 11, 82, 67, 0, 255, 207, 2, 255, 207,
 2, 213, 171, 2, 168, 0, 1, 5, 4, 255, 53, 7, 255, 2, 9,
 1, 237, 193, 255, 62, 37, 1, 7, 6, 255, 56, 13, 1, 48, 39,
 163, 0, 128, 1, 2, 0, 223, 180, 255, 50, 7, 1, 66, 53, 133,
 0, 1, 40, 33, 255, 74, 10, 128, 3, 157, 0, 1, 12, 10, 255,
 119, 13, 2, 122, 99, 1, 165, 0, 253, 0, 143, 0, 13, 37, 30,
 0, 255, 207, 2, 255, 207, 2, 246, 199, 2, 4, 3, 169, 0, 2,
 203, 164, 1, 255, 56, 6, 255, 2, 6, 2, 195, 159, 1, 162, 0,
 1, 22, 17, 255, 110, 7, 255, 2, 6, 1, 115, 95, 255, 53, 37,
 129, 0, 1, 147, 119, 255, 110, 7, 1, 144, 117, 255, 50, 7, 4,
 7, 6, 0, 246, 199, 255, 71, 7, 1, 33, 27, 157, 0, 1, 152,
 124, 255, 173, 10, 4, 233, 189, 2, 10, 8, 166, 0, 253, 0, 143,
 0, 5, 13, 10, 0, 255, 207, 2, 255, 2, 6, 1, 21, 17, 169,
 0, 2, 135, 109, 1, 255, 56, 9, 255, 2, 3, 2, 152, 124, 1,
 162, 0, 1, 42, 34, 255, 110, 10, 255, 2, 3, 1, 205, 166, 255,
 53, 37, 129, 0, 1, 67, 55, 255, 56, 7, 2, 237, 191, 2, 135,
 0, 1, 209, 169, 255, 131, 7, 1, 97, 80, 154, 0, 4, 22, 17,
 0, 252, 205, 255, 116, 10, 1, 97, 79, 169, 0, 253, 0, 143, 0,
 128, 1, 3, 0, 255, 207, 2, 255, 2, 6, 1, 55, 45, 169, 0,
 1, 61, 50, 255, 56, 10, 255, 2, 3, 2, 117, 95, 1, 162, 0,
 1, 79, 65, 255, 53, 13, 4, 244, 197, 2, 5, 4, 163, 0, 1,
 17, 14, 255, 56, 10, 1, 15, 12, 133, 0, 2, 101, 82, 1, 255,
 17, 6, 1, 201, 164, 255, 125, 28, 1, 171, 138, 255, 38, 7, 255,
 2, 3, 1, 223, 180, 255, 113, 40, 132, 0, 253, 0, 146, 0, 10,
 197, 159, 1, 255, 207, 2, 255, 207, 2, 101, 82, 169, 0, 4, 17,
 14, 0, 252, 205, 255, 53, 4, 255, 2, 6, 1, 63, 52, 163, 0,
 1, 121, 97, 255, 107, 7, 255, 2, 9, 1, 28, 23, 163, 0, 128,
 1, 2, 0, 229, 186, 255, 50, 7, 1, 66, 53, 133, 0, 2, 37,
 30, 0, 255, 71, 9, 1, 5, 4, 148, 0, 4, 31, 25, 0, 250,
 203, 255, 110, 10, 1, 77, 62, 172, 0, 253, 0, 146, 0, 11, 138,
 111, 1, 255, 207, 2, 255, 207, 2, 171, 139, 1, 168, 0, 128, 1,
 2, 0, 227, 186, 255, 53, 4, 255, 2, 6, 1, 29, 23, 163, 0,
 1, 156, 125, 255, 107, 7, 255, 2, 9, 1, 85, 69, 166, 0, 1,
 156, 127, 255, 59, 7, 1, 144, 117, 255, 167, 7, 4, 5, 4, 0,
 246, 199, 255, 71, 7, 1, 37, 31, 148, 0, 1, 178, 144, 255, 113,
 10, 3, 215, 175, 2, 2, 255, 226, 43, 130, 0, 253, 0, 146, 0,
 12, 78, 62, 0, 255, 207, 2, 255, 207, 2, 221, 178, 2, 1, 170,
 0, 2, 147, 119, 1, 255, 56, 6, 4, 246, 201, 2, 8, 6, 163,
 0, 1, 199, 161, 255, 50, 7, 1, 211, 171, 255, 8, 7, 1, 169,
 138, 255, 115, 40, 1, 71, 57, 255, 167, 7, 2, 237, 191, 2, 135,
 0, 1, 203, 164, 255, 71, 7, 1, 115, 93, 255, 71, 19, 4, 36,
 30, 0, 252, 205, 255, 29, 4, 255, 2, 6, 1, 60, 49, 175, 0,
 253, 0, 146, 0, 13, 36, 29, 0, 255, 207, 2, 255, 207, 2, 250,
 203, 2, 7, 6, 169, 0, 1, 52, 41, 255, 56, 7, 2, 197, 161,
 1, 165, 0, 1, 237, 191, 255, 47, 4, 4, 246, 199, 2, 92, 74,
 255, 59, 7, 3, 235, 191, 2, 1, 255, 61, 38, 1, 17, 14, 255,
 50, 7, 255, 2, 3, 1, 15, 13, 133, 0, 1, 95, 78, 255, 20,
 7, 2, 221, 180, 2, 144, 0, 2, 184, 149, 1, 255, 50, 9, 1,
 207, 168, 255, 164, 40, 138, 0, 253, 0, 146, 0, 5, 10, 8, 0,
 255, 207, 2, 255, 2, 6, 1, 27, 22, 169, 0, 10, 5, 4, 0,
 221, 178, 2, 229, 186, 2, 36, 29, 163, 0, 128, 2, 2, 0, 252,
 205, 255, 101, 4, 255, 53, 3, 1, 22, 18, 255, 116, 7, 255, 17,
 3, 1, 16, 13, 163, 0, 128, 1, 2, 0, 229, 186, 255, 167, 7,
 1, 71, 57, 133, 0, 1, 32, 26, 255, 188, 10, 1, 9, 7, 139,
 0, 1, 41, 33, 255, 110, 7, 255, 101, 6, 1, 47, 39, 178, 0,
 253, 0, 146, 0, 7, 1, 0, 0, 246, 199, 2, 255, 207, 255, 2,
 4, 1, 66, 53, 172, 0, 4, 47, 38, 0, 27, 21, 166, 0, 2,
 8, 7, 0, 255, 101, 6, 7, 176, 142, 1, 1, 0, 0, 248, 201,
 255, 116, 7, 1, 62, 50, 166, 0, 2, 156, 127, 1, 255, 50, 6,
 2, 154, 125, 1, 132, 0, 4, 4, 3, 0, 241, 195, 255, 71, 7,
 1, 53, 43, 139, 0, 1, 184, 149, 255, 44, 7, 255, 2, 3, 1,
 197, 161, 255, 47, 7, 174, 0, 253, 0, 149, 0, 11, 187, 152, 1,
 255, 207, 2, 255, 207, 2, 125, 103, 1, 216, 0, 2, 18, 15, 0,
 255, 101, 6, 1, 139, 114, 255, 101, 4, 1, 180, 146, 255, 116, 7,
 1, 138, 112, 255, 116, 40, 1, 71, 57, 255, 65, 7, 2, 241, 195,
 2, 255, 51, 9, 1, 182, 149, 255, 71, 7, 1, 135, 109, 255, 71,
 10, 1, 43, 34, 255, 41, 7, 255, 2, 6, 1, 40, 33, 181, 0,
 253, 0, 149, 0, 11, 124, 100, 1, 255, 207, 2, 255, 207, 2, 193,
 156, 1, 216, 0, 2, 32, 26, 0, 255, 101, 6, 1, 107, 86, 255,
 101, 4, 1, 92, 74, 255, 14, 7, 2, 223, 182, 2, 165, 0, 1,
 17, 14, 255, 50, 7, 255, 2, 3, 1, 22, 18, 133, 0, 1, 82,
 67, 255, 20, 7, 1, 244, 197, 255, 71, 10, 1, 186, 151, 255, 209,
 7, 255, 2, 3, 1, 195, 157, 255, 212, 58, 253, 0, 149, 0, 10,
 67, 54, 0, 255, 207, 2, 255, 207, 2, 233, 189, 129, 2, 214, 0,
 1, 54, 43, 255, 101, 7, 1, 72, 59, 130, 0, 1, 33, 27, 255,
 14, 7, 4, 250, 203, 2, 11, 9, 163, 0, 128, 1, 2, 0, 223,
 180, 255, 50, 4, 255, 2, 3, 1, 77, 62, 133, 0, 1, 23, 19,
 255, 71, 7, 255, 2, 3, 1, 17, 14, 130, 0, 1, 40, 32, 255,
 17, 10, 255, 2, 3, 255, 104, 3, 183, 0, 253, 0, 149, 0, 5,
 29, 23, 0, 255, 207, 2, 255, 2, 6, 1, 13, 11, 214, 0, 1,
 74, 60, 255, 101, 7, 1, 51, 41, 130, 0, 1, 5, 4, 255, 116,
 10, 1, 45, 37, 166, 0, 2, 147, 119, 1, 255, 50, 6, 2, 169,
 138, 1, 132, 0, 128, 1, 2, 0, 235, 191, 255, 71, 7, 1, 73,
 59, 130, 0, 1, 176, 142, 255, 35, 7, 255, 2, 3, 1, 184, 149,
 255, 38, 7, 180, 0, 253, 0, 149, 0, 5, 7, 5, 0, 255, 207,
 2, 255, 2, 6, 1, 43, 34, 214, 0, 1, 97, 79, 255, 101, 7,
 1, 31, 25, 133, 0, 1, 217, 176, 255, 116, 7, 2, 122, 99, 1,
 165, 0, 1, 62, 51, 255, 68, 7, 3, 250, 203, 2, 2, 255, 52,
 8, 2, 156, 125, 1, 255, 20, 6, 4, 173, 141, 1, 40, 32, 255,
 203, 10, 255, 2, 3, 1, 33, 27, 187, 0, 253, 0, 152, 0, 4,
 233, 189, 2, 255, 207, 255, 2, 4, 1, 91, 74, 214, 0, 2, 127,
 103, 1, 255, 98, 6, 1, 18, 14, 133, 0, 1, 117, 93, 255, 17,
 7, 2, 211, 171, 1, 165, 0, 2, 14, 11, 0, 255, 50, 6, 255,
 2, 3, 1, 30, 24, 133, 0, 1, 65, 52, 255, 20, 10, 1, 199,
 162, 255, 83, 7, 255, 2, 3, 1, 176, 144, 255, 86, 40, 150, 0,
 253, 0, 152, 0, 11, 162, 131, 1, 255, 207, 2, 255, 207, 2, 162,
 131, 1, 255, 98, 96, 1, 10, 8, 133, 0, 2, 48, 39, 0, 255,
 17, 6, 4, 248, 201, 2, 7, 6, 166, 0, 1, 217, 175, 255, 50,
 4, 255, 2, 3, 1, 99, 80, 133, 0, 4, 13, 10, 0, 252, 205,
 255, 20, 7, 255, 2, 12, 1, 35, 29, 190, 0, 253, 0, 152, 0,
 11, 99, 80, 0, 255, 207, 2, 255, 207, 2, 217, 175, 2, 213, 0,
 2, 197, 161, 1, 255, 98, 6, 0, 3, 255, 97, 8, 1, 9, 7,
 255, 116, 7, 255, 2, 3, 1, 40, 33, 166, 0, 1, 131, 107, 255,
 71, 7, 2, 191, 156, 1, 135, 0, 1, 221, 180, 255, 71, 7, 255,
 2, 9, 1, 186, 151, 255, 29, 10, 183, 0, 253, 0, 152, 0, 13,
 49, 40, 0, 255, 207, 2, 255, 207, 2, 250, 203, 2, 6, 5, 211,
 0, 1, 223, 182, 255, 95, 4, 2, 246, 201, 2, 138, 0, 1, 239,
 193, 255, 20, 4, 255, 2, 3, 2, 107, 87, 1, 165, 0, 0, 50,
 255, 170, 8, 255, 2, 3, 1, 5, 4, 133, 0, 2, 119, 97, 1,
 255, 20, 9, 255, 2, 6, 1, 36, 29, 193, 0, 253, 0, 152, 0,
 5, 17, 13, 0, 255, 207, 2, 255, 2, 6, 1, 29, 23, 211, 0,
 1, 246, 201, 255, 92, 4, 2, 225, 182, 2, 138, 0, 2, 136, 109,
 1, 255, 116, 6, 2, 205, 166, 1, 165, 0, 4, 8, 7, 0, 250,
 203, 255, 170, 7, 1, 45, 37, 133, 0, 1, 41, 33, 255, 191, 10,
 255, 2, 3, 2, 180, 147, 1, 255, 78, 40, 155, 0, 253, 0, 152,
 0, 128, 1, 3, 0, 255, 207, 2, 255, 2, 6, 1, 71, 57, 208,
 0, 4, 3, 2, 0, 252, 205, 255, 92, 4, 2, 199, 161, 1, 138,
 0, 1, 55, 45, 255, 119, 7, 4, 246, 199, 2, 5, 4, 166, 0,
 255, 68, 3, 255, 53, 6, 1, 127, 103, 255, 77, 7, 255, 59, 3,
 1, 244, 197, 255, 191, 7, 255, 2, 3, 1, 45, 37, 196, 0, 253,
 0, 155, 0, 11, 197, 159, 1, 255, 207, 2, 255, 207, 2, 138, 112,
 1, 207, 0, 2, 8, 6, 0, 255, 92, 6, 1, 173, 139, 255, 92,
 13, 1, 13, 10, 255, 23, 7, 255, 2, 3, 1, 37, 30, 166, 0,
 1, 107, 87, 255, 170, 7, 2, 225, 182, 2, 135, 0, 1, 187, 152,
 255, 20, 7, 1, 193, 156, 129, 1, 196, 0, 253, 0, 155, 0, 11,
 127, 103, 1, 255, 207, 2, 255, 207, 2, 205, 166, 1, 207, 0, 2,
 15, 12, 0, 255, 92, 6, 1, 133, 108, 255, 92, 16, 1, 244, 197,
 255, 23, 4, 255, 2, 3, 1, 101, 82, 255, 119, 40, 1, 33, 27,
 255, 77, 7, 255, 2, 3, 255, 89, 3, 132, 0, 1, 81, 66, 255,
 20, 7, 1, 55, 45, 199, 0, 253, 0, 155, 0, 13, 69, 56, 0,
 255, 207, 2, 255, 207, 2, 244, 199, 2, 5, 4, 205, 0, 1, 27,
 22, 255, 92, 7, 2, 104, 85, 1, 141, 0, 2, 147, 119, 1, 255,
 26, 6, 1, 205, 166, 255, 26, 16, 150, 0, 4, 4, 3, 0, 239,
 195, 255, 50, 4, 255, 2, 3, 1, 71, 57, 133, 0, 1, 19, 15,
 255, 98, 4, 3, 215, 173, 2, 2, 255, 70, 40, 160, 0, 253, 0,
 155, 0, 5, 29, 23, 0, 255, 207, 2, 255, 2, 6, 1, 25, 20,
 205, 0, 1, 40, 33, 255, 92, 7, 1, 74, 60, 142, 0, 1, 60,
 49, 255, 26, 7, 4, 246, 199, 2, 6, 5, 166, 0, 2, 175, 142,
 1, 255, 53, 6, 2, 166, 135, 1, 135, 0, 4, 231, 186, 2, 72,
 58, 202, 0, 253, 0, 155, 0, 5, 5, 4, 0, 255, 207, 2, 255,
 2, 6, 1, 66, 54, 205, 0, 1, 55, 45, 255, 92, 7, 1, 57,
 46, 142, 0, 1, 15, 12, 255, 119, 10, 1, 40, 33, 166, 0, 1,
 77, 62, 255, 53, 7, 1, 250, 203, 129, 2, 133, 0, 3, 109, 88,
 1, 3, 255, 11, 8, 195, 0, 253, 0, 158, 0, 4, 225, 182, 2,
 255, 207, 255, 2, 4, 2, 131, 107, 1, 204, 0, 2, 80, 65, 0,
 255, 89, 6, 1, 42, 34, 145, 0, 1, 244, 197, 255, 119, 7, 1,
 109, 90, 255, 119, 40, 1, 18, 14, 255, 80, 7, 255, 2, 3, 1,
 34, 28, 133, 0, 1, 5, 4, 205, 0, 253, 0, 158, 0, 11, 149,
 121, 1, 255, 207, 2, 255, 207, 2, 207, 168, 1, 204, 0, 1, 104,
 84, 255, 89, 7, 1, 27, 22, 145, 0, 1, 147, 119, 255, 29, 7,
 1, 209, 169, 255, 119, 40, 128, 1, 2, 0, 221, 178, 255, 50, 4,
 255, 2, 3, 1, 112, 91, 255, 173, 79, 135, 0, 253, 0, 158, 0,
 13, 86, 70, 0, 255, 207, 2, 255, 207, 2, 241, 195, 2, 4, 3,
 202, 0, 2, 127, 104, 1, 255, 89, 6, 1, 16, 13, 145, 0, 1,
 60, 49, 255, 119, 7, 4, 246, 199, 2, 6, 5, 166, 0, 1, 152,
 124, 255, 83, 7, 2, 217, 175, 2, 213, 0, 253, 0, 158, 0, 5,
 36, 29, 0, 255, 207, 2, 255, 2, 6, 1, 22, 18, 202, 0, 2,
 156, 127, 1, 255, 86, 4, 3, 205, 2, 7, 6, 145, 0, 1, 15,
 12, 255, 119, 10, 1, 40, 33, 163, 0, 4, 1, 0, 0, 215, 175,
 255, 50, 7, 255, 2, 3, 1, 12, 10, 211, 0, 253, 0, 158, 0,
 5, 7, 5, 0, 255, 207, 2, 255, 2, 6, 1, 62, 51, 202, 0,
 10, 186, 151, 1, 255, 207, 2, 252, 205, 2, 3, 2, 148, 0, 1,
 239, 193, 255, 119, 7, 2, 117, 93, 1, 162, 0, 1, 48, 39, 255,
 170, 10, 4, 248, 201, 2, 14, 11, 211, 0, 253, 0, 161, 0, 4,
 235, 191, 2, 255, 207, 255, 2, 4, 2, 130, 105, 1, 201, 0, 8,
 211, 171, 1, 255, 207, 2, 248, 201, 2, 255, 84, 24, 1, 136, 109,
 255, 32, 4, 255, 2, 3, 255, 41, 3, 162, 0, 1, 186, 152, 255,
 47, 7, 255, 2, 3, 1, 154, 125, 255, 170, 76, 138, 0, 253, 0,
 161, 0, 11, 156, 127, 1, 255, 207, 2, 255, 207, 2, 211, 171, 1,
 201, 0, 1, 235, 191, 255, 83, 4, 2, 235, 189, 2, 150, 0, 2,
 55, 45, 0, 255, 119, 6, 4, 250, 203, 2, 10, 8, 157, 0, 1,
 29, 23, 255, 44, 7, 255, 47, 6, 1, 28, 22, 214, 0, 253, 0,
 161, 0, 13, 86, 70, 0, 255, 207, 2, 255, 207, 2, 244, 199, 2,
 5, 4, 196, 0, 128, 1, 2, 0, 248, 201, 255, 83, 4, 2, 205,
 166, 1, 150, 0, 1, 10, 9, 255, 119, 7, 255, 2, 3, 1, 54,
 44, 157, 0, 2, 154, 125, 1, 255, 44, 9, 1, 176, 142, 255, 83,
 25, 192, 0, 253, 0, 161, 0, 5, 36, 29, 0, 255, 207, 2, 255,
 2, 6, 1, 27, 22, 196, 0, 4, 4, 3, 0, 252, 205, 255, 80,
 4, 2, 175, 141, 1, 153, 0, 1, 233, 189, 255, 119, 7, 1, 139,
 114, 255, 38, 28, 1, 15, 12, 255, 161, 10, 255, 2, 3, 1, 41,
 33, 217, 0, 253, 0, 161, 0, 5, 8, 7, 0, 255, 207, 2, 255,
 2, 6, 1, 72, 58, 196, 0, 1, 8, 6, 255, 83, 7, 2, 146,
 118, 1, 153, 0, 2, 122, 100, 1, 255, 38, 6, 2, 225, 184, 2,
 153, 0, 1, 115, 93, 255, 38, 7, 255, 2, 3, 2, 199, 161, 1,
 255, 81, 28, 191, 0, 253, 0, 164, 0, 4, 235, 191, 2, 255, 207,
 255, 2, 4, 2, 146, 118, 1, 195, 0, 2, 18, 15, 0, 255, 80,
 6, 1, 125, 103, 255, 80, 28, 1, 48, 39, 255, 38, 7, 4, 252,
 205, 2, 18, 15, 148, 0, 4, 5, 4, 0, 246, 199, 255, 158, 7,
 255, 2, 3, 1, 62, 51, 220, 0, 253, 0, 164, 0, 11, 156, 127,
 1, 255, 207, 2, 255, 207, 2, 219, 176, 2, 195, 0, 2, 27, 22,
 0, 255, 80, 6, 1, 99, 80, 154, 0, 1, 7, 6, 255, 38, 7,
 255, 2, 3, 1, 75, 60, 148, 0, 1, 80, 65, 255, 35, 10, 3,
 221, 178, 2, 3, 255, 160, 70, 151, 0, 253, 0, 164, 0, 13, 86,
 70, 0, 255, 207, 2, 255, 207, 2, 246, 201, 2, 7, 5, 193, 0,
 1, 39, 31, 255, 80, 7, 1, 74, 59, 157, 0, 2, 209, 171, 1,
 255, 41, 6, 2, 173, 139, 1, 144, 0, 128, 1, 2, 0, 221, 180,
 255, 29, 4, 255, 2, 6, 1, 92, 74, 223, 0, 253, 0, 164, 0,
 5, 33, 27, 0, 255, 207, 2, 255, 2, 6, 1, 34, 28, 193, 0,
 1, 54, 44, 255, 80, 7, 255, 8, 3, 156, 0, 1, 101, 82, 255,
 41, 7, 4, 237, 193, 2, 3, 3, 142, 0, 1, 48, 39, 255, 152,
 10, 4, 235, 191, 2, 8, 6, 223, 0, 253, 0, 164, 0, 5, 5,
 4, 0, 255, 207, 2, 255, 2, 6, 1, 91, 73, 193, 0, 1, 78,
 63, 255, 80, 7, 1, 40, 33, 157, 0, 1, 32, 26, 255, 122, 10,
 1, 34, 27, 142, 0, 2, 180, 146, 1, 255, 29, 9, 2, 128, 104,
 1, 225, 0, 253, 0, 167, 0, 4, 229, 186, 2, 255, 207, 255, 2,
 4, 2, 176, 142, 1, 192, 0, 2, 99, 80, 0, 255, 77, 6, 1,
 30, 24, 157, 0, 1, 2, 1, 255, 41, 7, 255, 2, 3, 1, 107,
 87, 255, 122, 13, 1, 21, 17, 255, 26, 10, 4, 250, 203, 2, 20,
 16, 226, 0, 253, 0, 167, 0, 13, 138, 112, 1, 255, 207, 2, 255,
 207, 2, 235, 189, 2, 1, 1, 190, 0, 1, 119, 97, 255, 77, 7,
 1, 19, 15, 160, 0, 1, 169, 138, 255, 44, 7, 1, 205, 168, 255,
 120, 13, 1, 128, 104, 255, 23, 7, 255, 2, 3, 1, 166, 135, 255,
 147, 65, 164, 0, 253, 0, 167, 0, 13, 69, 55, 0, 255, 207, 2,
 255, 207, 2, 252, 205, 2, 16, 13, 190, 0, 2, 142, 115, 1, 255,
 77, 6, 1, 9, 7, 160, 0, 1, 74, 60, 255, 122, 7, 4, 250,
 203, 2, 11, 9, 133, 0, 4, 6, 5, 0, 246, 201, 255, 20, 4,
 255, 2, 6, 1, 40, 33, 229, 0, 253, 0, 167, 0, 5, 25, 20,
 0, 255, 207, 2, 255, 2, 6, 1, 61, 50, 190, 0, 10, 182, 147,
 1, 255, 207, 2, 252, 205, 2, 3, 3, 160, 0, 1, 17, 13, 255,
 122, 10, 1, 59, 48, 133, 0, 1, 81, 66, 255, 20, 10, 1, 207,
 168, 129, 1, 229, 0, 253, 0, 167, 0, 5, 3, 2, 0, 255, 207,
 2, 255, 2, 6, 2, 135, 109, 1, 189, 0, 1, 211, 171, 255, 71,
 4, 3, 250, 203, 2, 1, 255, 73, 38, 1, 239, 193, 255, 122, 7,
 1, 157, 127, 255, 48, 4, 128, 1, 2, 0, 215, 175, 255, 17, 7,
 255, 2, 3, 1, 73, 59, 232, 0, 253, 0, 170, 0, 11, 205, 166,
 1, 255, 207, 2, 255, 207, 2, 231, 187, 2, 189, 0, 1, 239, 193,
 255, 71, 4, 255, 5, 3, 165, 0, 1, 127, 103, 255, 122, 7, 1,
 233, 189, 129, 2, 3, 0, 39, 31, 0, 255, 14, 6, 255, 140, 6,
 1, 6, 5, 232, 0, 253, 0, 170, 0, 5, 118, 96, 1, 255, 207,
 2, 255, 2, 6, 1, 8, 6, 184, 0, 128, 1, 2, 0, 250, 203,
 255, 68, 4, 2, 211, 171, 1, 165, 0, 2, 44, 36, 0, 255, 122,
 9, 4, 35, 29, 0, 159, 128, 255, 137, 10, 1, 124, 100, 255, 68,
 40, 195, 0, 253, 0, 170, 0, 5, 49, 40, 0, 255, 207, 2, 255,
 2, 6, 1, 43, 35, 184, 0, 1, 8, 6, 255, 71, 7, 2, 168,
 136, 1, 165, 0, 1, 5, 4, 255, 122, 10, 2, 169, 138, 1, 255,
 11, 9, 4, 250, 203, 2, 20, 16, 235, 0, 253, 0, 170, 0, 7,
 12, 10, 0, 252, 205, 2, 255, 207, 255, 2, 4, 2, 108, 88, 1,
 183, 0, 2, 18, 14, 0, 255, 68, 6, 1, 146, 118, 255, 68, 43,
 2, 191, 156, 1, 255, 53, 6, 255, 2, 12, 1, 176, 142, 255, 134,
 58, 180, 0, 253, 0, 173, 0, 4, 239, 195, 2, 255, 207, 255, 2,
 4, 2, 199, 161, 1, 183, 0, 2, 33, 27, 0, 255, 68, 6, 1,
 111, 88, 255, 68, 43, 1, 86, 70, 255, 53, 7, 255, 2, 12, 1,
 48, 39, 238, 0, 253, 0, 173, 0, 5, 151, 122, 1, 255, 207, 2,
 255, 2, 6, 1, 3, 2, 181, 0, 2, 71, 57, 0, 255, 65, 6,
 1, 74, 60, 169, 0, 1, 21, 16, 255, 53, 7, 255, 2, 9, 1,
 221, 178, 255, 131, 58, 183, 0, 253, 0, 173, 0, 5, 70, 56, 0,
 255, 207, 2, 255, 2, 6, 1, 31, 25, 181, 0, 2, 104, 85, 1,
 255, 65, 6, 1, 51, 41, 172, 0, 1, 239, 193, 255, 122, 7, 255,
 2, 6, 1, 96, 79, 241, 0, 253, 0, 173, 0, 5, 22, 18, 0,
 255, 207, 2, 255, 2, 6, 1, 96, 78, 181, 0, 2, 187, 152, 1,
 255, 65, 6, 1, 31, 25, 172, 0, 1, 127, 103, 255, 56, 7, 255,
 2, 3, 4, 246, 201, 2, 13, 10, 241, 0, 253, 0, 173, 0, 128,
 2, 5, 0, 237, 191, 2, 255, 207, 255, 2, 4, 2, 176, 144, 1,
 180, 0, 1, 225, 184, 255, 65, 7, 1, 12, 9, 172, 0, 2, 45,
 36, 0, 255, 56, 6, 255, 2, 3, 1, 159, 130, 255, 125, 55, 189,
 0, 253, 0, 176, 0, 13, 182, 147, 1, 255, 207, 2, 255, 207, 2,
 248, 201, 2, 4, 3, 175, 0, 2, 12, 10, 0, 255, 62, 6, 255,
 2, 3, 1, 3, 2, 172, 0, 1, 5, 4, 255, 59, 10, 1, 41,
 33, 244, 0, 253, 0, 176, 0, 5, 90, 73, 0, 255, 207, 2, 255,
 2, 6, 1, 34, 27, 175, 0, 1, 55, 44, 255, 62, 7, 2, 221,
 178, 2, 177, 0, 10, 180, 146, 1, 255, 207, 2, 213, 173, 1, 2,
 1, 244, 0, 253, 0, 176, 0, 5, 33, 27, 0, 255, 207, 2, 255,
 2, 6, 1, 103, 84, 175, 0, 2, 125, 101, 1, 255, 59, 6, 2,
 169, 138, 1, 177, 0, 1, 75, 60, 255, 125, 4, 1, 95, 77, 247,
 0, 253, 0, 176, 0, 7, 5, 4, 0, 239, 195, 2, 255, 207, 255,
 2, 4, 2, 199, 162, 1, 174, 0, 1, 211, 171, 255, 59, 7, 1,
 100, 80, 178, 0, 7, 14, 12, 0, 248, 203, 2, 13, 10, 247, 0,
 253, 0, 179, 0, 13, 197, 161, 1, 255, 207, 2, 255, 207, 2, 246,
 199, 2, 9, 7, 169, 0, 2, 13, 10, 0, 255, 56, 6, 255, 2,
 3, 1, 52, 42, 181, 0, 2, 133, 108, 1, 249, 0, 253, 0, 179,
 0, 5, 105, 86, 1, 255, 207, 2, 255, 2, 6, 1, 51, 41, 169,
 0, 2, 73, 59, 0, 255, 56, 9, 1, 11, 9, 181, 0, 1, 7,
 6, 250, 0, 253, 0, 179, 0, 5, 37, 31, 0, 255, 207, 2, 255,
 2, 6, 2, 147, 119, 1, 168, 0, 2, 195, 157, 1, 255, 53, 6,
 3, 231, 187, 2, 1, 255, 55, 43, 253, 0, 138, 0, 253, 0, 179,
 0, 7, 5, 4, 0, 246, 199, 2, 255, 207, 255, 2, 4, 4, 241,
 197, 2, 3, 2, 163, 0, 2, 17, 14, 0, 255, 50, 6, 255, 2,
 3, 2, 175, 142, 1, 253, 0, 182, 0, 5, 184, 149, 1, 255, 207,
 2, 255, 2, 6, 1, 36, 30, 163, 0, 1, 112, 91, 255, 50, 10,
 1, 69, 55, 253, 0, 183, 0, 253, 0, 182, 0, 5, 96, 78, 0,
 255, 207, 2, 255, 2, 6, 2, 131, 107, 1, 159, 0, 4, 5, 4,
 0, 225, 182, 255, 47, 7, 255, 2, 3, 1, 18, 14, 253, 0, 183,
 0, 253, 0, 182, 0, 5, 29, 23, 0, 255, 207, 2, 255, 2, 6,
 4, 246, 199, 2, 5, 4, 157, 0, 1, 99, 80, 255, 47, 10, 4,
 221, 178, 2, 1, 1, 253, 0, 183, 0, 253, 0, 182, 0, 128, 1,
 3, 0, 255, 207, 2, 255, 2, 9, 1, 74, 60, 154, 0, 4, 15,
 12, 0, 231, 187, 255, 44, 10, 2, 141, 114, 1, 253, 0, 185, 0,
 5, 159, 128, 1, 255, 207, 2, 255, 2, 6, 4, 215, 175, 2, 7,
 5, 148, 0, 128, 1, 2, 0, 168, 136, 255, 41, 10, 255, 2, 3,
 1, 45, 36, 253, 0, 186, 0, 253, 0, 185, 0, 5, 59, 49, 0,
 255, 207, 2, 255, 2, 9, 4, 125, 103, 1, 2, 2, 142, 0, 7,
 1, 0, 0, 104, 85, 1, 252, 205, 255, 38, 10, 4, 235, 189, 2,
 5, 4, 253, 0, 186, 0, 253, 0, 185, 0, 7, 9, 7, 0, 246,
 201, 2, 255, 207, 255, 2, 7, 7, 250, 203, 2, 131, 107, 1, 17,
 13, 136, 0, 6, 8, 6, 0, 122, 99, 1, 248, 255, 35, 11, 255,
 2, 3, 2, 154, 125, 1, 253, 0, 188, 0, 5, 178, 144, 1, 255,
 207, 2, 255, 2, 9, 19, 250, 203, 2, 215, 173, 2, 133, 107, 1,
 80, 66, 0, 95, 77, 0, 201, 162, 1, 250, 203, 255, 29, 10, 255,
 2, 6, 1, 39, 31, 253, 0, 189, 0, 253, 0, 188, 0, 5, 74,
 60, 0, 255, 207, 2, 255, 2, 15, 4, 252, 205, 2, 248, 201, 255,
 5, 4, 255, 26, 18, 4, 211, 169, 2, 1, 1, 253, 0, 189, 0,
 253, 0, 188, 0, 7, 12, 9, 0, 241, 195, 2, 255, 207, 255, 2,
 40, 1, 81, 67, 253, 0, 192, 0, 253, 0, 191, 0, 5, 171, 138,
 1, 255, 207, 2, 255, 2, 36, 4, 221, 178, 2, 5, 4, 253, 0,
 192, 0, 253, 0, 191, 0, 5, 51, 41, 0, 255, 207, 2, 255, 2,
 36, 1, 84, 68, 253, 0, 195, 0, 253, 0, 191, 0, 8, 3, 2,
 0, 205, 166, 1, 255, 207, 2, 255, 2, 30, 255, 35, 3, 255, 41,
 3, 253, 0, 194, 0, 5, 72, 58, 0, 255, 207, 2, 255, 2, 27,
 4, 244, 199, 2, 37, 31, 253, 0, 198, 0, 253, 0, 194, 0, 8,
 2, 1, 0, 193, 156, 1, 255, 207, 2, 255, 2, 24, 2, 101, 82,
 1, 253, 0, 200, 0, 253, 0, 197, 0, 7, 20, 16, 0, 213, 171,
 2, 255, 207, 255, 2, 16, 4, 252, 205, 2, 121, 99, 129, 1, 253,
 0, 201, 0, 253, 0, 200, 0, 10, 19, 16, 0, 159, 128, 1, 244,
 197, 2, 255, 207, 255, 2, 7, 7, 223, 182, 2, 93, 75, 0, 2,
 1, 253, 0, 204, 0, 253, 0, 203, 0, 16, 3, 2, 0, 43, 34,
 0, 107, 87, 1, 138, 112, 1, 87, 71, 0, 24, 19, 253, 0, 210,
 0, 253, 0, 253, 0, 253, 0, 177, 0
};
const uint16_t imageoffsets[] = {
 0, 11, 28, 46, 66, 87, 113, 137, 158, 181,
 205, 229, 253, 286, 325, 363, 403, 442, 485, 523,
 566, 606, 645, 687, 730, 769, 814, 855, 894, 936,
 980, 1021, 1063, 1104, 1148, 1189, 1229, 1269, 1309, 1350,
 1389, 1432, 1473, 1513, 1553, 1591, 1633, 1671, 1710, 1747,
 1783, 1818, 1857, 1897, 1939, 1981, 2020, 2058, 2097, 2136,
 2177, 2218, 2255, 2291, 2327, 2374, 2414, 2459, 2506, 2552,
 2598, 2653, 2697, 2750, 2794, 2850, 2909, 2978, 3057, 3141,
 3231, 3327, 3425, 3509, 3612, 3705, 3790, 3877, 3976, 4067,
 4159, 4257, 4372, 4484, 4596, 4701, 4808, 4917, 5026, 5132,
 5231, 5330, 5427, 5511, 5585, 5660, 5735, 5815, 5891, 5973,
 6048, 6128, 6206, 6279, 6358, 6432, 6507, 6571, 6637, 6693,
 6754, 6814, 6869, 6933, 6993, 7056, 7106, 7162, 7224, 7280,
 7345, 7400, 7458, 7520, 7576, 7642, 7697, 7756, 7817, 7869,
 7933, 7986, 8043, 8100, 8162, 8219, 8277, 8332, 8390, 8451,
 8510, 8569, 8627, 8684, 8749, 8821, 8897, 8983, 9063, 9146,
 9228, 9317, 9406, 9484, 9555, 9627, 9702, 9786, 9885, 9978,
 10082, 10205, 10310, 10398, 10481, 10568, 10655, 10764, 10879, 10975,
 11076, 11184, 11280, 11376, 11485, 11591, 11686, 11780, 11867, 11948,
 12038, 12127, 12214, 12296, 12382, 12464, 12543, 12641, 12727, 12817,
 12905, 12995, 13081, 13160, 13241, 13328, 13403, 13483, 13569, 13650,
 13738, 13821, 13901, 13981, 14067, 14163, 14261, 14367, 14473, 14577,
 14680, 14786, 14887, 15008, 15127, 15241, 15355, 15475, 15602, 15705,
 15805, 15907, 16005, 16095, 16201, 16308, 16405, 16488, 16571, 16639,
 16716, 16789, 16858, 16933, 17012, 17080, 17148, 17220, 17292, 17352,
 17416, 17483, 17548, 17618, 17675, 17734, 17798, 17862, 17930, 18006,
 18078, 18153, 18229, 18305, 18379, 18449, 18532, 18614, 18702, 18791,
 18882, 18977, 19073, 19170, 19253, 19344, 19449, 19549, 19656, 19749,
 19850, 19938, 20026, 20111, 20205, 20304, 20392, 20480, 20558, 20638,
 20709, 20776, 20845, 20915, 20991, 21071, 21151, 21234, 21316, 21400,
 21490, 21574, 21657, 21735, 21814, 21896, 21983, 22073, 22162, 22254,
 22346, 22441, 22536, 22629, 22725, 22825, 22920, 23017, 23113, 23211,
 23308, 23395, 23484, 23566, 23644, 23725, 23813, 23889, 23968, 24045,
 24116, 24185, 24254, 24322, 24388, 24449, 24508, 24569, 24628, 24683,
 24737, 24790, 24838, 24886, 24931, 24970, 25024, 25081, 25141, 25202,
 25260, 25314, 25368, 25422, 25475, 25530, 25579, 25631, 25693, 25764,
 25839, 25913, 25987, 26059, 26134, 26208, 26286, 26361, 26439, 26519,
 26592, 26668, 26742, 26821, 26903, 26983, 27063, 27149, 27235, 27322,
 27434, 27525, 27603, 27681, 27755, 27828, 27903, 27997, 28116, 28182,
 28246, 28313, 28380, 28450, 28517, 28587, 28652, 28718, 28788, 28859,
 28929, 28999, 29069, 29140, 29207, 29274, 29345, 29418, 29488, 29548,
 29611, 29670, 29725, 29772, 29824, 29877, 29928, 29976, 30023, 30077,
 30129, 30182, 30232, 30289, 30342, 30394, 30441, 30484, 30525, 30574,
 30624, 30675, 30724, 30774, 30823, 30872, 30918, 30964, 31013, 31073,
 31148, 31233, 31323, 31409, 31493, 31576, 31659, 31748, 31833, 31919,
 32009, 32106, 32209, 32308, 32411, 32510, 32606, 32698, 32792, 32880,
 32963, 33040, 33121, 33195, 33260, 33330, 33404, 33475, 33547, 33622,
 33693, 33764, 33832, 33893, 33950, 34002, 34058, 34113, 34167, 34228,
 34292, 34354, 34417, 34480, 34545, 34601, 34661, 34725, 34783, 34843,
 34902, 34962, 35027, 35089, 35150, 35209, 35266, 35323, 35380, 35437,
 35496, 35556, 35628, 35699, 35771, 35843, 35918, 35995, 36071, 36147,
 36226, 36301, 36372, 36445, 36516, 36599, 36682, 36779, 36869, 36960,
 37056, 37157, 37253, 37352, 37444, 37547, 37651, 37769, 37887, 38007,
 38124, 38236, 38345, 38454, 38561, 38683, 38791, 38897, 39014, 39127,
 39241, 39363, 39477, 39594, 39719, 39830, 39944, 40053, 40159, 40258,
 40361, 40451, 40537, 40629, 40720, 40806, 40894, 40978, 41068, 41158,
 41249, 41338, 41414, 41486, 41558, 41630, 41703, 41771, 41847, 41923,
 41991, 42058, 42122, 42178, 42237, 42302, 42368, 42435, 42502, 42576,
 42652, 42726, 42798, 42879, 42957, 43035, 43114, 43192, 43273, 43353,
 43433, 43518, 43601, 43690, 43794, 43891, 43998, 44101, 44209, 44325,
 44434, 44541, 44651, 44760, 44868, 44981, 45098, 45216, 45331, 45450,
 45562, 45670, 45793, 45922, 46050, 46154, 46245, 46339, 46437, 46531,
 46623, 46710, 46792, 46875, 46949, 47010, 47065, 47121, 47174, 47229,
 47286, 47340, 47394, 47453, 47505, 47568, 47623, 47676, 47732, 47792,
 47852, 47919, 47983, 48062, 48157, 48258, 48348, 48445, 48539, 48631,
 48731, 48823, 48924, 49041, 49148, 49254, 49371, 49481, 49579, 49666,
 49747, 49831, 49909, 49992, 50077, 50160, 50237, 50318, 50401, 50477,
 50563, 50645, 50722, 50804, 50886, 50971, 51056, 51139, 51227, 51308,
 51385, 51466, 51549, 51632, 51719, 51805, 51894, 51975, 52057, 52138,
 52223, 52304, 52395, 52470, 52552, 52631, 52711, 52798, 52880, 52968,
 53050, 53132, 53213, 53297, 53379, 53469, 53554, 53649, 53743, 53837,
 53918, 54012, 54101, 54190, 54279, 54371, 54465, 54562, 54660, 54757,
 54840, 54925, 55015, 55100, 55180, 55260, 55331, 55405, 55481, 55557,
 55634, 55705, 55777, 55858, 55923, 55987, 56049, 56111, 56170, 56231,
 56290, 56353, 56413, 56478, 56538, 56600, 56663, 56724, 56786, 56844,
 56898, 56958, 57019, 57083, 57140, 57203, 57260, 57318, 57370, 57423,
 57469, 57517, 57562, 57610, 57661, 57708, 57753, 57796, 57840, 57882,
 57918, 57957, 57995, 58027, 58066, 58104, 58136, 58177, 58221, 58266,
 58314, 58350, 58373, 58397, 58418, 58441, 58465, 58490, 58518, 58550,
 58576, 58576, 58576, 58576, 58576, 58576, 58576, 58576, 58576, 58576};

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
    if (!on_) {
      Power(false);
    }
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
  void SB_Lockup() override {  }

  void SB_Top() override {
    if (!millis_sum_) return;
    Serial.print("blade fps: ");
    Serial.println(updates_ * 1000.0 / millis_sum_);
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
         last_millis_ = 0;
         YIELD();
       }
       // Wait until it's our turn.
       while (current_blade) YIELD();
       current_blade = this;
       current_style_->run(this);
       while (monopodws.busy()) YIELD();
//       monopodws.begin(num_leds_, displayMemory, config_, pin_);
       monopodws.show();
       {
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
       }
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
  int updates_ = 0;
  int millis_sum_ = 0;
  uint32_t last_millis_ = 0;
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
    if (!on_) {
      Power(false);
    }
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
  void SB_Lockup() override {  }

  void SB_Top() override {
    if (!millis_sum_) return;
    Serial.print("blade fps: ");
    Serial.println(updates_ * 1000.0 / millis_sum_);
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
      last_millis_ = 0;
      return;
    }
    int m = millis();
    // This limits the blade to 1000 updates per second, which
    // may not be enough for POV-style things, but I suspect
    // than running it at full speed will cause sound problems.
    // Note that the FASTLED code is so far untested, so it might
    // not work right.
    if (m == last_millis_) return;
    if (last_millis_) {
      millis_sum_ += m - last_millis_;
      updates_ ++;
      if (updates_ > 1000) {
        updates_ /= 2;
        millis_sum_ /= 2;
      }
    }
    last_millis_ = m;
    current_style_->run(this);
    Show();
  }
  
private:
  int num_leds_;
  bool on_ = false;
  bool powered_ = false;
  bool clash_ = false;

  // TOOD: Break this out into a separate class.
  int updates_ = 0;
  int millis_sum_ = 0;
  uint32_t last_millis_ =0;
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
    analogWriteFrequency(pin_, 1000);
    analogWrite(pin_, 0);  // make it black
  }
  void set(const Color& c) {
    analogWrite(pin_, c_->PWM(c));
  }
  void set_overdrive(const Color& c) {
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
  void SB_Lockup() override {  }

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
  String_Blade(LEDInterface* c) :
    SaberBase(NOLINK),
    CommandParser(NOLINK),
    Looper(NOLINK),
    c_(c) {
  }

  void Activate() override {
    Serial.println("String Blade");
    analogWriteResolution(8);
    for (int i = 0; i < STRING_SEGMENTS; i++) {
      analogWriteFrequency(pin_[i], 1000);
      analogWrite(pin_[i], 0);  // make it black
    }
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
  }

  void set_overdrive(int led, Color c) override {
    analogWrite(pin_[led], c_->PWM_overdrive(c));
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
  void SB_Lockup() override {  }

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

// Possibly one LED driver per channel...
template<class LED>
class String_Blade *StringBladePtr() {
  static String_Blade blade(LEDPtr<LED>());
  return &blade;
}
#endif

typedef Rgb<255,0,0> RED;
typedef Rgb<0,255,0> GREEN;
typedef Rgb<0,0,255> BLUE;
typedef Rgb<255,255,0> YELLOW;
typedef Rgb<0,255,255> CYAN;
typedef Rgb<255,0,255> MAGENTA;
typedef Rgb<255,255,255> WHITE;
typedef Rgb<0,0,0> BLACK;

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
  { "font01", "tracks/title.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>() },
  { "font01", "tracks/cantina.wav",
    StylePtr<InOutSparkTip<SimpleClash<BLUE, WHITE>, 300, 800> >() },
  { "caliban", "tracks/duel.wav", StyleFirePtr<RED, YELLOW>() },
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 300, 800>() },
  { "font02", "tracks/title.wav", StyleFirePtr<BLUE, CYAN>() },
  { "igniter/font4", "tracks/duel.wav",
    StylePtr<InOutHelper<SimpleClash<OnSpark<GREEN> >, 300, 800> >() },
  { "font01", "tracks/duel.wav", StyleNormalPtr<WHITE, RED, 300, 800>() },
  { "font01", "tracks/walls.wav", StyleNormalPtr<YELLOW, BLUE, 300, 800>() },
  { "font01", "tracks/title.wav", 
    StylePtr<InOutSparkTip<SimpleClash<MAGENTA, WHITE>, 300, 800> >() },
  { "font02", "tracks/cantina.wav", StyleNormalPtr<Gradient<RED, BLUE>, WHITE, 300, 800>() },
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
  ButtonBase(const char* name)
    : Looper(),
      CommandParser(),
      name_(name),
      pushed_(false),
      clicked_(false),
      long_clicked_(false),
      push_millis_(0) {
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

  bool long_clicked() {
    bool ret = long_clicked_;
    long_clicked_ = false;
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
          clicked_ = true;
        } else {
          long_clicked_ = true;
        }
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
  bool clicked_;
  bool long_clicked_;
  bool eat_click_ = false;
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


#if 1
class Script : Looper, StateMachine {
public:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while(!run_) YIELD();
    run_ = false;
#ifdef ENABLE_AUDIO
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    beeper.Beep(0.05, 2000.0);
    SLEEP(100);
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    beeper.Beep(0.05, 2000.0);
    SLEEP(100);
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    beeper.Beep(0.05, 3000.0);
#endif
    SLEEP(20000);
    CommandParser::DoParse("on", NULL);
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
            power_(powerButtonPin, 1700, "pow"),
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

  unsigned long last_clash = 0;
  void Clash() {
    // TODO: Pick clash randomly and/or based on strength of clash.
    unsigned long t = millis();
    if (t - last_clash < 100) return;
    last_clash = t;
    if (on_) {
      SaberBase::DoClash();
    } else {
      if (power_.pushed_millis()) {
        power_.EatClick();
        Serial.println("Previous preset");
        previous_preset();
      }
    }
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
          first = f;
        } else {
          if (cmpdir(f.name(), first.name())*sign < 0) first = f;
        }
        if (cmpdir(f.name(), current_directory)*sign <= 0) continue;
        if (best && cmpdir(f.name(), best.name())*sign > 0) continue;
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
  bool lockup_ = false;
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
    bool disable_lockup_ = true;
#ifdef ENABLE_AUDIO
    if (track_player_ && !track_player_->isPlaying()) {
      track_player_ = NULL;
    }
#endif
    if (!on_) {
      if (power_.clicked()) {
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
      }
      if (power_.long_clicked()) {
        if (aux_.pushed_millis()) {
          aux_.EatClick();
          next_directory();
          Serial.println("next directory");
        } else {
          StartOrStopTrack();
        }
      }
      if (aux_.clicked()) {
        aux_on_ = true;
        On();
        Serial.println("On (aux)");
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
      if (a->clicked()) {
        Serial.println("Off");
        Off();
      }
      if (b->clicked()) {
        SaberBase::DoBlast();
        Serial.println("Blast");
      }
      if (a->long_clicked()) {
        Serial.println("Force");
        SaberBase::DoForce();
      }
      if (b->pushed_millis() > 500) {
        disable_lockup_ = false;
        if (!lockup_) {
          lockup_ = true;
          SaberBase::DoBeginLockup();
        }
      }
    }
    if (lockup_ && disable_lockup_) {
      lockup_ = false;
      SaberBase::DoEndLockup();
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

  bool on_;
  TouchButton power_;
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
    if (!strcmp(cmd, "top")) {
      // TODO: list cpu usage for various objects.
      SaberBase::DoTop();
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
      writeByte(CTRL2_G, 0x8A);   // 1.66kHz gyro, 2000 dps
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
        Serial.println(wav_players[i].isPlaying() ? "On" : "Off");
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
  delay(1000);
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
	SIM_SCGC5 = 0x00043F82;		// clocks active to all GPIO
	SIM_SCGC6 = SIM_SCGC6_RTC | SIM_SCGC6_FTM0 | SIM_SCGC6_FTM1 | SIM_SCGC6_ADC0 | SIM_SCGC6_FTFL;
#elif defined(__MK20DX256__)
	SIM_SCGC3 = SIM_SCGC3_ADC1 | SIM_SCGC3_FTM2;
	SIM_SCGC5 = 0x00043F82;		// clocks active to all GPIO
	SIM_SCGC6 = SIM_SCGC6_RTC | SIM_SCGC6_FTM0 | SIM_SCGC6_FTM1 | SIM_SCGC6_ADC0 | SIM_SCGC6_FTFL;
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
	SIM_SCGC3 = SIM_SCGC3_ADC1 | SIM_SCGC3_FTM2 | SIM_SCGC3_FTM3;
	SIM_SCGC5 = 0x00043F82;		// clocks active to all GPIO
	SIM_SCGC6 = SIM_SCGC6_RTC | SIM_SCGC6_FTM0 | SIM_SCGC6_FTM1 | SIM_SCGC6_ADC0 | SIM_SCGC6_FTFL;
	//PORTC_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_DSE | PORT_PCR_SRE;
	//GPIOC_PDDR |= (1<<5);
	//GPIOC_PSOR = (1<<5);
	//while (1);
#elif defined(__MKL26Z64__)
	SIM_SCGC4 = SIM_SCGC4_USBOTG | 0xF0000030;
	SIM_SCGC5 = 0x00003F82;		// clocks active to all GPIO
	SIM_SCGC6 = SIM_SCGC6_ADC0 | SIM_SCGC6_TPM0 | SIM_SCGC6_TPM1 | SIM_SCGC6_TPM2 | SIM_SCGC6_FTFL;
#endif
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
	SCB_CPACR = 0x00F00000;
#endif
#if defined(__MK66FX1M0__)
	LMEM_PCCCR = 0x85000003;
#endif
	
#define SETUP_PIN(X) do {						\
 CORE_PIN##X##_PORTREG &=~ CORE_PIN##X##_BITMASK;			\
 CORE_PIN##X##_CONFIG = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);	\
 CORE_PIN##X##_DDRREG |= CORE_PIN##X##_BITMASK;				\
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
  DataStreamWork::LockSD(lock);
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
  };

  struct MTPContainer {
    uint32_t len;  // 0
    uint16_t type; // 4
    uint16_t op;   // 6
    uint32_t transaction_id; // 8
    uint32_t params[5];    // 12
  };

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
