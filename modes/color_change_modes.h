#ifndef MODE_COLOR_CHANGE_MODE_H
#define MODE_COLOR_CHANGE_MODE_H

#include "mode.h"
#include "stepped_mode.h"
#include "smooth_mode.h"

namespace mode {

template<class SPEC>
struct SteppedVariationMode : public SPEC::SteppedMode {
  void mode_activate(bool onreturn) override {
    saved_ = get();
    SPEC::SteppedMode::mode_activate(onreturn);
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_STEPPED);
  }
  void exit() override {
    set(saved_);
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
    popMode();
  }
  void select() override {
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
    popMode();
  }
  void next() override { SaberBase::UpdateVariation(1); }
  void prev() override { SaberBase::UpdateVariation(-1); }

  int get() { return SaberBase::GetCurrentVariation(); }
  void set(int x) { SaberBase::SetVariation(x); }
private:
  int saved_;
};

template<class SPEC>
struct SmoothVariationMode : public SPEC::SmoothWraparoundMode {
  void mode_activate(bool onreturn) override {
    saved_ = get();
    SPEC::SmoothWraparoundMode::mode_activate(onreturn);
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_SMOOTH);
  }
  void exit() override {
    set(saved_);
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
    popMode();
  }
  void select() override {
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
    popMode();
  }
  int get() override { return SaberBase::GetCurrentVariation(); }
  void set(int x) override { SaberBase::SetVariation(x); }
private:
  int saved_;
};

template<class SPEC>
struct ColorChangeMode : public ModeInterface {
  void mode_activate(bool onreturn) override {
    current_mode = previous_;

    bool handles_color_change = false;
#define CHECK_SUPPORTS_COLOR_CHANGE2(N) \
    handles_color_change |= current_config->blade##N->current_style() && current_config->blade##N->current_style()->IsHandled(HANDLED_FEATURE_CHANGE_TICKED);
    ONCEPERBLADE(CHECK_SUPPORTS_COLOR_CHANGE2);
    if (!handles_color_change) {
      pushMode<typename SPEC::SmoothVariationMode>();
    } else {
#ifdef COLOR_CHANGE_DIRECT
      PVLOG_NORMAL << "Color change, TICK+\n";
      SaberBase::UpdateVariation(1);
#else	
      pushMode<typename SPEC::SteppedVariationMode>();
#endif      
    }
  }
};

}

template<class SPEC>
struct ColorChangeOnlyMenuSpec {
  typedef mode::SelectCancelMode SelectCancelMode;
  typedef mode::SteppedMode<SPEC> SteppedMode;
  typedef mode::SmoothWraparoundMode<SPEC> SmoothWraparoundMode;
  typedef mode::SmoothVariationMode<SPEC> SmoothVariationMode;
  typedef mode::SteppedVariationMode<SPEC> SteppedVariationMode;
  typedef mode::ColorChangeMode<SPEC> ColorChangeMenu;
};

#endif
