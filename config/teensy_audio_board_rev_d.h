#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define USE_I2S
#define AUDIO_CONTROL_SGTL5000
#define USE_TEENSY4
#define NO_BATTERY_MONITOR

enum SaberPins {
  // Bottom edge (in pin-out diagram)
  // sdCardSelectPin is actually used, and can be altered if desired. 
  // needs an external 10K pullup to 3.3V, provided on audio shield
  sdCardSelectPin = 10,           // (See File->Example->SD->ReadWrite for
                                  // other possible values.)

  // these values are just a reminder not to use these pins for anything else, 
  // they definitions are not actually used, and overwritten by the AUDIO_CONTROL_SGTL5000
  spiDataOut = 11,                // spi out, serial flash, spi led & sd card
  spiDataIn = 12,                 // spi in, serial flash & sd card
  spiClock = 13,                  // spi clock, flash, spi led & sd card

  // these values are just a reminder not to use these pins for anything else, 
  // they definitions are not actually used, and overwritten by the AUDIO_CONTROL_SGTL5000
  i2cDataPin = 18,                // I2C bus, SGTL5000 control
  i2cClockPin = 19,               // I2C bus, SGTL5000 control

//  auxPin = 15,                  // AUX button, (Volume control input, when potetiometer is mounted to the audioshield)
//  powerButtonPin = 16,          // power button
//  aux2Pin = 17,                 // AUX2 button

// these values are just a reminder not to use these pins for anything else, 
// they definitions are not actually used, and overwritten by the AUDIO_CONTROL_SGTL5000
  mclkPin = 23,                   // MCLK (digital audio)
  bclkPin = 21,                   // BCLK (digital audio)
  lrclkPin = 20,                  // LRCLK (digital audio) 
  txd0Pin = 7,                    // TXD0 (digital audio out)
  rxd0Pin = 8,                    // RXD0 (digital audio in)
    
  bladePowerPin1 = -1,
  bladePowerPin2 = -1,
  bladePowerPin3 = -1,
  bladePin = -1,
  bladeIdentifyPin = -1,

  // The first 8 pins are the same for Teensy 3.x and Teensy 4.x
  trigger1Pin = 0,
  trigger2Pin = 1,
  trigger3Pin = 2,
  trigger4Pin = 3,
  trigger5Pin = 4,
  trigger6Pin = 5,
  trigger7Pin = 16,
  trigger8Pin = 17,
  
  trigger9Pin = 9,
  trigger10Pin = 14,
};

#ifndef TEENSYDUINO
#error Please select your Teensy model from Tools->Board
#endif
