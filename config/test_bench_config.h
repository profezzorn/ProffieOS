// This is a configuration file that I use to test V3 boards.
// This configuration has:
//   o TeensySaber V3 hardware.
//   o Three buttons
//   o A very weird 3-channel LED configuration.
// If you have a V3 test rig, you may also want to use this configuration file.

#ifdef CONFIG_TOP

// V3 electronics
#include "v3_config.h"

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 1

// Number of buttons
#define NUM_BUTTONS 3

// Dual power buttons means that clicking AUX will also turn the saber on.
// If not defined, AUX will go to next preset when off.
#define DUAL_POWER_BUTTONS

// Volume, useful range is about 0-2000.
#define VOLUME 100

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
// #define ENABLE_SERIAL
#define ENABLE_DEBUG

// Must be 20 characters or less.
#define BLE_PASSWORD "password"

// FASTLED is experimental and untested right now
// #define ENABLE_FASTLED
// #define ENABLE_WATCHDOG
#define ENABLE_SD
// #define ENABLE_SERIALFLASH
// #define ENABLE_SSD1306

#endif

#ifdef CONFIG_PRESETS

typedef RandomFlicker<Rgb<200,200,200>, Rgb<40,40,40>> OnPulse;
typedef Pulsing<Rgb16<512,512,512>, Rgb16<50,50,50>, 3000> OffPulse;

Preset testing_presets[] = {
#if 1
  { "TeensySF", "tracks/mars.wav",
    StyleRainbowPtr<300, 800>(),
    "cyan"},
#endif  
#if 0
  { "font02", "tracks/cantina.wav", 
    StylePtr<InOutHelper<EASYBLADE(OnPulse, WHITE), 300, 800, OffPulse> >(),
    "ONOFF" },
#endif
  { "font02", "tracks/cantina.wav", StyleRainbowPtr<300, 800>(),
    "=RainboW++\n++BladE++" },
  { "graflex4", "tracks/title.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>(), "cyan" },
  { "graflex4", "tracks/cantina.wav", StyleNormalPtr<BLUE, RED, 300, 800>(), "blue" },
  { "caliban", "tracks/duel.wav", StyleFirePtr<RED, YELLOW>(), "red fire" },
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 300, 800>(), "red" },
  { "graflex5", "tracks/title.wav", StyleFirePtr<BLUE, CYAN>(), "blue fire" },
  { "igniter/font4", "tracks/duel.wav", StyleNormalPtr<GREEN, WHITE, 300, 800>(), "green" },
  { "graflex4", "tracks/duel.wav", StyleNormalPtr<WHITE, RED, 300, 800>(), "white" },
  { "graflex4", "tracks/walls.wav", StyleNormalPtr<YELLOW, BLUE, 300, 800>(), "yellow" },
  { "graflex4", "tracks/title.wav", StyleNormalPtr<MAGENTA, WHITE, 300, 800>(), "magenta" },
  { "graflex5", "tracks/cantina.wav", StyleStrobePtr<WHITE, RED, 15, 300, 800>(), "white" },
//  { "graflex5", "tracks/cantina.wav", &style_pov },

  { "charging", "tracks/duel.wav", &style_charging, "charging" },
};

BladeConfig blades[] = {
  // Testing configuration.
//  { 130000, SimpleBladePtr<CreeXPE2Red, CreeXPE2Green, Blue3mmLED, NoLED>(), CONFIGARRAY(testing_presets) }
  { 130000, WS2811BladePtr<97, WS2811_800kHz, bladePin, PowerPINS<bladePowerPin1>>(), CONFIGARRAY(testing_presets),  }
//  { 130000, WS2811BladePtr<97, WS2811_400kHz, 8, PowerPINS<bladePowerPin1>, WS2811SerialPin>(), CONFIGARRAY(testing_presets),  }
//  { 130000, SubBlade(0, 9, WS2811BladePtr<10, WS2811_800kHz | WS2811_GRB , bladePin, PowerPINS<bladePowerPin1>>()), CONFIGARRAY(testing_presets),  }

};

#endif

#ifdef CONFIG_BUTTONS
// There are currently three available button classes:
// Button (standard momentary button)
// TouchButton (similar to momentary button, but reacts to touch).
// LatchingButton (on/off button, always controls ignition)

//LatchingButton PowerButton(BUTTON_POWER, powerButtonPin, "pow");
//Button AuxButton(BUTTON_AUX, auxPin, "aux");
//Button Aux2Button(BUTTON_AUX2, aux2Pin, "aux2");
#endif
