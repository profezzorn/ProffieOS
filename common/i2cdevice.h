#ifndef COMMON_I2CDEVICE_H
#define COMMON_I2CDEVICE_H

#include "i2cbus.h"

extern I2CBus i2cbus;

class I2CDevice;
static I2CDevice* current_i2c_device = nullptr;
static I2CDevice* next_i2c_device = nullptr;

class I2CDevice {
public:
  explicit I2CDevice(uint8_t address) : address_(address) {}
  bool I2CLock() {
    if (!i2cbus.inited()) return false;
    if (current_i2c_device) {
      next_i2c_device = this;
      return false;
    }
    // 1-level fairness...
    if (next_i2c_device && next_i2c_device != this) {
      return false;
    }
    next_i2c_device = nullptr;
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

#ifdef PROFFIEBOARD
  // Without this define, the state machine gets mixed up with
  // inherited state machines later. No idea why that happens since
  // it is PRIVATE.
#define state_machine_ temp_state_machine_
    StateMachineState state_machine_;
  private:

// If we fail we just retry over and over again until timeout
#define FAIL() do { state_machine_.reset_state_machine(); return; } while(0)
#define PROGRESS() do { state_machine_.sleep_until_ = millis();	} while(0)

  inline void i2c_write_byte_loop(uint8_t reg, uint8_t data) {
    STATE_MACHINE_BEGIN();
    Wire._tx_data[0] = reg;
    Wire._tx_data[1] = data;
    if (!stm32l4_i2c_transmit(Wire._i2c, address_, Wire._tx_data, 2, 0)) FAIL();

    // Wait for write to finish.
    while (!stm32l4_i2c_done(Wire._i2c)) YIELD();

    // Check status.
    if (stm32l4_i2c_status(Wire._i2c)) FAIL();
    STATE_MACHINE_END();
  }

  inline void i2c_read_bytes_loop(uint8_t reg, uint8_t* data, size_t bytes) {
    STATE_MACHINE_BEGIN();
    // Write the register to the device.
    Wire._tx_data[0] = reg;
    if (!stm32l4_i2c_transmit(Wire._i2c, address_, Wire._tx_data, 1, I2C_CONTROL_RESTART)) FAIL();

    // Wait for write to finish.
    while (!stm32l4_i2c_done(Wire._i2c)) YIELD();

    // Progress was made.
    PROGRESS();

    // Check status.
    if (stm32l4_i2c_status(Wire._i2c)) FAIL();

    // Wire.requestFrom(address_, (uint8_t) bytes);
    if (!stm32l4_i2c_receive(Wire._i2c, address_, Wire._rx_data, bytes, 0)) FAIL();

    // Wait for write to finish.
    while (!stm32l4_i2c_done(Wire._i2c)) YIELD();

    // Check status.
    if (stm32l4_i2c_status(Wire._i2c)) FAIL();

    memcpy(data, Wire._rx_data, bytes);

    STATE_MACHINE_END();
  }
public:
  void Reset() {
    int i;
    for (i = 0; i < 10; i++) {
      if (!stm32l4_i2c_reset(Wire._i2c)) break;
      delay(1);
    }
    if (i == 10) {
      Wire._i2c->state = I2C_STATE_READY;
      stm32l4_i2c_reset(Wire._i2c);
    }
    state_machine_.reset_state_machine();
  }
  bool i2c_read_bytes_async(uint8_t reg, uint8_t* data, size_t bytes) {
    i2c_read_bytes_loop(reg, data, bytes);
    if (state_machine_.next_state_ != -2) return true;
    state_machine_.reset_state_machine();
    return false;
  }
  bool i2c_write_byte_async(uint8_t reg, uint8_t data) {
    i2c_write_byte_loop(reg, data);
    if (state_machine_.next_state_ != -2) return true;
    state_machine_.reset_state_machine();
    return false;
  }
#undef state_machine_

#define I2C_READ_BYTES_ASYNC(reg, data, bytes) do {			\
  temp_state_machine_.sleep_until_ = millis();				\
  while (i2c_read_bytes_async(reg, data, bytes)) {			\
    if (millis() - temp_state_machine_.sleep_until_ > I2C_TIMEOUT_MILLIS) { \
      temp_state_machine_.reset_state_machine();			\
      goto i2c_timeout;							\
    }									\
    YIELD();								\
  }									\
} while(0)

#define I2C_WRITE_BYTE_ASYNC(reg, data) do {				\
  temp_state_machine_.sleep_until_ = millis();				\
  while (i2c_write_byte_async(reg, data)) {				\
    if (millis() - temp_state_machine_.sleep_until_ > I2C_TIMEOUT_MILLIS) { \
      temp_state_machine_.reset_state_machine();			\
      goto i2c_timeout;							\
    }									\
    YIELD();								\
  }									\
} while(0)

#else
  // Do nothing, i2c_t3.h will handle most of the resetting for us.
  void Reset() {}
#define I2C_READ_BYTES_ASYNC(reg, data, bytes) do {			\
  StartReadBytes(reg, bytes);						\
  state_machine_.sleep_until_ = millis();				\
  while (Wire.available() < bytes) {					\
    if (millis() - state_machine_.sleep_until_ > I2C_TIMEOUT_MILLIS) goto i2c_timeout; \
    YIELD();								\
  }									\
  EndReadBytes(data, bytes);						\
} while(0)
#define I2C_WRITE_BYTE_ASYNC(reg, data) writeByte(reg, data)
#endif

  
protected:
  uint8_t address_;
};

#endif
