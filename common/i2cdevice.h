#ifndef COMMON_I2CDEVICE_H
#define COMMON_I2CDEVICE_H

#include "i2cbus.h"

extern I2CBus i2cbus;

class I2CDevice;
static I2CDevice* current_i2c_device = nullptr;

class I2CDevice {
public:
  explicit I2CDevice(uint8_t address) : address_(address) {}
  bool I2CLock() {
    if (current_i2c_device) return false;
    current_i2c_device = this;
    return true;
  }
  void I2CUnlock() {
    if (current_i2c_device == this)
      current_i2c_device = nullptr;
  }
  bool writeByte(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(address_);
    Wire.write(reg);
    Wire.write(data);
    return Wire.endTransmission() == 0;
  }
  void StartReadBytes(uint8_t reg, int bytes) {
    Wire.beginTransmission(address_);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(address_, (uint8_t) bytes);
  }
  int readByte(uint8_t reg) {
    StartReadBytes(reg, 1);
    if (Wire.available() < 1) {
      uint32_t start = millis();
      while (Wire.available() < 1) {
        if (millis() - start > I2C_TIMEOUT_MILLIS) return -1;
      }
    }
    return Wire.read();
  }
  int EndReadBytes(uint8_t* data, int bytes) {
    for (int i = 0; i < bytes; i++) {
      data[i] = Wire.read();
    }           
    return bytes;
  }
  int readBytes(uint8_t reg, uint8_t* data, int bytes) {
    StartReadBytes(reg, bytes);
    if (Wire.available() < bytes) {
      uint32_t start = millis();
      while (Wire.available() < bytes) {
        if (millis() - start > I2C_TIMEOUT_MILLIS) return -1;
      }
    }
    return EndReadBytes(data, bytes);
  }

#define I2C_READ_BYTES_ASYNC(reg, data, bytes) do {			\
  StartReadBytes(reg, bytes);						\
  state_machine_.sleep_until_ = millis();				\
  while (Wire.available() < bytes) {					\
    if (millis() - state_machine_.sleep_until_ > I2C_TIMEOUT_MILLIS) goto i2c_timeout; \
    YIELD();								\
  }									\
  EndReadBytes(data, bytes);						\
} while(0)
  
protected:
  uint8_t address_;
};

#endif
