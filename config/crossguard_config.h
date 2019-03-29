// This is a sample configuration file for a 3-blade saber. (Kylo style)
// This saber has:
//   o TeensySaber V2 hardware.
//   o A single button
//   o Three neopixel blades
// If you have a saber similar to this one, make a copy and use the copy.

#ifdef CONFIG_TOP

// V2 electronics
#include "v2_config.h"

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 3

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

// Fire speed, valid values are 1 - 10
#define FIRE1_SPEED 2
// How long to wait before firing up crossguards.
#define FIRE1_DELAY 800

// Each of these have three values: A, B, C
// A = non-random intensity
// B = random intensity
// C = cooling
// The first two control temperature, and as we add
// A + rnd(B) to the base of the blade each animation frame.
// The second controls how rapidly the fire cools down

// This is used during normal operation.
#define FIRE1_NORMAL 0, 1000, 2

// This is used when a clash occurs
#define FIRE1_CLASH  3000, 0, 0

// This is used during lockup.
#define FIRE1_LOCKUP 0, 5000, 10

// Helper
#define FIRE1PTR(NUM, DELAY) \
  StyleFirePtr<RED, YELLOW, NUM, DELAY, FIRE1_SPEED, \
    FIRE1_NORMAL, FIRE1_CLASH, FIRE1_LOCKUP>()


Preset presets[] = {
  { "font01", "tracks/title.wav",
    FIRE1PTR(0, 0),
    FIRE1PTR(1, FIRE1_DELAY),
    FIRE1PTR(2, FIRE1_DELAY),
  },
  { "font02", "tracks/title.wav",
    StyleNormalPtr<RED, WHITE, 200, 300>(),
    StyleNormalPtr<RED, WHITE, 200, 300>(),
    StyleNormalPtr<RED, WHITE, 200, 300>()
  },
  { "font02", "tracks/title.wav",
    StyleRainbowPtr<300, 800>(),
    StyleRainbowPtr<300, 800>(),
    StyleRainbowPtr<300, 800>()
  },
  { "font03", "tracks/title.wav",
    StyleNormalPtr<CYAN, WHITE, 300, 800>(),
    StyleNormalPtr<CYAN, WHITE, 300, 800>(),
     StyleNormalPtr<CYAN, WHITE, 300, 800>()
  },
  { "graflex7", "tracks/cantina.wav",
    StylePtr<InOutSparkTip<EASYBLADE(BLUE, WHITE), 300, 800> >(),
    StylePtr<InOutSparkTip<EASYBLADE(BLUE, WHITE), 300, 800> >(),
    StylePtr<InOutSparkTip<EASYBLADE(BLUE, WHITE), 300, 800> >()
  },
  { "font02", "tracks/title.wav",
    StyleFirePtr<BLUE, CYAN, 0>(),
    StyleFirePtr<BLUE, CYAN, 1>(),
    StyleFirePtr<BLUE, CYAN, 2>()
  },
  { "igniter/font4", "tracks/duel.wav",
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >(),
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >(),
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >()
  },
  { "font01", "tracks/duel.wav",
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(),
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(),
    StyleNormalPtr<WHITE, RED, 300, 800, RED>()
  },
  { "font01", "tracks/walls.wav",
    StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(),
    StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(),
    StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>()
  },
  { "font01", "tracks/title.wav", 
    StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >(),
    StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >(),
    StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >()
  },
  { "font02", "tracks/cantina.wav",
    StyleNormalPtr<Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>(),
    StyleNormalPtr<Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>(),
    StyleNormalPtr<Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>()
  },
  { "font02", "tracks/cantina.wav",
    StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(),
    StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(),
    StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>()
  },
  { "font02", "tracks/cantina.wav",
    &style_pov,
    StyleNormalPtr<BLACK, BLACK, 100,100>(),
    StyleNormalPtr<BLACK, BLACK, 100,100>(),
  },
  { "charging", "tracks/duel.wav",
    &style_charging,
    StyleNormalPtr<BLACK, BLACK, 100,100>(),
    StyleNormalPtr<BLACK, BLACK, 100,100>(),
  },
};


// Hook up the main blade as normal.
// Hook up one of the crossguards like this:
//  NEOPIXEL  +    <--> positive power (probably from charge port)
//  NEOPIXEL  -    <--> LED4 pad on the bottom of the teensysaber
//  NEOPIXEL data  <--> Pin 7 on the teensy  (through a 100ohm resistor)
// Hook up the other crossguards like this:
//  NEOPIXEL  +    <--> positive power (probably from charge port)
//  NEOPIXEL  -    <--> LED5 pad on the bottom of the teensysaber
//  NEOPIXEL data  <--> Pin 8 on the teensy  (through a 100ohm resistor)

BladeConfig blades[] = {
  { 0, // blade ID resistor not used
    // Main blade, 118 LEDs
    WS2811BladePtr<118, WS2811_800kHz | WS2811_GRB>(),
    // First crossguard, 26 LEDS, power on LED4, data on pin 7
    WS2811BladePtr<26, WS2811_800kHz | WS2811_GRB, 7, PowerPINS<bladePowerPin4> >(),
    // First crossguard, 26 LEDS, power on LED5, data on pin 8
    WS2811BladePtr<26, WS2811_800kHz | WS2811_GRB, 8, PowerPINS<bladePowerPin5> >(),
    CONFIGARRAY(presets)
  },
};

#endif

#ifdef CONFIG_BUTTONS
// There are currently three available button classes:
// Button (standard momentary button)
// TouchButton (similar to momentary button, but reacts to touch).
// LatchingButton (on/off button, always controls ignition)

Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
#endif
