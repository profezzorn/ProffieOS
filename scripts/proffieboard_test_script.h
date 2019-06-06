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
	  beeper.Beep(0.5, 3000.0);
	  SLEEP(1000);
	  beeper.Beep(0.5, 2000.0);
	  SLEEP(1000);
	  beeper.Beep(0.5, 3000.0);
	  SLEEP(1000);
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
    short_checker.SetPinState(powerButtonPin, PinState::InputPullup);
    short_checker.SetPinState(auxPin, PinState::InputPullup);
    short_checker.SetPinState(aux2Pin, PinState::InputPullup);
    if (prop.id() > 22687.0f) {
      STDOUT.println("ID is wrong, should be zero at first!!!");
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
      beeper.Beep(0.5, 3000.0);
      SLEEP(1000);
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
    }

    STDOUT.print(" Waiting for battery power ");
    while (battery_monitor.battery() < 3.5) {
      SLEEP(300);
      STDOUT.print(".");
    }
    STDOUT.println(" battery found.");
    EnableBooster();
    SLEEP(100);
    if (fabsf(prop.id() - 110000.0f) > 22687.0f) {
      STDOUT.println("ID IS WRONG (want 2.5 volts)!!!");
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
    }

    talkie.Say(spPRESS);
    talkie.Say(spBUTTON);
    stm32l4_gpio_pin_configure(GPIO_PIN_PH3,   (GPIO_PUPD_PULLUP | GPIO_OSPEED_HIGH | GPIO_MODE_INPUT));
    SLEEP(50);
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
    talkie.Say(spBUTTON);
    talkie.Say(spONE);
    SLEEP(50);
    while (digitalRead(auxPin) == HIGH) YIELD();
    SLEEP(50);
    while (digitalRead(auxPin) == LOW) YIELD();
    SLEEP(50);
    short_checker.SetPinState(auxPin, PinState::InputPullup);

    short_checker.SetPinState(aux2Pin, PinState::Unknown);
    talkie.Say(spPRESS);
    talkie.Say(spBUTTON);
    talkie.Say(spTWO);
    SLEEP(50);
    while (digitalRead(aux2Pin) == HIGH) YIELD();
    SLEEP(50);
    while (digitalRead(aux2Pin) == LOW) YIELD();
    SLEEP(50);
    short_checker.SetPinState(aux2Pin, PinState::InputPullup);

    CommandParser::DoParse("play", "cantina.wav");
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
      TESTPIN2(bladePin, PinState::Unknown); // hooked up to 10k voltage divider
      TESTPIN2(blade2Pin, PinState::Unknown); // hooked up to neopixels
      TESTPIN(blade3Pin);
      TESTPIN(blade4Pin);
#if VERSION_MAJOR == 4      
      TESTPIN(blade7Pin);
#endif
      TESTPIN(blade6Pin);
      TESTPIN(blade5Pin);
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
        beeper.Beep(0.5, 2000.0);
        SLEEP(1000);
        beeper.Beep(0.5, 2000.0);
        SLEEP(1000);
        beeper.Beep(0.5, 3000.0);
        SLEEP(1000);
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
        beeper.Beep(0.5, 2000.0);
        SLEEP(1000);
        beeper.Beep(0.5, 2000.0);
        SLEEP(1000);
        beeper.Beep(0.5, 3000.0);
        SLEEP(1000);
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

// Preparations:
// 1) In one window, start openocd:
//    cd .arduino15/packages/grumpyoldpizza/hardware/stm32l4/0.0.26
//    ./tools/linux/openocd/bin/openocd -s tools/share/openocd/scripts/ -f  ./variants/STM32L433CC-Butterfly/openocd_scripts/stm32l433cc_butterfly.cfg
// 2) In a second window:
//    tail -f /var/log/kern.log
// 3) Start up arduino
// 4) Hook up multimeter to test board and set it to beep on short
//
// For each board:
// A) Insert SD, put board on tester, plug in USB
// B) Check that openocd connects and that kernel window says STM32 bootloader
// C) press reset, make sure STM32 bootloader pops up again
// D) It should say LOW BATTERY repeatedly, no beeps
// E) Switch to battery power
// F) Press BOOT
// G) Press Power button
// H) Press AUX button
// I) Press AUX2 button
// J) verify that all LEDs light up in order
// H) verify that music is playing
// I) Switch back to short checking and turn off USB

#endif  // not TEENSYDUINO
#endif
