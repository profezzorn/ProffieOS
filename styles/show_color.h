#ifndef STYLES_SHOW_COLOR_H
#define STYLES_SHOW_COLOR_H

class ShowColorStyle : public BladeStyle {
  bool NoOnOff() override { return true; }
  bool Charging() override { return false; }
  bool IsHandled(HandledFeature effect) override { return false; }
  void activate() override {}

  void run(BladeBase *blade) override {
    int num_leds = blade->num_leds();
    for (int i = 0; i < num_leds; i++) {
      blade->set(i, color_);
    }
  }

  void SetColor(Color16 color) { color_ = color; }
  void Start() {
#define SHOW_COLOR_STYLE_START(N)				\
    style##N##_ = current_config->blade##N->UnSetStyle();	\
    current_config->blade##N->SetStyle(this);
    ONCEPERBLADE(SHOW_COLOR_STYLE_START);
  }
  void Stop() {
#define SHOW_COLOR_STYLE_STOP(N)			\
    current_config->blade##N->UnSetStyle();		\
    current_config->blade##N->SetStyle(style##N##_);
    ONCEPERBLADE(SHOW_COLOR_STYLE_STOP);
  }
private:
  Color16 color_;
#define SHOW_COLOR_STYLE_DEFINE(N) BladeStyle* style##N##_;
  ONCEPERBLADE(SHOW_COLOR_STYLE_DEFINE)
};

#endif  // STYLES_SHOW_COLOR_H
