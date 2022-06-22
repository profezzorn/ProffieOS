// Proffieboard V2 board testing configuration.
// Read ../scripts/proffieboard_tst_script.h for instructions.

#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 0
#define VOLUME 300
const unsigned int maxLedsPerStrip = 144; 
#define CLASH_THRESHOLD_G 3.5
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define BLADE_ID_CLASS ExternalPullupBladeID<bladeIdentifyPin, 68000>
#define ENABLE_DEVELOPER_COMMANDS
#endif

#ifdef CONFIG_PRESETS
Preset saber[] = {
  { "TeensySF", "", StylePtr<Black>(), "testrig"},
};  

BladeConfig blades[] = {
  { 0,  SimpleBladePtr<NoLED, NoLED, NoLED, NoLED, -1, -1, -1, -1>(), CONFIGARRAY(saber) },
};

#endif // CONFIG_PRESETS

#ifdef CONFIG_BUTTONS

#include "../scripts/proffieboard_test_script.h"
#warning !!! PROFFIEBOARD TEST SCRIPT ACTIVE !!!
V4TestScript script;
Blinker1 blinker1;
Blinker2 blinker2;
CapTest captest;

#endif  // CONFIG_BUTTONS
