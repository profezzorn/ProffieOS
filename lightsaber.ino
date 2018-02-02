/*
 Teensy Lightsaber Firmware
 http://fredrik.hubbe.net/lightsaber/teensy_saber.html
 Copyright (c) 2016-2017 Fredrik Hubinette
 Additional copyright holders listed inline below.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// You can have multiple configuration files, and specify which one
// to use here.

// #define CONFIG_FILE "default_v3_config.h"
// #define CONFIG_FILE "crossguard_config.h"
// #define CONFIG_FILE "graflex_v1_config.h"
// #define CONFIG_FILE "owk_v2_config.h"
// #define CONFIG_FILE "test_bench_config.h"
// #define CONFIG_FILE "toy_saber_config.h"
#define CONFIG_FILE "new_config.h"

#define CONFIG_TOP
#include CONFIG_FILE
#undef CONFIG_TOP

// #define ENABLE_DEBUG

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
//   stab detect/effect
// Make sure that sound is off before doing file command
// make "charging style" prevent you from turning the saber "on"
// 
// Audio work items:
//   Tune swings better
//   select clash from force
//   stab effect
// Blade stuff
//    better clash
// Implement menues:
//    select sound font
//    select color
//    adjust volume
// Disable motion when off to save power.
// Allow several blades to share power pins.


// If defined, DAC vref will be 3 volts, resulting in louder sound.
#define LOUD

// You can get better SD card performance by
// activating the  USE_TEENSY3_OPTIMIZED_CODE define
// in SD.h in the teensy library, however, my sd card
// did not work with that define.

#include <Arduino.h>
#include <EEPROM.h>

#ifdef TEENSYDUINO
#include <DMAChannel.h>
#include <usb_dev.h>
#include <kinetis.h>
#include <i2c_t3.h>
#include <SD.h>
#else
#include <Wire.h>
#include <FS.h>
#define digitalWriteFast digitalWrite
#include <stm32l4_wiring_private.h>
#include <stm32l4xx.h>
#include <armv7m.h>
#include <stm32l4_gpio.h>
#include <stm32l4_sai.h>
#include <stm32l4_dma.h>
#include <stm32l4_system.h>
#define DMAChannel stm32l4_dma_t
#define DMAMEM
#define NVIC_SET_PRIORITY(X,Y) NVIC_SetPriority((X), (IRQn_Type)(Y))
#endif

#include <SPI.h>
#include <math.h>
#include <malloc.h>

#ifdef ENABLE_SERIALFLASH

// This is a hack to let me access the internal stuff..
#define private public
#define protected public

#include <SerialFlash.h>

#undef private
#undef protected
#endif


#ifdef ENABLE_SNOOZE

#define startup_early_hook DISABLE_startup_early_hook
#include <Snooze.h>
#undef startup_early_hook

SnoozeTimer snooze_timer;
SnoozeDigital snooze_digital;
SnoozeTouch snooze_touch;
SnoozeBlock snooze_config(snooze_touch, snooze_digital, snooze_timer);
#endif

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


// Debug printout helper class
class Monitoring  {
public:
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
    MonitorSerial = 512,
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
  
  void Loop() {
    if (millis() - last_monitor_loop_ > monitor_frequency_ms_) {
      monitor_soon_ = active_monitors_;
      last_monitor_loop_ = millis();
    }
  }

  void Toggle(MonitorBit bit) {
    active_monitors_ ^= bit;
  }
private:
  uint32_t monitor_frequency_ms_ = 100;
  int last_monitor_loop_ = 0;
  uint32_t monitor_soon_ = 0;
  uint32_t active_monitors_ = 0;
};

Monitoring monitor;

Print* default_output = &Serial;
Print* stdout_output = &Serial;

class ConsoleHelper : public Print {
public:
  bool debug_is_on() const {
    return monitor.IsMonitoring(Monitoring::MonitorSerial) &&
      stdout_output != default_output;
  }
  size_t write(uint8_t b) override {
    size_t ret = stdout_output->write(b);
    if (debug_is_on()) default_output->write(b);
    return ret;
  }
  size_t write(const uint8_t *buffer, size_t size) override {
    size_t ret = stdout_output->write(buffer, size);
    if (debug_is_on()) default_output->write(buffer, size);
    return ret;
  }
#ifdef TEENSYDUINO
  int availableForWrite(void) override {
    return stdout_output->availableForWrite();
  }
  virtual void flush() override {
    stdout_output->flush();
    if (debug_is_on()) default_output->flush();
  }
#endif
};

ConsoleHelper STDOUT;

void PrintQuotedValue(const char *name, const char* str) {
  STDOUT.print(name);
  STDOUT.write('=');
  if (str) {
    while (*str) {
      switch (*str) {
        case '\n':
          STDOUT.print("\\n");
          break;
        case '\\':
          STDOUT.write('\\');
        default:
          STDOUT.write(*str);
      }
      ++str;
    }
  }
  STDOUT.write('\n');
}

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
      STDOUT.print("Linked list " #START " has invalid pointer @ ");    \
      STDOUT.print(__LINE__);                                           \
      STDOUT.print(" pointer: ");                                       \
      STDOUT.println((long)i, 16);                                      \
      START = NULL;                                                     \
      break;                                                            \
    }                                                                   \
    if (++len > 1000) {                                                 \
      STDOUT.print("Linked list " #START " has become infinite @ ");    \
      STDOUT.println(__LINE__);                                         \
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
#ifdef TEENSYDUINO
    ARM_DEMCR |= ARM_DEMCR_TRCENA;
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
    cycles_ = ARM_DWT_CYCCNT;
#endif    
  }
  ~ScopedCycleCounter() {
#ifdef TEENSYDUINO
    cycles_ = ARM_DWT_CYCCNT - cycles_;
    dest_ += cycles_;
#endif
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
      STDOUT.print(updates_ * 1000.0 / millis_sum_);
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
      ScopedCycleCounter cc2(l->cycles_);
      l->Loop();
    }
    global_loop_counter.Update();
  }
  static void DoSetup() {
    for (Looper *l = loopers; l; l = l->next_looper_) {
      l->Setup();
    }
  }
  static void LoopTop(double total_cycles) {
    for (Looper *l = loopers; l; l = l->next_looper_) {
      STDOUT.print(l->name());
      STDOUT.print(" loop: ");
      STDOUT.print(l->cycles_ * 100.0 / total_cycles);
      STDOUT.println("%");
      l->cycles_ = 0;
    }
  }

protected:
  virtual const char* name() = 0;
  virtual void Loop() = 0;
  virtual void Setup() {}
private:
  uint64_t cycles_ = 0;
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

// Debug printout helper class
class MonitorHelper : Looper, CommandParser {
public:
  MonitorHelper() : Looper(), CommandParser() {}
  const char* name() override { return "MonitorHelper"; }
  
protected:
  void Loop() { monitor.Loop(); }
  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "monitor") || !strcmp(cmd, "mon")) {
      if (!strcmp(arg, "swings")) {
        monitor.Toggle(Monitoring::MonitorSwings);
        return true;
      }
      if (!strcmp(arg, "gyro")) {
        monitor.Toggle(Monitoring::MonitorGyro);
        return true;
      }
      if (!strcmp(arg, "samples")) {
        monitor.Toggle(Monitoring::MonitorSamples);
        return true;
      }
      if (!strcmp(arg, "touch")) {
        monitor.Toggle(Monitoring::MonitorTouch);
        return true;
      }
      if (!strcmp(arg, "battery")) {
        monitor.Toggle(Monitoring::MonitorBattery);
        return true;
      }
      if (!strcmp(arg, "pwm")) {
        monitor.Toggle(Monitoring::MonitorPWM);
        return true;
      }
      if (!strcmp(arg, "clash")) {
        monitor.Toggle(Monitoring::MonitorClash);
        return true;
      }
      if (!strcmp(arg, "temp")) {
        monitor.Toggle(Monitoring::MonitorTemp);
        return true;
      }
      if (!strcmp(arg, "strokes")) {
        monitor.Toggle(Monitoring::MonitorStrokes);
        return true;
      }
      if (!strcmp(arg, "serial")) {
        monitor.Toggle(Monitoring::MonitorSerial);
        return true;
      }
    }
    return false;
  }
  void Help() {
    STDOUT.println(" mon[itor] swings/samples/touch/battery/pwm/clash/temp/serial - toggle monitoring");
  }
};

MonitorHelper monitor_helper;

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
  Vec3 operator/(int i) const {
    return Vec3(x / i, y / i, z / i);
  }
  Vec3 dot(const Vec3& o) const {
    return Vec3(x * o.x, y * o.y, z * o.z);
  }
  float len2() const { return x*x + y*y + z*z; }
  float len() const { return sqrt(len2()); }

  static void Rotate90(float& a, float& b) {
    float tmp = b;
    b = -a;
    a = tmp;
  }
  static void Rotate180(float& a, float& b) {
    a = -a;
    b = -b;
  }
  void RotateX90() { Rotate90(y, z); }
  void RotateZ90() { Rotate90(x, y); }
  void RotateY90() { Rotate90(z, x); }

  void RotateX180() { Rotate180(y, z); }
  void RotateZ180() { Rotate180(x, y); }
  void RotateY180() { Rotate180(z, x); }
  float x, y, z;
};

enum BUTTON : uint32_t {
  BUTTON_NONE = 0,   // used for gestures and the like
  BUTTON_POWER = 1,
  BUTTON_AUX = 2,
  BUTTON_AUX2 = 4,
  BUTTON_UP = 8,
  BUTTON_DOWN = 16,
  BUTTON_LEFT = 32,
  BUTTON_RIGHT = 64,
  BUTTON_SELECT = 128,
  MODE_ON = 256,
  MODE_OFF = 0,
};

enum EVENT : uint32_t {
  EVENT_NONE = 0,
  EVENT_PRESSED,
  EVENT_RELEASED,
  EVENT_CLICK_SHORT,
  EVENT_CLICK_LONG,
  EVENT_DOUBLE_CLICK, // Note, will also generate a short click
  EVENT_LATCH_ON,
  EVENT_LATCH_OFF,
  EVENT_STAB,
  EVENT_SWING,
  EVENT_SHAKE,
  EVENT_TWIST,
  EVENT_CLASH,
};

uint32_t current_modifiers = BUTTON_NONE;


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
  enum LockupType {
    LOCKUP_NONE,
    LOCKUP_NORMAL,
    LOCKUP_DRAG,
  };
  static LockupType Lockup() { return lockup_; }
  static void SetLockup(LockupType lockup) { lockup_ = lockup; }

  struct Blast {
    uint32_t start_micros;
    float location; // 0 = base, 1 = tip
  };

  static size_t NumBlasts() {
    while (num_blasts_ &&
           micros() - blasts_[num_blasts_-1].start_micros > 5000000) {
      num_blasts_--;
    }
    return num_blasts_;
  }
  static const Blast& getBlast(size_t i) {
    return blasts_[i];
  }
  static void addBlast(float location) {
    for (size_t i = NELEM(blasts_) - 1; i; i--) {
      blasts_[i] = blasts_[i-1];
    }
    blasts_[0].start_micros = micros();
    blasts_[0].location = location;
    num_blasts_ = min(num_blasts_ + 1, NELEM(blasts_));
  }

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
  SABERFUN(Speedup, (), ());                    \
                                                \
  /* Swing rotation speed degrees per second */ \
  SABERFUN(Motion, (const Vec3& gyro), (gyro)); \
  /* Accelertation in g */                      \
  SABERFUN(Accel, (const Vec3& accel), (accel));\
                                                \
  SABERFUN(Top, (), ());                        \
  SABERFUN(IsOn, (bool* on), (on));             \
  SABERFUN(Message, (const char* msg), (msg));

  SABERBASEFUNCTIONS();
#undef SABERFUN

private:
  static size_t num_blasts_;
  static struct Blast blasts_[3];
  static LockupType lockup_;
  SaberBase* next_saber_;
};

SaberBase::LockupType SaberBase::lockup_ = SaberBase::LOCKUP_NONE;
size_t SaberBase::num_blasts_ = 0;
struct SaberBase::Blast SaberBase::blasts_[3];

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


// Returns the decimals of a number, ie 12.2134 -> 0.2134
float fract(float x) { return x - floor(x); }

// clamp(x, a, b) makes sure that x is between a and b.
float clamp(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
float fmod(float a, float b) {
  return a - floor(a / b) * b;
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
#define AUDIO_BUFFER_SIZE 44
#define AUDIO_RATE 44100

#ifdef USE_I2S

#ifdef TEENSYDUINO
// MCLK needs to be 48e6 / 1088 * 256 = 11.29411765 MHz -> 44.117647 kHz sample rate
//
#if F_CPU == 96000000 || F_CPU == 48000000 || F_CPU == 24000000
  // PLL is at 96 MHz in these modes
  #define MCLK_MULT 2
  #define MCLK_DIV  17
#elif F_CPU == 72000000
  #define MCLK_MULT 8
  #define MCLK_DIV  51
#elif F_CPU == 120000000
  #define MCLK_MULT 8
  #define MCLK_DIV  85
#elif F_CPU == 144000000
  #define MCLK_MULT 4
  #define MCLK_DIV  51
#elif F_CPU == 168000000
  #define MCLK_MULT 8
  #define MCLK_DIV  119
#elif F_CPU == 180000000
  #define MCLK_MULT 16
  #define MCLK_DIV  255
  #define MCLK_SRC  0
#elif F_CPU == 192000000
  #define MCLK_MULT 1
  #define MCLK_DIV  17
#elif F_CPU == 216000000
  #define MCLK_MULT 8
  #define MCLK_DIV  153
  #define MCLK_SRC  0
#elif F_CPU == 240000000
  #define MCLK_MULT 4
  #define MCLK_DIV  85
#elif F_CPU == 16000000
  #define MCLK_MULT 12
  #define MCLK_DIV  17
#else
  #error "This CPU Clock Speed is not supported.";
#endif

#ifndef MCLK_SRC
#if F_CPU >= 20000000
  #define MCLK_SRC  3  // the PLL
#else
  #define MCLK_SRC  0  // system clock
#endif
#endif

#define CHANNELS 2

#else // TEENSYDUINO

#define CHANNELS 1

#endif

#else   // USE_I2S
#define CHANNELS 1
#endif  // USE_I2S

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_CONT | PDB_SC_PDBIE | PDB_SC_DMAEN)

class LS_DAC : CommandParser, Looper {
public:
  virtual void Loop() override {}
  virtual const char* name() { return "DAC"; }
  void Setup() override {
#ifdef TEENSYDUINO
    dma.begin(true); // Allocate the DMA channel first

#ifdef USE_I2S
    SIM_SCGC6 |= SIM_SCGC6_I2S;
    SIM_SCGC7 |= SIM_SCGC7_DMA;
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX;

    // enable MCLK output
    I2S0_MCR = I2S_MCR_MICS(MCLK_SRC) | I2S_MCR_MOE;
    while (I2S0_MCR & I2S_MCR_DUF) ;
    I2S0_MDR = I2S_MDR_FRACT((MCLK_MULT-1)) | I2S_MDR_DIVIDE((MCLK_DIV-1));

    // configure transmitter
    I2S0_TMR = 0;
    I2S0_TCR1 = I2S_TCR1_TFW(1);  // watermark at half fifo size
    I2S0_TCR2 = I2S_TCR2_SYNC(0) | I2S_TCR2_BCP | I2S_TCR2_MSEL(1)
      | I2S_TCR2_BCD | I2S_TCR2_DIV(3);
    I2S0_TCR3 = I2S_TCR3_TCE;
    I2S0_TCR4 = I2S_TCR4_FRSZ(1) | I2S_TCR4_SYWD(15) | I2S_TCR4_MF
      | I2S_TCR4_FSE | I2S_TCR4_FSP | I2S_TCR4_FSD;
    I2S0_TCR5 = I2S_TCR5_WNW(15) | I2S_TCR5_W0W(15) | I2S_TCR5_FBT(15);

    // configure pin mux for 3 clock signals
    CORE_PIN23_CONFIG = PORT_PCR_MUX(6); // pin 23, PTC2, I2S0_TX_FS (LRCLK)
    CORE_PIN9_CONFIG  = PORT_PCR_MUX(6); // pin  9, PTC3, I2S0_TX_BCLK
    CORE_PIN22_CONFIG = PORT_PCR_MUX(6); // pin 22, PTC1, I2S0_TXD0

#if defined(KINETISK)
    dma.TCD->SADDR = dac_dma_buffer;
    dma.TCD->SOFF = 2;
    dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
    dma.TCD->NBYTES_MLNO = 2;
    dma.TCD->SLAST = -sizeof(dac_dma_buffer);
    dma.TCD->DADDR = &I2S0_TDR0;
    dma.TCD->DOFF = 0;
    dma.TCD->CITER_ELINKNO = sizeof(dac_dma_buffer) / 2;
    dma.TCD->DLASTSGA = 0;
    dma.TCD->BITER_ELINKNO = sizeof(dac_dma_buffer) / 2;
    dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
#endif
    dma.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_TX);
    dma.enable();
    
    I2S0_TCSR = I2S_TCSR_SR;
    I2S0_TCSR = I2S_TCSR_TE | I2S_TCSR_BCE | I2S_TCSR_FRDE;

#else   // USE_I2S

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
#endif
    dma.attachInterrupt(isr);

#else  // teensyduino
    // check return value?
    stm32l4_dma_create(&dma, DMA_CHANNEL_DMA2_CH6_SAI1_A, STM32L4_SAI_IRQ_PRIORITY);
    // NVIC_SetPriority(sai->interrupt, sai->priority);
    // NVIC_EnableIRQ(sai->interrupt);
    SAI_Block_TypeDef *SAIx = SAI1_Block_A;
    uint32_t sai_cr1 = (SAI_xCR1_DS_2);
    uint32_t saiclk = SYSTEM_SAICLK_11289600;
    sai_cr1 |= SAI_xCR1_CKSTR;
    uint32_t sai_frcr = (31 << SAI_xFRCR_FRL_Pos) | (15 << SAI_xFRCR_FSALL_Pos) | SAI_xFRCR_FSDEF | SAI_xFRCR_FSOFF;
    uint32_t sai_slotr = SAI_xSLOTR_NBSLOT_0 | (0x0003 << SAI_xSLOTR_SLOTEN_Pos) | SAI_xSLOTR_SLOTSZ_0;
    stm32l4_system_periph_enable(SYSTEM_PERIPH_SAI1);
    stm32l4_dma_enable(&dma, &isr, 0);
    SAIx->CR1 = sai_cr1;
    SAIx->FRCR = sai_frcr;
    SAIx->SLOTR = sai_slotr;
    stm32l4_system_saiclk_configure(saiclk);
    stm32l4_gpio_pin_configure(GPIO_PIN_PB13_SAI1_SCK_A, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_gpio_pin_configure(GPIO_PIN_PB12_SAI1_FS_A, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_gpio_pin_configure(GPIO_PIN_PB15_SAI1_SD_A, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_dma_start(&dma, (uint32_t)&SAIx->DR, (uint32_t)dac_dma_buffer, AUDIO_BUFFER_SIZE * 2,
                      DMA_OPTION_EVENT_TRANSFER_DONE |
                      DMA_OPTION_EVENT_TRANSFER_HALF |
                      DMA_OPTION_MEMORY_TO_PERIPHERAL |
                      DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
                      DMA_OPTION_MEMORY_DATA_SIZE_16 |
                      DMA_OPTION_MEMORY_DATA_INCREMENT |
                      DMA_OPTION_PRIORITY_HIGH |
                      DMA_OPTION_CIRCULAR);

    SAIx->CR1 |= SAI_xCR1_DMAEN;
    if (!(SAIx->CR1 & SAI_xCR1_SAIEN))
    {
      SAIx->CR2 = SAI_xCR2_FTH_1;
      SAIx->CR1 |= SAI_xCR1_SAIEN;
      STDOUT.println("ENABLING SAI");
    }

#endif
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, "dacbuf")) {
#ifndef TEENSYDUINO
      SAI_Block_TypeDef *SAIx = SAI1_Block_A;
      STDOUT.print("STATUS: ");
      STDOUT.print(SAIx->SR, HEX);
      STDOUT.print(" CR1: ");
      STDOUT.print(SAIx->CR1, HEX);
      STDOUT.print(" CR2: ");
      STDOUT.println(SAIx->CR2, HEX);
#endif      
      STDOUT.print("Current position: ");
      STDOUT.println(((uint16_t*)current_position()) - dac_dma_buffer);
      for (size_t i = 0; i < NELEM(dac_dma_buffer); i++) {
        STDOUT.print(dac_dma_buffer[i] - 2048);
        if ((i & 0xf) == 0xf)
          STDOUT.println("");
        else
          STDOUT.print(" ");
      }
      STDOUT.println("");
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
    STDOUT.println(" dacbuf - print the current contents of the dac buffer");
  }

  void SetStream(class AudioStream* stream) {
    stream_ = stream;
  }

private:
  static uint32_t current_position() {
#ifdef TEENSYDUINO
    return (uint32_t)(dma.TCD->SADDR);
#else
    return (uint32_t)(dac_dma_buffer + stm32l4_dma_count(&dma));
#endif
  }
  // Interrupt handler.
  // Fills the dma buffer with new sample data.
#ifdef TEENSYDUINO
  static void isr(void)
#else
  static void isr(void* arg, unsigned long int event)
#endif
  {
    digitalWrite(38, !digitalRead(38));
    ScopedCycleCounter cc(audio_dma_interrupt_cycles);
    int16_t *dest, *end;
    uint32_t saddr = current_position();

#ifdef TEENSYDUINO
    dma.clearInterrupt();
#endif
    if (saddr < (uint32_t)dac_dma_buffer + sizeof(dac_dma_buffer) / 2) {
      // DMA is transmitting the first half of the buffer
      // so we must fill the second half
      dest = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*CHANNELS];
      end = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];
    } else {
      // DMA is transmitting the second half of the buffer
      // so we must fill the first half
      dest = (int16_t *)dac_dma_buffer;
      end = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*CHANNELS];
    }
    AudioStream *stream = stream_;
    int16_t data[AUDIO_BUFFER_SIZE];
    int n = 0;
    if (stream) {
      n = stream->read(data, (end-dest) / CHANNELS);
    }
    while (n < AUDIO_BUFFER_SIZE) data[n++] = 0;
    for (int i = 0; i < n; i++) {
#ifdef USE_I2S
#if CHANNELS == 2
      // Duplicate sample to left and right channel.
      *(dest++) = data[i];
#endif      
      *(dest++) = data[i];
#else
      *(dest++) = (((uint16_t*)data)[i] + 32768) >> 4;
#endif
    }
  }

  DMAMEM static uint16_t dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];
  static AudioStream * volatile stream_;
  static DMAChannel dma;
};

#ifdef TEENSYDUINO
DMAChannel LS_DAC::dma(false);
#else
DMAChannel LS_DAC::dma;
#endif  
AudioStream * volatile LS_DAC::stream_ = nullptr;
DMAMEM uint16_t LS_DAC::dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];

LS_DAC dac;

// Audio compressor, takes N input channels, sums them and divides the
// result by the square root of the average volume.
template<int N> class AudioDynamicMixer : public AudioStream, Looper {
public:
  AudioDynamicMixer() {
    for (int i = 0; i < N; i++) {
      streams_[i] = nullptr;
    }
  }
  const char* name() override { return "AudioDynamicMixer"; }

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
        v2 = v * volume_ / (my_sqrt(vol_) + 100);
        data[i] = clamptoi16(v2);
        peak_sum_ = max(abs(v), peak_sum_);
        peak_ = max(abs(v2), peak_);
      }
      data += to_do;
      elements -= to_do;
    }
    last_sample_ = v2;
    last_sum_ = v;
    
//    STDOUT.println(vol_);
    return ret;
  }

  void Loop() override {
    if (monitor.ShouldPrint(Monitoring::MonitorSamples)) {
      STDOUT.print("Samples: ");
      STDOUT.print(num_samples_);
      STDOUT.print(" AVG volume: ");
      STDOUT.print(vol_);
      STDOUT.print(" last input sample: ");
      STDOUT.print(last_sum_);
      STDOUT.print(" last output sample:");
      STDOUT.print(last_sample_);
      STDOUT.print(" peak sum: ");
      STDOUT.print(peak_sum_);
      STDOUT.print(" peak: ");
      STDOUT.println(peak_);
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

  void set_volume(int32_t volume) { volume_ = volume; }
  int32_t get_volume() const { return volume_; }

  AudioStream* streams_[N];
  int32_t vol_ = 0;
  int32_t last_sample_ = 0;
  int32_t last_sum_ = 0;
  int32_t peak_sum_ = 0;
  int32_t peak_ = 0;
  int32_t num_samples_ = 0;
  int32_t volume_ = VOLUME;
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

// This is adapted from the Talkie Library

#include "voice_data.h"

// TMS5200 coefficients
// These values are derived from the MAME project.
// See http://mamedev.org/source/src/emu/sound/tms5110r.c.html
//
const uint8_t tmsEnergy[0x10] = {
  0x00,0x01,0x02,0x03,0x04,0x06,0x08,0x0b,
  0x10,0x17,0x21,0x2f,0x3f,0x55,0x72,0x00
};
const uint8_t tmsPeriod[0x40] = {
  0x00,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
  0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,
  0x1d,0x1e,0x1f,0x20,0x22,0x24,0x26,0x28,
  0x29,0x2b,0x2d,0x30,0x31,0x33,0x36,0x37,
  0x39,0x3c,0x3e,0x40,0x44,0x48,0x4a,0x4c,
  0x51,0x55,0x57,0x5a,0x60,0x63,0x67,0x6b,
  0x70,0x75,0x7a,0x7f,0x85,0x8b,0x91,0x97,
  0x9d,0xa4,0xab,0xb2,0xba,0xc2,0xca,0xd3
};
// Coefficients below are shifted left 6 bits and in 2's complement form.
const uint16_t tmsK1[0x20] = {
  0x82c0,0x8380,0x8440,0x8580,0x86c0,0x8880,0x8ac0,0x8d40,
  0x9080,0x9440,0x9900,0x9ec0,0xa580,0xad40,0xb640,0xc0c0,
  0xcc40,0xd900,0xe680,0xf4c0,0x0340,0x1180,0x1f80,0x2cc0,
  0x3900,0x4400,0x4dc0,0x5640,0x5d80,0x63c0,0x6900,0x6d40
};
const uint16_t tmsK2[0x20] = {
  0xa200,0xa6c0,0xac40,0xb200,0xb880,0xbf80,0xc740,0xcf40,
  0xd7c0,0xe100,0xea40,0xf3c0,0xfd80,0x0740,0x1100,0x1a80,
  0x23c0,0x2c80,0x3500,0x3cc0,0x4400,0x4ac0,0x5100,0x5680,
  0x5b80,0x6000,0x6400,0x6780,0x6ac0,0x6d80,0x7000,0x7e80
};
// Coefficients below are shifted right 2 bits and in 2's complement form.
// Note that 2 bits of precision are lost, but 8 bit multiplicands are
// faster to compute.
const uint8_t tmsK3[0x10] = {
  0x9a,0xa1,0xa9,0xb2,0xbd,0xca,0xd8,0xe7,
  0xf6,0x06,0x16,0x25,0x34,0x40,0x4c,0x55
};
const uint8_t tmsK4[0x10] = {
  0xb8,0xc1,0xcc,0xd7,0xe4,0xf1,0xfe,0x0b,
  0x18,0x25,0x31,0x3c,0x46,0x4e,0x56,0x5d
};
const uint8_t tmsK5[0x10] = {
  0xb1,0xb9,0xc3,0xce,0xd9,0xe5,0xf2,0xff,
  0x0c,0x19,0x26,0x31,0x3c,0x46,0x4e,0x56
};
const uint8_t tmsK6[0x10] = {
  0xd0,0xda,0xe5,0xf0,0xfb,0x07,0x12,0x1d,
  0x28,0x32,0x3b,0x44,0x4b,0x53,0x59,0x5e
};
const uint8_t tmsK7[0x10] = {
  0xc1,0xca,0xd3,0xdd,0xe8,0xf3,0xfe,0x09,
  0x14,0x1f,0x29,0x33,0x3c,0x45,0x4c,0x53
};
const uint8_t tmsK8[0x08] = {
  0xcd,0xe4,0xfe,0x17,0x2f,0x43,0x54,0x61
};
const uint8_t tmsK9[0x08] = {
  0xc2,0xd2,0xe5,0xf8,0x0c,0x20,0x32,0x41
};
const uint8_t tmsK10[0x08] = {
  0xd1,0xdf,0xee,0xfe,0x0d,0x1d,0x2b,0x39
};

#define CHIRP_SIZE 41
const uint8_t chirp[CHIRP_SIZE] = {0x00,0x2a,0xd4,0x32,0xb2,0x12,0x25,0x14,0x02,0xe1,0xc5,0x02,0x5f,0x5a,0x05,0x0f,0x26,0xfc,0xa5,0xa5,0xd6,0xdd,0xdc,0xfc,0x25,0x2b,0x22,0x21,0x0f,0xff,0xf8,0xee,0xed,0xef,0xf7,0xf6,0xfa,0x00,0x03,0x02,0x01};

class Talkie : public AudioStream {
public:
  const uint8_t* words[30];
  size_t num_words = 0;

  void Say(const uint8_t* addr) {
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    noInterrupts();
    if (ptrAddr) {
      if (num_words < NELEM(words)) {
        words[num_words++] = addr;
      }
    } else {
      count_ = 199;
      ptrAddr = addr;
      ptrBit = 0;
    }
    interrupts();
  }

  void SayDigit(int digit) {
    switch (digit) {
      case 0: Say(spZERO); break;
      case 1: Say(spONE); break;
      case 2: Say(spTWO); break;
      case 3: Say(spTHREE); break;
      case 4: Say(spFOUR); break;
      case 5: Say(spFIVE); break;
      case 6: Say(spSIX); break;
      case 7: Say(spSEVEN); break;
      case 8: Say(spEIGHT); break;
      case 9: Say(spNINE); break;
    }
  }

  // The ROMs used with the TI speech were serial, not byte wide.
  // Here's a handy routine to flip ROM data which is usually reversed.
  static uint8_t rev(uint8_t a) {
    // 76543210
    a = (a>>4) | (a<<4); // Swap in groups of 4
    // 32107654
    a = ((a & 0xcc)>>2) | ((a & 0x33)<<2); // Swap in groups of 2
    // 10325476
    a = ((a & 0xaa)>>1) | ((a & 0x55)<<1); // Swap bit pairs
    // 01234567
    return a;
  }

  uint8_t getBits(uint8_t bits) {
    uint8_t value;
    uint16_t data;
    data = rev(pgm_read_byte(ptrAddr))<<8;
    if (ptrBit+bits > 8) {
      data |= rev(pgm_read_byte(ptrAddr+1));
    }
    data <<= ptrBit;
    value = data >> (16-bits);
    ptrBit += bits;
    if (ptrBit >= 8) {
      ptrBit -= 8;
      ptrAddr++;
    }
    return value;
  }

  void ReadFrame() {
    if (!ptrAddr) return;
    
    // Read speech data, processing the variable size frames.
    uint8_t energy = getBits(4);
    if (energy == 0) {
      // Energy = 0: rest frame
      synthEnergy = 0;
    } else if (energy == 0xf) {
      // Energy = 15: stop frame. Silence the synthesiser.
      synthEnergy = 0;
      synthK1 = 0;
      synthK2 = 0;
      synthK3 = 0;
      synthK4 = 0;
      synthK5 = 0;
      synthK6 = 0;
      synthK7 = 0;
      synthK8 = 0;
      synthK9 = 0;
      synthK10 = 0;
      if (num_words) {
        ptrAddr = words[0];
        num_words--;
        for (size_t i = 0; i < num_words; i++) words[i] = words[i + 1];
        ptrBit = 0;
      } else {
        ptrAddr = NULL;
      }
    } else {
      synthEnergy = tmsEnergy[energy];
      uint8_t repeat = getBits(1);
      synthPeriod = tmsPeriod[getBits(6)];
      // A repeat frame uses the last coefficients
      if (!repeat) {
        // All frames use the first 4 coefficients
        synthK1 = tmsK1[getBits(5)];
        synthK2 = tmsK2[getBits(5)];
        synthK3 = tmsK3[getBits(4)];
        synthK4 = tmsK4[getBits(4)];
        if (synthPeriod) {
          // Voiced frames use 6 extra coefficients.
          synthK5 = tmsK5[getBits(4)];
          synthK6 = tmsK6[getBits(4)];
          synthK7 = tmsK7[getBits(4)];
          synthK8 = tmsK8[getBits(3)];
          synthK9 = tmsK9[getBits(3)];
          synthK10 = tmsK10[getBits(3)];
        }
      }
    }
  }
  
  int16_t Get8kHz() {
    if (count_++ >= 200) {
      ReadFrame();
      count_ = 0;
    }
    
    static uint8_t periodCounter;
    static int16_t x0,x1,x2,x3,x4,x5,x6,x7,x8,x9;
    int16_t u0,u1,u2,u3,u4,u5,u6,u7,u8,u9,u10;

    if (synthPeriod) {
      // Voiced source
      if (periodCounter < synthPeriod) {
        periodCounter++;
      } else {
        periodCounter = 0;
      }
      if (periodCounter < CHIRP_SIZE) {
        u10 = ((chirp[periodCounter]) * (uint32_t) synthEnergy) >> 8;
      } else {
        u10 = 0;
      }
    } else {
      // Unvoiced source
      static uint16_t synthRand = 1;
      synthRand = (synthRand >> 1) ^ ((synthRand & 1) ? 0xB800 : 0);
      u10 = (synthRand & 1) ? synthEnergy : -synthEnergy;
    }
    // Lattice filter forward path
    u9 = u10 - (((int16_t)synthK10*x9) >> 7);
    u8 = u9 - (((int16_t)synthK9*x8) >> 7);
    u7 = u8 - (((int16_t)synthK8*x7) >> 7);
    u6 = u7 - (((int16_t)synthK7*x6) >> 7);
    u5 = u6 - (((int16_t)synthK6*x5) >> 7);
    u4 = u5 - (((int16_t)synthK5*x4) >> 7);
    u3 = u4 - (((int16_t)synthK4*x3) >> 7);
    u2 = u3 - (((int16_t)synthK3*x2) >> 7);
    u1 = u2 - (((int32_t)synthK2*x1) >> 15);
    u0 = u1 - (((int32_t)synthK1*x0) >> 15);

    // Output clamp
    if (u0 > 511) u0 = 511;
    if (u0 < -512) u0 = -512;

    // Lattice filter reverse path
    x9 = x8 + (((int16_t)synthK9*u8) >> 7);
    x8 = x7 + (((int16_t)synthK8*u7) >> 7);
    x7 = x6 + (((int16_t)synthK7*u6) >> 7);
    x6 = x5 + (((int16_t)synthK6*u5) >> 7);
    x5 = x4 + (((int16_t)synthK5*u4) >> 7);
    x4 = x3 + (((int16_t)synthK4*u3) >> 7);
    x3 = x2 + (((int16_t)synthK3*u2) >> 7);
    x2 = x1 + (((int32_t)synthK2*u1) >> 15);
    x1 = x0 + (((int32_t)synthK1*u0) >> 15);
    x0 = u0;

    return u0 << 5;
  }

  // Very very stupid upsamler, slightly better than
  // just repeating samples.
  int16_t tmp = 0;
  int16_t last = 0;
  int32_t N = 0;
  int16_t Get44kHz() {
    N += 80;
    if (N > 441) {
      last = Get8kHz();
      N -= 441;
    }
    tmp = (tmp * 3 + last) >> 2;
    return tmp;
  }
  
  int read(int16_t* data, int elements) override {
    for (int i = 0; i < elements; i++) {
      data[i] = Get44kHz();
    }
    return elements;
  }
  bool eof() const override {
    return ptrAddr == NULL && tmp == 0;
  }
  bool isPlaying() const {
    return !eof();
  }
  void Stop() override {}

private:
  const uint8_t * ptrAddr = NULL;
  uint8_t ptrBit = 0;
  
  uint8_t synthPeriod = 0;
  uint16_t synthEnergy = 0;
  int16_t synthK1 = 0;
  int16_t synthK2 = 0;
  int8_t synthK3 = 0;
  int8_t synthK4 = 0;
  int8_t synthK5 = 0;
  int8_t synthK6 = 0;
  int8_t synthK7 = 0;
  int8_t synthK8 = 0;
  int8_t synthK9 = 0;
  int8_t synthK10 = 0;

  uint8_t count_ = 0;
  uint8_t pos_ = 0;
};

Talkie talkie;


class LightSaberSynth : public AudioStream, Looper {
public:
  const char* name() override { return "LightSaberSynth"; }

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
      STDOUT.print("Last elements: ");
      STDOUT.print(last_elements);
      STDOUT.print("Last sample: ");
      STDOUT.print(last_value);
      STDOUT.print(" prevol: ");
      STDOUT.print(last_prevolume_value);
      STDOUT.print(" vol: ");
      STDOUT.println(volume_.value());
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
#ifdef TEENSYDUINO
    NVIC_SET_PRIORITY(IRQ_WAV, 240);
    _VectorsRam[IRQ_WAV + 16] = &ProcessAudioStreams;
    NVIC_ENABLE_IRQ(IRQ_WAV);
#endif    
  }
  ~AudioStreamWork() {
    for (AudioStreamWork** d = &data_streams; *d; d = &(*d)->next_) {
      if (*d == this) {
        *d = next_;
      }
    }
  }

  static void scheduleFillBuffer() {
#ifdef TEENSYDUINO
    if (!NVIC_IS_ACTIVE(IRQ_WAV))
      NVIC_TRIGGER_IRQ(IRQ_WAV);
#else
    armv7m_pendsv_enqueue((armv7m_pendsv_routine_t)ProcessAudioStreams, NULL, 0);
#endif    
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

#ifdef TEENSYDUINO
class LSFS {
public:
  typedef File FILE;
  static bool Begin() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
    // Prefer the built-in sd card for Teensy 3.5/3.6 as it is faster.
    return SD.begin(BUILTIN_SDCARD);
#else 
    return SD.begin(sdCardSelectPin);
#endif    
  }
  static bool Exists(const char* path) {
    return SD.exists(path);
  }
  static File Open(const char* path) {
    return SD.open(path);
  }
  static File OpenForWrite(const char* path) {
    return SD.open(path, FILE_WRITE);
  }
  class Iterator {
  public:
    explicit Iterator(const char* dirname) {
      dir_ = SD.open(dirname);
      if (dir_.isDirectory()) {
        f_ = dir_.openNextFile();
      }
    }
    explicit Iterator(Iterator& other) {
      dir_ = other.f_;
      other.f_ = File();
      f_ = dir_.openNextFile();
    }
    ~Iterator() {
      dir_.close();
      f_.close();
    }
    void operator++() {
      f_.close();
      f_ = dir_.openNextFile();
    }
    operator bool() { return f_; }
    bool isdir() { return f_.isDirectory(); }
    const char* name() { return f_.name(); }
    size_t size() { return f_.size(); }
    
  private:
    File dir_;
    File f_;
  };
};
#else
class LSFS {
public:
  typedef File FILE;
  static bool Begin() {
    return DOSFS.begin() && DOSFS.check();
  }
  static bool Exists(const char* path) {
    return DOSFS.exists(path);
  }
  static File Open(const char* path) {
    return DOSFS.open(path, "r");
  }
  static File OpenForWrite(const char* path) {
    return DOSFS.open(path, "wc");
  }
  class Iterator {
  public:
    explicit Iterator(const char* path) {
      char filename[F_MAXPATH];

      strcpy(_path, path);

      if (path[strlen(path)-1] != '/')  
        strcat(_path, "/");

      strcpy(filename, _path);
      strcat(filename, "*.*");

      if (f_findfirst(filename, &_find) != F_NO_ERROR) {
        _find.find_clsno = 0x0fffffff;
      }
    }
    explicit Iterator(Iterator& other) {
      char filename[F_MAXPATH];

      strcpy(_path, other._path);
      strcat(_path, other.name());

      if (_path[strlen(_path)-1] != '/')  
        strcat(_path, "/");

      strcpy(filename, _path);
      strcat(filename, "*.*");

      if (f_findfirst(filename, &_find) != F_NO_ERROR) {
        _find.find_clsno = 0x0fffffff;
      }
    }

    void operator++() {
      if (f_findnext(&_find) != F_NO_ERROR) {
        _find.find_clsno = 0x0fffffff;
      }
    }
    operator bool() { return _find.find_clsno != 0x0fffffff; }
    bool isdir() { return _find.attr & F_ATTR_DIR; }
    const char* name() { return _find.filename; }
    size_t size() { return _find.filesize; }
    
  private:
    char _path[F_MAXPATH];
    F_FIND _find;
  };
};
#endif

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

  // This class represents a specific effect file. It's main purpose
  // is to be smaller than using the filename to identify the file.
  class FileID {
   public:
    FileID(const Effect* effect, int file) : effect_(effect), file_(file) {}
    FileID() : effect_(nullptr), file_(0) {}

    bool operator==(const FileID& other) const {
      return other.effect_ == effect_ && file_ == other.file_;
    }

    operator bool() const { return effect_ != nullptr; }

    void GetName(char *filename) {
      effect_->GetName(filename, file_);
    }

   private:
    const Effect* effect_;
    int file_;
  };

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
    selected_ = -1;
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
      STDOUT.print("Found ");
      STDOUT.print(name_);
      STDOUT.print(" files: ");
      if (min_file_ <= max_file_) {
        STDOUT.print(min_file_);
        STDOUT.print("-");
        STDOUT.print(max_file_);
        if (digits_) {
          STDOUT.print(" using ");
          STDOUT.print(digits_);
          STDOUT.print(" digits");
        }
        if (unnumbered_file_found_) {
          STDOUT.print(" + ");
        }
      }
      if (unnumbered_file_found_) {
        STDOUT.print("one unnumbered file");
      }
      if (subdirs_) {
        STDOUT.print(" in subdirs");
      }
      STDOUT.println("");
    }
  }

  static void ShowAll() {
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Show();
    }
    STDOUT.println("Done listing effects.");
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

  void Select(int n) {
    selected_ = n;
  }

  FileID RandomFile() const {
    int num_files = files_found();
    if (num_files < 1) {
      default_output->print("No sounds found: ");
      default_output->println(name_);
      return FileID();
    }
    int n = rand() % num_files;
    if (selected_ != -1) n = selected_;
    return FileID(this, n);
  }

  bool Play(char *filename) {
    FileID f = RandomFile();
    if (f == FileID()) return false;
    f.GetName(filename);
    return true;
  }

  // Get the name of a specific file in the set.
  void GetName(char *filename, int n) const {
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
    
    default_output->print("Playing ");
    default_output->println(filename);
  }

  static void ScanAll(const char* filename) {
    if (Effect::IdentifyExtension(filename) == Effect::UNKNOWN) {
      return;
    }

#if 0
    // TODO: "monitor scan" command?
    STDOUT.print("SCAN ");
    STDOUT.println(filename);
#endif
    for (Effect* e = all_effects; e; e = e->next_) {
      e->Scan(filename);
    }
  }

  static void ScanDirectory(const char *directory) {
    STDOUT.print("Scanning sound font: ");
    STDOUT.print(directory);
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

#if 1
    if (LSFS::Exists(directory)) {
      for (LSFS::Iterator iter(directory); iter; ++iter) {
        if (iter.isdir()) {
          char fname[128];
          strcpy(fname, iter.name());
          strcat(fname, "/");
          char* fend = fname + strlen(fname);
          for (LSFS::Iterator i2(iter); i2; ++i2) {
            strcpy(fend, i2.name());
            ScanAll(fname);
          }
        } else {
          ScanAll(iter.name());
        }
      }
    }
#else
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
    
#ifdef ENABLE_AUDIO
    else if (strlen(directory)) {
      talkie.Say(spBANK);
      talkie.Say(spOPEN);
      talkie.Say(spFAILURE);
    }
#endif   // ENABLE_AUDIO    
#endif   // ENABLE_SD
    STDOUT.println(" done");
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

  // If not -1, return this file.
  int selected_;

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

// Looped swing fonts. (SmoothSwing V1/V2)
EFFECT(swingl);  // Looped swing, LOW
EFFECT(swingh);  // Looped swing, HIGH

// Drag effect, replaces "lock/lockup" in drag mode if present.
EFFECT(drag);

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

  const char* Filename() const {
    return filename_;
  }

  void PlayOnce(Effect* effect, float start = 0.0) {
    if (effect->Play(filename_)) {
      start_ = start;
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
    return ((uint32_t *)buffer)[n+2];
  }

  template<int bits> int16_t read2() {
    if (bits == 8) return *(ptr_++) << 8;
    ptr_ += bits / 8 - 2;
    return *((*((int16_t**)&ptr_))++);
  }

  template<int bits, int channels, int rate>
  void DecodeBytes4() {
    while (ptr_ < end_ - channels * bits / 8 &&
           num_samples_ < (int)NELEM(samples_)) {
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
        STDOUT.println("Unsupported rate.");
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
    else if (bits_ == 16) DecodeBytes2<16>();
    else if (bits_ == 24) DecodeBytes2<24>();
    else if (bits_ == 32) DecodeBytes2<32>();
  }

  int ReadFile(int n) {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) {
      return sf_file_.read(buffer + 8, n);
    }
#endif
#ifdef ENABLE_SD
    return sd_file_.read(buffer + 8, n);
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

  void Rewind() {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) {
      sf_file_.seek(0);
      return;
    }
#endif
#ifdef ENABLE_SD
    sd_file_.seek(0);
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
      new_file_id_ = Effect::FileID();
      if (!run_) {
        new_file_id_ = effect_->RandomFile();
        if (!new_file_id_) goto fail;
        new_file_id_.GetName(filename_);
        run_ = true;
      }
      if (new_file_id_ && new_file_id_ == old_file_id_) {
        // Minor optimization: If we're reading the same file
        // as before, then seek to 0 instead of open/close file.
        Rewind();
      } else {
#ifdef ENABLE_SERIALFLASH
        sf_file_ = SerialFlashChip::open(filename_);
        if (!sf_file_)
#endif
        {
#ifdef ENABLE_SD
          sd_file_.close();
          sd_file_ = LSFS::Open(filename_);
          YIELD();
          if (!sd_file_)
#endif
          {
            STDOUT.print("File ");
            STDOUT.print(filename_);
            STDOUT.println(" not found.");
            goto fail;
          }
        }
        old_file_id_ = new_file_id_;
      }
      wav_ = endswith(".wav", filename_);
      if (wav_) {
        if (ReadFile(12) != 12) {
          STDOUT.println("Failed to read 12 bytes.");
          goto fail;
        }
        if (header(0) != 0x46464952 || header(2) != 0x45564157) {
          STDOUT.println("Not RIFF WAVE.");
          YIELD();
          goto fail;
        }

        // Look for FMT header.
        while (true) {
          if (ReadFile(8) != 8) {
            STDOUT.println("Failed to read 8 bytes.");
            goto fail;
          }

          len_ = header(1);
          if (header(0) != 0x20746D66) {  // 'fmt '
            Skip(len_);
            continue;
          }
          if (len_ < 16) {
            STDOUT.println("FMT header is wrong size..");
            goto fail;
          }
          break;
        }
        
        if (16 != ReadFile(16)) {
          STDOUT.println("Read failed.");
          goto fail;
        }
        if (len_ > 16) Skip(len_ - 16);
        if ((header(0) & 0xffff) != 1) {
          STDOUT.println("Wrong format.");
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
      STDOUT.print("channels: ");
      STDOUT.print(channels_);
      STDOUT.print(" rate: ");
      STDOUT.print(rate_);
      STDOUT.print(" bits: ");
      STDOUT.println(bits_);

      ptr_ = buffer + 8;
      end_ = buffer + 8;
      
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

        if (start_ != 0.0) {
          int samples = fmod(start_, length()) * rate_;
          int bytes_to_skip = samples * channels_ * bits_ / 8;
          Skip(bytes_to_skip);
          len_ -= bytes_to_skip;
          start_ = 0.0;
        }

        while (len_) {
          {
            int bytes_read = ReadFile(AlignRead(min(len_, 512u)));
            if (bytes_read == 0)
              break;
            len_ -= bytes_read;
            end_ = buffer + 8 + bytes_read;
          }
          while (ptr_ < end_ - channels_ * bits_ / 8) {
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
          if (ptr_ < end_) {
            memmove(buffer + 8 - (end_ - ptr_),
                    ptr_,
                    end_ - ptr_);
          }
          ptr_ = buffer + 8 - (end_ - ptr_);
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
  Effect::FileID new_file_id_;
  Effect::FileID old_file_id_;
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
  float start_ = 0.0;

  int rate_;
  uint8_t channels_;
  uint8_t bits_;

  bool wav_;

  size_t len_ = 0;
  volatile size_t sample_bytes_ = 0;
  unsigned char* ptr_;
  unsigned char* end_;
  unsigned char buffer[512 + 8]  __attribute__((aligned(4)));

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
// 1 / 500 second for to change the volume. (2ms)
const uint32_t kDefaultSpeed = 500 * kMaxVolume / AUDIO_RATE;

template<class T>
class VolumeOverlay : public T {
public:
  
  VolumeOverlay() : volume_(kMaxVolume / 100) {
    volume_.set(kDefaultVolume);
    volume_.set_target(kDefaultVolume);
    volume_.set_speed(kDefaultSpeed);
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
    volume_.set_speed(kDefaultSpeed);
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

  void PlayOnce(Effect* effect, float start = 0.0) {
    STDOUT.print("unit = ");
    STDOUT.print(WhatUnit(this));
    STDOUT.print(" vol = ");
    STDOUT.print(volume());
    STDOUT.print(", ");

    pause_ = true;
    clear();
    wav.PlayOnce(effect, start);
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

  const char* Filename() const {
    return wav.Filename();
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
  const char* name() override { return "AudioSplicer"; }

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
          // STDOUT.println("AudioSPlicer::EOF!!");
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
    STDOUT.print("FADE SPEED: ");
    STDOUT.println(fade_speed_);
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
  dynamic_mixer.streams_[NELEM(wav_players)+1] = &talkie;
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
    STDOUT.println("Activating monophonic font.");
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
    size_t total = poweroff.files_found() + pwroff.files_found();
    if (total) {
      if ((rand() % total) < poweroff.files_found()) {
        audio_splicer.Play(&poweroff, NULL);
      } else {
        audio_splicer.Play(&pwroff, NULL);
      }
    }
  }
  void SB_Clash() override { audio_splicer.Play(&clash, &hum); }
  void SB_Stab() override { audio_splicer.Play(&stab, &hum); }
  void SB_Force() override { audio_splicer.Play(&force, &hum); }
  void SB_Blast() override { audio_splicer.Play(&blaster, &hum); }
  void SB_Boot() override { audio_splicer.Play(&boot,  NULL); }
  void SB_NewFont() override { audio_splicer.Play(&font,  NULL); }

  void SB_BeginLockup() override {
    if (SaberBase::Lockup() == SaberBase::LOCKUP_DRAG &&
        drag.files_found()) {
      audio_splicer.Play(&drag, &drag);
    } else if (lockup.files_found()) {
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

  int64_t readIntValue(File* f) {
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

  float readFloatValue(File* f) {
    float ret = 0.0;
    float sign = 1.0;
    float mult = 1.0;
    if (f->peek() == '-') {
      sign = -1.0;
      f->read();
    }
    while (f->available()) {
      int c = toLower(f->peek());
      if (c >= '0' && c <= '9') {
        if (mult == 1.0) {
          ret = (c - '0') + 10 * ret;
        } else {
          ret += (c - '0') * mult;
          mult /= 10.0;
        }
        f->read();
      } else if (c == '.') {
        if (mult != 1.0) return ret * sign;
        // Time to read decimals.
        mult /= 10.0;
        f->read();
      } else {
        return ret * sign;
      }
    }
    return ret * sign;
  }

  void Read(File* f) {
    SetVariable("=", 0.0);  // This resets all variables.
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
      float v = readFloatValue(f);
#if 0
      STDOUT.print(variable);
      STDOUT.print(" = ");
      STDOUT.println(v);
#endif

      SetVariable(variable, v);
    }
  }
#endif
  virtual void SetVariable(const char* variable, float v) = 0;

  void Read(const char *filename) {
#ifdef ENABLE_SD
    File f = LSFS::Open(filename);
    Read(&f);
    f.close();
#endif
  }

  void ReadInCurrentDir(const char* name) {
    char full_name[128];
    strcpy(full_name, current_directory);
    strcat(full_name, name);
    Read(full_name);
  }
};

class IgniterConfigFile : public ConfigFile {
public:
#define CONFIG_VARIABLE(X, DEF) do {            \
    if (variable[0] == '=') X = DEF;            \
    else if (!strcasecmp(variable, #X)) {       \
      X = v;                                    \
      STDOUT.print(variable);                   \
      STDOUT.print("=");                        \
      STDOUT.println(v);                        \
      return;                                   \
    }                                           \
} while(0)
        
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(humStart, 100);
    CONFIG_VARIABLE(volHum, 15);
    CONFIG_VARIABLE(volEff, 16);
  }
  int humStart;
  int volHum;
  int volEff;
};

  
// With polyphonic fonts, sounds are played more or less
// independently. Hum is faded in/out by changing the volume
// and all other sound effects are just played in parallel
// when needed.
class PolyphonicFont : public SaberBase {
public:
  PolyphonicFont() : SaberBase(NOLINK) { }
  void Activate() {
    STDOUT.println("Activating polyphonic font.");
    SetupStandardAudio();
    wav_players[0].set_volume_now(0);
    config_.ReadInCurrentDir("config.ini");
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
        if (delay_ms > 0 && delay_ms < 30000) {
          hum_start_ += delay_ms;
        }
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
    Effect* e = &lock;
    if (SaberBase::Lockup() == SaberBase::LOCKUP_DRAG &&
        drag.files_found()) {
      e = &drag;
    }
    if (!lock_player_) {
      lock_player_ = Play(e);
      if (lock_player_) {
        lock_player_->PlayLoop(e);
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
    uint32_t m = micros();
    switch (state_) {
      case STATE_OFF:
        volume_ = 0.0f;
        return;
      case STATE_OUT:
        volume_ = 0.0f;
        if (millis() - hum_start_ < 0x7fffffffUL) {
          state_ = STATE_HUM_FADE_IN;
        }
        break;
      case STATE_HUM_FADE_IN: {
        uint32_t delta = m - last_micros_;
        volume_ += (delta / 1000000.0) / 0.2; // 0.2 seconds
        if (volume_ >= 1.0f) {
          volume_ = 1.0f;
          state_ = STATE_HUM_ON;
        }
        break;
      }
      case STATE_HUM_ON:
        break;
      case STATE_HUM_FADE_OUT: {
        uint32_t delta = m - last_micros_;
        volume_ -= (delta / 1000000.0) / 0.2; // 0.2 seconds
        if (volume_ <= 0.0f) {
          volume_ = 0.0f;
          state_ = STATE_OFF;
        }
        break;
      }
    }
    last_micros_ = m;
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

  IgniterConfigFile config_;
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

class SmoothSwingConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(Version, 1);
    CONFIG_VARIABLE(SwingSensitivity, 450.0);
    CONFIG_VARIABLE(MaximumHumDucking, 75.0);
    CONFIG_VARIABLE(SwingSharpness, 1.75);
    CONFIG_VARIABLE(SwingStrengthThreshold, 20.0);
    CONFIG_VARIABLE(Transition1Degrees, 45.0);
    CONFIG_VARIABLE(Transition2Degrees, 160.0);
    CONFIG_VARIABLE(MaxSwingVolume, 3.0);
  };

  int  Version;
  float SwingSensitivity;
  float MaximumHumDucking;
  float SwingSharpness;
  float SwingStrengthThreshold;
  float Transition1Degrees;
  float Transition2Degrees;
  float MaxSwingVolume;
};

SmoothSwingConfigFile smooth_swing_config;

// SmoothSwing V1
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
    STDOUT.println("Activating looped swing sounds");
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
      STDOUT.println("Looped swings cannot allocate wav player.");
    }
    high_ = GetFreeWavPlayer();
    if (high_) {
      high_->set_volume_now(0);
      high_->PlayOnce(&swingh);
      high_->PlayLoop(&swingh);
    } else {
      STDOUT.println("Looped swings cannot allocate wav player.");
    }
#if 0
    STDOUT.print("UNITS: ");
    STDOUT.print(WhatUnit(low_));
    STDOUT.print(",");
    STDOUT.println(WhatUnit(high_));
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
      STDOUT.print("S:");
      STDOUT.print(speed);
      STDOUT.print(" T:");
      STDOUT.print(t);
      STDOUT.print(" s:");
      STDOUT.print(s);
      STDOUT.print(" c:");
      STDOUT.print(c);
      STDOUT.print(" blend:");
      STDOUT.print(blend);
      STDOUT.print(" vol:");
      STDOUT.print(vol);
      STDOUT.print(" hi:");
      STDOUT.print(high);
      STDOUT.print(" lo:");
      STDOUT.print(low);
      STDOUT.print(" hum:");
      STDOUT.println(hum);
    }
  }
};

LoopedSwingWrapper looped_swing_wrapper;

template<class T, int N>
class BoxFilter {
public:
  T filter(const T& v) {
    data[pos] = v;
    pos++;
    if (pos == N) pos = 0;

    T ret = data[0];
    for (int i = 1; i < N; i++) {
      ret += data[i];
    }
    return ret / N;
  }

  T data[N];
  int pos = 0;
};

// SmoothSwing V2, based on Thexter's excellent work.
// For more details, see:
// http://therebelarmory.com/thread/9138/smoothswing-v2-algorithm-description
//
class SmoothSwingV2 : public SaberBasePassThrough {
public:
  SmoothSwingV2() : SaberBasePassThrough() {}

  void Activate(SaberBase* base_font) {
    STDOUT.println("Activating SmoothSwing V2");
    SetDelegate(base_font);
    if (swingl.files_found() != swingh.files_found()) {
      STDOUT.println("Warning, swingl and swingh should have the same number of files.");
    }
    swings_ = min(swingl.files_found(), swingh.files_found());
  }

  void Deactivate() {
    SetDelegate(NULL);
  }

  void Swap() {
    Data C = A;
    A = B;
    B = C;
  }

  // Should only be done when the volume is near zero.
  void PickRandomSwing() {
    if (!on_) return;
    int swing = random(swings_);
    float start = millis() / 1000.0;
    A.Stop();
    B.Stop();
    swingl.Select(swing);
    swingh.Select(swing);
    A.Play(&swingl, start);
    B.Play(&swingh, start);
    if (random(2)) Swap();
    float t1_offset = random(1000) / 1000.0 * 50 + 10;
    A.SetTransition(t1_offset, smooth_swing_config.Transition1Degrees);
    B.SetTransition(t1_offset + 180.0,
      smooth_swing_config.Transition2Degrees);
  }

  void SB_On() override {
    on_ = true;
    // Starts hum, etc.
    delegate_->SB_On();
    PickRandomSwing();
    if (!A.player || !B.player) {
      STDOUT.println("SmoothSwing V2 cannot allocate wav player.");
    }
  }
  void SB_Off() override {
    on_ = false;
    A.Off();
    B.Off();
    delegate_->SB_Off();
  }

  enum class SwingState {
    OFF, // waiting for swing to start
    ON,  // swinging
    OUT, // Waiting for sound to fade out
  };

  void SB_Motion(const Vec3& raw_gyro) override {
    Vec3 gyro = gyro_filter_.filter(raw_gyro);
    // degrees per second
    // May not need to smooth gyro since volume is smoothed.
    float speed = sqrt(gyro.z * gyro.z + gyro.y * gyro.y);
    uint32_t t = micros();
    uint32_t delta = t - last_micros_;
    if (delta > 1000000) delta = 1;
    last_micros_ = t;
    float hum_volume = 1.0;
    
    switch (state_) {
      case SwingState::OFF:
        if (speed < smooth_swing_config.SwingStrengthThreshold) {
#if 1
          if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
            STDOUT.print("speed: ");
            STDOUT.println(speed);
          }
#endif
          break;
        }
        state_ = SwingState::ON;
        
      case SwingState::ON:
        if (speed >= smooth_swing_config.SwingStrengthThreshold * 0.9) {
          float swing_strength =
            min(1.0, speed / smooth_swing_config.SwingSensitivity);
          A.rotate(-speed * delta / 1000000.0);
          // If the current transition is done, switch A & B,
          // and set the next transition to be 180 degrees from the one
          // that is done.
          while (A.end() < 0.0) {
            B.midpoint = A.midpoint + 180.0;
	    Swap();
          }
          float mixab = 0.0;
          if (A.begin() < 0.0)
            mixab = clamp(- A.begin() / A.width, 0.0, 1.0);

          float mixhum =
            pow(swing_strength, smooth_swing_config.SwingSharpness);

          hum_volume =
            1.0 - mixhum * smooth_swing_config.MaximumHumDucking / 100.0;

          mixhum *= smooth_swing_config.MaxSwingVolume;

          if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
            STDOUT.print("speed: ");
            STDOUT.print(speed);
            STDOUT.print(" R: ");
            STDOUT.print(-speed * delta / 1000000.0);
            STDOUT.print(" MP: ");
            STDOUT.print(A.midpoint);
            STDOUT.print(" B: ");
            STDOUT.print(A.begin());
            STDOUT.print(" E: ");
            STDOUT.print(A.end());
            STDOUT.print("  mixhum: ");
            STDOUT.print(mixhum);
            STDOUT.print("  mixab: ");
            STDOUT.print(mixab);
            STDOUT.print("  hum_volume: ");
            STDOUT.println(hum_volume);
          }
          A.set_volume(mixhum * mixab);
          B.set_volume(mixhum * (1.0 - mixab));
          break;
        }
        A.set_volume(0);
        B.set_volume(0);
        state_ = SwingState::OUT;

      case SwingState::OUT:
        if (!A.isOff() || !B.isOff()) {
          if (monitor.ShouldPrint(Monitoring::MonitorSwings)) {
            Serial.println("Waiting for volume = 0");
          }
        }
        PickRandomSwing();
        state_ = SwingState::OFF;
    }
    // Must always set hum volume, or fade-out doesn't work.
    delegate_->SetHumVolume(hum_volume);
  }

private:
  struct Data {
    void set_volume(float v) {
      if (player) player->set_volume(v);
    }
    void Play(Effect* effect, float start = 0.0) {
      if (!player) {
	player = GetFreeWavPlayer();
	if (!player) return;
      }
      player->set_volume(0.0);
      player->PlayOnce(effect, start);
      player->PlayLoop(effect);
    }
    void Off() {
      if (!player) return;
      player->set_fade_time(0.2);  // Read from config file?
      player->FadeAndStop();
      player = NULL;
    }
    void Stop() {
      if (!player) return;
      player->Stop();
    }
    bool isOff() {
      if (!player) return true;
      return player->isOff();
    }
    void SetTransition(float mp, float w) {
      midpoint = mp;
      width = w;
    }
    float begin() const { return midpoint - width / 2; }
    float end() const { return midpoint + width / 2; }
    void rotate(float degrees) {
      midpoint += degrees;
    }
    BufferedWavPlayer *player = nullptr;
    float midpoint;
    float width;
  };
  Data A;
  Data B;

  bool on_ = false;;
  BoxFilter<Vec3, 3> gyro_filter_;
  int swings_;
  uint32_t last_micros_;
  SwingState state_ = SwingState::OFF;;
};

SmoothSwingV2 smooth_swing_v2;

#endif  // ENABLE_AUDIO

class BatteryMonitor : Looper, CommandParser {
public:
  const char* name() override { return "BatteryMonitor"; }
  float battery_now() {
    // This is the volts on the battery monitor pin.
    float volts = 3.3 * analogRead(batteryLevelPin) / 1024.0;
#ifdef V2
    float pulldown = 220000;  // External pulldown
    float pullup = 2000000;  // External pullup
#else
    float pulldown = 33000;  // Internal pulldown is 33kOhm
    float pullup = BATTERY_PULLUP_OHMS;  // External pullup
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
  float battery_percent() {
    // Energy is roughly proportional to voltage squared.
    float v = battery();
    float min_v = 3.0;
    float max_v = 4.2;
    return 100.0 * (v * v - min_v * min_v) / (max_v * max_v - min_v * min_v);
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
      STDOUT.print("Battery voltage: ");
      STDOUT.println(battery());
      last_print_millis_ = millis();
    }
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, "battery_voltage")) {
      STDOUT.println(battery());
      return true;
    }
    if (!strcmp(cmd, "batt") || !strcmp(cmd, "battery")) {
      STDOUT.print("Battery voltage: ");
      float v = battery();
      STDOUT.println(v);
#ifdef ENABLE_AUDIO
      talkie.SayDigit((int)floor(v));
      talkie.Say(spPOINT);
      talkie.SayDigit(((int)floor(v * 10)) % 10);
      talkie.SayDigit(((int)floor(v * 100)) % 10);
      talkie.Say(spVOLTS);
#endif
      return true;
    }
    return false;
  }
  void Help() override {
    STDOUT.println(" batt[ery[_voltage]] - show battery voltage");
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
class Color8 {
  public:
  Color8() : r(0), g(0), b(0) {}
  Color8(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
  // x = 0..256
  Color8 mix(const Color8& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color8( ((256-x) * r + x * other.r) >> 8,
                   ((256-x) * g + x * other.g) >> 8,
                   ((256-x) * b + x * other.b) >> 8);
  }
  // x = 0..16384
  Color8 mix2(const Color8& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color8( ((16384-x) * r + x * other.r) >> 14,
                   ((16384-x) * g + x * other.g) >> 14,
                   ((16384-x) * b + x * other.b) >> 14);
  }
  uint8_t select(const Color8& other) const {
    uint8_t ret = 255;
    if (other.r) ret = min(ret, r * 255 / other.r);
    if (other.g) ret = min(ret, g * 255 / other.g);
    if (other.b) ret = min(ret, b * 255 / other.b);
    return ret;
  }
  
  uint8_t r, g, b;
};

class Color16 {
  public:
  Color16() : r(0), g(0), b(0) {}
  Color16(const Color8& c) : r(c.r << 8), g(c.g << 8), b(c.b << 8) {}
  Color16(uint16_t r_, uint16_t g_, uint16_t b_) : r(r_), g(g_), b(b_) {}
  // x = 0..256
  Color16 mix(const Color16& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color16( ((256-x) * r + x * other.r) >> 8,
                    ((256-x) * g + x * other.g) >> 8,
                    ((256-x) * b + x * other.b) >> 8);
  }
  // x = 0..16384
  Color16 mix2(const Color16& other, int x) const {
    // Wonder if there is an instruction for this?
    return Color16( ((16384-x) * r + x * other.r) >> 14,
                    ((16384-x) * g + x * other.g) >> 14,
                    ((16384-x) * b + x * other.b) >> 14);
  }
  uint16_t select(const Color16& other) const {
    uint32_t ret = 65535;
    uint32_t tmp = 65535;
    if (other.r) ret = min(ret, r * tmp / other.r);
    if (other.g) ret = min(ret, g * tmp / other.g);
    if (other.b) ret = min(ret, b * tmp / other.b);
    return ret;
  }

  Color8 dither(int n) const {
    return Color8(min(255, (r+n) >> 8), min(255, (g+n) >> 8), min(255, (b+n) >> 8));
  }

  uint16_t r, g, b;
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

class MonopodWS2811 {
public:
  void begin(uint32_t numPerStrip,
              void *frameBuf,
              uint8_t config,
              int pin);
  void setPixel(uint32_t num, Color8 color) {
    drawBuffer[num] = color;
  }
  Color8 getPixel(uint32_t num) {
    return drawBuffer[num];
  }

  void show();
  int busy(void);
  int in_progress(void);

  int numPixels(void) {
    return stripLen;
  }

  static Color8 drawBuffer[maxLedsPerStrip];
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

Color8 MonopodWS2811::drawBuffer[maxLedsPerStrip];
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
      STDOUT.println("Invalid monopodws pin!");
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

    case WS2811_ACTUALLY_800kHz:
      frequency = 800000;
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

void CopyOut(int params, class Color8* inbuf, void* frameBuffer, int num) {
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
  virtual void set(int led, Color16 c) = 0;

  // Bypasses battery voltage based PWM, intended to be used for
  // brief flashes only.
  virtual void set_overdrive(int led, Color16 c) { set(led, c); }

  // Returns true when a clash occurs.
  // Returns true only once per clash.
  virtual bool clash() = 0;

  // Clear blade colors.
  virtual void clear() {
    for (int i = 0; i < num_leds(); i++) set(i, Color16());
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

class BladeWrapper : public BladeBase {
public:
  int num_leds() const override { return blade_->num_leds(); }
  bool is_on() const override { return blade_->is_on(); }
  void set(int led, Color16 c) override { return blade_->set(led, c); }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(led, c);
  }
  bool clash() override { return blade_->clash(); }
  void clear() override { return blade_->clear(); }
  void allow_disable() override { blade_->allow_disable(); }
  void Activate() override { blade_->Activate(); }

  BladeBase* blade_;
};


// Charging blade style.
// Slowly pulsating battery indicator.
class StyleCharging : public BladeStyle {
public:
  void activate() override {
    STDOUT.println("Charging Style");
  }
  void run(BladeBase *blade) override {
    int black_mix = 128 + 100 * sin(millis() / 500.0);
    float volts = battery_monitor.battery();
    Color8 colors[] = {
      Color8(0,255,0),   // Green > 4.0
      Color8(0,0,255),
      Color8(255,128,0),
      Color8(255,0,0),
      Color8(255,0,0) 
    };
    float x = (4.0 - volts) * 2.0;
    int i = floor(x);
    i = clampi32(i, 0, NELEM(colors) - 2);
    // Blend colors over 0.1 volts.
    int blend = (x - i) * 10 * 255;
    blend = clampi32(blend, 0, 255);
    Color8 c = colors[i].mix(colors[i + 1], blend);
    c = c.mix(Color8(), black_mix);
    int num_leds = blade->num_leds();

    float min_volts = 2.7;
    float max_volts = 4.2;
    float pos = (volts - min_volts) * num_leds / (max_volts - min_volts);
    int p = pos * 32;
    for (int i = 0; i < num_leds; i++) {
      blade->set(i, Color16(Color8().mix(c, max(0, 256 - abs(p - i * 32)))));
    }
  };

  bool NoOnOff() override { return false; }
};

// No need to templetize this one, as there are no arguments.
StyleCharging style_charging;

struct FireConfig {
  FireConfig(int b, int r, int c) : intensity_base(b),
                                     intensity_rand(r),
                                     cooling(c) {}
  int intensity_base;
  int intensity_rand;
  int cooling;
};

// Fire-style
template<int BLADE_NUM>
class StyleFire : public BladeStyle {
public:
  StyleFire(Color8 c1,
             Color8 c2,
             uint32_t delay,
             uint32_t speed,
             FireConfig normal,
             FireConfig clash,
             FireConfig lockup) :
    c1_(c1), c2_(c2),
    delay_(delay),
    speed_(speed),
    normal_(normal),
    clash_(clash),
    lockup_(lockup) {
  }
  enum OnState {
    STATE_OFF = 0,
    STATE_ACTIVATING,
    STATE_ON,
  };
  void activate() override {
    STDOUT.println("Fire Style");
    for (size_t i = 0; i < NELEM(heat_); i++) heat_[i] = 0;
  }
  bool On(BladeBase* blade) {
    if (!blade->is_on()) {
      state_ = STATE_OFF;
      return false;
    }
    switch (state_) {
      default:
         state_ = STATE_ACTIVATING;
         on_time_ = millis();
      case STATE_ACTIVATING:
         if (millis() - on_time_ < delay_) return false;
         state_ = STATE_ON;
      case STATE_ON:
         return true;
    }
  }
  void run(BladeBase* blade) override {
    uint32_t m = millis();
    int num_leds = blade->num_leds();
    if (m - last_update_ >= 10) {
      last_update_ = m;

      FireConfig config(0,0,0);
      if (blade->clash()) {
         config = clash_;
      } else if (On(blade)) {
        if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
          config = normal_;
        } else {
          config = lockup_;
        }
      } else {
         config = normal_;
         config.intensity_base = 0;
         config.intensity_rand = 0;
      }
      // Note heat_[0] is tip of blade
      for (int i = 0; i < speed_; i++) {
         heat_[num_leds + i] = config.intensity_base +
           random(random(random(config.intensity_rand)));
      }
      for (int i = 0; i < num_leds; i++) {
         int x = (heat_[i+speed_-1] * 3  +
                  heat_[i+speed_] * 10 +
                  heat_[i+speed_+1] * 3) >> 4;
         heat_[i] = clampi32(x - random(config.cooling), 0, 65535);
      }
    }
    bool zero = true;
    for (int i = 0; i < num_leds; i++) {
      int h = heat_[num_leds - 1 - i];
      Color8 c;
      if (h < 256) {
         c = Color8().mix(c1_, h);
      } else if (h < 512) {
         c = c1_.mix(c2_, h - 256);
      } else if (h < 768) {
         c = c2_.mix(Color8(255,255,255), h - 512);
      } else {
         c = Color8(255,255,255);
      }
      if (h) zero = false;
      blade->set(i, c);
    }
    if (zero) blade->allow_disable();
  }

private:
  static uint32_t last_update_;
  static unsigned short heat_[maxLedsPerStrip + 13];
  Color8 c1_, c2_;
  uint32_t delay_;
  OnState state_;
  uint32_t on_time_;
  int speed_;
  FireConfig normal_;
  FireConfig clash_;
  FireConfig lockup_;
};

template<int BLADE_NUM>
uint32_t StyleFire<BLADE_NUM>::last_update_ = 0;

template<int BLADE_NUM>
unsigned short StyleFire<BLADE_NUM>::heat_[maxLedsPerStrip + 13];

// If you have multiple blades, make sure to use a different BLADE_NUM
// for each blade.
template<class COLOR1, class COLOR2,
          int BLADE_NUM=0, int DELAY=0, int SPEED=2,
          int NORM_INT_BASE = 0, int NORM_INT_RAND=2000, int NORM_COOLING = 5,
          int CLSH_INT_BASE = 3000, int CLSH_INT_RAND=0, int CLSH_COOLING = 0,
          int LOCK_INT_BASE = 0, int LOCK_INT_RAND=5000, int LOCK_COOLING = 10>
class BladeStyle *StyleFirePtr() {
  static StyleFire<BLADE_NUM> style(
    COLOR1::color().dither(0), COLOR2::color().dither(0),
    DELAY, SPEED,
    FireConfig(NORM_INT_BASE, NORM_INT_RAND, NORM_COOLING),
    FireConfig(CLSH_INT_BASE, CLSH_INT_RAND, CLSH_COOLING),
    FireConfig(LOCK_INT_BASE, LOCK_INT_RAND, LOCK_COOLING));
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
  Color16 c;
  bool overdrive;
};
// Simple solid color.
template<int R, int G, int B>
class Rgb {
public:
  static Color16 color() { return Color16(Color8(R,G,B)); }
  void run(BladeBase* base) {}
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.c = color();
    ret.overdrive = false;
    return ret;
  }
};

// Simple solid color with 16-bit precision.
template<int R, int G, int B>
class Rgb16 {
public:
  static Color16 color() { return Color16(R, G, B); }
  void run(BladeBase* base) {}
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.c = color();
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
    a.c = a.c.mix2(b.c, led * 16384 / num_leds_);
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
#ifdef ENABLE_AUDIO    
    mix_ = clampi32(dynamic_mixer.last_sum() >> 4, 0, 255);
#else
    mix_ = 0;
#endif
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

// Randomly selects between A and B, but keeps nearby
// pixels looking similar.
template<class A, class B, int grade>
class BrownNoiseFlicker {
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
    mix_ = clampi32(mix_ + random(grade * 2 + 1) - grade, 0, 255);
    return a;
  }
private:
  A a_;
  B b_;
  int mix_;
};

// Makes a random "hump" which is about 2xHUMP_WIDTH leds wide.
template<class A, class B, int HUMP_WIDTH>
class HumpFlicker {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    int num_leds_ = blade->num_leds();
    pos_ = random(num_leds_);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    int mix_ = clampi32(abs(led - pos_) * 255 / HUMP_WIDTH, 0, 255);
    a.c = a.c.mix(b.c, mix_);
    return a;
  }
private:
  A a_;
  B b_;
  int pos_;
};

// Basic RGB rainbow.
class Rainbow {
public:
  void run(BladeBase* base) {
    m = millis();
  }
  OverDriveColor getColor(int led) {
    Color16 c(max(0, (sin_table[((m * 3 + led * 50)) & 0x3ff] << 2)),
              max(0, (sin_table[((m * 3 + led * 50 + 1024 / 3)) & 0x3ff] << 2)),
              max(0, (sin_table[((m * 3 + led * 50 + 1024 * 2 / 3)) & 0x3ff] << 2)));
    OverDriveColor ret;
    ret.c = c;
    ret.overdrive = false;
    return ret;
  }
private:
  uint32_t m;
};

class Range {
public:
  uint32_t start;
  uint32_t end;
  Range(uint32_t s, uint32_t e) : start(s), end(e) {}
  uint32_t size() const {
    if (start >= end) return 0;
    return end - start;
  }
  Range operator&(const Range& other) {
    return Range(max(start, other.start),
                 min(end, other.end));
  }
};

// This is intended for a small ring of neopixels
// A section of the ring is lit at the specified color
// and rotates at the specified speed. The size of the
// lit up section is defined by "percentage".
template<class COLOR, int percentage, int rpm,
         class ON_COLOR = COLOR,
         int on_percentage = percentage,
         int on_rpm = rpm,
         int fade_time_millis = 1>
class ColorCycle {
public:
  void run(BladeBase* base) {
    c_.run(base);
    on_c_.run(base);

    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;

    float fade_delta = delta / 1000.0 / fade_time_millis;
    if (!base->is_on()) fade_delta = - fade_delta;
    fade_ = max(0.0, min(1.0, fade_ + fade_delta));

    float current_rpm = rpm * (1 - fade_) + on_rpm * fade_;
    float current_percentage =
       percentage * (1 - fade_) + on_percentage * fade_;
    fade_int_ = (int)(16384 * fade_);
    pos_ = fract(pos_ + delta / 60000000.0 * current_rpm);
    num_leds_ = base->num_leds() * 16384;
    start_ = pos_ * num_leds_;
    end_ = fract(pos_ + current_percentage / 100.0) * num_leds_;
  }
  OverDriveColor getColor(int led) {
    led *= 16384;
    Range led_range(led, led + 16384);
    int black_mix = 0;
    if (start_ < end_) {
      black_mix = (Range(start_, end_) & led_range).size();
    } else {
      black_mix = (Range(0, end_) & led_range).size() +
                  (Range(start_, num_leds_) & led_range).size();
    }
    OverDriveColor c = c_.getColor(led);
    OverDriveColor on_c = on_c_.getColor(led);
    c.c = c.c.mix2(on_c.c, fade_int_);
    c.c = Color16().mix2(c.c, black_mix);
    return c;
  }
private:
  float fade_ = 0.0;
  int fade_int_;
  float pos_ = 0.0;
  uint32_t start_;
  uint32_t end_;
  uint32_t num_leds_;
  COLOR c_;
  ON_COLOR on_c_;
  uint32_t last_micros_;
};

// Cylon/Knight Rider effect, a section of the strip is
// lit up and moves back and forth. Speed, color and fraction
// illuminated can be configured separately for on and off
// states.
template<class COLOR, int percentage, int rpm,
         class ON_COLOR = COLOR,
         int on_percentage = percentage,
         int on_rpm = rpm,
         int fade_time_millis = 1>
class Cylon {
public:
  void run(BladeBase* base) {
    c_.run(base);
    on_c_.run(base);

    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;

    float fade_delta = delta / 1000.0 / fade_time_millis;
    if (!base->is_on()) fade_delta = - fade_delta;
    fade_ = max(0.0, min(1.0, fade_ + fade_delta));

    float current_rpm = rpm * (1 - fade_) + on_rpm * fade_;
    float current_percentage =
       percentage * (1 - fade_) + on_percentage * fade_;
    fade_int_ = (int)(16384 * fade_);
    pos_ = fract(pos_ + delta / 60000000.0 * current_rpm);
    float fraction = current_percentage / 100.0;
    float pos = 0.5 + sin(pos_ * M_PI * 2) * (1.0 - fraction) / 2.0 - fraction / 2.0;
    num_leds_ = base->num_leds() * 16384;
    start_ = pos_ * num_leds_;
    end_ = (pos + fraction) * num_leds_;
  }
  OverDriveColor getColor(int led) {
    led *= 16384;
    Range led_range(led, led + 16384);
    int black_mix = 0;
    if (start_ < end_) {
      black_mix = (Range(start_, end_) & led_range).size();
    } else {
      black_mix = (Range(0, end_) & led_range).size() +
                  (Range(start_, num_leds_) & led_range).size();
    }
    OverDriveColor c = c_.getColor(led);
    OverDriveColor on_c = on_c_.getColor(led);
    c.c = c.c.mix2(on_c.c, fade_int_);
    c.c = Color16().mix2(c.c, black_mix);
    return c;
  }
private:
  float fade_ = 0.0;
  int fade_int_;
  float pos_ = 0.0;
  uint32_t start_;
  uint32_t end_;
  uint32_t num_leds_;
  COLOR c_;
  ON_COLOR on_c_;
  uint32_t last_micros_;
};

// This class renders BASE as normal, but delays ignition by
// the specified number of milliseconds. Intended for kylo-style
// quillions.
template<int delay_millis, class BASE>
class IgnitionDelay : public BladeWrapper {
public:
  void run(BladeBase* base) {
    blade_ = base;
    if (base->is_on()) {
      if (!waiting_) {
        waiting_ = true;
        wait_start_time_ = millis();
      }
      uint32_t waited = millis() - wait_start_time_;
      if (waited > delay_millis) {
        is_on_ = true;
        wait_start_time_ = millis() - delay_millis - 1;
      }
    } else {
      waiting_ = false;
      is_on_ = false;
    }
    base_.run(this);
  }
  OverDriveColor getColor(int led) { return base_.getColor(led); }
  bool is_on() const override { return is_on_; }
private:
  bool is_on_ = false;
  bool waiting_ = false;
  uint32_t wait_start_time_;
  BASE base_;
};

template<class COLOR1, class COLOR2, int pulse_millis>
class Pulsing {
public:
  void run(BladeBase* base) {
    c1_.run(base);
    c2_.run(base);
    uint32_t now = micros();
    uint32_t delta = now - last_micros_;
    last_micros_ = now;
    pos_ = fract(pos_ + delta / (1000.0 * pulse_millis));
    mix_ = (sin_table[(int)floor(pos_ * 0x400)] + 16384) >> 1;
  }

  OverDriveColor getColor(int led) {
    OverDriveColor c1 = c1_.getColor(led);
    OverDriveColor c2 = c2_.getColor(led);
    c1.c = c1.c.mix2(c2.c, mix_);
    return c1;
  }

private:
  COLOR1 c1_;
  COLOR2 c2_;
  int mix_;
  uint32_t last_micros_;
  float pos_ = 0.0;
};

// Very fast Red, Green, Blue cycle, result should essentially be white
// until you start swinging it around.
class RGBCycle {
public:
  void run(BladeBase* base) {
    if (millis() == millis_at_last_call_)
      return;
    millis_at_last_call_ = millis();
    n_++;
    if (n_ >= 3) n_ = 0;
  }  
  OverDriveColor getColor(int led) {
    OverDriveColor ret;
    ret.overdrive = false;
    switch (n_) {
      case 0: ret.c = Color16(65535, 0, 0); break;
      case 1: ret.c = Color16(0, 65535, 0); break;
      case 2: ret.c = Color16(0, 0, 65535); break;
    }
    return ret;
  }
private:
  uint32_t millis_at_last_call_;
  uint8_t n_;
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
// Also handles "Drag" effect.
template<class BASE, class LOCKUP>
class Lockup {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    lockup_.run(blade);
    int num_leds = blade->num_leds();
    if (num_leds > 6) {
      drag_cutoff_ = num_leds * 98 / 100;
    } else {
      drag_cutoff_ = 0;
    }
  }
  OverDriveColor getColor(int led) {
    // Good luck desciphering this one...
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_DRAG:
        if (led >= drag_cutoff_) {
          case SaberBase::LOCKUP_NORMAL:
            return lockup_.getColor(led);
        } else {
          case SaberBase::LOCKUP_NONE:
            break;
        }
    }
    return base_.getColor(led);
  }
private:
  int drag_cutoff_;
  BASE base_;
  LOCKUP lockup_;
};

template<class BASE, class BLAST>
class Blast {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    blast_.run(blade);
    num_leds_ = blade->num_leds();
    num_blasts_ = SaberBase::NumBlasts();
  }
  OverDriveColor getColor(int led) {
    OverDriveColor base = base_.getColor(led);
    if (num_blasts_ == 0) return base;
    float mix = 0.0;
    for (size_t i = 0; i < num_blasts_; i++) {
      // TODO(hubbe): Use sin_table and avoid floats
      const SaberBase::Blast b = SaberBase::getBlast(i);
      float x = (b.location - led/(float)num_leds_) * 30.0;
      uint32_t T = micros() - b.start_micros;
      float t = 0.5 + T / 200000.0;
      if (x == 0.0) {
        mix += 1.0f / (t*t);
      } else {
        mix += sinf(x / (t*t)) / x;
      }
    }
    OverDriveColor blast = blast_.getColor(led);
    base.c = base.c.mix(blast.c, min(mix, 1.0) * 256);
    return base;
  }
private:
  BASE base_;
  BLAST blast_;
  int num_leds_;
  size_t num_blasts_;
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

template<class T, class U>
struct is_same_type { static const bool value = false; };
 
template<class T>
struct is_same_type<T, T> { static const bool value = true; };


template<class T, int OUT_MILLIS, int IN_MILLIS, class OFF_COLOR=Rgb<0,0,0> >
class InOutHelper {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    off_color_.run(blade);
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
      if (is_same_type<OFF_COLOR, Rgb<0,0,0> >::value)
        if (extension == 0.0)
          blade->allow_disable();
      extension -= delta / (IN_MILLIS * 1000.0);
      extension = max(extension, 0.0f);
    }
    thres = extension * blade->num_leds() * 256;
  }
  OverDriveColor getColor(int led) {
    int black_mix = clampi32(thres - led * 256, 0, 255);
    OverDriveColor ret = base_.getColor(led);
    OverDriveColor off_color  = off_color_.getColor(led);
    ret.c = off_color.c.mix(ret.c, black_mix);
    return ret;
  }
private:
  T base_;
  OFF_COLOR off_color_;
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
    ret.c = Color16().mix(ret.c, black_mix);
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

// This macro has a problem with commas, please don't use it.
#define EASYBLADE(COLOR, CLASH_COLOR) \
  SimpleClash<Lockup<Blast<COLOR, WHITE>, AudioFlicker<COLOR, WHITE> >, CLASH_COLOR>

// Use EasyBlade<COLOR, CLASH_COLOR> instead of EASYBLADE(COLOR, CLASH_COLOR)
template<class color, class clash_color, class lockup_flicker_color = WHITE>
using EasyBlade = SimpleClash<Lockup<Blast<color, WHITE>, AudioFlicker<color, lockup_flicker_color> >, clash_color>;

// The following functions are mostly for illustration.
// The templates above gives you more power and functionality.

// Arguments: color, clash color, turn-on/off time
template<class base_color,
          class clash_color,
          int out_millis,
          int in_millis,
         class lockup_flicker_color = WHITE,
         class blast_color = WHITE>
BladeStyle *StyleNormalPtr() {
  typedef AudioFlicker<base_color, lockup_flicker_color> AddFlicker;
  typedef Blast<base_color, blast_color> AddBlast;
  typedef Lockup<AddBlast, AddFlicker> AddLockup;
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
    STDOUT.println("POV Style");
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
    STDOUT.print("SLOPE: ");
    STDOUT.print(slope.x * 100.0);
    STDOUT.print(", ");
    STDOUT.print(slope.y * 100.0);
    STDOUT.print(", ");
    STDOUT.println(slope.z * 100.0);
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
    // STDOUT.print("F:");
    // STDOUT.println(fraction);
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
  Color8* data_;
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
  const char* name() override { return "WS2811_Blade"; }

  void Power(bool on) {
    power_->Power(on);
//    pinMode(bladePin, on ? OUTPUT : INPUT);
    powered_ = on;
    allow_disable_ = false;
  }

  // No need for a "deactivate", the blade stays active until
  // you take it out, which also cuts the power.
  void Activate() override {
    STDOUT.print("WS2811 Blade with ");
    STDOUT.print(num_leds_);
    STDOUT.println(" leds");
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
  void set(int led, Color16 c) override {
    monopodws.setPixel(led, c.dither(0));
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
         SB_Off();
         return true;
      }
    }
    return false;
  }

  void Help() override {
    STDOUT.println(" blade on/off - turn ws2811 blade on off");
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
#if NUM_BLADES > 1
        monopodws.begin(num_leds_, displayMemory, config_, pin_);
#endif
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
      // Bitbang on separate pins, need to lock anything.
      FastLED.show();
    }
  }

  // No need for a "deactivate", the blade stays active until
  // you take it out, which also cuts the power.
  void Activate() override {
    STDOUT.print("FASTLED Blade with ");
    STDOUT.print(num_leds_);
    STDOUT.println(" leds");
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
    ((Color8*)displayMemory)[led] = c;
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
         SB_Off();
         return true;
      }
    }
    return false;
  }

  void Help() override {
    STDOUT.println(" blade on/off - turn apa102 blade on off");
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
  // Given a color, return a the right PWM level (0-65535).
  virtual int PWM(Color16 c) = 0;

  // Same as PWM(), but ignores battery voltage.
  virtual int PWM_overdrive(Color16 c) = 0;
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
      STDOUT.print("AMPS = ");
      STDOUT.print(amps);
      STDOUT.print(" / ");
      STDOUT.print(LED::MaxAmps);
      STDOUT.print(" PWM = ");
      STDOUT.println(100.0 * LED::MaxAmps / amps);
    }
    if (amps <= LED::MaxAmps) {
      return 1.0f;
    }

    return LED::MaxAmps / amps;
  }
  int PWM(Color16 c) override {
    return c.select(Color16(Color8(LED::Red, LED::Green, LED::Blue))) * PWMMultiplier();
  }
  int PWM_overdrive(Color16 c) override {
    return c.select(Color16(Color8(LED::Red, LED::Green, LED::Blue)));
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
  void set(const Color16& c) {
    if (pin_ == -1) return;
    analogWrite(pin_, c_->PWM(c));
  }
  void set_overdrive(const Color16& c) {
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
  const char* name() override { return "Simple_Blade"; }

  void Activate() override {
    STDOUT.println("Simple Blade");
    analogWriteResolution(16);
    power_ = true;
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
  void set(int led, Color16 c) override {
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i].set(c);
  }

  void set_overdrive(int led, Color16 c) override {
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
    if (on_ || power_) *on = true;
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
    STDOUT.println(" blade on/off - turn simple blade on off");
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
  const char* name() override { return "String_Blade"; }

  void Activate() override {
    STDOUT.println("String Blade");
    analogWriteResolution(16);
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
  void set(int led, Color16 c) override {
    analogWrite(pin_[led], c_->PWM(c));
    if (led == 0) clash_pin_.set(c);
  }

  void set_overdrive(int led, Color16 c) override {
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
    STDOUT.println(" blade on/off - turn string blade on off");
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


class SubBladeWrapper : public BladeWrapper, BladeStyle {
public:
  int num_leds() const override { return num_leds_; }
  void set(int led, Color16 c) override {
    return blade_->set(led + offset_, c);
  }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(led + offset_, c);
  }
  void allow_disable() override { allow_disable_ = true; }
  void Activate() override {
    if (!offset_) BladeWrapper::Activate();
  }
  void clear() override {
    if (!offset_) BladeWrapper::clear();
  }
  void SetStyle(BladeStyle* style) {
    BladeWrapper::SetStyle(style);
    if (!offset_) blade_->SetStyle(this);
  }
  
  void SetupSubBlade(BladeBase* base, int offset, int num_leds) {
    blade_ = base;
    offset_ = offset;
    num_leds_ = num_leds;
  }

  void SetNext(SubBladeWrapper* next) {
    next_ = next;
  }

  // Bladestyle implementation
  virtual void activate() override {
    current_style_->activate();
  }
  virtual void deactivate() override {
    current_style_->deactivate();
  }
  virtual void run(BladeBase* blade) override {
    SubBladeWrapper* tmp = this;
    bool allow_disable = true;
    do {
      tmp->allow_disable_ = false;
      tmp->current_style_->run(tmp);
      allow_disable &= tmp->allow_disable_;
      tmp = tmp->next_;
    } while(tmp != this);
    if (allow_disable) blade_->allow_disable();
  }

 bool NoOnOff() override {
    SubBladeWrapper* tmp = this;
    do {
      if (tmp->current_style_->NoOnOff()) return true;
      tmp = tmp->next_;
    } while(tmp != this);
    return false;
  }

private:
  int num_leds_;
  int offset_;
  bool allow_disable_;
  SubBladeWrapper* next_;
};

// This let's you split a single chain of neopixels into multiple blades.
// Let's say you build saber with an 8-led PLI, a single led for a crystal chamber
// crystal chamber and 3 accent LEDs all hooked up as a single neopixel chain.
// The blades[] entry could then look like this:
// { 2000,
//   WS2811BladePtr<144, WS2811_ACTUALLY_800kHz | WS211_GRB>(),
//   SubBlade(0,  7, WS2811BladePtr<15, WS2811_580kHz>()),  // PLI
//   SubBlade(8,  8, NULL),  // crystal chamber
//   SubBlade(9, 11, NULL),  // accent leds
//   CONFIGARRAY(presets) }
//
// In the example above, NUM_BLADES must be 4, so you get to specify
// a style for each section of the string.
class BladeBase* SubBlade(int first_led, int last_led, BladeBase* blade) {
  static SubBladeWrapper* last = NULL;
  static SubBladeWrapper* first = NULL;
  if (first_led == 0)  {
    first = last = NULL;
    if (!blade) return NULL;
  } else {
    if (blade) return NULL;
    blade = first->blade_;
  }
  
  SubBladeWrapper* ret = new SubBladeWrapper();
  if (first) {
    ret->SetNext(last);
    first->SetNext(ret);
    last = ret;
  } else {
    ret->SetNext(ret);
    first = last = ret;
  }
  ret->SetupSubBlade(blade, first_led, last_led + 1 - first_led);
  return ret;
}

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
#if NUM_BLADES >= 5
  BladeStyle* style5;
#endif
#if NUM_BLADES >= 6
  BladeStyle* style6;
#endif
#if NUM_BLADES >= 7
  BladeStyle* style7;
#endif
#if NUM_BLADES >= 8
  BladeStyle* style8;
#endif

  const char* name;
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
#if NUM_BLADES >= 5
  BladeBase* blade5;
#endif
#if NUM_BLADES >= 6
  BladeBase* blade6;
#endif
#if NUM_BLADES >= 7
  BladeBase* blade7;
#endif
#if NUM_BLADES >= 8
  BladeBase* blade8;
#endif

  // Blade presets
  Preset* presets;
  size_t num_presets;
};

#define CONFIG_PRESETS
#include CONFIG_FILE
#undef CONFIG_PRESETS

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
  const char* name() override { return "Script"; }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    SLEEP(2000);
    CommandParser::DoParse("on", NULL);
    SLEEP(2000);
    CommandParser::DoParse("batt", NULL);
    SLEEP(2000);
    CommandParser::DoParse("play", "cantina.wav");
#if 0    
    while (true) {
      if (dac.isSilent()) {
        SLEEP(2000);
      } else {
        CommandParser::DoParse("clash", NULL);
        STDOUT.print("alloced: ");
        STDOUT.println(mallinfo().uordblks);
        SLEEP(100);
      }
    }
#endif    
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
  Saber() : CommandParser() {}
  const char* name() override { return "Saber"; }

  bool IsOn() const {
    return on_;
  }

  bool NeedsPower() {
    return on_ || current_preset_->style->NoOnOff();
  }

  void On() {
    if (on_) return;
    if (current_preset_->style->NoOnOff()) return;
    STDOUT.println("Ignition.");
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    delay(10);             // allow time to wake up

    on_ = true;
    SaberBase::DoOn();
  }

  void Off() {
    on_ = false;
    if (SaberBase::Lockup()) {
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      SaberBase::DoEndLockup();
    }
    SaberBase::DoOff();
  }

  uint32_t last_clash_ = 0;
  uint32_t clash_timeout_ = 100;
  void Clash() {
    // No clashes in lockup mode.
    if (SaberBase::Lockup()) return;
    // TODO: Pick clash randomly and/or based on strength of clash.
    uint32_t t = millis();
    if (t - last_clash_ < clash_timeout_) {
      last_clash_ = t; // Vibration cancellation
      return;
    }
    if (Event(BUTTON_NONE, EVENT_CLASH)) {
      clash_timeout_ = 400;  // For events, space clashes out more.
    } else {
      clash_timeout_ = 100;
      if (on_) SaberBase::DoClash();
    }
    last_clash_ = t;
  }

  bool chdir(const char* dir) {
    if (strlen(dir) > 1 && dir[strlen(dir)-1] == '/') {
      STDOUT.println("Directory must not end with slash.");
      return false;
    }
#ifdef ENABLE_AUDIO
    smooth_swing_v2.Deactivate();
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
        smooth_swing_config.ReadInCurrentDir("smoothsw.ini");
        switch (smooth_swing_config.Version) {
          case 1:
            looped_swing_wrapper.Activate(font);
            break;
          case 2:
            smooth_swing_v2.Activate(font);
            break;
        }
      }
    }
#endif
    return false;
  }

  // Select preset (font/style)
  void SetPreset(Preset* preset, bool announce) {
    if (announce) {
      if (preset->name) {
        SaberBase::DoMessage(preset->name);
      } else {
        char message[64];
        strcpy(message, "Preset: ");
        itoa(preset - current_config_->presets + 1,
             message + strlen(message), 10);
        strcat(message, "\n");
        strncat(message + strlen(message),
                preset->font, sizeof(message) - strlen(message));
        message[sizeof(message) - 1] = 0;
        SaberBase::DoMessage(message);
      }
    }

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
#if NUM_BLADES >= 5
    current_config_->blade5->SetStyle(preset->style5);
#endif
#if NUM_BLADES >= 6
    current_config_->blade6->SetStyle(preset->style6);
#endif
#if NUM_BLADES >= 7
    current_config_->blade7->SetStyle(preset->style7);
#endif
#if NUM_BLADES >= 8
    current_config_->blade8->SetStyle(preset->style8);
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
    SetPreset(tmp, true);
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
    SetPreset(tmp, true);
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
    STDOUT.print("ID: ");
    STDOUT.print(blade_id);
    STDOUT.print(" volts ");
    STDOUT.print(volts);
    STDOUT.print(" resistance= ");
    STDOUT.println(resistor);
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
    STDOUT.print("blade= ");
    STDOUT.println(best_config);
    current_config_ = blades + best_config;

#define ACTIVATE(BLADE) do {     \
    if (!BLADE) goto bad_blade;  \
    BLADE->Activate();           \
  } while(0);

    ACTIVATE(current_config_->blade);
#if NUM_BLADES >= 2
    ACTIVATE(current_config_->blade2)
#endif
#if NUM_BLADES >= 3
    ACTIVATE(current_config_->blade3);
#endif
#if NUM_BLADES >= 4
    ACTIVATE(current_config_->blade4);
#endif
#if NUM_BLADES >= 5
    ACTIVATE(current_config_->blade5);
#endif
#if NUM_BLADES >= 6
    ACTIVATE(current_config_->blade6);
#endif
#if NUM_BLADES >= 7
    ACTIVATE(current_config_->blade7);
#endif
#if NUM_BLADES >= 8
    ACTIVATE(current_config_->blade8);
#endif
    SetPreset(current_config_->presets, false);
    return;

   bad_blade:
    STDOUT.println("BAD BLADE");
#ifdef ENABLE_AUDIO
    talkie.Say(spABORT);
#endif    
  }


  void SB_Message(const char* text) override {
    STDOUT.print("DISPLAY: ");
    STDOUT.println(text);
  }

  float peak = 0.0;
  Vec3 at_peak;
  void SB_Accel(const Vec3& accel) override {
    float v = (accel_ - accel).len();
    // If we're spinning the saber, require a stronger acceleration
    // to activate the clash.
    if (v > CLASH_THRESHOLD_G + filtered_gyro_.len() / 200.0) {
      // Needs de-bouncing
      Clash();
    }
    if (v > peak) {
      peak = v;
      at_peak = accel_ - accel;
    }
    accel_ = accel;
    if (monitor.ShouldPrint(Monitoring::MonitorClash)) {
      STDOUT.print("ACCEL: ");
      STDOUT.print(accel.x);
      STDOUT.print(", ");
      STDOUT.print(accel.y);
      STDOUT.print(", ");
      STDOUT.print(accel.z);
      STDOUT.print(" peak ");
      STDOUT.print(at_peak.x);
      STDOUT.print(", ");
      STDOUT.print(at_peak.y);
      STDOUT.print(", ");
      STDOUT.print(at_peak.z);
      STDOUT.print(" (");
      STDOUT.print(sqrt(peak));
      STDOUT.println(")");
      peak = 0.0;
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
      STDOUT.print("Stroke: ");
      switch (strokes[NELEM(strokes)-1].type) {
        case TWIST_LEFT:
          STDOUT.print("TwistLeft");
          break;
        case TWIST_RIGHT:
          STDOUT.print("TwistRight");
          break;
        default: break;
      }
      STDOUT.print(" len=");
      STDOUT.print(strokes[NELEM(strokes)-1].length());
      STDOUT.print(" separation=");
      uint32_t separation =
        strokes[NELEM(strokes)-1].start_millis -
        strokes[NELEM(strokes)-2].end_millis;
      STDOUT.println(separation);
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
          STDOUT.println("TWIST");
          // We have a twisting gesture.
          Event(BUTTON_NONE, EVENT_TWIST);
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

  BoxFilter<Vec3, 5> gyro_filter_;
  Vec3 filtered_gyro_;
  void SB_Motion(const Vec3& gyro) override {
    filtered_gyro_ = gyro_filter_.filter(gyro);
    if (monitor.ShouldPrint(Monitoring::MonitorGyro)) {
      // Got gyro data
      STDOUT.print("GYRO: ");
      STDOUT.print(gyro.x);
      STDOUT.print(", ");
      STDOUT.print(gyro.y);
      STDOUT.print(", ");
      STDOUT.println(gyro.z);
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
  bool pointing_down_ = false;
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
        STDOUT.println("No available WAV players.");
      }
    }
#else
    STDOUT.println("Audio disabled.");
#endif
  }

  bool aux_on_ = true;
  uint32_t last_beep_;

  void Loop() override {
    if (battery_monitor.low()) {
      if (current_preset_->style != &style_charging) {
        if (on_) {
          STDOUT.print("Battery low, turning off: ");
          STDOUT.println(battery_monitor.battery());
          Off();
        } else if (millis() - last_beep_ > 5000) {
          STDOUT.println("Battery low beep");
#ifdef ENABLE_AUDIO
          // TODO: allow this to be replaced with WAV file
          talkie.Say(spLOW);
          talkie.Say(spPOWER);
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
  }

  void PrintButton(uint32_t b) {
    if (b & BUTTON_POWER) STDOUT.print("Power");
    if (b & BUTTON_AUX) STDOUT.print("Aux");
    if (b & BUTTON_AUX2) STDOUT.print("Aux2");
    if (b & BUTTON_UP) STDOUT.print("Up");
    if (b & BUTTON_DOWN) STDOUT.print("Down");
    if (b & BUTTON_LEFT) STDOUT.print("Left");
    if (b & BUTTON_RIGHT) STDOUT.print("Right");
    if (b & BUTTON_SELECT) STDOUT.print("Select");
    if (b & MODE_ON) STDOUT.print("On");
  }

  void PrintEvent(EVENT e) {
    switch (e) {
      case EVENT_NONE: STDOUT.print("None"); break;
      case EVENT_PRESSED: STDOUT.print("Pressed"); break;
      case EVENT_RELEASED: STDOUT.print("Released"); break;
      case EVENT_CLICK_SHORT: STDOUT.print("Shortclick"); break;
      case EVENT_CLICK_LONG: STDOUT.print("Longclick"); break;
      case EVENT_DOUBLE_CLICK: STDOUT.print("Doubleclick"); break;
      case EVENT_LATCH_ON: STDOUT.print("On"); break;
      case EVENT_LATCH_OFF: STDOUT.print("Off"); break;
      case EVENT_STAB: STDOUT.print("Stab"); break;
      case EVENT_SWING: STDOUT.print("Swing"); break;
      case EVENT_SHAKE: STDOUT.print("Shake"); break;
      case EVENT_TWIST: STDOUT.print("Twist"); break;
      case EVENT_CLASH: STDOUT.print("Clash"); break;
    }
  }

public:
  bool Event(enum BUTTON button, EVENT event) {
    STDOUT.print("EVENT: ");
    if (button) {
      PrintButton(button);
      STDOUT.print("-");
    }
    PrintEvent(event);
    if (current_modifiers & ~button) {
      STDOUT.print(" mods ");
      PrintButton(current_modifiers);
    }
    if (on_) STDOUT.print(" ON");
    STDOUT.println("");

#define EVENTID(BUTTON, EVENT, MODIFIERS) (((EVENT) << 24) | ((BUTTON) << 12) | ((MODIFIERS) & ~(BUTTON)))
    if (on_ && aux_on_) {
      if (button == BUTTON_POWER) button = BUTTON_AUX;
      if (button == BUTTON_AUX) button = BUTTON_POWER;
    }
    
    bool handled = true;
    switch (EVENTID(button, event, current_modifiers | (on_ ? MODE_ON : MODE_OFF))) {
      default:
        handled = false;
        break;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        if (accel_.x < -0.15) {
          pointing_down_ = true;
        } else {
          pointing_down_ = false;
        }
        break;

#if NUM_BUTTONS == 0
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
#endif
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
        aux_on_ = false;
        On();
        break;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF):
#ifdef DUAL_POWER_BUTTONS
        aux_on_ = true;
        On();
#else
        next_preset();
#endif
        break;
         
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_OFF, MODE_ON):
#if NUM_BUTTONS == 0
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
#endif
        Off();
        break;

      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_OFF):
        SaberBase::DoSpeedup();
        break;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        SaberBase::DoForce();
        break;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
        // Avoid the base and the very tip.
        SaberBase::addBlast((200 + random(700)) / 1000.0);
        SaberBase::DoBlast();
        break;

        // Lockup
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          if (pointing_down_) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          }
          SaberBase::DoBeginLockup();
        } else {
          handled = false;
        }
        break;

      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ON):
        if (SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          SaberBase::DoEndLockup();
        } else {
          handled = false;
        }
        break;

        // Off functions
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
        StartOrStopTrack();
        break;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
        next_preset();
        break;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
        previous_preset();
        break;

      case EVENTID(BUTTON_AUX2, EVENT_CLICK_SHORT, MODE_OFF):
#ifdef DUAL_POWER_BUTTONS
        next_preset();
#else
        previous_preset();
#endif
        break;
    }
    if (handled) {
      current_modifiers = BUTTON_NONE;
      return true;
    } else {
      return false;
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
    if (!strcmp(cmd, "get_on")) {
      STDOUT.println(on_);
      return true;
    }
    if (!strcmp(cmd, "clash")) {
      Clash();
      return true;
    }
    if (!strcmp(cmd, "blast")) {
      // Avoid the base and the very tip.
      SaberBase::addBlast((200 + random(700)) / 1000.0);
      SaberBase::DoBlast();
      return true;
    }
    if (!strcmp(cmd, "lock") || !strcmp(cmd, "lockup")) {
      STDOUT.print("Lockup ");
      if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        SaberBase::DoBeginLockup();
        STDOUT.println("ON");
      } else {
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        SaberBase::DoEndLockup();
        STDOUT.println("OFF");
      }
      return true;
    }
    if (!strcmp(cmd, "drag")) {
      STDOUT.print("Drag ");
      if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
        SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
        SaberBase::DoBeginLockup();
        STDOUT.println("ON");
      } else {
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        SaberBase::DoEndLockup();
        STDOUT.println("OFF");
      }
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
        STDOUT.print("Playing ");
        STDOUT.println(arg);
        player->Play(arg);
      } else {
        STDOUT.println("No available WAV players.");
      }
      return true;
    }
    if (!strcmp(cmd, "play_track")) {
      if (!arg) {
        StartOrStopTrack();
        return true;
      }
      if (track_player_) {
        track_player_->Stop();
        track_player_ = NULL;
      }
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      track_player_ = GetFreeWavPlayer();
      if (track_player_) {
        STDOUT.print("Playing ");
        STDOUT.println(arg);
        track_player_->Play(arg);
      } else {
        STDOUT.println("No available WAV players.");
      }
      return true;
    }
    if (!strcmp(cmd, "stop_track")) {
      if (track_player_) {
        track_player_->Stop();
        track_player_ = NULL;
      }
      return true;
    }
    if (!strcmp(cmd, "get_track")) {
      if (track_player_) {
        STDOUT.println(track_player_->Filename());
      }
      return true;
    }
    if (!strcmp(cmd, "say")) {
      talkie.Say(spBANK);
      talkie.Say(spOPEN);
      talkie.Say(spFAILURE);
      return true;
    }
    if (!strcmp(cmd, "volumes")) {
      for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
        STDOUT.print(" Unit ");
        STDOUT.print(unit);
        STDOUT.print(" Volume ");
        STDOUT.println(wav_players[unit].volume());
      }
      STDOUT.print("Splicer Volume ");
      STDOUT.println(audio_splicer.volume());
      return true;
    }
    if (!strcmp(cmd, "buffered")) {
      for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
        STDOUT.print(" Unit ");
        STDOUT.print(unit);
        STDOUT.print(" Buffered: ");
        STDOUT.println(wav_players[unit].buffered());
      }
      return true;
    }
#endif
    if (!strcmp(cmd, "cd")) {
      chdir(arg);
      SaberBase::DoNewFont();
      return true;
    }
#if 0
    if (!strcmp(cmd, "mkdir")) {
      SD.mkdir(arg);
      return true;
    }
#endif    
    if (!strcmp(cmd, "pwd")) {
      STDOUT.println(current_directory);
      return true;
    }
    if (!strcmp(cmd, "n") || (!strcmp(cmd, "next") && arg && (!strcmp(arg, "preset") || !strcmp(arg, "pre")))) {
      next_preset();
      return true;
    }
    if (!strcmp(cmd, "p") || (!strcmp(cmd, "prev") && arg && (!strcmp(arg, "preset") || !strcmp(arg, "pre")))) {
      previous_preset();
      return true;
    }
    if (!strcmp(cmd, "message") && arg) {
      SaberBase::DoMessage(arg);
      return true;
    }

    if (!strcmp(cmd, "list_presets")) {
      for (size_t i = 0; i < current_config_->num_presets; i++) {
        Preset *p = current_config_->presets + i;
        PrintQuotedValue("FONT", p->font);
        PrintQuotedValue("TRACK", p->track);
        PrintQuotedValue("NAME", p->name);
      }
      return true;
    }

    if (!strcmp(cmd, "get_preset")) {
      STDOUT.println(current_preset_ - current_config_->presets);
      return true;
    }
    if (!strcmp(cmd, "get_volume")) {
#ifdef ENABLE_AUDIO
      STDOUT.println(dynamic_mixer.get_volume());
#else
      STDOUT.println(0);
#endif      
      return true;
    }
    if (!strcmp(cmd, "set_volume") && arg) {
#ifdef ENABLE_AUDIO
      int32_t volume = strtol(arg, NULL, 0);
      if (volume >= 0 && volume <= 3000)
        dynamic_mixer.set_volume(volume);
#endif      
      return true;
    }
    
    if (!strcmp(cmd, "set_preset") && arg) {
      size_t preset = strtol(arg, NULL, 0);
      if (preset < current_config_->num_presets) {
        Preset *p = current_config_->presets + preset;
        if (p != current_preset_) {
          SetPreset(p, true);
          SaberBase::DoNewFont();
        }
      }
      return true;
    }
    
#ifdef ENABLE_SD
    if (!strcmp(cmd, "list_tracks")) {
      LOCK_SD(true);
      for (LSFS::Iterator iter("/"); iter; ++iter) {
        if (iter.isdir()) {
          char fname[128];
          strcpy(fname, iter.name());
          strcat(fname, "/");
          char* fend = fname + strlen(fname);
          bool isfont = false;
          if (!isfont) {
            strcpy(fend, "hum.wav");
            isfont = LSFS::Exists(fname);
          }
          if (!isfont) {
            strcpy(fend, "hum01.wav");
            isfont = LSFS::Exists(fname);
          }
          if (!isfont) {
            strcpy(fend, "hum");
            isfont = LSFS::Exists(fname);
          }
          if (!isfont) {
            for (LSFS::Iterator i2(iter); i2; ++i2) {
              if (endswith(".wav", i2.name()) && i2.size() > 200000) {
                strcpy(fend, i2.name());
                STDOUT.println(fname);
              }
            }
          }
        } else {
          if (endswith(".wav", iter.name()) && iter.size() > 200000) {
            STDOUT.println(iter.name());
          }
        }
      }
      LOCK_SD(false);
      return true;
    }
#endif
    return false;
  }
  void Help() override {
    STDOUT.println(" clash - trigger a clash");
    STDOUT.println(" on/off - turn saber on/off");
    STDOUT.println(" lock - begin/end lockup");
#ifdef ENABLE_AUDIO
    STDOUT.println(" pwd - print current directory");
    STDOUT.println(" cd directory - change directory, and sound font");
    STDOUT.println(" play filename - play file");
    STDOUT.println(" next/prev font - walk through directories in alphabetical order");
    STDOUT.println(" next/prev pre[set] - walk through presets.");
    STDOUT.println(" beep - play a beep");
#endif
  }

private:
  BladeConfig* current_config_ = NULL;
  Preset* current_preset_ = NULL;

  bool on_;  // <- move to SaberBase
};

Saber saber;

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
  ButtonBase(const char* name, enum BUTTON button)
    : Looper(),
      CommandParser(),
      name_(name),
      button_(button) {
  }

  const char* name() override { return name_; }

protected:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!DebouncedRead()) YIELD();
      saber.Event(button_, EVENT_PRESSED);
      if (millis() - push_millis_ < 500) {
        saber.Event(button_, EVENT_DOUBLE_CLICK);
      } else {
        push_millis_ = millis();
        current_modifiers |= button_;
      }
      while (DebouncedRead()) YIELD();
      saber.Event(button_, EVENT_RELEASED);
      if (current_modifiers & button_) {
        current_modifiers &=~ button_;
        if (millis() - push_millis_ < 500) {
          saber.Event(button_, EVENT_CLICK_SHORT);
        } else {
          saber.Event(button_, EVENT_CLICK_LONG);
        }
      } else {
        // someone ate our clicks
        push_millis_ = millis() - 10000; // disable double click
      }
    }
    STATE_MACHINE_END();
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      saber.Event(button_, EVENT_CLICK_SHORT);
      return true;
    }
    return false;
  }

  void Help() override {
    STDOUT.print(" ");
    STDOUT.print(name_);
    STDOUT.print(" - clicks the ");
    STDOUT.print(name_);
    STDOUT.println(" button");
  }

  const char* name_;
  enum BUTTON button_;
  uint32_t push_millis_;
  StateMachineState state_machine_;
};

// Latching button
class LatchingButton : public Looper,
                       public CommandParser,
                       public DebouncedButton {
public:
  LatchingButton(enum BUTTON button, int pin, const char* name)
    : Looper(),
      CommandParser(),
      name_(name),
      button_(button),
      pin_(pin) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_digital.pinMode(pin, INPUT_PULLUP, RISING);
#endif
  }

  const char* name() override { return name_; }

protected:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!DebouncedRead()) YIELD();
      saber.Event(button_, EVENT_LATCH_ON);
      while (DebouncedRead()) YIELD();
      saber.Event(button_, EVENT_LATCH_OFF);
    }
    STATE_MACHINE_END();
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      if (current_modifiers & button_) {
        saber.Event(button_, EVENT_LATCH_ON);
      } else {
        saber.Event(button_, EVENT_LATCH_OFF);
      }
      return true;
    }
    return false;
  }

  void Help() override {
    STDOUT.print(" ");
    STDOUT.print(name_);
    STDOUT.print(" - toggles the ");
    STDOUT.print(name_);
    STDOUT.println(" button");
  }

  bool Read() override {
    return digitalRead(pin_) == LOW;
  }

  const char* name_;
  enum BUTTON button_;
  StateMachineState state_machine_;
  uint8_t pin_;
};

class Button : public ButtonBase {
public:
  Button(enum BUTTON button, int pin, const char* name) : ButtonBase(name, button), pin_(pin) {
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
#define TOUCH_BUTTON_SUPPORTED
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
#define TOUCH_BUTTON_SUPPORTED
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
#define TOUCH_BUTTON_SUPPORTED
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255
};

#endif

#ifdef TOUCH_BUTTON_SUPPORTED
class TouchButton : public ButtonBase {
public:
  TouchButton(BUTTON button, int pin, int threshold, const char* name)
    : ButtonBase(name, button),
      pin_(pin),
      threshold_(threshold) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_touch.pinMode(pin, threshold);
#endif
#if defined(__MK64FX512__)
    STDOUT.println("Touch sensor not supported!\n");
#endif
    if (pin >= NUM_DIGITAL_PINS) {
      STDOUT.println("touch pin out of range");
      return;
    }
    if (pin2tsi[pin_] == 255) {
      STDOUT.println("Not a touch-capable pin!");
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
      STDOUT.print("Touch ");
      STDOUT.print(name_);
      STDOUT.print(" = ");
      STDOUT.print(value);
      STDOUT.print(" (");
      STDOUT.print(min_);
      STDOUT.print(" - ");
      STDOUT.print(max_);
      STDOUT.println(")");

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

#endif

#define CONFIG_BUTTONS
#include CONFIG_FILE
#undef CONFIG_BUTTONS

class Commands : public CommandParser {
 public:
  bool Parse(const char* cmd, const char* e) override {
    if (!strcmp(cmd, "help")) {
      // STDOUT.println("  red, green, blue, yellow, cyan, magenta, white");
      CommandParser::DoHelp();
      return true;
    }

#ifdef ENABLE_SERIALFLASH
    if (!strcmp(cmd, "ls")) {
      char tmp[128];
      SerialFlashChip::opendir();
      uint32_t size;
      while (SerialFlashChip::readdir(tmp, sizeof(tmp), size)) {
        STDOUT.print(tmp);
        STDOUT.print(" ");
        STDOUT.println(size);
      }
      STDOUT.println("Done listing files.");
      return true;
    }
    if (!strcmp(cmd, "rm")) {
      if (SerialFlashChip::remove(e)) {
        STDOUT.println("Removed.\n");
      } else {
        STDOUT.println("No such file.\n");
      }
      return true;
    }
    if (!strcmp(cmd, "format")) {
      STDOUT.print("Erasing ... ");
      SerialFlashChip::eraseAll();
      while (!SerialFlashChip::ready());
      STDOUT.println("Done");
      return true;
    }
#endif
#ifdef ENABLE_SD
    if (!strcmp(cmd, "dir")) {
      LOCK_SD(true);
      for (LSFS::Iterator dir(e ? e : current_directory); dir; ++dir) {
        STDOUT.print(dir.name());
        STDOUT.print(" ");
        STDOUT.println(dir.size());
      }
      LOCK_SD(false);
      STDOUT.println("Done listing files.");
      return true;
    }
    
    if (!strcmp(cmd, "readalot")) {
      uint8_t tmp[10];
      LOCK_SD(true);
      File f = LSFS::Open(e);
      for (int i = 0; i < 10000; i++) {
        f.seek(0);
        f.read(tmp, 10);
        f.seek(1000);
        f.read(tmp, 10);
      }
      LOCK_SD(true);
      STDOUT.println("Done");
      return true;
    }
    if (!strcmp(cmd, "sdtest")) {
      char filename[128];
      uint8_t block[512];
      hum.Play(filename);
      File f = LSFS::Open(filename);
      uint32_t start_millis = millis();
      int bytes = 0;
      for (int j = 0; j < 128; j++) {
        f.seek(0);
        for (int i = 0; i < 128; i++) {
          bytes += f.read(block, 512);
        }
        STDOUT.print(".");
      }
      uint32_t end_millis = millis();
      LOCK_SD(true);
      STDOUT.println("Done");
      // bytes per ms = kb per s (note, not kibibytes)
      float kb_per_sec = bytes / (float)(end_millis - start_millis);
      STDOUT.println("SD card speed: ");
      STDOUT.print(kb_per_sec);
      STDOUT.print(" kb/s = ");
      STDOUT.print(kb_per_sec / 88.2);
      STDOUT.println(" simultaneous audio streams.");
      return true;
    }
#endif
#if defined(ENABLE_SD) && defined(ENABLE_SERIALFLASH)
    if (!strcmp(cmd, "cache")) {
      LOCK_SD(true);
      File f = LSFS::Open(e);
      if (!f) {
        STDOUT.println("File not found.");
        return true;
      }
      int bytes = f.size();
      if (!SerialFlashChip::create(e, bytes)) {
        STDOUT.println("Not enough space on serial flash chip.");
        return true;
      }
      SerialFlashFile o = SerialFlashChip::open(e);
      while (bytes) {
        char tmp[256];
        int b = f.read(tmp, min(bytes, (int)NELEM(tmp)));
        o.write(tmp, b);
        bytes -= b;
      }
      LOCK_SD(false);
      STDOUT.println("Cached!");
      return true;
    }
#endif
    if (!strcmp(cmd, "effects")) {
      Effect::ShowAll();
      return true;
    }
#if 0
    if (!strcmp(cmd, "df")) {
      STDOUT.print(SerialFlashChip::capacity());
      STDOUT.println(" bytes available.");
      return true;
    }
#endif
#ifdef ENABLE_AUDIO
#if 0
    if (!strcmp(cmd, "ton")) {
      digitalWrite(amplifierPin, HIGH); // turn on the amplifier
      dac.SetStream(&saber_synth);
      saber_synth.on_ = true;
      return true;
    }
    if (!strcmp(cmd, "tof")) {
      saber_synth.on_ = false;
      return true;
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
          STDOUT.print(tmp[i]);
          STDOUT.print(" ");
        }
        STDOUT.println("");
      }
      wav_players[0].Stop();
      return true;
    }
#endif
    if (!strcmp(cmd, "twiddle")) {
      int pin = strtol(e, NULL, 0);
      STDOUT.print("twiddling ");
      STDOUT.println(pin);
      pinMode(pin, OUTPUT);
      for (int i = 0; i < 1000; i++) {
        digitalWrite(pin, HIGH);
        delay(10);
        digitalWrite(pin, LOW);
        delay(10);
      }
      STDOUT.println("done");
      return true;
    }
    if (!strcmp(cmd, "twiddle2")) {
      int pin = strtol(e, NULL, 0);
      STDOUT.print("twiddling ");
      STDOUT.println(pin);
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
      STDOUT.println("done");
      return true;
    }
    if (!strcmp(cmd, "malloc")) {
      STDOUT.print("alloced: ");
      STDOUT.println(mallinfo().uordblks);
      STDOUT.print("Free: ");
      STDOUT.println(mallinfo().fordblks);
      return true;
    }
    if (!strcmp(cmd, "top")) {
      // TODO: list cpu usage for various objects.
      double total_cycles =
        (double)(audio_dma_interrupt_cycles +
                 wav_interrupt_cycles +
                 loop_cycles);
      STDOUT.print("Audio DMA: ");
      STDOUT.print(audio_dma_interrupt_cycles * 100.0 / total_cycles);
      STDOUT.println("%");
      STDOUT.print("Wav reading: ");
      STDOUT.print(wav_interrupt_cycles * 100.0 / total_cycles);
      STDOUT.println("%");
      STDOUT.print("LOOP: ");
      STDOUT.print(loop_cycles * 100.0 / total_cycles);
      STDOUT.println("%");
      STDOUT.print("Global loops / second: ");
      global_loop_counter.Print();
      STDOUT.println("");
      SaberBase::DoTop();
      Looper::LoopTop(total_cycles);
      noInterrupts();
      audio_dma_interrupt_cycles = 0;
      wav_interrupt_cycles = 0;
      loop_cycles = 0;
      interrupts();
      return true;
    }
    if (!strcmp(cmd, "version")) {
      STDOUT.println(version);
      return true;
    }
#ifdef TEENSYDUINO    
    if (!strcmp(cmd, "reset")) {
      SCB_AIRCR = 0x05FA0004;
      STDOUT.println("Reset failed.");
      return true;
    }
#endif    
    return false;
  }

  void Help() override {
    STDOUT.println(" version - show software version");
    STDOUT.println(" reset - restart software");
    STDOUT.println(" effects - list current effects");
#ifdef ENABLE_SERIALFLASH
    STDOUT.println("Serial Flash memory management:");
    STDOUT.println("   ls, rm <file>, format, play <file>, effects");
    STDOUT.println("To upload files: tar cf - files | uuencode x >/dev/ttyACM0");
#endif
#ifdef ENABLE_SD
    STDOUT.println(" dir [directory] - list files on SD card.");
    STDOUT.println(" sdtest - benchmark SD card");
#endif
  }
};

Commands commands;


class SerialAdapter {
public:
  static void begin() { Serial.begin(115200); }
  static bool Connected() { return !!Serial; }
  static bool AlwaysConnected() { return false; }
  static Stream& stream() { return Serial; }
  static const char* response_header() { return ""; }
  static const char* response_footer() { return ""; }
};

class Serial3Adapter {
public:
  static void begin() { Serial3.begin(115200); }
  static bool Connected() { return true; }
  static bool AlwaysConnected() { return true; }
  static Stream& stream() { return Serial3; }
  static const char* response_header() { return "-+=BEGIN_OUTPUT=+-\n"; }
  static const char* response_footer() { return "-+=END_OUTPUT=+-\n"; }
};

// Command-line parser. Easiest way to use it is to start the arduino
// serial monitor.
template<class SA> /* SA = Serial Adapter */
class Parser : Looper, StateMachine {
public:
  Parser() : Looper(), len_(0) {
  }
  const char* name() override { return "Parser"; }

  void Setup() override {
    SA::begin();
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!SA::Connected()) YIELD();
      if (!SA::AlwaysConnected()) {
        STDOUT.println("Welcome to TeensySaber, type 'help' for more info.");
      }

      while (SA::Connected()) {
        while (!SA::stream().available()) YIELD();
        int c = SA::stream().read();
        if (c < 0) { len_ = 0; break; }
#if 0
        if (monitor.IsMonitoring(Monitoring::MonitorSerial) &&
            default_output != &SA::stream()) {
          default_output->print("SER: ");
          default_output->println(c, HEX);
        }
#endif  
        if (c == '\n') { ParseLine(); len_ = 0; continue; }
        cmd_[len_] = c;
        cmd_[len_ + 1] = 0;
        if (len_ + 1 < (int)sizeof(cmd_)) len_++;
      }
    }
    STATE_MACHINE_END();
  }

  void ParseLine() {
    if (len_ == 0 || len_ == (int)sizeof(cmd_)) return;
    while (len_ > 0 && (cmd_[len_-1] == '\r' || cmd_[len_-1] == ' ')) {
      len_--;
      cmd_[len_] = 0;
    }
    if (cmd_[0] == '#') {
      Serial.println(cmd_);
      return;
    }
    stdout_output = &SA::stream();
    STDOUT.print(SA::response_header());
    char *cmd = cmd_;
    while (*cmd == ' ') cmd++;
    char *e = cmd;
    while (*e != ' ' && *e) e++;
    if (*e) {
      *e = 0;
      e++;  // e is now argument (if any)
    }
    if (monitor.IsMonitoring(Monitoring::MonitorSerial) &&
        default_output != &SA::stream()) {
      default_output->print("Received command: ");
      default_output->print(cmd);
      if (e) {
        default_output->print(" arg: ");
        default_output->print(e);
      }
      default_output->print(" HEX ");
      for (size_t i = 0; i < strlen(cmd); i++) {
        default_output->print(cmd[i], HEX);
        default_output->print(" ");
      }
      default_output->println("");
    }
    if (!CommandParser::DoParse(cmd, e)) {
      STDOUT.print("Whut? :");
      STDOUT.println(cmd);
    }
    STDOUT.print(SA::response_footer());
    stdout_output = default_output;
  }

private:
  int len_;
  char cmd_[256];
};

Parser<SerialAdapter> parser;

#ifdef ENABLE_SERIAL
Parser<Serial3Adapter> serial_parser;

class SerialCommands : public CommandParser {
 public:
  void HM1XCmd(const char* cmd) {
    STDOUT.print("Sending: ");
    STDOUT.println(cmd);
    STDOUT.print("Reply: ");
    Serial3.print(cmd);
    uint32_t last_char = millis();
    uint32_t timeout = 300;
    while (millis() - last_char < timeout) {
      if (Serial3.available()) {
        last_char = millis();
        timeout = 100;
        STDOUT.write(Serial3.read());
      }
    }
    STDOUT.println("");
  }
  bool Parse(const char* cmd, const char* e) override {
#if 0
    if (!strcmp(cmd, "hm1Xpin")) {
      // Doesn't work, pine is 4 chars, pinb is 6
      HM1XCmd("AT+AUTH1");
      HM1XCmd("AT+DUAL1");
      Serial3.write("AT+PINE");
      Serial3.println(e);
      Serial3.write("AT+PINB");
      Serial3.println(e);
      return true;
    }
    if (!strcmp(cmd, "hm1Xname")) {
      Serial3.write("AT+NAME");
      Serial3.println(e);
      Serial3.write("AT+NAMB");
      Serial3.println(e);
      return true;
    }
#endif
    if (cmd[0] == 'A' && cmd[1] == 'T') {
      HM1XCmd(cmd);
      return true;
    }
    if (!strcmp(cmd, "send") && e) {
      Serial3.println(e);
      STDOUT.print("Wrote: ");
      STDOUT.println(e);
      return true;
    }
    if (!strcmp(cmd, "make_default_console")) {
      default_output = stdout_output;
      return true;
    }
#ifdef BLE_PASSWORD
    if (!strcmp(cmd, "get_ble_config")) {
      PrintQuotedValue("password", BLE_PASSWORD);
#ifndef BLE_NAME
#define BLE_NAME "TeensySaber"
#endif
      PrintQuotedValue("name", BLE_NAME);
#ifdef BLE_SHORTNAME
      PrintQuotedValue("shortname", BLE_SHORTNAME);
#else
      if (sizeof(BLE_NAME) - sizeof("") <= 9) {
        PrintQuotedValue("shortname", BLE_NAME);
      } else {
        PrintQuotedValue("shortname", "Saber");
      }
#endif
      return true;
    }
#endif
    return false;
  }
  void Help() override {
    // STDOUT.println(" hm13pin PIN - configure HM13 PIN");
    // STDOUT.println(" hm13name NAME - configure HM13 NAME");
    STDOUT.println(" get_ble_config - show BLE PIN");
    if (default_output != stdout_output)
      STDOUT.println(" make_default_console - make this connection the default connection");
  }
};

SerialCommands serial_commands;

#endif


#ifdef ENABLE_MOTION

#define I2C_TIMEOUT_MILLIS 300

class I2CBus : Looper, StateMachine {
public:
  const char* name() override { return "I2CBus"; }
  void Loop() {
    STATE_MACHINE_BEGIN();
    SLEEP(1000);

#ifdef TEENSYSABER    
    // Check that we have pullups.
    while (true) {
      STDOUT.println("I2C TRY");
      pinMode(i2cDataPin, INPUT_PULLDOWN);
      pinMode(i2cClockPin, INPUT_PULLDOWN);
      SLEEP_MICROS(10);
#ifdef TEENSYSABER      
      data_detected = analogRead(i2cDataPin) > 800;
      clock_detected = analogRead(i2cClockPin) > 800;
#else
      data_detected = digitalRead(i2cDataPin);
      clock_detected = digitalRead(i2cClockPin);
#endif      
      
      pinMode(i2cDataPin, INPUT);
      pinMode(i2cClockPin, INPUT);
      if (data_detected && clock_detected) {
        // All good, proceed.
        break;
      }
      if (clock_detected && !data_detected) {
        STDOUT.println("I2C pending data detected, trying to clear...");
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
        if (!clock_detected)
          Serial.println("No I2C clock pullup detected.");
        SLEEP(1000); // Try again later
      }
    }

    STDOUT.println("I2C pullups found, initializing...");
#endif
    
    Wire.begin();
    Wire.setClock(400000);
#ifdef TEENSYDUINO
    Wire.setDefaultTimeout(I2C_TIMEOUT_MILLIS * 1000);
#endif
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
protected:
  uint8_t address_;
};

#ifdef ENABLE_SSD1306
struct Glyph {
  int8_t skip;
  int8_t xoffset;
  int8_t yoffset;
  uint8_t columns;
  const uint32_t* data;
};

const uint32_t BatteryBar16_data[] = {
   0b00000000000000000000000000000000UL,
   0b00000111111111111111111111100000UL,
   0b00011111111111111111111111111000UL,
   0b00111111111111111111111111111100UL,
   0b01111111111111111111111111111110UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b01111111111111111111111111111110UL,
   0b00111111111111111111111111111100UL,
   0b00011111111111111111111111111000UL,
   0b00000111111111111111111111100000UL,
};

#define GLYPHDATA(X) NELEM(X), X

const Glyph BatteryBar16 = { 16, 0, 0, GLYPHDATA(BatteryBar16_data) };

#include "StarJedi10Font.h"

class SSD1306 : public I2CDevice, Looper, StateMachine, SaberBase {
public:
  static const int WIDTH = 128;
  static const int HEIGHT = 32;
  const char* name() override { return "SSD1306"; }

  enum Commands {
    SETCONTRAST = 0x81,
    DISPLAYALLON_RESUME = 0xA4,
    DISPLAYALLON = 0xA5,
    NORMALDISPLAY = 0xA6,
    INVERTDISPLAY = 0xA7,
    DISPLAYOFF = 0xAE,
    DISPLAYON = 0xAF,

    SETDISPLAYOFFSET = 0xD3,
    SETCOMPINS = 0xDA,

    SETVCOMDETECT = 0xDB,

    SETDISPLAYCLOCKDIV = 0xD5,
    SETPRECHARGE = 0xD9,

    SETMULTIPLEX = 0xA8,

    SETLOWCOLUMN = 0x00,
    SETHIGHCOLUMN = 0x10,

    SETSTARTLINE = 0x40,

    MEMORYMODE = 0x20,
    COLUMNADDR = 0x21,
    PAGEADDR   = 0x22,

    COMSCANINC = 0xC0,
    COMSCANDEC = 0xC8,

    SEGREMAP = 0xA0,

    CHARGEPUMP = 0x8D,

    EXTERNALVCC = 0x1,
    SWITCHCAPVCC = 0x2,

    // Scrolling commands
    ACTIVATE_SCROLL = 0x2F,
    DEACTIVATE_SCROLL = 0x2E,
    SET_VERTICAL_SCROLL_AREA = 0xA3,
    RIGHT_HORIZONTAL_SCROLL = 0x26,
    LEFT_HORIZONTAL_SCROLL = 0x27,
    VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29,
    VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A,
  };

  enum Screen {
    SCREEN_STARTUP,
    SCREEN_MESSAGE,
    SCREEN_PLI,
  };

  SSD1306() : I2CDevice(0x3C) { }
  void Send(int c) { writeByte(0, c); }

  void Draw(const Glyph& glyph, int x, int y) {
    x += glyph.xoffset;
    y += glyph.yoffset;
    int begin = max(0, -x);
    int end = min(glyph.columns, WIDTH - x);
    uint32_t *pos = frame_buffer_ + x;
    if (y > 0) {
      for (int i = begin; i < end; i++) pos[i] |= glyph.data[i] << y;
    } else if (y < 0) {
      for (int i = begin; i < end; i++) pos[i] |= glyph.data[i] >> -y;
    } else {
      for (int i = begin; i < end; i++) pos[i] |= glyph.data[i];
    }
  }

  void DrawBatteryBar(const Glyph& bar) {
    int start, end;
    if (bar.skip < bar.columns) {
      start = -bar.skip;
      end = WIDTH + bar.columns - 1;
    } else {
      start = 0;
      end = WIDTH;
    }
    int max_bars = (end - start) / bar.skip;
    int pos = start;
    int bars = floor(
        battery_monitor.battery_percent() * (0.5 + max_bars) / 100);
    for (int i = 0; i < bars; i++) {
      Draw(bar, pos, 0);
      pos += bar.skip;
    }
  }
  void DrawText(const char* str,
                int x, int y,
                const Glyph* font) {
    while (*str) {
      if (*str == '\n') {
        x = 0;
        y += 16;
      } else if (*str >= 0x20 && *str <= 0x7f) {
        Draw(font[*str - 0x20], x, y);
        x += font[*str - 0x20].skip;
      }
      str++;
    }
  }

  void FillFrameBuffer() {
    memset(frame_buffer_, 0, sizeof(frame_buffer_));

    if (millis() - displayed_when_ > 5000)
      screen_ = SCREEN_PLI;

    switch (screen_) {
      case SCREEN_STARTUP:
        DrawText("==SabeR===", 0,15, Starjedi10pt7bGlyphs);
        DrawText("++Teensy++",-4,31, Starjedi10pt7bGlyphs);
        break;

      case SCREEN_PLI:
        DrawBatteryBar(BatteryBar16);
        break;

      case SCREEN_MESSAGE:
        if (strchr(message_, '\n')) {
          DrawText(message_, 0, 15, Starjedi10pt7bGlyphs);
        } else {
          DrawText(message_, 0, 23, Starjedi10pt7bGlyphs);
        }
    }
  }

  void SB_Message(const char* text) override {
    strncpy(message_, text, sizeof(message_));
    message_[sizeof(message_)-1] = 0;
    displayed_when_ = millis();
    screen_ = SCREEN_MESSAGE;
  }

  void SB_Top() override {
    STDOUT.print("display fps: ");
    loop_counter_.Print();
    STDOUT.println("");
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (!i2cbus.inited()) YIELD();

    // Init sequence
    Send(DISPLAYOFF);                    // 0xAE
    Send(SETDISPLAYCLOCKDIV);            // 0xD5
    Send(0x80);                                  // the suggested ratio 0x80
    
    Send(SETMULTIPLEX);                  // 0xA8
    Send(HEIGHT - 1);
    
    Send(SETDISPLAYOFFSET);              // 0xD3
    Send(0x0);                                   // no offset
    Send(SETSTARTLINE | 0x0);            // line #0
    Send(CHARGEPUMP);                    // 0x8D
    Send(0x14);
    Send(MEMORYMODE);                    // 0x20
    Send(0x01);                          // vertical address mode
    Send(SEGREMAP | 0x1);
    Send(COMSCANDEC);

    Send(SETCOMPINS);                    // 0xDA
    Send(0x02);  // may need to be 0x12 for some displays
    Send(SETCONTRAST);                   // 0x81
    Send(0x8F);

    Send(SETPRECHARGE);                  // 0xd9
    Send(0xF1);
    Send(SETVCOMDETECT);                 // 0xDB
    Send(0x40);
    Send(DISPLAYALLON_RESUME);           // 0xA4
    Send(NORMALDISPLAY);                 // 0xA6
    
    Send(DEACTIVATE_SCROLL);

    Send(DISPLAYON);                     //--turn on oled panel

    STDOUT.println("Display initialized.");
    screen_ = SCREEN_STARTUP;
    displayed_when_ = millis();
    
    while (true) {
      FillFrameBuffer();
      Send(COLUMNADDR);
      Send(0);   // Column start address (0 = reset)
      Send(WIDTH-1); // Column end address (127 = reset)

      Send(PAGEADDR);
      Send(0); // Page start address (0 = reset)
      switch (HEIGHT) {
        case 64:
          Send(7); // Page end address
          break;
        case 32:
          Send(3); // Page end address
          break;
        case 16:
          Send(1); // Page end address
          break;
        default:
          STDOUT.println("Unknown display height");
      }

      //STDOUT.println(TWSR & 0x3, DEC);
        
      // I2C
      for (i=0; i < WIDTH * HEIGHT / 8; ) {
        // send a bunch of data in one xmission
        Wire.beginTransmission(address_);
        Wire.write(0x40);
        for (uint8_t x=0; x<16; x++) {
          Wire.write(((unsigned char*)frame_buffer_)[i]);
          i++;
        }
        Wire.endTransmission();
        YIELD();
      }
      loop_counter_.Update();
    }
    
    STATE_MACHINE_END();
  }

private:
  uint16_t i;
  uint32_t frame_buffer_[WIDTH];
  LoopCounter loop_counter_;
  char message_[32];
  uint32_t displayed_when_;
  Screen screen_;
};

SSD1306 display;
#endif

class MPU6050 : public I2CDevice, Looper, StateMachine {
public:
  const char* name() override { return "MPU6050"; }
  enum Registers {
    AUX_VDDIO          = 0x01,   // R/W
    SELF_TEST_X        = 0x0D,   // R/W
    SELF_TEST_Y        = 0x0E,   // R/W
    SELF_TEST_Z        = 0x0F,   // R/W
    SELF_TEST_A        = 0x10,   // R/W
    SMPLRT_DIV         = 0x19,   // R/W
    CONFIG             = 0x1A,   // R/W
    GYRO_CONFIG        = 0x1B,   // R/W
    ACCEL_CONFIG       = 0x1C,   // R/W
    FF_THR             = 0x1D,   // R/W
    FF_DUR             = 0x1E,   // R/W
    MOT_THR            = 0x1F,   // R/W
    MOT_DUR            = 0x20,   // R/W
    ZRMOT_THR          = 0x21,   // R/W
    ZRMOT_DUR          = 0x22,   // R/W
    FIFO_EN            = 0x23,   // R/W
    I2C_MST_CTRL       = 0x24,   // R/W
    I2C_SLV0_ADDR      = 0x25,   // R/W
    I2C_SLV0_REG       = 0x26,   // R/W
    I2C_SLV0_CTRL      = 0x27,   // R/W
    I2C_SLV1_ADDR      = 0x28,   // R/W
    I2C_SLV1_REG       = 0x29,   // R/W
    I2C_SLV1_CTRL      = 0x2A,   // R/W
    I2C_SLV2_ADDR      = 0x2B,   // R/W
    I2C_SLV2_REG       = 0x2C,   // R/W
    I2C_SLV2_CTRL      = 0x2D,   // R/W
    I2C_SLV3_ADDR      = 0x2E,   // R/W
    I2C_SLV3_REG       = 0x2F,   // R/W
    I2C_SLV3_CTRL      = 0x30,   // R/W
    I2C_SLV4_ADDR      = 0x31,   // R/W
    I2C_SLV4_REG       = 0x32,   // R/W
    I2C_SLV4_DO        = 0x33,   // R/W
    I2C_SLV4_CTRL      = 0x34,   // R/W
    I2C_SLV4_DI        = 0x35,   // R
    I2C_MST_STATUS     = 0x36,   // R
    INT_PIN_CFG        = 0x37,   // R/W
    INT_ENABLE         = 0x38,   // R/W
    INT_STATUS         = 0x3A,   // R
    ACCEL_XOUT_H       = 0x3B,   // R
    ACCEL_XOUT_L       = 0x3C,   // R
    ACCEL_YOUT_H       = 0x3D,   // R
    ACCEL_YOUT_L       = 0x3E,   // R
    ACCEL_ZOUT_H       = 0x3F,   // R
    ACCEL_ZOUT_L       = 0x40,   // R
    TEMP_OUT_H         = 0x41,   // R
    TEMP_OUT_L         = 0x42,   // R
    GYRO_XOUT_H        = 0x43,   // R
    GYRO_XOUT_L        = 0x44,   // R
    GYRO_YOUT_H        = 0x45,   // R
    GYRO_YOUT_L        = 0x46,   // R
    GYRO_ZOUT_H        = 0x47,   // R
    GYRO_ZOUT_L        = 0x48,   // R
    EXT_SENS_DATA_00   = 0x49,   // R
    EXT_SENS_DATA_01   = 0x4A,   // R
    EXT_SENS_DATA_02   = 0x4B,   // R
    EXT_SENS_DATA_03   = 0x4C,   // R
    EXT_SENS_DATA_04   = 0x4D,   // R
    EXT_SENS_DATA_05   = 0x4E,   // R
    EXT_SENS_DATA_06   = 0x4F,   // R
    EXT_SENS_DATA_07   = 0x50,   // R
    EXT_SENS_DATA_08   = 0x51,   // R
    EXT_SENS_DATA_09   = 0x52,   // R
    EXT_SENS_DATA_10   = 0x53,   // R
    EXT_SENS_DATA_11   = 0x54,   // R
    EXT_SENS_DATA_12   = 0x55,   // R
    EXT_SENS_DATA_13   = 0x56,   // R
    EXT_SENS_DATA_14   = 0x57,   // R
    EXT_SENS_DATA_15   = 0x58,   // R
    EXT_SENS_DATA_16   = 0x59,   // R
    EXT_SENS_DATA_17   = 0x5A,   // R
    EXT_SENS_DATA_18   = 0x5B,   // R
    EXT_SENS_DATA_19   = 0x5C,   // R
    EXT_SENS_DATA_20   = 0x5D,   // R
    EXT_SENS_DATA_21   = 0x5E,   // R
    EXT_SENS_DATA_22   = 0x5F,   // R
    EXT_SENS_DATA_23   = 0x60,   // R
    MOT_DETECT_STATUS  = 0x61,   // R
    I2C_SLV0_DO        = 0x63,   // R/W
    I2C_SLV1_DO        = 0x64,   // R/W
    I2C_SLV2_DO        = 0x65,   // R/W
    I2C_SLV3_DO        = 0x66,   // R/W
    I2C_MST_DELAY_CTRL = 0x67,   // R/W
    SIGNAL_PATH_RESET  = 0x68,   // R/W
    MOT_DETECT_CTRL    = 0x69,   // R/W
    USER_CTRL          = 0x6A,   // R/W
    PWR_MGMT_1         = 0x6B,   // R/W
    PWR_MGMT_2         = 0x6C,   // R/W
    FIFO_COUNTH        = 0x72,   // R/W
    FIFO_COUNTL        = 0x73,   // R/W
    FIFO_R_W           = 0x74,   // R/W
    WHO_AM_I           = 0x75,   // R
  };

  MPU6050() : I2CDevice(0x86) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();

    while (1) {
      unsigned char databuffer[6];

      STDOUT.print("Motion setup ... ");
      writeByte(PWR_MGMT_1, 0); // wake up
      writeByte(CONFIG, 1);     // digital filter config ~180Hz, 1khz rate, 2ms delay
      writeByte(SMPLRT_DIV, 0); // sample rate = 1khz / 1
      writeByte(GYRO_CONFIG, 3 << 3); // 2000 degrees / s
      writeByte(ACCEL_CONFIG, 1 << 3); // 4g range
      writeByte(INT_ENABLE, 1); // enable data ready interrupt

      if (readByte(WHO_AM_I) == 0x86) {
        STDOUT.println("done.");
      } else {
        STDOUT.println("failed.");
      }

      while (1) {
        YIELD();
        int status_reg = readByte(INT_STATUS);
        if (status_reg == -1) {
          // motion fail, reboot motion chip.
          STDOUT.println("Motion chip timeout, reboot motion chip!");
          // writeByte(CTRL3_C, 1);
          delay(20);
          break;
        }
        if (status_reg & 0x1) {
          // gyroscope data available
          if (readBytes(GYRO_XOUT_H, databuffer, 6) == 6) {
            SaberBase::DoMotion(
              Vec3::MSB(databuffer, 2000.0 / 32768.0)); // 2000 dps
          }

          // accel data available
          if (readBytes(ACCEL_XOUT_H, databuffer, 6) == 6) {
            SaberBase::DoAccel(
              Vec3::MSB(databuffer, 4.0 / 32768.0));  // 4 g range
          }

          // Temp data available
          // TODO: Temp Shutdown
          if (readBytes(TEMP_OUT_H, databuffer, 2) == 2) {
            int16_t temp_data = (databuffer[0] << 8) + databuffer[1];
            float temp = temp_data / 340.0 + 36.53;
            if (monitor.ShouldPrint(Monitoring::MonitorTemp)) {
              STDOUT.print("TEMP: ");
              STDOUT.println(temp);
            }
          }
        }
      }
    }
    STATE_MACHINE_END();
  }
};

class LSM6DS3H : public I2CDevice, Looper, StateMachine {
public:
  const char* name() override { return "LSM6DS3H"; }
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

      STDOUT.print("Motion setup ... ");

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
        STDOUT.println("done.");
      } else {
        STDOUT.println("failed.");
      }

      while (1) {
        YIELD();
        int status_reg = readByte(STATUS_REG);
        if (status_reg == -1) {
          // motion fail, reboot motion chip.
          STDOUT.println("Motion chip timeout, reboot motion chip!");
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
              STDOUT.print("TEMP: ");
              STDOUT.println(temp);
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

class FXOS8700 : public I2CDevice, Looper, StateMachine {
public:
  const char* name() override { return "FXOS8700"; }
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
      STDOUT.print("Accel setup ... ");

      if (readByte(WHO_AM_I) != 0xC7) {
        STDOUT.print("Failed.");
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

      STDOUT.println(" Done");

      while (1) {
        YIELD();
        int status = readByte(STATUS);
        if (status == -1) {
          // motion fail, reboot gyro chip.
          STDOUT.println("Motion chip timeout, reboot motion chip!");
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
  const char* name() override { return "FXAS21002"; }
  enum Registers {
    I2C_ADDR0         = 0x20, // SA0 = Gnd
    I2C_ADDR1         = 0x21, // SA0 = Vcc
    STATUS            = 0x00, // Alias for DR_STATUS or F_STATUS
    OUT_X_MSB         = 0x01, // MSB of 16 bit X-axis data sample
    OUT_X_LSB         = 0x02, // LSB of 16 bit X-axis data sample
    OUT_Y_MSB         = 0x03 ,// MSB of 16 bit Y-axis data sample
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

      STDOUT.print("Gyro setup ... ");

      if (readByte(WHO_AM_I) != 0xD7) {
        STDOUT.println("Failed.");
        SLEEP(1000);
        return;
      }

      // Standby
      writeByte(CTRL_REG1, 0);
      // switch to active mode, 800 Hz output rate
      writeByte(CTRL_REG0, 0x00);
      writeByte(CTRL_REG1, 0x02);

      STDOUT.println(" Done");

      while (1) {
        YIELD();
        int status = readByte(STATUS);
        if (status == -1) {
          // motion fail, reboot gyro chip.
          STDOUT.println("Motion chip timeout, reboot motion chip!");
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

#ifdef CLASH_RECORDER
class ClashRecorder : public SaberBase {
public:
  void SB_Clash() override {
    time_to_dump_ = NELEM(buffer_) / 2;
  }
  void SB_Accel(const Vec3& accel) override {
    buffer_[pos_] = accel;
    pos_++;
    if (pos_ == NELEM(buffer_)) pos_ = 0;
    if (time_to_dump_) {
      time_to_dump_--;
      if (time_to_dump_ == 0) {
        LOCK_SD(true);
        char file_name[16];
        size_t file_num = last_file_ + 1;

        while (true) {
          char num[16];
          itoa(file_num, num, 10);
          strcpy(file_name, "CLS");
          while(strlen(num) + strlen(file_name) < 8) strcat(file_name, "0");
          strcat(file_name, num);
          strcat(file_name, ".CSV");
          
          int last_skip = file_num - last_seen_;
          if (LSFS::Exists(file_name)) {
            last_file_ = file_num;
            file_num += last_skip * 2;
            continue;
          }

          if (file_num - last_file_ > 1) {
            file_num = last_file_ + last_skip / 2;
            continue;
          }
          break;
        }
        File f = LSFS::OpenForWrite(file_name);
        for (size_t i = 0; i < NELEM(buffer_); i++) {
          const Vec3& v = buffer_[(pos_ + i) % NELEM(buffer_)];
          f.print(v.x);
          f.print(", ");
          f.print(v.y);
          f.print(", ");
          f.print(v.z);
          f.print("\n");
        }
        f.close();
        LOCK_SD(false);
      }
    }
  }
private:
  size_t last_file_ = 0;
  size_t time_to_dump_ = 0;
  size_t pos_ = 0;
  Vec3 buffer_[512];
};

ClashRecorder clash_recorder;
#endif


#ifdef GYRO_CLASS
// Can also be gyro+accel.
GYRO_CLASS gyroscope;
#endif

#ifdef ACCEL_CLASS
ACCEL_CLASS accelerometer;
#endif

#endif   // ENABLE_MOTION


#ifdef ENABLE_AUDIO
// Turns off amplifier when no audio is played.
// Maybe name this IdleHelper or something instead??
class Amplifier : Looper, StateMachine, CommandParser {
public:
  Amplifier() : Looper(), CommandParser() {}
  const char* name() override { return "Amplifier"; }

  bool Active() {
//    if (saber_synth.on_) return true;
    if (audio_splicer.isPlaying()) return true;
    if (beeper.isPlaying()) return true;
    if (talkie.isPlaying()) return true;
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
      STDOUT.println("Amplifier off.");
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
      STDOUT.print("Saber bases: ");
      STDOUT.println(on ? "On" : "Off");
      STDOUT.print("Audio splicer: ");
      STDOUT.println(audio_splicer.isPlaying() ? "On" : "Off");
      STDOUT.print("Beeper: ");
      STDOUT.println(beeper.isPlaying() ? "On" : "Off");
      STDOUT.print("Talker: ");
      STDOUT.println(talkie.isPlaying() ? "On" : "Off");
      for (size_t i = 0; i < NELEM(wav_players); i++) {
        STDOUT.print("Wav player ");
        STDOUT.print(i);
        STDOUT.print(": ");
        STDOUT.print(wav_players[i].isPlaying() ? "On" : "Off");
        STDOUT.print(" (eof =  ");
        STDOUT.print(wav_players[i].eof());
        STDOUT.print(" volume = ");
        STDOUT.print(wav_players[i].volume());
        STDOUT.println(")");
      }
      return true;
    }
    return false;
  }

  void Help() {
    STDOUT.println(" amp on/off - turn amplifier on or off");
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

  Serial.begin(9600);
  // Wait for all voltages to settle.
  // Accumulate some entrypy while we wait.
  uint32_t now = millis();
  while (millis() - now < 1000) {
    srand((rand() * 917823) ^ analogRead(batteryLevelPin));
  }

#ifdef ENABLE_SERIALFLASH
  SerialFlashChip::begin(serialFlashSelectPin);
#endif
#ifdef ENABLE_SD
  bool sd_card_found = LSFS::Begin();
  if (!sd_card_found) {
    STDOUT.println("No sdcard found.");
  } else {
    STDOUT.println("Sdcard found..");
  }
#endif
  // Time to identify the blade.
  Looper::DoSetup();
  saber.FindBlade();
  SaberBase::DoBoot();
#if defined(ENABLE_SD) && defined(ENABLE_AUDIO)
  if (!sd_card_found) {
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    talkie.Say(spPLEASE);
    talkie.Say(spCONNECT);
    talkie.Say(spS);
    talkie.Say(spD);
    talkie.Say(spUNIT);
  }
#endif // ENABLE_AUDIO && ENABLE_SD
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
  const char* name() override { return "WatchDog"; }
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

  virtual uint32_t free_objects() = 0;

  // Get Name of storage unit.
  virtual const char *GetName() = 0;

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
  virtual void SendObject(uint32_t length) {}
  virtual void write(const char* data, uint32_t size);
  virtual void close() {}
  virtual bool DeleteObject(uint32_t object) = 0;
  virtual bool Format() { return false; }
};

// MTP Responder.
class MTPD {
public:
  void AddStorage(MTPStorageInterface* storage) {
    for (size_t i = 0; i < NELEM(storage_); i++) {
      if (!storage_[i]) {
        storage_[i] = storage;
        return;
      }
    }
  }
    
  MTPD() {
    for (size_t i = 0; i < NELEM(storage_); i++) storage_[i] = 0;
  }

private:
  MTPStorageInterface* storage_[16];

  MTPStorageInterface* Stor(uint32_t id) {
    return storage_[id >> 28];
  }

  // Make an external object id from an internal one.
  uint32_t EXT(uint32_t id, uint32_t storage) {
    if (id == 0 || id == 0xFFFFFFFFUL) return id;
    return id | (storage & 0xF0000000UL);
  }

  // Make an internal object id from an external one.
  uint32_t INT(uint32_t id) {
    if (id == 0 || id == 0xFFFFFFFFUL) return id;
    return id & 0xFFFFFFFUL;
  }

  uint32_t NextStorage(uint32_t store) {
    uint32_t i = store >> 28;
    while (true) {
      i++;
      if (i >= NELEM(storage_)) return 0;
      if (storage_[i]) return (i << 28) | 1;
    }
  }

  uint32_t FirstStorage() {
    for (size_t i = 0; i < NELEM(storage_); i++) {
      if (storage_[i]) return (i << 28) | 1;
    }
    return 0;
  }

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
      STDOUT.print("0123456789ABCDEF"[x->buf[i] >> 4]);
      STDOUT.print("0123456789ABCDEF"[x->buf[i] & 0xf]);
      if ((i & 3) == 3) STDOUT.print(" ");
    } 
    STDOUT.println("");
#endif
#if 0
    MTPContainer *tmp = (struct MTPContainer*)(x->buf);
    STDOUT.print(" len = ");
    STDOUT.print(tmp->len, HEX);
    STDOUT.print(" type = ");
    STDOUT.print(tmp->type, HEX);
    STDOUT.print(" op = ");
    STDOUT.print(tmp->op, HEX);
    STDOUT.print(" transaction_id = ");
    STDOUT.print(tmp->transaction_id, HEX);
    for (int i = 0; i * 4 < x->len - 12; i ++) {
      STDOUT.print(" p");
      STDOUT.print(i);
      STDOUT.print(" = ");
      STDOUT.print(tmp->params[i], HEX);
    }
    STDOUT.println("");
    STDOUT.flush();
    delay(10);
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
    write32(15);
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
    write16(0x100F);  // FormatStore

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
    int32_t num = 0;
    for (size_t i = FirstStorage(); i; i = NextStorage(i)) num++;
    write32(num);
    for (size_t i = FirstStorage(); i; i = NextStorage(i)) write32(i);
  }

  void GetStorageInfo(uint32_t storage) {
    write16(Stor(storage)->readonly() ? 0x0001 : 0x0004);   // storage type (removable RAM)
    write16(Stor(storage)->has_directories() ? 0x0002: 0x0001);   // filesystem type (generic hierarchical)
    write16(0x0000);   // access capability (read-write)
    write64(Stor(storage)->size());  // max capacity
    write64(Stor(storage)->free());  // free space (100M)
    write32(Stor(storage)->free_objects());  // free space (objects)
    writestring(Stor(storage)->GetName());  // storage descriptor
    writestring("");  // volume identifier
  }

  uint32_t GetNumObjects(uint32_t storage,
                         uint32_t parent) {
    uint32_t num = 0;
    if (storage == 0xFFFFFFFFUL) {
      for (size_t i = FirstStorage(); i; i = NextStorage(i)) {
        Stor(i)->StartGetObjectHandles(INT(parent));
        while (Stor(i)->GetNextObjectHandle()) num++;
      }
    } else {
      Stor(storage)->StartGetObjectHandles(INT(parent));
      while (Stor(storage)->GetNextObjectHandle()) num++;
    }
    return num;
  }

  void GetObjectHandles(uint32_t storage,
                        uint32_t parent) {
    uint32_t num = 0;
    int handle;
    if (!write_get_length_) {
      num = GetNumObjects(storage, parent);
    }
    write32(num);
    if (storage == 0xFFFFFFFFUL) {
      for (size_t i = FirstStorage(); i; i = NextStorage(i)) {
        Stor(i)->StartGetObjectHandles(INT(parent));
        while ((handle = Stor(i)->GetNextObjectHandle()))
          write32(EXT(handle, i));
      }
    } else {
      Stor(storage)->StartGetObjectHandles(INT(parent));
      while ((handle = Stor(storage)->GetNextObjectHandle()))
        write32(EXT(handle, storage));
    }
  }

  void GetObjectInfo(uint32_t handle) {
    char filename[256];
    uint32_t size, parent;
    Stor(handle)->GetObjectInfo(INT(handle), filename, &size, &parent);

    write32(handle & 0xF0000000UL); // storage
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
    uint32_t size = Stor(object_id)->GetSize(INT(object_id));
    if (write_get_length_) {
      write_length_ += size;
    } else {
      uint32_t pos = 0;
      while (pos < size) {
        get_buffer();
        uint32_t avail = sizeof(data_buffer_->buf) - data_buffer_->len;
        uint32_t to_copy = min(pos - size, avail);
        // Read directly from storage into usb buffer.
        Stor(object_id)->read(INT(object_id),
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

  uint32_t new_object;
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
    if (storage == 0) storage = FirstStorage();
    new_object = EXT(Stor(storage)->Create(INT(parent), INT(dir), filename), storage);
    return new_object;
  }

  void SendObject() {
    uint32_t len = ReadMTPHeader();
    Stor(new_object)->SendObject(len);
    while (len) {
      receive_buffer();
      uint32_t to_copy = data_buffer_->len - data_buffer_->index;
      to_copy = min(to_copy, len);
      Stor(new_object)->write((char*)(data_buffer_->buf + data_buffer_->index),
                              to_copy);
      data_buffer_->index += to_copy;
      len -= to_copy;
      if (data_buffer_->index == data_buffer_->len) {
        usb_free(data_buffer_);
        data_buffer_ = NULL;
      }
    }
    Stor(new_object)->close();
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
  void GetObjectPropValue(uint32_t handle, uint32_t prop) {
    switch (prop) {
      case 0xdc04: // size
        write64(Stor(handle)->GetSize(INT(handle)));
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
        CONTAINER->len = receive_buffer->len = 12;
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
                CONTAINER->len = receive_buffer->len = 16;
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
                if (!Stor(CONTAINER->params[0])->DeleteObject(
                      INT(CONTAINER->params[0]))) {
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
            case 0x100F:  // FormatStore
              if (!Stor(CONTAINER->params[0])->Format()) {
                return_code = 0x201D; // invalid parameter
              }
              break;
            case 0x1014:  // GetDevicePropDesc
              TRANSMIT(GetDevicePropDesc(CONTAINER->params[0]));
              break;
            case 0x1015:  // GetDevicePropvalue
              TRANSMIT(GetDevicePropValue(CONTAINER->params[0]));
              break;
            case 0x9803:  // GetObjectPropValue
              TRANSMIT(GetObjectPropValue(CONTAINER->params[0],
                                          CONTAINER->params[1]));
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

MTPD mtpd;

#ifdef ENABLE_SD
// Storage implementation for SD. SD needs to be already initialized.
class MTPStorage_SD : public MTPStorageInterface {
public:
  explicit MTPStorage_SD(MTPD* mtpd) { mtpd->AddStorage(this); }
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

  bool readonly() override { return false; }
  bool has_directories() override { return true; }
  uint64_t size() override { return 1 << 30; }
  uint64_t free() override { return 1 << 29; }
  uint32_t free_objects() override { return 0xFFFFFFFEUL; }

  const char* GetName() override {
    return "SD Card";
  }

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

MTPStorage_SD sd_storage(&mtpd);

#endif

#ifdef ENABLE_SERIALFLASH

// Need to expose a bunch of functions in SerialFlash to make this work..
// Also need to support multiple MTPStorage in the MTP responder.
class MTPStorage_SerialFlash : public MTPStorageInterface {
public:
  explicit MTPStorage_SerialFlash(MTPD* mtpd) { mtpd->AddStorage(this); }
private:
  int entry = 0;
  int last_entry_ = -1;
  int maxfiles_ = -1;
  uint16_t readHash(int index) {
    uint16_t hash;
    mtp_lock_storage(true);
    SerialFlash.read(8 + index * 2, &hash, 2);
    mtp_lock_storage(false);                           
    return hash;
  }

  int maxfiles() {
    if (maxfiles_ != -1) return maxfiles_;
    uint32_t sig[2];
    mtp_lock_storage(true);
    SerialFlash.read(0, sig, 8);
    mtp_lock_storage(false);
    if (sig[0] == 0xFA96554C) {
      maxfiles_ = sig[1] & 0xffff;
      return maxfiles_;
    }
    return 0;
  }
  
  int last_entry() {
    if (last_entry_ == -1) {
      int max = maxfiles();
      int min = -1;
      while (max - min > 1) {
        int mid = (max + min) / 2;
        if (readHash(mid) == 0xffff) {
          max = mid;
        } else {
          min = mid;
        }
      }
      last_entry_ = min;
    }
    return last_entry_;
  }

  SerialFlashFile open(int index) {
    SerialFlashFile file;
    uint32_t buf[2];
    mtp_lock_storage(true);
    SerialFlash.read(8 + maxfiles() * 2 + index * 10, buf, 8);
    mtp_lock_storage(false);
    file.address = buf[0];
    file.length = buf[1];
    file.offset = 0;
    file.dirindex = index;
    return file;
  }

public:
  // Return true if this storage is read-only
  bool readonly() override {
    return false;
  }
 
  // Does it have directories?
  bool has_directories() override {
    return false;
  }

  // Return size of storage in bytes.
  uint64_t size() override {
    uint8_t id[5];
    mtp_lock_storage(true);
    SerialFlash.readID(id);
    uint64_t ret = SerialFlashChip::capacity(id);
    mtp_lock_storage(false);
    return ret;
  }

  // Return free space in bytes.
  uint64_t free() override {
    int entry = last_entry();
    if (entry == -1) return size();
    SerialFlashFile last = open(entry);
    return size() - (last.getFlashAddress() + last.size());
  }

  uint32_t free_objects() override {
    return maxfiles() - last_entry() - 1;
  }

  const char* GetName() override {
    return "SerialFlash";
  }

  // parent = 0 means get all handles.
  // parent = 0xFFFFFFFF means get root folder.
  void StartGetObjectHandles(uint32_t parent) override {
    entry = 0;
  }
  
  uint32_t GetNextObjectHandle() override {
    while (true) {
      if (entry >= maxfiles()) return 0;
      uint16_t hash = readHash(entry);
      if (hash == 0xFFFF) return 0;
      if (hash == 0) continue; // deleted
      entry++;
      return entry;
    };
  }
  // Size should be 0xFFFFFFFF if it's a directory.
  void GetObjectInfo(uint32_t handle,
                     char* name,
                     uint32_t* size,
                     uint32_t* parent) override {
    handle--;
    uint32_t buf[3];
    buf[2] = 0;
    mtp_lock_storage(true);
    SerialFlash.read(8 + maxfiles() * 2 + handle * 10, buf, 10);
    mtp_lock_storage(false);
    int straddr = 8 + maxfiles() * 12 + buf[2] * 4;
    name[255] = 0;
    mtp_lock_storage(true);
    SerialFlash.read(straddr, name, 255); // TODO max filename length
    mtp_lock_storage(false);
    *size = buf[1];
    *parent = 0; // Top level
  }
  uint32_t GetSize(uint32_t handle) override {
    handle--;
    return open(handle).size();
  }
  void read(uint32_t handle,
            uint32_t pos,
            char* buffer,
            uint32_t bytes) override {
    handle--;
    SerialFlashFile file = open(handle);
    mtp_lock_storage(true);
    file.seek(pos);
    file.read(buffer, bytes);
    mtp_lock_storage(false);
  }
  char new_filename[256];
  uint32_t Create(uint32_t parent,
                  bool folder,
                  const char* filename) override {
    if (strlen(filename) > 255) return 0;
    strcpy(new_filename, filename);
    return last_entry() + 1 + 1;
  }
  SerialFlashFile file;
  virtual void SendObject(uint32_t length) {
    last_entry_++;
    mtp_lock_storage(true);
    SerialFlash.create(new_filename, length);
    mtp_lock_storage(false);
    file = open(last_entry_);
  }
  void write(const char* data, uint32_t size) override {
    mtp_lock_storage(true);
    file.write(data, size);
    mtp_lock_storage(false);
  }
  void close() override {
  }
  bool DeleteObject(uint32_t object) override {
    object--;
    SerialFlashFile file = open(object);
    mtp_lock_storage(true);
    bool ret = SerialFlashChip::remove(file);
    mtp_lock_storage(false);
    return ret;
  }
  virtual bool Format() {
    mtp_lock_storage(true);
    last_entry_ = -1;
    maxfiles_ = -1;
    SerialFlashChip::eraseAll();
    while (!SerialFlashChip::ready()) {
      mtp_lock_storage(false);
      mtp_yield();
      mtp_lock_storage(true);
    }
    SerialFlash.open("bogus"); // creates signature
    mtp_lock_storage(false);
    return true;
  }
};

MTPStorage_SerialFlash serialflash_storage(&mtpd);

#endif

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
