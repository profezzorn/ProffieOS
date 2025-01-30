#ifndef MODES_SMOOTH_MODE_H
#define MODES_SMOOTH_MODE_H

#include "mode.h"
#include "select_cancel_mode.h"
#include "../common/angle.h"

#if defined(SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION) && SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION > 0
#define SOUND_TICK_ANGLE (M_PI * 2 / SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION)
#endif

namespace mode {

class DeltaAngle {
public:
  void init() {
    last_angle_ = fusor.angle2();
  }
  Angle get() {
    SaberBase::RequestMotion();
    Angle a = fusor.angle2();
    Angle delta = a - last_angle_;
    last_angle_ = a;
    return delta;
  }
private:
  Angle last_angle_;
};

template<class SPEC>
struct SmoothBase : public SPEC::SelectCancelMode {
  // 3 rotations to get back to the original color
  virtual float revolutions() { return 3.0f; }
  void mode_activate(bool onreturn) override {
    if (!onreturn) saved_ = get();
    delta_.init();
    angle_ = Angle::fromFixed(get());
  }

  void exit() override {
    set(saved_);
    SPEC::SelectCancelMode::exit();
  }
  void select() override {
    popMode();
  }

  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, 0)) {
      case EVENTID(BUTTON_LEFT, EVENT_CLICK_SHORT, 0):
	angle_ += M_PI * 2.0 / 36.0;
	return true;

      case EVENTID(BUTTON_RIGHT, EVENT_CLICK_SHORT, 0):
	angle_ -= M_PI * 2.0 / 36.0;
	return true;
    }
    return SPEC::SelectCancelMode::mode_Event2(button, event, modifiers);
  }

  // x = 0-32767
  virtual int get() = 0;
  virtual void set(int x) = 0;


  DeltaAngle delta_;
  Angle angle_;
  int saved_;
};
  
template<class SPEC>
struct SmoothWraparoundMode : public SmoothBase<SPEC> {
  void mode_Loop() override {
    this->angle_ += this->delta_.get() / this->revolutions();
    this->set(this->angle_.fixed());

#ifdef SMOOTH_COLORCHANGE_TICKS_PER_REVOLUTION
    if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_SMOOTH) {
      // Tick Sound in Smooth Mode ---
      static float tick_sound_angle_smooth = 0.0f; 
      float current_angle = fusor.angle2();
      float diff = current_angle - tick_sound_angle_smooth;

      // Normalize angle to (-PI, PI)
      if (diff > M_PI)  diff -= (2.0f * M_PI);
      if (diff < -M_PI) diff += (2.0f * M_PI);

      if (fabsf(diff) >= SOUND_TICK_ANGLE) {
        int steps = (int)floorf(fabsf(diff) / SOUND_TICK_ANGLE);
        if (diff < 0) steps = -steps;

        tick_sound_angle_smooth += steps * SOUND_TICK_ANGLE;
        if (tick_sound_angle_smooth > M_PI)  tick_sound_angle_smooth -= 2.0f * M_PI;
        if (tick_sound_angle_smooth < -M_PI) tick_sound_angle_smooth += 2.0f * M_PI;

        hybrid_font.PlayPolyphonic(&SFX_ccchange);
      }
    }
#endif

  }
};

template<class SPEC>
struct SmoothMode : public SmoothBase<SPEC> {
  // x = 0-32767
  virtual void min_bump() {}
  virtual void max_bump() {}
  virtual float margin_fraction() { return 0.02; /* 2% */ }

  void mode_activate(bool onreturn) override {
    SmoothBase<SPEC>::mode_activate(onreturn);
    this->angle_ = Angle::fromFixedWithMargin(this->get(), margin_fraction());
  }
  void mode_Loop() override {  float last_angle_;
    switch (this->angle_.increment_with_guardrails(
	      this->delta_.get() / this->revolutions())) {
      case -1: min_bump(); break;
      case  1: max_bump(); break;
    }
    this->set(this->angle_.fixed_with_margin(margin_fraction()));
  }
};

} // namespace mode

#endif
