#ifdef CONFIG_TOP
#include "proffieboard_v1_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 1850
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.1
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
   { "Light", "tracks/rots.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<AudioFlicker<Rgb<35, 70, 236>, Rgb<17,35,128>>>, WHITE>, 300, 800> >(), "Light"},
   { "Graflex8", "tracks/rey.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Cyan,Rgb<0,64,128>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Cyan,Rgb<0,64,128>>,White,200>,White>>,White,40>,300,800,Black>>(), "Graflex8"},
   { "Kguard", "tracks/kylotlj.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,Blue,200,100,400>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>>,White,40>,300,800,Black>>(), "Kguard"},
    { "R1Vader", "tracks/hall.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,Green>,AudioFlicker<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,White>>,White,40>,700,800,Black>>(), "R1Vader"},
    { "LukeROTJ", "tracks/jaba.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<AudioFlicker<GREEN, Rgb<0,128,0>>>, WHITE>, 200, 800> >(), "LukeROTJ"},
    { "Traya", "tracks/traya.wav",
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Strobe<BLACK, Yellow, 50, 1>, Strobe<RED, Black, 50, 1>, 50>>, WHITE>, 300, 800> >(), "Traya"},
    { "DrkKyber", "tracks/mars.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Pulsing<AudioFlicker<Red,Rgb<50,0,0>>,AudioFlicker<Rgb<50,0,0>,Red>,250>,Green>,AudioFlicker<Pulsing<AudioFlicker<Red,Rgb<50,0,0>>,AudioFlicker<Rgb<50,0,0>,Red>,250>,White>>,Green,40>,300,800,Black>>(), "DrkKyber"},
    { "balance", "tracks/revan.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<RandomPerLEDFlicker<Gradient<Green,Yellow>,Gradient<Yellow,Green>>,Cyan,200>,Blue>,AudioFlicker<OnSpark<RandomPerLEDFlicker<Gradient<Green,Yellow>,Gradient<Yellow,Green>>,White,200>,Cyan>>,Cyan,40>,500,800,Black>>(), "balance"},
   { "vengence", "tracks/mercury.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Blue>,AudioFlicker<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Cyan>>,Cyan,40>,300,800,Black>>(), "vengence"},
   { "RgueCmdr", "tracks/venus.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<ColorCycle<Yellow,  0, 1, AudioFlicker<DarkOrange, Rgb<128,0,0>>,  100, 3000, 800>>, White>, 300, 800> >(), "RgueCmdr"},
   { "SAVANT", "tracks/mars.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<GREEN, Blue, 50>>, WHITE>, 300, 800> >(), "SAVANT"},
   { "SmthGrey", "tracks/mercury.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<Strobe<BLACK, Gradient<Magenta, Red>, 30, 1>>, WHITE>, 300, 800> >(), "SmthGrey"},
   { "SmthJedi", "tracks/uranus.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Yellow, Orange, 50>>, WHITE>, 300, 800> >(), "SmthJedi"},
   { "TeensySF", "tracks/venus.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Strobe<BLACK, White, 50, 1>, Strobe<Cyan, Black, 50, 1>, 50>>, WHITE>, 300, 800> >(), "TeensySF"},
   { "TthCrstl", "tracks/mars.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<RandomPerLEDFlicker<Green, Cyan>>, WHITE>, 300, 800> >(), "TthCrstl"},
   { "SmthJedi", "tracks/mars.wav",
    &style_charging, "Battery\nLevel"}
};
BladeConfig blades[] = {
 { 0, WS2811BladePtr<138, WS2811_ACTUALLY_800kHz | WS2811_GRB>(), CONFIGARRAY(presets) },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
