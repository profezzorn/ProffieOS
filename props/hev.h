// HALF-LIFE - Hazardous Environment Suit Prop
//
// How to use:
// POWER button:
//   Long-click: ON/OFF
//   hold button to recharge armor
//   double-click: toggle track
//   triple-click: next preset
// AUX button:
//   hold button to heal.
//   single click: deactivate hazard
//   double-click: armor readout
//   triple-click: previous preset
//
// Sound files needed:
// in.wav, out.wav - for power on/off. Use flashlight sound
// bgnlb.wav, lb.wav, endlb.wave - recharge armor sound sequence
// bgnlock.wav, lock.wav, endlock.wav - heal sound sequence
// font.wav - next/previous preset
// armor_alarm.wav - for when Clash damage is greater than or equal to 30
// armor_compromised.wav - for when armor falls to 0
// boot.wav - Bootup welcome message
// death.wav - for when health is 0
// clash**.wav - physical clash sounds
// health00.wav to health100.wav - for health alert sounds
// armor00.wav to armor100.wav - for armor readout sounds
//
// Random Hazard sounds:
// These are played using the altchng method. So create 7 directories:
// alt00, alt01, alt02, alt03, alt04, alt05, alt06.
// Each directory should contain two folders, altchng and stun.
// In alt01 to alt06, place the hazard detection voice lines (bio, blood toxins, chemical, radiation, shock, fire) in altchng.
// In alt01 to alt06, place appropriate sound effects (spark, burn, geiger counter) in stun.
// In each stun folder, use the same amount of files as the other stun folders. So if you use 4 stun sounds, use 4 in all of them.
// In each altchng folder, use the same amount of files as the other altchng folders. I use 2 in all of them.
// One for the main voice line, the other is the same, but with a follow up health notification for variety.
// For alt00, have the same amount of files as the other alt** folders but leave the files blank.
//
// Notes:
// Clash will cause varying damage based on strength
// Damage and Hazards are only active when prop is ON.

#ifndef PROPS_HEV_H
#define PROPS_HEV_H
#define PROP_TYPE Hev

#include "prop_base.h"
#include <cmath>

EFFECT(health);
EFFECT(armor);
EFFECT(stun);
EFFECT(death);
EFFECT(armor_alarm);
EFFECT(armor_compromised);
EFFECT(major);
EFFECT(minor);
EFFECT(morphine);

struct HEVTimer {
  uint32_t start_;
  bool active_ = false;

  void reset() { active_ = false; }
  void start() { active_ = true; start_ = millis(); }
  bool expired(uint32_t timeout) const {
    if (!active_) return false;
    return (millis() - start_ > timeout);
  }
  bool check(uint32_t timeout) {
    if (!active_) return false;
    if (millis() - start_ > timeout) {
      reset();
      return true;
    }
    return false;
  }
};

class Hev : public PROP_INHERIT_PREFIX PropBase {
public:
  Hev() : PropBase() {}
  const char* name() override { return "Hev"; }

  int health_ = 100;
  int armor_ = 100;

  HEVTimer clash_timer_;
  HEVTimer random_event_timer_;
  HEVTimer post_death_cooldown_timer_;
  HEVTimer hazard_delay_timer_;
  HEVTimer hazard_damage_timer_;
  HEVTimer health_increase_timer_;
  HEVTimer armor_increase_timer_;

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

  // Calculate Physical Damage
  void DoDamage(int damage, bool quiet = false) {
    int tens = health_ / 10;
    if (armor_ >= damage) {
      armor_ -= ceilf(damage * 0.80 / 2);
      health_ -= ceilf(damage * 0.20);
    } else if (armor_ > 0) {
      int excess_physical = damage - armor_;
      health_ -= excess_physical;
      // Make armor compromised sound play before setting armor to 0
      SaberBase::DoEffect(EFFECT_USER2, 0.0, 0);
      PVLOG_NORMAL << "Armor Compromised!\n";
      armor_ = 0;
    } else {
      health_ -= damage;
    }

    // Enforce minimum values
    if (health_ < 0) health_ = 0;
    if (armor_ < 0) armor_ = 0;

    // Damage
    if (!quiet) SaberBase::DoEffect(EFFECT_STUN, 0.0, damage);
    
    // Death Sound
    if (health_ == 0) {
      SaberBase::DoEffect(EFFECT_EMPTY, 0.0, 100);
      return;
    }
    
    // Health Alert - only plays when health enters a new multiple of 10
    int new_tens = health_ / 10;
    if (tens != new_tens) {
      SaberBase::DoEffect(EFFECT_USER1, 0.0, 0);
    }

    // Print Damage, Health and Armor
    PVLOG_NORMAL << "DAMAGE: -" << damage << "\n";
    PVLOG_NORMAL << "HEALTH: " << health_ << " / ";
    PVLOG_NORMAL << "ARMOR: " << armor_ << "\n";
  }

  // Armor Readout
  void armor_readout() {
    PVLOG_NORMAL << "Current Armor: " << armor_ << "\n"; // Debug logging
    SFX_armor.SelectFloat(armor_ / 100.0);
    hybrid_font.PlayCommon(&SFX_armor);
  }

  // Clashes
  void Clash(bool stab, float strength) override {
    // Check and reset expired timer first
    if (clash_timer_.active_ && clash_timer_.expired(HEV_CLASH_DEBOUNCE_MS)) {
      clash_timer_.reset();
    }

    // Skip if dead or within debounce period
    if (health_ == 0 || clash_timer_.active_) {
      return;
    }

    // Start new debounce period
    clash_timer_.start();

    // Damage is based on strength, capped at 50
    int damage = std::min((int)(strength * 4), 50);

    // Play Clash sounds based on strength
    float v = (strength - GetCurrentClashThreshold()) / 3;
    SFX_clash.SelectFloat(v);
    SFX_clsh.SelectFloat(v);
    SFX_stab.SelectFloat(v);

    // Play Armor Alarm if Damage is 30 or more
    if (damage >= 30) {
      hybrid_font.PlayPolyphonic(&SFX_armor_alarm);
    }

    // Apply Damage and forward Clash event. No Stabs!
    DoDamage(damage, true);
    PropBase::Clash(false, strength);
  }

  // Swings do nothing!
  void DoMotion(const Vec3& motion, bool clear) override {
    PropBase::DoMotion(Vec3(0), clear);
  }

  // Random Hazards
  uint32_t last_random_draw_ = millis();
  uint32_t post_death_cooldown_ = 0;

  // Check and activation
  void CheckRandomEvent() {
    // Don't allow Hazards if dead or during post-death cooldown.
    // This prevents Hazards immediately after healing.
    if (health_ == 0 || (post_death_cooldown_ &&
      (millis() - post_death_cooldown_ < HEV_POST_DEATH_COOLDOWN_MS))) {
      return;
    }

    // Activate a Hazard, only if health is above 0 and no Hazard is currently active.
    if (millis() - last_random_draw_ > HEV_RANDOM_EVENT_INTERVAL_MS &&
      health_ > 0 &&
      current_hazard_ == HAZARD_NONE) {
      last_random_draw_ = millis();
      if (random(100) < HEV_RANDOM_HAZARD_CHANCE) {
        PVLOG_NORMAL << "Activating hazard.\n";
        current_hazard_ = (Hazard)(1 + random(6));
        hazard_decrease_millis_ = millis();
        hazard_start_delay_ = 0;
        SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);
      }
    }
  }

  // Hazard Damage. Decrease armor (if any) or health over time.
  uint32_t hazard_start_delay_ = 0;
  uint32_t hazard_decrease_millis_ = millis();
  void HazardDecrease() {
    if (current_hazard_ == HAZARD_NONE) {
      hazard_start_delay_ = 0;
      return;
    }

    // Check if this is a new hazard that needs initial delay
    if (hazard_start_delay_ == 0) {
      hazard_start_delay_ = millis();
      return;
    }

    // Wait for initial delay before starting damage
    if (millis() - hazard_start_delay_ < HEV_HAZARD_DELAY_MS) {
      return;
    }

    // Normal damage interval after initial delay
    if (millis() - hazard_decrease_millis_ > HEV_HAZARD_DECREASE_MS) {
      hazard_decrease_millis_ = millis();

      // Play stun sounds if there is armor
      if (armor_ > 0) {
        armor_--;
        SaberBase::DoEffect(EFFECT_STUN, 0.0, 1);
        // Check if armor is 0 then play armor compromised sound.
        if (armor_ == 0) {
          SaberBase::DoEffect(EFFECT_USER2, 0.0, 0);
          PVLOG_NORMAL << "Armor Compromised!\n";
        }
      } else {
        DoDamage(1);
      }

      // Stop the current Hazard if health drops to 0
      if (health_ == 0) {
        current_hazard_ = HAZARD_NONE;
        hazard_start_delay_ = 0; // Reset delay for next hazard
        SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);
      }
    }
  }

  // Increase health (Hold AUX).
  uint32_t health_increase_millis_ = millis();
  void IncreaseHealth() {
    if (SaberBase::Lockup()) {
      if (millis() - health_increase_millis_ > HEV_HEALTH_INCREASE_MS) {
        health_increase_millis_ = millis();

        // Start cooldown timer when health increases above 0.
        // This starts the check to prevent immediate Hazards after healing.
        if (health_ == 0) {
          post_death_cooldown_ = millis(); // Start cooldown timer
        }

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

  // Increase armor (Hold POWER).
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

  // Main Loop
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

  // Button Events
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
    // Don't queue new sounds if dead (except death sound).
    // Once dead, if a queued sound is currently playing, allow it to finish
    // alongside death sound. However all pending sounds should be cleared.
    switch (effect) {
      default: return;
  
    // Random Hazard Sounds
      case EFFECT_STUN:
        hybrid_font.PlayCommon(&SFX_stun);
        return;

    // Armor Compromised Sound
      case EFFECT_USER2:
        SOUNDQ->Play(&SFX_armor_compromised);
        return;
    
    // Health Alert
      case EFFECT_USER1:
        if (health_ == 0) return; // Don't queue health sounds if dead
        SFX_health.SelectFloat(health_ / 100.0);
        SOUNDQ->Play(&SFX_health);
        return;
    
    // Death Sound
      case EFFECT_EMPTY:
        if (health_ == 0) {
          SOUNDQ->clear_pending();
        }
        hybrid_font.PlayCommon(&SFX_death);
        return;
    }
  }
};

#endif
