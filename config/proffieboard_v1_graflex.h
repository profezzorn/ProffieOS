// This is a sample configuration file.
// This saber has:
//   o Proffieboard V1 hardware.
//   o A touchbutton and a regular button
//   o An 8-pin blade connector
//   o All blades and presets are stored in common_config.h
// If you have a saber similar to this one, make a copy and use the copy.

#ifdef CONFIG_TOP

// Proffieboard V1 electronics
#include "proffieboard_v1_config.h"

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
#define CLASH_THRESHOLD_G 2.0

// Define this if your power button is a touch button.
#define POWER_TOUCHBUTTON_SENSITIVITY 1700
// #define AUX_TOUCHBUTTON_SENSITIVITY 1700
// #define AUX2_TOUCHBUTTON_SENSITIVITY 1700

#define BLADE_ID_CLASS EnablePowerBladeID<PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>, BridgedPullupBladeID<bladeIdentifyPin, blade2Pin>>

// Feature defines, these let you turn off large blocks of code
// used for debugging.
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD

#define SPEAK_TOUCH_VALUES

#endif

#include "common_presets.h"

#ifdef CONFIG_BUTTONS

// There are currently three available button classes:
// Button (standard momentary button)
// TouchButton (similar to momentary button, but reacts to touch).
// LatchingButton (on/off button, always controls ignition)

TouchButton PowerButton(BUTTON_POWER, powerButtonPin, 1500, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif

