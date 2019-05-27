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

// #define CONFIG_FILE "config/default_proffieboard_config.h"
// #define CONFIG_FILE "config/default_v3_config.h"
// #define CONFIG_FILE "config/crossguard_config.h"
// #define CONFIG_FILE "config/graflex_v1_config.h"
// #define CONFIG_FILE "config/prop_shield_fastled_v1_config.h"
// #define CONFIG_FILE "config/owk_v2_config.h"
// #define CONFIG_FILE "config/test_bench_config.h"
// #define CONFIG_FILE "config/toy_saber_config.h"
// #define CONFIG_FILE "config/proffieboard_v1_test_bench_config.h"
#define CONFIG_FILE "config/td_pb1_config.h"


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
// Generally speaking, there are usually two registered SaberBase
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

#ifdef TEENSYDUINO
#include <DMAChannel.h>
#include <usb_dev.h>
#include <kinetis.h>
#include <i2c_t3.h>
#include <SD.h>

#define INPUT_ANALOG INPUT
#else

// This is a hack to let me access the internal stuff..
#define private public
#include <Wire.h>
#undef private

#include <FS.h>
#define digitalWriteFast digitalWrite
#include <stm32l4_wiring_private.h>
#include <stm32l4xx.h>
#include <armv7m.h>
#include <stm32l4_gpio.h>
#include <stm32l4_sai.h>
#include <stm32l4_dma.h>
#include <stm32l4_system.h>
#include <arm_math.h>
#include <STM32.h>
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
#include "common/monitoring.h"
#include "common/stdout.h"

Monitoring monitor;
DEFINE_COMMON_STDOUT_GLOBALS;

void PrintQuotedValue(const char *name, const char* str) {
  STDOUT.print(name);
  STDOUT.write('=');
  if (str) {
    while (*str) {
      switch (*str) {
        case '\n':
          STDOUT.print("\\n");
          break;
        case '\t':
          STDOUT.print("\\t");
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

const char* mkstr(const char* str) {
  int len = strlen(str);
  char* ret = (char*)malloc(len + 1);
  if (!ret) return "";
  memcpy(ret, str, len + 1);
  return ret;
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

#endif

#include "common/scoped_cycle_counter.h"

uint64_t audio_dma_interrupt_cycles = 0;
uint64_t wav_interrupt_cycles = 0;
uint64_t loop_cycles = 0;

#include "common/loop_counter.h"

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))

#include "common/linked_list.h"
#include "common/looper.h"
#include "common/command_parser.h"
#include "common/monitor_helper.h"

CommandParser* parsers = NULL;
MonitorHelper monitor_helper;

#include "common/vec3.h"
#include "common/ref.h"

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
  MODE_ANY_BUTTON = 256,
  MODE_ON = 512,
  MODE_OFF = 0,
};

enum EVENT : uint32_t {
  EVENT_NONE = 0,
  EVENT_PRESSED,
  EVENT_RELEASED,
  EVENT_HELD,
  EVENT_HELD_LONG,
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

#define EVENTID(BUTTON, EVENT, MODIFIERS) (((EVENT) << 24) | ((BUTTON) << 12) | ((MODIFIERS) & ~(BUTTON)))

uint32_t current_modifiers = BUTTON_NONE;

#include "common/saber_base.h"
#include "common/saber_base_passthrough.h"

SaberBase* saberbases = NULL;
SaberBase::LockupType SaberBase::lockup_ = SaberBase::LOCKUP_NONE;
bool SaberBase::on_ = false;
uint32_t SaberBase::last_motion_request_ = 0;

#include "common/box_filter.h"

// Returns the decimals of a number, ie 12.2134 -> 0.2134
float fract(float x) { return x - floorf(x); }

// clamp(x, a, b) makes sure that x is between a and b.
float clamp(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
float Fmod(float a, float b) {
  return a - floorf(a / b) * b;
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

void EnableBooster();
void EnableAmplifier();
void MountSDCard();

#include "common/lsfs.h"
#ifdef ENABLE_AUDIO

#include "sound/click_avoider_lin.h"
#include "sound/waveform_sampler.h"
#include "sound/audiostream.h"
#include "sound/dac.h"
#include "sound/dynamic_mixer.h"
#include "sound/beeper.h"
#include "sound/talkie.h"
#include "sound/lightsaber_synth.h"

AudioDynamicMixer<9> dynamic_mixer;
Beeper beeper;
Talkie talkie;

// LightSaberSynth saber_synth;
#include "sound/buffered_audio_stream.h"

#else  // ENABLE_AUDIO

#include "common/sd_card.h"
#define LOCK_SD(X) do { } while(0)

#endif  // ENABLE_AUDIO

#include "common/strfun.h"

char current_directory[128];

#include "sound/effect.h"

#ifdef ENABLE_AUDIO

size_t WhatUnit(class BufferedWavPlayer* player);

#include "sound/buffered_wav_player.h"

BufferedWavPlayer wav_players[6];
RefPtr<BufferedWavPlayer> track_player_;

RefPtr<BufferedWavPlayer> GetFreeWavPlayer()  {
  // Find a free wave playback unit.
  for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
    if (wav_players[unit].Available()) {
      wav_players[unit].reset_volume();
      return RefPtr<BufferedWavPlayer>(wav_players + unit);
    }
  }
  return RefPtr<BufferedWavPlayer>();
}

RefPtr<BufferedWavPlayer> RequireFreeWavPlayer()  {
  while (true) {
    RefPtr<BufferedWavPlayer> ret = GetFreeWavPlayer();
    if (ret) return ret;
    STDOUT.println("Failed to get hum player, trying again!");
    delay(100);
  }
}

size_t WhatUnit(class BufferedWavPlayer* player) {
  if (!player) return -1;
  return player - wav_players;
}

void SetupStandardAudioLow() {
//  audio_splicer.Deactivate();
  for (size_t i = 0; i < NELEM(wav_players); i++) {
    if (wav_players[i].refs() != 0) {
      STDOUT.println("WARNING, wav player still referenced!");
    }
    dynamic_mixer.streams_[i] = wav_players + i;
    wav_players[i].reset_volume();
  }
  dynamic_mixer.streams_[NELEM(wav_players)] = &beeper;
  dynamic_mixer.streams_[NELEM(wav_players)+1] = &talkie;
}

void SetupStandardAudio() {
  dac.SetStream(NULL);
  SetupStandardAudioLow();
  dac.SetStream(&dynamic_mixer);
}


#include "common/config_file.h"
#include "sound/hybrid_font.h"

HybridFont hybrid_font;

class SmoothSwingConfigFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(Version, 1);
    CONFIG_VARIABLE(SwingSensitivity, 450.0f);
    CONFIG_VARIABLE(MaximumHumDucking, 75.0f);
    CONFIG_VARIABLE(SwingSharpness, 1.75f);
    CONFIG_VARIABLE(SwingStrengthThreshold, 20.0f);
    CONFIG_VARIABLE(Transition1Degrees, 45.0f);
    CONFIG_VARIABLE(Transition2Degrees, 160.0f);
    CONFIG_VARIABLE(MaxSwingVolume, 3.0f);
    CONFIG_VARIABLE(AccentSwingSpeedThreshold, 0.0f);
  };

  int  Version;
  float SwingSensitivity;
  float MaximumHumDucking;
  float SwingSharpness;
  float SwingStrengthThreshold;
  float Transition1Degrees;
  float Transition2Degrees;
  float MaxSwingVolume;
  float AccentSwingSpeedThreshold;
};

SmoothSwingConfigFile smooth_swing_config;

#include "sound/looped_swing_wrapper.h"
#include "sound/smooth_swing_v2.h"

LoopedSwingWrapper looped_swing_wrapper;
SmoothSwingV2 smooth_swing_v2;

#endif  // ENABLE_AUDIO

#include "common/battery_monitor.h"
#include "common/color.h"
#include "common/range.h"
#include "blades/blade_base.h"
#include "blades/blade_wrapper.h"

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

template<class T, class U>
struct is_same_type { static const bool value = false; };
 
template<class T>
struct is_same_type<T, T> { static const bool value = true; };

// This really ought to be a typedef, but it causes problems I don't understand.
#define StyleAllocator class StyleFactory*

#include "styles/rgb.h"
#include "styles/rgb_arg.h"
#include "styles/charging.h"
#include "styles/fire.h"
#include "styles/sparkle.h"
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
#include "styles/blinking.h"
#include "styles/on_spark.h"
#include "styles/rgb_cycle.h"
#include "styles/clash.h"
#include "styles/lockup.h"  // Also does "drag"
#include "styles/blast.h"
#include "styles/strobe.h"
#include "styles/inout_helper.h"
#include "styles/inout_sparktip.h"
#include "styles/colors.h"
#include "styles/mix.h"
#include "styles/style_ptr.h"
#include "styles/file.h"
#include "styles/stripes.h"
#include "styles/random_blink.h"
#include "styles/sequence.h"

// functions
#include "functions/ifon.h"
#include "functions/change_slowly.h"
#include "functions/int.h"
#include "functions/int_arg.h"
#include "functions/sin.h"
#include "functions/scale.h"
#include "functions/battery_level.h"
#include "functions/trigger.h"
#include "functions/bump.h"

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

// Arguments: color, clash color, turn-on/off time
template<class base_color,
         class clash_color,
         class out_millis,
         class in_millis,
         class lockup_flicker_color = WHITE,
         class blast_color = WHITE>
StyleAllocator StyleNormalPtrX() {
  typedef AudioFlicker<base_color, lockup_flicker_color> AddFlicker;
  typedef Blast<base_color, blast_color> AddBlast;
  typedef Lockup<AddBlast, AddFlicker> AddLockup;
  typedef SimpleClash<AddLockup, clash_color> AddClash;
  return StylePtr<InOutHelperX<AddClash, InOutFuncX<out_millis, in_millis>> >();
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

// Rainbow blade.
// Arguments: color, clash color, turn-on/off time
template<class out_millis,
          class in_millis,
          class clash_color = WHITE,
          class lockup_flicker_color = WHITE>
StyleAllocator StyleRainbowPtrX() {
  typedef AudioFlicker<Rainbow, lockup_flicker_color> AddFlicker;
  typedef Lockup<Rainbow, AddFlicker> AddLockup;
  typedef SimpleClash<AddLockup, clash_color> AddClash;
  return StylePtr<InOutHelperX<AddClash, InOutFuncX<out_millis, in_millis>> >();
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

class NoLED;

#include "blades/power_pin.h"
#include "blades/drive_logic.h"
#include "blades/pwm_pin.h"
#include "blades/ws2811_blade.h"
#include "blades/fastled_blade.h"
#include "blades/simple_blade.h"
#include "blades/sub_blade.h"
#include "blades/leds.h"
#include "common/preset.h"
#include "common/blade_config.h"
#include "common/current_preset.h"
#include "styles/style_parser.h"

BladeConfig* current_config = nullptr;
ArgParserInterface* CurrentArgParser;

#define CONFIG_PRESETS
#include CONFIG_FILE
#undef CONFIG_PRESETS

#define CONFIG_PROP
#include CONFIG_FILE
#undef CONFIG_PROP

#ifndef PROP_TYPE
#include "props/saber.h"
#endif

PROP_TYPE prop;

#include "scripts/v3_test_script.h"
#include "scripts/proffieboard_test_script.h"

#if 0
#warning !!! V3 TEST SCRIPT ACTIVE !!!
V3TestScript script;
#endif

#if 0
#warning !!! PROFFIEBOARD TEST SCRIPT ACTIVE !!!
V4TestScript script;
Blinker1 blinker1;
Blinker2 blinker2;
CapTest captest;
#endif

#include "buttons/latching_button.h"
#include "buttons/button.h"
#ifdef TEENSYDUINO
#include "buttons/touchbutton.h"
#else
#include "buttons/stm32l4_touchbutton.h"

uint32_t startup_AHB1ENR;
uint32_t startup_AHB2ENR;
uint32_t startup_AHB3ENR;
uint32_t startup_APB1ENR1;
uint32_t startup_APB1ENR2;
uint32_t startup_APB2ENR;
uint32_t startup_MODER[4];

#endif

#define CONFIG_BUTTONS
#include CONFIG_FILE
#undef CONFIG_BUTTONS

class Commands : public CommandParser {
 public:
  enum PinType {
    PinTypeFloating,
    PinTypePulldown,
    PinTypeCap,
    PinTypeOther,
  };

  bool TestPin(int pin, PinType t) {
    int ret = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delayMicroseconds(20);
    ret <<= 1;
    ret |= digitalRead(pin);

    digitalWrite(pin, HIGH);
    delayMicroseconds(20);
    ret <<= 1;
    ret |= digitalRead(pin);

    // Discharge time
    pinMode(pin, INPUT_PULLDOWN);
    uint32_t start = micros();
    uint32_t end;
    while (digitalRead(pin)) {
      end = micros();
      if (end - start > 32768) break; // 32 millis
    }
    ret <<= 16;
    ret |= (end - start);

    pinMode(pin, INPUT_PULLUP);
    delayMicroseconds(20);
    ret <<= 1;
    ret |= digitalRead(pin);
    pinMode(pin, INPUT);

    return ret;
  }
  bool Parse(const char* cmd, const char* e) override {
    if (!strcmp(cmd, "help")) {
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
      if (!e || LSFS::Exists(e)) {
        for (LSFS::Iterator dir(e ? e : ""); dir; ++dir) {
          STDOUT.print(dir.name());
          STDOUT.print(" ");
          STDOUT.println(dir.size());
        }
        STDOUT.println("Done listing files.");
      } else {
        STDOUT.println("No such directory.");
      }
      LOCK_SD(false);
      return true;
    }
    
    if (!strcmp(cmd, "cat") && e) {
      LOCK_SD(true);
      File f = LSFS::Open(e);
      while (f.available()) {
        STDOUT.write(f.read());
      }
      f.close();
      LOCK_SD(false);
      return true;
    }
    
    if (!strcmp(cmd, "del") && e) {
      LOCK_SD(true);
      LSFS::Remove(e);
      LOCK_SD(false);
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
    if (!strcmp(cmd, "high") && e) {
      digitalWrite(atoi(e), HIGH);
      STDOUT.println("Ok.");
      return true;
    }
    if (!strcmp(cmd, "low") && e) {
      digitalWrite(atoi(e), LOW);
      STDOUT.println("Ok.");
      return true;
    }
#if VERSION_MAJOR >= 4
    if (!strcmp(cmd, "booster")) {
       if (!strcmp(e, "on")) {
         digitalWrite(boosterPin, HIGH);
         STDOUT.println("Booster on.");
         return true;
       }
       if (!strcmp(e, "off")) {
         digitalWrite(boosterPin, LOW);
         STDOUT.println("Booster off.");
         return true;
       }
    }
#endif
#ifdef ENABLE_AUDIO
#if 0
    if (!strcmp(cmd, "ton")) {
      EnableAmplifier();
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
    if (!strcmp(cmd, "make_default_console")) {
      default_output = stdout_output;
      return true;
    }
#if 0
    // Not finished yet
    if (!strcmp(cmd, "selftest")) {
      struct PinDefs { int8_t pin; PinType type; };
      static PinDefs pin_defs[]  = {
        { bladePowerPin1, PinTypePulldown },
        { bladePowerPin2, PinTypePulldown },
        { bladePowerPin3, PinTypePulldown },
        { bladePowerPin4, PinTypePulldown },
        { bladePowerPin5, PinTypePulldown },
        { bladePowerPin6, PinTypePulldown },
        { bladePin,       PinTypeOther },
        { blade2Pin,      PinTypeFloating },
        { blade3Pin,      PinTypeFloating },
        { blade4Pin,      PinTypeFloating },
        { blade5Pin,      PinTypeFloating },
        { amplifierPin,   PinTypeFloating },
        { boosterPin,     PinTypeFloating },
        { powerButtonPin, PinTypeFloating },
        { auxPin,         PinTypeFloating },
        { aux2Pin,        PinTypeFloating },
        { rxPin,          PinTypeOther },
        { txPin,          PinTypeFloating },
      };
      for (size_t test_index = 0; test_index < NELEM(pin_defs); test_index++) {
        int pin = pin_defs[test_index].pin;
        for (size_t i = 0; i < NELEM(pin_defs); i++)
          pinMode(pin_defs[i].pin, INPUT);

        // test
        for (size_t i = 0; i < NELEM(pin_defs); i++) {
          pinMode(pin_defs[i].pin, OUTPUT);
          digitalWrite(pin_defs[i].pin, HIGH);
          // test
          digitalWrite(pin_defs[i].pin, LOW);
          // test
          pinMode(pin_defs[i].pin, INPUT);
        }
      }
    }
#endif
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
        CoreDebug->DEMCR |= 1<<24; // DEMCR_TRCENA_Msk;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        STDOUT.println("Cycle counting enabled, top will work next time.");
        return true;
      }
#endif

      // TODO: list cpu usage for various objects.
      float total_cycles =
        (float)(audio_dma_interrupt_cycles +
                 wav_interrupt_cycles +
                 loop_cycles);
      STDOUT.print("Audio DMA: ");
      STDOUT.print(audio_dma_interrupt_cycles * 100.0f / total_cycles);
      STDOUT.println("%");
      STDOUT.print("Wav reading: ");
      STDOUT.print(wav_interrupt_cycles * 100.0f / total_cycles);
      STDOUT.println("%");
      STDOUT.print("LOOP: ");
      STDOUT.print(loop_cycles * 100.0f / total_cycles);
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
    if (!strcmp(cmd, "reset")) {
#ifdef TEENSYDUINO    
      SCB_AIRCR = 0x05FA0004;
#else
      STM32.reset();
#endif      
      STDOUT.println("Reset failed.");
      return true;
    }
#ifndef TEENSYDUINO    
    if (!strcmp(cmd, "shutdown")) {
      STDOUT.println("Sleeping 10 seconds.\n");
      STM32.stop(100000);
      return true;
    }
    if (!strcmp(cmd, "stm32info")) {
      STDOUT.print("VBAT: ");
      STDOUT.println(STM32.getVBAT());
      STDOUT.print("VREF: ");
      STDOUT.println(STM32.getVREF());
      STDOUT.print("TEMP: ");
      STDOUT.println(STM32.getTemperature());
      return true;
    }
    if (!strcmp(cmd, "portstates")) {
      GPIO_TypeDef *GPIO;
      for (int i = 0; i < 4; i++) {
	switch (i) {
	  case 0:
	    GPIO = (GPIO_TypeDef *)GPIOA_BASE;
	    STDOUT.print("PORTA: ");
	    break;
	  case 1:
	    GPIO = (GPIO_TypeDef *)GPIOB_BASE;
	    STDOUT.print("PORTB: ");
	    break;
	  case 2:
	    GPIO = (GPIO_TypeDef *)GPIOC_BASE;
	    STDOUT.print("PORTC: ");
	    break;
	  case 3:
	    GPIO = (GPIO_TypeDef *)GPIOH_BASE;
	    STDOUT.print("PORTH: ");
	    break;
	}
	for (int j = 15; j >= 0; j--) {
	  uint32_t now = (GPIO->MODER >> (j * 2)) & 3;
	  uint32_t saved = (startup_MODER[i] >> (j * 2)) & 3;
	  STDOUT.print((now == saved ? "ioga" : "IOGA")[now]);
	  if (!(j & 3)) STDOUT.print(" ");
	}
	STDOUT.println("");
      }
      return true;
    }
    if (!strcmp(cmd, "whatispowered")) {
      STDOUT.print("ON: ");
#define PRINTIFON(REG, BIT) do {					\
	if (RCC->REG & RCC_##REG##_##BIT##EN) {				\
          STDOUT.print(" " #BIT);					\
          if (!(startup_##REG & RCC_##REG##_##BIT##EN)) STDOUT.print("+"); \
        }								\
      } while(0)
      
      PRINTIFON(AHB1ENR,FLASH);
      PRINTIFON(AHB1ENR,DMA1);
      PRINTIFON(AHB1ENR,DMA2);
      PRINTIFON(AHB2ENR,GPIOA);
      PRINTIFON(AHB2ENR,GPIOB);
#if defined(STM32L433xx) || defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(AHB2ENR,GPIOC);
      PRINTIFON(AHB2ENR,GPIOD);
      PRINTIFON(AHB2ENR,GPIOE);
#endif
#if defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(AHB2ENR,GPIOF);
      PRINTIFON(AHB2ENR,GPIOG);
#endif
      PRINTIFON(AHB2ENR,GPIOH);
#if defined(STM32L496xx)
      PRINTIFON(AHB2ENR,GPIOI);
#endif
      PRINTIFON(AHB2ENR,ADC);
      PRINTIFON(APB1ENR1,DAC1);
#if defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(AHB2ENR,OTGFS);
#else
      PRINTIFON(APB1ENR1,USBFS);
#endif
      PRINTIFON(APB2ENR,USART1);
      PRINTIFON(APB1ENR1,USART2);
#if defined(STM32L433xx) || defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB1ENR1,USART3);
#endif
#if defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB1ENR1,UART4);
      PRINTIFON(APB1ENR1,UART5);
#endif
      PRINTIFON(APB1ENR2,LPUART1);
      PRINTIFON(APB1ENR1,I2C1);
#if defined(STM32L433xx) || defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB1ENR1,I2C2);
#endif
      PRINTIFON(APB1ENR1,I2C3);
#if defined(STM32L496xx)
      PRINTIFON(APB1ENR2,I2C4);
#endif
      PRINTIFON(APB2ENR,SPI1);
#if defined(STM32L433xx) || defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB1ENR1,SPI2);
#endif
      PRINTIFON(APB1ENR1,SPI3);
      PRINTIFON(APB1ENR1,CAN1);
#if defined(STM32L496xx)
      PRINTIFON(APB1ENR1,CAN2);
#endif
      PRINTIFON(AHB3ENR,QSPI);
#if defined(STM32L433xx) || defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB2ENR,SDMMC1);
#endif
      PRINTIFON(APB2ENR,SAI1);
#if defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB2ENR,SAI2);
      PRINTIFON(APB2ENR,DFSDM1);
#endif
      PRINTIFON(APB2ENR,TIM1);
      PRINTIFON(APB1ENR1,TIM2);
#if defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB1ENR1,TIM3);
      PRINTIFON(APB1ENR1,TIM4);
      PRINTIFON(APB1ENR1,TIM5);
#endif
      PRINTIFON(APB1ENR1,TIM6);
      PRINTIFON(APB1ENR1,TIM7);
#if defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB2ENR,TIM8);
#endif
      PRINTIFON(APB2ENR,TIM15);
      PRINTIFON(APB2ENR,TIM16);
#if defined(STM32L476xx) || defined(STM32L496xx)
      PRINTIFON(APB2ENR,TIM17);
#endif
      PRINTIFON(APB1ENR1,LPTIM1);
      PRINTIFON(APB1ENR2,LPTIM2);

      STDOUT.println("");
      return true;
    }
#endif  // TEENSYDUINO
    
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
  static void begin() {
    // Already configured in Setup().
    // Serial.begin(115200);
  }
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

#ifdef USB_CLASS_WEBUSB
class WebUSBSerialAdapter {
public:
  static void begin() { WebUSBSerial.begin(115200); }
  // static bool Connected() { return !!WebUSBSerial; }
  static bool Connected() { return true; }
  static bool AlwaysConnected() { return true; }
  static Stream& stream() { return WebUSBSerial; }
  static const char* response_header() { return "-+=BEGIN_OUTPUT=+-\n"; }
  static const char* response_footer() { return "-+=END_OUTPUT=+-\n"; }
};
#endif

// Command-line parser. Easiest way to use it is to start the arduino
// serial monitor.
template<class SA> /* SA = Serial Adapter */
class Parser : Looper, StateMachine {
public:
  Parser() : Looper() {}
  const char* name() override { return "Parser"; }

  void Setup() override {
    SA::begin();
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!SA::Connected()) YIELD();
      if (!SA::AlwaysConnected()) {
        STDOUT.println("Welcome to ProffieOS, type 'help' for more info.");
      }

      while (SA::Connected()) {
        while (!SA::stream().available()) YIELD();
        int c = SA::stream().read();
        if (c < 0) { break; }
#if 0   
        STDOUT.print("GOT:");
        STDOUT.println(c);
#endif  
#if 0
        if (monitor.IsMonitoring(Monitoring::MonitorSerial) &&
            default_output != &SA::stream()) {
          default_output->print("SER: ");
          default_output->println(c, HEX);
        }
#endif  
        if (c == '\n') {
          if (cmd_) ParseLine();
          len_ = 0;
          space_ = 0;
          free(cmd_);
          cmd_ = nullptr;
          continue;
        }
        if (len_ + 1 >= space_) {
          int new_space = space_ * 3 / 2 + 8;
          char* tmp = (char*)realloc(cmd_, new_space);
          if (tmp) {
            space_ = new_space;
            cmd_ = tmp;
          } else {
            STDOUT.println("Line too long.");
            len_ = 0;
            space_ = 0;
            free(cmd_);
            cmd_ = nullptr;
            continue;
          }
        }
        cmd_[len_] = c;
        cmd_[len_ + 1] = 0;
        len_++;
      }
      len_ = 0;
      space_ = 0;
      free(cmd_);
      cmd_ = nullptr;
    }
    STATE_MACHINE_END();
  }

  void ParseLine() {
    if (len_ == 0) return;
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
    } else {
      e = nullptr;
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
  int len_ = 0;
  char* cmd_ = nullptr;
  int space_ = 0;
};

Parser<SerialAdapter> parser;

#ifdef ENABLE_SERIAL
Parser<Serial3Adapter> serial_parser;
#define ENABLE_SERIAL_COMMANDS
#endif

#ifdef USB_CLASS_WEBUSB
Parser<WebUSBSerialAdapter> webusb_parser;
#endif

#ifdef ENABLE_SERIAL_COMMANDS
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
#ifdef BLE_PASSWORD
    if (!strcmp(cmd, "get_ble_config")) {
      PrintQuotedValue("password", BLE_PASSWORD);
#ifndef BLE_NAME
#define BLE_NAME "ProffieOS"
#endif
      PrintQuotedValue("name", BLE_NAME);
#ifdef BLE_SHORTNAME
      PrintQuotedValue("shortname", BLE_SHORTNAME);
#else
      if (sizeof(BLE_NAME) - sizeof("") <= 8) {
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


#if defined(ENABLE_MOTION) || defined(ENABLE_SSD1306)
#include "common/i2cdevice.h"
I2CBus i2cbus;
#endif

#ifdef ENABLE_SSD1306
#include "display/ssd1306.h"
SSD1306 display;
#endif

#ifdef ENABLE_MOTION

#ifndef ORIENTATION
#define ORIENTATION ORIENTATION_NORMAL
#endif

#include "motion/mpu6050.h"
#include "motion/lsm6ds3h.h"
#include "motion/fxos8700.h"
#include "motion/fxas21002.h"

// Define this to record clashes to sd card as CSV files
// #define CLASH_RECORDER

#include "scripts/clash_recorder.h"

#ifdef GYRO_CLASS
// Can also be gyro+accel.
GYRO_CLASS gyroscope;
#endif

#ifdef ACCEL_CLASS
ACCEL_CLASS accelerometer;
#endif

#endif   // ENABLE_MOTION

#include "sound/amplifier.h"
#include "common/sd_card.h"
#include "common/booster.h"

void setup() {
#if VERSION_MAJOR >= 4
#define SAVE_RCC(X) startup_##X = RCC->X
  SAVE_RCC(AHB1ENR);
  SAVE_RCC(AHB2ENR);
  SAVE_RCC(AHB3ENR);
  SAVE_RCC(APB1ENR1);
  SAVE_RCC(APB1ENR2);
  SAVE_RCC(APB2ENR);
#define SAVE_MODER(PORT, X) startup_MODER[X] = ((GPIO_TypeDef *)GPIO##PORT##_BASE)->MODER
  SAVE_MODER(A, 0);
  SAVE_MODER(B, 1);
  SAVE_MODER(C, 2);
  SAVE_MODER(H, 3);
  
  // TODO enable/disable as needed
  pinMode(boosterPin, OUTPUT);
  digitalWrite(boosterPin, HIGH);
#endif

  Serial.begin(9600);
#if VERSION_MAJOR >= 4
  // TODO: Figure out if we need this.
  // Serial.blockOnOverrun(false);
#endif
    
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
    if (sdCardSelectPin >= 0 && sdCardSelectPin < 255) {
      STDOUT.println("No sdcard found.");
      pinMode(sdCardSelectPin, OUTPUT);
      digitalWrite(sdCardSelectPin, 0);
      delayMicroseconds(2);
      pinMode(sdCardSelectPin, INPUT);
      delayMicroseconds(2);
      if (digitalRead(sdCardSelectPin) != HIGH) {
        STDOUT.println("SD select not pulled high!");
      }
    }
#if VERSION_MAJOR >= 4
    stm32l4_gpio_pin_configure(GPIO_PIN_PA5,   (GPIO_PUPD_PULLUP | GPIO_OSPEED_HIGH | GPIO_MODE_INPUT));
    delayMicroseconds(10);
    if (!stm32l4_gpio_pin_read(GPIO_PIN_PA5)) {
      STDOUT.println("SCK won't go high!");
    }
    stm32l4_gpio_pin_configure(GPIO_PIN_PA5,   (GPIO_PUPD_PULLDOWN | GPIO_OSPEED_HIGH | GPIO_MODE_INPUT));
    delayMicroseconds(10);
    if (stm32l4_gpio_pin_read(GPIO_PIN_PA5)) {
      STDOUT.println("SCK won't go low!");
    }
#endif
  } else {
    STDOUT.println("Sdcard found..");
  }
#endif

  Looper::DoSetup();
  // Time to identify the blade.
  prop.FindBlade();
  SaberBase::DoBoot();
#if defined(ENABLE_SD) && defined(ENABLE_AUDIO)
  if (!sd_card_found) {
    talkie.Say(talkie_sd_card_15, 15);
    talkie.Say(talkie_not_found_15, 15);
  }
#endif // ENABLE_AUDIO && ENABLE_SD
}

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

  bool on = false;
  SaberBase::DoIsOn(&on);
  if (!on && !Serial && !prop.NeedsPower()
#ifdef ENABLE_AUDIO
       && !amplifier.Active()
#endif
    ) {
    if (millis() - last_activity > 30000) {
#if VERSION_MAJOR >= 4
      // stm32l4_system_sysclk_configure(1000000, 500000, 500000);
      // Delay will enter low-power mode.
      // TODO: Do we need to disable this when serial port is active?
      delay(50);         // ~8 mA
      // STM32.stop(50);  // ~16 mA
      // STM32.standby(50); // not better
      // STM32.shutdown(10000); // this actually resets the cpu after the timeout, but uses a lot less power. (~3.6mA right now)
      // stm32l4_system_sysclk_configure(_SYSTEM_CORE_CLOCK_, _SYSTEM_CORE_CLOCK_/2, _SYSTEM_CORE_CLOCK_/2);
      
#elif defined(ENABLE_SNOOZE)
      snooze_timer.setTimer(500);
      Snooze.sleep(snooze_config);
      Serial.begin(9600);
#endif
    }
  } else {
    last_activity = millis();
  }
}
