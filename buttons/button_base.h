#ifndef BUTTONS_BUTTON_BASE_H
#define BUTTONS_BUTTON_BASE_H

#include "debounced_button.h"

// Simple button handler. Keeps track of clicks and lengths of pushes.
class ButtonBase : public Looper,
                   public CommandParser,
                   public DebouncedButton {
public:
  ButtonBase(const char* name, enum BUTTON button)
    : Looper(),
      CommandParser(),
      name_(name),
      button_(button) {
  }

  const char* name() override { return name_; }

protected:
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!DebouncedRead()) YIELD();
      saber.Event(button_, EVENT_PRESSED);
      if (millis() - push_millis_ < 500) {
        saber.Event(button_, EVENT_DOUBLE_CLICK);
      } else {
        push_millis_ = millis();
        current_modifiers |= button_;
      }
      while (DebouncedRead()) {
	if (millis() - push_millis_ > 300) {
	  saber.Event(button_, EVENT_HELD);
	  while (DebouncedRead()) {
	    if (millis() - push_millis_ > 2000) {
	      saber.Event(button_, EVENT_HELD_LONG);
	      while (DebouncedRead()) YIELD();
	    }
	    YIELD();
	  }
	}
	YIELD();
      }
      while (DebouncedRead()) YIELD();
      saber.Event(button_, EVENT_RELEASED);
      if (current_modifiers & button_) {
        current_modifiers &=~ button_;
        if (millis() - push_millis_ < 500) {
          saber.Event(button_, EVENT_CLICK_SHORT);
        } else {
          saber.Event(button_, EVENT_CLICK_LONG);
        }
      } else {
        // someone ate our clicks
        push_millis_ = millis() - 10000; // disable double click
      }
    }
    STATE_MACHINE_END();
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, name_)) {
      saber.Event(button_, EVENT_CLICK_SHORT);
      return true;
    }
    return false;
  }

  void Help() override {
    STDOUT.print(" ");
    STDOUT.print(name_);
    STDOUT.print(" - clicks the ");
    STDOUT.print(name_);
    STDOUT.println(" button");
  }

  const char* name_;
  enum BUTTON button_;
  uint32_t push_millis_;
  StateMachineState state_machine_;
};

#endif
