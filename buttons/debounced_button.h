#ifndef BUTTONS_DEBOUNCED_BUTTON_H
#define BUTTONS_DEBOUNCED_BUTTON_H

// Abstract class which de-bounces a potentially noisy
// Read() function by waiting a certain number of ms
// before letting it switch again.

class DebouncedButton {
public:
  void Update() {
    STATE_MACHINE_BEGIN();
    while (true) {
      pushed_ = false;
      last_off_ = millis();
      YIELD();
      if (!Read()) continue;
      YIELD();
      while (Read()) {
	pushed_ = millis() - last_off_ > timeout();
	YIELD();
      }
    }
    STATE_MACHINE_END();
  }
  bool DebouncedRead() {
    Update();
    return pushed_;
  }

protected:
  virtual uint32_t timeout() { return 10; }
  virtual bool Read() = 0;

private:
  uint32_t last_off_;
  bool pushed_ = false;
  StateMachineState state_machine_;
};

#endif
