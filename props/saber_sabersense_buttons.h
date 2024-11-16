/*
============================================================
=================== SABERSENSE PROP FILE ===================
============================================================

Built on SA22C programming by Matthew McGeary.
Modified by Chris Carter with help and contributions 
from Brian Connor and Fredrik Hubinette.
November 2024, first edition.


============================================================
=============== MODIFICATION AND LOGIC NOTES =============== 

The Sabersense prop file has been engineered for harmonized 
controls between one-button and two-button operation without 
compromising the inevitable increase in user-friendliness 
of two-button operation.
Where practicable, the same controls apply to both states, 
with two-button operation benefitting from some extra 
features and controls that are more user-friendly.
Where possible without causing conflicts, some one-button 
controls are duplicated in two-button mode, despite there 
also being different two-button controls for the same function. 
The overall intention is that users need only remember a 
minimum set of control principles in order to access 
all functions. As such, the logic is that the same button 
presses do the same thing within the various states, 
with obvious variants.

Hence:
ONE AND TWO BUTTON:
	Single click MAIN always lights the blade...
		Short click lights blade with sound
		Long click lights blade mute
	
	Double click MAIN always plays a sound file...
		Character Quote or Music track with blade OFF
		Character Quote or Force Effect with blade ON
		
	Holding down MAIN (1-button) or AUX (2-button) 
	and waiting with blade OFF always skips to specific preset...
		Hilt pointing up - first preset
		Hilt horizontal - middle preset
		Hilt pointing down - last preset
		
	Triple-clicking MAIN always enters a control menu
		Colour change with blade ON
		BladeID with blade OFF

TWO BUTTON ONLY:	
	Short clicking AUX with blade OFF always moves to a different preset, 
	forward with hilt pointing up, backwards with hilt pointing down...
		Single short click - one preset
		Double short click - five presets
		Triple short click - ten presets
	
	Holding MAIN and short clicking AUX always enters a control menu...
		Colour change with blade ON
		Volume menu with blade OFF


==========================================================		
=================== 1 BUTTON CONTROLS ==================== 

MAIN FUNCTIONS
Activate blade			Short click while OFF.
Activate blade mute   	Long click while OFF (hold for one second then release). 
Deactivate blade		Hold and wait until blade is off while ON.

FUNCTIONS WITH BLADE OFF
Next preset	           Double click and hold while OFF pointing blade tip up.
Previous preset		   Double click and hold while OFF pointing blade tip down.
Skip to first preset   Press and hold until it switches, hilt pointing upwards.
Skip to middle preset  Press and hold until it switches, hilt horizontal.
Skip to last preset    Press and hold until it switches, hilt pointing downwards.
Play Character Quote   Fast double-click, hilt pointing up. Quotes play sequentially.
							To reset sequential quotes, change font or run BladeID.
Play Music Track	   Fast double-click, hilt pointing down.
Speak battery voltage  Fast four clicks while OFF.
Run BladeID Manually   Fast triple-click while OFF. (Applicable installs only, toggles arrays).
Enter/Exit VOLUME MENU Hold and clash while OFF.
Volume up               	Click MAIN while in VOLUME MENU, hilt pointing up.
Volume down					Click MAIN while in VOLUME MENU, hilt pointing down.
						   	Volume adjusts in increments per click.
						   	You must exit VOLUME MENU to resume using lightsaber normally.

FUNCTIONS WITH BLADE ON		
Blade lockup			Hold and hit clash while ON.
Blade tip drag			Hold and hit clash while ON pointing the blade tip down.
Force Effect (Quote)	Fast double-click MAIN, hilt pointing down.
							Plays random Force effect (usually character quote).
Character Quote			Fast double-click MAIN, hilt pointing up.
							Plays sequential character quote.
Lightning block			Double click and hold while ON.
Melt         		    Hold and thrust forward clash while ON. (Selected fonts only).
Blaster blocks			Short click/double click/triple click while ON.
Enter multi-blast mode		Hold while swinging for one second and release.
							To trigger blaster block, swing saber while in multi-blast mode.
							To exit, hold while swinging for one second and release.

COLOUR CHANGE FUNCTIONS WITH BLADE ON
Enter COLOUR MENU		Fast triple-click while ON.
							Announcement confirms you are in the COLOUR MENU. 
Cycle to next colour    Rotate hilt whilst in COLOUR MENU until desired colour is reached.
Exit COLOUR MENU		Fast triple-click or hold and wait. 
							Announcement confirms you are exiting COLOUR MENU.
                        	You must exit COLOUR MENU to resume using lightsaber normally.


============================================================
===================== 2 BUTTON CONTROLS ====================

MAIN FUNCTIONS
Activate blade			Short click MAIN.
Activate blade mute   	Long click MAIN (hold for one second then release). 
Deactivate blade		Press and hold MAIN and wait until blade is off.

FUNCTIONS WITH BLADE OFF
Next preset				Short click AUX, hilt pointing upwards.
Previous preset			Short click AUX, hilt pointing downwards.
Previous preset			Hold AUX and short click MAIN.
							(Duplicate command for backwards compatibility).
Skip to first preset    Press and hold any button until it switches, hilt upwards.
Skip to middle preset   Press and hold any button  until it switches, hilt horizontal.
Skip to last preset     Press and hold any button  until it switches, hilt downwards.
Skip forward 5 presets	Fast double-click AUX, hilt pointing upwards.
Skip back 5 presets		Fast double-click AUX, hilt pointing downwards.
Skip forward 10 presets	Fast triple-click AUX, hilt pointing upwards.
Skip back 10 presets	Fast triple-click AUX, hilt pointing downwards.
Play Character Quote	Fast double-click MAIN, pointing up. Quotes play sequentially.
							To reset sequential quotes, change font or run BladeID.
Play Music Track	    Fast double-click MAIN, pointing down.
Speak battery voltage	Fast four clicks MAIN.
Run/Toggle BladeID		Fast triple-click MAIN. (Applicable installs only).
Enter/Exit VOLUME MENU 	Hold MAIN then quickly click AUX
						and release both buttons simultaneously.
Volume up               	Click MAIN while in VOLUME MENU, hilt pointing up.
Volume down					Click MAIN while in VOLUME MENU, hilt pointing down, or
							click AUX while in VOLUME MENU.
							Volume adjusts in increments per click.
							You must exit VOLUME MENU to resume using saber normally.
							
FUNCTIONS WITH BLADE ON
Blade lockup			Press and hold AUX.
Blade tip drag			Press and hold AUX while blade is pointing down.
Force Effect (Quote)	Fast double-click MAIN, hilt pointing down.
							Plays random Force effect (usually character quote).
Character Quote			Fast double-click MAIN, hilt pointing up.
							Plays sequential character quote.
Lightning block			Double-click MAIN and hold.
Melt         			Hold MAIN and push blade tip against wall (clash).
Blaster blocks			Short click AUX.
Enter multi-blast mode	Hold MAIN while swinging for one second and release. 
							Saber will play two quick blasts confirming mode.
							Swing blade to trigger blaster block.
							To exit multi-blast mode, fast single click AUX.

COLOUR CHANGE FUNCTIONS WITH BLADE ON
Enter/Exit COLOUR MENU	Hold MAIN then quickly click AUX and release both
						buttons simultaneously. Or fast triple-click MAIN.
							Announcement confirms you are in the COLOUR MENU.
Cycle to next colour    Rotate hilt whilst in COLOUR MENU until desired colour is reached.
							Most presets have 12 colour options.
Alt Exit COLOUR MENU	Hold MAIN and wait. 
							Announcement confirms you are exiting COLOUR MENU.
							You must exit COLOUR MENU to resume using lightsaber normally.

							
===========================================================		
========================= DEFINES =========================
#define SABERSENSE_FAKE_BLADE_ID
Replaces regular BladeID and allows toggling between two different 
blade/preset arrays regardless of actual BladeID status.

#define SABERSENSE_NO_LOCKUP_HOLD	
Applicable to two-button mode only, reverts to lockup being triggered
by clash while holding aux.

GESTURE CONTROLS
There are four gesture types: Twist, Stab, Swing and Thrust.
Gesture controls bypass all preon effects.
Below are the options to add to the config to enable the various gestures. MM.
#define SABERSENSE_TWIST_ON
#define SABERSENSE_TWIST_OFF
#define SABERSENSE_STAB_ON
#define SABERSENSE_SWING_ON
#define SABERSENSE_THRUST_ON
#define SABERSENSE_FORCE_PUSH

#define SABERSENSE_FORCE_PUSH_LENGTH 5
Allows for adjustment to Push gesture length in millis needed to trigger Force Push
Recommended range 1 ~ 10, 1 = shortest, easiest to trigger, 10 = longest. MM.

Tightened click timings
I've shortened the timeout for short and double click detection from 500ms
to 300ms.  I think it feels more responsive this way but still gives enough
timeout to ensure all button actions can be achieved consistently
I've included all button timings so they can be easily tweaked to suit
individual tastes. MM.

============================================================
============================================================
*/
#ifndef PROPS_SABER_SABERSENSE_BUTTONS_H
#define PROPS_SABER_SABERSENSE_BUTTONS_H


//  Used instead of BladeID and allows toggle switching 
//  between two blade/preset arrays manually irrespective
//  of actual BladeID status.
	#ifdef SABERSENSE_FAKE_BLADE_ID
		struct SabersenseFakeBladeID {
   		static int return_value;
   		float id() { return return_value; }
   		static void toggle() {
     return_value = NO_BLADE - return_value;
   	}
	};
		int SabersenseFakeBladeID::return_value = 0;
		#undef BLADE_ID_CLASS_INTERNAL
		#define BLADE_ID_CLASS_INTERNAL SabersenseFakeBladeID
		#undef BLADE_ID_CLASS
		#define BLADE_ID_CLASS SabersenseFakeBladeID
	#endif


#include "prop_base.h"
#include "../sound/hybrid_font.h"

#undef PROP_TYPE
#define PROP_TYPE SabersenseButtons

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 15 * 1000
#endif

#ifndef SABERSENSE_SWING_ON_SPEED
#define SABERSENSE_SWING_ON_SPEED 250
#endif

#ifndef SABERSENSE_LOCKUP_DELAY
#define SABERSENSE_LOCKUP_DELAY 200
#endif

#ifndef SABERSENSE_FORCE_PUSH_LENGTH
#define SABERSENSE_FORCE_PUSH_LENGTH 5
#endif

#ifndef BUTTON_DOUBLE_CLICK_TIMEOUT
#define BUTTON_DOUBLE_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_SHORT_CLICK_TIMEOUT
#define BUTTON_SHORT_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_TIMEOUT
#define BUTTON_HELD_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_MEDIUM_TIMEOUT
#define BUTTON_HELD_MEDIUM_TIMEOUT 1000
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif

#ifdef SABERSENSE_SWING_ON
#define SABERSENSE_SWING_GESTURE
#endif

#ifdef SABERSENSE_STAB_ON
#define SABERSENSE_STAB_GESTURE
#endif

#ifdef SABERSENSE_TWIST_ON
#define SABERSENSE_TWIST_GESTURE
#endif

#ifdef SABERSENSE_THRUST_ON
#define SABERSENSE_THRUST_GESTURE
#endif

#ifdef SABERSENSE_FAKE_BLADE_ID
#define SABERSENSE_FAKE_BLADE_ID
#endif

//  #define FORCE_PUSH_CONDITION battle_mode_

EFFECT(dim);      // for EFFECT_POWERSAVE
EFFECT(battery);  // for EFFECT_BATTERY_LEVEL
EFFECT(bmbegin);  // for Begin Battle Mode
EFFECT(bmend);    // for End Battle Mode
EFFECT(vmbegin);  // for Begin Volume Menu
EFFECT(vmend);    // for End Volume Menu
EFFECT(volup);    // for increse volume
EFFECT(voldown);  // for decrease volume
EFFECT(volmin);   // for minimum volume reached
EFFECT(volmax);   // for maximum volume reached
EFFECT(faston);   // for EFFECT_FAST_ON
EFFECT(blstbgn);  // for Begin Multi-Blast
EFFECT(blstend);  // for End Multi-Blast
EFFECT(push);     // for Force Push gesture in Battle Mode
EFFECT(quote);    // for playing quotes



// The Saber class implements the basic states and actions
// for the saber.
class SabersenseButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  SabersenseButtons() : PropBase() {}
  const char* name() override { return "SabersenseButtons"; }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    Vec3 mss = fusor.mss();
    if (SaberBase::IsOn()) {
      DetectSwing();
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > SABERSENSE_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > SABERSENSE_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }

      // EVENT_PUSH
      if (fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 70 &&
          fusor.swing_speed() < 30 &&
          fabs(fusor.gyro().x) < 10) {
        if (millis() - push_begin_millis_ > SABERSENSE_FORCE_PUSH_LENGTH) {
          Event(BUTTON_NONE, EVENT_PUSH);
          push_begin_millis_ = millis();
        }
      } else {
        push_begin_millis_ = millis();
      }

    } else {
      // EVENT_SWING - Swing On gesture control to allow fine tuning of speed needed to ignite
      if (millis() - saber_off_time_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (swinging_ && fusor.swing_speed() < 90) {
          swinging_ = false;
        }
        if (!swinging_ && fusor.swing_speed() > SABERSENSE_SWING_ON_SPEED) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
      }
      // EVENT_THRUST
      if (mss.y * mss.y + mss.z * mss.z < 16.0 &&
          mss.x > 14  &&
          fusor.swing_speed() < 150) {
        if (millis() - thrust_begin_millis_ > 15) {
          Event(BUTTON_NONE, EVENT_THRUST);
          thrust_begin_millis_ = millis();
        }
      } else {
        thrust_begin_millis_ = millis();
      }
    }
  }

// Volume Menu
  void VolumeUp() {
    STDOUT.println("Volume up");
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1,
        dynamic_mixer.get_volume() + VOLUME * 0.10));
      if (SFX_volup) {
        hybrid_font.PlayCommon(&SFX_volup);
      } else {
        beeper.Beep(0.5, 2000);
      }
      STDOUT.print("Volume Up - Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      // Cycle through ends of Volume Menu option
      #ifdef VOLUME_MENU_CYCLE
        if (!max_vol_reached_) {
          if (SFX_volmax) {
            hybrid_font.PlayCommon(&SFX_volmax);
          } else {
            beeper.Beep(0.5, 3000);
          }
          STDOUT.print("Maximum Volume: ");
          max_vol_reached_ = true;
        } else {
          dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
          dynamic_mixer.get_volume() - VOLUME * 0.90));
          if (SFX_volmin) {
            hybrid_font.PlayCommon(&SFX_volmin);
          } else {
            beeper.Beep(0.5, 1000);
          }
          STDOUT.print("Minimum Volume: ");
          max_vol_reached_ = false;
        }
      #else
        if (SFX_volmax) {
          hybrid_font.PlayCommon(&SFX_volmax);
        } else {
          beeper.Beep(0.5, 3000);
        }
        STDOUT.print("Maximum Volume: ");
      #endif
    }
  }

  void VolumeDown() {
    STDOUT.println("Volume Down");
    if (dynamic_mixer.get_volume() > (0.10 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
        dynamic_mixer.get_volume() - VOLUME * 0.10));
      if (SFX_voldown) {
        hybrid_font.PlayCommon(&SFX_voldown);
      } else {
        beeper.Beep(0.5, 2000);
      }
      STDOUT.print("Volume Down - Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    } else {
      #ifdef VOLUME_MENU_CYCLE
        if (!min_vol_reached_) {
          if (SFX_volmin) {
            hybrid_font.PlayCommon(&SFX_volmin);
          } else {
            beeper.Beep(0.5, 1000);
          }
          STDOUT.print("Minimum Volume: ");
          min_vol_reached_ = true;
        } else {
          dynamic_mixer.set_volume(VOLUME);
          if (SFX_volmax) {
            hybrid_font.PlayCommon(&SFX_volmax);
          } else {
            beeper.Beep(0.5, 3000);
          }
          STDOUT.print("Maximum Volume: ");
          min_vol_reached_ = false;
        }
      #else
        if (SFX_volmin) {
          hybrid_font.PlayCommon(&SFX_volmin);
        } else {
          beeper.Beep(0.5, 1000);
        }
        STDOUT.print("Minimum Volume: ");
      #endif
      
    }
  }


//  Button Clicker to play press/release wav files when buttons are pressed.
//  Intended for Scavenger hilt where wheel makes tactile feel difficult.
//  Requires press.wav and release.wav files to work.
    void PlaySound(const char* sound) {
        RefPtr<BufferedWavPlayer> player = GetFreeWavPlayer();
        if (player) {
            if (!player->PlayInCurrentDir(sound)) player->Play(sound);
        }
    }            
 bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {      
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ANY_BUTTON | MODE_OFF):
        SaberBase::RequestMotion();
        PlaySound("press.wav");
        return false;
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
        PlaySound("release.wav");
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          return true;
        } else {
          return false;
        }
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
        if (accel_.x < -0.15) {
          pointing_down_ = true;
        } else {
          pointing_down_ = false;
        }
    	return true;
   

//  Gesture Controls
	#ifdef SABERSENSE_SWING_ON
  		case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
    		// Due to motion chip startup on boot creating false ignition we delay Swing On at boot for 3000ms
   	 		if (millis() > 3000) {
    			FastOn();
				//  On();
    		}
    	return true;
	#endif


	#ifdef SABERSENSE_TWIST_ON
  		case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
    		// Delay twist events to prevent false trigger from over twisting
    		if (millis() - last_twist_ > 2000 &&
        	millis() - saber_off_time_ > 1000) {
    			FastOn();
				//  On();
      		last_twist_ = millis();
    		}
    	return true;
	#endif


	#ifdef SABERSENSE_TWIST_OFF
  		case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
   			// Delay twist events to prevent false trigger from over twisting
    		if (millis() - last_twist_ > 3000) {
      		Off();
      			last_twist_ = millis();
      			saber_off_time_ = millis();
      			battle_mode_ = false;
    		}
    	return true;
	#endif


	#ifdef SABERSENSE_STAB_ON
      	case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        	if (millis() - saber_off_time_ > 1000) {
				FastOn();
				//  On();
        	}
        return true;
	#endif


	#ifdef SABERSENSE_THRUST_ON
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        	if (millis() - saber_off_time_ > 1000) {
    			FastOn();
				//  On();
        	}
        return true;
	#endif


	#ifdef SABERSENSE_FORCE_PUSH
      	case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
          if (FORCE_PUSH_CONDITION &&
            millis() - last_push_ > 2000) {
          if (SFX_push) {
            hybrid_font.PlayCommon(&SFX_push);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
          	last_push_ = millis();
          }
        return true;
	#endif


//  Multiple Skips only available with 2 button installs.
//  Skips forward five fonts if pointing up, skips back five fonts if pointing down.
	case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
    		// backwards if pointing down
    		SetPreset(current_preset_.preset_num + (fusor.angle1() < -M_PI / 4 ? -5 : 5), true);
		#ifdef SAVE_PRESET
    		SaveState(current_preset_.preset_num);
		#endif
    return true;

 
// Skips forward ten fonts if pointing up, skips back ten fonts if pointing down.
	case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
    		// backwards if pointing down
    		SetPreset(current_preset_.preset_num + (fusor.angle1() < -M_PI / 4 ? -10 : 10), true);
		#ifdef SAVE_PRESET
    		SaveState(current_preset_.preset_num);
		#endif
    return true;

   
// Saber ON AND Volume Adjust.
	case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
    	if (!mode_volume_) {
      		On();
    	} else {
     	if (fusor.angle1() > 0) {
      		VolumeUp();
    	} else {
      		VolumeDown();
    	}
    	}
    return true;


// Modified 2 button 'next/previous preset' AND volume down,
// to align with multiple skips above.
// Forward one font pointing up, back one font pointing down.
	case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
    	// backwards if pointing down
    	if (!mode_volume_) {
    		SetPreset(current_preset_.preset_num + (fusor.angle1() < -M_PI / 4 ? -1 : 1), true);
        } else {
      		VolumeDown();
    	}
		#ifdef SAVE_PRESET
    		SaveState(current_preset_.preset_num);
		#endif
	return true;


// 1 button 'next/previous preset' command.
// Forward one font pointing up, back one font pointing down.
	#if NUM_BUTTONS == 1 
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_OFF):
    	// backwards if pointing down
    	if (!mode_volume_) {
    		SetPreset(current_preset_.preset_num + (fusor.angle1() < -M_PI / 4 ? -1 : 1), true);
        } else {
      		VolumeDown();
    	}
		#ifdef SAVE_PRESET
    		SaveState(current_preset_.preset_num);
		#endif
    	return true;
	#endif


//  1 Button skips to first preset (up) or last preset (down)
//  or middle preset if horizontal:
		#if NUM_BUTTONS == 2
			case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_OFF):
		#endif
			case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF):
			#define DEGREES_TO_RADIANS (M_PI / 180)
 				if (fusor.angle1() > 45 * DEGREES_TO_RADIANS) {
					// If pointing up
					SetPreset(0, true);
 				} else if (fusor.angle1() < -45 * DEGREES_TO_RADIANS) {
					// If pointing down
				SetPreset(-1, true);
 				} else {
					// If horizontal
					CurrentPreset tmp;
					tmp.SetPreset(-1);
					SetPreset(tmp.preset_num / 2, true);
				}
			#ifdef SAVE_PRESET
    				SaveState(current_preset_.preset_num);
			#endif
    return true;


// 1 and 2 button: Previous Preset, retained legacy control.
	#if NUM_BUTTONS == 2
		case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
	#endif
    	if (!mode_volume_) {
      	previous_preset();
   	 }
    return true;


//  Skips to first preset (up) or last (battery or charge) preset (down)
//  or middle preset if horizontal:
	#if NUM_BUTTONS == 2
	// 2 button: First Preset
		case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF):
	#endif
		#define DEGREES_TO_RADIANS (M_PI / 180)
 		if (fusor.angle1() > 45 * DEGREES_TO_RADIANS) {
			// If pointing up
			SetPreset(0, true);
		} else if (fusor.angle1() < -45 * DEGREES_TO_RADIANS) {
			// If pointing down
			SetPreset(-1, true);
 		} else {
			// If horizontal
			CurrentPreset tmp;
			tmp.SetPreset(-1);
			SetPreset(tmp.preset_num / 2, true);
		}
		#ifdef SAVE_PRESET
    		SaveState(current_preset_.preset_num);
		#endif
    return true;
    

//  Manual Blade ID Options
	case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
		#ifdef FAKE_BLADE_ID
		//  Toggles between blade arrays regardless of BladeID status.
		//  Cannot use more than two blade/preset arrays.
      		SabersenseFakeBladeID::toggle();
      		FindBladeAgain();
      		SaberBase::DoBladeDetect(true);
        	return true;
		#else
		//  Runs BladeID Manually to actually check BladeID status.
		//  Won't change arrays unless status tells it to (i.e. Data/Neg resistance changes).
		//  Can use any number of blade/preset arrays.
    		FindBladeAgain();
    		SaberBase::DoBladeDetect(true);
			return true;
		#endif

 
// Activate Muted
	//  case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_OFF):
    case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
    	if (SetMute(true)) {
      		unmute_on_deactivation_ = true;
      	On();
    	}
    return true;


// Turn Blade OFF
#if NUM_BUTTONS > 1
	// 2 button
		case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
 	#else
	// 1 button
  		case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_ON):
	#endif
    	if (!SaberBase::Lockup()) {
		#ifndef DISABLE_COLOR_CHANGE
      	if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
        	// Just exit color change mode.
        	// Don't turn saber off.
        	ToggleColorChangeMode();
        return true;
      	}
	#endif
      	if (!battle_mode_) {
       		Off();
      		}
    	}
    	saber_off_time_ = millis();
    	battle_mode_ = false;
    	swing_blast_ = false;
    return true;


// Character Quote and Force Effect, Blade ON.
// Hilt pointed UP for Character Quote, plays sequentially,
// Hilt pointed DOWN for Force Effect, plays randomly.
	case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
    	if (SFX_quote) {
     		if (fusor.angle1() > 0) {
            	SFX_quote.SelectNext();
          		SaberBase::DoEffect(EFFECT_QUOTE, 0);
        	} else {
          		SaberBase::DoForce();
        	}
       	}
	return true;


//  Character Quote and Music Track, Blade OFF.
//  Play sequential quote pointing up, play music track pointing down.
case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):  
   if (SFX_quote) {
     if (fusor.angle1() > 0) {
            SFX_quote.SelectNext();
          SaberBase::DoEffect(EFFECT_QUOTE, 0);
    } else {
    StartOrStopTrack();
    }
    }
    return true;
    

//  Colour Change.
	//  1 and 2 button modes.
	#ifndef DISABLE_COLOR_CHANGE
		case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
      		ToggleColorChangeMode();
    	return true;

	//  2 button mode only.
		#if NUM_BUTTONS == 2
			case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
      			ToggleColorChangeMode();
    		return true;
		#endif
	#endif


// Blaster Deflection
	#if NUM_BUTTONS == 1
  		// 1 button
  		case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
		//  case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
		//  case EVENTID(BUTTON_POWER, EVENT_THIRD_CLICK_SHORT, MODE_ON):
	#else
  		// 2 button
  		case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
  		case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
		//  case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
		//  case EVENTID(BUTTON_AUX, EVENT_THIRD_CLICK_SHORT, MODE_ON):
	#endif
    	swing_blast_ = false;
    	SaberBase::DoBlast();
    return true;


// Multi-Blaster Deflection mode
	case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_POWER):
    	swing_blast_ = !swing_blast_;
    	if (swing_blast_) {
      		if (SFX_blstbgn) {
        		hybrid_font.PlayCommon(&SFX_blstbgn);
      		} else {
        		hybrid_font.SB_Effect(EFFECT_BLAST, 0);
      		}
    	} else {
      		if (SFX_blstend) {
        	hybrid_font.PlayCommon(&SFX_blstend);
      	} else {
        	hybrid_font.SB_Effect(EFFECT_BLAST, 0);
      		}
    	}
    return true;

	case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
    	if (swing_blast_) {
      		SaberBase::DoBlast();
    	}
  	return true;


// Lockup
	#if NUM_BUTTONS == 1
  		// 1 button lockup
  		case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
	#else
		#ifndef SABERSENSE_NO_LOCKUP_HOLD
  		// 2 button lockup
  		case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD, MODE_ON):
	#else
  		case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
	#endif
	#endif
    	if (!SaberBase::Lockup() && !battle_mode_) {
    	if (accel_.x < -0.15) {
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
  	// 1 and 2 button
	case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_ON):
		if (!battle_mode_) {
      		SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
      		swing_blast_ = false;
      		SaberBase::DoBeginLockup();
      	return true;
    	}
  	break;


// Melt
  	case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
    	if (!SaberBase::Lockup() && !battle_mode_) {
      		SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
      		swing_blast_ = false;
      		SaberBase::DoBeginLockup();
      	return true;
    	}
    break;

	//  case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
	//  case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_OFF):
  	case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_OFF):
    	SaberBase::RequestMotion();
    return true;


// Enter Volume MENU
	#if NUM_BUTTONS == 1
  		// 1 button
  		case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
	#else
  		// 2 button
  		case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_POWER):
		//  case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
	#endif
    	if (!mode_volume_) {
      		mode_volume_ = true;
      	if (SFX_vmbegin) {
        	hybrid_font.PlayCommon(&SFX_vmbegin);
      	} else {
        	beeper.Beep(0.5, 3000);
      	}
      		STDOUT.println("Enter Volume Menu");
    	} else {
      		mode_volume_ = false;
      	if (SFX_vmend) {
        	hybrid_font.PlayCommon(&SFX_vmend);
      	} else {
        	beeper.Beep(0.5, 3000);
      	}
      STDOUT.println("Exit Volume Menu");
    }
    return true;


// Battery level
	case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):
	//  case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF):
    	talkie.SayDigit((int)floorf(battery_monitor.battery()));
    	talkie.Say(spPOINT);
    	talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
    	talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
    	talkie.Say(spVOLTS);
    	SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);        
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


// Events that needs to be handled regardless of what other buttons
// are pressed.
	//  case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
	//  case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
    case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      	if (SaberBase::Lockup()) {
        	SaberBase::DoEndLockup();
        	SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        return true;
      }
    }
    return false;
  }



	void SB_Effect(EffectType effect, float location) override {
    	switch (effect) {
      
      	case EFFECT_QUOTE: hybrid_font.PlayCommon(&SFX_quote); return;
      
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

      default: break; // avoids compiler warning
    }
  }

private:
  	bool pointing_down_ = false;
  	bool swing_blast_ = false;
  	bool mode_volume_ = false;
 	bool auto_lockup_on_ = false;
  	bool auto_melt_on_ = false;
  	bool battle_mode_ = false;
  	bool max_vol_reached_ = false;
  	bool min_vol_reached_ = false;
  	uint32_t thrust_begin_millis_ = millis();
  	uint32_t push_begin_millis_ = millis();
  	uint32_t clash_impact_millis_ = millis();
  	uint32_t last_twist_ = millis();
  	uint32_t last_push_ = millis();
  	uint32_t saber_off_time_ = millis();
	};

#endif
