/*
This is a sample configuration file for use with OS6 and above.
This saber has:
 - Proffieboard V2.2.
 - Two buttons: one POW, one AUX.
 - A 144 pixel blade on LED 2&3 and data 1 pads.
Examples given for:
- POV data file define
- Custom prop file inclusion (Fett263) with suggested minimum of #defines.
- OS6 Edit Mode compliant blade style
- POV blade style

More info on Edit Mode can be found here: 
https://fett263.s3.us-east-2.amazonaws.com/proffieOS6-edit-mode.html
More info on using POV can be found here:
https://github.com/profezzorn/ProffieOS/wiki/POV-(Persistance-Of-Vision)
If you have a saber similar to this one, make a copy and use the copy.
*/

#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 1000
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define SAVE_STATE
#define ENABLE_ALL_EDIT_OPTIONS     // required for Edit Mode functionality
#define FETT263_EDIT_MODE_MENU      // required for menu system
#define FETT263_SAY_COLOR_LIST      // recommended to enable spoken color names in Color List editing
#define FETT263_SAY_COLOR_LIST_CC   // recommended to enable spoken color names for Color List Color Change Mode
#define DISABLE_BASIC_PARSER_STYLES // recommended for memory saving and using built-in styles for ProffieOS Workbench
#define DISABLE_DIAGNOSTIC_COMMANDS // recommended for additional memory saving, will disable most Serial Monitor commands.
#define POV_FILE_INCLUDE "image_data.h" // If no file provided like this, the default Star Wars logo will be used.
#endif

#ifdef CONFIG_PROP
#include "../props/saber_fett263_buttons.h"
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
  { "TeensySF;common", "tracks/venus.wav",
    /* Â© Fett263 Rotoscope (Primary Blade) OS6 Style
Style Options
Default(0): Base Color (Original)
1: Base Color + Alt Color (Original)
2: Base Color (Fast)
3: Base Color + Alt Color (Fast)
4: Base Color (Slow)
5: Base Color + Alt Color (Slow)
Effects =
Ignition Effect Options: Default (0): Standard Ignition, 1: Wipe In, 2: Cycle Up, 3: SparkTip Ignition, 4: Center Out, 5: Center In
Power Up Effect Options: Default (0): Disabled, 1: Unstable Power Up, 2: Bright Power Up , 3: AudioFlicker Power Up
Retraction Effect Options: Default (0): Standard Retraction, 1: Run Up, 2: Cycle Down, 3: SparkTip Retraction 4: Split Retraction, 5: Center In
Cool Down Effect Options: Default (0): Disabled, 1: Unstable Cool Down, 2: Bright Cool Down, 3: AudioFlicker Cool Down
Preon Effect Options: Default(0): Disabled, 1: Faulty Ignition V1, 2: Faulty Ignition V2, 3: Overload, 4: Erratic, 5: Pulsing
Swing Effect: Default (0): Disabled, 1: Enhanced Bright Swing, 2: Bright Swing, 3: Force Boost Enhanced Bright Swing, 4: Force Boost Bright Swing
Lockup Effect: Intensity Lockup
Lightning Block Effect: Responsive Lightning Block
Drag Effect: Standard Drag
Melt Effect: Intensity Melt
Blast Effect: Multi-Blast (Sequence)
Clash Effect: Real Clash
Battery Level: % Blade - Red to Green
*/
StylePtr<Layers<
ColorSelect<IntArg<STYLE_OPTION_ARG, 0>,TrInstant,RandomFlicker<Stripes<10000,-2600,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RandomFlicker<Stripes<10000,-2600,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<ALT_COLOR_ARG,Rgb<255,68,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<ALT_COLOR_ARG,Rgb<255,68,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RandomFlicker<Stripes<14000,-4000,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RandomFlicker<Stripes<14000,-4000,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<ALT_COLOR_ARG,Rgb<255,68,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<ALT_COLOR_ARG,Rgb<255,68,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RandomFlicker<Stripes<24000,-1400,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<11565>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RandomFlicker<Stripes<24000,-1400,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<ALT_COLOR_ARG,Rgb<255,68,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<ALT_COLOR_ARG,Rgb<255,68,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,
ColorSelect<IntArg<SWING_OPTION_ARG, 0>,TrInstant,AlphaL<RgbArg<SWING_COLOR_ARG,Rgb<125,125,125>>,Int<0>>,AlphaL<RgbArg<SWING_COLOR_ARG,Rgb<125,125,125>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,AlphaL<RgbArg<SWING_COLOR_ARG,Rgb<125,125,125>>,SwingSpeed<600>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<SWING_COLOR_ARG,Rgb<125,125,125>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,TrDelay<30000>>,EFFECT_FORCE>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<SWING_COLOR_ARG,Rgb<125,125,125>>,SwingSpeed<600>>,TrDelay<30000>>,EFFECT_FORCE>>,
TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<10280>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFadeX<IgnitionTime<300>>>,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,TrFadeX<IgnitionTime<300>>>,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrFadeX<IgnitionTime<300>>>>,EFFECT_IGNITION>,
TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrInstant,HumpFlickerL<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,40>,TrDelayX<RetractionTime<0>>>,TrConcat<TrInstant,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,TrDelayX<RetractionTime<0>>>>,EFFECT_RETRACTION>,
LockupTrL<AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<16000>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<  Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,  Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<  Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>  ,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,  Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,
ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,
EffectSequence<EFFECT_BLAST,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,BlastF<200,200>>,TrFade<300>>,EFFECT_BLAST>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>,Int<400>>,ResponsiveBlastWaveL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<SwingSpeed<400>,Int<500>,Int<200>>,Scale<SwingSpeed<400>,Int<100>,Int<400>>>,ResponsiveBlastFadeL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<SwingSpeed<400>,Int<6000>,Int<12000>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>,Scale<SwingSpeed<400>,Int<200>,Int<100>>,Scale<SwingSpeed<400>,Int<400>,Int<200>>>>,
Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,ResponsiveBlastWaveL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>,EFFECT_CLASH>>,
LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,31000>,Int<2000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,OrangeRed,DarkOrange>,Mix<TwistAngle<>,OrangeRed,Orange>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<6000>>>,TrConcat<TrWipeIn<100>,AlphaL<Red,SmoothStep<Int<29000>,Int<8000>>>,TrExtend<2000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<29000>,Int<8000>>>,TrFade<3000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<IgnitionTime<300>>,TrWipeInX<IgnitionTime<300>>,TrColorCycleX<IgnitionTime<300>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,IgnitionTime<300>>,TrCenterWipeX<IgnitionTime<300>,Int<16384>>,TrCenterWipeInX<IgnitionTime<300>,Int<16384>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<RetractionTime<0>>,TrWipeX<RetractionTime<0>>,TrColorCycleX<RetractionTime<0>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RetractionTime<0>>,TrCenterWipeInX<RetractionTime<0>,Int<16384>>,TrCenterWipeX<RetractionTime<0>,Int<16384>>>,RotateColorsX<Variation,Black>>,
TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<Black,RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,Rgb<50,50,50>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,RotateColorsX<Int<4000>,RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>>>,White,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrInstant,AlphaL<RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,Black>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,10>,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,15>,SmoothStep<Sum<Int<2000>,IntArg<PREON_SIZE_ARG,2000>>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,20>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,2000>>,Int<-4000>>>,TrBoingX<Mult<Int<8192>,WavLen<EFFECT_PREON>>,3>>,TrConcat<TrBoingX<Mult<Int<10922>,WavLen<EFFECT_PREON>>,5>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,Int<100>>,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrBoingX<Mult<Int<10922>,WavLen<EFFECT_PREON>>,3>,Black,TrBoingX<Mult<Int<7260>,WavLen<EFFECT_PREON>>,3>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,Int<200>>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,2000>>,Int<-4000>>>,TrBoingX<Mult<Int<3664>,WavLen<EFFECT_PREON>>,3>>,TrConcat<TrFadeX<Mult<Int<4096>,WavLen<EFFECT_PREON>>>,AlphaL<PulsingL<RgbArg<PREON_COLOR_ARG,Rgb<255,0,0>>,Int<800>>,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrDelayX<Mult<Int<28672>,WavLen<EFFECT_PREON>>>>>,EFFECT_PREON>,
TransitionEffectL<TrConcat<TrExtend<3000,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<1000>>,EFFECT_BATTERY_LEVEL>
>>(), 
"fett263\nrotoscope"},
  
  { "SmthJedi", "tracks/mars.wav",
    &style_pov,
    "POV"}

};
BladeConfig blades[] = {
 { 0,
  WS281XBladePtr<144, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
     CONFIGARRAY(presets) },
  };
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
