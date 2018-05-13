// This config file contains all the things should be the same for all
// V1 proffiboard configurations. In most cases there should be no need
// to change anything in this file.

#define VERSION_MAJOR 4
#define VERSION_MINOR 1
#define V2
#define V3
#define USE_I2S
#define GYRO_CLASS LSM6DS3H

// Proffieboard pin map
enum SaberPins {
  // I2S
  bclkPin = 3,                   // BCLK (digital audio)   PB13
  txd0Pin = 5,                   // TXD0 (digital audio)   PB15
  lrclkPin = 2,                  // LRCLK (digital audio)  PB12

  // I2C
  i2cDataPin = 7,                 // I2C bus, Used by motion sensors  PB9
  i2cClockPin = 30,               // I2C bus, Used by motion sensors  PA9

  // Buttons
  powerButtonPin = 21,            // power button  PB6
  auxPin = 23,                    // AUX button    PB5
  aux2Pin = 22,                   // AUX2 button   PB4

  // Memory card
  sdCardSelectPin = 14,           // PA4
  spiDataOut = 11,                // PA7
  spiDataIn = 12,                 // PA6
  spiClock = 13,                  // PA1

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

  // Blade power control
  bladePowerPin1 = 17,            // blade power control PA3
  bladePowerPin2 = 31,            // blade power control PA10
  bladePowerPin3 = 18,            // blade power control PB0
  bladePowerPin4 = 10,            // blade power control PA8
  bladePowerPin5 = 4,             // blade power control PB5
  bladePowerPin6 = 6,             // blade power control PB6

  // Analog pins
  batteryLevelPin = 19,           // battery level input PB1

  FreePin1 = 15,  // PA5
  FreePin2 = 24,  // PB3
};
