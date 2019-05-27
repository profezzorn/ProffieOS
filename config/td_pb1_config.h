// This is a sample configuration file for a Thermal Detonator
// This configuration has
//   o Proffieboard V1 hardware.
//   o Two buttons
//   o Three individual LEDs on 1, 2, 3
//   o A button LED on LED4
// If you have a similar configuration, make a copy and use that.

#ifdef CONFIG_TOP
#include "proffieboard_v1_config.h"
#define NUM_BLADES 2
#define NUM_BUTTONS 2
#define VOLUME 2000
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 2.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#endif

#ifdef CONFIG_PROP
#include "../props/detonator.h"
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "ThermalD", "tracks/cantina.wav",
    StyleNormalPtr<ColorSequence<2000,
                                 Rgb<255,0,0>,Rgb<255,255,0>,Rgb<255,0,255>,
                                 Rgb<0,255,255>,Rgb<0,255,0>,Rgb<0,0,255>,
                                 Rgb<255,255,0>,Rgb<0,255,255> >, WHITE, 200, 200>(),
    StyleNormalPtr<RED, RED, 300, 800>(),
    ""},
};
BladeConfig blades[] = {
 { 0,
     SimpleBladePtr<CH1LED,CH2LED,CH3LED,NoLED,
                    bladePowerPin1, bladePowerPin2, bladePowerPin3, -1>(),
     SimpleBladePtr<CH1LED,NoLED,NoLED,NoLED,
                    bladePowerPin4, -1, -1 -1>(),
     CONFIGARRAY(presets) },
  };
#endif

#ifdef CONFIG_BUTTONS
InvertedLatchingButton PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX2, aux2Pin, "aux");
#endif
