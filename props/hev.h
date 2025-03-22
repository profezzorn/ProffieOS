#ifndef PROPS_HEV_H
#define PROPS_HEV_H
#define PROP_TYPE Hev

// Half-Life Hazardous Environment Suit Prop
//
// How to use:
// Power button:
//   Long-click: on/off
//   double-click: sound track on/off
//   triple-click: next preset
// AUX button:
//   hold button to heal.
//   single click: deactivate hazard
//   double-click: armor readout
//   triple-click: previous preset
// Clash will cause varying damage based on strength
//
// Damage and Hazards are only active when prop is on.

#include "prop_base.h"
#include <cmath>

EFFECT(health);
EFFECT(armor);
EFFECT(stun);
EFFECT(death);
EFFECT(armor_alarm);
EFFECT(armor_depleted);
EFFECT(major);
EFFECT(minor);
EFFECT(morphine);

class Hev : public PROP_INHERIT_PREFIX PropBase {
public:
  Hev() : PropBase() {}
  const char* name() override { return "Hev"; }

  int health_ = 100;
  int armor_ = 100;

  bool dead_ = false;
  bool armor_depleted_ = false;
  bool morphine_triggered_ = false;

  enum Hazard {
    HAZARD_NONE = 0,
    HAZARD_BIO = 1,
    HAZARD_RAD = 2,
    HAZARD_BLO = 3,
    HAZARD_CHE = 4,
    HAZARD_HEA = 5,
    HAZARD_SHO = 6,
  };

  Hazard current_hazard_ = HAZARD_NONE;
  
  void LowBatteryOff() override {
    if (SFX_poweron) {
      PropBase::LowBatteryOff();
    }
  }


  // Calculate Physical Damage //////////////////////////////////////////////////
  void DoDamage(int damage, bool quiet = false) {
    int tens = health_ / 10;
    if (armor_ >= damage) {
        armor_ -= ceilf(damage * 0.80 / 2);  // Applies 80% divided by 2 of damage to armor, if armor is enough.
        health_ -= ceilf(damage * 0.20);  // Applies 20% of damage to health, if armor is enough.
    } else {
        int excess_physical = damage - armor_;
        health_ -= excess_physical;  // Only the excess of physical damage impacts health.
        armor_ = 0;  // Armor is depleted.
    }

    // Enforce minimum values
    if (health_ < 0) health_ = 0;
    if (armor_ < 0) armor_ = 0;

    // Damage
    if (!quiet) SaberBase::DoEffect(EFFECT_STUN, 0.0, damage);
    
    // Death Sound
    if (health_ == 0 && !dead_) { // Check flag to avoid death sound spam
      SaberBase::DoEffect(EFFECT_EMPTY, 0.0, 100);
      dead_ = true;
      return;
    }

    // Reset flag if health is above 0
    if (health_ > 0) {
      dead_ = false;
    }

    // Health Alert - only plays when health enters a new multiple of 10
    int new_tens = health_ / 10;
    if (tens != new_tens) {
      SaberBase::DoEffect(EFFECT_USER1, 0.0, 0);
    }

    // Print health and armor
    PVLOG_NORMAL << "Health: " << health_ << " / ";
    PVLOG_NORMAL << "Armor: " << armor_ << "\n";
  }


  // Armor Readout /////////////////////////////////////////////////////////////
  void armor_readout() {
    PVLOG_NORMAL << "Current Armor: " << armor_ << "\n"; // Debug logging
    SFX_armor.SelectFloat(armor_ / 100.0);
    hybrid_font.PlayCommon(&SFX_armor);
  }


  // Clashes ///////////////////////////////////////////////////////////////////
  uint32_t last_clash_time_ = 0;
  const uint32_t CLASH_DEBOUNCE_MS = 100; // Adjust debounce time as needed

  void Clash(bool stab, float strength) override {
    // Ignore Clash if within debounce period
    uint32_t current_time = millis();
    if (current_time - last_clash_time_ < CLASH_DEBOUNCE_MS) {
      return;
    }
    last_clash_time_ = current_time;

    // Stop Clashes if health is 0
    if (health_ == 0) {
      return;
    }

     // Strength multiplier
    int damage = (int)(strength * 4);

    // Play Clash sounds based on strength
    float v = (strength - GetCurrentClashThreshold()) / 3;
    SFX_clash.SelectFloat(v);
    SFX_clsh.SelectFloat(v);
    SFX_stab.SelectFloat(v);

    // Cap damage at 50
    if (damage > 50) {
      damage = 50;
    }

    // Play Major or Minor Detected Voice Lines
    // if (damage >= 25) {
    //   morphine_triggered_ = true;
    //   SaberBase::DoEffect(EFFECT_MAJOR, 0.0, damage);
    // } else {
    //   SaberBase::DoEffect(EFFECT_MINOR, 0.0, damage);
    // }

    // Play Armor Alarm if damage is 30 or more
    if (damage >= 30) {
      hybrid_font.PlayPolyphonic(&SFX_armor_alarm);
    }

    // Apply Physical Damage and print to log
    DoDamage(damage, true);
    PVLOG_NORMAL << "Physical Damage: -" << damage << "\n";

    // No stabs!
    PropBase::Clash(false, strength);
  }


  // Swings do nothing! ////////////////////////////////////////////////////
  void DoMotion(const Vec3& motion, bool clear) override {
    PropBase::DoMotion(Vec3(0), clear);
  }


  // Random Hazards ////////////////////////////////////////////////////////////
  uint32_t last_random_draw_ = millis();

  // Activate a Random Hazard, only if health is above 0 and no Hazard is currently active.
  void CheckRandomEvent() {
    if (millis() - last_random_draw_ > HEV_RANDOM_EVENT_INTERVAL_MS && health_ > 0 && current_hazard_ == HAZARD_NONE) {
      last_random_draw_ = millis();
      if (random(100) < HEV_RANDOM_HAZARD_CHANCE) {
	PVLOG_NORMAL << "Activating hazard.\n";
	current_hazard_ = (Hazard)(1 + random(6));
  hazard_decrease_millis_ = millis();
	SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);
      }
    }
  }

  // Decrease armor (if any) or health over time.
  uint32_t hazard_decrease_millis_ = millis();
  void HazardDecrease() {
    if (current_hazard_ != HAZARD_NONE) {
      if (millis() - hazard_decrease_millis_ > HEV_HAZARD_DECREASE_MS) {
        hazard_decrease_millis_ = millis();

        // Play stun sounds if there is armor
        if (armor_ > 0) {
          armor_--;
          SaberBase::DoEffect(EFFECT_STUN, 0.0, 1);
        } else {
          DoDamage(1);
        }

        // Stop the current Hazard if health drops to 0
        if (health_ == 0) {
          current_hazard_ = HAZARD_NONE;
          SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);
        
        // Print health and armor
        PVLOG_NORMAL << "Health: " << health_ << " / ";
        PVLOG_NORMAL << "Armor: " << armor_ << "\n";
        }
      }
    }
  }


  // Healing ///////////////////////////////////////////////////////////////////
  // Increase health (if healing).
  uint32_t health_increase_millis_ = millis();
  void IncreaseHealth() {
    if (SaberBase::Lockup()) {
      if (millis() - health_increase_millis_ > HEV_HEALTH_INCREASE_MS) {
	health_increase_millis_ = millis();
	health_++;
	PVLOG_NORMAL << "Health: " << health_ << "\n";
	if (health_ >= 100) {
	  health_ = 100;
	  SaberBase::DoEndLockup();
	  SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
	}
      }
    }
  }


  // Recharging ////////////////////////////////////////////////////////////////
  // Increase armor (if recharging).
  uint32_t armor_increase_millis_ = millis();
  void IncreaseArmor() {
    if (SaberBase::Lockup()) {
      if (millis() - armor_increase_millis_ > HEV_ARMOR_INCREASE_MS) {
        armor_increase_millis_ = millis();
        armor_++;
        PVLOG_NORMAL << "Armor: " << armor_ << "\n";
        if (armor_ >= 100) {
          armor_ = 100;
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        }
      }
    }
  }
  

  // Main Loop //////////////////////////////////////////////////////////////////
  void Loop() override {
    CheckRandomEvent();
    HazardDecrease();
    if (SaberBase::Lockup() == SaberBase::LOCKUP_NORMAL) {
      IncreaseHealth();
    } else if (SaberBase::Lockup() == SaberBase::LOCKUP_LIGHTNING_BLOCK) {
      IncreaseArmor();
    }
    PropBase::Loop();
  }


  // Button Events /////////////////////////////////////////////////////////////
  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

      // On/Off long-click
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
	On();
	return true;
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON):
	Off();
	return true;

	// short-click AUX to clear hazard
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_OFF):
	if (current_hazard_) {
	  current_hazard_ = HAZARD_NONE;
	  SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);
	  return true;
	}
	// Play a no-hazard sound ?
	return true;

	// Double-click power to start/stop track.
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_OFF):
        StartOrStopTrack();
        return true;
	
	// Double-click AUX for Armor Readout.
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_OFF):
        armor_readout();
        return true;
	
      // Next/Previous preset. Triple-click on either button.
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_OFF):
        next_preset();
        return true;
      case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ANY_BUTTON | MODE_OFF):
        previous_preset();
        return true;


	// Hold AUX to start healing
      case EVENTID(BUTTON_AUX, EVENT_HELD_MEDIUM, MODE_ON):
	if (!SaberBase::Lockup()) {
	  SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
	  SaberBase::DoBeginLockup();
	  return true;
	}
	break;
	// Release AUX to stop healing (or wait until full).
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
	if (SaberBase::Lockup()) {
	  SaberBase::DoEndLockup();
	  SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
	  return true;
	}
	break;

  // Hold POWER to start recharging armor
      case EVENTID(BUTTON_POWER, EVENT_HELD_MEDIUM, MODE_ON):
    if (!SaberBase::Lockup()) {
      SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
      SaberBase::DoBeginLockup();
      return true;
    }
    break;
    // Release POWER to stop recharging armor (or wait until full).
        case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
    if (SaberBase::Lockup()) {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      return true;
    }
    break;
	
	
		    
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
    }
    return false;
  }


   // Hev effects, auto fire is handled by begin/end lockup
  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;

  // Damage Detection Voice Lines
      // case EFFECT_MAJOR:
      // hybrid_font.PlayCommon(&SFX_major);
      // if (morphine_triggered_) {
      //   hybrid_font.PlayCommon(&SFX_morphine);
      //   morphine_triggered_ = false;
      // }
      // return;

      // case EFFECT_MINOR:
      // hybrid_font.PlayCommon(&SFX_minor);
      // return;
  
	// Random Hazard Sounds
      case EFFECT_STUN:
	hybrid_font.PlayCommon(&SFX_stun);
	return;
	
	// Health Alert
      case EFFECT_USER1:
  if (dead_) return; // Don't queue health sounds if dead
  SFX_health.SelectFloat(health_ / 100.0);
  SOUNDQ->Play(&SFX_health);
  return;
	
	// Death Sound
      case EFFECT_EMPTY:
	hybrid_font.PlayCommon(&SFX_death);
	return;
  
    }
  }
};

#endif

