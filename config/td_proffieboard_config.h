// This is a sample configuration file for a Thermal Detonator
// This configuration has
//   o Proffieboard V1 hardware.
//   o Two buttons
//   o Three individual LEDs on 1, 2, 3
//   o A button LED on LED4
// If you have a similar configuration, make a copy and use that.

#ifdef CONFIG_TOP
#include "proffieboard_config.h"
#define NUM_BLADES 2
#define NUM_BUTTONS 2
#define VOLUME 2500
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 2.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define DELAYED_OFF
#endif

#ifdef CONFIG_PROP
#include "../props/detonator.h"
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "tdmod", "tracks/laptinek.wav",
    StylePtr<InOutHelperTD<SimpleClash<ColorSequence<2000,
                         Rgb<255,0,0>,Rgb<255,255,0>,Rgb<255,0,255>,
                         Rgb<0,255,255>,Rgb<0,255,0>,Rgb<0,0,255>,
                         Rgb<255,255,0>,Rgb<0,255,255> >, RandomFlicker<WHITE, BLACK>, 6000, EFFECT_BLAST>, 100, 100, 6000, BLACK>>(),
    StylePtr<InOutHelperTD<Lockup<SimpleClash<RED,RandomFlicker<RED,BLACK>, 6000, EFFECT_BLAST>, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100, 6000>>(),
    ""},
  { "ThermalD", "tracks/cantina.wav",
    StyleRainbowPtr<100, 100>(),
    StylePtr<InOutHelper<Lockup<RED, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100>>(),
    ""},
};
BladeConfig blades[] = {
 { 0,
     // Note, I use bladePowerPin5 instead of bladePowerPin2!
     SimpleBladePtr<CH1LED,CH2LED,CH3LED,NoLED,
                    bladePowerPin1, bladePowerPin5, bladePowerPin3, -1>(),
     SimpleBladePtr<CH1LED,NoLED,NoLED,NoLED,
                    bladePowerPin4, -1, -1, -1>(),
     CONFIGARRAY(presets) },
  };
#endif

#ifdef CONFIG_BUTTONS
InvertedLatchingButton PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX2, aux2Pin, "aux");
#endif
