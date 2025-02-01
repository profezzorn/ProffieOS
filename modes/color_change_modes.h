#ifndef MODE_COLOR_CHANGE_MODE_H
#define MODE_COLOR_CHANGE_MODE_H

#include "mode.h"
#include "stepped_mode.h"
#include "smooth_mode.h"

#if defined(SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION) && SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION > 0
  #define SOUND_TICK_ANGLE (M_PI * 2 / SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION)
#endif

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
    SPEC::SmoothWraparoundMode::mode_activate(onreturn);
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_SMOOTH);
  }
  void mode_deactivate() override {
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
  }
  int get() override { return SaberBase::GetCurrentVariation(); }
  void set(int x) override { SaberBase::SetVariation(x); }

  void mode_Loop() override {
    SPEC::SmoothWraparoundMode::mode_Loop();

#ifdef SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION
    static float tick_sound_angle_smooth = 0.0f; 
    float current_angle = fusor.angle2();
    float diff = current_angle - tick_sound_angle_smooth;

    if (diff > M_PI)  diff -= (2.0f * M_PI);
    if (diff < -M_PI) diff += (2.0f * M_PI);

    if (fabsf(diff) >= SOUND_TICK_ANGLE) {
      int steps = (int)floorf(fabsf(diff) / SOUND_TICK_ANGLE);
      if (diff < 0) steps = -steps;

      tick_sound_angle_smooth += steps * SOUND_TICK_ANGLE;
      if (tick_sound_angle_smooth > M_PI)  tick_sound_angle_smooth -= 2.0f * M_PI;
      if (tick_sound_angle_smooth < -M_PI) tick_sound_angle_smooth += 2.0f * M_PI;

      if (!hybrid_font.PlayPolyphonic(&SFX_mclick)) beeper.Beep(0.03, 5000);;
    }
#endif
  }
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
  typedef mode::SteppedModeBase<SPEC> SteppedModeBase;
  typedef mode::SteppedMode<SPEC> SteppedMode;
  typedef mode::SmoothWraparoundMode<SPEC> SmoothWraparoundMode;
  typedef mode::SmoothVariationMode<SPEC> SmoothVariationMode;
  typedef mode::SteppedVariationMode<SPEC> SteppedVariationMode;
  typedef mode::ColorChangeMode<SPEC> ColorChangeMenu;
};

#endif
