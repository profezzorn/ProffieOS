// This is a sample configuration file.
// This saber has:
//   o Proffieboard V1 hardware.
//   o Two buttons
//   o An XP-E2 RGB LED star on LED 4-5-6
//   o A neopixel blade on LED 1-2-3 and neopixel data 1.
// If you have a saber similar to this one, make a copy and use the copy.
// This is also the default configuration file. Pre-programmed boards will
// use this configuration file.

#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 1
#define VOLUME 50
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_WS2811
#define ENABLE_SD
#define ENABLE_SERIAL
#define ENABLE_DEVELOPER_COMMANDS
// #define DISABLE_DIAGNOSTIC_COMMANDS
#define SAVE_COLOR_CHANGE
// #define DISABLE_COLOR_CHANGE
#define SAVE_STATE
// #define KEEP_SAVEFILES_WHEN_PROGRAMMING
// Must be 20 characters or less.
// #define BLE_PASSWORD "password"
// FASTLED is experimental and untested right now
// #define ENABLE_FASTLED
// #define ENABLE_WATCHDOG
// #define ENABLE_SERIALFLASH
// #define ENABLE_SSD1306
// #define NO_BATTERY_MONITOR

// #define ENABLE_DEBUG

   #define IDLE_OFF_TIME 100000  // if defined, pulsing glow will not work,it stops after 100 seconds
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
 { "common;VaderII", "tracks/main.wav",
    StylePtr<Layers<
		Blue,
		BlastL<White>,
		SimpleClashL<White>,
		ResponsiveLockupL<Strobe<White,BrownNoiseFlicker<White,Red,300>,50,1>,TrConcat<TrInstant,White,TrFade<200>>,TrFade<400>,Scale<BladeAngle<0,16000>,Int<10000>,Int<30000>>,Int<10000>,Scale<SwingSpeed<100>,Int<10000>,Int<14000>>>,
		ResponsiveDragL<AudioFlicker<Blue,White>,TrInstant,TrInstant,Int<10000>,Int<16384>>,
		ResponsiveMeltL<Mix<TwistAngle<>,Red,Orange>,TrWipeIn<600>,TrSmoothFade<600>>,
		ResponsiveStabL<Orange,TrWipeIn<50>,TrWipe<50>>,BlastL<White>,
		ResponsiveLightningBlockL<White>,
		InOutTrL<  TrWipeX< Pct<WavLen<EFFECT_IGNITION>,Int<10>> >, TrWipeInX< Pct<WavLen<EFFECT_RETRACTION>, Int<5>> >,Black>,
		TransitionEffectL<TrConcat<TrInstant,AlphaL<White,Bump<Int<0>,Int<6000>>>,TrFadeX<WavLen<EFFECT_RETRACTION>>,AlphaL<Orange,Bump<Int<0>,Int<4000>>>,TrFadeX< Pct<WavLen<EFFECT_PSTOFF>,Int<60>> >,AlphaL<Red,Bump<Int<0>,Int<3000>>>,TrFadeX< Pct<WavLen<EFFECT_PSTOFF>,Int<40>>> >,EFFECT_RETRACTION>,
		TransitionEffectL<TrConcat<TrFadeX< Pct<WavLen<EFFECT_PREON>,Int<125>> >,AlphaL<HumpFlickerL<Orange,10>,Bump<Int<0>,Int<4000>>>,TrFadeX< Pct<WavLen<EFFECT_IGNITION>,Int<50>> >,AlphaL<HumpFlickerL<Orange,15>,Bump<Int<0>,Int<5000>>>,TrFadeX< Pct<WavLen<EFFECT_IGNITION>,Int<25>> >,AlphaL<HumpFlickerL<White,20>,Bump<Int<0>,Int<6000>>>,TrBoingX< Pct<WavLen<EFFECT_IGNITION>,Int<15>>,3>>,EFFECT_PREON>>
		>(),	
	"layers vader"},
  { "common;Kanan", "tracks/main.wav",
	StylePtr<Layers<
	  Blue,
	  BlastL<White>,
	  LockupL<AudioFlicker<Blue,White>>,
	  SimpleClashL<White>,
	  InOutHelperL<InOutFuncX< Pct<WavLen<EFFECT_IGNITION>,Int<50>>,WavLen<EFFECT_RETRACTION> >>
	>>(),
    "layers kanan wavlen pct"},  
  { "common;Kanan", "tracks/main.wav",
	StylePtr<Layers<
	  Blue,
	  BlastL<White>,
	  LockupL<AudioFlicker<Blue,White>>,
	  SimpleClashL<White>,
	  InOutHelperL<InOutFuncX< WavLen<EFFECT_IGNITION>,WavLen<EFFECT_RETRACTION> >>
	>>(),
    "layers kanan wavlen"},  
  { "common;Dark", "tracks/main.wav",
	StylePtr<
	  Layers<
			Blue,
			BlastL<White>,
			LockupL<AudioFlicker<Blue,White>>,
			TransitionEffectL<TrConcat<TrInstant,AlphaL<White,Bump<Int<16384>>>,TrFadeX<WavLen<EFFECT_CLASH>>>,EFFECT_CLASH>,
			InOutTrL<TrDelayX<WavLen<EFFECT_IGNITION>>,TrInstant>, //Make sure Transition Effect has enough time to do its thing
			TransitionEffectL<TrConcat<TrInstant,AlphaL<Blue,SmoothStep< Scale<Mult<Mult<WavRem<EFFECT_IGNITION>,WavRem<EFFECT_IGNITION>>,WavRem<EFFECT_IGNITION>>,Int<32768>,Int<0> >,Int<-1>> >,TrDelayX<WavLen<EFFECT_IGNITION>>>,EFFECT_IGNITION>, //position controller extension
			TransitionEffectL<TrConcat<TrInstant,AlphaL<Blue,SmoothStep< Scale<Mult<Mult<WavPos<EFFECT_RETRACTION>,WavPos<EFFECT_RETRACTION>>,WavPos<EFFECT_RETRACTION>>,Int<32768>,Int<0> >,Int<-1>> >,TrDelayX<WavLen<EFFECT_RETRACTION>>>,EFFECT_RETRACTION> //position controller retraction
		    >
	>(),
    "layers progressive dark with wavlen for clash"},  
};
BladeConfig blades[] = {
 { 0, WS281XBladePtr<123, bladePin, Color8::GRB, PowerPINS<bladePowerPin1> >(), 
    CONFIGARRAY(presets) },
};

#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
//PullDownButton AuxButton(BUTTON_AUX, auxPin, "aux");
//Button Aux2Button(BUTTON_AUX2, aux2Pin, "aux2");
//PullDownButton UpButton(BUTTON_UP, blade4Pin, "aux3");
#endif


//1 button setup
//Sketch uses 201136 bytes (81%) of program storage space. Maximum is 245760 bytes.
//Sketch uses 201144 bytes (81%) of program storage space. Maximum is 245760 bytes.  -> 8 bytes extra, with option pull down...

//4-button setup
//Proffieboard without Pulldown option:  Sketch uses 201328 bytes (81%) of program storage space. Maximum is 245760 bytes.
//Proffieboard with Pulldown mod:        Sketch uses 201384 bytes (81%) of program storage space. Maximum is 245760 bytes.  -> 56 additional bytes when option are left default
//Proffieboard with Pulldown mod:        Sketch uses 201392 bytes (81%) of program storage space. Maximum is 245760 bytes.  -> 64 additional bytes when option are set true/false/true/false


//Teensy 4 button button

//Sketch uses 185136 bytes (70%) of program storage space. Maximum is 262144 bytes.
//Global variables use 29904 bytes (45%) of dynamic memory, leaving 35632 bytes for local variables. Maximum is 65536 bytes.

// true/false/true/false
// Sketch uses 185200 bytes (70%) of program storage space. Maximum is 262144 bytes.
// Global variables use 29904 bytes (45%) of dynamic memory, leaving 35632 bytes for local variables. Maximum is 65536 bytes.


//single button
//no fifference Sketch uses 201256 bytes (81%) of program storage space. Maximum is 245760 bytes.

//2 button setup
// original: Sketch uses 201368 bytes (81%) of program storage space. Maximum is 245760 bytes.
// 2x ButtonPullDown Sketch uses 201368 bytes (81%) of program storage space. Maximum is 245760 bytes. -> exact same
// 1x Button, 1x ButtonPullDown Sketch uses 201480 bytes (81%) of program storage space. Maximum is 245760 bytes. -> 112 bytes extra

//4button setup 
//original, 												  Sketch uses 201456 bytes (81%) of program storage space. Maximum is 245760 bytes.

//Using additional class ButtonPullDown
// 4 buttons original class									  Sketch uses 201456 bytes (81%) of program storage space. Maximum is 245760 bytes. total bytes unchanged
// 4 buttons all ButtonPullDown type						  Sketch uses 201448 bytes (81%) of program storage space. Maximum is 245760 bytes.  -> 8 bytes less (!)
// only 1 button uses ButtonPullDown class			          Sketch uses 201632 bytes (82%) of program storage space. Maximum is 245760 bytes.  -> 176 extra
// 2x Buttons, 2x ButtonPullDown   							  Sketch uses 201680 bytes (82%) of program storage space. Maximum is 245760 bytes. -> 224 bytes extra
// 3 buttons use ButtonPullDown type 						  Sketch uses 201632 bytes (82%) of program storage space. Maximum is 245760 bytes. -> 176 bytes extra
