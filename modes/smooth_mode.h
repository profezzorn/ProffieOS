#ifndef MODES_SMOOTH_MODE_H
#define MODES_SMOOTH_MODE_H

#include "mode.h"
#include "select_cancel_mode.h"

namespace mode {

template<class SPEC>
struct SmoothMode : public SPEC::SelectCancelMode {
  // 3 rotations to get back to the original color
  virtual float revolutions() { return 3.0f; }
  void mode_activate(bool onreturn) override {
    last_angle_ = fusor.angle2();
    angle_ = (get() + 0.5) * M_PI * 2 * revolutions() / 32768;
  }

  // x = 0-32767
  virtual int get() = 0;
  virtual void set(int x) = 0;

  void mode_Loop() override {
    float a = fusor.angle2();
    float delta = a - last_angle_;
    last_angle_ = a;
    angle_ = fmodf(angle_ + delta, M_PI * 2 * revolutions());
    SaberBase::SetVariation(0x7fff & (int32_t)(a * (32768 / (M_PI * 2 * revolutions()))));
  }
  
  float angle_;
  float last_angle_;
};

} // namespace mode

#endif
