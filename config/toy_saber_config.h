// This is a sample configuration file.
// This saber has:
//   o TeensySaber V2 hardware.
//   o Zero buttons (just a latching on-off switch that toggles the power directly.)
//   o A six-segment string blade
// If you have a saber similar to this one, make a copy and use the copy.

#ifdef CONFIG_TOP

// V2 electronics
#include "v2_config.h"

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 1

// Number of buttons
#define NUM_BUTTONS 0

// Dual power buttons means that clicking AUX will also turn the saber on.
// If not defined, AUX will go to next preset when off.
// #define DUAL_POWER_BUTTONS

// Volume, useful range is about 0-2000.
#define VOLUME 1800

// If you have two 144 LED/m strips in your blade, connect
// both of them to bladePin and drive them in parallel.
const unsigned int maxLedsPerStrip = 144;

// This defines how sensitive the clash detection is.
#define CLASH_THRESHOLD_G 1.0

// Define this if your power button is a touch button.
// #define POWER_TOUCHBUTTON_SENSITIVITY 1700
// #define AUX_TOUCHBUTTON_SENSITIVITY 1700
// #define AUX2_TOUCHBUTTON_SENSITIVITY 1700

// If your electonics inverts the bladePin for some reason, define this.
// #define INVERT_WS2811

// Feature defines, these let you turn off large blocks of code
// used for debugging.
#define ENABLE_AUDIO
#define ENABLE_MOTION
// #define ENABLE_SNOOZE
#define ENABLE_WS2811

// FASTLED is experimental and untested right now
// #define ENABLE_FASTLED
// #define ENABLE_WATCHDOG
#define ENABLE_SD
// #define ENABLE_SERIALFLASH

#endif

#ifdef CONFIG_PRESETS

// Each preset line consists of:
// { "font directory", "sound track", Style },
Preset presets[] = {
  { "TeensySF", "tracks/title.wav", StyleNormalPtr<BLUE, WHITE, 200, 300>() },
  { "graflex7", "tracks/duel.wav", StyleNormalPtr<BLUE, WHITE, 200, 300>() },
  { "font03", "tracks/cantina.wav", StyleStrobePtr<BLUE, WHITE, 15, 200, 300>() },
};

// Each line of configuration should be:
//     { blade id resistor ohms, blade, CONFIGARRAY(array of presets) },
BladeConfig blades[] = {
  { 0, StringBladePtr<Blue3mmLED>(), CONFIGARRAY(presets) },
};

#endif

