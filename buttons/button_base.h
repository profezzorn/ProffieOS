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
      prop.Event(button_, EVENT_PRESSED);
      if (millis() - push_millis_ < 500) {
        doubleclicked_ = true;
        prop.Event(button_, EVENT_DOUBLE_CLICK_PRESSED);
      } else {
        push_millis_ = millis();
        current_modifiers |= button_;
      }
      while (DebouncedRead()) {
        if (millis() - push_millis_ > 300) {
          if (!doubleclicked_) {
            prop.Event(button_, EVENT_HELD);
          } else {
            prop.Event(button_, EVENT_DOUBLE_CLICK_HELD);
            doubleclickedandheld_ = true;
          }
          while (DebouncedRead()) {
            if (millis() - push_millis_ > 800){
              if (!doubleclicked_) {
                prop.Event(button_, EVENT_HELD_MEDIUM);
              }
              while (DebouncedRead()) {
                if (millis() - push_millis_ > 2000) {
                  if (!doubleclicked_) { 
                    prop.Event(button_, EVENT_HELD_LONG);
                  }
                  while (DebouncedRead()) YIELD();
                }
                YIELD();
              }
            }
            YIELD();
          }
        }
        YIELD();
      }
      while (DebouncedRead()) YIELD();
      prop.Event(button_, EVENT_RELEASED);
      if (doubleclicked_) {
        if (!doubleclickedandheld_) {
          prop.Event(button_, EVENT_DOUBLE_CLICK);
        }
        doubleclicked_ = false;
      }
      doubleclickedandheld_ = false;
      if (current_modifiers & button_) {
        current_modifiers &=~ button_;
        if (millis() - push_millis_ < 500) {
          prop.Event(button_, EVENT_CLICK_SHORT);
        } else if (millis() - push_millis_ > 500 && millis() - push_millis_ < 2500) {
          prop.Event(button_, EVENT_CLICK_LONG);
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
      prop.Event(button_, EVENT_CLICK_SHORT);
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

  bool doubleclicked_ = false;
  bool doubleclickedandheld_ = false;
  const char* name_;
  enum BUTTON button_;
  uint32_t push_millis_;
  StateMachineState state_machine_;
};

#endif
