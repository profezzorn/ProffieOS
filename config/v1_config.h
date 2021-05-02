// This config file contains all the things should be the same for all
// V1 teensysaber configurations. In most cases there should be no need
// to change anything in this file.

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define GYRO_CLASS FXAS21002
#define ACCEL_CLASS FXOS8700
#define GYRO_MEASUREMENTS_PER_SECOND 800
#define ACCEL_MEASUREMENTS_PER_SECOND 800

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
  freePin1 = 1,                   // FREE
  motionSensorInterruptPin = 2,   // motion sensor interrupt (prop shield)
  freePin3 = 3,                   // FREE
  freePin4 = 4,                   // FREE (make this sdCardSelectPin if you're using a Wiz820+SD shield)
  amplifierPin = 5,               // Amplifier enable pin (prop shield)
  serialFlashSelectPin = 6,       // serial flash chip select (prop shield)
  spiLedSelect = 7,               // APA102/dotstar chip select (prop shield)
  freePin8 = 8,                   // FREE
  spiLedDataOut = 11,
  spiLedClock = 13,
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
  bladePowerPin1 = 21,            // blade power control
  bladePowerPin2 = 22,            // blade power control
  bladePowerPin3 = 23,            // blade power control
};

#ifndef TEENSYDUINO
#error Please select your Teensy model from Tools->Board
#endif
