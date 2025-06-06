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
// clsh**.wav - physical clash sounds
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
// Custom Clash Functionality:
// Upon a hit (clash), the user will take damage ranging from 1 - 50.
// If hit >= 25, a major hit is registered. Otherwise a minor hit is registered.
// Beyond the two major and minor types, there are Fracture and Laceration hit sfx categories.
// The hit sfx are in the clsh folder, they are named clshXX.wav from 01 - 08.
// The hit sfx system handles the following:
// Minor Laceration:
// Low intensity (index 0): clsh01.wav
// Medium intensity (index 1): clsh02.wav
// High intensity (index 2): clsh03.wav
// Minor Fracture (index 3): clsh04.wav
// Major Laceration:
// Low intensity (index 4): clsh05.wav
// Medium intensity (index 5): clsh06.wav
// High intensity (index 6): clsh07.wav
// Major Fracture (index 7): clsh08.wav
// It accounts for the amount of force applied to the board which determines the intensity of the hit sfx.
// Previous testing has produced two clash events and sounds, one being from the original prop_base and
// the 2nd from the new custom logic.
// Completely removing the prop_base functionality also removes the clash visual effects, so this has been kept in to some degree through saber_base.
// Testing styles will come later, but the current implementation is to create an interactive clash system that follows the damage system from HL
// while utilising the Proffieboard's existing clash functionality.
//
// Notes:
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
EFFECT(fuzz_high);
EFFECT(fuzz_low);

struct HEVTimer {
  uint32_t start_ = 0;
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
      if (random(2) == 0) {
          hybrid_font.PlayCommon(&SFX_fuzz_high);
      } else {
          hybrid_font.PlayCommon(&SFX_fuzz_low);
      }

      // Small delay to ensure fuzz finishes before armor readout plays.
      delay(475);
    }

    // Play armor value
    SFX_armor.SelectFloat(armor_ / 100.0);
    hybrid_font.PlayCommon(&SFX_armor);
  }

  // Clashes
  void Clash(bool stab, float strength) override {
    // Don't process clashes if dead or during cooldown
    if (health_ == 0 || (clash_timer_.active_ && !clash_timer_.check(this->clash_timeout_))) {
      return;
    }
    
    // Calculate normalized strength and damage
    float v = (strength - GetCurrentClashThreshold()) / 3;
    clash_normalized_strength_ = v;
    
    // Damage is based on strength, capped at 50
    int damage = std::min((int)(strength * 4), 50);
    last_clash_damage_ = damage;
    
    // Categorize hit type: Major vs Minor, Fracture vs Laceration  
    is_major_hit_ = (damage >= 25);
    is_fracture_hit_ = (random(100) < 25);
    
    // ***** KEY CHANGE: Don't use DoClash at all - handle everything directly ****
    // Play our clash sound based on hit type
    int sound_index = 0;
    
    if (is_fracture_hit_) {
      // Fractures: index 3 for minor, 7 for major
      sound_index = is_major_hit_ ? 7 : 3;
    } else {
      // Lacerations: scale intensity within category
      float intensity = constrain(v, 0.0f, 1.5f) / 1.5f;
      if (is_major_hit_) {
        // Major laceration: indices 4-6
        sound_index = 4 + min(2, (int)(intensity * 3));
      } else {
        // Minor laceration: indices 0-2
        sound_index = min(2, (int)(intensity * 3));
      }
    }
    
    // Play selected sound and trigger visual effects
    PVLOG_NORMAL << "DEBUG: Selected clash sound index: " << sound_index << "\n";
    SFX_clsh.Select(sound_index);
    hybrid_font.PlayCommon(&SFX_clsh);
    
    // Trigger the clash effect for visuals only, without the sound
    SaberBase::SetClashStrength(strength);
    clash_pending_ = false; // Prevents duplicate sounds
    
    // Log hit type information
    const char* intensity_level = "";
    if (!is_fracture_hit_) {
      if (sound_index == 0 || sound_index == 4) intensity_level = " (low intensity)";
      else if (sound_index == 1 || sound_index == 5) intensity_level = " (medium intensity)";
      else intensity_level = " (high intensity)";
    }
    
    PVLOG_NORMAL << "HIT: " << (is_major_hit_ ? "Major " : "Minor ") 
                 << (is_fracture_hit_ ? "Fracture" : "Laceration") 
                 << intensity_level
                 << " (damage: " << damage << ", strength: " << v << ")\n";
    
    // Play additional sounds
    if (random(100) < 40 && is_major_hit_) {
      // 40% chance of voice line for major hits
      hybrid_font.PlayCommon(is_fracture_hit_ ? &SFX_major : &SFX_minor);
    }
    
    // Alarm for serious hits
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
    // Skip clash processing - we handle it directly in Clash method
    if (effect == EFFECT_CLASH) {
      return;
    }
    
    // Process other effects
    switch (effect) {
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
        
      default:
        PropBase::SB_Effect(effect, location);
        return;
    }
  }

private:
  bool is_major_hit_ = false;
  bool is_fracture_hit_ = false;
  float clash_normalized_strength_ = 0.0;
  int last_clash_damage_ = 0;  // Store damage value for use in SB_Effect
};

#endif
