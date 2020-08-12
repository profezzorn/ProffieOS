#ifdef CONFIG_TOP
#include "v3_config.h"
#define NUM_BLADES 2
#define NUM_BUTTONS 2
#define VOLUME 2000
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define ENABLE_BLASTER_AUTO
#define BLASTER_SHOTS_UNTIL_EMPTY 30
#define BLASTER_JAM_PERCENTAGE 3
#endif

#ifdef CONFIG_PROP
#include "../props/blaster.h"
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  // Default basic blast color with red audio flicker on blast
  { "_blstr1", "tracks/mars.wav",
    StylePtr<Lockup<BlastFadeout<BlastFadeout<Black,AudioFlicker<Black,Red>,250,EFFECT_FIRE>,AudioFlicker<Black,Blue>,1500,EFFECT_STUN>,AudioFlicker<Black,Red>>>(),
    StylePtr<Lockup<BlastFadeout<BlastFadeout<Black,AudioFlicker<Black,Red>,250,EFFECT_FIRE>,AudioFlicker<Black,Blue>,1500,EFFECT_STUN>,AudioFlicker<Black,Red>>>() }
};

BladeConfig blades[] = {
 { 10000, WS2811BladePtr<125, WS2811_ACTUALLY_800kHz | WS2811_GRB>(),
   WS2811BladePtr<14, WS2811_ACTUALLY_800kHz | WS2811_GRB, 7, PowerPINS<bladePowerPin6> >(),
   CONFIGARRAY(presets) },
};

#endif

#ifdef CONFIG_BUTTONS
Button FireButton(BUTTON_FIRE, powerButtonPin, "fire");
Button ModeButton(BUTTON_MODE_SELECT, auxPin, "modeselect");
#endif
