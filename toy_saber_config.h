
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

// This defines how sensetive the clash detection is.
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
  { "font01", "tracks/title.wav", StyleNormalPtr<BLUE, WHITE, 200, 300>() },
  { "graflex7", "tracks/duel.wav", StyleNormalPtr<BLUE, WHITE, 200, 300>() },
  { "font03", "tracks/cantina.wav", StyleStrobePtr<BLUE, WHITE, 15, 200, 300>() },
};

// Each line of configuration should be:
//     { blade id resistor ohms, blade, CONFIGARRAY(array of presets) },
// Where "blade", can be one of the following:
//     WS2811BladePtr<number of leds, WS2811 configuration flags>()
//     SimpleBladePTR<Color of channel 1, color of channel 2, color of channel 3, color of channel 4>()
// All colors can be specied as three numbers or using one the handy macros above.
BladeConfig blades[] = {
  { 0, StringBladePtr<Blue3mmLED>(), presets },
};

#endif

