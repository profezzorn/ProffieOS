// Default V3 config file, set up for
// a single font, a single track and an XP-E2 RGB star.

#ifdef CONFIG_TOP
#include "v3_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 1000
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "TeensySF", "tracks/mars.wav",
    StyleNormalPtr<CYAN, WHITE, 300, 800>(), "cyan"},
  { "TeensySF", "tracks/mars.wav",
    StylePtr<InOutSparkTip<EASYBLADE(BLUE, WHITE), 300, 800> >(), "blue"},
  { "TeensySF", "tracks/mars.wav",
    StyleNormalPtr<RED, WHITE, 300, 800>(), "red"},
  { "TeensySF", "tracks/mars.wav",
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >(), "green"},
  { "TeensySF", "tracks/mars.wav",
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(), "white"},
  { "TeensySF", "tracks/mars.wav",
    StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(), "yellow"},
  { "TeensySF", "tracks/mars.wav",
    StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >(), "magenta"},
  { "TeensySF", "tracks/mars.wav",
    StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(), "strobe"}
};
BladeConfig blades[] = {
  { 0, SimpleBladePtr<CreeXPE2Red, CreeXPE2Blue, CreeXPE2Green, NoLED>(), CONFIGARRAY(presets) },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
