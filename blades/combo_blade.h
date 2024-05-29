#ifndef BLADES_COMBO_BLADE_H
#define BLADES_COMBO_BLADE_H

/*
Usage: ComboBlade(blade1, blade2)
This allows multiple blades to be combined into a single linear blade even if
they use different data lines for control.

The resulting blade will have a length of the number of leds in the first and
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

class ComboBladeWrapper;

class ComboBladeStyleWrapper : public BladeStyle {
public:
  // Bladestyle implementation
  virtual void activate() override {
    if (real_style_)
      real_style_->activate();
  }
  virtual void deactivate() override {
    if (real_style_)
      real_style_->deactivate();
  }
  virtual void run(BladeBase* blade) override;

  bool IsHandled(HandledFeature effect) override {
    if (real_style_)
      return real_style_->IsHandled(effect);
    return false;
  }

  bool NoOnOff() override {
    if (real_style_)
      return real_style_->NoOnOff();
    return true;
  }

  void setRealStyle(BladeStyle* style){real_style_ = style;}

  BladeStyle* GetRealStyle(){return real_style_;}

  BladeStyle* UnSetRealStyle(){
    BladeStyle* result = real_style_;
    real_style_ = nullptr;
    return result;
  }

  ComboBladeStyleWrapper(ComboBladeWrapper* comboBlade, BladeBase* primary_blade) :
    comboBlade_(comboBlade),
    primary_blade_(primary_blade),
    real_style_(nullptr){
  }
protected:
  ComboBladeWrapper* comboBlade_;
  BladeBase* primary_blade_;
  BladeStyle* real_style_;
};

class ComboBladeWrapper : public BladeWrapper {
public:
  ComboBladeWrapper(BladeBase* blade1, BladeBase* blade2):blade2_(blade2){
    blade_ = blade1;
    dummy_style_ = new ComboBladeStyleWrapper(this, blade1);
    if (blade1)
      blade1->SetStyle(dummy_style_);
    if (blade2)
      blade2->SetStyle(dummy_style_);
  }

  int num_leds() const override {
    int result = 0;
    if (blade_) {
      result += blade_->num_leds();
    }
    if (blade2_) {
      result += blade2_->num_leds();
    }
    return result;
  }
  void set(int led, Color16 c) override {
    if (led < blade_->num_leds()) {
      if (blade_)
        return blade_->set(led, c);
    } else {
      if (blade2_ && blade_)
        return blade2_->set(led - blade_->num_leds(), c);
    }
  }
  void set_overdrive(int led, Color16 c) override {
    if (led < blade_->num_leds()) {
      if (blade_)
        return blade_->set_overdrive(led, c);
    } else {
      if (blade2_ && blade_)
        return blade2_->set_overdrive(led - blade_->num_leds(), c);
    }
  }

  void allow_disable() override {
    if (blade_)
      blade_->allow_disable();
    if (blade2_)
      blade2_->allow_disable();
  }

  void clear() override {
    if (blade_)
      blade_->clear();
    if (blade2_)
      blade2_->clear();
  }

  void Activate(int blade_number) override {
    if (blade_)
      blade_->Activate(blade_number);
    if (blade2_)
      blade2_->Activate(blade_number);
  }

  void Deactivate() override {
    if (blade_)
      blade_->Deactivate();
    if (blade2_)
      blade2_->Deactivate();
  }

  void SetStyle(BladeStyle* style) override {
    if (dummy_style_)
      dummy_style_->setRealStyle(style);
  }

  BladeStyle* UnSetStyle() override {
    if (dummy_style_)
      return dummy_style_->UnSetRealStyle();
    else
      return nullptr;
  }

  BladeStyle* current_style() const override {
    if (dummy_style_)
      return dummy_style_->GetRealStyle();
    else
      return nullptr;
  }

private:
  BladeBase* blade2_;
  ComboBladeStyleWrapper* dummy_style_;
};

void ComboBladeStyleWrapper::run(BladeBase* blade) {
  if (real_style_ && comboBlade_ && blade && blade == primary_blade_) {
    real_style_->run(static_cast<BladeBase*>(comboBlade_));
  }
}

BladeBase* ComboBlade(BladeBase* blade1, BladeBase* blade2)
{
  ComboBladeWrapper*result =  new ComboBladeWrapper(blade1, blade2);
  return result;
}
#endif
