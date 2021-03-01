#ifndef COMMON_I2CBUS_H
#define COMMON_I2CBUS_H

#define I2C_TIMEOUT_MILLIS 300

class I2CBus : Looper, StateMachine {
public:
  const char* name() override { return "I2CBus"; }
  void Loop() {
    STATE_MACHINE_BEGIN();
    SLEEP(1000);
#ifdef TEENSYDUINO
    // Check that we have pullups.
    while (true) {
      STDOUT.println("I2C TRY");
      pinMode(i2cDataPin, INPUT_PULLDOWN);
      pinMode(i2cClockPin, INPUT_PULLDOWN);
      SLEEP_MICROS(10);
      data_detected = analogRead(i2cDataPin) > 800;
      clock_detected = analogRead(i2cClockPin) > 800;
      
      pinMode(i2cDataPin, INPUT);
      pinMode(i2cClockPin, INPUT);
      if (data_detected && clock_detected) {
	// All good, proceed.
	break;
      }
      if (clock_detected && !data_detected) {
	STDOUT.println("I2C pending data detected, trying to clear...");
	pinMode(i2cClockPin, OUTPUT);
	for (i = 0; i < 100; i++) {
	  SLEEP_MICROS(1);
	  digitalWrite(i2cClockPin, HIGH);
	  SLEEP_MICROS(1);
	  digitalWrite(i2cClockPin, LOW);
	}
	pinMode(i2cClockPin, INPUT);
	SLEEP(100); // Try again soon
      } else {
	if (!clock_detected)
	  Serial.println("No I2C clock pullup detected.");
	SLEEP(1000); // Try again later
      }
    }
    
    STDOUT.println("I2C pullups found, initializing...");
    Wire.begin();
    Wire.setClock(400000);
    Wire.setDefaultTimeout(I2C_TIMEOUT_MILLIS * 1000);
    i2c_detected_ = true;
    Looper::Unlink();
#else
    while (true) {
      STDOUT.println("I2C init..");
      Wire.begin();
      Wire.setClock(400000);
      i2c_detected_ = true;
      while (millis() - last_request_millis_ < 2000) YIELD();
      i2c_detected_ = false;
      STDOUT.println("I2C sleeping..");
      Wire.end();
      while (millis() - last_request_millis_ >= 2000) YIELD();
      }
#endif      
    STATE_MACHINE_END();
  }

  bool used() {
    return millis() - last_request_millis_ < 2000;
  }

  bool inited() {
    last_request_millis_ = millis();
    return i2c_detected_;
  };
  
private:
  uint32_t last_request_millis_ = 0;
  int i;
  bool clock_detected, data_detected;
  bool i2c_detected_ = false;
};

#endif
