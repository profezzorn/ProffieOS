#ifdef CONFIG_TOP
#include "v3_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 1600
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 0.8
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define ORIENTATION ORIENTATION_USB_TOWARDS_BLADE
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
   { "LukeROTJ", "tracks/jaba_scene.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Orange,200,100,400>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Green,Rgb<0,139,0>>,White,200>,Red>>,White,40>,200,500,Black>>(), "green"},
   { "Graflex8", "tracks/rey_training.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Rgb<0,123,255>,Rgb<0,62,139>>,White,200>,Red>,AudioFlicker<OnSpark<AudioFlicker<Blue,Rgb<0,0,139>>,White,200>,White>>,White,40>,350,500,Black>>(), "blue"},
   { "kguard", "tracks/mercury.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<StyleFire<Rgb<255,0,0>,Rgb<255,80,0>,0,4,FireConfig<0,2000,0>,FireConfig<5000,5000,0>,FireConfig<0,5000,0>,FireConfig<100,0,50>>,White>,AudioFlicker<StyleFire<Red,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>,White>>,White,40>,300,800,Black>>(), "fire"},
   { "Vader", "tracks/hallway.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Red,Rgb<139,0,0>>,Blue,200>,Green>,AudioFlicker<OnSpark<AudioFlicker<Red,Rgb<139,0,0>>,White,200>,White>>,White,40>,500,500,Black>>(), "red"},
   { "SmthJedi", "tracks/venus.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Magenta,Rgb<139,0,139>>,White,200>,Red>,AudioFlicker<OnSpark<AudioFlicker<Magenta,Rgb<139,0,139>>,White,200>,White>>,White,40>,300,800,Black>>(), "purple"},
   { "TthCrstl", "tracks/mars.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Orange,Rgb<139,82,0>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Orange,Rgb<139,82,0>>,White,200>,White>>,White,40>,300,800,Black>>(), "orange"},
   { "TeensySF", "tracks/mercury.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Yellow,Rgb<139,139,0>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Yellow,Rgb<139,139,0>>,White,200>,White>>,White,40>,300,800,Black>>(), "Yellow"},
   { "Graflex", "tracks/venus.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<AudioFlicker<Cyan,Rgb<0,75,75>>,White,200>,White>,AudioFlicker<OnSpark<AudioFlicker<Cyan,Rgb<0,75,75>>,White,200>,White>>,White,40>,300,800,Black>>(), "Cyan"},
   { "SmthFuzz", "tracks/mars.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<StyleFire<Blue,Cyan,0,4,FireConfig<0,2000,0>,FireConfig<5000,5000,0>,FireConfig<0,5000,0>,FireConfig<100,0,50>>,White>,AudioFlicker<StyleFire<Blue,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>,White>>,White,40>,300,800,Black>>(), "BlueFire"},
   { "RgueCmdr", "tracks/mercury.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<HumpFlicker<Yellow,Green,30>,White,200>,White>,AudioFlicker<OnSpark<HumpFlicker<Yellow,Green,30>,White,200>,White>>,White,40>,300,800,Black>>(), "green-yellow"},
   { "SmthGrey", "tracks/uranus.wav",
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<Strobe<Rgb<126,250,255>,Black,20,1>,RandomPerLEDFlicker<White,Black>>,RandomPerLEDFlicker<Strobe<Black,White,20,1>,Black>>,RandomPerLEDFlicker<Strobe<Black,White,20,1>,Black>,150>,50,200,White>>(), "strobe"},
   { "DrkKyber", "tracks/venus.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<ColorCycle<Orange,0,1,AudioFlicker<Red,Rgb<139,0,0>>,100,5000,1000>,White,200>,White>,AudioFlicker<OnSpark<ColorCycle<Orange,0,1,AudioFlicker<Red,Rgb<139,0,0>>,100,5000,1000>,White,200>,White>>,White,40>,300,800,Black>>(), "CyRed"},
	{ "Vengence", "tracks/venus.wav",
	StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<Sparkle<Red, Yellow, 300, 1024>,White,200>,White>,AudioFlicker<OnSpark<Sparkle<Red, Yellow, 900, 1024>,White,200>,White>>,White,40>,300,800,Black>>(), "redspark"},
	{ "SAVANT", "tracks/venus.wav",
	StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<Sparkle<Orange, Red, 300, 1024>,White,200>,White>,AudioFlicker<OnSpark<Sparkle<Orange, Red, 900, 1024>,White,200>,White>>,White,40>,300,800,Black>>(), "ospark"},
   { "SmthJedi", "tracks/mars.wav",
    &style_charging, "Battery\nLevel"}
};
BladeConfig blades[] = {
 { 0, WS2811BladePtr<128, WS2811_ACTUALLY_800kHz | WS2811_GRB>(), CONFIGARRAY(presets) },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, aux2Pin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
