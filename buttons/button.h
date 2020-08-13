#ifndef BUTTONS_BUTTON_H
#define BUTTONS_BUTTON_H

#include "button_base.h"

class Button : public ButtonBase {
public:
  Button(enum BUTTON button, int pin, const char* name, bool pullup = true) : ButtonBase(name, button), pin_(pin) {
	pullup_ = pullup;  
    if (pullup){
		pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
		snooze_digital.pinMode(pin, INPUT_PULLUP, RISING);
#endif
    }else{
		pinMode(pin, INPUT_PULLDOWN);
#ifdef ENABLE_SNOOZE
		snooze_digital.pinMode(pin, INPUT_PULLDOWN, FALLING);
#endif
	}
  }
protected:
  bool Read() override {
	if (pullup_){  
		return digitalRead(pin_) == LOW;
	}else{
		return digitalRead(pin_) == HIGH;
	}
  }
  uint8_t pin_;
  bool pullup_;
};

#endif
