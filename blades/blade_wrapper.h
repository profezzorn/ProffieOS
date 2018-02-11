#ifndef BLADES_BLADE_WRAPPER_H
#define BLADES_BLADE_WRAPPER_H

#include "blade_base.h"

class BladeWrapper : public BladeBase {
public:
  int num_leds() const override { return blade_->num_leds(); }
  bool is_on() const override { return blade_->is_on(); }
  void set(int led, Color16 c) override { return blade_->set(led, c); }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(led, c);
  }
  bool clash() override { return blade_->clash(); }
  void clear() override { return blade_->clear(); }
  void allow_disable() override { blade_->allow_disable(); }
  void Activate() override { blade_->Activate(); }

  BladeBase* blade_;
};

#endif
