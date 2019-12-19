// myhero_config.h - 3.0
//
// This Hero saber has:
//   o Proffie v2.2 hardware.
//   o Aux and a regular button
//   o Uses alternate SA22c buttons
//   o An 7-pin TCSS blade connector
//   o All presets are stored in /styles
//   o 28mm WOW speaker
//   o Multiple blade length selection
//   o 2 Neopixel Crystal Chamber
//   o Seperate 2x White Standard LEDs  in control box w/ colored windows

#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 3
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
#define SHARED_POWER_PINS
#define ENABLE_POWER_FOR_ID PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>
#endif

#ifdef CONFIG_PROP
#include "../props/saber_sa22c_buttons.h"
#endif

#ifdef CONFIG_PRESETS

Preset presets[] = {
#include "styles/HeroBlades.h"
#include "styles/Vader.h"    
#include "styles/StockBlades.h"    
};

BladeConfig blades[] = {
 { 645, // Blade ID used
  // 32" Neopixel
   WS281XBladePtr<125, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
  // 2 neopixel chamber
    WS281XBladePtr<2, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4> >(),
  // Control box green LED
    SimpleBladePtr<CreeXPE2WhiteTemplate<550>, NoLED, NoLED, NoLED, bladePowerPin5, -1, -1, -1>(),
  // Control box red LED
    SimpleBladePtr<CreeXPE2WhiteTemplate<550>, NoLED, NoLED, NoLED, bladePowerPin6, -1, -1, -1>(),
  
    CONFIGARRAY(presets) },
};
#endif
 
#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif