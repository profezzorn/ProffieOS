// 0 Buttons:
// Activate Muted - None
// Activate blade - forward or backward horizontal thrust movement or sharp swing movement (Swing On)
// Play/Stop Music - hit the saber (perform clash event) holding the blade up while it's OFF
// Turn the blade off - twist the saber like a bike handle holding the saber horizontally or blade down
// Next Preset - slightly shake the saber like a soda can while blade is OFF (hold the saber vertically blade up in range up to 30 degrees tilt)
// Previous Preset - None
// Lockup - automatic by default (Battle Mode) - activates when clash happens and keeps active until swing is registered
// Drag - None
// Blaster Blocks - None
// Force Effects - perform a "push" gesture holding the saber vertically or horizontally perpendicular to the arm
// Enter Color Change mode - slightly shake the saber like a soda can while blade is ON (hold the saber vertically blade up in range up to 30 degrees tilt)
// Confirm selected color and exit Color Change mode - while in Color Change mode hit the saber (perform clash event)
// Melt - None
// Lightning Block - None
// Enter Multi-Block mode - None
//
// 1 Button:
// Activate Muted - fast double click while OFF
// Activate blade - short click while OFF or forward thrust movement + hit or forward thrust movement or sharp swing movement (Swing On)
// Play/Stop Music - hold 1 second and release while ON
// Turn the blade off - hold and wait till blade is off while ON (like in Plecter boards) or twist the saber like a bike handle
// Next Preset - hold 1 second and release while OFF
// Previous Preset - hold and wait while OFF
// Lockup - hold + hit clash while ON
// Drag - hold + hit clash while ON pointing the blade tip down
// Blaster Blocks - short click while ON
// Force Effects - hold the button + perform "push" gesture holding the hilt vertically
// Enter Color Change mode - hold the button + twist the hilt then release the button while ON
// Confirm selected color and exit Color Change mode - hold the button until confirmation sound
// Melt - hold the button + stab while ON
// Lightning Block - fast double click + hold the button while ON
// Enter Multi-Block mode - hold the button + swing the saber and release the button while ON (now swing the saber, blaster blocks will trigger automatically)
// Exit Multi-Block mode - short click the button while ON
// Enter Battle Mode - hold the button + use "Gesture/Swing Ignition" then release the button while OFF (blade will turn ON in Battle Mode)
// Exit Battle Mode - turn the blade OFF
// Enter Volume Menu - hold + clash while OFF
// Volume DOWN - hold and release while in Volume Menu
// Volume UP - short click while in Volume Menu
// Exit Volume Menu - hold + clash while OFF and in Volume Menu
// Battery Level - triple click while OFF
//
// 2 Buttons:
// Activate Muted - fast double click Activation button while OFF
// Activate blade - short click Activation button while OFF or forward thrust movement + hit or forward thrust movement or sharp swing movement (Swing On)
// Play/Stop Music - hold 1 second and release Activation button while OFF or ON
// Turn the blade off - hold and wait till blade is off while ON (like in Plecter boards) or twist the saber like a bike handle
// Next Preset - short click AUX button while OFF
// Previous Preset - hold AUX and click Activation button while OFF
// Lockup - hold AUX button while ON (like in Plecter boards)
// Drag - hold AUX button while ON pointing the blade tip down
// Blaster Blocks - short click AUX button while ON
// Force Effects - short click Activation button while ON or perform "push" gesture holding the hilt vertically
// Enter Color Change mode - hold AUX and quickly press and release Activation button while ON then release AUX button
// Confirm selected color and exit Color Change mode - hold the button until confirmation sound
// Melt - hold AUX (or Activation) button + perform stab action while ON
// Lightning Block - hold Activation button + short click AUX button while ON
// Enter Multi-Block mode - hold the Activation button + swing the saber and release the button while ON (now swing the saber, blaster blocks will trigger automatically)
// Exit Multi-Block mode - short click AUX button while ON
// Enter Battle Mode - hold the AUX button + swing the saber and release the button while ON or hold the Activation button + use "Gesture/Swing Ignition" then release the button while OFF (blade will turn ON in Battle Mode)
// Exit Battle Mode - hold the AUX button + swing the saber and release the button while ON or turn the blade OFF
// Enter Volume Menu - long click AUX button while OFF
// Volume UP - short click Activation button while OFF and in Volume Menu
// Volume DOWN - short click AUX button while OFF and in Volume Menu
// Exit Volume Menu - long click AUX button while OFF and in Volume Menu
// Battery level - hold AUX button while OFF
//
//
// CUSTOM SOUNDS SUPPORTED (add to font to enable):
//
// On Demand Power Save - dim.wav
// On Demand Battery Level - battery.wav
// Battle Mode On (on toggle) - bmbegin.wav
// Battle Mode Off (on toggle) - bmend.wav
// Enter Volume Menu - vmbegin.wav
// Exit Volume Menu - vmend.wav
// Force Push - push.wav
// Fast On (optional) - faston.wav
// Multi-Blast Mode On - blstbgn.wav
// Multi-Blast Mode Off - blstend.wav

#ifndef PROPS_SABER_SHTOK_BUTTONS_H
#define PROPS_SABER_SHTOK_BUTTONS_H

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 5 * 1000
#endif

#include "prop_base.h"
#include "../sound/hybrid_font.h"

#undef PROP_TYPE
#define PROP_TYPE SaberShtokButtons

EFFECT(dim); // for EFFECT_POWERSAVE
EFFECT(battery); // for EFFECT_BATTERY_LEVEL
EFFECT(bmbegin); // for Begin Battle Mode
EFFECT(bmend); // for End Battle Mode
EFFECT(faston); // for EFFECT_FAST_ON
EFFECT(push); // for Force Push gesture in Battle Mode

// The Saber class implements the basic states and actions for the saber.
class SaberShtokButtons : public PROP_INHERIT_PREFIX PropBase {
public:
SaberShtokButtons() : PropBase() {}
  const char* name() override { return "SaberShtokButtons"; }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    DetectShake();
    Vec3 mss = fusor.mss();
    if (SaberBase::IsOn()) {
      DetectSwing();
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > 300 &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > 300 &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }

// EVENT_PUSH - move forward holding the hilt vertically 
      if (fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 70 &&
          fusor.swing_speed() < 30 &&
          fabs(fusor.gyro().x) < 10) {
        if (millis() - push_begin_millis_ > 3) {
          Event(BUTTON_NONE, EVENT_PUSH);
          push_begin_millis_ = millis();
        } 
      } else {
        push_begin_millis_ = millis();
      }
      
    } else {

// EVENT_SWING - "Swing On" gesture control to allow fine tuning of speed needed to ignite
      if (millis() - saber_off_time_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (swinging_ && fusor.swing_speed() < 100) {
          swinging_ = false;
        }
        if (!swinging_ && fusor.swing_speed() > 500) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
      }

// EVENT_THRUST - move forward or backward
      if (mss.y * mss.y + mss.z * mss.z < 16.0 &&
          mss.x > 14  &&
          fusor.swing_speed() < 150) {
        if (millis() - thrust_begin_millis_ > 30) {
          Event(BUTTON_NONE, EVENT_THRUST);
          thrust_begin_millis_ = millis();
        } 
      } else {
        thrust_begin_millis_ = millis();
      }
    }
  }

// SA22C Volume Menu
  void ChangeVolume(bool up) {
    if (up) {
      STDOUT.println("Volume up");
      if (dynamic_mixer.get_volume() < VOLUME) {
        dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1,
          dynamic_mixer.get_volume() + VOLUME * 0.10));
        beeper.Beep(0.5, 2000);
        STDOUT.print("Current Volume: ");
        STDOUT.println(dynamic_mixer.get_volume());
      }
      else {
        beeper.Beep(0.5, 3000);
      }
    } else {
      STDOUT.println("Volume Down");
      if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
        dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
          dynamic_mixer.get_volume() - VOLUME * 0.10));
        beeper.Beep(0.5, 2000);
        STDOUT.print("Current Volume: ");
        STDOUT.println(dynamic_mixer.get_volume());
      }
      else{
        beeper.Beep(0.5, 1000);
      }
    }
  }


  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        if (accel_.x < -0.15) {
          pointing_down_ = true;
        } else {
          pointing_down_ = false;
        }
        return true;



// 0-buttons code

#if NUM_BUTTONS == 0

// Turn Blade ON Swing
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
          battle_mode_ = true;
	  if(millis() > 3000) { 
          On();
          }
        return true;

// Turn Blade ON Thrust
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
          battle_mode_ = true;
	  if(millis() > 3000) { 
        if (fusor.angle1() <  M_PI / 4) {
          On();
          }
        }
        return true;

// Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
#ifndef DISABLE_COLOR_CHANGE
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            ToggleColorChangeMode();
            return true;
          }
#endif            
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (swinging_) return false;
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        auto_lockup_on_ = true;
        SaberBase::DoBeginLockup();
        return true;          
            
// Next Preset
      case EVENTID(BUTTON_NONE, EVENT_SHAKE, MODE_OFF):
        if (fusor.angle1() >  M_PI / 4) {
        next_preset();
        }
        return true;


// Activate Muted
      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
        if (millis() - activated_ < 500) {
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
          }
        }
        return true;

// Turn Blade OFF Button
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
 	saber_off_time_ = millis();
        swing_blast_ = false;
        battle_mode_ = false;
      }
        return true;

// Turn Blade OFF Twist
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            // Just exit color change mode.
            // Don't turn saber off.
            ToggleColorChangeMode();
            return true;
          }
#endif
        if (fusor.angle1() <  M_PI / 4) {
          Off();
 	saber_off_time_ = millis();
        swing_blast_ = false;
        battle_mode_ = false;
        }
      }
        return true;

// Force
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
        SaberBase::DoForce();
        return true;

// Color Change mode
#ifndef DISABLE_COLOR_CHANGE
      case EVENTID(BUTTON_NONE, EVENT_SHAKE, MODE_ON):
        if (fusor.angle1() >  M_PI / 4) {
        ToggleColorChangeMode();
        }
        break;
#endif
            
// Blaster Deflection
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
        swing_blast_ = false;
        SaberBase::DoBlast();
        return true;

// Push
      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
        if (millis() - last_push_ > 2000) {
          if (SFX_push) {
            hybrid_font.PlayCommon(&SFX_push);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
          last_push_ = millis();
        }
        return true;

// Lockup
      case EVENTID(BUTTON_AUX, EVENT_HELD, MODE_ON):
        if (!SaberBase::Lockup()) {
          if (pointing_down_) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          }
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Start or Stop Music Track
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF):
         if (fusor.angle1() >  M_PI / 4) {
        StartOrStopTrack();
        }
        return true; 



// 1-button code

#elif NUM_BUTTONS == 1

// Turn Blade ON and Volume Up
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
	if(millis() > 3000) { 
          if (!mode_volume_) {
      On();
    } else {
      ChangeVolume(true);
    }
        }
    return true;      
            
// Next Preset and Volume Down
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
    if (!mode_volume_) {
      next_preset();
    } else {
      ChangeVolume(false);
    }
    return true;

// Previous Preset
      case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_OFF):
        previous_preset();
        return true;

// Activate Muted
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
            On();
          }
        return true;

// Turn Blade OFF Button
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
 	saber_off_time_ = millis();
        swing_blast_ = false;
        battle_mode_ = false;
      }
        return true;

// Turn Blade OFF Twist
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            // Just exit color change mode.
            // Don't turn saber off.
            ToggleColorChangeMode();
            return true;
          }
#endif
        if (fusor.angle1() <  M_PI / 4) {
          Off();
 	saber_off_time_ = millis();
        swing_blast_ = false;
        battle_mode_ = false;
        }
      }
        return true;

// Force
      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON | BUTTON_POWER):
        SaberBase::DoForce();
        return true;

// Color Change mode
#ifndef DISABLE_COLOR_CHANGE
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
        ToggleColorChangeMode();
        return true;
#endif

// Blaster Deflection
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
        swing_blast_ = false;
        SaberBase::DoBlast();
        return true;

// Multi-Blaster Deflection mode
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_POWER):
        swing_blast_ = true;
        hybrid_font.SB_Effect(EFFECT_BLAST, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
        if (swing_blast_) {
          SaberBase::DoBlast();
        }
        return true;

// Battle Mode
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_POWER):
        if (!battle_mode_) {
          battle_mode_ = true;
	  if(millis() > 3000) { 
          On();
        }
          if (SFX_bmbegin) {
            hybrid_font.PlayCommon(&SFX_bmbegin);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
        } else {
          battle_mode_ = false;
          if (SFX_bmend) {
            hybrid_font.PlayCommon(&SFX_bmend);
          } else {
            beeper.Beep(0.5, 3000);
          }
        }
        return true;

// Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (swinging_) return false;
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        auto_lockup_on_ = true;
        SaberBase::DoBeginLockup();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (!swinging_) {
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
          auto_melt_on_ = true;
          SaberBase::DoBeginLockup();
        }
        return true;

// Push
      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
        if (millis() - last_push_ > 2000) {
          if (SFX_push) {
            hybrid_font.PlayCommon(&SFX_push);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
          last_push_ = millis();
        }
        return true;

// Lockup
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          if (pointing_down_) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          }
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Lightning Block
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_ON):
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
        swing_blast_ = false;
        SaberBase::DoBeginLockup();
        return true;

// Melt
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Start or Stop Music Track
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        StartOrStopTrack();
        return true;

// Enter Volume Menu
  case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
    if (!mode_volume_) {
      mode_volume_ = true;
      beeper.Beep(0.1, 2000);
      beeper.Beep(0.1, 2500);
      STDOUT.println("Enter Volume Menu");
    } else {
      mode_volume_ = false;
      beeper.Beep(0.1, 2500);
      beeper.Beep(0.1, 2000);
      STDOUT.println("Exit Volume Menu");
    }
    return true;

// Battery level
  case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
    talkie.SayDigit((int)floorf(battery_monitor.battery()));
    talkie.Say(spPOINT);
    talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
    talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
    talkie.Say(spVOLTS);
    return true;



// 2-button code

#elif NUM_BUTTONS == 2

// Turn Blade ON and Volume Up
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
	if(millis() > 3000) { 
          if (!mode_volume_) {
      On();
    } else {
      ChangeVolume(true);
    }
        }
    return true;

// Next Preset and Volume Down
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF):
    if (!mode_volume_) {
      next_preset();
    } else {
      ChangeVolume(false);
    }
    return true;

// Previous Preset
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
        previous_preset();
        return true;

// Activate Muted
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
            On();
          }
        return true;

// Turn Blade OFF Button
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
 	saber_off_time_ = millis();
        swing_blast_ = false;
        battle_mode_ = false;
      }
        return true;

// Turn Blade OFF Twist
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            // Just exit color change mode.
            // Don't turn saber off.
            ToggleColorChangeMode();
            return true;
          }
#endif
        if (fusor.angle1() <  M_PI / 4) {
          Off();
 	saber_off_time_ = millis();
        swing_blast_ = false;
        battle_mode_ = false;
        }
      }
        return true;

// Force
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
        SaberBase::DoForce();
        return true;

// Color Change mode
#ifndef DISABLE_COLOR_CHANGE
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
        ToggleColorChangeMode();
        return true;
#endif

// Blaster Deflection
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
        swing_blast_ = false;
        SaberBase::DoBlast();
        return true;

// Multi-Blaster Deflection mode
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_POWER):
        swing_blast_ = true;
        hybrid_font.SB_Effect(EFFECT_BLAST, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
        if (swing_blast_) {
          SaberBase::DoBlast();
        }
        return true;

// Battle Mode
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_AUX):
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_POWER):
        if (!battle_mode_) {
          battle_mode_ = true;
	  if(millis() > 3000) { 
          On();
        }
          if (SFX_bmbegin) {
            hybrid_font.PlayCommon(&SFX_bmbegin);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
        } else {
          battle_mode_ = false;
          if (SFX_bmend) {
            hybrid_font.PlayCommon(&SFX_bmend);
          } else {
            beeper.Beep(0.5, 3000);
          }
        }
        return true;

// Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (swinging_) return false;
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        auto_lockup_on_ = true;
        SaberBase::DoBeginLockup();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
        if (!battle_mode_) return false;
        clash_impact_millis_ = millis();
        swing_blast_ = false;
        if (!swinging_) {
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
          auto_melt_on_ = true;
          SaberBase::DoBeginLockup();
        }
        return true;

// Push
      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
        if (millis() - last_push_ > 2000) {
          if (SFX_push) {
            hybrid_font.PlayCommon(&SFX_push);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
          last_push_ = millis();
        }
        return true;

// Lockup
      case EVENTID(BUTTON_AUX, EVENT_HELD, MODE_ON):
        if (!SaberBase::Lockup()) {
          if (pointing_down_) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          }
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Lightning Block
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
        swing_blast_ = false;
        SaberBase::DoBeginLockup();
        return true;

// Melt
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Start or Stop Music Track
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        StartOrStopTrack();
        return true;


#else
#error only 0, 1 and 2 buttons are supported by this mod
#endif

// Enter Volume Menu
  case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
    if (!mode_volume_) {
      mode_volume_ = true;
      beeper.Beep(0.1, 2000);
      beeper.Beep(0.1, 2500);
      STDOUT.println("Enter Volume Menu");
    } else {
      mode_volume_ = false;
      beeper.Beep(0.1, 2500);
      beeper.Beep(0.1, 2000);
      STDOUT.println("Exit Volume Menu");
    }
    return true;

// Battery level
  case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_OFF):
    talkie.SayDigit((int)floorf(battery_monitor.battery()));
    talkie.Say(spPOINT);
    talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
    talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
    talkie.Say(spVOLTS);
    return true;

#ifdef BLADE_DETECT_PIN
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = true;
        FindBladeAgain();
        SaberBase::DoBladeDetect(true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = false;
        FindBladeAgain();
        SaberBase::DoBladeDetect(false);
        return true;
#endif

      case EVENTID(BUTTON_AUX2, EVENT_CLICK_SHORT, MODE_OFF):
#ifdef DUAL_POWER_BUTTONS
        next_preset();
#else
        previous_preset();
#endif
        return true;

// Events that needs to be handled regardless of what other buttons are pressed.
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          swing_blast_ = false;
          return true;
        }
    }
    return false;
  }

  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      case EFFECT_POWERSAVE:
        if (SFX_dim) {
          hybrid_font.PlayCommon(&SFX_dim);
        } else {
          beeper.Beep(0.5, 3000);
        }
        return;
      case EFFECT_BATTERY_LEVEL:
        if (SFX_battery) {
          hybrid_font.PlayCommon(&SFX_battery);
        } else {
          beeper.Beep(0.5, 3000);
        }
        return;
      case EFFECT_FAST_ON:
        if (SFX_faston) {
          hybrid_font.PlayCommon(&SFX_faston);
        }
        return;
    }
  }

private:
  bool aux_on_ = true;
  bool pointing_down_ = false;
  bool swing_blast_ = false;
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
  bool battle_mode_ = false;
  bool mode_volume_ = false;
  uint32_t thrust_begin_millis_ = millis();
  uint32_t push_begin_millis_ = millis();
  uint32_t clash_impact_millis_ = millis();
  uint32_t last_twist_ = millis();
  uint32_t last_push_ = millis();
  uint32_t saber_off_time_ = millis();
};

#endif
