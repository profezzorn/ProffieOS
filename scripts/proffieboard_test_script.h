/*
Preparations to use the test script:
1. Edit ProffieOS.ino. Define the CONFIG_FILE to use proffieboard_v2_testing_config.h.
2. Use the default ProffieOS SD card.
  - https://fredrik.hubbe.net/lightsaber/sound.html
3. Set up a multimeter to beep on short (continuity mode).
  - Check BATT+ against all other pads.
  - Attach the leads to one of the power inputs on the breakout board and make sure there's 
    no beep when putting the board on the test bed pins.
4. Start up Arduino
5. Choose the Proffieboard in menu Tools>Port.
6. Open Serial Monitor
7. Open up OpenOCD or ST-link utility and run it while testing.
8. On the breakout board, populate all jumpers except LED1->PXL and "PXL"

For each board:
1. Make sure power switch is on the Multimeter, and that the multimeter is still on and in beep mode.
2. Insert SD card, put board on tester, and plug in USB.
  - Upload to the board with the settings above. 
  - boot.wav should be heard when programming is finished.
  - If you hear a beep from the multimeter, stop now and do NOT turn on battery power.
  - If you hear beeps from the board itself, check serial monitor for errors.
3. Press reset button.
  - Same thing should happen as in previous step
4. Check OpenOCD/St-link to make sure that SWDIO pins are working.
5. Check Serial Monitor and verify that the board connects.
  - Also make sure there are no motion related errors.
6. Switch on battery power.
  - You should hear "press boot button"
7. Press the BOOT button.
  - You should hear "press power button"
8. Press the Power button (button closest to speaker connector)
  - You should hear "press A-U-X button"
9. Press the AUX button (button farthest from speaker connector)
  - You should hear "press second A-U-X button"
10. Press the AUX2 button (middle button)
  - You should hear "check lights"
11. Verify that LEDs light up in order.
  - LED1-6 should light up in order.
  - DATA1 should glow dimly
  - DATA2, DATA3, DATA4, RX and TX should light up in order.
  - Data5, SDA, SCL, and 5V should be lit solid.
12. Verify that music is playing.
13. Switch off battery power then remove USB.

Troubleshooting:

If you get beeps in the beginning, you might have left the power on.
If you get beeps when turning on battery power, then the 5v booster might not be working correctly, or PU/PD jumpers are not populated.
If you see CAP FAIL, then the touch capacitor is not working properly.
If motion is not working, you'll see "motion timeout" errors in the serial monitor.

*/

#ifndef SCRIPTS_PROFFIEBOARD_TEST_SCRIPT_H
#define SCRIPTS_PROFFIEBOARD_TEST_SCRIPT_H

#ifndef TEENSYDUINO

enum class PinState {
  Unknown,
  Unconnected,
  InputPullup,
  LowOrInput,
};

class V4ShortChecker : Looper, StateMachine {
public:
  V4ShortChecker() : Looper(), StateMachine() {
    for (size_t i = 0; i < NELEM(pins_); i++) pins_[i] = PinState::Unknown;
  }
  void SetPinState(int pin, PinState state) {
    pins_[pin] = state;
  }
  const char* name() override { return "ShortChecker"; }
  void fail(const char* error) {
    STDOUT.print("Short on pin ");
    STDOUT.print(current_pin);
    STDOUT.print(" ");
    STDOUT.println(error);
    fail_ = true;
  }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while(1) {
      for (current_pin = 0; current_pin < NELEM(pins_); current_pin++) {
	switch (pins_[current_pin]) {
	  case PinState::Unknown: // Do nothing
	    break;
	  case PinState::Unconnected:
	    pinMode(current_pin, INPUT_PULLUP);
	    delayMicroseconds(10);
	    if (digitalRead(current_pin) != HIGH)
	      fail("expected high with pullup");
	    // fall through
	  case PinState::LowOrInput:
	    pinMode(current_pin, INPUT_PULLDOWN);
	    delayMicroseconds(10);
	    if (digitalRead(current_pin) != LOW)
	      fail("expected low with pulldown");
	    break;
	  case PinState::InputPullup:
	    pinMode(current_pin, INPUT_PULLDOWN);
	    delayMicroseconds(10);
	    if (digitalRead(current_pin) != LOW)
	      fail("expected low with pulldown");
	    pinMode(current_pin, INPUT_PULLUP);
	    delayMicroseconds(10);
	    if (digitalRead(current_pin) != HIGH)
	      fail("expected high with pullup");
	    break;
	}
	if (fail_) {
	  beeper.Beep(0.5, 900.0);
	  SLEEP(300);
	  fail_ = false;
	}
	SLEEP_MICROS(100);
      }
    }
    STATE_MACHINE_END();
  }
private:
  bool fail_ = false;
  size_t current_pin;
  PinState pins_[40];
};

V4ShortChecker short_checker;

class V4TestScript : Looper, StateMachine {
public:
  const char* name() override { return "Script"; }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (millis() < 5000) {
      STDOUT.println("Waiting for Serial Monitor to catch up");
      SLEEP(1000);
    }
    short_checker.SetPinState(powerButtonPin, PinState::InputPullup);
    short_checker.SetPinState(auxPin, PinState::InputPullup);
    short_checker.SetPinState(aux2Pin, PinState::InputPullup);
    if (prop.id() > 22687.0f) {
      STDOUT.println("ID is wrong, should be zero at first!!!");
      beeper.Beep(0.5, 500.0);
      SLEEP(300);
      beeper.Beep(0.5, 900.0);
      SLEEP(300);
      beeper.Beep(0.5, 500.0);
      SLEEP(300);
    }
    while (battery_monitor.battery() < 3.5) {
      STDOUT.println(" Waiting for battery power");
      SLEEP(3000);
    }
    STDOUT.println(" Battery found.");
    EnableBooster();
    STDOUT.println(" Booster enabled.");
    dynamic_mixer.set_volume(700);
    STDOUT.println(" Volume set to 700.");
    SLEEP(100);
    if (fabsf(prop.id() - 110000.0f) > 22687.0f) {
      STDOUT.println("ID IS WRONG (want 2.5 volts)!!!");
      beeper.Beep(0.5, 1100.0);
      SLEEP(300);
      beeper.Beep(0.5, 800.0);
      SLEEP(300);
      beeper.Beep(0.5, 500.0);
      SLEEP(300);
    }
    talkie.Say(spPRESS);
    talkie.Say(spBOOT);
    talkie.Say(spBUTTON);
    stm32l4_gpio_pin_configure(GPIO_PIN_PH3,   (GPIO_PUPD_PULLDOWN | GPIO_OSPEED_HIGH | GPIO_MODE_INPUT));
    SLEEP(300);
    while (!stm32l4_gpio_pin_read(GPIO_PIN_PH3)) YIELD();
    short_checker.SetPinState(powerButtonPin, PinState::Unknown);
    talkie.Say(spPRESS);
    talkie.Say(spPOWER);
    talkie.Say(spBUTTON);

    SLEEP(50);
    while (digitalRead(powerButtonPin) == HIGH) YIELD();
    SLEEP(50); // Debounce
    while (digitalRead(powerButtonPin) == LOW) YIELD();
    SLEEP(50); // Debounce
    short_checker.SetPinState(powerButtonPin, PinState::InputPullup);
    short_checker.SetPinState(auxPin, PinState::Unknown);
    talkie.Say(spPRESS);
    talkie.Say(spA);
    talkie.Say(spU);
    talkie.Say(spX);
    talkie.Say(spBUTTON);
    SLEEP(50);
    while (digitalRead(auxPin) == HIGH) YIELD();
    SLEEP(50);
    while (digitalRead(auxPin) == LOW) YIELD();
    SLEEP(50);
    short_checker.SetPinState(auxPin, PinState::InputPullup);
    short_checker.SetPinState(aux2Pin, PinState::Unknown);
    talkie.Say(spPRESS);
    talkie.Say(spSECONDS);
    talkie.Say(spA);
    talkie.Say(spU);
    talkie.Say(spX);
    talkie.Say(spBUTTON);
    SLEEP(50);
    while (digitalRead(aux2Pin) == HIGH) YIELD();
    SLEEP(50);
    while (digitalRead(aux2Pin) == LOW) YIELD();
    SLEEP(50);
    short_checker.SetPinState(aux2Pin, PinState::InputPullup);
    CommandParser::DoParse("play", "tracks/mars.wav");

    talkie.Say(spCHECK);
    talkie.Say(spLIGHTS);

    STATE_MACHINE_END();
  }
};

class Blinker1 : Looper, StateMachine {
public:
  const char* name() override { return "Blinker1"; }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
#define TESTPIN2(X, DEFAULT_STATE) do {			\
    short_checker.SetPinState(X, PinState::Unknown);	\
    pinMode(X, OUTPUT);					\
    digitalWrite(X, HIGH);				\
    SLEEP(200);						\
    digitalWrite(X, LOW);				\
    short_checker.SetPinState(X, DEFAULT_STATE); \
  } while(0);

#define TESTPIN(X) TESTPIN2(X, PinState::LowOrInput)      

      TESTPIN(bladePowerPin1);
      TESTPIN(bladePowerPin2);
      TESTPIN(bladePowerPin3);
      TESTPIN(bladePowerPin4);
      TESTPIN(bladePowerPin5);
      TESTPIN(bladePowerPin6);
    }
    STATE_MACHINE_END();
  }
};

class Blinker2 : Looper, StateMachine {
public:
  const char* name() override { return "Blinker2"; }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
//      TESTPIN2(bladePin, PinState::Unknown); // hooked up to 10k voltage divider
      TESTPIN(blade2Pin);
      TESTPIN(blade3Pin);
      TESTPIN(blade4Pin);
#if VERSION_MAJOR == 4      
      TESTPIN(blade7Pin);
#endif
      TESTPIN(blade5Pin);
      TESTPIN(blade6Pin);
    }
    STATE_MACHINE_END();
  }
};

class CapTest : Looper, StateMachine {
public:
  const char* name() override { return "CapTest"; }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      pinMode(20, INPUT_PULLDOWN);
      SLEEP(100);
      pinMode(20, INPUT_PULLUP);
      last_eq_ = start_ = micros();
      first_ne_ = start_ + 10000;
      while (true) {
	if (digitalRead(20) == LOW) {
	  last_eq_ = micros();
	  if (last_eq_ - start_ > 10000) break;
	} else {
	  first_ne_ = micros();
	  break;
	}
	YIELD();
      }

      if (first_ne_ - start_ < 2000 || last_eq_ - start_ > 6000) {
        STDOUT.print("CAP FAIL LOW-HIGH! ");
        STDOUT.print(last_eq_ - start_);
        STDOUT.print(" ");
        STDOUT.println(first_ne_ - start_);
        loops_ = 10000;
        beeper.Beep(0.5, 500.0);
        SLEEP(300);
        beeper.Beep(0.5, 500.0);
        SLEEP(300);
        beeper.Beep(0.5, 900.0);
        SLEEP(300);
      }

      SLEEP(100);
      pinMode(20, INPUT_PULLDOWN);
      last_eq_ = start_ = micros();
      first_ne_ = start_ + 10000;
      while (true) {
	if (digitalRead(20) == HIGH) {
	  last_eq_ = micros();
	  if (last_eq_ - start_ > 10000) break;
	} else {
	  first_ne_ = micros();
	  break;
	}
	YIELD();
      }

      if (first_ne_ - start_ < 2000 || last_eq_ - start_ > 6000) {
        STDOUT.print("CAP FAIL HIGH-LOW! ");
        STDOUT.print(last_eq_ - start_);
        STDOUT.print(" ");
        STDOUT.println(first_ne_ - start_);
        loops_ = 10000;
        beeper.Beep(0.5, 500.0);
        SLEEP(300);
        beeper.Beep(0.5, 500.0);
        SLEEP(300);
        beeper.Beep(0.5, 900.0);
        SLEEP(300);
      }
      if (loops_++ == 20) {
        STDOUT.println("Cap OK");
      }
    }
    STATE_MACHINE_END();
  }
  void Run() {
    state_machine_.reset_state_machine();
  }
  int loops_ = 0;
  uint32_t start_, last_eq_, first_ne_;
};

#endif  // not TEENSYDUINO
#endif

