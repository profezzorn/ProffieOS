#ifdef CONFIG_TOP
#include "v3_config.h"
#define NUM_BLADES 3
#define NUM_BUTTONS 2
#define VOLUME 1800
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define ORIENTATION ORIENTATION_USB_TOWARDS_BLADE
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
    { "LukeROTJ", "tracks/jabba.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Orange,200,100,400>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Red>>,White,40>,200,500,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "LukeROTJ"},
    { "R1Vader", "tracks/hall.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,Green>,AudioFlicker<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,White>>,White,40>,700,800,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "R1Vader"},
    { "Graflex8", "tracks/rey_training.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Rgb<0,123,255>,Rgb<0,62,139>>,White,200>,Red>,AudioFlicker<OnSpark<AudioFlicker<Blue,Rgb<0,0,139>>,White,200>,White>>,White,40>,350,500,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "Graflex8"},
    { "Kguard", "tracks/kylotlj.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,Blue,200,100,400>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>>,White,40>,300,800,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "Kguard"},
    { "balance", "tracks/venus.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<RandomPerLEDFlicker<HumpFlicker<Cyan,Green,50>,BrownNoiseFlicker<Green,Cyan,50>>,Cyan,200>,Blue>,AudioFlicker<OnSpark<RandomPerLEDFlicker<Gradient<Cyan, Green>,Gradient<Green, Cyan>>,White,200>,White>>,White,40>,500,800,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "balance"},
    { "Traya", "tracks/mars.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Strobe<BLACK, Yellow, 50, 1>, Strobe<RED, Black, 50, 1>, 50>>, WHITE>, 300, 800> >(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "Traya"},
    { "Light", "tracks/rots.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,SteelBlue,200,100,400>,AudioFlicker<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,Yellow>,AudioFlicker<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,White>>,White,40>,300,800,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "Light"},
    { "DrkKyber", "tracks/mars.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Sparkle<AudioFlicker<Red,Rgb<128,0,0>>,Yellow,300,1024>,Blue,200,100,400>,AudioFlicker<Sparkle<Red,Yellow,300,1024>,Cyan>,AudioFlicker<Sparkle<Blue,White,300,1024>,White>>,White,40>,300,800,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "DrkKyber"},
    { "vengence", "tracks/mercury.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Blue>,AudioFlicker<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Cyan>>,Cyan,40>,300,800,Black>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "vengence"},
    { "RgueCmdr", "tracks/venus.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<ColorCycle<Yellow,  0, 1, AudioFlicker<DarkOrange, Rgb<128,0,0>>,  100, 3000, 800>>, White>, 300, 800> >(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "RgueCmdr"},
    { "SAVANT", "tracks/mars.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<GREEN, Blue, 50>>, WHITE>, 300, 800> >(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "SAVANT"},
    { "SmthGrey", "tracks/mercury.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<Strobe<BLACK, Gradient<Magenta, Red>, 30, 1>>, WHITE>, 300, 800> >(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "SmthGrey"},
    {"SmthJedi", "tracks/uranus.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Yellow, Orange, 50>>, WHITE>, 300, 800> >(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "SmthJedi"},
    { "TeensySF", "tracks/venus.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Strobe<BLACK, White, 50, 1>, Strobe<Cyan, Black, 50, 1>, 50>>, WHITE>, 300, 800> >(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "TeensySF"},
    { "TthCrstl", "tracks/mars.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<RandomPerLEDFlicker<Green, Cyan>>, WHITE>, 300, 800> >(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "TthCrstl"},
    { "SmthFuzz", "tracks/rots.wav",
        StylePtr<InOutSparkTip<EasyBlade<AudioFlicker<HumpFlicker<Green, Blue, 50>, Red>, WHITE>, 300, 800>>(),
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "SmthFuzz"},
    { "SmthJedi", "tracks/mars.wav",
        &style_charging,
        StylePtr<InOutHelper<Blinking<Green,Black,200,500>,300,800,Blinking<Green, Black, 2000, 500>>>(),
        StylePtr<InOutHelper<Blinking<Black,Red,200,500>,300,800,Blinking<Black, Red, 2000, 500>>>(), "Battery\nLevel"}
};
BladeConfig blades[] = {
    { 0, WS2811BladePtr<130, WS2811_ACTUALLY_800kHz | WS2811_GRB>(),
        SubBlade(0,1, WS2811BladePtr<144, WS2811_800kHz, 8, PowerPINS<bladePowerPin6> >()),
        SubBlade(1,2, NULL)
        , CONFIGARRAY(presets) },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, aux2Pin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif

