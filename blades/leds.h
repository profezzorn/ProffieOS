#ifndef BLADES_LEDS_H
#define BLADES_LEDS_H

// These structs below describe the properties of the LED circuit
// so that we know how to drive it properly.
// I've added structs for all the Cree XP-E2 LEDs, you can modify
// or copy-and-add as many other ones as you like.
// See DriveLogic above for more explanations.
template<int milliohms = 550>
struct CreeXPE2WhiteTemplate {
  // These four values come from the datasheet.

  // Maximum (average) amperes for the LED.
  // If you have multiple LEDs hooked up in parallel, multiply
  // the amps from the datasheet by the number of LEDs.
  static constexpr float MaxAmps = 1.0;
  // LED voltage at MaxAmps.
  static constexpr float MaxVolts = 3.15;

  // P2Amps should be less than MaxAmps.
  // If you have multiple LEDs hooked up in parallel, multiply
  // the amps from the datasheet by the number of LEDs.
  static constexpr float P2Amps = 0.7;
  // LED voltage at P2Amps.
  static constexpr float P2Volts = 3.05;

  // Value of the actual resistor you hooked up to the LED.
  static constexpr float R = milliohms / 1000.0;

  // LED color
  static const int Red = 255;
  static const int Green = 255;
  static const int Blue = 255;
};

using CreeXPE2White = CreeXPE2WhiteTemplate<>;

// Note, also good for royal blue.
template<int milliohms = 240>
struct CreeXPE2BlueTemplate {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 3.4;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 3.1;
  static constexpr float R = milliohms / 1000.0;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

using CreeXPE2Blue = CreeXPE2BlueTemplate<>;

template<int milliohms = 0>
struct CreeXPE2GreenTemplate {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 3.7;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 3.2;
  static constexpr float R = milliohms / 1000.0;
  static const int Red = 0;
  static const int Green = 255;
  static const int Blue = 0;
};

using CreeXPE2Green = CreeXPE2GreenTemplate<>;

template<int milliohms = 420>
struct CreeXPE2PCAmberTemplate {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 3.28;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 3.05;
  static constexpr float R = milliohms / 1000.0;
  // TODO(hubbe): Find correct values for PC Amber
  static const int Red = 255;
  static const int Green = 128;
  static const int Blue = 0;
};

using CreeXPE2PCAmber = CreeXPE2PCAmberTemplate<>;

template<int milliohms = 1000>
struct CreeXPE2RedTemplate {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 2.65;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 2.2;
  static constexpr float R = milliohms / 1000.0;
  static const int Red = 255;
  static const int Green = 0;
  static const int Blue = 0;
};

using CreeXPE2Red = CreeXPE2RedTemplate<>;

template<int milliohms = 1000>
struct CreeXPE2RedOrangeTemplate {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 2.65;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 2.2;
  static constexpr float R = milliohms / 1000.0;
  // TODO(hubbe): Find correct values for red-orange
  static const int Red = 255;
  static const int Green = 196;
  static const int Blue = 0;
};

using CreeXPE2RedOrange = CreeXPE2RedOrangeTemplate<>;

template<int milliohms = 1000>
struct CreeXPE2AmberTemplate {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 2.65;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 2.2;
  static constexpr float R = milliohms / 1000.0;
  // TODO(hubbe): Find correct values for Amber
  static const int Red = 255;
  static const int Green = 100;
  static const int Blue = 0;
};

using CreeXPE2Amber = CreeXPE2AmberTemplate<>;

template<int milliohms = 150>
struct CreeXPLTemplate {
  static constexpr float MaxAmps = 3.0;
  static constexpr float MaxVolts = 3.32;
  static constexpr float P2Amps = 1.05;
  static constexpr float P2Volts = 2.95;
  static constexpr float R = milliohms / 1000.0;
  static const int Red = 255;
  static const int Green = 255;
  static const int Blue = 255;
};

using CreeXPL = CreeXPLTemplate<>;

// This is a "superbright 3mm blue led" that I found on ebay.
// I used this to build an LED string with ~150 LEDs.
// Since I don't have a proper datasheet, I measured these values.
struct Blue3mmLED {
  static constexpr float MaxAmps = 0.03 * 150;
  static constexpr float MaxVolts = 3.4;
  static constexpr float P2Amps = 0.016 * 150;
  static constexpr float P2Volts = 3.2;
  static constexpr float R = 0.05; // assumed wire resistance
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

// This blade has 100 x 0.5W red 8mm straw-hat LEDs.
// Since I don't have a proper datasheet, I measured these values.
struct Red8mmLED100 {
  static constexpr float MaxAmps = 11.74;
  static constexpr float MaxVolts = 2.97;
  static constexpr float P2Amps = 7.35;
  static constexpr float P2Volts = 2.5;
  static constexpr float R = 0.001;
  static const int Red = 255;
  static const int Green = 0;
  static const int Blue = 0;
};

// This blade has 100 x 0.5W blue 8mm straw-hat LEDs.
// Since I don't have a proper datasheet, I measured these values.
struct Blue8mmLED100 {
  static constexpr float MaxAmps = 8.9;
  static constexpr float MaxVolts = 3.8;
  static constexpr float P2Amps = 6.05;
  static constexpr float P2Volts = 3.5;
  static constexpr float R = 0.001;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

// For when there is no LED hooked up to a channel.
struct NoLED {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 1000.0;
  static constexpr float P2Amps = 0.0;
  static constexpr float P2Volts = 0.0;
  static constexpr float R = 0.0;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 0;
};

// Basic RED led, no over-voltage protection
struct CH1LED {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 1000.0;
  static constexpr float P2Amps = 0.0;
  static constexpr float P2Volts = 0.0;
  static constexpr float R = 0.0;
  static const int Red = 255;
  static const int Green = 0;
  static const int Blue = 0;
};
struct CH2LED {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 1000.0;
  static constexpr float P2Amps = 0.0;
  static constexpr float P2Volts = 0.0;
  static constexpr float R = 0.0;
  static const int Red = 0;
  static const int Green = 255;
  static const int Blue = 0;
};
struct CH3LED {
  static constexpr float MaxAmps = 1.0;
  static constexpr float MaxVolts = 1000.0;
  static constexpr float P2Amps = 0.0;
  static constexpr float P2Volts = 0.0;
  static constexpr float R = 0.0;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

#endif
