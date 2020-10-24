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

  bool active_ = false;
  bool SomeSubBladeIsActive() {
    bool some_subblade_is_active = false;
    SubBladeWrapper* tmp = this;
    do {
      some_subblade_is_active |= tmp->active_;
      tmp = tmp->next_;
    } while (tmp != this);
    return some_subblade_is_active;
  }
  void Activate() override {
    if (!active_) {
      if (!SomeSubBladeIsActive()) BladeWrapper::Activate();
      active_ = true;
    }
  }
  void Deactivate() override {
    if (active_) {
      active_ = false;
      if (!SomeSubBladeIsActive()) BladeWrapper::Deactivate();
    }
  }
  virtual bool primary() const {
    return !offset_;
  }
  void clear() override {
    if (primary()) BladeWrapper::clear();
  }
  void SetStyle(BladeStyle* style) override {
    // current_style should be nullptr;
    current_style_ = style;
    if (current_style_) {
      current_style_->activate();
    }
    if (primary()) blade_->SetStyle(this);
  }

  BladeStyle* UnSetStyle() override {
    if (primary()) blade_->UnSetStyle();
    BladeStyle *ret = current_style_;
    if (ret) {
      ret->deactivate();
    }
    current_style_ = nullptr;
    return ret;
  }

  BladeStyle* current_style() const override {
    return current_style_;
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
    if (current_style_)
      current_style_->activate();
  }
  virtual void deactivate() override {
    if (current_style_)
      current_style_->deactivate();
  }
  virtual void run(BladeBase* blade) override {
    SubBladeWrapper* tmp = this;
    bool allow_disable = true;
    do {
      tmp->allow_disable_ = false;
      if (tmp->current_style_)
	tmp->current_style_->run(tmp);
      allow_disable &= tmp->allow_disable_;
      tmp = tmp->next_;
    } while(tmp != this);
    if (allow_disable) blade_->allow_disable();
  }
  bool IsHandled(HandledFeature effect) override {
    if (current_style_)
      return false;
    return current_style_->IsHandled(effect);
  }

 bool NoOnOff() override {
    SubBladeWrapper* tmp = this;
    do {
      if (tmp->current_style_ && tmp->current_style_->NoOnOff()) return true;
      tmp = tmp->next_;
    } while(tmp != this);
    return false;
  }

protected:
  BladeStyle *current_style_ = nullptr;
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
  virtual bool primary() const override {
    return offset_ + 1 == num_leds_;
  }
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

class SubBladeWrapperWithStride : public SubBladeWrapper {
public:
  void SetupStride(int stride) {
    stride_ = stride;
  }
  void set(int led, Color16 c) override {
    return blade_->set((led * stride_) + offset_, c);
  }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive((led * stride_) + offset_, c);
  }
  
protected:
  int stride_;
 };

// Like SubBlade, but LEDs are indexed with an additional 'stride' parameter.
class BladeBase* SubBladeWithStride(int first_led, int last_led, int stride, BladeBase* blade) {
  if (blade)  {
    first_subblade_wrapper = last_subblade_wrapper = NULL;
  } else {
    if (!first_subblade_wrapper) return NULL;
    blade = first_subblade_wrapper->blade_;
  }

  if (last_led >= blade->num_leds()) {
    return NULL;
  }

  SubBladeWrapperWithStride* ret = new SubBladeWrapperWithStride();
  if (first_subblade_wrapper) {
    ret->SetNext(last_subblade_wrapper);
    first_subblade_wrapper->SetNext(ret);
    last_subblade_wrapper = ret;
  } else {
    ret->SetNext(ret);
    first_subblade_wrapper = last_subblade_wrapper = ret;
  }
  ret->SetupStride(stride);
  ret->SetupSubBlade(blade, first_led, (last_led - first_led)/stride + 1);
  return ret;
}

#endif
