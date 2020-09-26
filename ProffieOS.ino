/*
 ProffieOS: Control software for lightsabers and other props.
 http://fredrik.hubbe.net/lightsaber/teensy_saber.html
 Copyright (c) 2016-2019 Fredrik Hubinette
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

#define CONFIG_FILE "config/default_proffieboard_config.h"
// #define CONFIG_FILE "config/default_v3_config.h"
// #define CONFIG_FILE "config/crossguard_config.h"
// #define CONFIG_FILE "config/graflex_v1_config.h"
// #define CONFIG_FILE "config/prop_shield_fastled_v1_config.h"
// #define CONFIG_FILE "config/owk_v2_config.h"
// #define CONFIG_FILE "config/test_bench_config.h"
// #define CONFIG_FILE "config/toy_saber_config.h"
// #define CONFIG_FILE "config/proffieboard_v1_test_bench_config.h"
// #define CONFIG_FILE "config/td_proffieboard_config.h"
// #define CONFIG_FILE "config/teensy_audio_shield_micom.h"
// #define CONFIG_FILE "config/proffieboard_v2_ob4.h"

#ifdef CONFIG_FILE_TEST
#undef CONFIG_FILE
#define CONFIG_FILE CONFIG_FILE_TEST
#endif

#define CONFIG_TOP
#include CONFIG_FILE
#undef CONFIG_TOP

#ifdef SAVE_STATE
#define SAVE_VOLUME
#define SAVE_PRESET
#define SAVE_COLOR_CHANGE
#endif

// #define ENABLE_DEBUG


//
// OVERVIEW
//
// Here explain some general code concepts to make it easier
// to understand the code below.
//
// Most things start with the ProbBase class. Depending on the
// configuration, this class is extended by the Saber class,
// the Detonator class, or some other class. The extended class
// is instantiated as "prop", and is responsible for handling
// button clicks, clashes, swings and other events. These events
// are then send to all registered SaberBase classes.
///
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
// Once the directory has been scanned, we'll decide how to play
// sounds. In the past, there was one class for handling NEC style
// fonts and another for handling Plecter style fonts. However,
// both of those have now been merged into the HybridFont class
// which can do both. It is also capable of doing some mix and matching,
// so you can use a plecter style hum together with a NEC style
// swing if you so desire. The HybridFont class inherit from
// SaberBase and listen on on/off/clash/etc. events, just like
// BladeBase classes do.
//
// HybridFont tells the audio subsystem
// to trigger and mix sounds as aproperiate. The sound subsystem
// starts with an DMA channel which feeds data to a digital-to-analog
// converter. Once the data buffer is half-gone, and interrupt is
// triggered in the DAC class, which tries to fill it up by
// reading data from a int16_t AudioStream. Generally, that data
// stream is hooked up to the AudioDynamicMixer class. This
// class is responsible for taking multiple audio inputs,
// summing them up and then adjusting the volume to minimize
// clipping.

// TODO LIST:
//   stab detect/effect
//
// Audio work items:
//   select clash from force
//   stab effect
// Blade stuff
//    better clash
// Allow several blades to share power pins.

// If defined, DAC vref will be 3 volts, resulting in louder sound. (teensy only)
#define LOUD

// You can get better SD card performance by
// activating the  USE_TEENSY3_OPTIMIZED_CODE define
// in SD.h in the teensy library, however, my sd card
// did not work with that define.

#include <Arduino.h>

#ifdef TEENSYDUINO
#include <DMAChannel.h>
#include <usb_dev.h>

#ifndef USE_TEENSY4
#include <kinetis.h>
#endif

#include <i2c_t3.h>
#include <SD.h>
#include <SPI.h>

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

const char version[] = "$Id: ce12a06a1e236b5101ec60c950530a9a4719a74d $";
const char install_time[] = __DATE__ " " __TIME__;

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
#include "common/profiling.h"

uint64_t audio_dma_interrupt_cycles = 0;
uint64_t pixel_dma_interrupt_cycles = 0;
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
#include "common/quat.h"
#include "common/ref.h"
#include "common/events.h"
#include "common/saber_base.h"
#include "common/saber_base_passthrough.h"

SaberBase* saberbases = NULL;
SaberBase::LockupType SaberBase::lockup_ = SaberBase::LOCKUP_NONE;
SaberBase::ColorChangeMode SaberBase::color_change_mode_ =
  SaberBase::COLOR_CHANGE_MODE_NONE;
bool SaberBase::on_ = false;
uint32_t SaberBase::last_motion_request_ = 0;
uint32_t SaberBase::current_variation_ = 0;

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
int32_t clamptoi24(int32_t x) {
  return clampi32(x, -8388608, 8388607);
}

#include "common/sin_table.h"

void EnableBooster();
void EnableAmplifier();
bool AmplifierIsActive();
void MountSDCard();
const char* GetSaveDir();

#include "common/lsfs.h"
#include "common/strfun.h"

// Double-zero terminated array of search paths.
// No trailing slashes!
char current_directory[128];
const char* next_current_directory(const char* dir) {
  dir += strlen(dir);
  dir ++;
  if (!*dir) return NULL;
  return dir;
}

#include "sound/sound.h"
#include "common/battery_monitor.h"
#include "common/color.h"
#include "common/range.h"
#include "common/fuse.h"
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
#include "styles/retraction_delay.h"
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
#include "styles/byteorder.h"
#include "styles/rotate_color.h"
#include "styles/colorchange.h"
#include "styles/transition_effect.h"
#include "styles/transition_loop.h"
#include "styles/effect_sequence.h"

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
#include "functions/smoothstep.h"
#include "functions/swing_speed.h"
#include "functions/sound_level.h"
#include "functions/blade_angle.h"
#include "functions/variation.h"
#include "functions/twist_angle.h"
#include "functions/layer_functions.h"
#include "functions/islessthan.h"
#include "functions/circular_section.h"
#include "functions/marble.h"
#include "functions/slice.h"
#include "functions/mult.h"

// transitions
#include "transitions/fade.h"
#include "transitions/join.h"
#include "transitions/concat.h"
#include "transitions/instant.h"
#include "transitions/delay.h"
#include "transitions/wipe.h"
#include "transitions/join.h"
#include "transitions/boing.h"
#include "transitions/random.h"
#include "transitions/colorcycle.h"
#include "transitions/wave.h"

//responsive styles
#include "styles/responsive_styles.h"

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
#if 0
  typedef AudioFlicker<base_color, lockup_flicker_color> AddFlicker;
  typedef Blast<base_color, blast_color> AddBlast;
  typedef Lockup<AddBlast, AddFlicker> AddLockup;
  typedef SimpleClash<AddLockup, clash_color> AddClash;
  return StylePtr<InOutHelper<AddClash, out_millis, in_millis> >();
#else
 typedef Layers<base_color,
                SimpleClashL<clash_color>,
                LockupL<AudioFlickerL<lockup_flicker_color> >,
                BlastL<blast_color> > Blade;
  return StylePtr<InOutHelper<Blade, out_millis, in_millis> >();
#endif  
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
#include "blades/dim_blade.h"
#include "blades/leds.h"
#include "blades/blade_id.h"
#include "common/preset.h"
#include "common/blade_config.h"
#include "common/current_preset.h"
#include "styles/style_parser.h"
#include "styles/length_finder.h"

BladeConfig* current_config = nullptr;
class BladeBase* GetPrimaryBlade() {
#if NUM_BLADES == 0
  return nullptr;
#else  
  return current_config->blade1;
#endif  
}
const char* GetSaveDir() {
  if (!current_config) return "";
  if (!current_config->save_dir) return "";
  return current_config->save_dir;
}

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

#if 0
#include "scripts/v3_test_script.h"
#warning !!! V3 TEST SCRIPT ACTIVE !!!
V3TestScript script;
#endif

#if 0
#include "scripts/proffieboard_test_script.h"
#warning !!! PROFFIEBOARD TEST SCRIPT ACTIVE !!!
V4TestScript script;
Blinker1 blinker1;
Blinker2 blinker2;
CapTest captest;
#endif

#include "buttons/floating_button.h"
#include "buttons/latching_button.h"
#include "buttons/button.h"
#ifdef TEENSYDUINO
#include "buttons/touchbutton.h"
#else
#include "buttons/stm32l4_touchbutton.h"
#endif

#include "ir/ir.h"
#include "ir/receiver.h"
#include "ir/blaster.h"
#include "ir/print.h"
#include "ir/nec.h"
#include "ir/rc6.h"
#include "ir/stm32_ir.h"

#ifndef TEENSYDUINO

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

#ifdef BLADE_DETECT_PIN
LatchingButtonTemplate<FloatingButtonBase<BLADE_DETECT_PIN>>
    BladeDetect(BUTTON_BLADE_DETECT, BLADE_DETECT_PIN, "blade_detect");
#endif

#include "common/sd_test.h"

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
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "help")) {
      CommandParser::DoHelp();
      return true;
    }
#endif    

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

#ifndef DISABLE_DIAGNOSTIC_COMMANDS
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
#endif

#ifndef DISABLE_DIAGNOSTIC_COMMANDS
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
#endif    

    if (!strcmp(cmd, "del") && e) {
      LOCK_SD(true);
      LSFS::Remove(e);
      LOCK_SD(false);
      return true;
    }

#ifdef ENABLE_DEVELOPER_COMMANDS
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
#endif // ENABLE_DEVELOPER_COMMANDS

#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "sdtest")) {
      SDTestHelper sdtester;
      if (e && !strcmp(e, "all")) {
	sdtester.TestDir("");
      } else {
	sdtester.TestFont();
      }
      return true;
    }
#endif

#endif  // ENABLE_SD

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
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "effects")) {
      Effect::ShowAll();
      return true;
    }
#endif    
#if 0
    if (!strcmp(cmd, "df")) {
      STDOUT.print(SerialFlashChip::capacity());
      STDOUT.println(" bytes available.");
      return true;
    }
#endif
#ifdef ENABLE_DEVELOPER_COMMANDS
    if (!strcmp(cmd, "high") && e) {
      pinMode(atoi(e), OUTPUT);
      digitalWrite(atoi(e), HIGH);
      STDOUT.println("Ok.");
      return true;
    }
#endif // ENABLE_DEVELOPER_COMMANDS
#ifdef ENABLE_DEVELOPER_COMMANDS
    if (!strcmp(cmd, "low") && e) {
      pinMode(atoi(e), OUTPUT);
      digitalWrite(atoi(e), LOW);
      STDOUT.println("Ok.");
      return true;
    }
#endif // ENABLE_DEVELOPER_COMMANDS

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
#ifdef ENABLE_DEVELOPER_COMMANDS
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
#endif // ENABLE_DEVELOPER_COMMANDS
#endif
#ifdef ENABLE_DEVELOPER_COMMANDS
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
#endif // ENABLE_DEVELOPER_COMMANDS
#ifdef ENABLE_DEVELOPER_COMMANDS
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
#endif // ENABLE_DEVELOPER_COMMANDS

#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "malloc")) {
      STDOUT.print("alloced: ");
      STDOUT.println(mallinfo().uordblks);
      STDOUT.print("Free: ");
      STDOUT.println(mallinfo().fordblks);
      return true;
    }
#endif    
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

#ifndef DISABLE_DIAGNOSTIC_COMMANDS
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
	        pixel_dma_interrupt_cycles +
                 wav_interrupt_cycles +
		 Looper::CountCycles() +
		 CountProfileCycles());
      STDOUT.print("Audio DMA: ");
      STDOUT.print(audio_dma_interrupt_cycles * 100.0f / total_cycles);
      STDOUT.println("%");
      STDOUT.print("Wav reading: ");
      STDOUT.print(wav_interrupt_cycles * 100.0f / total_cycles);
      STDOUT.println("%");
      STDOUT.print("Pixel DMA: ");
      STDOUT.print(pixel_dma_interrupt_cycles * 100.0f / total_cycles);
      STDOUT.println("%");
      STDOUT.print("LOOP: ");
      STDOUT.print(loop_cycles * 100.0f / total_cycles);
      STDOUT.println("%");
      STDOUT.print("Global loops / second: ");
      global_loop_counter.Print();
      STDOUT.println("");
      SaberBase::DoTop(total_cycles);
      Looper::LoopTop(total_cycles);
      DumpProfileLocations(total_cycles);
      noInterrupts();
      audio_dma_interrupt_cycles = 0;
      pixel_dma_interrupt_cycles = 0;
      wav_interrupt_cycles = 0;
      interrupts();
      return true;
    }
#endif

    if (!strcmp(cmd, "version")) {
      STDOUT.println(version);
      STDOUT.print("Installed: ");
      STDOUT.println(install_time);
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
    if (!strcmp(cmd, "RebootDFU")) {
      stm32l4_system_dfu();
      return true;
    }
#ifdef ENABLE_DEVELOPER_COMMANDS
    if (!strcmp(cmd, "stm32info")) {
      STDOUT.print("VBAT: ");
      STDOUT.println(STM32.getVBAT());
      STDOUT.print("VREF: ");
      STDOUT.println(STM32.getVREF());
      STDOUT.print("TEMP: ");
      STDOUT.println(STM32.getTemperature());
      return true;
    }
#endif // ENABLE_DEVELOPER_COMMANDS
#ifdef ENABLE_DEVELOPER_COMMANDS
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
        STDOUT.print("  ");
        for (int j = 15; j >= 0; j--) {
          uint32_t now = (GPIO->PUPDR >> (j * 2)) & 3;
          STDOUT.print("-ud?"[now]);
          if (!(j & 3)) STDOUT.print(" ");
        }
        STDOUT.print("  ");
        for (int j = 15; j >= 0; j--) {
          uint32_t now = ((GPIO->IDR >> j) & 1) | (((GPIO->ODR >> j) & 1) << 1);
          STDOUT.print("lhLH"[now]);
          if (!(j & 3)) STDOUT.print(" ");
        }
        STDOUT.println("");
      }
      return true;
    }
#endif // ENABLE_DEVELOPER_COMMANDS
#ifdef ENABLE_DEVELOPER_COMMANDS
    if (!strcmp(cmd, "CLK")) {
      if (e) {
        uint32_t c = atoi(e) * 1000000;
        stm32l4_system_sysclk_configure(c, c/2, c/2);
      }
      STDOUT.print("Clocks: hse=");
      STDOUT.print(stm32l4_system_hseclk());
      STDOUT.print(" lse=");
      STDOUT.print(stm32l4_system_lseclk());
      STDOUT.print(" sys=");
      STDOUT.print(stm32l4_system_sysclk());
      STDOUT.print(" f=");
      STDOUT.print(stm32l4_system_fclk());
      STDOUT.print(" h=");
      STDOUT.print(stm32l4_system_hclk());
      STDOUT.print(" p1=");
      STDOUT.print(stm32l4_system_pclk1());
      STDOUT.print(" p2=");
      STDOUT.print(stm32l4_system_pclk2());
      STDOUT.print(" sai=");
      STDOUT.println(stm32l4_system_saiclk());
      return true;
    }
#endif // ENABLE_DEVELOPER_COMMANDS
#ifdef ENABLE_DEVELOPER_COMMANDS
    if (!strcmp(cmd, "whatispowered")) {
      STDOUT.print("ON: ");
#define PRINTIFON(REG, BIT) do {                                        \
        if (RCC->REG & RCC_##REG##_##BIT##EN) {                         \
          STDOUT.print(" " #BIT);                                       \
          if (!(startup_##REG & RCC_##REG##_##BIT##EN)) STDOUT.print("+"); \
        }                                                               \
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

      // Not sure what CPUs implement these
      PRINTIFON(AHB1ENR, CRC);
      PRINTIFON(AHB1ENR, TSC);
      PRINTIFON(AHB2ENR, RNG);
      PRINTIFON(APB1ENR1, LCD);
      PRINTIFON(APB1ENR1, RTCAPB);
      PRINTIFON(APB1ENR1, WWDG);
      PRINTIFON(APB1ENR1, CRS);
      PRINTIFON(APB1ENR1, CAN1);
      PRINTIFON(APB1ENR1, PWR);
      PRINTIFON(APB1ENR1, OPAMP);
      PRINTIFON(APB1ENR2, SWPMI1);
      PRINTIFON(APB2ENR, SYSCFG);
      PRINTIFON(APB2ENR, FW);

      STDOUT.println("");
      STDOUT.print("VBUS: ");
      STDOUT.println(stm32l4_gpio_pin_read(GPIO_PIN_PB2));
      STDOUT.print("USBD connected: ");
      STDOUT.println(USBD_Connected());
      return true;
    }
#endif // ENABLE_DEVELOPER_COMMANDS

#ifdef ENABLE_DEVELOPER_COMMANDS
#ifdef HAVE_STM32L4_DMA_GET    
    if (!strcmp(cmd, "dmamap")) {
      for (int channel = 0; channel < 16; channel++) {
	stm32l4_dma_t *dma = stm32l4_dma_get(channel);
	if (dma) {
	  STDOUT.print(" DMA");
	  STDOUT.print( 1 +(channel / 8) );
	  STDOUT.print("_CH");
	  STDOUT.print( channel % 8 );
	  STDOUT.print(" = ");
	  STDOUT.println(dma->channel >> 4, HEX);
	}
      }
      return true;
    }
#endif // HAVE_STM32L4_DMA_GET    
#endif // ENABLE_DEVELOPER_COMMANDS

#endif  // TEENSYDUINO

    return false;
  }

  void Help() override {
    STDOUT.println(" version - show software version");
    STDOUT.println(" reset - restart software");
#ifndef DISABLE_DIAGNOSTIC_COMMANDS    
    STDOUT.println(" effects - list current effects");
#endif    
#ifdef ENABLE_SERIALFLASH
    STDOUT.println("Serial Flash memory management:");
    STDOUT.println("   ls, rm <file>, format, play <file>, effects");
    STDOUT.println("To upload files: tar cf - files | uuencode x >/dev/ttyACM0");
#endif
#if defined(ENABLE_SD) && !defined(DISABLE_DIAGNOSTIC_COMMANDS)
    STDOUT.println(" dir [directory] - list files on SD card.");
    STDOUT.println(" sdtest - benchmark SD card");
#endif
  }
};

StaticWrapper<Commands> commands;

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

#ifdef RFID_SERIAL
class RFIDParser : public Looper {
public:
  RFIDParser() : Looper() {}
  const char* name() override { return "Parser"; }
  void Setup() override {
    RFID_SERIAL.begin(9600);
  }

#define RFID_READCHAR() do {						\
  state_machine_.sleep_until_ = millis();				\
  while (!RFID_SERIAL.available()) {					\
    if (millis() - state_machine_.sleep_until_ > 200) goto retry;	\
    YIELD();								\
  }									\
  getc();								\
} while (0)

  int c, x;
  uint64_t code;

  void getc() {
    c = RFID_SERIAL.read();
    if (monitor.IsMonitoring(Monitoring::MonitorSerial)) {
      default_output->print("SER: ");
      default_output->println(c, HEX);
    }
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
    retry:
      RFID_READCHAR();
      if (c != 2) goto retry;
      code = 0;
      for (x = 0; x < 10; x++) {
	RFID_READCHAR();
	code <<= 4;
	if (c >= '0' && c <= '9') {
	  code |= c - '0';
	} else if (c >= 'A' && c <= 'F') {
	  code |= c - ('A' - 10);
	} else {
	  goto retry;
	}
      }
      RFID_READCHAR();
      x = code ^ (code >> 24);
      x ^= (x >> 8) ^ (x >> 16);
      x &= 0xff;
      if (c != x) goto retry;
      RFID_READCHAR();
      if (c == 3) {
	default_output->print("RFID: ");
	for (int i = 36; i >= 0; i-=4) {
	  default_output->print((int)((code >> i) & 0xf), HEX);
	}
	default_output->println("");
	for (size_t i = 0; i < NELEM(RFID_Commands); i++) {
	  if (code == RFID_Commands[i].id) {
	    CommandParser::DoParse(RFID_Commands[i].cmd, RFID_Commands[i].arg);
	  }
	}
      }
    }
    STATE_MACHINE_END();
  }

private:
  StateMachineState state_machine_;
};

StaticWrapper<RFIDParser> rfid_parser;
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
        if (c == '\n' || c == '\r') {
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

StaticWrapper<Parser<SerialAdapter>> parser;

#ifdef ENABLE_SERIAL
StaticWrapper<Parser<Serial3Adapter>> serial_parser;
#define ENABLE_SERIAL_COMMANDS
#endif

#ifdef USB_CLASS_WEBUSB
StaticWrapper<Parser<WebUSBSerialAdapter>> webusb_parser;
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

StaticWrapper<SerialCommands> serial_commands;

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

#include "motion/motion_util.h"
#include "motion/mpu6050.h"
#include "motion/lsm6ds3h.h"
#include "motion/fxos8700.h"
#include "motion/fxas21002.h"

// Define this to record clashes to sd card as CSV files
// #define CLASH_RECORDER

#include "scripts/clash_recorder.h"

#ifdef GYRO_CLASS
// Can also be gyro+accel.
StaticWrapper<GYRO_CLASS> gyroscope;
#endif

#ifdef ACCEL_CLASS
StaticWrapper<ACCEL_CLASS> accelerometer;
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
#ifdef DOSFS_CONFIG_STARTUP_DELAY
#define PROFFIEOS_SD_STARTUP_DELAY DOSFS_CONFIG_STARTUP_DELAY
#else
#define PROFFIEOS_SD_STARTUP_DELAY 1000
#endif

#ifndef CONFIG_STARTUP_DELAY
#define CONFIG_STARTUP_DELAY 0
#endif

#if PROFFIEOS_SD_STARTUP_DELAY > CONFIG_STARTUP_DELAY
#define PROFFIEOS_STARTUP_DELAY PROFFIEOS_SD_STARTUP_DELAY
#else
#define PROFFIEOS_STARTUP_DELAY CONFIG_STARTUP_DELAY
#endif

  while (millis() - now < PROFFIEOS_STARTUP_DELAY) {
#ifndef NO_BATTERY_MONITOR  
    srand((rand() * 917823) ^ LSAnalogRead(batteryLevelPin));
#endif

#ifdef BLADE_DETECT_PIN
    // Figure out if blade is connected or not.
    // Note that if PROFFIEOS_STARTUP_DELAY is smaller than
    // the settle time for BladeDetect, this won't work properly.
    BladeDetect.Warmup();
#endif
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

#include "common/clock_control.h"

void loop() {
#ifdef MTP_RX_ENDPOINT
  mtpd.loop();
#endif
  Looper::DoLoop();
}
