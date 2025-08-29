#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 50
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 3.0
//If not disabled, Armor Readout play back floats around current Armor value.
#define DISABLE_NO_REPEAT_RANDOM
#endif

#ifdef CONFIG_PROP
#include "../props/hev.h"
#endif

// All HALF-LIFE sfx audio can be found in the game's "sound" directory.
// Specifically, HEV Suit voice lines can be found within the "fvox" dir.
// The music tracks can be found in the "media" dir in the root folder.

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "hev;common_hev", "common_hev/tracks/hl1_ost/10 Valve Theme [Extended].wav",
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
