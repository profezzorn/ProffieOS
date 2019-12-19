// This is a configuration file that I use to test proffieboard V1.
// This configuration has:
//   o Proffieboard V1 hardware.
//   o Three buttons
//   o A very weird 3-channel LED configuration. (TODO: Change to neopixels.)
// If you have a proffieboard V1 test rig, you may also want to use this configuration file.

#ifdef CONFIG_TOP

// Proffieboard config
#include "proffieboard_config.h"

// Number of simultaneously connected blades.
// (For interchangeable blades, see the blades[] array.)
#define NUM_BLADES 2

// Number of buttons
#define NUM_BUTTONS 3

// Dual power buttons means that clicking AUX will also turn the saber on.
// If not defined, AUX will go to next preset when off.
#define DUAL_POWER_BUTTONS

// Volume, useful range is about 0-2000.
#define VOLUME 100

// If you have two 144 LED/m strips in your blade, connect
// both of them to bladePin and drive them in parallel.
const unsigned int maxLedsPerStrip = 196;

// This defines how sensitive the clash detection is.
#define CLASH_THRESHOLD_G 1.0

// Feature defines, these let you turn off large blocks of code
// used for debugging.
#define ENABLE_AUDIO
#define ENABLE_MOTION
// #define ENABLE_SNOOZE
#define ENABLE_WS2811
#define ENABLE_SERIAL
#define ENABLE_DEVELOPER_COMMANDS
// #define DISABLE_DIAGNOSTIC_COMMANDS
#define SAVE_COLOR_CHANGE
// #define DISABLE_COLOR_CHANGE
#define SAVE_STATE

// Must be 20 characters or less.
// #define BLE_PASSWORD "password"

// FASTLED is experimental and untested right now
// #define ENABLE_FASTLED
// #define ENABLE_WATCHDOG
#define ENABLE_SD
// #define ENABLE_SERIALFLASH
#define ENABLE_SSD1306

// #define ENABLE_DEBUG

#define IDLE_OFF_TIME 100000

#define BLADE_DETECT_PIN aux2Pin

#endif

#ifdef CONFIG_PRESETS

typedef RandomFlicker<Rgb<200,200,200>, Rgb<40,40,40>> OnPulse;
typedef Pulsing<Rgb<128,128,128>, Rgb16<50,50,50>, 3000> OffPulse;

Preset testing_presets[] = {
  { "CRG", "tracks/cantina.wav",
    &style_charging,
    &style_charging, "CHARGING" },
  { "SmthFuzz", "tracks/cantina.wav",
    StylePtr<ByteOrderStyle<Color8::GRB , InOutHelper<EASYBLADE(RED, WHITE), 300, 800> > >(),
//    StyleNormalPtr<StrobeX<BLACK, WHITE, Scale<SwingSpeed<450>, Int<2>, Int<20>>, Int<1>>, Rainbow, 300, 800>(),
//    StyleNormalPtr<ColorChange<TrFade<100>, Hue<8192*0>, Hue<8192*1>, Hue<8192*2>, Hue<8192*3>>, WHITE, 300, 800>(),
//    StylePtr<InOutHelper<LengthFinder<>, 300, 800>>(),
//    StyleRainbowPtr<300, 800>(),
    StylePtr<InOutHelper<EASYBLADE(OnPulse, WHITE), 300, 800, OffPulse> >(),
//    StylePtr<Rgb<0x00, 0xF2, 0xF3>>(),
//    StylePtr<Rgb<0x00, 0xF2, 0xF3>>(),
    "=RainboW++\n++BladE++" },
  { "TeensySF", "tracks/venus.wav",
    // Doesn't always turn off all the way!
    StyleNormalPtr<ColorChange<TrRandom<TrWipeIn<200>, TrWipe<200>>,RED, GREEN, BLUE>, WHITE, 300, 800>(),
//    StylePtr<InOutTr<BLUE, TrColorCycle<10000, 0, 6000>, TrFade<800>, RED> >(),
    // StylePtr<InOutHelper<Stripes<20000,-60,HotPink,HotPink,Magenta,Magenta,Rgb<150,40,200>,Rgb<150,40,200>,Blue,Blue,DeepSkyBlue,DeepSkyBlue,Green,Green,Yellow,Yellow,DarkOrange,DarkOrange,Red,Red>,300,500,Black>>(),

    StylePtr<InOutHelper<EASYBLADE(OnPulse, WHITE), 300, 800, OffPulse> >(),
    "cyan1"},
  { "SmthJedi", "tracks/venus.wav",
    StylePtr<RotateColors<10000, InOutTr<BLUE, TrColorCycle<10000>, TrFade<800>>>>(),
    StylePtr<InOutHelper<EASYBLADE(OnPulse, WHITE), 300, 800, OffPulse> >(),
    "blorg" },

#if 0  
  
//    StyleNormalPtr<Gradient<ColorChange<TrFade<300>,RED, GREEN, BLUE>, ColorChange<TrFade<500>, CYAN, MAGENTA>>, WHITE, 300, 800>(), "cyan"},
  { "charging", "tracks/duel.wav",
    StylePtr< Mix<Sin<Int<20>, Int<8192>, Int<32768>>, Black, Mix<Bump<BatteryLevel, Int<8000>>, Black,
    Mix<BatteryLevel, Red, Red, Orange, Orange, Orange, Blue, Blue, Blue, Blue, Green, Green, Green> >>>(),
    "charging" },

  { "charging", "tracks/duel.wav",
    StylePtr< Mix<Sin<Int<20>, Int<8192>, Int<32768>>, Black, 
    Mix<BatteryLevel, Red, Red, Orange, Orange, Orange, Blue, Blue, Blue, Blue, Green, Green, Green> >>(),
    "charging" },

#if 0
  { "charging", "tracks/duel.wav",
    StylePtr< Mix<Bump<BatteryLevel, Int<8000>>, Black,
    Mix<BatteryLevel, Red, Red, Orange, Orange, Orange, Blue, Blue, Blue, Blue, Green, Green, Green> >>(),
    "charging" },

  { "TeensySF", "tracks/mars.wav", StyleNormalPtr<Stripes<1000, 1000, RED, GREEN, BLUE, CYAN, YELLOW, MAGENTA>, WHITE, 300, 800>(), "BLUE" },
  { "TeensySF", "tracks/mars.wav", StyleNormalPtr<RandomBlink<1000, RED, GREEN>, WHITE, 300, 800>(), "BLUE" },
  { "TeensySF", "tracks/mars.wav", StylePtr<FromFileStyle<>>(), "BLUE" },
  { "TeensySF", "tracks/cantina.wav", StylePtr<InOutHelper<SimpleClash<
    Lockup<Blast<Blue,White>,
           AudioFlicker<Blue,White>,
           AudioFlicker<Green,White>
    >,White>, 300, 800>>(), "DEFAULT" },
  { "TeensySF", "tracks/cantina.wav", StyleNormalPtr<BLUE, WHITE, 300, 800>(), "BLUE" },
  { "TeensySF", "tracks/cantina.wav",
    StylePtr<InOutHelper<LocalizedClash<
      Lockup<
        BlastFadeout<
          OnSpark<
            HumpFlicker<Black,Blue,5>,
          White,
          250>,
        White>,
      AudioFlicker<OnSpark<Blue,White,200>,Black>>,White,40>,300,800,Black>> (),
    "ONOFF" },
  { "TeensySF", "tracks/cantina.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<HumpFlicker<Black,Blue,5>,White,250>,White>,AudioFlicker<OnSpark<Blue,White,200>,Black>>,White,40>,300,800,Black>> (),
    "ONOFF" },
  { "TeensySF", "tracks/cantina.wav", StyleNormalPtr<Sparkle<BLUE>, WHITE, 300, 800>(), "BLUE" },
  { "TeensySF", "tracks/cantina.wav", StyleNormalPtr<RED, WHITE, 300, 800>(), "RED" },
  { "TeensySF", "tracks/cantina.wav", StyleNormalPtr<GREEN, WHITE, 300, 800>(), "GREEN" },

  { "savant", "tracks/mars.wav",
    StyleNormalPtr<HumpFlicker<Yellow, Red ,50>, WHITE, 300, 800, RED>(), "HumpFlicker"},
  
  { "TeensySF", "tracks/cantina.wav",
    StyleNormalPtr<CYAN, WHITE, 3000, 8000>(),
    "ONOFF" },
  { "font02", "tracks/cantina.wav",
    StylePtr<InOutHelper<SimpleClash<Lockup<Blast<OnSpark<HumpFlicker<BLACK, BLUE, 5>, WHITE, 250>, YELLOW>, AudioFlicker<OnSpark<BLUE, WHITE, 200>, YELLOW>>, AudioFlicker<BLUE, YELLOW>, 200>, 400, 800>>(),
    "ONOFF" },
#endif
  { "font02", "tracks/cantina.wav", StyleRainbowPtr<300, 800>(),
    "=RainboW++\n++BladE++" },
  { "graflex4", "tracks/title.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>(), "cyan" },
  { "graflex4", "tracks/cantina.wav", StyleNormalPtr<BLUE, RED, 300, 800>(), "blue" },
  { "caliban", "tracks/duel.wav", StyleFirePtr<RED, YELLOW>(), "red fire" },
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 300, 800>(), "red" },
  { "graflex5", "tracks/title.wav", StyleFirePtr<BLUE, CYAN>(), "blue fire" },
  { "igniter/font4", "tracks/duel.wav", StyleNormalPtr<GREEN, WHITE, 300, 800>(), "green" },
  { "graflex4", "tracks/duel.wav", StyleNormalPtr<WHITE, RED, 300, 800>(), "white" },
  { "graflex4", "tracks/walls.wav", StyleNormalPtr<YELLOW, BLUE, 300, 800>(), "yellow" },
  { "graflex4", "tracks/title.wav", StyleNormalPtr<MAGENTA, WHITE, 300, 800>(), "magenta" },
  { "graflex5", "tracks/cantina.wav", StyleStrobePtr<WHITE, RED, 15, 300, 800>(), "white" },
//  { "graflex5", "tracks/cantina.wav", &style_pov },

  { "charging", "tracks/duel.wav", &style_charging, "charging" },
#endif  
};

BladeConfig blades[] = {
  { 1,
    SubBlade(0, 1, WS2811BladePtr<10, WS2811_800kHz | WS2811_GRB , bladePin, PowerPINS<bladePowerPin1>>()),
    SubBlade(8, 9, NULL),
    CONFIGARRAY(testing_presets), "save" },
//  { 130000, WS2811BladePtr<97, WS2811_800kHz, blade2Pin, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>>(), CONFIGARRAY(testing_presets) }
//  { 130000, WS281XBladePtr<131, blade2Pin, Color8::RGBw>(), CONFIGARRAY(testing_presets) },
#if 0  
  // Testing configuration.
//  { 130000, StringBladePtr<Blue3mmLED>(), CONFIGARRAY(testing_presets) }
//  { 1, WS2811BladePtr<10, WS2811_800kHz | WS2811_GRB , bladePin, PowerPINS<bladePowerPin1>>(), CONFIGARRAY(testing_presets) }
  { 1,
    DimBlade(10.0, SubBladeReverse(0, 143, WS2811BladePtr<144, WS2811_800kHz | WS2811_GRB , bladePin, PowerPINS<bladePowerPin1>>())),
//    DimBlade(10.0, SubBladeReverse(0, 9, WS2811BladePtr<10, WS2811_800kHz | WS2811_GRB , bladePin, PowerPINS<bladePowerPin1>>())),
//    DimBlade(5.0, WS2811BladePtr<10, WS2811_800kHz | WS2811_GRB , bladePin, PowerPINS<bladePowerPin1>>()),
    SimpleBladePtr<CreeXPE2WhiteTemplate<550>, NoLED, NoLED, NoLED, bladePowerPin6, -1, -1, -1>(),
    CONFIGARRAY(testing_presets) },
//  { 130000, WS2811BladePtr<97, WS2811_800kHz, blade2Pin, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>>(), CONFIGARRAY(testing_presets) }
//  { 130000, WS281XBladePtr<131, blade2Pin, Color8::RGBw>(), CONFIGARRAY(testing_presets) },
#endif

#if 0
  { NO_BLADE,
    WS2811BladePtr<10, WS2811_800kHz | WS2811_RGB , bladePin, PowerPINS<bladePowerPin1>>(),
//    WS2811BladePtr<10, WS2811_800kHz | WS2811_GRB , blade4Pin, PowerPINS<bladePowerPin1>>(),
//    DimBlade(5.0, WS2811BladePtr<10, WS2811_800kHz | WS2811_GRB , bladePin, PowerPINS<bladePowerPin1>>()),
    SimpleBladePtr<CreeXPE2WhiteTemplate<550>, NoLED, NoLED, NoLED, bladePowerPin6, -1, -1, -1>(),
    CONFIGARRAY(testing_presets) }
//  { 130000, WS2811BladePtr<97, WS2811_800kHz, blade2Pin, PowerPINS<bladePowerPin1, bladePowerPin2, bladePowerPin3>>(), CONFIGARRAY(testing_presets) }
//  { 130000, WS281XBladePtr<131, blade2Pin, Color8::RGBw>(), CONFIGARRAY(testing_presets) },
#endif
  
};

#endif

#ifdef CONFIG_BUTTONS
// There are currently three available button classes:
// Button (standard momentary button)
// TouchButton (similar to momentary button, but reacts to touch).
// LatchingButton (on/off button, always controls ignition)

//LatchingButton PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
// Button Aux2Button(BUTTON_AUX2, aux2Pin, "aux2");

//TouchButton Aux2Button(BUTTON_AUX2, aux2Pin, 1700, "aux2");

//IRReceiver<blade3Pin> ir_receiver;
//BlasterDecoder blaster_decoder;
//NECDecoder nec_decoder;
//RC6Decoder rc6_decoder;
//PrintDecoder print_decoder;
#endif
