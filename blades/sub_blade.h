#ifndef BLADES_SUB_BLADE_H
#define BLADES_SUB_BLADE_H

class SubBladeWrapper : public BladeWrapper, BladeStyle {
public:
  int num_leds() const override { return num_leds_; }
  void set(int led, Color16 c) override {
    return blade_->set(led + offset_, c);
  }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(led + offset_, c);
  }
  void allow_disable() override { allow_disable_ = true; }
  void Activate() override {
    if (!offset_) BladeWrapper::Activate();
  }
  void clear() override {
    if (!offset_) BladeWrapper::clear();
  }
  void SetStyle(BladeStyle* style) {
    BladeWrapper::SetStyle(style);
    if (!offset_) blade_->SetStyle(this);
  }
  
  BladeStyle* UnSetStyle() {
    if (!offset_) blade_->UnSetStyle();
    return BladeWrapper::UnSetStyle();
  }
  
  void SetupSubBlade(BladeBase* base, int offset, int num_leds) {
    blade_ = base;
    offset_ = offset;
    num_leds_ = num_leds;
  }

  void SetNext(SubBladeWrapper* next) {
    next_ = next;
  }

  // Bladestyle implementation
  virtual void activate() override {
    current_style_->activate();
  }
  virtual void deactivate() override {
    current_style_->deactivate();
  }
  virtual void run(BladeBase* blade) override {
    SubBladeWrapper* tmp = this;
    bool allow_disable = true;
    do {
      tmp->allow_disable_ = false;
      tmp->current_style_->run(tmp);
      allow_disable &= tmp->allow_disable_;
      tmp = tmp->next_;
    } while(tmp != this);
    if (allow_disable) blade_->allow_disable();
  }

 bool NoOnOff() override {
    SubBladeWrapper* tmp = this;
    do {
      if (tmp->current_style_->NoOnOff()) return true;
      tmp = tmp->next_;
    } while(tmp != this);
    return false;
  }

protected:
  int num_leds_;
  int offset_;
  bool allow_disable_;
  SubBladeWrapper* next_;
};

SubBladeWrapper* first_subblade_wrapper = NULL;
SubBladeWrapper* last_subblade_wrapper = NULL;

// This let's you split a single chain of neopixels into multiple blades.
// Let's say you build saber with an 8-led PLI, a single led for a crystal chamber
// crystal chamber and 3 accent LEDs all hooked up as a single neopixel chain.
// The blades[] entry could then look like this:
// { 2000,
//   WS2811BladePtr<144, WS2811_ACTUALLY_800kHz | WS211_GRB>(),
//   SubBlade(0,  7, WS2811BladePtr<15, WS2811_580kHz>()),  // PLI
//   SubBlade(8,  8, NULL),  // crystal chamber
//   SubBlade(9, 11, NULL),  // accent leds
//   CONFIGARRAY(presets) }
//
// In the example above, NUM_BLADES must be 4, so you get to specify
// a style for each section of the string.
class BladeBase* SubBlade(int first_led, int last_led, BladeBase* blade) {
  if (blade)  {
    first_subblade_wrapper = last_subblade_wrapper = NULL;
  } else {
    if (!first_subblade_wrapper) return NULL;
    blade = first_subblade_wrapper->blade_;
  }
  
  if (last_led >= blade->num_leds()) {
    return NULL;
  }
  
  SubBladeWrapper* ret = new SubBladeWrapper();
  if (first_subblade_wrapper) {
    ret->SetNext(last_subblade_wrapper);
    first_subblade_wrapper->SetNext(ret);
    last_subblade_wrapper = ret;
  } else {
    ret->SetNext(ret);
    first_subblade_wrapper = last_subblade_wrapper = ret;
  }
  ret->SetupSubBlade(blade, first_led, last_led + 1 - first_led);
  return ret;
}

class SubBladeWrapperReverse : public SubBladeWrapper {
  void set(int led, Color16 c) override {
    return blade_->set(offset_ - led, c);
  }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(offset_ - led, c);
  }
};

// Like SubBlade, but LEDs are indexed in reverse.
class BladeBase* SubBladeReverse(int first_led, int last_led, BladeBase* blade) {
  if (blade)  {
    first_subblade_wrapper = last_subblade_wrapper = NULL;
  } else {
    if (!first_subblade_wrapper) return NULL;
    blade = first_subblade_wrapper->blade_;
  }
  
  if (last_led >= blade->num_leds()) {
    return NULL;
  }
  
  SubBladeWrapper* ret = new SubBladeWrapperReverse();
  if (first_subblade_wrapper) {
    ret->SetNext(last_subblade_wrapper);
    first_subblade_wrapper->SetNext(ret);
    last_subblade_wrapper = ret;
  } else {
    ret->SetNext(ret);
    first_subblade_wrapper = last_subblade_wrapper = ret;
  }
  ret->SetupSubBlade(blade, last_led, last_led + 1 - first_led);
  return ret;
}
#endif
