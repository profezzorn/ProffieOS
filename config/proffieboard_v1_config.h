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
  // I2S (fixed)
  bclkPin = 3,                    // BCLK (digital audio)  PB13
  txd0Pin = 5,                   // TXD0 (digital audio)   PB15
  lrclkPin = 2,                  // LRCLK (digital audio)  PB12

  // I2C (fixed)
  i2cDataPin = 20,                // I2C bus, Used by motion sensors  PB7
  i2cClockPin = 21,               // I2C bus, Used by motion sensors  PB6

  // Buttons (don't change)
  powerButtonPin = 21,            // power button  PB6
  auxPin = 23,                    // AUX button    PB5
  aux2Pin = 22,                   // AUX2 button   PB4

  // Memory card (fixed)
  sdCardSelectPin = 10,           // PA8
  spiDataOut = 11,                // PA7
  spiDataIn = 12,                 // PA6
  spiClock = 13,                  // PA1

  // These may be changed
  amplifierPin = 26,              // Amplifier enable pin
  boosterPin = 38,                // Booster enable pin
  motionSensorInterruptPin = 2,   // motion sensor interrupt (TeensySaber V2)   // TODO: is this correct?

  // Not sure if these should be free pins or what...
  spiLedSelect = -1,              // APA102/dotstar chip select
  spiLedDataOut = 7,
  spiLedClock = 8,

  // Neopixel pins
  bladePin = 16,                  // blade control, either WS2811 or PWM  PA0_TIM2_CH1
  bladeIdentifyPin = 16,          // blade identify input / FoC
  blade2Pin = 17,                 // PA3 TIM2 CH4
  blade3Pin = -1,                 //     TIM2 CH

  // Blade power control
  bladePowerPin1 = 18,            // PB0 TIM1 CH2N
  bladePowerPin2 = -1,            // TIM1
  bladePowerPin3 = 14,            // blade power control    NO TIMER AVAILABLE!!!!!
  bladePowerPin4 = 9,             // blade power control    TIM2 CH3
  bladePowerPin5 = 15,            // blade power control   TIM2 CH1!!!!! PA5!!!
  bladePowerPin6 = 24,            // blade power control   TIM2 CH2

  // Analog pins
  batteryLevelPin = 5,           // battery level input
};
