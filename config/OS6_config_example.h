/*
This is a sample configuration file for use with OS6 and above.
This saber has:
 - Proffieboard V2.2.
 - Two buttons: one POW, one AUX.
 - A 136 pixel blade on LED 2&3 and data 1 pads.
 - A 6 pixel accent/crystal strip on LED4 and data 2 pads.
Examples given for:
- Custom prop file inclusion (Fett263) with suggested minimum of #defines.
- OS6 Edit Mode compliant blade style

More info on Edit Mode can be found here: 
https://fett263.s3.us-east-2.amazonaws.com/proffieOS6-edit-mode.html
If you have a saber similar to this one, make a copy and use the copy.
*/

#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 2
#define NUM_BUTTONS 2
#define VOLUME 1800
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 4.5
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define COLOR_CHANGE_DIRECT
#define DISABLE_DIAGNOSTIC_COMMANDS
#define DISABLE_BASIC_PARSER_STYLES
#define NO_REPEAT_RANDOM
#define FETT263_EDIT_MODE_MENU
#define ENABLE_ALL_EDIT_OPTIONS
#define KEEP_SAVEFILES_WHEN_PROGRAMMING
#define FETT263_THRUST_ON
#define FETT263_SWING_ON
#define FETT263_SWING_ON_SPEED 300
#define FETT263_TWIST_OFF
#define FETT263_TWIST_ON
#define MOTION_TIMEOUT 60 * 15 * 1000
#define IDLE_OFF_TIME 5 * 60 * 1000
#define FETT263_MULTI_PHASE
#define FETT263_CLASH_STRENGTH_SOUND
#define FETT263_DUAL_MODE_SOUND
#define FETT263_MAX_CLASH 16
#define FETT263_BM_CLASH_DETECT 7
#define FETT263_SAY_BATTERY_PERCENT
#define FETT263_SAY_COLOR_LIST
#define FETT263_SAY_COLOR_LIST_CC
//#define FETT263_SAVE_CHOREOGRAPHY
#endif

#ifdef CONFIG_PROP
#include "../props/saber_fett263_buttons.h"
#endif

#ifdef CONFIG_PRESETS

Preset presets[] = {

{ "Fallen;common", "Fallen/tracks/JFO6.wav",
/* copyright Fett263 FallenOrder (Primary Blade) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#FallenOrder
OS6.5 v1.0
This Style Contains 648 Unique Combinations
Style Options:
Default (0): Fallen Order Cal Kestis
1: Fallen Order Second Sister

--Effects Included--
Preon Effect Options: Default (0): Disable, 1: Overload (PreOn Color), 2: Sparking (PreOn Color)
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Up = Fast)
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect Options: Default (0): Standard Retraction, 1: Dual Mode Wipe In (Up = Fast)
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Lockup Effect: Intensity Lockup V1
LightningBlock Effect: Responsive Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Sound Based), Blast Ripple, Blast Fade (Sound Based)
Clash Effect: Real Clash V1
Swing Effect Options: Default (0): Disable, 1: Force Slow
Battery Level: Blade Location (Green to Red)
*/
StylePtr<Layers<
  ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,StripesX<Int<6000>,Scale<SlowNoise<Int<2000>>,Int<-1600>,Int<-3200>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,BrownNoiseFlicker<Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,300>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Black,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,StripesX<Int<1500>,Scale<SlowNoise<Int<2500>>,Int<-3000>,Int<-5000>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<2570>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  MultiTransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrWaveX<Remap<CenterDistF<EffectPosition<>>,Stripes<1500,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<160>,Int<100>,Int<300>,EffectPosition<>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSparkX<Remap<CenterDistF<EffectPosition<>>,Stripes<2000,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<100>,Int<300>,EffectPosition<>>>,TrConcat<TrInstant,AlphaMixL<Bump<EffectPosition<>,Scale<WavLen<>,Int<9000>,Int<12000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,
  TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,
  TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
  LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,
  ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,
  LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
  LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,OrangeRed,DarkOrange>,Mix<TwistAngle<>,OrangeRed,Orange>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<Red,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrExtend<4000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
  InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<IgnitionTime<300>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,IgnitionTime<300>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<IgnitionTime<300>,Int<16384>>,IgnitionTime<300>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<RetractionTime<0>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<0>,Int<16384>>,RetractionTime<0>>>>,Black>,
  TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,10>,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,15>,SmoothStep<Sum<Int<2000>,IntArg<PREON_SIZE_ARG,2000>>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,20>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,2000>>,Int<-4000>>>,TrBoingX<Mult<Int<8192>,WavLen<EFFECT_PREON>>,3>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<4000>>>,Int<-2000>>>,TrDelayX<WavLen<EFFECT_PREON>>>>,EFFECT_PREON>,
  TransitionEffectL<TrConcat<TrExtend<2000,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<300>>,EFFECT_BATTERY_LEVEL>
>>(),

/* copyright Fett263 FallenOrder (Crystal Chamber) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#FallenOrder
OS6.5 v1.0
This Style Contains 72 Unique Combinations
Style Options:
Default (0): Fallen Order Cal Kestis
1: Fallen Order Second Sister

Off Behavior Options: Default (0): Heart Beat, 1: Kyber Flow Up

--Effects Included--
Ignition Effect: Instant
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect: Instant
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Swing Effect Options: Default (0): Disable, 1: Force Slow
*/
StylePtr<Layers<
  ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,StripesX<Int<6000>,Scale<SlowNoise<Int<2000>>,Int<-1600>,Int<-3200>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,BrownNoiseFlicker<Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,300>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Black,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,StripesX<Int<1500>,Scale<SlowNoise<Int<2500>>,Int<-3000>,Int<-5000>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<2570>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  InOutTrL<TrInstant,TrInstant,ColorSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TransitionLoop<RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,TrConcat<TrBoing<1200,2>,Mix<Int<1285>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,TrFade<1200>>>,Stripes<5000,-2500,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>>>
>>(),
"preset 1" },

{ "Luke;common", "Luke/tracks/LukeTheme.wav",
/* copyright Fett263 Static/Flicker (Primary Blade) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#Static/Flicker
OS6.5 v1.0
This Style Contains 1296 Unique Combinations
Style Options:
Default (0): AudioFlicker (Single Color (50%))
1: RandomFlicker (Single Color (50%))
2: AudioFlicker (Two Color)
3: RandomFlicker (Two Color)

--Effects Included--
Preon Effect Options: Default (0): Disable, 1: Overload (PreOn Color), 2: Sparking (PreOn Color)
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Up = Fast)
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect Options: Default (0): Standard Retraction, 1: Dual Mode Wipe In (Up = Fast)
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Lockup Effect: Intensity Lockup V1
LightningBlock Effect: Responsive Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Sound Based), Blast Ripple, Blast Fade (Sound Based)
Clash Effect: Real Clash V1
Swing Effect Options: Default (0): Disable, 1: Force Slow
Battery Level: Blade Location (Green to Red)
*/
StylePtr<Layers<
  ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,RandomFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<ALT_COLOR_ARG,Rgb<255,50,50>>>,RandomFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<ALT_COLOR_ARG,Rgb<255,50,50>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  MultiTransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrWaveX<Remap<CenterDistF<EffectPosition<>>,Stripes<1500,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<160>,Int<100>,Int<300>,EffectPosition<>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSparkX<Remap<CenterDistF<EffectPosition<>>,Stripes<2000,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<100>,Int<300>,EffectPosition<>>>,TrConcat<TrInstant,AlphaMixL<Bump<EffectPosition<>,Scale<WavLen<>,Int<9000>,Int<12000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,
  TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,
  TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
  LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,
  ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,
  LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
  LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,OrangeRed,DarkOrange>,Mix<TwistAngle<>,OrangeRed,Orange>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<Red,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrExtend<4000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
  InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<IgnitionTime<300>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,IgnitionTime<300>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<IgnitionTime<300>,Int<16384>>,IgnitionTime<300>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<RetractionTime<0>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<0>,Int<16384>>,RetractionTime<0>>>>,Black>,
  TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,10>,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,15>,SmoothStep<Sum<Int<2000>,IntArg<PREON_SIZE_ARG,2000>>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,20>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,2000>>,Int<-4000>>>,TrBoingX<Mult<Int<8192>,WavLen<EFFECT_PREON>>,3>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<4000>>>,Int<-2000>>>,TrDelayX<WavLen<EFFECT_PREON>>>>,EFFECT_PREON>,
  TransitionEffectL<TrConcat<TrExtend<2000,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<300>>,EFFECT_BATTERY_LEVEL>
>>(),

/* copyright Fett263 Static/Flicker (Crystal Chamber) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#Static/Flicker
OS6.5 v1.0
This Style Contains 144 Unique Combinations
Style Options:
Default (0): AudioFlicker (Single Color (50%))
1: RandomFlicker (Single Color (50%))
2: AudioFlicker (Two Color)
3: RandomFlicker (Two Color)

Off Behavior Options: Default (0): Heart Beat, 1: Kyber Flow Up

--Effects Included--
Ignition Effect: Instant
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect: Instant
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Swing Effect Options: Default (0): Disable, 1: Force Slow
*/
StylePtr<Layers<
  ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,RandomFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<ALT_COLOR_ARG,Rgb<255,50,50>>>,RandomFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RgbArg<ALT_COLOR_ARG,Rgb<255,50,50>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  InOutTrL<TrInstant,TrInstant,ColorSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TransitionLoop<RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,TrConcat<TrBoing<1200,2>,Mix<Int<1285>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,TrFade<1200>>>,Stripes<5000,-2500,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>>>
>>(),
"preset 2" },

{ "Kylo;common", "Kylo/tracks/KyloTheme.wav",
/* copyright Fett263 KyloRen (Primary Blade) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#KyloRen
OS6.5 v1.0
This Style Contains 324 Unique Combinations
Style Options:
Default (0): Kylo Ren Unstable (Film Based)
1: Kylo Ren Unstable (Supreme Leader)

--Effects Included--
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Up = Fast)
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect Options: Default (0): Standard Retraction, 1: Run Up, 2: Dual Mode Wipe In (Up = Fast)
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Lockup Effect: Intensity Lockup V1
LightningBlock Effect: Responsive Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Sound Based), Blast Ripple, Blast Fade (Sound Based)
Clash Effect: Real Clash V1
Swing Effect Options: Default (0): Disable, 1: Force Slow
Battery Level: Blade Location (Green to Red)
*/
StylePtr<Layers<
  ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<3213>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,StripesX<Int<1500>,Scale<SlowNoise<Int<2500>>,Int<-3000>,Int<-5000>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<2570>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  MultiTransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrWaveX<Remap<CenterDistF<EffectPosition<>>,Stripes<1500,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<160>,Int<100>,Int<300>,EffectPosition<>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSparkX<Remap<CenterDistF<EffectPosition<>>,Stripes<2000,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<100>,Int<300>,EffectPosition<>>>,TrConcat<TrInstant,AlphaMixL<Bump<EffectPosition<>,Scale<WavLen<>,Int<9000>,Int<12000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,
  TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,
  TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
  LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,
  ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,
  LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
  LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,OrangeRed,DarkOrange>,Mix<TwistAngle<>,OrangeRed,Orange>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<Red,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrExtend<4000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
  InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<IgnitionTime<300>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,IgnitionTime<300>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<IgnitionTime<300>,Int<16384>>,IgnitionTime<300>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<RetractionTime<0>>,TrWipeX<RetractionTime<0>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<0>,Int<16384>>,RetractionTime<0>>>>,Black>,
  TransitionEffectL<TrConcat<TrExtend<2000,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<300>>,EFFECT_BATTERY_LEVEL>
>>(),

/* copyright Fett263 KyloRen (Crystal Chamber) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#KyloRen
OS6.5 v1.0
This Style Contains 72 Unique Combinations
Style Options:
Default (0): Kylo Ren Unstable (Film Based)
1: Kylo Ren Unstable (Supreme Leader)

Off Behavior Options: Default (0): Heart Beat, 1: Kyber Flow Up

--Effects Included--
Ignition Effect: Instant
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect: Instant
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Swing Effect Options: Default (0): Disable, 1: Force Slow
*/
StylePtr<Layers<
  ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<3213>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,StripesX<Int<1500>,Scale<SlowNoise<Int<2500>>,Int<-3000>,Int<-5000>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<2570>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  InOutTrL<TrInstant,TrInstant,ColorSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TransitionLoop<RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,TrConcat<TrBoing<1200,2>,Mix<Int<1285>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,TrFade<1200>>>,Stripes<5000,-2500,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>>>
>>(), 
"preset 3" },

{ "Maul;common", "Maul/tracks/DuelofFates.wav",
/* copyright Fett263 UnstableBlades (Primary Blade) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#UnstableBlades
OS6.5 v1.0
This Style Contains 162 Unique Combinations
Base Style: Unstable Rage

--Effects Included--
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Up = Fast)
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect Options: Default (0): Standard Retraction, 1: Run Up, 2: Dual Mode Wipe In (Up = Fast)
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Lockup Effect: Intensity Lockup V1
LightningBlock Effect: Responsive Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Sound Based), Blast Ripple, Blast Fade (Sound Based)
Clash Effect: Real Clash V1
Swing Effect Options: Default (0): Disable, 1: Force Slow
Battery Level: Blade Location (Green to Red)
*/
StylePtr<Layers<
  Stripes<3000,-3500,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<3855>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,200>,RandomPerLEDFlicker<Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<3855>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  MultiTransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrWaveX<Remap<CenterDistF<EffectPosition<>>,Stripes<1500,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<160>,Int<100>,Int<300>,EffectPosition<>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSparkX<Remap<CenterDistF<EffectPosition<>>,Stripes<2000,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<100>,Int<300>,EffectPosition<>>>,TrConcat<TrInstant,AlphaMixL<Bump<EffectPosition<>,Scale<WavLen<>,Int<9000>,Int<12000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,
  TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,
  TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
  LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,
  ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,
  LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
  LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,OrangeRed,DarkOrange>,Mix<TwistAngle<>,OrangeRed,Orange>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<Red,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrExtend<4000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
  InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<IgnitionTime<300>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,IgnitionTime<300>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<IgnitionTime<300>,Int<16384>>,IgnitionTime<300>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<RetractionTime<0>>,TrWipeX<RetractionTime<0>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<0>,Int<16384>>,RetractionTime<0>>>>,Black>,
  TransitionEffectL<TrConcat<TrExtend<2000,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<300>>,EFFECT_BATTERY_LEVEL>
>>(),

/* copyright Fett263 UnstableBlades (Crystal Chamber) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#UnstableBlades
OS6.5 v1.0
This Style Contains 36 Unique Combinations
Base Style: Unstable Rage

Off Behavior Options: Default (0): Heart Beat, 1: Kyber Flow Up

--Effects Included--
Ignition Effect: Instant
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect: Instant
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Swing Effect Options: Default (0): Disable, 1: Force Slow
*/
StylePtr<
  Layers<Stripes<3000,-3500,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<3855>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,200>,RandomPerLEDFlicker<Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<3855>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  InOutTrL<TrInstant,TrInstant,ColorSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TransitionLoop<RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,TrConcat<TrBoing<1200,2>,Mix<Int<1285>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,TrFade<1200>>>,Stripes<5000,-2500,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>>>
>>(),
"preset 4" },

{ "Dark;common", "Dark/tracks/mars.wav",
/* copyright Fett263 DarkSaber (Primary Blade) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#DarkSaber
OS6.5 v1.0
This Style Contains 162 Unique Combinations
Base Style: Darksaber

--Effects Included--
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Up = Fast)
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect Options: Default (0): Standard Retraction, 1: Run Up, 2: Dual Mode Wipe In (Up = Fast)
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Lockup Effect: Intensity Lockup V1
LightningBlock Effect: Responsive Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Sound Based), Blast Ripple, Blast Fade (Sound Based)
Clash Effect: Real Clash V1
Swing Effect Options: Default (0): Disable, 1: Force Slow
Battery Level: Blade Location (Green to Red)
*/
StylePtr<Layers<
  AudioFlicker<BrownNoiseFlicker<Mix<SwingSpeed<400>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>>,Stripes<5000,-300,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<25700>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<300>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  MultiTransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrWaveX<Remap<CenterDistF<EffectPosition<>>,Stripes<1500,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<160>,Int<100>,Int<300>,EffectPosition<>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<16384>,Int<6000>>>,TrFade<50>,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSparkX<Remap<CenterDistF<EffectPosition<>>,Stripes<2000,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Int<100>,Int<300>,EffectPosition<>>>,TrConcat<TrInstant,AlphaMixL<Bump<EffectPosition<>,Scale<WavLen<>,Int<9000>,Int<12000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,
  TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,
  TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
  LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,
  ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,
  LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
  LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,OrangeRed,DarkOrange>,Mix<TwistAngle<>,OrangeRed,Orange>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<Red,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrExtend<4000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
  InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<IgnitionTime<300>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,IgnitionTime<300>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<IgnitionTime<300>,Int<16384>>,IgnitionTime<300>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<RetractionTime<0>>,TrWipeX<RetractionTime<0>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<0>,Int<16384>>,RetractionTime<0>>>>,Black>,
  TransitionEffectL<TrConcat<TrExtend<2000,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<300>>,EFFECT_BATTERY_LEVEL>
>>(),

/* copyright Fett263 DarkSaber (Crystal Chamber) OS6 Style
https://fett263.s3.us-east-2.amazonaws.com/fett263-proffieOS6-style-library.html#DarkSaber
OS6.5 v1.0
This Style Contains 36 Unique Combinations
Base Style: Darksaber

Off Behavior Options: Default (0): Heart Beat, 1: Kyber Flow Up

--Effects Included--
Ignition Effect: Instant
PowerUp Effect Options: Default (0): Disable, 1: Unstable Power Up Forward, 2: Power Burst Forward
Retraction Effect: Instant
CoolDown Effect Options: Default (0): Disable, 1: Unstable Cool Down Reverse, 2: Power Burst Reverse
Swing Effect Options: Default (0): Disable, 1: Force Slow
*/
StylePtr<Layers<
  AudioFlicker<BrownNoiseFlicker<Mix<SwingSpeed<400>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>>,Stripes<5000,-300,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<25700>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>>,
  ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,
  TransitionEffectL<TrConcat<TrExtend<30000,TrFade<300>>,Pulsing<RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,3000>,TrFade<300>>,EFFECT_FORCE>>,
  TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<IgnitionTime<0>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,
  TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrExtendX<RetractionTime<0>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
  InOutTrL<TrInstant,TrInstant,ColorSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TransitionLoop<RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,TrConcat<TrBoing<1200,2>,Mix<Int<1285>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,TrFade<1200>>>,Stripes<5000,-2500,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>>>>
>>(),
"preset 5" },

};

BladeConfig blades[] = {
  { 0, WS281XBladePtr<136, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
    WS281XBladePtr<6, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin4> >()
  , CONFIGARRAY(presets) },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif
