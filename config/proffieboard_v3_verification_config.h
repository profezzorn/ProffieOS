/* Proffieboard V3 light stapler test config.
 */
#ifdef CONFIG_TOP
#include "proffieboard_v3_config.h"
#define NUM_BLADES 3
#define NUM_BUTTONS 2
#define VOLUME 100
const unsigned int maxLedsPerStrip = 144; 
#define CLASH_THRESHOLD_G 3.5
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define SHARED_POWER_PINS
#define SAVE_STATE
#define BLADE_DETECT_PIN aux2Pin
#define ENABLE_DEVELOPER_COMMANDS
#define IDLE_OFF_TIME 60 * 2 * 1000
#define MOTION_TIMEOUT 60 * 1 * 1000

#define BLASTER_SHOTS_UNTIL_EMPTY 15  // (whatever number)
#define BLASTER_JAM_PERCENTAGE 10     // if not defined, random.

#endif

#ifdef CONFIG_PROP
#include "../props/dual_prop.h"
#include "../props/saber.h"
#undef PROP_TYPE
#include "../props/blaster.h"
#undef PROP_TYPE
#define PROP_TYPE SaberBlasterProp<Saber, Blaster>
#endif


#ifdef CONFIG_PRESETS
Preset saber [] = {
{ "teensysf", "tracks/mars.wav",
  StylePtr<Layers<
  Red,
  InOutTrL<TrWipe<300>,TrWipeInX<WavLen<EFFECT_RETRACTION> >>>>(),
  StylePtr<Red>(),
  StylePtr<Red>(),
  "Teensy"},
};

Preset blaster[] = {
{ "blaster/bank1", "tracks/venus.wav",
  StylePtr<White>(),
  StylePtr<White>(),
  StylePtr<White>(),
  "blaster"},
};

BladeConfig blades[] = {
{ 0,  
  WS281XBladePtr<144, bladePin, Color8::GRB, PowerPINS<bladePowerPin3> >(),
  WS281XBladePtr<144, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin3> >(),
  WS281XBladePtr<4, blade5Pin, Color8::GRB, PowerPINS<bladePowerPin4> >(),
  CONFIGARRAY(saber),
  "SaberSave"}, 
{ NO_BLADE, 
  WS281XBladePtr<144, bladePin, Color8::GRB, PowerPINS<bladePowerPin3> >(),
  WS281XBladePtr<144, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin3> >(),
  WS281XBladePtr<4, blade5Pin, Color8::GRB, PowerPINS<bladePowerPin4> >(),
  CONFIGARRAY(blaster),
  "blasterSave"}
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif

