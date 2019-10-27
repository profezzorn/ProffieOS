// This is a sample configuration file.
// This saber has:
//   o TeensySaber V2 hardware.
//   o A single button
//   o An 8-pin blade connector
//   o All blades and presets are stored in common_config.h
// If you have a saber similar to this one, make a copy and use the copy.

#ifdef CONFIG_TOP

// V2 electronics
#include "v2_config.h"

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 1

// Number of buttons
#define NUM_BUTTONS 1

// Dual power buttons means that clicking AUX will also turn the saber on.
// If not defined, AUX will go to next preset when off.
// #define DUAL_POWER_BUTTONS

// Volume, useful range is about 0-2000.
#define VOLUME 1800

// If you have two 144 LED/m strips in your blade, connect
// both of them to bladePin and drive them in parallel.
const unsigned int maxLedsPerStrip = 144;

// This defines how sensitive the clash detection is.
#define CLASH_THRESHOLD_G 1.5

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

#define ENABLE_POWER_FOR_ID PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>

#endif

#include "common_presets.h"

#ifdef CONFIG_BUTTONS
// There are currently three available button classes:
// Button (standard momentary button)
// TouchButton (similar to momentary button, but reacts to touch).
// LatchingButton (on/off button, always controls ignition)

Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
#endif
