
#ifdef CONFIG_TOP

// V2 electronics
#include "v2_config.h"

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 1

// Number of buttons
#define NUM_BUTTONS 1

// Volume, useful range is about 0-2000.
#define VOLUME 1000

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
//  { "graflex5", "tracks/cantina.wav", &style_pov },

  { "charging", "tracks/duel.wav", &style_charging },
};

BladeConfig blades[] = {
  // Testing configuration.
  { 130000, SimpleBladePtr<CreeXPE2Red, CreeXPE2Green, Blue3mmLED, NoLED>(), CONFIGARRAY(testing_presets) }
};

#endif
