#ifndef BLADES_COMBO_BLADE_H
#define BLADES_COMBO_BLADE_H

/*
Usage: ComboBlade(blade1, blade2)
This allows multiple blades to be combined into a single linear blade even if
they use different data lines for control.

The resulting blade will have a length of the number of LEDs in the first and
second blade combined. When the style is running, the second blade's LEDs will
be controlled as though they were in line and after the first blade's LEDs

For a simple example, if you have a saber with 2 crystal LEDs running on different
data lines rather than in line with one another, you might often do the following.

{ 0,
  WS281XBladePtr<128, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
  WS281XBladePtr<1, blade4Pin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
  WS281XBladePtr<1, blade3Pin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
}

The above way allows us to assign either the same style or different styles to
both LEDs

However, if we assign the same style the LEDs will do the same thing since they
are both seen as the first LED in a 1 LED longht blade. To avoid this, we can
use ComboBlade, which will result in the style being run against an intermediate
blade with a length of 2, rainbow and stripe styles to move between the two

{ 0,
  WS281XBladePtr<128, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
  ComboBlade(WS281XBladePtr<1, blade4Pin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
             WS281XBladePtr<1, blade3Pin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3>>()),

In the above example we must reduce the NUM_BLADES by 1 since the ComboBlade itself
is the only thing that counts as an entry in the blade array.

*/

class ComboBladeWrapper : public BladeWrapper, BladeStyle {
public:
  ComboBladeWrapper(BladeBase* blade1, BladeBase* blade2):
    blade1_num_leds_(blade1->num_leds()),
    blade2_num_leds_(blade2->num_leds()),
    blade2_(blade2),
    real_style_(nullptr),
    blade2_cache_(new OverDriveColor[blade2_num_leds_])
  {
    blade_ = blade1;
  }

  // BladeStyle implementation
  virtual void activate() override {
    real_style_->activate();
  }
  virtual void deactivate() override {
    real_style_->deactivate();
  }
  virtual void run(BladeBase* blade) override {
    if (blade == blade_) {
      real_style_->run(this);
    } else {
      // On blade2_ we just apply cached values from when blade_ was run
      for (int i = 0; i < blade2_num_leds_; i++){
        if (blade2_cache_[i].overdrive) {
          blade2_->set_overdrive(i, blade2_cache_[i].c);
        } else {
          blade2_->set(i, blade2_cache_[i].c);
        }
      }
    }
  }

  bool NoOnOff() override {
    return real_style_->NoOnOff();
  }

  virtual bool Charging() { return real_style_->Charging(); }

  bool IsHandled(HandledFeature effect) override {
    return real_style_->IsHandled(effect);
  }

  OverDriveColor getColor(int i) override { return real_style_->getColor(i); }

  int get_max_arg(int arg) override { return real_style_->get_max_arg(arg); }

  // BladeBase implementation
  int num_leds() const override {
    return blade1_num_leds_ + blade2_num_leds_;
  }
  void set(int led, Color16 c) override {
    if (led < blade1_num_leds_) {
      return blade_->set(led, c);
    } else {
      blade2_cache_[led - blade1_num_leds_].c = c;
      blade2_cache_[led - blade1_num_leds_].overdrive = false;
    }
  }

  void set_overdrive(int led, Color16 c) override {
    if (led < blade1_num_leds_) {
      return blade_->set_overdrive(led, c);
    } else {
      blade2_cache_[led - blade1_num_leds_].c = c;
      blade2_cache_[led - blade1_num_leds_].overdrive = true;
    }
  }

  void allow_disable() override {
    blade_->allow_disable();
    blade2_->allow_disable();
  }

  void clear() override {
    blade_->clear();
    blade2_->clear();
  }

  void Activate(int blade_number) override {
    blade_->Activate(blade_number);
    blade2_->Activate(blade_number);
  }

  void Deactivate() override {
    blade_->Deactivate();
    blade2_->Deactivate();
  }

  void SetStyle(BladeStyle* style) override {
    real_style_ = style;
    blade_->SetStyle(this);
    blade2_->SetStyle(this);
  }

  BladeStyle* UnSetStyle() override {
    BladeStyle* ret = real_style_;
    if (ret) {
      ret->deactivate();
    }
    real_style_ = nullptr;
    return ret;
  }

  BladeStyle* current_style() const override {
    return real_style_;
  }

private:
  int blade1_num_leds_;
  int blade2_num_leds_;
  BladeBase* blade2_;
  BladeStyle* real_style_;
  OverDriveColor *blade2_cache_;
};

BladeBase* ComboBlade(BladeBase* blade1, BladeBase* blade2)
{
  return new ComboBladeWrapper(blade1, blade2);
}

#endif
