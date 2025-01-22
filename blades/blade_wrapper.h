#ifndef BLADES_BLADE_WRAPPER_H
#define BLADES_BLADE_WRAPPER_H

#include "blade_base.h"

class BladeWrapper : public BladeBase {
public:
  int num_leds() const override { return blade_->num_leds(); }
  Color8::Byteorder get_byteorder() const override {
    return blade_->get_byteorder();
  }
  bool is_on() const override {
    int blade_number = GetBladeNumber();
    if (blade_number) return SaberBase::BladeIsOn(blade_number);
    return blade_->is_on();
  }
  bool is_powered() const override { return blade_->is_powered(); }
  void set(int led, Color16 c) override { return blade_->set(led, c); }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(led, c);
  }
  void clear() override { return blade_->clear(); }
  void allow_disable() override { blade_->allow_disable(); }
  void Activate(int blade_number) override { blade_->Activate(blade_number); }
  void Deactivate() override { blade_->Deactivate(); }
  void SetStyle(BladeStyle* style) override { blade_->SetStyle(style); }
  BladeStyle* UnSetStyle() override { return blade_->UnSetStyle(); }
  BladeStyle* current_style() const  override { return blade_->current_style(); }
  int GetBladeNumber() const override { return blade_->GetBladeNumber(); }

  BladeBase* blade_;
};

#endif
