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

#define CONFIG_FILE "default_v3_config.h"
// #define CONFIG_FILE "crossguard_config.h"
// #define CONFIG_FILE "graflex_v1_config.h"
// #define CONFIG_FILE "owk_v2_config.h"
// #define CONFIG_FILE "test_bench_config.h"
// #define CONFIG_FILE "toy_saber_config.h"
// #define CONFIG_FILE "new_config.h"


#ifdef CONFIG_FILE_TEST
#undef CONFIG_FILE
#define CONFIG_FILE CONFIG_FILE_TEST
#endif

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

#include "common/state_machine.h"

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

#include "common/scoped_cycle_counter.h"

uint64_t audio_dma_interrupt_cycles = 0;
uint64_t wav_interrupt_cycles = 0;
uint64_t loop_cycles = 0;

#include "common/loop_counter.h"

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))

// Magic type used to prevent linked-list types from automatically linking.
enum NoLink { NOLINK = 17 };

#include "common/looper.h"
#include "common/command_parser.h"

CommandParser* parsers = NULL;

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

#include "common/vec3.h"

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

#include "common/saber_base.h"

SaberBase* saberbases = NULL;
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

#include "common/sin_table.h"

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


#include "sound/dac.h"
#include "sound/dynamic_mixer.h"
#include "sound/beeper.h"
#include "sound/talkie.h"
#include "sound/lightsaber_synth.h"

AudioDynamicMixer<8> dynamic_mixer;
Beeper beeper;
Talkie talkie;

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


#endif

#include "sound/buffered_audio_stream.h"

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

#include "sound/effect.h"

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

size_t WhatUnit(class BufferedWavPlayer* player);

#include "sound/buffered_wav_player.h"

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

#include "sound/audio_splicer.h"

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

#include "sound/monophonic_font.h"

MonophonicFont monophonic_font;

// Reads an config file, looking for variable assignments.
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

  void ReadInCurrentDir(const char* name) {
    char full_name[128];
    strcpy(full_name, current_directory);
    strcat(full_name, name);
    Read(full_name);
  }
};

#include "sound/polyphonic_font.h"  

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

#include "sound/looped_swing_wrapper.h"
#include "sound/smooth_swing_v2.h"

LoopedSwingWrapper looped_swing_wrapper;
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

#include "blades/monopodws.h"

class BladeBase;

// Base class for blade styles.
// Blade styles are responsible for the colors and patterns
// of the colors of the blade. Each time run() is called, the
// BladeStyle shouldl call blade->set() to update the color
// of all the LEDs in the blade.
class BladeStyle {
public:
  virtual ~BladeStyle() {}
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

  virtual BladeStyle* UnSetStyle() {
    BladeStyle *ret = current_style_;
    if (ret) {
      ret->deactivate();
    }
    current_style_ = nullptr;
    return ret;
  }
  virtual void SetStyle(BladeStyle* style) {
    // current_style should be nullptr;
    current_style_ = style;
    if (current_style_) {
      current_style_->activate();
    }
  }

  BladeStyle* current_style() const {
    return current_style_;
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


class StyleFactory {
public:
  virtual BladeStyle* make() = 0;
};


template<class STYLE>
class StyleFactoryImpl : public StyleFactory {
  BladeStyle* make() override {
    return new STYLE();
  }
};

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

template<class T, class U>
struct is_same_type { static const bool value = false; };
 
template<class T>
struct is_same_type<T, T> { static const bool value = true; };

#include "styles/rgb.h"
#include "styles/charging.h"
#include "styles/fire.h"
#include "styles/gradient.h"
#include "styles/random_flicker.h"
#include "styles/random_per_led_flicker.h"
#include "styles/audio_flicker.h"
#include "styles/brown_noise_flicker.h"
#include "styles/hump_flicker.h"
#include "styles/rainbow.h"
#include "styles/color_cycle.h"
#include "styles/cylon.h"
#include "styles/ignition_delay.h"
#include "styles/pulsing.h"
#include "styles/on_spark.h"
#include "styles/rgb_cycle.h"
#include "styles/simple_clash.h"
#include "styles/lockup.h"  // Also does "drag"
#include "styles/blast.h"
#include "styles/strobe.h"
#include "styles/inout_helper.h"
#include "styles/inout_sparktip.h"
#include "styles/colors.h"

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

// This really ought to be a typedef, but it causes problems I don't understand.
#define StyleAllocator class StyleFactory*

// Get a pointer to class.
template<class STYLE>
StyleAllocator StylePtr() {
  static StyleFactoryImpl<Style<STYLE> > factory;
  return &factory;
};


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
StyleAllocator StyleNormalPtr() {
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
StyleAllocator StyleRainbowPtr() {
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
StyleAllocator StyleStrobePtr() {
  typedef Strobe<BLACK, strobe_color, frequency, 1> strobe;
  typedef Strobe<BLACK, strobe_color, 3* frequency, 1> fast_strobe;
  typedef Lockup<strobe, fast_strobe> AddLockup;
  typedef SimpleClash<AddLockup, clash_color> clash;
  return StylePtr<InOutHelper<clash, out_millis, in_millis> >();
}

#include "styles/pov.h"

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


class NoLED;

#include "blades/ws2811_blade.h"
#include "blades/fastled_blade.h"
#include "blades/simple_blade.h"
#include "blades/string_blade.h"
#include "blades/sub_blade.h"

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

#if NUM_BLADES == 1
#define ONCEPERBLADE(F) F(1)
#elif NUM_BLADES == 2
#define ONCEPERBLADE(F) F(1) F(2)
#elif NUM_BLADES == 3
#define ONCEPERBLADE(F) F(1) F(2) F(3)
#elif NUM_BLADES == 4
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4)
#elif NUM_BLADES == 5
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5)
#elif NUM_BLADES == 6
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6)
#elif NUM_BLADES == 7
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7)
#elif NUM_BLADES == 8
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8)
#elif NUM_BLADES == 9
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9)
#elif NUM_BLADES == 10
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10)
#else
#error NUM_BLADES is too big
#endif

struct Preset {
  // Sound font.
  const char* font;

  // Sound track
  const char* track;

  // Blade config.
#define DEFINE_BLADE_STYLES(N) StyleAllocator style_allocator##N;
  ONCEPERBLADE(DEFINE_BLADE_STYLES);

  const char* name;
};

struct BladeConfig {
  // Blade identifier resistor.
  int ohm;

  // Blade driver.
#define DEFINE_BLADES(N) BladeBase* blade##N;
  ONCEPERBLADE(DEFINE_BLADES);

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

  BladeStyle* current_style(){
    return current_config_->blade1->current_style();
  }

  bool NeedsPower() {
    if (on_) return true;
    if (current_style() && current_style()->NoOnOff())
      return true;
    return false;
  }

  void On() {
    if (on_) return;
    if (current_style() && current_style()->NoOnOff())
      return;
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

    // First free all styles, then allocate new ones to avoid memory
    // fragmentation.
#define UNSET_BLADE_STYLE(N) \
    delete current_config_->blade##N->UnSetStyle();
    ONCEPERBLADE(UNSET_BLADE_STYLE)
#define SET_BLADE_STYLE(N) \
    current_config_->blade##N->SetStyle(preset->style_allocator##N->make());
    ONCEPERBLADE(SET_BLADE_STYLE)
    chdir(preset->font);
  }

  // Go to the next Preset.
  void next_preset() {
    if (current_config_->num_presets == 1)
      return;
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

#define ACTIVATE(N) do {     \
    if (!current_config_->blade##N) goto bad_blade;  \
    current_config_->blade##N->Activate();           \
  } while(0);

    ONCEPERBLADE(ACTIVATE);
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
      if (current_preset_->style_allocator1 != &style_charging) {
        if (on_) {
          STDOUT.print("Battery low, turning off. Battery voltage: ");
          STDOUT.println(battery_monitor.battery());
          Off();
        } else if (millis() - last_beep_ > 5000) {
          STDOUT.println("Battery low beep");
#ifdef ENABLE_AUDIO
          // TODO: allow this to be replaced with WAV file
          talkie.Say(spLOW);
          talkie.Say(spPOWER);
#endif
	  last_beep_ = millis();
        }
      }
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
    if (!handled) {
      // Events that needs to be handled regardless of what other buttons
      // are pressed.
      switch (EVENTID(button, event, on_ ? MODE_ON : MODE_OFF)) {
	case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ON):
	case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ON):
	  if (SaberBase::Lockup()) {
	    SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
	    SaberBase::DoEndLockup();
	  } else {
	    handled = false;
	  }
        break;

      }
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

#include "buttons/latching_button.h"
#include "buttons/button.h"
#include "buttons/touchbutton.h"

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
      f.close();
      LOCK_SD(false);
      STDOUT.println("Done");
      return true;
    }
    if (!strcmp(cmd, "sdtest")) {
      char filename[128];
      uint8_t block[512];
      if (!hum.Play(filename)) {
        STDOUT.println("hum file not found, try cd <fontir>");
	return true;
      }
      LOCK_SD(true);
      File f = LSFS::Open(filename);
      if (!f) {
        STDOUT.println("Unable to open hum file.");
      } else {
	STDOUT.println("Each dot is 64kB");
	uint32_t start_millis = millis();
	int bytes = 0;
	for (int k = 0; k < 8; k++) {
	  for (int j = 0; j < 16; j++) {
	    f.seek(0);
	    int tmp = 0;
	    for (int i = 0; i < 128; i++) {
	      tmp += f.read(block, 512);
	    }
	    STDOUT.print(tmp == 0x10000 ? "." : "!");
	    bytes += tmp;
	  }
	  STDOUT.println("");
	}
	f.close();
	uint32_t end_millis = millis();
	STDOUT.println("Done");
	// bytes per ms = kb per s (note, not kibibytes)
	float kb_per_sec = bytes / (float)(end_millis - start_millis);
	STDOUT.println("SD card speed: ");
	STDOUT.print(kb_per_sec);
	STDOUT.print(" kb/s = ");
	STDOUT.print(kb_per_sec / 88.2);
	STDOUT.println(" simultaneous audio streams.");
      }
      LOCK_SD(false);
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
#ifdef TEENSYDUINO
      if (!(ARM_DWT_CTRL & ARM_DWT_CTRL_CYCCNTENA)) {
        ARM_DEMCR |= ARM_DEMCR_TRCENA;
        ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
        STDOUT.println("Cycle counting enabled, top will work next time.");
        return true;
      }
#else
      if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        CoreDebug->DEMCR |= DEMCR_TRCENA_Msk;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        STDOUT.println("Cycle counting enabled, top will work next time.");
        return true;
      }
#endif

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

#include "motion/mpu6050.h"
#include "motion/lsm6ds3h.h"
#include "motion/fxos8700.h"
#include "motion/fxas21002.h"

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

#include "mtp/mtpd.h"
MTPD mtpd;

#ifdef ENABLE_SD
#include "mtp/mtp_storage_sd.h"
MTPStorage_SD sd_storage(&mtpd);
#endif

#ifdef ENABLE_SERIALFLASH
#include "mtp/mtp_storage_serialflash.h"
MTPStorage_SerialFlash serialflash_storage(&mtpd);
#endif

#endif  // MTP_RX_ENDPOINT

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
