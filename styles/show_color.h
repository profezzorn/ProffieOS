#ifndef STYLES_SHOW_COLOR_H
#define STYLES_SHOW_COLOR_H

#include "../common/preset.h"

class ShowColorStyle {
public:
  void run(BladeBase *blade) {}
  SimpleColor getColor(int led) { return color_; }
  static void SetColor(Color16 color) { color_ = color; }
private:
  static Color16 color_;
};

Color16 ShowColorStyle::color_;

template<class T, class SHORT_STYLE = ShowColorStyle>
class ShowColorAllBladesTemplate {
public:
  void SetStyle(BladeBase *blade, const char* str) {
    ArgParser ap(SkipWord(str));
    CurrentArgParser = &ap;
    if (blade->num_leds() > 10) {
      blade->SetStyle(new Style<T>());
    } else {
      blade->SetStyle(new Style<SHORT_STYLE>());
    }
  }
  void Start(const char* str) {
#define SHOW_COLOR_STYLE_START(N)				\
    style##N##_ = current_config->blade##N->UnSetStyle();	\
    SetStyle(current_config->blade##N, str);
    ONCEPERBLADE(SHOW_COLOR_STYLE_START);
  }
  void Start() { Start(""); }
  void Stop() {
#define SHOW_COLOR_STYLE_STOP(N)			\
    delete current_config->blade##N->UnSetStyle();	\
    current_config->blade##N->SetStyle(style##N##_);
    ONCEPERBLADE(SHOW_COLOR_STYLE_STOP);
  }
private:
#define SHOW_COLOR_STYLE_DEFINE(N) BladeStyle* style##N##_;
  ONCEPERBLADE(SHOW_COLOR_STYLE_DEFINE)
};

template<class T, class SHORT_STYLE = ShowColorStyle>
class ShowColorSingleBladeTemplate {
public:
  void SetStyle(BladeBase *blade, const char* str) {
    style_ = blade->UnSetStyle();
    ArgParser ap(SkipWord(str));
    CurrentArgParser = &ap;
    if (blade->num_leds() > 10) {
      blade->SetStyle(new Style<T>());
    } else {
      blade->SetStyle(new Style<SHORT_STYLE>());
    }
  }
  void Start(int blade, const char* str) {
#define SHOW_COLOR_STYLE_START2(N)		\
    case N:					\
      SetStyle(current_config->blade##N, str);	\
      break;

    switch (blade) {
      ONCEPERBLADE(SHOW_COLOR_STYLE_START2)
    }
  }
  void Start(int blade) { Start(blade, ""); }
  void Stop(int blade) {
#define SHOW_COLOR_STYLE_STOP2(N)			\
    case N:						\
      delete current_config->blade##N->UnSetStyle();	\
      current_config->blade##N->SetStyle(style_);	\
      break;

    switch (blade) {
      ONCEPERBLADE(SHOW_COLOR_STYLE_STOP2)
    }
  }
private:
  BladeStyle* style_;
};

using ShowColorAllBlades = ShowColorAllBladesTemplate<ShowColorStyle>();
using ShowColorSingleBlade = ShowColorSingleBladeTemplate<ShowColorStyle>();

#endif  // STYLES_SHOW_COLOR_H
