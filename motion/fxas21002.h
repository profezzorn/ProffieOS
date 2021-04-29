#ifndef MOTION_FXAS21002_H
#define MOTION_FXAS21002_H

class FXAS21002 : public I2CDevice, Looper, StateMachine {
public:
  const char* name() override { return "FXAS21002"; }
  enum Registers {
    I2C_ADDR0         = 0x20, // SA0 = Gnd
    I2C_ADDR1         = 0x21, // SA0 = Vcc
    STATUS            = 0x00, // Alias for DR_STATUS or F_STATUS
    OUT_X_MSB         = 0x01, // MSB of 16 bit X-axis data sample
    OUT_X_LSB         = 0x02, // LSB of 16 bit X-axis data sample
    OUT_Y_MSB         = 0x03 ,// MSB of 16 bit Y-axis data sample
    OUT_Y_LSB         = 0x04, // LSB of 16 bit Y-axis data sample
    OUT_Z_MSB         = 0x05, // MSB of 16 bit Z-axis data sample
    OUT_Z_LSB         = 0x06, // LSB of 16 bit Z-axis data sample
    DR_STATUS         = 0x07, // Data-ready status information
    F_STATUS          = 0x08, // FIFO Status
    F_SETUP           = 0x09, // FIFO setup
    F_EVENT           = 0x0A, // FIFO event
    INT_SRC_FLAG      = 0x0B, // Interrupt event source status flags
    WHO_AM_I          = 0x0C, // Device ID
    CTRL_REG0         = 0x0D, // Full-scale range selection, high-pass filter setting, SPI mode selection
    RT_CFG            = 0x0E, // Rate threshold function configuration
    RT_SRC            = 0x0F, // Rate threshold event flags status register
    RT_THS            = 0x10, // Rate threshold function threshold register
    RT_COUNT          = 0x11, // Rate threshold function debounce counter
    TEMP              = 0x12, // Device temperature in C
    CTRL_REG1         = 0x13, // Operating mode, ODR, self-test and soft reset
    CTRL_REG2         = 0x14, // Interrupt configuration
    CTRL_REG3         = 0x15, // Auto-increment address configuration, external power control, FSR expansion
  };

  FXAS21002() : I2CDevice(0x20) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();
    while (1) {
      unsigned char databuffer[6];

      STDOUT.print("Gyro setup ... ");

      if (readByte(WHO_AM_I) != 0xD7) {
        STDOUT.println("Failed.");
        SLEEP(1000);
        return;
      }

      // Standby
      writeByte(CTRL_REG1, 0);
      // switch to active mode, 800 Hz output rate
      writeByte(CTRL_REG0, 0x00);
      writeByte(CTRL_REG1, 0x02);

      STDOUT.println(" Done");

      while (1) {
        YIELD();
        int status = readByte(STATUS);
        if (status == -1) {
          // motion fail, reboot gyro chip.
          STDOUT.println("Motion chip timeout, trying auto-reboot of motion chip!");
          // writeByte(CTRL3_C, 1);
          delay(20);
          break;
        }
        if (status) {
          // gyroscope data available
          if (readBytes(OUT_X_MSB, databuffer, 6) == 6) {
	    prop.DoMotion(MotionUtil::FromData(databuffer,
					       2000.0 / 32768.0,
					       Vec3::BYTEORDER_MSB,
					       Vec3::ORIENTATION),
	      false);
          }
        }
      }
    }
    STATE_MACHINE_END();
  }
};

#endif
