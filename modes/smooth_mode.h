#ifndef MODES_SMOOTH_MODE_H
#define MODES_SMOOTH_MODE_H

#include "mode.h"
#include "select_cancel_mode.h"
#include "../common/angle.h"

namespace mode {

class DeltaAngle {
public:
  void init() {
    last_angle_ = fusor.angle2();
  }
  Angle get() {
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
    saved_ = get();
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

  // x = 0-32767
  virtual int get() = 0;
  virtual void set(int x) = 0;

  void mode_Loop() override { set(angle_.fixed()); }

  DeltaAngle delta_;
  Angle angle_;
  int saved_;
};
  
template<class SPEC>
struct SmoothWraparoundMode : public SmoothBase<SPEC> {
  void mode_Loop() override {
    this->angle_ += this->delta_.get() / this->revolutions();
    SmoothBase<SPEC>::mode_Loop();
  }
};

template<class SPEC>
struct SmoothMode : public SmoothBase<SPEC> {
  // x = 0-32767
  virtual void min_bump() {}
  virtual void max_bump() {}

  void mode_Loop() override {  float last_angle_;
    switch (this->angle_.increment_with_guardrails(
	      this->delta_.get() / this->revolutions())) {
      case -1: min_bump(); break;
      case  1: max_bump(); break;
    }
    SmoothBase<SPEC>::mode_Loop();
  }
};

} // namespace mode

#endif
