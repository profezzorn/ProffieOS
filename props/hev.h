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
// fuzz_high.wav, fuzz_low.wav - for pre-armor readout notification sound
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
EFFECT(fuzz);

struct HEVTimer {
  uint32_t start_ = 0;

  // next_tick_ ensures precise timing for damage events by storing the exact
  // timestamp when the next damage should occur, preventing timing drift.
  // Mainly used for hazard damage, controlled by HEV_HAZARD_DECREASE_MS.
  uint32_t next_tick_ = 0;

  bool active_ = false;

  void reset() { active_ = false; next_tick_ = 0; }
  
  void start() { active_ = true; start_ = next_tick_ = millis(); }

  // Returns true if timer is inactive or timeout exceeded
  bool check(uint32_t timeout) {
    return !active_ || (millis() - start_ > timeout);
  }

  // Manages hazard warning and damage sequence timing
  bool hazard_sequence(uint32_t warning_period, uint32_t tick_interval) {
    if (!active_) return false;
    uint32_t now = millis();
    
    return (now - start_ >= warning_period) && 
           ((now >= next_tick_) ? (next_tick_ = now + tick_interval, true) : false);
  }

  // Returns true if timer is active and interval has elapsed
  bool ready(uint32_t interval) {
    return active_ && ((millis() - start_) >= interval);
  }

  // Adds a non-blocking delay, can be used to wait for a specific duration
  void delay(uint32_t ms) {
  uint32_t end_time = millis() + ms;
  while (millis() < end_time) {
    SaberBase::DoEffect(EFFECT_NONE, 0);
    }
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
    PVLOG_NORMAL << "Current Armor: " << armor_ << "\n";
    
    // Play random "fuzz" sound only if armor is above 0
    if (armor_ > 0) {
      hybrid_font.PlayCommon(&SFX_fuzz);

      // Non-blocking delay to ensure fuzz finishes before armor readout plays
      HEVTimer().delay(475);
    }

    // Play armor value
    SFX_armor.SelectFloat(armor_ / 100.0);
    hybrid_font.PlayCommon(&SFX_armor);
  }

  // Clashes
  void Clash(bool stab, float strength) override {
    // Don't allow Clashes if dead.
    if (health_ == 0) {
      return;
    }

    // This code implements a cooldown period between clash events.
    // If the clash timer is currently active AND we haven't reached 
    // the timeout period specified by PropBase's clash_timeout_, then
    // we return early, preventing the processing of another clash event.
    // This prevents multiple clash detections from a single impact by
    // ensuring sufficient time passes between registered clashes.
    if (clash_timer_.active_ && !clash_timer_.check(this->clash_timeout_)) {
      return;
    }
    
    // Forward Clash event. No Stabs!
    PropBase::Clash(false, strength);

    // Damage is based on strength, capped at 50
    int damage = std::min((int)(strength * 4), 50);
    float v = (strength - GetCurrentClashThreshold()) / 3;

    // Play Clash sounds based on strength
    SFX_clash.SelectFloat(v);
    SFX_clsh.SelectFloat(v);
    SFX_stab.SelectFloat(v);

    // Play Armor Alarm if Damage is 30 or more
    if (damage >= 30) {
      hybrid_font.PlayPolyphonic(&SFX_armor_alarm);
    }

    // Apply Damage and restart clash timer.
    DoDamage(damage, true);
    clash_timer_.start();
  }

  // Swings do nothing!
  void DoMotion(const Vec3& motion, bool clear) override {
    PropBase::DoMotion(Vec3(0), clear);
  }

  // Random Hazards
  void CheckRandomEvent() {
    // Skip Hazard check if dead or during post-death cooldown
    if (health_ == 0 || !post_death_cooldown_timer_.check(HEV_POST_DEATH_COOLDOWN_MS)) {
      return;
    }

    // Initialize timer. Stops immediate Hazard at boot
    if (!random_event_timer_.active_) {
      random_event_timer_.start();
      return;
    }

    // Check for new Hazard if timer expired and no current Hazard
    if (random_event_timer_.check(HEV_RANDOM_EVENT_INTERVAL_MS) && 
      current_hazard_ == HAZARD_NONE) {
            
      // Roll for Random Hazard
      if (random(100) < HEV_RANDOM_HAZARD_CHANCE) {
        PVLOG_NORMAL << "Activating hazard.\n";
        current_hazard_ = (Hazard)(1 + random(6));
        SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);
      }
    }
  }

  // Hazard Damage. Decrease armor (if any) or health over time.
  void HazardDecrease() {
    // Reset timer when hazard is cleared
    if (current_hazard_ == HAZARD_NONE) {
      hazard_delay_timer_.reset();
      return;
    }

    // Start sequence if not running
    if (!hazard_delay_timer_.active_) {
      hazard_delay_timer_.start();
      return;
    }

    // Check sequence and apply damage
    if (hazard_delay_timer_.hazard_sequence(HEV_HAZARD_DELAY_MS, HEV_HAZARD_DECREASE_MS)) {
      if (armor_ > 0) {
        armor_--;
        SaberBase::DoEffect(EFFECT_STUN, 0.0, 1);
        if (armor_ == 0) {
          SaberBase::DoEffect(EFFECT_USER2, 0.0, 0);
          PVLOG_NORMAL << "Armor Compromised!\n";
        }
      } else {
        DoDamage(1);
      }

      // Clear hazard on death
      if (health_ == 0) {
        current_hazard_ = HAZARD_NONE;
        hazard_delay_timer_.reset();
        SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);
      }
    }
  }

  // Increase health (Hold AUX).
  void IncreaseHealth() {
    if (health_ >= 100) {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      return;
    }

    if (!SaberBase::Lockup() || !health_increase_timer_.ready(HEV_HEALTH_INCREASE_MS)) {
      return;
    }

    if (health_ == 0) {
      post_death_cooldown_timer_.start();
    }

    health_++;
    health_increase_timer_.start();
    PVLOG_NORMAL << "Health: " << health_ << "\n";
  }

  // Increase armor (Hold POWER).
  void IncreaseArmor() {
    if (armor_ >= 100) {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      return;
    }

    if (!SaberBase::Lockup() || !armor_increase_timer_.ready(HEV_ARMOR_INCREASE_MS)) {
      return;
    }

    armor_++;
    armor_increase_timer_.start();
    PVLOG_NORMAL << "Armor: " << armor_ << "\n";
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
          random_event_timer_.reset();
          random_event_timer_.start();
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
          health_increase_timer_.start();
          return true;
        }
        break;

      // Release AUX to stop healing (or wait until full).
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          health_increase_timer_.reset();
          return true;
        }
        break;

      // Hold POWER to start recharging armor
      case EVENTID(BUTTON_POWER, EVENT_HELD_MEDIUM, MODE_ON):
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
          SaberBase::DoBeginLockup();
          armor_increase_timer_.start();
          return true;
        }
        break;

      // Release POWER to stop recharging armor (or wait until full).
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          armor_increase_timer_.reset();
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
