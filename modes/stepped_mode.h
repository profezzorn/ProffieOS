#ifndef MODES_STEPPED_MODE_H
#define MODES_STEPPED_MODE_H

#include "mode.h"
#include "select_cancel_mode.h"
#include "../common/cyclint.h"
#include "../common/angle.h"

namespace mode {

// Base class for stepped menues/modes.
// This class makes no assumptions about how sound is played.
template<class SPEC>
struct SteppedModeBase : public SPEC::SelectCancelMode {
  virtual void next() = 0;
  virtual void prev() = 0;
  virtual void update() {}

  virtual int steps_per_revolution() { return 6; }
  virtual int dead_zone_percent() { return 25; }

  // Size of one menu entry.
  virtual float stepsize() {
    return 2 * M_PI / steps_per_revolution();
  }

  // radians from center of one menu option until we
  // reach the next menu option.
  virtual float twistsize() {
    return stepsize() / 2.0 * (200 + dead_zone_percent()) / 200.0;
  }

  void mode_activate(bool onreturn) override {
    angle_ = fusor.angle2();
  }

  
  void mode_Loop() override {
    SaberBase::RequestMotion();
    
    Angle current_angle = fusor.angle2();
    Angle diff = current_angle - angle_;
    if (fabs(diff) > twistsize()) {
	// Acceleration
	float twist_speed = fabs(fusor.gyro().x); // degrees / s
	twist_speed /= 360.0;
	float step_size = stepsize();
	if (twist_speed > 1.0f) {
	  twist_speed *= twist_speed;
	  step_size /= twist_speed;
	}
	
	if (diff > 0) {
	  angle_ += step_size;
	  next();
	} else  {
	  angle_ -= step_size;
	  prev();
	}
	update();
    }
  }

  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, 0)) {
      case EVENTID(BUTTON_LEFT, EVENT_CLICK_SHORT, 0):
	prev();
	update();
	return true;

      case EVENTID(BUTTON_RIGHT, EVENT_CLICK_SHORT, 0):
	next();
	update();
	return true;
    }
    return SPEC::SelectCancelMode::mode_Event2(button, event, modifiers);
  }

  bool mode_Parse(const char *cmd, const char* arg) override {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    // TODO: What if there are "left" and "right" buttons?
    if (!strcmp(cmd, "left") || !strcmp(cmd, "l")) {
      prev();
      update();
      return true;
    }
    if (!strcmp(cmd, "right") || !strcmp(cmd, "r")) {
      next();
      update();
      return true;
    }
#endif
    return SPEC::SelectCancelMode::mode_Parse(cmd, arg);
  }

  static Angle angle_;
};

template<class SPEC>
Angle SteppedModeBase<SPEC>::angle_ = 0.0f;

// Base class for stepped menues/modes.
// This class assumes that say() and fadout() will be used to play the sound
// when the steps change.
template<class SPEC>
struct SteppedMode : public SPEC::SteppedModeBase {
  virtual void say() {}
  virtual void fadeout(float len) {}

  void update() override {
    SaberBase::DoEffect(EFFECT_MENU_CHANGE, 0); // should play a quiet "tick" every time.
    say_time_ = Cyclint<uint32_t>(millis()) + (uint32_t)(SaberBase::sound_length * 1000);
    if (!say_time_) say_time_+=1;
    fadeout(SaberBase::sound_length);
  }

  void mode_Loop() override {
    SPEC::SteppedModeBase::mode_Loop();
    if (say_time_ && Cyclint<uint32_t>(millis()) > say_time_) {
      say_time_ = Cyclint<uint32_t>(0);
      say();
    }
  }

  static Cyclint<uint32_t> say_time_;
};

template<class SPEC>
Cyclint<uint32_t> SteppedMode<SPEC>::say_time_;

}  // namespace mode

#endif
