#ifdef CONFIG_TOP
#include "teensy_audio_board_rev_d.h"
#define NUM_BLADES 0
#define NUM_BUTTONS 8
#define VOLUME 2500
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 2.0
#define ENABLE_AUDIO
#define ENABLE_SD
#define ENABLE_DEVELOPER_COMMANDS
#endif

#ifdef CONFIG_PROP
#include "../props/audiofx.h"
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "vader", "tracks/cantina.wav", ""},
  { "TeensySF", "tracks/title.wav", ""},
};
BladeConfig blades[] = { { 0, CONFIGARRAY(presets) }, };
#endif

#ifdef CONFIG_BUTTONS
Button Bt1(BUTTON_TRIGGER_ONE, trigger1Pin, "b1");
Button Bt2(BUTTON_TRIGGER_TWO, trigger2Pin, "b2");
Button Bt3(BUTTON_TRIGGER_THREE, trigger3Pin, "b3");
Button Bt4(BUTTON_TRIGGER_FOUR, trigger4Pin, "b4");
Button Bt5(BUTTON_TRIGGER_FIVE, trigger5Pin, "b5");
Button Bt6(BUTTON_TRIGGER_SIX, trigger6Pin, "b6");
Button Bt7(BUTTON_TRIGGER_SEVEN, trigger7Pin, "b7");
Button Bt8(BUTTON_TRIGGER_EIGHT, trigger8Pin, "b8");
#endif
