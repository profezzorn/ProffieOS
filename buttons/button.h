#ifndef BUTTONS_BUTTON_H
#define BUTTONS_BUTTON_H

#include "button_base.h"

class Button : public ButtonBase {
public:
  Button(enum BUTTON button, int pin, const char* name) : ButtonBase(name, button), pin_(pin) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_digital.pinMode(pin, INPUT_PULLUP, RISING);
#endif
  }
protected:
  bool Read() override {
    return digitalRead(pin_) == LOW;
  }
  uint8_t pin_;
};

#endif
