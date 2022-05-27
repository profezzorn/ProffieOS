/*
 * This is the config file for my EP4 Obi-Wan saber.
 * Specs:
 *  Proffieboard V2.2
 *  21700 battery
 *  33mm speaker (in the center)
 *  529 LED 4-way zigzag blade
 *  5 LEDs in pogo pin PCB
 *  7 LEDs under bubble strip
 *  30 LEDs in the pommel
 *  micro-usb in activation box
 *  kill switch in activation box
 *  hidden compartment for allen wrench
 *  charge port under the pommel
 * All of these LEDs means that this config file needs to be compiled
 * with the "Fastest" optimization, or it won't be very smooth.
 */

#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 7
#define NUM_BUTTONS 2
// #define VOLUME 2000
#define VOLUME 100
const unsigned int maxLedsPerStrip = 530;
#define CLASH_THRESHOLD_G 2.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define SHARED_POWER_PINS
// #define ENABLE_TRACING
#define EXTRA_COLOR_BUFFER_SPACE 30
#define IDLE_OFF_TIME 5 * 60 * 1000
#endif

#include "../scripts/clash_recorder_config.h"

#ifdef CONFIG_PRESETS

//using BatteryColor = Mix<BatteryLevel,Red,Red,Orange,Orange,Yellow,Yellow,Blue,Blue,Blue,Green,Green,Green>;
//using BatteryLevelStyle = Mix<Bump<BatteryLevel, Int<6000>>, Black, BatteryColor>;
using BatteryLevelStyle = InOutHelperX<Gradient<Red,Orange,Yellow,Green,Green,Green,Green>, BatteryLevel>;
using VUGradient = Gradient<Red,Orange,Yellow,Green>;
using VUMeterStyle2 = Mix<Bump<ChangeSlowly<SmoothSoundLevel, Int<8000>>, Int<8000>>, Black, VUGradient>;
using VUMeterStyle1 = InOutHelperX<VUGradient, SmoothSoundLevel>;
using VUMeterStyle3 = InOutHelperX<VUGradient, SmoothSoundLevel, VUMeterStyle2>;

template<class C>
using FanShade = StripesX<Int<293>, ChangeSlowly<Ifon<Int<5000>, Int<20>>, Int<5000>>, Black, Black, C>;

#if NUM_BLADES == 7
#define QUADBLADE(...) __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__
#else
#define QUADBLADE(...) __VA_ARGS__
#endif

Preset presets[] = {
/*
 * We start with some Obi-Wan presets
 */
  
   // First preset is for accuracy, no extra lights.
   { "OB4", "tracks/title.wav",
     QUADBLADE( StylePtr<ClashRecorderStyle>() ),
    StyleNormalPtr<DeepSkyBlue, WHITE, 300, 800>(),  // pogo pin PCB

     // Battery level on boot
    StylePtr<Layers<
      Black,
      TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,  BatteryLevelStyle ,TrFade<1000>>, EFFECT_BOOT>
    >>(),
     
    StylePtr<BLACK>(),
     "obiwan kenobi"},

};
BladeConfig blades[] = {
 { 0,
   // zigzag blade, total length = 
   // pixel 1 - 132 133 = straight out
   // pixel 134 - 264
   // pixel 265 - 396 397 = straight out
   // 398 - 529
   // bladePowerPin1 is not actually hooked up, but it makes the test bench happy...
#if NUM_BLADES == 4
   WS281XBladePtr<530, bladePin, Color8::GRB, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>, DefaultPinClass, 1000000>(),
#else   
   SubBlade(0, 132, WS281XBladePtr<529, bladePin, Color8::GRB, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>, DefaultPinClass, 1000000>()),
   SubBlade(264, 396, NULL),
   SubBladeReverse(133, 263, NULL),
   SubBladeReverse(397, 528, NULL),
#endif   
   WS281XBladePtr<5, blade4Pin,  Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
   WS281XBladePtr<7, blade3Pin, Color8::GRB, PowerPINS<bladePowerPin6> >(),
   WS281XBladePtr<30, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin5> >()
 , CONFIGARRAY(presets) },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
