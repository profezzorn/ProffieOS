//// This is a sample configuration file.
// This saber has:
//   o Teensy 4.0 hardware.
//   o Two buttons
//   o Neopixel blades on LED 1, 2, 3 and neopixel data 1.
//   o An XP-E2 RGB LED star on LED 4-5-6
// If you have a saber similar to this one, make a copy and use the copy.
// This is also the default configuration file. Pre-programmed boards will
// use this configuration file.

#ifdef CONFIG_TOP
#include "aat_teensy40_hardware_config.h"
#define NUM_BLADES 4
#define NUM_BUTTONS 1
#define VOLUME 50
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define ENABLE_SERIAL
#define ENABLE_DEVELOPER_COMMANDS
// #define DISABLE_DIAGNOSTIC_COMMANDS
#define SAVE_COLOR_CHANGE
// #define DISABLE_COLOR_CHANGE
#define SAVE_STATE
// #define KEEP_SAVEFILES_WHEN_PROGRAMMING
// Must be 20 characters or less.
// #define BLE_PASSWORD "password"
// FASTLED is experimental and untested right now
// #define ENABLE_FASTLED
// #define ENABLE_WATCHDOG
// #define ENABLE_SERIALFLASH
// #define ENABLE_SSD1306

// #define ENABLE_DEBUG

   #define IDLE_OFF_TIME 100000  // if defined, pulsing glow will not work,it stops after 100 seconds
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "kanan", "tracks/main.wav",
    StyleNormalPtr<Pulsing<Blue, Red, 1500>, White, 500, 1800>(),
    StyleNormalPtr<Pulsing<Red, Green, 1500>, White, 500, 1800>(),
    StyleNormalPtr<Pulsing<Green, Blue, 1500>, White, 500, 1800>(),
    StyleNormalPtr<Green, White, 500, 1800>(),
	"blue"},
  { "SmthJedi", "tracks/rebels.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<Red, White, 1555>, White>, 1300, 800> >(),
    StylePtr<InOutHelper<EasyBlade<OnSpark<Green, White, 1555>, White>, 1300, 800> >(),
    StylePtr<InOutHelper<EasyBlade<OnSpark<Blue, White, 1555>, White>, 1300, 800> >(),
    StylePtr<InOutHelper<EasyBlade<OnSpark<Blue, White, 1555>, White>, 1300, 800> >(),
	"green"},
  { "TeensySF", "tracks/impmarch.wav",
    StyleNormalPtr<Red, BLUE, 300, 800>(),
    StyleNormalPtr<Red, BLUE, 300, 800>(),
    StyleNormalPtr<Red, BLUE, 300, 800>(),
    StyleNormalPtr<Red, BLUE, 300, 800>(),
	"red"},
  { "Fire", "tracks/dueloffates.wav",
    StyleFirePtr<RED, YELLOW, 0>(),
    StyleFirePtr<RED, YELLOW, 0>(),
    StyleFirePtr<RED, YELLOW, 0>(),
    StyleFirePtr<RED, YELLOW, 0>(),
	"fire"},
  { "ArtnTech", "tracks/muse.wav",
    StyleFirePtr<BLUE, CYAN, 0>(),
    StyleFirePtr<BLUE, CYAN, 0>(),
    StyleFirePtr<BLUE, CYAN, 0>(),
    StyleFirePtr<BLUE, CYAN, 0>(),
	"blue fire"},
  { "Electric", "tracks/cantina.wav",
	StylePtr<InOutHelper<Lockup<StyleFire<Blast<LocalizedClash<Gradient<Red,OrangeRed,Orange,GreenYellow,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,Pulsing<StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,3500>>,150,800,Black>>(),
	StylePtr<InOutHelper<Lockup<StyleFire<Blast<LocalizedClash<Gradient<Red,OrangeRed,Orange,GreenYellow,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,Pulsing<StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,3500>>,150,800,Black>>(),
	StylePtr<InOutHelper<Lockup<StyleFire<Blast<LocalizedClash<Gradient<Red,OrangeRed,Orange,GreenYellow,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,Pulsing<StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,3500>>,150,800,Black>>(),
	StylePtr<InOutHelper<Lockup<StyleFire<Blast<LocalizedClash<Gradient<Red,OrangeRed,Orange,GreenYellow,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,Pulsing<StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,Aqua,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,StyleFire<Blast<LocalizedClash<Gradient<Red,Orange,Strobe<White,RandomPerLEDFlicker<Blue,White>,50,1>,Green,DodgerBlue,Blue,Rgb<20,0,255>>,White,200,80>,White,300,200,400>,Red,10,2,FireConfig<100,2000,5>,FireConfig<2000,500,50>,FireConfig<0,5000,10>,FireConfig<100,2000,5>>,3500>>,150,800,Black>>(),
	"rainbow new style"}

};

// 3x Neopixel + 1x RGB inhilt
BladeConfig blades[] = {
 { 0, DimBlade(40,WS281XBladePtr<50, bladePin, Color8::BRG, PowerPINS<> >()),
      DimBlade(40,WS281XBladePtr<49, blade2Pin, Color8::BRG, PowerPINS<> >()),
      DimBlade(40,WS281XBladePtr<48, blade3Pin, Color8::BRG, PowerPINS<> >()),
	   SimpleBladePtr<CreeXPE2RedTemplate<1500>,
                    CreeXPE2GreenTemplate<470>,
                    CreeXPE2BlueTemplate<680>, NoLED, bladePowerPin1, bladePowerPin2, bladePowerPin3, -1>(),
    CONFIGARRAY(presets) },
};

#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
//Button Aux2Button(BUTTON_AUX2, aux2Pin, "aux2");
//Button UpButton(BUTTON_UP, bladePowerPin4, "aux3");
#endif