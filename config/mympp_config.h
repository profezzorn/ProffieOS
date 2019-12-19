// mygraflex_config1.h - 3.0
//
// This Graflex saber has:
//   o Proffie v1.5 hardware.
//   o Aux and a regular button
//   o An 7-pin TCSS blade connector
//   o All presets are stored in /styles
//   o ssd1306 OLED Screen
//   o Custom ssd1306.h
//   o Bluetooth Module
//   o 28mm BASS speaker
//   o Multiple blade length selection

#ifdef CONFIG_TOP
#include "proffieboard_v1_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 1500
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 3.5
#define ENABLE_AUDIO
#define ENABLE_MOTION 
#define ENABLE_WS2811
#define ENABLE_SD
#define ENABLE_SERIAL
#define SAVE_COLOR_CHANGE
#define IDLE_OFF_TIME 10000
//#define ENABLE_DEVELOPER_COMMANDS
//#define BLADE_ID_CLASS BridgedPullupBladeID<bladeIdentifyPin, BRIDGED_PIN>
//#define SAVE_STATE
// Max 20 characters
///#define BLE_PASSWORD "625656"
// Max 32 characters.
//#define BLE_NAME "Graflex1"
// Max 9 characters
//#define BLE_SHORTNAME "Saber"
//#define ENABLE_SSD1306
#define SHARED_POWER_PINS
//#define BLADE_ID_CLASS BridgedPullupBladeID<bladeIdentifyPin, bladePowerPin3>
//#define BLADE_ID_CLASS ExternalPullupBladeID<bladeIdentifyPin, PULLUP_RESISTANCE>
#define ENABLE_POWER_FOR_ID PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>
#endif

#ifdef CONFIG_PROP
#include "../props/saber_sa22c_buttons.h"
#endif


#ifdef CONFIG_PRESETS


Preset presets[] = {
 
#include "styles/Vader.h"    
#include "styles/GraflexBlades.h"
#include "styles/Grievous.h"
#include "styles/StockBlades.h"    
};

BladeConfig blades[] = {
{ 645, WS2811BladePtr<125, WS2811_ACTUALLY_800kHz | WS2811_GRB, bladePin, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>>(), CONFIGARRAY(presets) },   // Standard VV 34" blade
{ 14, WS2811BladePtr<69, WS2811_ACTUALLY_800kHz | WS2811_GRB, bladePin, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>>(), CONFIGARRAY(presets) }, // My 19" demo blade
{ 594, WS2811BladePtr<1, WS2811_ACTUALLY_800kHz | WS2811_GRB, bladePin, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>>(), CONFIGARRAY(presets) }, // My charge indicator
};
#endif
 
#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif