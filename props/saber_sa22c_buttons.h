#ifndef PROPS_SABER_SA22C_BUTTONS_H
#define PROPS_SABER_SA22C_BUTTONS_H

#include "prop_base.h"

#undef PROP_TYPE
#define PROP_TYPE SaberSA22CButtons

// The Saber class implements the basic states and actions
// for the saber.
class SaberSA22CButtons : public PropBase {
public:
  SaberSA22CButtons() : PropBase() {}
  const char* name() override { return "SaberSA22CButtons"; }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
        if (accel_.x < -0.15) {
          pointing_down_ = true;
        } else {
          pointing_down_ = false;
        }
      return true;

#if NUM_BUTTONS == 0
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
#endif
      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
 #if NUM_BUTTONS == 2
        if (MODE_VOLUME){
          STDOUT.println("Volume up");
          if (dynamic_mixer.get_volume() < VOLUME){
            current_volume_ += VOLUME * 0.10;
            dynamic_mixer.set_volume(current_volume_);
            beeper.Beep(0.5, 2000);
            STDOUT.print("Current Volume: ");
            STDOUT.println(dynamic_mixer.get_volume());
          }
          else{
           beeper.Beep(0.5, 3000);
          }
        }
#endif
        if (!MODE_VOLUME){
          aux_on_ = false;
          On();
        }
	return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF):
#if NUM_BUTTONS == 2
        if (MODE_VOLUME){
          STDOUT.println("Volume Down");
          if (dynamic_mixer.get_volume() > (0.10 * VOLUME)){
            current_volume_ = dynamic_mixer.get_volume();
            current_volume_ -= (VOLUME * 0.10) ;
            dynamic_mixer.set_volume(current_volume_);
            beeper.Beep(0.5, 2000);
            STDOUT.print("Current Volume: ");
            STDOUT.println(dynamic_mixer.get_volume());
          }
          else{
            beeper.Beep(0.5, 1000);
          }
        }
#endif
#if NUM_BUTTONS == 3
        if (MODE_VOLUME){
          STDOUT.println("Volume up");
          if (dynamic_mixer.get_volume() < VOLUME){
            current_volume_ += VOLUME * 0.10;
            dynamic_mixer.set_volume(current_volume_);
            beeper.Beep(0.5, 2000);
            STDOUT.print("Current Volume: ");
            STDOUT.println(dynamic_mixer.get_volume());
          }
          else{
            beeper.Beep(0.5, 3000);
          }
        }
#endif
        if (!MODE_VOLUME) {
#ifdef DUAL_POWER_BUTTONS
          aux_on_ = true;
          On();
#else
      if (!MODE_VOLUME) {
        next_preset();
	  }

#endif
        }
	return true;

#if NUM_BUTTONS > 1
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
        SaberBase::DoBlast();
        return true;
#endif
#if NUM_BUTTONS == 1
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
#ifndef DISABLE_COLOR_CHANGE
        if (accel_.x < -0.15) {
				  ToggleColorChangeMode();
		} else {
			  SaberBase::DoForce();
		}
#else
        SaberBase::DoForce();
#endif
        return true;
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        StartOrStopTrack();
        return true;
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
        SaberBase::DoBlast();
        return true;
#endif
      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
        if (millis() - activated_ < 500) {
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
          }
        }
#if NUM_BUTTONS > 1
        else{
          SaberBase::DoForce();
        }
#endif
	return true;

#if NUM_BUTTONS == 1
        case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_ON):
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
	        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
	          // Just exit color change mode.
	          // Don't turn saber off.
	          ToggleColorChangeMode();
	          return true;
	        }
#endif
          Off();
        }
		return true;
#endif

#ifndef DISABLE_COLOR_CHANGE
#ifdef NO_LOCKUP_HOLD
    case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
      ToggleColorChangeMode();
	    break;
#else
    case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
      ToggleColorChangeMode();
	    break;    
#endif
#endif
#if NUM_BUTTONS > 1
      case EVENTID(BUTTON_POWER, EVENT_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_OFF, MODE_ON):
#if NUM_BUTTONS == 0
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
#endif
#endif
#if NUM_BUTTONS != 1
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
	        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
	          // Just exit color change mode.
	          // Don't turn saber off.
	          ToggleColorChangeMode();
	          return true;
	        }
#endif
          Off();
        }
#endif
        return true;

	  case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
    case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
#ifndef NO_LOCKUP_HOLD
	  case EVENTID(BUTTON_AUX, EVENT_HELD, MODE_ON):
#endif
		if (!SaberBase::Lockup()) {
		  if (pointing_down_) {
			SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
		  } else {
			SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
		  }
		  SaberBase::DoBeginLockup();
	  return true;
		}

        // Off functions
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
#if NUM_BUTTONS == 1
	  if (!MODE_VOLUME);
        next_preset();

        return true;
#endif
#if NUM_BUTTONS > 1
        StartOrStopTrack();
#endif
	return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_OFF):
      current_volume_ = dynamic_mixer.get_volume();
      if (MODE_VOLUME){
        MODE_VOLUME = false;
        beeper.Beep(0.5, 3000);
        STDOUT.println("Exit Volume Menu");
      }
      else{
        MODE_VOLUME = true;
        beeper.Beep(0.5, 3000);
        STDOUT.println("Enter Volume Menu");
      }
      return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        SaberBase::RequestMotion();
	return true;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
	    if (!MODE_VOLUME) {
          next_preset();
		}

	return true;

#if NUM_BUTTONS < 3
      case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_OFF):
#endif
        if (!MODE_VOLUME) {
          previous_preset();
		}
	return true;
      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF):
      talkie.SayDigit((int)floorf(battery_monitor.battery()));
      talkie.Say(spPOINT);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
      talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
      talkie.Say(spVOLTS);
      return true;

      case EVENTID(BUTTON_AUX2, EVENT_CLICK_SHORT, MODE_OFF):
      STDOUT.println(NUM_BUTTONS);
#if NUM_BUTTONS == 3
        if (MODE_VOLUME){
          STDOUT.println("Volume Down");
          if (dynamic_mixer.get_volume() > (0.10 * VOLUME)){
            current_volume_ = dynamic_mixer.get_volume();
            current_volume_ -= (VOLUME * 0.10) ;
            dynamic_mixer.set_volume(current_volume_);
            beeper.Beep(0.5, 2000);
            STDOUT.print("Current Volume: ");
            STDOUT.println(dynamic_mixer.get_volume());
          }
          else{
            beeper.Beep(0.5, 1000);
          }
        }

#endif

#ifdef DUAL_POWER_BUTTONS
        if (!MODE_VOLUME) {
			next_preset();

		}
#else
        if (!MODE_VOLUME) {
			previous_preset();

		}
#endif
	return true;

	// Events that needs to be handled regardless of what other buttons
	// are pressed.
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
	if (SaberBase::Lockup()) {
	  SaberBase::DoEndLockup();
	  SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
	  return true;
	}
    }
    return false;
  }
private:
  bool aux_on_ = true;
  bool pointing_down_ = false;
  bool MODE_VOLUME = false;
  int32_t current_volume_ = dynamic_mixer.get_volume();
};

#endif

