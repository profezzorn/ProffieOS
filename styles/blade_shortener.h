#ifndef STYLE_BLADE_SHORTENER_H
#define STYLE_BLADE_SHORTENER_H

class BladeShortenerWrapper : public BladeStyle, public BladeWrapper {
public:
  BladeShortenerWrapper(int len, BladeStyle* style) : num_leds_(len), style_(style) {
  }
  ~BladeShortenerWrapper() {
    delete style_;
  }

  void run(BladeBase* blade) override {
    blade_ = blade;
    num_leds_ = std::min<int>(blade->num_leds(), num_leds_);
    style_->run(this);
    // Fill rest of blade with black.
    int blade_length = blade_->num_leds();
    for (int i = num_leds_; i < blade_length; i++) {
      blade->set(i, Color16());
    }
  }

  void activate() override { style_->activate(); }
  void deactivate() override { style_->deactivate(); }
  bool NoOnOff() override { return style_->NoOnOff(); }
  bool Charging() override { return style_->Charging(); }
  bool IsHandled(HandledFeature feature) { return style_->IsHandled(feature); }

  int num_leds() const override { return num_leds_; }
  void set_length(int length) {
    num_leds_ = length;
  }
private:
  int num_leds_;
  BladeStyle* style_;
};

#endif
