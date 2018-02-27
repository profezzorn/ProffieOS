// This config file contains all the things should be the same for all
// V1 proffiboard configurations. In most cases there should be no need
// to change anything in this file.

#define VERSION_MAJOR 4
#define VERSION_MINOR 1
#define V2
#define V3
#define USE_I2S
#define GYRO_CLASS LSM6DS3H

// Teensy 3.2 pin map:
// A lot of these can be changed, but be careful, because:
//   o The pins used by the prop shield cannot be easily changed.
//   o Pins that are going to control normal LEDs (not neopixels) need PWM capability,
//     and not all teensy pins can do PWM.
//   o Touch input is not available on all pins.
//   o Sdcard chip select pin depends on what shield you use.
//   o Battery level and blade identification needs analog input, which is not possible
//     on all pins.
//
// See the teensy 3.2 pinout diagram for more info: https://www.pjrc.com/teensy/pinout.html
enum SaberPins {
  // Bottom edge (in pin-out diagram)
  amplifierPin = 1,               // Amplifier enable pin (TeensySaber V2)
  motionSensorInterruptPin = 2,   // motion sensor interrupt (TeensySaber V2)
  bladePowerPin4 = -1,             // Optional power control (TeensySaber V2)
  bladePowerPin5 = -1,             // Optional power control (TeensySaber V2)
  bladePowerPin6 = -1,             // Optional power control (TeensySaber V2)
  bladePowerPin3 = -1,            // blade power control
  spiLedSelect = -1,              // APA102/dotstar chip select
  spiLedDataOut = 7,
  spiLedClock = 8,
  bclkPin = 3,                    // BCLK (digital audio)
  sdCardSelectPin = 10,
  spiDataOut = 11,                // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card

  // Top edge
  spiClock = 13,                  // spi clock, flash, spi led & sd card
  batteryLevelPin = 14,           // battery level input
  auxPin = 15,                    // AUX button
  powerButtonPin = 16,            // power button
  aux2Pin = 17,                   // AUX2 button

  bladePin = 18,                  // blade control, either WS2811 or PWM
  bladeIdentifyPin = 19,          // blade identify input / FoC

  i2cDataPin = 20,                // I2C bus, Used by motion sensors
  i2cClockPin = 21,               // I2C bus, Used by motion sensors

  txd0Pin = 5,                   // TXD0 (digital audio)
  lrclkPin = 2,                  // LRCLK (digital audio)

  bladePowerPin1 = 38,             // blade power control
  bladePowerPin2 = 26,            // blade power control
};
