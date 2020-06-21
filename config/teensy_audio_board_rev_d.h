#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define USE_I2S
#define AUDIO_CONTROL_SGTL5000
#define USE_TEENSY4
#define NO_BATTERY_MONITOR

enum SaberPins {
  // Bottom edge (in pin-out diagram)
  sdCardSelectPin = 10,           // (See File->Example->SD->ReadWrite for
                                  // other possible values.)
  bclkPin = 21,                   // BCLK (digital audio)
  spiDataOut = 7,                 // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card
  spiClock = 10,                  // spi clock, flash, spi led & sd card

//  auxPin = 15,                    // AUX button
//  powerButtonPin = 16,            // power button
//  aux2Pin = 17,                   // AUX2 button

  txd0Pin = 7,                    // TXD0 (digital audio)
  lrclkPin = 20,                  // LRCLK (digital audio)

  bladePowerPin1 = -1,
  bladePowerPin2 = -1,
  bladePowerPin3 = -1,
  bladePin = -1,
  bladeIdentifyPin = -1,


  trigger1Pin = 0,
  trigger2Pin = 1,
  trigger3Pin = 2,
  trigger4Pin = 3,
  trigger5Pin = 4,
  trigger6Pin = 5,
  trigger7Pin = 9,
  trigger8Pin = 14,
  trigger9Pin = 16,
  trigger10Pin = 17,
};

#ifndef TEENSYDUINO
#error Please select your Teensy model from Tools->Board
#endif
