#ifndef MOTION_MPU6050_H
#define MOTION_MPU6050_H

class MPU6050 : public I2CDevice, Looper, StateMachine {
public:
  const char* name() override { return "MPU6050"; }
  enum Registers {
    AUX_VDDIO          = 0x01,   // R/W
    SELF_TEST_X        = 0x0D,   // R/W
    SELF_TEST_Y        = 0x0E,   // R/W
    SELF_TEST_Z        = 0x0F,   // R/W
    SELF_TEST_A        = 0x10,   // R/W
    SMPLRT_DIV         = 0x19,   // R/W
    CONFIG             = 0x1A,   // R/W
    GYRO_CONFIG        = 0x1B,   // R/W
    ACCEL_CONFIG       = 0x1C,   // R/W
    FF_THR             = 0x1D,   // R/W
    FF_DUR             = 0x1E,   // R/W
    MOT_THR            = 0x1F,   // R/W
    MOT_DUR            = 0x20,   // R/W
    ZRMOT_THR          = 0x21,   // R/W
    ZRMOT_DUR          = 0x22,   // R/W
    FIFO_EN            = 0x23,   // R/W
    I2C_MST_CTRL       = 0x24,   // R/W
    I2C_SLV0_ADDR      = 0x25,   // R/W
    I2C_SLV0_REG       = 0x26,   // R/W
    I2C_SLV0_CTRL      = 0x27,   // R/W
    I2C_SLV1_ADDR      = 0x28,   // R/W
    I2C_SLV1_REG       = 0x29,   // R/W
    I2C_SLV1_CTRL      = 0x2A,   // R/W
    I2C_SLV2_ADDR      = 0x2B,   // R/W
    I2C_SLV2_REG       = 0x2C,   // R/W
    I2C_SLV2_CTRL      = 0x2D,   // R/W
    I2C_SLV3_ADDR      = 0x2E,   // R/W
    I2C_SLV3_REG       = 0x2F,   // R/W
    I2C_SLV3_CTRL      = 0x30,   // R/W
    I2C_SLV4_ADDR      = 0x31,   // R/W
    I2C_SLV4_REG       = 0x32,   // R/W
    I2C_SLV4_DO        = 0x33,   // R/W
    I2C_SLV4_CTRL      = 0x34,   // R/W
    I2C_SLV4_DI        = 0x35,   // R
    I2C_MST_STATUS     = 0x36,   // R
    INT_PIN_CFG        = 0x37,   // R/W
    INT_ENABLE         = 0x38,   // R/W
    INT_STATUS         = 0x3A,   // R
    ACCEL_XOUT_H       = 0x3B,   // R
    ACCEL_XOUT_L       = 0x3C,   // R
    ACCEL_YOUT_H       = 0x3D,   // R
    ACCEL_YOUT_L       = 0x3E,   // R
    ACCEL_ZOUT_H       = 0x3F,   // R
    ACCEL_ZOUT_L       = 0x40,   // R
    TEMP_OUT_H         = 0x41,   // R
    TEMP_OUT_L         = 0x42,   // R
    GYRO_XOUT_H        = 0x43,   // R
    GYRO_XOUT_L        = 0x44,   // R
    GYRO_YOUT_H        = 0x45,   // R
    GYRO_YOUT_L        = 0x46,   // R
    GYRO_ZOUT_H        = 0x47,   // R
    GYRO_ZOUT_L        = 0x48,   // R
    EXT_SENS_DATA_00   = 0x49,   // R
    EXT_SENS_DATA_01   = 0x4A,   // R
    EXT_SENS_DATA_02   = 0x4B,   // R
    EXT_SENS_DATA_03   = 0x4C,   // R
    EXT_SENS_DATA_04   = 0x4D,   // R
    EXT_SENS_DATA_05   = 0x4E,   // R
    EXT_SENS_DATA_06   = 0x4F,   // R
    EXT_SENS_DATA_07   = 0x50,   // R
    EXT_SENS_DATA_08   = 0x51,   // R
    EXT_SENS_DATA_09   = 0x52,   // R
    EXT_SENS_DATA_10   = 0x53,   // R
    EXT_SENS_DATA_11   = 0x54,   // R
    EXT_SENS_DATA_12   = 0x55,   // R
    EXT_SENS_DATA_13   = 0x56,   // R
    EXT_SENS_DATA_14   = 0x57,   // R
    EXT_SENS_DATA_15   = 0x58,   // R
    EXT_SENS_DATA_16   = 0x59,   // R
    EXT_SENS_DATA_17   = 0x5A,   // R
    EXT_SENS_DATA_18   = 0x5B,   // R
    EXT_SENS_DATA_19   = 0x5C,   // R
    EXT_SENS_DATA_20   = 0x5D,   // R
    EXT_SENS_DATA_21   = 0x5E,   // R
    EXT_SENS_DATA_22   = 0x5F,   // R
    EXT_SENS_DATA_23   = 0x60,   // R
    MOT_DETECT_STATUS  = 0x61,   // R
    I2C_SLV0_DO        = 0x63,   // R/W
    I2C_SLV1_DO        = 0x64,   // R/W
    I2C_SLV2_DO        = 0x65,   // R/W
    I2C_SLV3_DO        = 0x66,   // R/W
    I2C_MST_DELAY_CTRL = 0x67,   // R/W
    SIGNAL_PATH_RESET  = 0x68,   // R/W
    MOT_DETECT_CTRL    = 0x69,   // R/W
    USER_CTRL          = 0x6A,   // R/W
    PWR_MGMT_1         = 0x6B,   // R/W
    PWR_MGMT_2         = 0x6C,   // R/W
    FIFO_COUNTH        = 0x72,   // R/W
    FIFO_COUNTL        = 0x73,   // R/W
    FIFO_R_W           = 0x74,   // R/W
    WHO_AM_I           = 0x75,   // R
  };

  MPU6050() : I2CDevice(0x68) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();

    while (1) {
      unsigned char databuffer[6];

      STDOUT.print("Motion setup ... ");
      writeByte(PWR_MGMT_1, 0); // wake up
      writeByte(CONFIG, 1);     // digital filter config ~180Hz, 1khz rate, 2ms delay
      writeByte(SMPLRT_DIV, 0); // sample rate = 1khz / 1
      writeByte(GYRO_CONFIG, 3 << 3); // 2000 degrees / s
      writeByte(ACCEL_CONFIG, 1 << 3); // 4g range
      writeByte(INT_PIN_CFG, 0x20);  // interrupt on data available
      writeByte(INT_ENABLE, 1); // enable data ready interrupt

      if (readByte(WHO_AM_I) == 0x68) {
        STDOUT.println("done.");
      } else {
        STDOUT.println("failed.");
      }

      while (1) {
        YIELD();
        int status_reg = readByte(INT_STATUS);
        if (status_reg == -1) {
          // motion fail, reboot motion chip.
          STDOUT.println("Motion chip timeout, reboot motion chip!");
          // writeByte(CTRL3_C, 1);
          delay(20);
          break;
        }
        if (status_reg & 0x1) {
	// Do the accel data first to make clashes as fast as possible.
          // accel data available
          if (readBytes(ACCEL_XOUT_H, databuffer, 6) == 6) {
	    prop.DoAccel(
              Vec3::FromData(databuffer, 4.0 / 32768.0,   // 4 g range
			     Vec3::BYTEORDER_MSB, Vec3::ORIENTATION),
	      false);
          }

          // gyroscope data available
          if (readBytes(GYRO_XOUT_H, databuffer, 6) == 6) {
	    prop.DoMotion(
              Vec3::FromData(databuffer, 2000.0 / 32768.0,  // 2000 dps
			     Vec3::BYTEORDER_MSB, Vec3::ORIENTATION),
	      false);
          }

          // Temp data available
          // TODO: Temp Shutdown
          if (readBytes(TEMP_OUT_H, databuffer, 2) == 2) {
            int16_t temp_data = (databuffer[0] << 8) + databuffer[1];
            float temp = temp_data / 340.0 + 36.53;
            if (monitor.ShouldPrint(Monitoring::MonitorTemp)) {
              STDOUT.print("TEMP: ");
              STDOUT.println(temp);
            }
          }
        }
      }
    }
    STATE_MACHINE_END();
  }
};

#endif
