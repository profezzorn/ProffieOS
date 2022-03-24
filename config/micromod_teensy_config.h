// MicroMod Teensy Configuration file.
// Primarily used for experimentation as micromod boards are too bit to
// fit in a saber.
// Not fully tested.

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define V2
#define V3
#define USE_I2S
#define GYRO_CLASS LSM6DS3H
#define GYRO_MEASUREMENTS_PER_SECOND 1600
#define ACCEL_MEASUREMENTS_PER_SECOND 1600
#define USE_TEENSY4

enum SaberPins {
  // SDIO card adapter. Note that this does NOT work with the
  // feather-compatible micromod carrier.
#define sdCardSelectPin BUILTIN_SDCARD
  amplifierPin = 1,               // Not tested
  motionSensorInterruptPin = -1,   // motion sensor interrupt (not used, we just use qwiic)
  bladePowerPin1 = 6,             // Not tested
  bladePowerPin2 = 10,            // Not tested
  bladePowerPin3 = 23,            // Not tested
  bladePowerPin4 = 3,             // Not tested
  bladePowerPin5 = 4,             // Not tested
  bladePowerPin6 = 5,             // Not tested
  
  spiLedSelect = -1,              //
//  spiLedDataOut = 7,

  spiLedClock = 8,

  // Only used if you have an SPI sd card
  spiDataOut = 11,                // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card
  spiClock = 13,                  // spi clock, flash, spi led & sd card
  
  batteryLevelPin = 14,           // A0: battery level input 

  powerButtonPin = 42,            // G2: power button
  auxPin = 43,                    // G3: AUX button
  aux2Pin = 44,                   // G4: AUX2 button

  // QWIIC
  i2cDataPin = 18,                // I2C bus, Used by motion sensors
  i2cClockPin = 19,               // I2C bus, Used by motion sensors

  // I2S
  txd0Pin = 7,                    // TXD0 (digital audio)
  lrclkPin = 20,                  // LRCLK (digital audio)
  bclkPin = 21,                   // BCLK (digital audio)

  bladePin = 15,                  // A1: blade control, either WS2811 or PWM
  bladeIdentifyPin = 15,          // A1: blade identify input / FoC

};

#ifndef TEENSYDUINO
#error Please select your Teensy model from Tools->Board
#endif
