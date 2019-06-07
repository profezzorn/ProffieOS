#ifdef CONFIG_TOP
#include "proffieboard_v1_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 3
#define VOLUME 260
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 5.1
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define USB_CLASS_WEBUSB
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
    { "savage", "tracks/kylo.wav",
            StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<AudioFlicker<DeepSkyBlue,SteelBlue>,Gradient<Red,Orange,Yellow,White>,200,100,400>,Pulsing<Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,3500>,AudioFlicker<White,DeepSkyBlue>>,White,40,85>,180,500,Black>>(), "Light"},
    { "light", "tracks/kylo.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
      StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<AudioFlicker<DeepSkyBlue,SteelBlue>,Gradient<Red,Orange,Yellow,White>,200,100,400>,Pulsing<Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,3500>,AudioFlicker<White,DeepSkyBlue>>,White,40,85>,180,500,Black>>(), "Light"},
    { "balance", "tracks/venus.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
      StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<AudioFlicker<DodgerBlue,Blue>,White,200,100,400>,Pulsing<Gradient<AudioFlicker<DodgerBlue,Blue>,AudioFlicker<DodgerBlue,Blue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1> ,100>,AudioFlicker<DodgerBlue,Blue>,AudioFlicker<DodgerBlue,Blue>,AudioFlicker<DodgerBlue,Blue>>,Gradient<AudioFlicker<DodgerBlue,Blue>,AudioFlicker<DodgerBlue,Blue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker <DodgerBlue,Blue>,AudioFlicker<DodgerBlue,Blue>>,3500>,AudioFlicker<White,DodgerBlue>>,White,40>,180,500,Black>>(), "balance"},
  { "Graflex8", "tracks/rots.wav",
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
      StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<AudioFlicker<DeepSkyBlue,SteelBlue>,Gradient<Red,Orange,Yellow,White>,200,100,400>,Pulsing<Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,3500>,AudioFlicker<White,DeepSkyBlue>>,White,40,85>,180,500,Black>>(), "Graflex8"},
  { "Kguard", "tracks/kylotlj.wav",
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
    StylePtr<InOutHelper<OnSpark<Lockup<StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,300>,Rgb<80,0,0>,0,6,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,Pulsing<Gradient<StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,RandomPerLEDFlicker<White,Strobe<Blue,White,50,1>>,StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>>,Gradient<StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,RandomPerLEDFlicker<White,Strobe<Blue,White,50,1>>,StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>,StyleFire<Blast<LocalizedClash<BrownNoiseFlicker<Red,RandomPerLEDFlicker<Rgb<25,0,0>,Rgb<60,0,0>>,200>,White,80,50>,White,200,100,400>,Rgb<80,0,0>,0,8,FireConfig<10,1000,2>,FireConfig<4000,0,0>,FireConfig<2,1000,5>,FireConfig<100,0,5>>>,3500>,RandomPerLEDFlicker<Red,White>>,BrownNoiseFlicker<StyleFire<Red,DarkOrange,0,7,FireConfig<0,2000,3>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,5>>,RandomPerLEDFlicker<Red,Rgb<100,0,0>>,30>,1000>,150,500,Black>>(), "Kguard"},
  { "R1Vader", "tracks/hall.wav",
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
    StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<AudioFlicker<Red,Rgb<120,0,0>>,DeepSkyBlue,200,100,400>,Pulsing<Gradient<AudioFlicker<Red,Rgb<120,0,0>>,AudioFlicker<Red,Rgb<120,0,0>>,BrownNoiseFlicker<White,Strobe<SteelBlue,White,50,1>,100>,AudioFlicker<Red,Rgb<120,0,0>>,AudioFlicker<Red,Rgb<120,0,0>>,AudioFlicker<Red,Rgb<120,0,0>>>,Gradient<AudioFlicker<Red,Rgb<120,0,0>>,AudioFlicker<Red,Rgb<120,0,0>>,BrownNoiseFlicker<White,Strobe<SteelBlue,White,50,1>,100>,AudioFlicker<Red,Rgb<120,0,0>>,AudioFlicker<Red,Rgb<120,0,0>>>,3500>,AudioFlicker<Red,White>>,White,80,50>,800,500,Black>>(), "R1Vader"},
  { "LukeROTJ", "tracks/jabba.wav",
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
    //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
    StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<AudioFlicker<Green,Rgb<0,200,0>>,FloralWhite,200,200,400>,Pulsing<Gradient<AudioFlicker<Green,Rgb<0,200,0>>,AudioFlicker<Green,Rgb<0,200,0>>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<Green,Rgb<0,200,0>>,AudioFlicker<Green,Rgb<0,200,0>>,AudioFlicker<Green,Rgb<0,200,0>>>,Gradient<AudioFlicker<Green,Rgb<0,200,0>>,AudioFlicker<Green,Rgb<0,200,0>>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<Green,Rgb<0,200,0>>,AudioFlicker<Green,Rgb<0,200,0>>>,3500>,AudioFlicker<White,Green>>,FloralWhite,80,80>,180,500,Black>>(),"LukeROTJ"},
    { "vengence", "tracks/mercury.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<AudioFlicker<Magenta,Red>,Gradient<Blue,Cyan,White>,200,100,400>,Pulsing<Gradient<AudioFlicker<Magenta,Red>,AudioFlicker<Magenta,Red>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<Magenta,Red>,AudioFlicker<Magenta,Red>,AudioFlicker<Magenta,Red>>,Gradient<AudioFlicker<Magenta,Red>,AudioFlicker<Magenta,Red>,BrownNoiseFlicker<White,Strobe<Red,White,50,1>,100>,AudioFlicker<Magenta,Red>,AudioFlicker<Magenta,Red>>,3500>,AudioFlicker<White,Magenta>>,White,40,85>,300,500,Black>>(), "Vengence"},
    { "Traya", "tracks/mars.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<LocalizedClash<Lockup<Blast<OnSpark<BrownNoiseFlicker<Strobe<Black,Yellow,50,1>,Strobe<Red,Black,50,1>,50>,Gradient<GreenYellow,Green,White>,200>,White,200,100,400>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Yellow,50,1>,Strobe<Red,Black,50,1>,50>,White,200>,White>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Yellow,50,1>,Strobe<Red,Black,50,1>,50>,White,200>,White>>,White,40,100>,300,800,Black>>(), "Traya"},
    { "Satele", "tracks/revan.wav",
      StylePtr<InOutHelper<OnSpark<LocalizedClash<Lockup<Blast<AudioFlicker<Yellow,LightYellow>,Gradient<White,Red,White>,200,100,400>,Pulsing<Gradient<AudioFlicker<Yellow, LightYellow>,AudioFlicker<Yellow, LightYellow>,BrownNoiseFlicker<Red,Strobe<Red,Orange,50,1>,100>,AudioFlicker<Yellow, LightYellow>,AudioFlicker<Yellow, LightYellow>,AudioFlicker<Yellow, LightYellow>>,Gradient<AudioFlicker<Yellow, LightYellow>,AudioFlicker<Yellow, LightYellow>,BrownNoiseFlicker<Orange,Strobe<Red,Orange,50,1>,100>,AudioFlicker<Yellow, LightYellow>,AudioFlicker<Yellow, LightYellow>>,3500>,RandomPerLEDFlicker<White,LightYellow>>,Orange,40>,HumpFlicker<LightYellow,White,40>,800>,180,500,Black>>(), "Satele Shan"},
    { "GreyTwo", "tracks/rots.wav",
      StylePtr<InOutHelper<OnSpark<LocalizedClash<Lockup<Blast<AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,Yellow,200,100,400>,Pulsing<Gradient<AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,BrownNoiseFlicker<Yellow,Strobe<Red,Yellow,50,1>,100>,AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>>,Gradient<AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,BrownNoiseFlicker<Yellow,Strobe<Red,Yellow,50,1>,100>,AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>,AudioFlicker<Rgb<120,20,235>,Rgb<60,10,170>>>,3500>,RandomPerLEDFlicker<White,Rgb<120,20,235>>>,Yellow,40>,HumpFlicker<Rgb<120,20,235>,White,40>,800>,180,500,Black>>(), "GreyTwo" },
  { "Tano", "tracks/ahsoka.wav",
    StylePtr<InOutHelper<OnSpark<LocalizedClash<Lockup<Blast<AudioFlicker<Rgb<128,128,128>,Cyan>,Gradient<White,Red,White>,200,100,400>,Pulsing<Gradient<AudioFlicker<Rgb<128,128,128>,Cyan>,AudioFlicker<Rgb<128,128,128>,Cyan>,BrownNoiseFlicker<Red,Strobe<Red,Orange,50,1>,100>,AudioFlicker<Rgb<128,128,128>,Cyan>,AudioFlicker<Rgb<128,128,128>,Cyan>,AudioFlicker<Rgb<128,128,128>,Cyan>>,Gradient<AudioFlicker<Rgb<128,128,128>,Cyan>,AudioFlicker<Rgb<128,128,128>,Cyan>,BrownNoiseFlicker<Orange,Strobe<Red,Orange,50,1>,100>,AudioFlicker<Rgb<128,128,128>,Cyan>,AudioFlicker<Rgb<128,128,128>,Cyan>>,3500>,RandomPerLEDFlicker<White,Cyan>>,Orange,40>,HumpFlicker<Rgb<128,128,128>,Cyan,40>,800>,180,500,Black>>(),"Ahsoka"},
  { "TFlexA", "TFA_Duel.wav",
  StylePtr<InOutHelper<OnSpark<LocalizedClash<Lockup<Blast<AudioFlicker<DeepSkyBlue,SteelBlue>,Yellow,200,100,400>,Pulsing<Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<Yellow,Strobe<Red,Yellow,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,Gradient<AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>,BrownNoiseFlicker<Yellow,Strobe<Red,Yellow,50,1>,100>,AudioFlicker<DeepSkyBlue,SteelBlue>,AudioFlicker<DeepSkyBlue,SteelBlue>>,3500>,RandomPerLEDFlicker<White,DeepSkyBlue>>,Yellow,40>,HumpFlicker<DeepSkyBlue,White,40>,800>,180,500,Black>>()},
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
    { "DrkKyber", "tracks/mars.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Sparkle<AudioFlicker<Red,Rgb<128,0,0>>,Yellow,300,1024>,Blue,200,100,400>,AudioFlicker<Sparkle<Red,Yellow,300,1024>,Cyan>,AudioFlicker<Sparkle<Blue,White,300,1024>,White>>,White,40>,300,800,Black>>(), "DrkKyber"},
    { "RgueCmdr", "tracks/venus.wav",
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Blinking<Blue, Black,200,500>,White>,Blinking<Blue,Black,50,500>>,White>, 300, 800>>(),
        //StylePtr<InOutHelper<SimpleClash<Lockup<Blast<Strobe<Black,White,15,1>,White>,AudioFlicker<Strobe<Black,White,15,1>,WHITE>>,White>, 300, 800>>(),
        StylePtr<InOutHelper<EasyBlade<OnSpark<ColorCycle<Yellow,  0, 1, AudioFlicker<DarkOrange, Rgb<128,0,0>>,  100, 3000, 800>>, White>, 300, 800> >(), "RgueCmdr"},
      { "LSword", "tracks/rots.wav",
        StylePtr<InOutSparkTip<LocalizedClash<Lockup<Blast<BrownNoiseFlicker<Green,Blue,75>,Gradient<Red,Orange,Yellow,White>,200,100,400>,AudioFlicker<BrownNoiseFlicker<Green,Blue,75>,LemonChiffon>,AudioFlicker<BrownNoiseFlicker<Green,Blue,75>,Orange>>,LemonChiffon,40,100>,300,800,White>>(), "LSword"},
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
    { 0, WS2811BladePtr<132, WS2811_ACTUALLY_800kHz | WS2811_GRB>(), CONFIGARRAY(presets) },
    
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
Button Aux2Button(BUTTON_AUX2, aux2Pin, "aux2");
#endif
