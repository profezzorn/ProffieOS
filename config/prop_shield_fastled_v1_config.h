// This is a sample configuration file.
// This saber has:
//   o TeensySaber V1 hardware.
//   o Two buttons
//   o A dotstar pixel blade. (beware: this is experimental and untested!!)
// If you have a saber similar to this one, make a copy and use the copy.

#ifdef CONFIG_TOP

// V1 electronics
#include "v1_config.h"

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 1

// Number of buttons
#define NUM_BUTTONS 2

// Dual power buttons means that clicking AUX will also turn the saber on.
// If not defined, AUX will go to next preset when off.
#define DUAL_POWER_BUTTONS

// Volume, useful range is about 0-2000.
#define VOLUME 1800

// If you have two 144 LED/m strips in your blade, connect
// both of them to bladePin and drive them in parallel.
const unsigned int maxLedsPerStrip = 144;

// This defines how sensitive the clash detection is.
#define CLASH_THRESHOLD_G 1.0

// For V1 electronics, there is an external pullup resistor to measure
// battery voltage. This specifies how many Ohms it is.
#define BATTERY_PULLUP_OHMS 23000

// If your electonics inverts the bladePin for some reason, define this.
// #define INVERT_WS2811

// Feature defines, these let you turn off large blocks of code
// used for debugging.
#define ENABLE_AUDIO
#define ENABLE_MOTION
// #define ENABLE_SNOOZE
// #define ENABLE_WS2811

// FASTLED is experimental and untested right now
#define ENABLE_FASTLED
// #define ENABLE_WATCHDOG
// #define ENABLE_SD
#define ENABLE_SERIALFLASH
#define ENABLE_SSD1306

#endif

#ifdef CONFIG_PRESETS

Preset presets[] = {
  { "font03", "tracks/title.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>(), "cyan" },
  { "graflex7", "tracks/cantina.wav",
    StylePtr<InOutSparkTip<EasyBlade<BLUE, WHITE>, 300, 800> >(), "blue" },
  { "caliban", "tracks/duel.wav", StyleFirePtr<RED, YELLOW>(), "fire" },
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 300, 800>(), "red" },
  { "font02", "tracks/title.wav", StyleFirePtr<BLUE, CYAN>(), "blue fire" },
  { "igniter/font4", "tracks/duel.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<GREEN>, WHITE>, 300, 800> >(), "green" },
  { "font01", "tracks/duel.wav",
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(), "white" },
  { "font01", "tracks/walls.wav",
    StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(), "yellow\nflicker" },
  { "font01", "tracks/title.wav", 
    StylePtr<InOutSparkTip<EasyBlade<MAGENTA, WHITE>, 300, 800> >(), "magenta" },
  { "font02", "tracks/cantina.wav", StyleNormalPtr<
    Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>(), "gradient" },
  { "font02", "tracks/duel.wav",
    StyleNormalPtr<Pulsing<RED, Rgb<50,0,0>, 5000>, WHITE, 300, 800, RED>(), "pulsing" },
  { "font02", "tracks/cantina.wav", StyleRainbowPtr<300, 800>(), "rainbow" },
  { "font02", "tracks/cantina.wav", StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(), "strobe" },
#ifdef ENABLE_WS2811
  { "font02", "tracks/cantina.wav", &style_pov, "POV" },
#endif

  { "charging", "tracks/duel.wav", &style_charging, "battery" },
};

BladeConfig blades[] = {
  { 0, FASTLEDBladePtr<WS2801, RGB, DATA_RATE_MHZ(2), 144>(), CONFIGARRAY(presets) }
};

#endif

#ifdef CONFIG_BUTTONS

// There are currently three available button classes:
// Button (standard momentary button)
// TouchButton (similar to momentary button, but reacts to touch).
// LatchingButton (on/off button, always controls ignition)

TouchButton PowerButton(BUTTON_POWER, powerButtonPin, 1700, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif

