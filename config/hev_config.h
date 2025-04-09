#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 30
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 3.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define DISABLE_NO_REPEAT_RANDOM
#define HEV_POST_DEATH_COOLDOWN_MS 30000
#define HEV_RANDOM_EVENT_INTERVAL_MS 30000
#define HEV_RANDOM_HAZARD_CHANCE 50
#define HEV_HAZARD_DELAY_MS 6000
#define HEV_HAZARD_DECREASE_MS 1000
#define HEV_HEALTH_INCREASE_MS 100
#define HEV_ARMOR_INCREASE_MS 100
#endif

#ifdef CONFIG_PROP
#include "../props/hev.h"
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "_hev;common_hev", "common_hev/tracks/hl1_ost/10 Valve Theme [Extended].wav",
    StyleNormalPtr<Orange, Blue, 800, 300>(),
  }
};

BladeConfig blades[] = {
 { 10000, WS2811BladePtr<125, WS2811_ACTUALLY_800kHz | WS2811_GRB>(),
   CONFIGARRAY(presets) },
};

#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
