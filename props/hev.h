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
// In alt01 to alt06, place the hazard detection voice lines in the altchng dir
// (bio, blood toxins, chemical, radiation, shock, fire).
// In alt01 to alt06, place appropriate sound effects in the stun dir
// (spark, burn, geiger counter).
// In each stun folder, use the same amount of files as the other stun folders.
// So if you use 4 stun sounds, use 4 in all of them.
// In each altchng folder, use the same amount of files as the other altchng folders.
// I use 2 in all of them.
// One for the main voice line, the other is the same,
// but with a follow up health notification for variety.
// For alt00, have the same amount of files as the other alt** folders
// but leave the files blank.
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

struct HEVTimerBase {
  uint32_t start_ = 0;

  // Time between applying damage. HEV_HAZARD_DECREASE_MS
  uint32_t interval_ = 0;

  bool active_ = false;

  void reset() { active_ = false; }
  
  void start() { active_ = true; start_ = millis(); }

  void configure(uint32_t interval) {
    interval_ = interval;
  }

  // Returns true if timer is inactive or timeout exceeded.
  bool check() {
    return !active_ || (millis() - start_ > interval_);
  }

  // Returns true if timer is active and interval has elapsed.
  bool ready() {
    return active_ && ((millis() - start_) >= interval_);
  }
};

struct HEVTimerHazard : public HEVTimerBase {
  // next_tick_ ensures precise timing for damage events by storing the exact
  // timestamp when the next damage should occur, preventing timing drift.
  // Mainly used for hazard damage, controlled by HEV_HAZARD_DECREASE_MS.
  uint32_t next_tick_ = 0;
  // Delay before applying damage. HEV_HAZARD_DELAY_MS
  uint32_t delay_ = 0;

  void reset() {
    HEVTimerBase::reset();
    next_tick_ = 0; 
  }

  void start() {
    HEVTimerBase::start();
    next_tick_ = start_; 
  }

  void configure(uint32_t interval, uint32_t delay_time = 0) {
    HEVTimerBase::configure(interval);
    delay_ = delay_time;
  }

  // delay_ is a grace period. It's argument is HEV_HAZARD_DELAY_MS.
  // interval_ uses HEV_HAZARD_DECREASE_MS as an argument. Currently 1000ms.
  //
  // The hazard sequence method returns true when:
  // - The timer is active (active_ is true)
  // - The grace period has elapsed (now - start_ >= delay_)
  // - It's time for the next damage tick (now >= next_tick_)
  //
  // It returns false when:
  // - The timer is not active (active_ is false)
  // - The warning period hasn't elapsed yet
  // - It's not time for the next damage tick
  bool hazard_sequence() {
    if (!active_) return false;
    uint32_t now = millis();
    
    return (now - start_ >= delay_) &&
           ((now >= next_tick_) ? (next_tick_ = now + interval_, true) : false);
  }
};

class Hev : public PROP_INHERIT_PREFIX PropBase {
public:

  HEVTimerBase timer_clash_;
  HEVTimerBase timer_post_death_cooldown_;
  HEVTimerBase timer_random_event_;
  HEVTimerBase timer_hazard_damage_;
  HEVTimerBase timer_health_increase_;
  HEVTimerBase timer_armor_increase_;
  HEVTimerHazard timer_hazard_delay_;

  Hev() : PropBase() {
    // Configure all timers with their respective timings from hev_config.h
    // Clash timer uses clash_timeout_ from PropBase for debounce
    timer_clash_.configure(this->clash_timeout_);
    timer_post_death_cooldown_.configure(HEV_POST_DEATH_COOLDOWN_MS);
    timer_random_event_.configure(HEV_RANDOM_EVENT_INTERVAL_MS);
    timer_hazard_delay_.configure(HEV_HAZARD_DECREASE_MS, HEV_HAZARD_DELAY_MS);
    timer_health_increase_.configure(HEV_HEALTH_INCREASE_MS);
    timer_armor_increase_.configure(HEV_ARMOR_INCREASE_MS);
  }

  const char* name() override { return "Hev"; }

  int health_ = 100;
  int armor_ = 100;

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
    // Don't process clashes if dead or during cooldown.
    // HEV Suit clash detection uses a fundamentally different approach than PropBase.
    // The two work together: timer tracks state, timeout defines duration.
    //
    //  1. PropBase uses direct timestamp comparison:
    //     - Stores last_clash_ timestamp and clash_timeout_ value.
    //     - Compares (millis() - last_clash_ < clash_timeout_) on each check.
    //     - Simple but limited to basic cooldown functionality.
    // 
    //  2. HEV uses object-oriented HEVTimer system:
    //     - timer_clash_ encapsulates timer state (active flag, start time).
    //     - clash_timeout_ provides the duration value.
    //     - Enables more complex patterns like sequences and ready-state checking.
    // 
    // This is advantageous because:
    // - HEV's sophisticated hit categorization needs finer state control.
    // - Consistent with other HEV timers (health_timer_, armor_timer_, etc.)
    // - Provides clear separation between "is a timer running?" and "how long should it run?".
    // - Allows more complex timing patterns needed for authentic HEV behavior.
    // - Supports future extensions like variable cooldowns based on hit type.
    if (health_ == 0 || (timer_clash_.active_ && !timer_clash_.check())) {
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
    timer_clash_.start();
  }

  // Swings do nothing!
  void DoMotion(const Vec3& motion, bool clear) override {
    PropBase::DoMotion(Vec3(0), clear);
  }

  // Random Hazards
  void CheckRandomEvent() {
    // Skip Hazard check if dead or during post-death cooldown
    if (health_ == 0 || !timer_post_death_cooldown_.check()) {
      return;
    }

    // Initialize timer. Stops immediate Hazard at boot
    if (!timer_random_event_.active_) {
      timer_random_event_.start();
      return;
    }

    // Check for new Hazard if timer expired and no current Hazard
    if (timer_random_event_.check() && current_hazard_ == HAZARD_NONE) {
            
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
      timer_hazard_delay_.reset();
      return;
    }

    // Start sequence if not running
    if (!timer_hazard_delay_.active_) {
      timer_hazard_delay_.start();
      return;
    }

    // Check sequence and apply damage
    if (timer_hazard_delay_.hazard_sequence()) {
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
        timer_hazard_delay_.reset();
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

    if (!SaberBase::Lockup() || !timer_health_increase_.ready()) {
      return;
    }

    if (health_ == 0) {
      timer_post_death_cooldown_.start();
    }

    health_++;
    timer_health_increase_.start();
    PVLOG_NORMAL << "Health: " << health_ << "\n";
  }

  // Increase armor (Hold POWER).
  void IncreaseArmor() {
    if (armor_ >= 100) {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      return;
    }

    if (!SaberBase::Lockup() || !timer_armor_increase_.ready()) {
      return;
    }

    armor_++;
    timer_armor_increase_.start();
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
          timer_random_event_.reset();
          timer_random_event_.start();
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
          timer_health_increase_.start();
          return true;
        }
        break;

      // Release AUX to stop healing (or wait until full).
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          timer_health_increase_.reset();
          return true;
        }
        break;

      // Hold POWER to start recharging armor
      case EVENTID(BUTTON_POWER, EVENT_HELD_MEDIUM, MODE_ON):
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
          SaberBase::DoBeginLockup();
          timer_armor_increase_.start();
          return true;
        }
        break;

      // Release POWER to stop recharging armor (or wait until full).
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_OFF):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          timer_armor_increase_.reset();
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
