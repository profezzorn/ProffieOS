
#ifdef CONFIG_TOP

// Board version
#define VERSION_MAJOR 2
#define VERSION_MINOR 3

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 3

// Number of buttons
#define NUM_BUTTONS 1

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

Preset presets[] = {
  { "font01", "tracks/title.wav",
    StyleFirePtr<RED, YELLOW, 0>(),
    StyleFirePtr<RED, YELLOW, 1, 800>(), // 800ms delay
    StyleFirePtr<RED, YELLOW, 2, 800>()
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
};

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
