/*
What to expect:
The test script will test capacitors and should report "CAP: OK".
Pullup/pulldown resistors will check BLADE ID reading reporting "ID: N" where N is the resistor value. ??? It should be something close to 35000 ???
The script will cycle through the LED1-6 and data1-4 pads 1:1 in sequence
and light up the yellow diagnostic LEDs. Step 5 and 6 of the sequence should 
also light TX and RX repectively. Data5, SDA, SCL, and 5V should be lit solid.
Data2 should be lit but not full.
Any LED1-6 or data1-4 LED not lighting or staying lit indicates a problem.
The speaker should play spoken prompts to test buttons.

Preparations to use the test script:
1. Edit ProffieOS.ino. Define the CONFIG_FILE to use proffieboard_v2_test_bench_config.h
2. Put the TestRig sound font folder on the SD card. Download from here:
  - https://fredrik.hubbe.net/lightsaber/v5/test_rig.html
3. Make a font folder on the SD card named TestRig with just a boot.wav (and clsh.wav) in it.
4. Set up a multimeter to beep on short (continuity mode).
  - Check BATT+ against all other pads.
  - Attach the leads to one of the power inputs on the breakout board and make sure there's 
    no beep when putting the board on the test bed pins.
5. Start up Arduino
6. Choose the Proffieboard in menu Tools>Port.
7. Open Serial Monitor

For each board:
1. Insert SD card, put board on tester, and plug in USB.
  - boot.wav should be heard.
  - Verify OLED shows ProffieOS startup screen.
2. Watch Serial Monitor and verify that the board connects.
3. Press the RESET button, make sure the "Welcome to ProffieOS" message pops up again.
  - Capacitors test should report "CAP: OK"
  - Blade ID test should report info. Note the USB-only power will skew these value for now.
  - It should say "Waiting for battery power" repeatedly, no beeps.
4. Switch on battery power.
  - Verify OLED displays Battery Meter PLI.
  - Blade ID test should report info again, this time with correct values.
5. Type n in Serial Monitor and hit send (press enter).
  - font.wav should play.
  - Verify OLED says TestRig then shows a test pattern loop.
6. Press the BOOT button.
7. Press Power (button 1).
8. Press AUX (button 2).
9. Press AUX2 (button 3).
10. Verify that all LEDs light up in order.
11. Verify that music is playing.
12. Verify tapping board triggers an EVENT: Clash in Serial Monitor.
13. Switch off battery power then remove USB.
14. Confirm no power before removing board from tester pins.

Optional: 
*Note* The following is for using the test rig and proffieboard_v2_test_bench_config without running the script test.
This can be useful for just testing blade styles and button actions of a prop file etc...
#define INCLUDE_V2_TEST_SCRIPT should be commented out for this type of use.

Onboard 10 pixels can act as data1/LED1 blade showing a blade style fom the 
config with jumpers placed on "LED1-PXL" and D1's PXL pins on the breakout test board.
Additional external blades can be patched to the breakout board jumpers.
Any connected blades (including onboard pixels as data1/LED1 blade)
will flash in sync with the diagnostic sequence, showing the blade style assigned to them.
The default blade colors assigned in order data1->data4 are Red, Green, Blue, Yellow.
Any blades declared in the BladeConfig that are not actually hooked up will be reported as a short on that pin (expected).
Edit proffieboard_v2_test_bench_config.h:
  - Set NUM_BLADES accordingly if more than the onboard pixels are used.
  - Edit pixel counts in the blades[] array accordingly for additional external blades.
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
    talkie.Say(spTHE);
    talkie.Say(spBOOT);
    talkie.Say(spBUTTON);
    stm32l4_gpio_pin_configure(GPIO_PIN_PH3,   (GPIO_PUPD_PULLDOWN | GPIO_OSPEED_HIGH | GPIO_MODE_INPUT));
    SLEEP(300);
    while (!stm32l4_gpio_pin_read(GPIO_PIN_PH3)) YIELD();
    short_checker.SetPinState(powerButtonPin, PinState::Unknown);
    talkie.Say(spPRESS);
    talkie.Say(spPOWER);
    talkie.Say(spBUTTON);
    talkie.Say(spONE);
    SLEEP(50);
    while (digitalRead(powerButtonPin) == HIGH) YIELD();
    SLEEP(50); // Debounce
    while (digitalRead(powerButtonPin) == LOW) YIELD();
    SLEEP(50); // Debounce
    short_checker.SetPinState(powerButtonPin, PinState::InputPullup);
    short_checker.SetPinState(auxPin, PinState::Unknown);
    talkie.Say(spPRESS);
    talkie.Say(spAUXILIARY);
    talkie.Say(spBUTTON);
    talkie.Say(spTWO);
    SLEEP(50);
    while (digitalRead(auxPin) == HIGH) YIELD();
    SLEEP(50);
    while (digitalRead(auxPin) == LOW) YIELD();
    SLEEP(50);
    short_checker.SetPinState(auxPin, PinState::InputPullup);
    short_checker.SetPinState(aux2Pin, PinState::Unknown);
    talkie.Say(spPRESS);
    talkie.Say(spSECONDS);
    talkie.Say(spAUXILIARY);
    talkie.Say(spBUTTON);
    talkie.Say(spTHREE);
    SLEEP(50);
    while (digitalRead(aux2Pin) == HIGH) YIELD();
    SLEEP(50);
    while (digitalRead(aux2Pin) == LOW) YIELD();
    SLEEP(50);
    short_checker.SetPinState(aux2Pin, PinState::InputPullup);
    CommandParser::DoParse("play", "TestRig/tracks/mars.wav");

    talkie.Say(spCHECK);
    talkie.Say(spLIGHTS);
    talkie.Say(spSEQUENCE);
    talkie.Say(spAND);
    talkie.Say(spMUSIC);
    SLEEP(12000);
    talkie.Say(spENTER);
    SLEEP(50);
    talkie.Say(spN);
    talkie.Say(spAND);    
    talkie.Say(spCHECK);
    talkie.Say(spDISPLAY);

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
