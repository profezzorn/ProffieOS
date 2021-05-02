#ifndef MOTION_FXOS8700_H
#define MOTION_FXOS8700_H

class FXOS8700 : public I2CDevice, Looper, StateMachine {
public:
  const char* name() override { return "FXOS8700"; }
  enum Registers {
    STATUS              = 0x00, // Real-time data-ready status or FIFO status
    OUT_X_MSB           = 0x01, // 8 MSBs of 14-bit sample / Root pointer to XYZ FIFO
    OUT_X_LSB           = 0x02, // 6 LSBs of 14-bit real-time sample
    OUT_Y_MSB           = 0x03, // 8 MSBs of 14-bit real-time sample
    OUT_Y_LSB           = 0x04, // 6 LSBs of 14-bit real-time sample
    OUT_Z_MSB           = 0x05, // 8 MSBs of 14-bit real-time sample
    OUT_Z_LSB           = 0x06, // 6 LSBs of 14-bit real-time sample
    F_SETUP             = 0x09, // FIFO setup
    TRIG_CFG            = 0x0A, // FIFO event trigger configuration register
    SYSMOD              = 0x0B, // Current system mode
    INT_SOURCE          = 0x0C, // Interrupt status
    WHO_AM_I            = 0x0D, // Device ID
    XYZ_DATA_CFG        = 0x0E, // Acceleration dynamic range and filter enable settings
    P_FILTER_CUTOFF     = 0x0F, // Pulse detection high- pass and low-pass filter enable bits. High-pass filter cutoff frequency selection
    PL_STATUS           = 0x10, // Landscape/Portrait orientation status
    PL_CFG              = 0x11, // Landscape/Portrait configuration
    PL_COUNT            = 0x12, // Landscape/Portrait debounce counter
    PL_BF_ZCOMP         = 0x13, // Back/Front Trip angle threshold
    PL_THS_REG          = 0x14, // Portrait to Landscape Trip Threshold angle and hysteresis settings
    A_FFMT_CFG          = 0x15, // Freefall/Motion function configuration
    A_FFMT_SRC          = 0x16, // Freefall/Motion event source register
    A_FFMT_THS          = 0x17, // Freefall/Motion threshold register
    A_FFMT_COUNT        = 0x18, // Freefall/Motion debounce counter
    TRANSIENT_CFG       = 0x1D, // FIFO setup
    TRANSIENT_SRC       = 0x1E, // Transient event status register
    TRANSIENT_THS       = 0x1F, // Transient event threshold
    TRANSIENT_COUNT     = 0x20, // Transient debounce counter
    PULSE_CFG           = 0x21, // Pulse function configuration
    PULSE_SRC           = 0x22, // Pulse function source register
    PULSE_THSX          = 0x23, // X-axis pulse threshold
    PULSE_THSY          = 0x24, // Y-axis pulse threshold
    PULSE_THSZ          = 0x25, // Z-axis pulse threshold
    PULSE_TMLT          = 0x26, // Time limit for pulse detection
    PULSE_LTCY          = 0x27, // Latency time for second pulse detection
    PULSE_WIND          = 0x28, // Window time for second pulse detection
    ASLP_COUNT          = 0x29, // In activity counter setting for Auto-Sleep
    CTRL_REG1           = 0x2A, // System ODR, accelerometer OSR, operating mode
    CTRL_REG2           = 0x2B, // Self-Test, Reset, accelerometer OSR and Sleep mode settings
    CTRL_REG3           = 0x2C, // Sleep mode interrupt wake enable, interrupt polarity, push-pull/open-drain configuration
    CTRL_REG4           = 0x2D, // Interrupt enable register
    CTRL_REG5           = 0x2E, // Interrupt pin (INT1/INT2) map
    OFF_X               = 0x2F, // X-axis accelerometer offset adjust
    OFF_Y               = 0x30, // Y-axis accelerometer offset adjust
    OFF_Z               = 0x31, // Z-axis accelerometer offset adjust
    M_DR_STATUS         = 0x32, // Magnetic data ready
    M_OUT_X_MSB         = 0x33, // MSB of 16-bit magnetic data for X-axis
    M_OUT_X_LSB         = 0x34, // LSB of 16-bit magnetic data for X-axis
    M_OUT_Y_MSB         = 0x35, // MSB of 16-bit magnetic data for Y-axis
    M_OUT_Y_LSB         = 0x36, // LSB of 16-bit magnetic data for Y-axis
    M_OUT_Z_MSB         = 0x37, // MSB of 16-bit magnetic data for Z-axis
    M_OUT_Z_LSB         = 0x38, // LSB of 16-bit magnetic data for Z-axis
    CMP_X_MSB           = 0x39, // Bits [13:8] of integrated X-axis accerleration data
    CMP_X_LSB           = 0x3A, // Bits [7:0] of integrated X-axis accerleration data
    CMP_Y_MSB           = 0x3B, // Bits [13:8] of integrated Y-axis accerleration data
    CMP_Y_LSB           = 0x3C, // Bits [7:0] of integrated Y-axis accerleration data
    CMP_Z_MSB           = 0x3D, // Bits [13:8] of integrated Z-axis accerleration data
    CMP_Z_LSB           = 0x3E, // Bits [7:0] of integrated Z-axis accerleration data
    M_OFF_X_MSB         = 0x3F, // MSB of magnetometer of X-axis offset
    M_OFF_X_LSB         = 0x40, // LSB of magnetometer of X-axis offset
    M_OFF_Y_MSB         = 0x41, // MSB of magnetometer of Y-axis offset
    M_OFF_Y_LSB         = 0x42, // LSB of magnetometer of Y-axis offset
    M_OFF_Z_MSB         = 0x43, // MSB of magnetometer of Z-axis offset
    M_OFF_Z_LSB         = 0x44, // LSB of magnetometer of Z-axis offset
    MAX_X_MSB           = 0x45, // Magnetometer X-axis maximum value MSB
    MAX_X_LSB           = 0x46, // Magnetometer X-axis maximum value LSB
    MAX_Y_MSB           = 0x47, // Magnetometer Y-axis maximum value MSB
    MAX_Y_LSB           = 0x48, // Magnetometer Y-axis maximum value LSB
    MAX_Z_MSB           = 0x49, // Magnetometer Z-axis maximum value MSB
    MAX_Z_LSB           = 0x4A, // Magnetometer Z-axis maximum value LSB
    MIN_X_MSB           = 0x4B, // Magnetometer X-axis minimum value MSB
    MIN_X_LSB           = 0x4C, // Magnetometer X-axis minimum value LSB
    MIN_Y_MSB           = 0x4D, // Magnetometer Y-axis minimum value MSB
    MIN_Y_LSB           = 0x4E, // Magnetometer Y-axis minimum value LSB
    MIN_Z_MSB           = 0x4F, // Magnetometer Z-axis minimum value MSB
    MIN_Z_LSB           = 0x50, // Magnetometer Z-axis minimum value LSB
    TEMP                = 0x51, // Device temperature, valid range of -128 to 127 C when M_CTRL1[m_hms] > 0b00
    M_THS_CFG           = 0x52, // Magnetic threshold detection function configuration
    M_THS_SRC           = 0x53, // Magnetic threshold event source register
    M_THS_X_MSB         = 0x54, // X-axis magnetic threshold MSB
    M_THS_X_LSB         = 0x55, // X-axis magnetic threshold LSB
    M_THS_Y_MSB         = 0x56, // Y-axis magnetic threshold MSB
    M_THS_Y_LSB         = 0x57, // Y-axis magnetic threshold LSB
    M_THS_Z_MSB         = 0x58, // Z-axis magnetic threshold MSB
    M_THS_Z_LSB         = 0x59, // Z-axis magnetic threshold LSB
    M_THS_COUNT         = 0x5A, // Magnetic threshold debounce counter
    M_CTRL_REG1         = 0x5B, // Control for magnetic sensor functions
    M_CTRL_REG2         = 0x5C, // Control for magnetic sensor functions
    M_CTRL_REG3         = 0x5D, // Control for magnetic sensor functions
    M_INT_SRC           = 0x5E, // Magnetic interrupt source
    A_VECM_CFG          = 0x5F, // Acceleration vector-magnitude configuration register
    A_VECM_THS_MSB      = 0x60, // Acceleration vector-magnitude threshold MSB
    A_VECM_THS_LSB      = 0x61, // Acceleration vector-magnitude threshold LSB
    A_VECM_CNT          = 0x62, // Acceleration vector-magnitude debounce count
    A_VECM_INITX_MSB    = 0x63, // Acceleration vector-magnitude X-axis reference value MSB
    A_VECM_INITX_LSB    = 0x64, // Acceleration vector-magnitude X-axis reference value LSB
    A_VECM_INITY_MSB    = 0x65, // Acceleration vector-magnitude Y-axis reference value MSB
    A_VECM_INITY_LSB    = 0x66, // Acceleration vector-magnitude Y-axis reference value LSB
    A_VECM_INITZ_MSB    = 0x67, // Acceleration vector-magnitude Z-axis reference value MSB
    A_VECM_INITZ_LSB    = 0x68, // Acceleration vector-magnitude Z-axis reference value LSB
    M_VECM_CFG          = 0x69, // Magnetic vector-magnitude configuration register
    M_VECM_THS_MSB      = 0x6A, // Magnetic vector-magnitude threshold MSB
    M_VECM_THS_LSB      = 0x6B, // Magnetic vector-magnitude threshold LSB
    M_VECM_CNT          = 0x6C, // Magnetic vector-magnitude debounce count
    M_VECM_INITX_MSB    = 0x6D, // Magnetic vector-magnitude reference value X-axis MSB
    M_VECM_INITX_LSB    = 0x6E, // Magnetic vector-magnitude reference value X-axis LSB
    M_VECM_INITY_MSB    = 0x6F, // Magnetic vector-magnitude reference value Y-axis MSB
    M_VECM_INITY_LSB    = 0x70, // Magnetic vector-magnitude reference value Y-axis LSB
    M_VECM_INITZ_MSB    = 0x71, // Magnetic vector-magnitude reference value Z-axis MSB
    M_VECM_INITZ_LSB    = 0x72, // Magnetic vector-magnitude reference value Z-axis LSB
    A_FFMT_THS_X_MSB    = 0x73, // X-axis FMT threshold MSB
    A_FFMT_THS_X_LSB    = 0x74, // X-axis FFMT threshold LSB
    A_FFMT_THS_Y_MSB    = 0x75, // Y-axis FFMT threshold MSB
    A_FFMT_THS_Y_LSB    = 0x76, // Y-axis FFMT threshold LSB
    A_FFMT_THS_Z_MSB    = 0x77, // Z-axis FFMT threshold MSB
    A_FFMT_THS_Z_LSB    = 0x78, // Z-axis FFMT threshold LSB
  };

  FXOS8700() : I2CDevice(0x1E) {}

  void Loop() override {
    STATE_MACHINE_BEGIN();

    while (!i2cbus.inited()) YIELD();

    while (1) {
      unsigned char databuffer[6];
      STDOUT.print("Accel setup ... ");

      if (readByte(WHO_AM_I) != 0xC7) {
        STDOUT.print("Failed.");
        SLEEP(1000);
        continue;
      }

      // Standby
      writeByte(CTRL_REG1, 0);
      // configure magnetometer
      // writeByte(M_CTRL_REG1, 0x1F);
      // writeByte(M_CTRL_REG2, 0x20);
      writeByte(M_CTRL_REG2, 0x00); // Disable magnetometer
      // configure accelerometer
      writeByte(XYZ_DATA_CFG, 0x01);  // 4G range
      writeByte(CTRL_REG2, 0x02);  // hires
      // writeByte(CTRL_REG1, 0x15);  // 100Hz A+M 
      writeByte(CTRL_REG1, 0x01);  // 800Hz Accel only

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
	    prop.DoAccel(
              MotionUtil::FromData(databuffer, 4.0 / 32768.0, // 4 g range
				   Vec3::BYTEORDER_MSB, Vec3::ORIENTATION),
	      false);
          }
        }
      }
    }
    STATE_MACHINE_END();
  }
};

#endif
