#ifndef BLADES_PWM_PIN_H
#define BLADES_PWM_PIN_H

#include "led_interface.h"

class PWMPin {
public:
  void Init(int pin, LEDInterface*c) {
    pin_ = pin;
    c_ = c;
  }
  void Activate() {
    if (pin_ == -1) return;
    analogWriteFrequency(pin_, 1000);
    analogWrite(pin_, 0);  // make it black
  }
  void set(const Color16& c) {
    if (pin_ == -1) return;
    analogWrite(pin_, c_->PWM(c));
  }
  void set_overdrive(const Color16& c) {
    if (pin_ == -1) return;
    analogWrite(pin_, c_->PWM_overdrive(c));
  }

  int pin_;
  LEDInterface* c_;
};

#endif
