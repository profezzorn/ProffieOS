#ifndef BLADES_DIM_BLADE_WRAPPER_H
#define BLADES_DIM_BLADE_WRAPPER_H

class DimBladeWrapper : public BladeWrapper, BladeStyle 
#ifdef ENABLE_MENU_DIMBLADE
      ,protected SaberBase
#endif
      {
public:
  DimBladeWrapper(BladeBase* blade, int fraction) {
    blade_ = blade;
#ifdef ENABLE_MENU_DIMBLADE
    SaberBase::SetBrightness((float)fraction/16384.0*100.0);
#else
    fraction_ = fraction;
#endif
    }
  void set(int led, Color16 c) override {
#ifdef ENABLE_MENU_DIMBLADE
    int fraction_ = SaberBase::GetCurrentBrightness();
#endif
    Color16 ret;
    ret.r = clampi32((c.r * fraction_) >> 14, 0, 65535);
    ret.g = clampi32((c.g * fraction_) >> 14, 0, 65535);
    ret.b = clampi32((c.b * fraction_) >> 14, 0, 65535);
    return blade_->set(led, ret);
  }

  void SetStyle(BladeStyle* style) override {
    // current_style should be nullptr;
    current_style_ = style;
    if (current_style_) {
      current_style_->activate();
    }
    blade_->SetStyle(this);
  }
  BladeStyle* UnSetStyle() override {
    blade_->UnSetStyle();
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
    if (current_style_)
    current_style_->run(this);
  }
  bool IsHandled(HandledFeature effect) override {
    if (!current_style_) return false;
    return current_style_->IsHandled(effect);
  }

  bool NoOnOff() override {
    if (!current_style_) return false;
    return current_style_->NoOnOff();
  }
  
private:
  BladeStyle *current_style_ = nullptr;
#ifndef ENABLE_MENU_DIMBLADE
  int fraction_;
#endif
};

// Reduces the brightness of the blade.
// percentage = 0 -> dark
// percentage = 100 -> no change
class BladeBase* DimBlade(float percentage, BladeBase* blade) {
  return new DimBladeWrapper(blade, (int)(percentage * 16384 / 100.0));
}

#endif
