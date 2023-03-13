#ifndef BLADES_BLADE_WRAPPER_H
#define BLADES_BLADE_WRAPPER_H

#include "blade_base.h"

class BladeWrapper : public BladeBase {
public:
  int num_leds() const override { return blade_->num_leds(); }
  Color8::Byteorder get_byteorder() const override {
    return blade_->get_byteorder();
  }
  bool is_on() const override { return blade_->is_on(); }
  bool is_powered() const override { return blade_->is_powered(); }
  void set(int led, Color16 c) override { return blade_->set(led, c); }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(led, c);
  }
  size_t GetEffects(BladeEffect** blade_effects) override {
    return blade_->GetEffects(blade_effects);
  }
  void clear() override { return blade_->clear(); }
  void allow_disable() override { blade_->allow_disable(); }
  void Activate() override { blade_->Activate(); }
  void Deactivate() override { blade_->Deactivate(); }
  bool IsPrimary() override { return blade_->IsPrimary(); }
  void SetStyle(BladeStyle* style) override { blade_->SetStyle(style); }
  BladeStyle* UnSetStyle() override { return blade_->UnSetStyle(); }
  BladeStyle* current_style() const  override { return blade_->current_style(); }
  

  BladeBase* blade_;
};

#endif
