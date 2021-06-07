// This config file contains all the things should be the same for all
// V2 proffiboard configurations. In most cases there should be no need
// to change anything in this file.

#define VERSION_MAJOR 6
#define VERSION_MINOR 1
#define V2
#define V3
#define PROFFIEBOARD
#define USE_I2S
#define GYRO_CLASS LSM6DS3H

// Proffieboard pin map
enum SaberPins {
  // I2C
  i2cDataPin = 18,                 // I2C bus, Used by motion sensors  PB7
  i2cClockPin = 19,               // I2C bus, Used by motion sensors   PB8

  // Buttons
  powerButtonPin = 11,            // power button  PB14 + PC5
  auxPin = 13,                    // AUX button    PB13 + PA2
  aux2Pin = 15,                   // AUX2 button   PB15

  // Memory card now uses SDIO
  sdCardSelectPin = 255,

  amplifierPin = 32,              // Amplifier enable pin PH1
  boosterPin = 31,                // Booster enable pin   PH0
  motionSensorInterruptPin = 30,  // motion sensor interrupt PC13

  // No fastled support yet
  spiLedSelect = -1,
  spiLedDataOut =-1,
  spiLedClock = -1,

  // Neopixel pins
  bladePin = 0,                   // blade control, either WS2811 or PWM PA7+PA6
  bladeIdentifyPin = 1,           // blade identify input / FoC
  blade2Pin = 2,                  // PA9 + PB4
  blade3Pin = 4,                  // PA10 + PB5
  blade4Pin = 6,                  // PA4
  blade5Pin = 7,                  // Free1 PB3
  blade6Pin = 8,                  // Free2 PB10
  blade7Pin = 9,                  // Free3 PB11 + PC2
  blade8Pin = 16,                 // also uart PC0
  blade9Pin = 17,                 // also uart PC1

  // Blade power control
  bladePowerPin1 = 20,            // blade power control PA1
  bladePowerPin2 = 21,            // blade power control PB6
  bladePowerPin3 = 22,            // blade power control PC7
  bladePowerPin4 = 23,            // blade power control PB1
  bladePowerPin5 = 24,            // blade power control PC6
  bladePowerPin6 = 25,            // blade power control PB0

  // Free1/2/3 can also do PWM
  // You can drive 20mA LEDs with a resistor from these pins, or
  // hook up an external FET to drive more powerful LEDs
  bladePowerPin7 = 7,
  bladePowerPin8 = 8,
  bladePowerPin9 = 9,

  // Status LED
  statusLEDPin = 26,

  // If there is no neopixels, these can be used as PWM output
  // If neopixels are present, but not on these pins, PWM may still
  // be possible at 800kHz.
  bladePowerPin10 = 2,             // PA9
  bladePowerPin11 = 4,             // PA10

  // Analog pins
  batteryLevelPin = 29,           // battery level input PC4

  chargeDetectPin = 27,           // PA0

  // UART
  rxPin = 16,                      // PC0
  txPin = 17,                      // PC1

  // MiCOM setup
  trigger1Pin = 11,   // power button
  trigger2Pin = 13,   // aux button
  trigger3Pin = 15,   // aux2 button
  trigger4Pin = 7,    // free1
  trigger5Pin = 8,    // free2
  trigger6Pin = 9,    // free3
  trigger7Pin = 2,    // data2
  trigger8Pin = 4,    // data3
};

#if PROFFIEBOARD_VERSION - 0 != 3
#error Please select Proffieboard V3 in Tools->Board
#endif
#if DOSFS_SDCARD - 0 < 2
#error Tools->DOSFS should be set to SDIO (High Speed)
#endif
