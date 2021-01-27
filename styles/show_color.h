#ifndef STYLES_SHOW_COLOR_H
#define STYLES_SHOW_COLOR_H

#include "../common/preset.h"

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
private:
  Color16 color_;
};

class ShowColorAllBlades : public ShowColorStyle {
public:
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
#define SHOW_COLOR_STYLE_DEFINE(N) BladeStyle* style##N##_;
  ONCEPERBLADE(SHOW_COLOR_STYLE_DEFINE)
};

class ShowColorSingleBlade : public ShowColorStyle {
public:
  void Start(int blade) {
#define SHOW_COLOR_STYLE_START2(N)				\
    case N:							\
      style_ = current_config->blade##N->UnSetStyle();	        \
      current_config->blade##N->SetStyle(this);			\
      break;

    switch (blade) {
      ONCEPERBLADE(SHOW_COLOR_STYLE_START2)
    }
  }
  void Stop(int blade) {
#define SHOW_COLOR_STYLE_STOP2(N)			\
    case N:						\
      current_config->blade##N->UnSetStyle();		\
      current_config->blade##N->SetStyle(style_);	\
      break;

    switch (blade) {
      ONCEPERBLADE(SHOW_COLOR_STYLE_STOP2)
    }
  }
private:
  BladeStyle* style_;
};

#endif  // STYLES_SHOW_COLOR_H
