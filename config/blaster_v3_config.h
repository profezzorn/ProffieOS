#ifdef CONFIG_TOP
#include "v3_config.h"
#define NUM_BLADES 2
#define NUM_BUTTONS 2
#define VOLUME 2000
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_WS2811
#define ENABLE_SD
#define DELAYED_OFF
#define ENABLE_BLASTER_AUTO
#endif

#ifdef CONFIG_PROP
#include "../props/blaster.h"
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  // Default basic blast color with red audio flicker on blast
  { "_blstr1", "tracks/mars.wav",
    StylePtr<BlastFadeout<Black,AudioFlicker<Black,Red>,375>>(), 
    StylePtr<BlastFadeout<Black,AudioFlicker<Black,Red>,375>>() }
};

BladeConfig blades[] = {
 { 10000, WS2811BladePtr<125, WS2811_ACTUALLY_800kHz | WS2811_GRB>(),
   WS2811BladePtr<14, WS2811_ACTUALLY_800kHz | WS2811_GRB, 7, PowerPINS<bladePowerPin6> >(),
   CONFIGARRAY(presets) },
};

#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
