#ifdef CONFIG_TOP
#include "proffieboard_v1_config.h"
#define NUM_BLADES 2
#define NUM_BUTTONS 2
#define VOLUME 1800
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 2.1
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
    { "TFAFlex", "tracks/rey_training.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50, Blue,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Rgb<0,123,255>,Rgb<0,62,139>>,White,200>,Red>,AudioFlicker<OnSpark<AudioFlicker<Blue,Rgb<0,0,139>>,White,200>,White>>,White,40>,350,500,Black>>(), "Graflex8"},
    { "Darkness", "tracks/venus.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50, BrownNoiseFlicker<Green,Blue,75>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<BrownNoiseFlicker<Green,Blue,75>,Red,200,100,400>,AudioFlicker<BrownNoiseFlicker<Green,Blue,75>,LemonChiffon>,AudioFlicker<BrownNoiseFlicker<Green,Blue,75>,Orange>>,LemonChiffon,40>,300,800,White>>(), "balance"},
    { "savage", "tracks/mercury.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50, AudioFlicker<Magenta,Red>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Blue>,AudioFlicker<OnSpark<AudioFlicker<Magenta,Red>,White,200>,Cyan>>,Cyan,40>,300,800,Black>>(), "Vengence"},
    { "fulcrum", "tracks/mars.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50, BrownNoiseFlicker<Strobe<BLACK, Yellow, 50, 1>, Strobe<RED, Black, 50, 1>, 50>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Strobe<BLACK, Yellow, 50, 1>, Strobe<RED, Black, 50, 1>, 50>>, WHITE>, 300, 800> >(), "Traya"},
    { "DrkSbr", "tracks/cyber.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50, AudioFlicker<SpringGreen,Rgb<0,128,0>>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<GreenYellow,0,1,AudioFlicker<SpringGreen,Rgb<0,128,0>>,100,3000,1000>, WHITE>, 300, 800> >(), "cyber"},
    { "ESaber", "tracks/proto1.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<Red,Rgb<128,0,0>>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<Orange,0,1,AudioFlicker<Red,Rgb<128,0,0>>,100,3000,1000>, WHITE>, 300, 800> >(), "proto1"},
    { "EStaff", "tracks/proto2.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<SteelBlue,Rgb<0,0,128>>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<Cyan,0,1,AudioFlicker<SteelBlue,Rgb<0,0,128>>,100,3000,1000>, WHITE>, 300, 800> >(), "proto2"},
    { "Grevious", "tracks/proto3.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<Magenta,Pink>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutSparkTip<EasyBlade<ColorCycle<Pink,0,1,AudioFlicker<Magenta,Pink>,100,3000,1000>, WHITE>, 300, 800> >(), "proto3"},
    { "Father", "tracks/hall.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<Red,Rgb<128,0,0>>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,Green>,AudioFlicker<OnSpark<AudioFlicker<Red,Rgb<128,0,0>>,White,200>,White>>,White,40>,700,800,Black>>(), "R1Vader"},
    { "Crimson", "tracks/kylotlj.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,Blue,200,100,400>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Red,50,1>,Strobe<Red,Orange,50,1>,50>,White,200>,White>>,White,40>,300,800,Black>>(), "Kguard"},
    { "Maul", "tracks/mars.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<Red,Rgb<128,0,0>>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Sparkle<AudioFlicker<Red,Rgb<128,0,0>>,Yellow,300,1024>,Blue,200,100,400>,AudioFlicker<Sparkle<Red,Yellow,300,1024>,Cyan>,AudioFlicker<Sparkle<Blue,White,300,1024>,White>>,White,40>,300,800,Black>>(), "DrkKyber"},
    { "OB4", "tracks/venus.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<DarkOrange, Rgb<128,0,0>>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<ColorCycle<Yellow,  0, 1, AudioFlicker<DarkOrange, Rgb<128,0,0>>,  100, 3000, 800>>, White>, 300, 800> >(), "RgueCmdr"},

    { "LukeTLJ", "tracks/jabba.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<Green,Rgb<0,139,0>>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Orange,200,100,400>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Red>>,White,40>,200,500,Black>>(),"LukeROTJ"},
    { "ESBFlex", "tracks/rots.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<Coral,Moccasin>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,SteelBlue,200,100,400>,AudioFlicker<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,Yellow>,AudioFlicker<OnSpark<AudioFlicker<Coral,Moccasin>,White,200>,White>>,White,40>,300,800,Black>>(), "Light"},
    { "Outcast", "tracks/mars.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,BrownNoiseFlicker<GREEN, Blue, 50>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<GREEN, Blue, 50>>, WHITE>, 300, 800> >(), "SAVANT"},
    { "Rey", "tracks/mercury.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,Gradient<Magenta, Red>,  17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<Strobe<BLACK, Gradient<Magenta, Red>, 30, 1>>, WHITE>, 300, 800> >(), "SmthGrey"},
    {"Battle", "tracks/uranus.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,BrownNoiseFlicker<Yellow, Orange, 50>,17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<BrownNoiseFlicker<Yellow, Orange, 50>>, WHITE>, 300, 800> >(), "SmthJedi"},
    { "Survivor", "tracks/venus.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,AudioFlicker<PeachPuff, PapayaWhip>,17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<AudioFlicker<PeachPuff, PapayaWhip>>, WHITE>, 300, 800> >(), "TeensySF"},
    { "Unstable", "tracks/mars.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,RandomPerLEDFlicker<Green, Cyan>,17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<RandomPerLEDFlicker<Green, Cyan>>, WHITE>, 300, 800> >(),  "TthCrstl"},
    { "Violence", "tracks/rots.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,HumpFlicker<Green, Blue, 50>,17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        StylePtr<InOutSparkTip<EasyBlade<AudioFlicker<HumpFlicker<Green, Blue, 50>, Red>, WHITE>, 300, 800>>(), "SmthFuzz"},
    { "Battery", "tracks/mars.wav",
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<ColorCycle<White,  17, 50,HumpFlicker<Green, Blue, 50>,17, 450, 4000>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>,Strobe<Black,White,15,1>>, 300, 6000, ColorCycle<White,  17, 50, White,  17, 50, 1>>>(),
        &style_charging, "Battery\nLevel"}
};
BladeConfig blades[] = {
    { 0, SubBlade(0,5, WS2811BladePtr<144, WS2811_ACTUALLY_800kHz | WS2811_GRB>()),
        SubBlade(6,130, NULL),
         CONFIGARRAY(presets) },

};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
//Button Aux2Button(BUTTON_AUX2, aux2Pin, "aux2");
#endif
