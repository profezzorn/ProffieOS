#ifndef BUTTONS_BUTTON_BASE_H
#define BUTTONS_BUTTON_BASE_H

#include "debounced_button.h"

// These timeouts can be configured in your config file.
// Also, the prop file can set them, but should use #ifndef same as here
// so that the prop file values can be overriden in the config file.

#ifndef BUTTON_DOUBLE_CLICK_TIMEOUT
#define BUTTON_DOUBLE_CLICK_TIMEOUT 500
#endif

#ifndef BUTTON_SHORT_CLICK_TIMEOUT
#define BUTTON_SHORT_CLICK_TIMEOUT 500
#endif

#ifndef BUTTON_HELD_TIMEOUT
#define BUTTON_HELD_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_MEDIUM_TIMEOUT
#define BUTTON_HELD_MEDIUM_TIMEOUT 800
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif


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
  // We send two events:
  // the event itself, and the event with a count, dependinging on how
  // may double-presses there has been. The count starts at FIRST, that way
  // the first event can be distinguished from the rest.
  bool Send(uint32_t event) {
    if (!prop.Event(button_, (EVENT)(event + (EVENT_SECOND_PRESSED - EVENT_FIRST_PRESSED) * press_count_))) {
      // Only send the second event if the first event didn't trigger a response.
      return prop.Event(button_, (EVENT)event);
    }
    return true;
  }

  // We send the click immediately, but we also hold a "saved" event
  // which is sent a little bit later unless a double-click occurs.
  void SendClick(uint32_t event) {
    if (!Send(event)) {
      // Don't save the event if it's already been handled.
      saved_event_ = event + (EVENT_SAVED_CLICK_SHORT - EVENT_CLICK_SHORT);
    }
  }
  
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!DebouncedRead()) {
	if (saved_event_ &&
	    millis() - push_millis_ > BUTTON_DOUBLE_CLICK_TIMEOUT) {
	  Send(saved_event_);
	  saved_event_ = 0;;
	}
	YIELD();
      }
      saved_event_ = 0;
      if (millis() - push_millis_ < BUTTON_DOUBLE_CLICK_TIMEOUT) {
	if (press_count_ < 4) press_count_++;
      } else {
	press_count_ = 1;
      }
      Send(EVENT_PRESSED);
      push_millis_ = millis();
      current_modifiers |= button_;
      while (DebouncedRead() && (current_modifiers & button_)) {
        if (millis() - push_millis_ > BUTTON_HELD_TIMEOUT) {
          Send(EVENT_HELD);
          while (DebouncedRead() && (current_modifiers & button_)) {
            if (millis() - push_millis_ > BUTTON_HELD_MEDIUM_TIMEOUT){
              Send(EVENT_HELD_MEDIUM);
	      while (DebouncedRead() && (current_modifiers & button_)) {
                if (millis() - push_millis_ > BUTTON_HELD_LONG_TIMEOUT) {
                  Send(EVENT_HELD_LONG);
		  while (DebouncedRead() && (current_modifiers & button_)) YIELD();
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
      Send(EVENT_RELEASED);
      if (current_modifiers & button_) {
        current_modifiers &=~ button_;
        if (millis() - push_millis_ < BUTTON_SHORT_CLICK_TIMEOUT) {
          SendClick(EVENT_CLICK_SHORT);
        } else if (millis() - push_millis_ < 2500) {
	  // Long clicks cannot be "saved", so just emit immediately.
          Send(EVENT_CLICK_LONG);
        }
      } else {
        // someone ate our clicks
        push_millis_ = millis() - 10000; // disable double click
      }
    }
    STATE_MACHINE_END();
  }

  bool Parse(const char* cmd, const char* arg) override {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS    
    if (!strcmp(cmd, name_)) {
      EVENT e = EVENT_CLICK_SHORT;
      int cnt = 0;
      if (arg) {
	if (strlen(arg) > 2) {
	  bool ret;
	  current_modifiers |= button_;
	  const char *tmp = strchr(arg, ' ');
	  if (tmp) {
	    char cmd2[32];
	    memcpy(cmd2, arg, tmp - arg);
	    cmd2[tmp-arg] = 0;
	    ret = CommandParser::DoParse(cmd2, tmp + 1);
	  } else {
	    ret = CommandParser::DoParse(arg, NULL);
	  }
	  current_modifiers &=~ button_;
	  return ret;
	}
	switch (arg[0]) {
	case 'p': e = EVENT_PRESSED; break;
	case 'r': e = EVENT_RELEASED; break;
	case 'h': e = EVENT_HELD; break;
	case 'm': e = EVENT_HELD_MEDIUM; break;
	case 'l': e = EVENT_HELD_LONG; break;
	case 'S': e = EVENT_CLICK_SHORT; break;
	case 's': e = EVENT_SAVED_CLICK_SHORT; break;
	case 'L': e = EVENT_CLICK_LONG; break;
	}
	if (strlen(arg) > 1) {
	  cnt = atoi(arg + 1);
	}
      }
      prop.Event(button_, (EVENT)(e + (EVENT_SECOND_PRESSED - EVENT_FIRST_PRESSED) * cnt));
      return true;
    }
#endif    
    return false;
  }

  const char* name_;
  enum BUTTON button_;
  uint32_t push_millis_;
  int press_count_ = 0;
  StateMachineState state_machine_;
  uint32_t saved_event_ = 0;
};

#endif
