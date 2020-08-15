#ifndef BUTTONS_BUTTON_H
#define BUTTONS_BUTTON_H

#include "button_base.h"

// For all setups following the wiring diagram (V2) described on https://fredrik.hubbe.net/lightsaber/v5/
// 
// momentary buttons are expected to be connected to gnd and 1 of the Proffieboard/Teensy input pins
//
// Usage example, put this in the button config section: 
// Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");  
//

class Button : public ButtonBase {
public:

  Button(enum BUTTON button, int pin, const char* name) : ButtonBase(name, button), pin_(pin) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_digital.pinMode(pin, INPUT_PULLUP, RISING);
#endif
  }

protected:
  uint8_t pin_;

  bool Read() override {
    return digitalRead(pin_) == LOW;
  }
};


// For special setups were you want to hook up a momentary button to VCC(3.3V) or Vbat
// and the other end to the inputs of the Proffieboard / Teensy 
// 
// For Proffieboards all inputs can handle Vbat, except for Data3 / blade3pin
//
// This class will add some additional bytes to your code, 
// (224 bytes, for a 4 button setup with 2 button pullup, and 2 buttons pulldown)
// However, if all of your used switches are made ButtonPullDown 
// it will save you a couple of bytes compared to the original Button class 
// (8 bytes for 4 button setup)
//
// Usage example, put this in the button config section: 
// ButtonPullDown PowerButton(BUTTON_POWER, powerButtonPin, "pow");  
//

class ButtonPullDown : public ButtonBase{
public:
  ButtonPullDown(enum BUTTON button, int pin, const char* name) : ButtonBase(name, button), pin_(pin) {
    pinMode(pin, INPUT_PULLDOWN);
#ifdef ENABLE_SNOOZE
    snooze_digital.pinMode(pin, INPUT_PULLDOWN, FALLING);
#endif
  }
protected:
  uint8_t pin_;

  bool Read() override {
	 return digitalRead(pin_) == HIGH;
  }
   
};

#endif


