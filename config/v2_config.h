// This config file contains all the things should be the same for all
// V2 teensysaber configurations. In most cases there should be no need
// to change anything in this file.

#define VERSION_MAJOR 2
#define VERSION_MINOR 3
#define V2
#define GYRO_CLASS LSM6DS3H
#define GYRO_MEASUREMENTS_PER_SECOND 1600
#define ACCEL_MEASUREMENTS_PER_SECOND 1600

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
  sdCardSelectPin = 0,            // (See File->Example->SD->ReadWrite for
                                  // other possible values.)
  amplifierPin = 1,               // Amplifier enable pin (TeensySaber V2)
  motionSensorInterruptPin = 2,   // motion sensor interrupt (TeensySaber V2)
  bladePowerPin4 = 3,             // Optional power control (TeensySaber V2)
  bladePowerPin5 = 4,             // Optional power control (TeensySaber V2)
  bladePowerPin6 = 5,             // Optional power control (TeensySaber V2)
  freePin6 = 6,
  spiLedSelect = -1,               // APA102/dotstar chip select
  spiLedDataOut = 7,
  spiLedClock = 8,
  freePin9 = 9,                   // FREE
  freePin10 = 10,                 // FREE
  spiDataOut = 11,                // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card

  // Top edge
  spiClock = 13,                  // spi clock, flash, spi led & sd card
  batteryLevelPin = 14,           // battery level input
  auxPin = 15,                    // AUX button
  powerButtonPin = 16,            // power button
  aux2Pin = 17,                   // AUX2 button
  i2cDataPin = 18,                // I2C bus, Used by motion sensors
  i2cClockPin = 19,               // I2C bus, Used by motion sensors
  bladePin = 20,                  // blade control, either WS2811 or PWM
  bladeIdentifyPin = 20,          // blade identify input / FoC
  bladePowerPin3 = 21,            // blade power control
  bladePowerPin2 = 22,            // blade power control
  bladePowerPin1 = 23,            // blade power control
};

#ifndef TEENSYDUINO
#error Please select your Teensy model from Tools->Board
#endif
