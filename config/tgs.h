#ifdef CONFIG_TOP
#include "proffieboard_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 3
#define VOLUME 1700
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 3.1
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define SAVE_STATE
#endif

#ifdef CONFIG_PROP

#include "../props/saber_sa22c_buttons.h"

#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "ANH", "tracks/kylo.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,DeepSkyBlue>,RotateColorsX<Variation,Rgb<0,0,128>>>,TransitionEffectL<TrConcat<TrInstant,BrownNoiseFlickerL<AlphaL<White,Int<16000>>,Int<50>>,TrSmoothFade<600>>,EFFECT_LOCKUP_END>,ResponsiveLockupL<Strobe<White,BrownNoiseFlicker<White,Red,300>,50,1>,TrConcat<TrInstant,White,TrFade<200>>,TrFade<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<10000>,Int<14000>>>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,AlphaL<White,Bump<Int<0>,Scale<TwistAngle<>,Int<2000>,Int<10000>>>>,ResponsiveBlastWaveL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>,Int<400>>,ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveDragL<BrownNoiseFlickerL<White,Int<300>>,TrWipeIn<400>,TrFade<400>>,ResponsiveMeltL<Mix<TwistAngle<>,Red,Orange>,TrWipeIn<600>,TrSmoothFade<600>>,InOutTrL<TrWipe<300>,TrWipeIn<500>,Black>>>(), "ANH"},
  { "balance", "tracks/venus.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<BrownNoiseFlicker<Green,Blue,75>,Red,200,100,400>,AudioFlicker<BrownNoiseFlicker<Green,Blue,75>,LemonChiffon>,AudioFlicker<BrownNoiseFlicker<Green,Blue,75>,Orange>>,LemonChiffon,40>,300,800,White>>(), "balance"},
    { "vengence", "tracks/mercury.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Blue>,AudioFlicker<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Cyan>>,Cyan,40>,300,800,Black>>(), "Vengence"},
    { "Traya", "tracks/mars.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Strobe<BLACK, Yellow, 50, 1>, Strobe<RED, Black, 50, 1>, 50>>, WHITE>, 300, 800> >(), "Traya"},
    { "cyber", "tracks/cyber.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<GreenYellow,0,1,AudioFlicker<SpringGreen,Rgb<0,128,0>>,100,3000,1000>, WHITE>, 300, 800> >(), "cyber"},
    { "proto1", "tracks/proto1.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<Orange,0,1,AudioFlicker<Red,Rgb<128,0,0>>,100,3000,1000>, WHITE>, 300, 800> >(), "proto1"},
    { "proto2", "tracks/proto2.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<Cyan,0,1,AudioFlicker<SteelBlue,Rgb<0,0,128>>,100,3000,1000>, WHITE>, 300, 800> >(), "proto2"},
    { "proto3", "tracks/proto3.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<Pink,0,1,AudioFlicker<Magenta,Pink>,100,3000,1000>, WHITE>, 300, 800> >(), "proto3"},
    { "R1Vader", "tracks/hall.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,Green>,AudioFlicker<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,White>>,White,40>,700,800,Black>>(), "R1Vader"},
    { "Kguard", "tracks/kylotlj.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,Blue,200,100,400>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>>,White,40>,300,800,Black>>(), "Kguard"},
    { "DrkKyber", "tracks/mars.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Sparkle<AudioFlicker<Red,Rgb<128,0,0>>,Yellow,300,1024>,Blue,200,100,400>,AudioFlicker<Sparkle<Red,Yellow,300,1024>,Cyan>,AudioFlicker<Sparkle<Blue,White,300,1024>,White>>,White,40>,300,800,Black>>(), "DrkKyber"},
    { "RgueCmdr", "tracks/venus.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<ColorCycle<Yellow,  0, 1, AudioFlicker<DarkOrange, Rgb<128,0,0>>,  100, 3000, 800>>, White>, 300, 800> >(), "RgueCmdr"},

    { "LukeROTJ", "tracks/jabba.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Orange,200,100,400>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Red>>,White,40>,200,500,Black>>(),"LukeROTJ"},
    { "Graflex8", "tracks/rots.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,SteelBlue,200,100,400>,AudioFlicker<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,Yellow>,AudioFlicker<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,White>>,White,40>,300,800,Black>>(), "Light"},
    { "SAVANT", "tracks/mars.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<GREEN, Blue, 50>>, WHITE>, 300, 800> >(), "SAVANT"},
    { "SmthGrey", "tracks/mercury.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<Strobe<BLACK, Gradient<Magenta, Red>, 30, 1>>, WHITE>, 300, 800> >(), "SmthGrey"},
    {"SmthJedi", "tracks/uranus.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Yellow, Orange, 50>>, WHITE>, 300, 800> >(), "SmthJedi"},
    { "TeensySF", "tracks/venus.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<AudioFlicker<PeachPuff, PapayaWhip>>, WHITE>, 300, 800> >(), "TeensySF"},
    { "TthCrstl", "tracks/mars.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<RandomPerLEDFlicker<Green, Cyan>>, WHITE>, 300, 800> >(),  "TthCrstl"},
    { "SmthFuzz", "tracks/rots.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutSparkTip<EasyBlade<AudioFlicker<HumpFlicker<Green, Blue, 50>, Red>, WHITE>, 300, 800>>(), "SmthFuzz"},
    { "SmthJedi", "tracks/mars.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        &style_charging, "Battery\nLevel"}
};
BladeConfig blades[] = {
    //{ 0, SubBlade(0,2, WS2811BladePtr<144, WS2811_ACTUALLY_800kHz | WS2811_GRB>()),
     //   SubBlade(3,5, NULL),
     //   SubBlade(6,130, NULL),
     //    CONFIGARRAY(presets) },
    { 0, WS2811BladePtr<128, WS2811_ACTUALLY_800kHz | WS2811_GRB>(), CONFIGARRAY(presets) },

};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, auxPin, "pow");
Button AuxButton(BUTTON_AUX, aux2Pin, "aux");
Button Aux2Button(BUTTON_AUX2, powerButtonPin, "aux2");
#endif
