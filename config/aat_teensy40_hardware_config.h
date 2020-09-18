#define VERSION_MAJOR 3  //teensy compatible
#define VERSION_MINOR 0
#define V2
#define V3
#define USE_I2S
#define USE_TEENSY4
#define NO_BATTERY_MONITOR
#define GYRO_CLASS MPU6050 // external motion sensor

enum SaberPins {

  amplifierPin    =  0,           // Amplifier enable pin
  
  sdCardSelectPin = 10,           // SD card select pin
  spiDataOut      = 11,           // spi out,   SD card, serial flash, spi led
  spiDataIn       = 12,           // spi in,    SD card, serial flash
  spiClock        = 13,           // spi clock, SD card  serial flash, spi led

  powerButtonPin  = 8,            // power button
  auxPin          = 9,            // AUX button
  aux2Pin         = 6,            // AUX2 button, when this is used, Neopixel blade3Pin can not be used
  
  bclkPin         = 21,           // BCLK (digital audio)
  lrclkPin        = 20,           // LRCLK (digital audio) 
  txd0Pin         =  7,           // TXD0 (digital audio)

  i2cDataPin      = 18,           // I2C bus, Used by motion sensors
  i2cClockPin     = 19,           // I2C bus, Used by motion sensors
  motionSensorInterruptPin = 22,  // motion sensor interrupt
  
  batteryLevelPin = 23,           // analogue input, near VCC input 

// potential candidates for neopixels
// OctoWS2811
// For Teensy4.0 any pin can be used
// default pin list is backward compatible for Teensy3.2
// pin 2, 14, 7, 8, 6, 20, 21, 5 FramSYNC 12. (7, 20, 21 already in use for I2S and I2C)
// Neopixel suggested for V4: 14, 15, 6

  bladeIdentifyPin = 14, // ADC read, on main blade
  bladePin         = 14,          // Neopixel main blade, pin 14
  blade2Pin        = 15,          // Neopixel 2nd blade, pin 15, when set to -1 this pin can be used for other purposes
  blade3Pin        =  6,          // Neopixel 3rd blade, pin  6, when set to -1 this pin can be used for other purposes
 
  //Additonal neopixel pins. can be any pin, but most likely aux2pin, 
  //or any of the bladePowerPins, typical bladePowerPin4, 5, 6, 
  //leave -1 if not used
  blade4Pin        =  -1,         // Neopixel 4th blade, 6 hen 
  blade5Pin        =  -1,         // Neopixel 5th blade
  blade6Pin        =  -1,         // Neopixel 6th blade

  //blade power pins are used to shutdown power to blades, or control PWM moldulated leds.
  //thse pins can be used for buttons, or 
  bladePowerPin1   = 1,           // Main blade power control
  bladePowerPin2   = 2,           // R- Optional power control
  bladePowerPin3   = 3,           // G- Optional power control
  bladePowerPin4   = 4,           // B- Optional power control
  bladePowerPin5   = 16,          // W- blade power control   -> also Uart4-RX for BT
  bladePowerPin6   = 17,          // free blade power control -> also Uart4-TX for BT

  // UART
  rxPin = 16,                     // Uart4-RX for BT
  txPin = 17,                     // Uart4-TX for BT


};

#ifndef TEENSYDUINO
#error Please select your Teensy model from Tools->Board
#endif
