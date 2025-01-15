#ifdef CONFIG_TOP
#include "proffieboard_v3_config.h"
#define NUM_BLADES 2
#define NUM_BUTTONS 2
#define VOLUME 400
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define SHARED_POWER_PINS
#define ENABLE_SERIAL
#define IDLE_OFF_TIME 60 * 10 * 1000
#define SAVE_STATE
#define ENABLE_ALL_EDIT_OPTIONS
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "Jk1Katarn", "",
    StylePtr<InOutHelper<EASYBLADE(Green, WHITE),300,800>>(),
    StylePtr<Blue>(), "JK1\nKatarn" },
  { "Jk1KatarnSmth", "",
    StylePtr<InOutHelper<OnSpark<EASYBLADE(Green, WHITE),White,500>,300,400>>(),
    StylePtr<Blue>(), "JK1\nKatarn\nSmooth" },
  { "Jk2Katarn", "",
    StyleNormalPtr<AudioFlicker<BLUE,Rgb<0,0,220>>, WHITE, 300, 1000>(),
    StylePtr<Blue>(), "JK2\nKatarn" },
  { "TeensySF", "tracks/venus.wav",
    StyleNormalPtr<CYAN, WHITE, 300, 800>(),
    StylePtr<Blue>(), "cyan" },
  { "SmthJedi", "tracks/mars.wav",
    StylePtr<InOutSparkTip<EASYBLADE(BLUE, WHITE), 300, 800> >(),
    StylePtr<Blue>(), "blue" },
  { "SmthGrey", "tracks/mercury.wav",
    StyleFirePtr<RED, YELLOW, 0>(),
    StylePtr<Blue>(), "fire" },
  { "SmthFuzz", "tracks/uranus.wav",
    StyleNormalPtr<RED, WHITE, 300, 800>(),
    StylePtr<Blue>(), "red" },
  { "RgueCmdr", "tracks/venus.wav",
    StyleFirePtr<DodgerBlue, CYAN, 0>(),
    StylePtr<Blue>(), "blue fire" },
  { "TthCrstl", "tracks/mars.wav",
    StylePtr<InOutHelper<OnSpark<Green,White,850>,0,800>>(),
    StylePtr<Blue>(), "green" },
  { "BatteryLevel", "",
    &style_charging,
    StylePtr<Blue>(), "Battery\nLevel" },
  { "BatteryLevel", "",
    StylePtr<InOutHelperX<Gradient<Red,Orange,Yellow,Green,Green,Green,Green>,BatteryLevel>>(),
    StylePtr<Blue>(), "Battery\nLevel Test" }
};
BladeConfig blades[] = {
  { 0,
    WS281XBladePtr<133, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
    SimpleBladePtr<CH3LED, NoLED, NoLED, NoLED, bladePowerPin4, -1, -1, -1>(), // Bluetooth
    CONFIGARRAY(presets) },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
