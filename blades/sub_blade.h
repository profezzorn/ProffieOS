#ifndef BLADES_SUB_BLADE_H
#define BLADES_SUB_BLADE_H

/*
Usage: SubBlade(first_led, last_led, blade_definition)
This let's you split a single chain of pixel LEDs into multiple blades.
Let's say you build saber with:
A main blade of 144 pixels, on the data 1 pad and powered off LED pads 2 and 3.
Also, 3 accent pixels, a single pixel for a crystal chamber,
and a 5 pixel hilt PCB connector, all hooked up in that order in a series data
chain off of data 2 pad, powered by LED pad 6. (data in -> data out -> data in etc...)
The blades[] entry could then look like this:
{ 0,
  WS281XBladePtr<144, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),    // Main Blade
  SubBlade(4, 8, WS281XBladePtr<9, blade2Pin, Color8::GRB, PowerPINS<bladePowerPin6> >() ),  // Hilt PCB connector
  SubBlade(3, 3, NULL),  // crystal chamber
  SubBlade(0, 2, NULL),  // accent leds
  CONFIGARRAY(presets) }

In the example above, NUM_BLADES must be 4, so you get to specify
a style for each section of the string.
Note that the first pixel address starts at zero.
Also note the use of NULL for the blade definitions for the remaining SubBlades.

Usage: SubBladeReverse(first_led, last_led, blade_definition)
Exactly like SubBlade, but LEDs are indexed in reverse, so the last LED gets data first etc...

Usage: SubBladeWithStride(first_led, last_led, stride_length, blade_definition)
Like SubBlade, but LEDs are indexed with an additional 'stride' parameter,
allowing you to "skip" over a regular number of pixels in the data chain. (Such as every other one)

Usage: SubBladeWithList<int1, int2, ...>(blade_definition)
Like SubBlade, but you provide a custom list of LED indices instead of a range.
Useful for ring-based or irregular LED layouts.

For more in-depth explanations, see the SubBlade Wiki pages here:
https://github.com/profezzorn/ProffieOS/wiki/SubBlade
https://github.com/profezzorn/ProffieOS/wiki/SubBladeReverse
*/

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
  void Activate(int blade_number) override {
    blade_number_ = blade_number;
    if (!active_) {
      if (!SomeSubBladeIsActive()) BladeWrapper::Activate(0);
      active_ = true;
    }
  }
  void Deactivate() override {
    if (active_) {
      active_ = false;
      if (!SomeSubBladeIsActive()) BladeWrapper::Deactivate();
    }
  }

  bool primary() const {
    return primary_;
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
    if (next_ == this) primary_ = true;
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

  int GetBladeNumber() const override {
    return blade_number_;
  }

protected:
  BladeStyle *current_style_ = nullptr;
  int num_leds_;
  int offset_;
  bool allow_disable_;
  SubBladeWrapper* next_;
  int blade_number_;
  bool primary_ = false;
};

SubBladeWrapper* first_subblade_wrapper = NULL;
SubBladeWrapper* last_subblade_wrapper = NULL;

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

class SubBladeWrapperZZ : public SubBladeWrapperWithStride {
public:
  void SetCol(int col) {
    col_ = col;
  }
  int translate(int led) const {
    return offset_ + led * stride_ + ((led & 1) ? col_ : (stride_ - 1 - col_));
  }
  void set(int led, Color16 c) override {
    return blade_->set(translate(led), c);
  }
  void set_overdrive(int led, Color16 c) override {
    return blade_->set_overdrive(translate(led), c);
  }
private:
  int col_;
};

class BladeBase* SubBladeZZ(int first_led, int last_led, int stride, int column, BladeBase* blade) {
  if (blade)  {
    first_subblade_wrapper = last_subblade_wrapper = NULL;
  } else {
    if (!first_subblade_wrapper) return NULL;
    blade = first_subblade_wrapper->blade_;
  }

  if (last_led >= blade->num_leds()) {
    return NULL;
  }

  SubBladeWrapperZZ* ret = new SubBladeWrapperZZ();
  if (first_subblade_wrapper) {
    ret->SetNext(last_subblade_wrapper);
    first_subblade_wrapper->SetNext(ret);
    last_subblade_wrapper = ret;
  } else {
    ret->SetNext(ret);
    first_subblade_wrapper = last_subblade_wrapper = ret;
  }
  ret->SetupStride(stride);
  ret->SetCol(column);
  ret->SetupSubBlade(blade, first_led, (last_led - first_led)/stride + 1);
  return ret;
}

class SubBladeWrapperWithList : public SubBladeWrapper {
public:
  SubBladeWrapperWithList(const int* indices, int count)
      : indices_(indices), count_(count) {}
  void set(int led, Color16 c) override {
    if (led >= 0 && led < count_ && indices_[led] >= 0) {
      blade_->set(indices_[led], c);
    }
  }
  void set_overdrive(int led, Color16 c) override {
    if (led >= 0 && led < count_ && indices_[led] >= 0) {
      blade_->set_overdrive(indices_[led], c);
    }
  }

  int num_leds() const override { return count_; }

protected:
  const int* indices_;
  int count_;
};

BladeBase* SubBladeWithList(const int* indices, int count, BladeBase* blade) {
  if (blade) {
    first_subblade_wrapper = last_subblade_wrapper = NULL;
  } else {
    if (!first_subblade_wrapper) return NULL;
    blade = first_subblade_wrapper->blade_;
  }

  SubBladeWrapperWithList* ret = new SubBladeWrapperWithList(indices, count);

  if (first_subblade_wrapper) {
    ret->SetNext(last_subblade_wrapper);
    first_subblade_wrapper->SetNext(ret);
    last_subblade_wrapper = ret;
  } else {
    ret->SetNext(ret);
    first_subblade_wrapper = last_subblade_wrapper = ret;
  }

  ret->SetupSubBlade(blade, 0, count);
  return ret;
}

// Allow inline LED indices in the blade config
template <int... Indices>
BladeBase* SubBladeWithList(BladeBase* blade) {
  static const int arr[sizeof...(Indices)] = { Indices... };
  return SubBladeWithList(arr, sizeof...(Indices), blade);
}

class BarBackWrapper : public SubBladeWrapper {
public:
  void set(int led, Color16 c) override {
    int unit = led / 10;
    led %= 10;
    int chip = led / 3;
    int channel = led % 3;
    if (cnt_ == 0) {
      if (chip == 3) {
	cnt_ = 1;
      } else {
	cnt_ = 3;
      }
    }
    int w = std::max<uint16_t>(std::max<uint16_t>(c.r,c.g), c.b);
    switch ((inverted_blade_byte_order_ >> (8 - channel * 4)) & 0xf) {
      case 1: tmp.r = w; break;
      case 2: tmp.g = w; break;
      case 3: tmp.b = w; break;
    }
    if (--cnt_ == 0) {
      blade_->set(unit * 4 + chip + offset_, tmp);
      tmp = Color16(65535,65535,65535);
    }
  }
  void set_overdrive(int led, Color16 c) override {
    set(led, c);
  }
  void SetupBarBack(BladeBase* base, int first_led, int units) {
    units_ = units;
    SubBladeWrapper::SetupSubBlade(base, first_led, 4 * units);
    inverted_blade_byte_order_ = Color8::invert_byteorder(base->get_byteorder());
  }
  int num_leds() const override { return units_ * 10; }
private:
  Color16 tmp;
  uint16_t cnt_ = 0;
  uint16_t units_ = 0;
  int inverted_blade_byte_order_;
};

class BladeBase* BarBack(int first_led, int units, BladeBase* blade) {
  if (blade)  {
    first_subblade_wrapper = last_subblade_wrapper = NULL;
  } else {
    if (!first_subblade_wrapper) return NULL;
    blade = first_subblade_wrapper->blade_;
  }

  int last_led = first_led + 4 * units - 1;

  if (last_led >= blade->num_leds()) {
    return NULL;
  }

  BarBackWrapper* ret = new BarBackWrapper();
  if (first_subblade_wrapper) {
    ret->SetNext(last_subblade_wrapper);
    first_subblade_wrapper->SetNext(ret);
    last_subblade_wrapper = ret;
  } else {
    ret->SetNext(ret);
    first_subblade_wrapper = last_subblade_wrapper = ret;
  }
  ret->SetupBarBack(blade, first_led, units);
  return ret;
}

#endif
