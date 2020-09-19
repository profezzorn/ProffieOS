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
#define VOLUME 2000
// #define VOLUME 100
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
    QUADBLADE( StyleNormalPtr<DeepSkyBlue, WHITE, 300, 800>()),
    StyleNormalPtr<DeepSkyBlue, WHITE, 300, 800>(),  // pogo pin PCB

     // Battery level on boot
    StylePtr<Layers<
      Black,
      TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,  BatteryLevelStyle ,TrFade<1000>>, EFFECT_BOOT>
    >>(),
     
    StylePtr<BLACK>(),
     "obiwan kenobi"},

   { "OB4", "tracks/duelfate.wav",
     QUADBLADE( StyleNormalPtr<DeepSkyBlue, WHITE, 300, 800>()),
     StyleNormalPtr<DeepSkyBlue, WHITE, 300, 800>(),  // pogo pin PCB
     StyleNormalPtr<Black, White, 300, 800>(),
     StyleNormalPtr<Black, White, 300, 800>(),
     "ben kenobi"},

    { "HeroPack/Obi-Wan Kenobi;HeroPack/font", "tracks/walls.wav",
      QUADBLADE( StylePtr<InOutSparkTip<EASYBLADE(DeepSkyBlue, WHITE), 300, 800> >() ),
      StylePtr<InOutSparkTip<EASYBLADE(DeepSkyBlue, WHITE), 300, 800> >(),
      // cylon
      StylePtr<SimpleClash<Lockup<Blast<Cylon<Rgb<0,0,64>,15,10, DeepSkyBlue,15,100,2000>,White>,AudioFlicker<Blue,White>>,White>>(),
      // Fan
      StylePtr<FanShade<Mix<InOutFunc<1000,1000>, Rgb<0,0,30>, EasyBlade<DeepSkyBlue, White>>>>(),
//      StylePtr<InOutSparkTip<EASYBLADE(BLUE, WHITE), 300, 800> >(),
      "obiwan"},

   // Blingy preset
   { "unlight", "tracks/darkdeed.wav",
     QUADBLADE( StylePtr<InOutHelper<EASYBLADE(OnSpark<CYAN>, WHITE), 300, 800> >() ),
    StylePtr<InOutHelper<EASYBLADE(OnSpark<CYAN>, WHITE), 300, 800> >(),

     // FIXME
     //StylePtr<BatteryLevelStyle>(),

     StylePtr<SimpleClash<Lockup<Blast<Cylon<Rgb<0,0,64>,15,10, DeepSkyBlue,15,100,2000,Cylon<Rgb<64,0,0>,15,6, Red,15,57,2000,Cylon<Rgb<0,40,0>,15,7, Green,15,37,2000>>>,White>,AudioFlicker<Blue,White>>,White>>(),

     // Marble style
     StylePtr<Mix<
          CircularSectionF<
          MarbleF<Int<-2000>, Int<40000>, Ifon<Int<827680>, Int<0>>, Int<1276800>>,
          /* circle fraction = */ ChangeSlowly<Ifon<Int<32768>, Int<3276>>, Int<2048>>>,
         Black,
         Mix<InOutFuncX<Int<2000>,Int<2000>>, Rgb<0,0,20>, Rgb<0,255,255>
     >>>(),
    "chaotic good"},

/*
 * Next, we have some character-based presets.
 */
   
   // Need a preset with some per-led random in it...
   { "unstable", "tracks/duelfate.wav",
     QUADBLADE(StyleNormalPtr<
                 StyleFire<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,300>,Rgb<80,0,0>,0,6,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>
                 >,  WHITE, 300, 500>()),
     StyleNormalPtr<
                 StyleFire<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,300>,Rgb<80,0,0>,0,6,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>
                 >,  WHITE, 300, 500>(),
     StyleNormalPtr<
                 StyleFire<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,300>,Rgb<80,0,0>,0,6,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>
                 >,  WHITE, 300, 500>(),
     StyleNormalPtr<
                 StyleFire<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,300>,Rgb<80,0,0>,0,6,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>,
                   FireConfig<10,1000,2>
                 >,  WHITE, 300, 500>(),
   "kylo" },

   { "HeroPack/Ahsoka Tano;HeroPack/font", "tracks/venus.wav",
     QUADBLADE( StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >() ),
     StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >(),

     // Luke-inspired random red/green blink
     StylePtr<RandomBlink<1000,Black,RandomBlink<500,Red,Green>>>(),
     StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>, WHITE), 300, 800> >(), "ashoka"},

   { "HeroPack/Mace Windu;HeroPack/font", "tracks/mars.wav",
     QUADBLADE( StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >() ),
     StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >(),
     StylePtr<VUMeterStyle1>(),
     StylePtr<InOutSparkTip<EASYBLADE(MAGENTA, WHITE), 300, 800> >(), "mace"},


/*
 * Finally, we have silly, funny or just annoying presets....
 */

#if 1   
   // Fire preset
   { "fire", "tracks/mars.wav",
     QUADBLADE( StyleFirePtr<RED, YELLOW, 0,0, 3>() ),
     StyleFirePtr<RED, YELLOW>(),
     StyleFirePtr<RED, YELLOW>(),
     // Pommel looks odd with fire, maybe change it to a random preset instead.
     StyleFirePtr<RED, YELLOW>(), "fire"},
#endif   

#if 1
   // FIXME: FONT
   { "proxima", "tracks/cantina.wav",
     // Four out-of-sync rainbows
#define STRIPE_COLORS Red, Green, Blue
     StyleNormalPtr<Stripes<1000, -1000, STRIPE_COLORS>, White, 300, 800>(),
#if NUM_BLADES == 7     
     StyleNormalPtr<Stripes<1000, -1200, STRIPE_COLORS>, White, 300, 800>(),
     StyleNormalPtr<Stripes<1000, -1050, STRIPE_COLORS>, White, 300, 800>(),
     StyleNormalPtr<Stripes<1000, -900,  STRIPE_COLORS>, White, 300, 800>(),
#endif
     StyleNormalPtr<Stripes<1000, -1000, STRIPE_COLORS>, White, 300, 800>(),

     // Smaller rainbow for bubble strip
     StyleNormalPtr<Stripes<500, -500, STRIPE_COLORS>, White, 300, 800>(),

     // 488 makes the colors line up in the pommel = no seam
     // TO calculate, do 50000 * LEDs / (REPETITIONS * COLORS * 341)
     StyleNormalPtr<Stripes<488, 500, STRIPE_COLORS>, White, 300, 800>(),
     "rainbow"},
#endif   

#if 1
   // Spectrum song
   { "spectrum", "",
     QUADBLADE( StyleNormalPtr<FromHumFileStyle<170,0, 25,1>, WHITE, 300, 800>() ),
     StyleNormalPtr<FromHumFileStyle<170,0, 25,1>, WHITE, 300, 800>(),
     StyleNormalPtr<FromHumFileStyle<170,0, 25,1>, WHITE, 300, 800>(),
     StyleNormalPtr<FromHumFileStyle<170,0, 25,1>, WHITE, 300, 800>(), "spectrum"},
#endif

#if 1
   // FIXME: polyphonic, fire alarm "track"?
   { "beedoo", "",
     StyleNormalPtr<Mix<Saw<Int<200>>, Red, Black, Black, Black, Red>, RED, 300, 800, RED>(),
#if NUM_BLADES == 7
     StyleNormalPtr<Mix<Saw<Int<200>>, Black, Red, Black, Black, Black>, RED, 300, 800, RED>(),
     StyleNormalPtr<Mix<Saw<Int<200>>, Black, Black, Red, Black, Black>, RED, 300, 800, RED>(),
     StyleNormalPtr<Mix<Saw<Int<200>>, Black, Black, Black, Red, Black>, RED, 300, 800, RED>(),
#endif
     StyleNormalPtr<BLACK, RED, 300, 800, RED>(),
     StyleNormalPtr<BLACK, RED, 300, 800, RED>(),
     StyleNormalPtr<BLACK, RED, 300, 800, RED>(), "beedoo"},
#endif

#if 1
   { "father", "tracks/impbells.wav",
     StyleNormalPtr<Stripes<500, -1000, Red, Red, White, White>, RED, 300, 800, RED>(),
#if NUM_BLADES == 7
     StyleNormalPtr<Stripes<500, -1000, White, Red, Red, White>, RED, 300, 800, RED>(),
     StyleNormalPtr<Stripes<500, -1000, White, White, Red, Red>, RED, 300, 800, RED>(),
     StyleNormalPtr<Stripes<500, -1000, Red, White, White, Red>, RED, 300, 800, RED>(),
#endif
     StyleNormalPtr<BLACK, RED, 300, 800, RED>(),
     StyleNormalPtr<BLACK, RED, 300, 800, RED>(),
     StyleNormalPtr<BLACK, RED, 300, 800, RED>(), "darth santa"},
#endif

#if 1
   // FIXME: TRACK?
   { "estaff", "tracks/uranus.wav",
     QUADBLADE(StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>() ),
     StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(),
     StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(),
     StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(), "strobe"},
#endif   

/*
 * Rejected and Disabled presets.
 */
#if 0
     { "RgueCmdr", "tracks/mercury.wav",
       // Rainbow swirl
	StyleNormalPtr<Stripes<250, -1000, Red, Red, Red, Red, Green, Green, Green, Green, Blue, Blue, Blue, Blue, Red>, White, 300, 800>(),
#if NUM_BLADES == 7     
       StyleNormalPtr<Stripes<250, -1000, Red, Green, Green, Green, Green, Blue, Blue, Blue, Blue, Red, Red, Red, Red>, White, 300, 800>(),
       StyleNormalPtr<Stripes<250, -1000, Green, Green, Blue, Blue, Blue, Blue, Red, Red, Red, Red, Green, Green, Green>, White, 300, 800>(),
       StyleNormalPtr<Stripes<250, -1000, Blue, Blue, Blue, Red, Red, Red, Red, Green, Green, Green, Green, Blue, Blue>, White, 300, 800>(),
#endif
     StyleNormalPtr<Stripes<1000, -1000, STRIPE_COLORS>, White, 300, 800>(),

     // Smaller rainbow for bubble strip
     StyleNormalPtr<Stripes<500, -500, STRIPE_COLORS>, White, 300, 800>(),

     // 488 makes the colors line up in the pommel = no seam
     // TO calculate, do 50000 * LEDs / (REPETITIONS * COLORS * 341)
     StyleNormalPtr<Stripes<488, 500, STRIPE_COLORS>, White, 300, 800>(),
     "rainbow swirl"},
#endif   

#if 0   
   { "SmthGrey", "tracks/mercury.wav",
     QUADBLADE( StylePtr<Layers<
      Red,
      ResponsiveLockupL<White,TrInstant,TrInstant,Int<26000>>,
      ResponsiveLightningBlockL<White>,
      ResponsiveMeltL<Mix<TwistAngle<>,Red,Yellow>>,
      ResponsiveDragL<White>,
      ResponsiveClashL<White,TrInstant,TrFade<200>,Int<26000>>,
      ResponsiveBlastL<White>,
      ResponsiveBlastWaveL<White>,
      ResponsiveBlastFadeL<White>,
      ResponsiveStabL<White>,
                InOutTrL<TrWipe<300>,TrWipeIn<500>>>>() ),
    StylePtr<Layers<
      Red,
      ResponsiveLockupL<White,TrInstant,TrInstant,Int<26000>>,
      ResponsiveLightningBlockL<White>,
      ResponsiveMeltL<Mix<TwistAngle<>,Red,Yellow>>,
      ResponsiveDragL<White>,
      ResponsiveClashL<White,TrInstant,TrFade<200>,Int<26000>>,
      ResponsiveBlastL<White>,
      ResponsiveBlastWaveL<White>,
      ResponsiveBlastFadeL<White>,
      ResponsiveStabL<White>,
      InOutTrL<TrWipe<300>,TrWipeIn<500>>>>(),
    StylePtr<Layers<
      Red,
      ResponsiveLockupL<White,TrInstant,TrInstant,Int<26000>>,
      ResponsiveLightningBlockL<White>,
      ResponsiveMeltL<Mix<TwistAngle<>,Red,Yellow>>,
      ResponsiveDragL<White>,
      ResponsiveClashL<White,TrInstant,TrFade<200>,Int<26000>>,
      ResponsiveBlastL<White>,
      ResponsiveBlastWaveL<White>,
      ResponsiveBlastFadeL<White>,
      ResponsiveStabL<White>,
      InOutTrL<TrWipe<300>,TrWipeIn<500>>,
      TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,  BatteryLevelStyle ,TrFade<3000>>,EFFECT_NEWFONT>
     >>(),
    StylePtr<Layers<
      Red,
      ResponsiveLockupL<White,TrInstant,TrInstant,Int<26000>>,
      ResponsiveLightningBlockL<White>,
      ResponsiveMeltL<Mix<TwistAngle<>,Red,Yellow>>,
      ResponsiveDragL<White>,
      ResponsiveClashL<White,TrInstant,TrFade<200>,Int<26000>>,
      ResponsiveBlastL<White>,
      ResponsiveBlastWaveL<White>,
      ResponsiveBlastFadeL<White>,
      ResponsiveStabL<White>,
      InOutTrL<TrWipe<300>,TrWipeIn<500>>
     >>(),
   "responsive"},
#endif   
#if 0   
   { "TeensySF", "tracks/mercury.wav",
     QUADBLADE(StyleNormalPtr<WHITE, RED, 300, 800, RED>() ),
     StyleNormalPtr<WHITE, RED, 300, 800, RED>(),
     StyleNormalPtr<WHITE, RED, 300, 800, RED>(),
     StyleNormalPtr<WHITE, RED, 300, 800, RED>(), "white"},
#endif

#if 0   
   { "SmthJedi", "tracks/uranus.wav",
     QUADBLADE( StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>() ),
     StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(),
     StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(),
     StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(), "yellow"},
#endif

#if 0   
   { "SmthFuzz", "tracks/mars.wav",
     QUADBLADE( StyleNormalPtr<Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>() ),
     StyleNormalPtr<Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>(),
     StyleNormalPtr<Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>(),
     StyleNormalPtr<Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>(),
     "gradient"},
#endif   
   
#if 0
   { "TeensySF", "tracks/mercury.wav",
     QUADBLADE(StyleNormalPtr<Mix<SliceF<FastSmokeDF>, Black, Cyan>, RED, 300, 800, RED>() ),
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(),
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(),
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(), "smoke"},
#endif


#if 0   
   { "", "",
     QUADBLADE( &style_charging ),
    StyleNormalPtr<BLACK, BLACK, 300, 800>(),
    StyleNormalPtr<BLACK, BLACK, 300, 800>(),
    StyleNormalPtr<BLACK, BLACK, 300, 800>(), "Battery\nLevel"}
#endif   
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
