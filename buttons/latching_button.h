#ifndef BUTTONS_LATCHING_BUTTON_H
#define BUTTONS_LATCHING_BUTTON_H

#include "debounced_button.h"

// Latching button
template<class BASE = DebouncedButton>
class LatchingButtonTemplate : public Looper,
  public CommandParser,
  public BASE {
public:
  LatchingButtonTemplate(enum BUTTON button, int pin, const char* name)
    : Looper(),
      CommandParser(),
      name_(name),
      button_(button),
      pin_(pin) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_digital.pinMode(pin, INPUT_PULLUP, RISING);
#endif
  }

  const char* name() override { return name_; }

  void Warmup() { Loop(); }

protected:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!BASE::DebouncedRead()) YIELD();
      prop.Event(button_, EVENT_LATCH_ON);
      while (BASE::DebouncedRead()) YIELD();
      prop.Event(button_, EVENT_LATCH_OFF);
    }
    STATE_MACHINE_END();
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      if (current_modifiers & button_) {
        prop.Event(button_, EVENT_LATCH_ON);
      } else {
        prop.Event(button_, EVENT_LATCH_OFF);
      }
      return true;
    }
    return false;
  }

  bool Read() override {
    return digitalRead(pin_) == LOW;
  }

  const char* name_;
  enum BUTTON button_;
  StateMachineState state_machine_;
  uint8_t pin_;
};

using LatchingButton = LatchingButtonTemplate<>;

class InvertedLatchingButton : public LatchingButton {
public:
  InvertedLatchingButton(enum BUTTON button, int pin, const char* name)
    : LatchingButton(button, pin, name) {
#ifdef ENABLE_SNOOZE
    snooze_digital.pinMode(pin, INPUT_PULLUP, FALLING);
#endif
  }

  bool Read() override {
    return digitalRead(pin_) == HIGH;
  }
};

#endif
