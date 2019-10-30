// If you have multiple sabers that use the same blade connector, and your
// blades use blade ID resistors, you will probably want to share your blades[]
// and preset[] arrays between all your sabers. This file is an example of
// how to do that. My recommendation is to make a copy of this file, edit it
// for your purposes, and then include it from your config file.
// See "graflex_v1_config.h" and "owk_v2_config.h" for examples of how I do this.

#ifdef CONFIG_PRESETS

// CONFIGURABLE
// Each preset line consists of:
// { "font directory", "sound track", Style },
// Where Style is one of:
//   StyleNormalPtr<BaseColor, FlashColor, out millis, in millis>()
//   StyleFirePtr<LowHeatColor, HighHeatColor>()
//   StyleRainBowPtr<out millis, in millis>()
//   StyleStrobePtr<BaseColor, FlashColor, out millis, in millis>()
//   &style_charging
// All colors can be specied as three numbers or using one the handy macros above.
// If you wish to have different presets for different blades, copy this array and
// name it something other than "preset", then use the new name inside the blade
// configuration array below. See "simple_presets" and "charging_presets"
// below for examples.
Preset presets[] = {
  { "hero", "tracks/duel.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>(), "cyan" },
  { "font03", "tracks/title.wav", StyleNormalPtr<CYAN, WHITE, 300, 800>(), "cyan" },
  { "graflex7", "tracks/cantina.wav",
    StylePtr<InOutSparkTip<EasyBlade<BLUE, WHITE>, 300, 800> >(), "blue" },
  { "caliban", "tracks/duel.wav", StyleFirePtr<RED, YELLOW>(), "fire" },
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 300, 800>(), "red" },
  { "font02", "tracks/title.wav", StyleFirePtr<BLUE, CYAN>(), "blue fire" },
  { "igniter/font4", "tracks/duel.wav",
    StylePtr<InOutHelper<EasyBlade<OnSpark<GREEN>, WHITE>, 300, 800> >(), "green" },
  { "font01", "tracks/duel.wav",
    StyleNormalPtr<WHITE, RED, 300, 800, RED>(), "white" },
  { "font01", "tracks/walls.wav",
    StyleNormalPtr<AudioFlicker<YELLOW, WHITE>, BLUE, 300, 800>(), "yellow\nflicker" },
  { "font01", "tracks/title.wav", 
    StylePtr<InOutSparkTip<EasyBlade<MAGENTA, WHITE>, 300, 800> >(), "magenta" },
  { "font02", "tracks/cantina.wav", StyleNormalPtr<
    Gradient<RED, BLUE>, Gradient<CYAN, YELLOW>, 300, 800>(), "gradient" },
  { "font02", "tracks/duel.wav",
    StyleNormalPtr<Pulsing<RED, Rgb<50,0,0>, 5000>, WHITE, 300, 800, RED>(), "pulsing" },
  { "font02", "tracks/cantina.wav", StyleRainbowPtr<300, 800>(), "rainbow" },
  { "font02", "tracks/cantina.wav", StyleStrobePtr<WHITE, Rainbow, 15, 300, 800>(), "strobe" },
#ifdef ENABLE_WS2811
  { "font02", "tracks/cantina.wav", &style_pov, "POV" },
#endif

  { "charging", "tracks/duel.wav", &style_charging, "battery" },
};

Preset red_presets[] = {
  { "igniter/font2", "tracks/vader.wav", StyleNormalPtr<RED, WHITE, 100, 200>()},
  { "caliban", "tracks/duel.wav", StyleNormalPtr<RED, WHITE, 100, 200>()},
  { "font02", "tracks/cantina.wav", StyleStrobePtr<RED, WHITE, 15, 100, 200>()},
};

Preset simple_presets[] = {
  { "font01", "tracks/title.wav", StyleNormalPtr<BLUE, WHITE, 100, 200>() },
  { "font02", "tracks/duel.wav", StyleNormalPtr<BLUE, WHITE, 100, 200>() },
  { "font02", "tracks/cantina.wav", StyleStrobePtr<BLUE, WHITE, 15, 100, 200>() },
};

Preset white_presets[] = {
  { "font01", "tracks/title.wav", StyleNormalPtr<WHITE, WHITE, 100, 200>() },
  { "font02", "tracks/duel.wav", StyleNormalPtr<WHITE, WHITE, 100, 200>() },
  { "font02", "tracks/cantina.wav", StyleStrobePtr<WHITE, WHITE, 15, 100, 200>() },
};

Preset charging_presets[] = {
  { "charging", "", &style_charging },
};

// CONFIGURABLE
// Each line of configuration should be:
//     { blade id resistor ohms, blade, CONFIGARRAY(array of presets) },

BladeConfig blades[] = {
#ifdef ENABLE_WS2811
  // PL9823 blade, 97 LEDs
  {   2600, WS2811BladePtr<97, WS2811_580kHz>(), CONFIGARRAY(presets) },

  // Charging adapter, single PL9823 LED.
  {  15000, WS2811BladePtr<1, WS2811_580kHz>(), CONFIGARRAY(charging_presets) },

  // For testing (makes the charging blade behave like a normal blade.)
  //  {  15000, WS2811BladePtr<1, WS2811_580kHz>(), CONFIGARRAY(presets) },

  // WS2811 string blade 144 LEDs
  {   7800, WS2811BladePtr<144, WS2811_ACTUALLY_800kHz | WS2811_GRB>(), CONFIGARRAY(presets) },
#endif

  // Simple blue string blade.
  {   5200, SimpleBladePtr<Blue3mmLED,Blue3mmLED,Blue3mmLED,NoLED>(), CONFIGARRAY(simple_presets) },

  // Blue-Blue-White LED star
  { 20000, SimpleBladePtr<CreeXPE2White, CreeXPE2Blue, CreeXPE2Blue, NoLED>(), CONFIGARRAY(simple_presets) },

  // Blue LED string (with green flourescent tube)
  { 27000, SimpleBladePtr<Blue8mmLED100, Blue8mmLED100, Blue8mmLED100, NoLED>(), CONFIGARRAY(simple_presets) },

  // Red LED string
//  { 33000, SimpleBladePtr<Red8mmLED100, Red8mmLED100, Red8mmLED100, NoLED>(), CONFIGARRAY(red_presets) },

  // 3 x Cree XL-L LED star
  { 100000, SimpleBladePtr<CreeXPL, CreeXPL, CreeXPL, NoLED>(), CONFIGARRAY(white_presets) },
};

#endif
