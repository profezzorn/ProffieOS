#ifndef BUTTONS_FLOATING_BUTTON_H
#define BUTTONS_FLOATING_BUTTON_H

// Returns true when the specified pin is not floating.
template<int PIN>
class FloatingButtonBase {
public:
  // Requires 10 votes of float/non float agreement
  // before changing state.
  void Vote(int floating) {
    if (floating != floating_) {
      if (++votes_for_change_ <= 10) return;
      floating_ = floating;
    }
    votes_for_change_ = 0;
  }

  void Update() {
    STATE_MACHINE_BEGIN();
    while (true) {
      YIELD();
      pinMode(PIN, INPUT_PULLUP);
      SLEEP(1);
      if (!digitalRead(PIN)) {
	Vote(false);
	continue;
      }

      pinMode(PIN, INPUT_PULLDOWN);
      SLEEP(1);
      if (digitalRead(PIN)) {
	Vote(false);
	continue;
      }
      Vote(true);
    }
    STATE_MACHINE_END();
  }

  // Returns true if NOT floating.
  bool DebouncedRead() {
    Update();
    return !floating_;
  }
  virtual bool Read() = 0;
private:
  bool floating_ = true;
  uint8_t votes_for_change_ = 0;
  StateMachineState state_machine_;
};

#endif
