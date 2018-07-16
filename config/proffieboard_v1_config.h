// This config file contains all the things should be the same for all
// V1 proffiboard configurations. In most cases there should be no need
// to change anything in this file.

#define VERSION_MAJOR 4
#define VERSION_MINOR 1
#define V2
#define V3
#define PROFFIEBOARD
#define USE_I2S
#define GYRO_CLASS LSM6DS3H

// Proffieboard pin map
enum SaberPins {
  // I2S
  bclkPin = 3,                   // BCLK (digital audio)   PB13
  txd0Pin = 31,                  // TXD0 (digital audio)   PA10
  lrclkPin = 2,                  // LRCLK (digital audio)  PB12

  // I2C
  i2cDataPin = 7,                 // I2C bus, Used by motion sensors  PB9
  i2cClockPin = 30,               // I2C bus, Used by motion sensors  PA9

  // Buttons
  powerButtonPin = 21,            // power button  PB6
  auxPin = 23,                    // AUX button    PB5
  aux2Pin = 22,                   // AUX2 button   PB4

  // Memory card
  sdCardSelectPin = 4,            // PB14
  spiDataOut = 11,                // PA7
  spiDataIn = 12,                 // PA6
  spiClock = 15,                  // PA5

  amplifierPin = 26,              // Amplifier enable pin PH1
  boosterPin = 38,                // Booster enable pin   PH0
  motionSensorInterruptPin = 39,   // motion sensor interrupt PC13

  // No fastled support yet
  spiLedSelect = -1,
  spiLedDataOut =-1,
  spiLedClock = -1,

  // Neopixel pins
  bladePin = 16,                  // blade control, either WS2811 or PWM  PA4
  bladeIdentifyPin = 16,          // blade identify input / FoC
  blade2Pin = 1,                  // PB10
  blade3Pin = 0,                  // PB11
  blade4Pin = 17,                 // PA03
  blade5Pin = 24,                 // PB03

  // Blade power control
  bladePowerPin1 = 13,            // blade power control PA1
  bladePowerPin2 = 5,             // blade power control PB15
  bladePowerPin3 = 18,            // blade power control PB0
  bladePowerPin4 = 10,            // blade power control PA8
  bladePowerPin5 = 19,            // blade power control PB1
  bladePowerPin6 = 6,             // blade power control PB6

  // Analog pins
  batteryLevelPin = 14,           // battery level input PA04

  // UART
  rxPin = 8,                      // PA15
  txPin = 9,                      // PA02
};
