//=====================================================================//
//                        H A L F - L I F E                            //
//                      ─────────────────────                          //
//              Hazardous Environment Suit Diagnostics                 //
//                H.E.V. Mk V - Configuration Protocol                 //
//---------------------------------------------------------------------//
//                      PROPERTY OF BLACK MESA                         //
//=====================================================================//

//=====================================================================//
//                                                                     //
//                    H.E.V. SUIT - USAGE GUIDE                        //
//                    ─────────────────────────                        //
//                                                                     //
// - POWER Button:                                                     //
//     - Long-click           - ON/OFF                                 //
//     - Hold                 - Recharge Armor                         //
//     - Double-click         - Toggle track                           //
//     - Triple-click         - Next preset                            //
// - AUX Button:                                                       //
//     - Hold                 - Recharge Health                        //
//     - Single-click         - Deactivate Hazard                      //
//     - Double-click         - Armor Readout                          //
//     - Triple-click         - Previous preset                        //
//                                                                     //
//---------------------------------------------------------------------//
//              PHYSICAL DAMAGE & HAZARD DAMAGE LOGIC                  //
//---------------------------------------------------------------------//
//                                                                     //
// - The HEV suit has two main resources: Health and Armor.            //
//     - Both active only while the suit is ON.                        //
//     - Both have a maximum value of 100.                             //
// - Random Hazards:                                                   //
//     - Active only while the suit is ON.                             //
//     - Triggered at intervals with a chance-based system.            //
//     - Always damages Armor first, then Health.                      //
// - When Armor is depleted:                                           //
//     - Hazards begins directly damaging Health.                      //
//     - Clashes deal full damage to Health.                           //
// - When Armor is active:                                             //
//     - Clashes (physical impacts) are negated:                       //
//         ▪ 80% of the impact is absorbed by Armor.                   //
//         ▪ 20% is applied to Health.                                 //
// - Clash damage is based on force of impact:                         //
//     - Cannot exceed 50 total damage.                                //
//                                                                     //
//=====================================================================//

//=====================================================================//
//                                                                     //
//                     AUDIO SYSTEM CATEGORIES                         //
//                    ─────────────────────────                        //
//                                                                     //
//----------------------  HEV VOICE LINES -----------------------------//
//                                                                     //
// - armor**.wav             - Armor Readouts                          //
// - health**.wav            - Health Alerts                           //
// - armor_compromised.wav   - Plays when armor drops to 0             //
// - boot.wav                - HEV welcome message (shortened)         //
// - boot_long.wav           - HEV welcome message (original)          //
// - hazard**.wav            - Hazard Alerts                           //
// - morphine.wav            - Plays after a Major Clash               //
//                                                                     //
//-------------------- HEV UI SYSTEM SOUNDS ---------------------------//
//                                                                     //
// - armor_alarm.wav         - Short alert // plays when damage ≥ 30   //
// - battery.wav             - Battery SFX // armor pickup             //
// - bgnlb.wav               - Start armor charge                      //
// - bgnlock.wav             - Start health charge                     //
// - death.wav               - Heart monitor flatline // health = 0    //
// - endlb.wav               - End armor charge                        //
// - endlock.wav             - End health charge                       //
// - font.wav                - Weapon select SFX (placeholder)         //
// - fuzz**.wav              - Subtle alert // precedes armor**.wav    //
// - in.wav                  - Torch ON                                //
// - lb.wav                  - Looping armor charge                    //
// - lock.wav                - Looping health charge                   //
// - medkit.wav              - Medkit SFX // health pickup             //
// - out.wav                 - Torch OFF                               //
// - warning.wav             - No Armor SFX // Armor = 0               //
//                                                                     //
//-------------------- ENVIRONMENTAL EFFECTS --------------------------//
//                                                                     //
// - stun**.wav              - Hazard SFX // alt001–alt006 = hazards   //
// - clsh**.wav              - Physical impacts, slashes, collisions   //
//                                                                     //
//----------------------------- MISC ----------------------------------//
//                                                                     //
// - blank.wav               - Placeholder inside altchng/ folder      //
//                                                                     //
//------------------ Random Hazard Sounds Setup -----------------------//
//                                                                     //
// ▪ Create 7 directories: alt00 through alt06                         //
// ▪ Each alt**/ folder must contain:                                  //
//    ├── hazard00.wav                                                 //
//    └── hazard01.wav                                                 //
//      → These are the Hazard Alert (HEV VOICE LINES) for that folder.//
//                                                                     //
// ▪ Inside each alt**/ folder, create an altchng/ folder:             //
// ▪ Inside each altchng/ folder:                                      //
//    - Include one file: blank.wav                                    //
//      → A silent placeholder used to satisfy the folder structure.   //
// ▪ Inside each alt**/ folder, create a stun/ folder:                 //
// ▪ Inside each stun/ folder:                                         //
//    - Include the ENVIRONMENTAL EFFECTS that match                   //
//      the Hazard type (e.g., spark, burn, Geiger counter)            //
//                                                                     //
// ▪ All alt**/ folders (alt00–alt06) must have the **same number** of //
//    WAVs inside including **all** sub folders.                       //
//                                                                     //
//=====================================================================//

//=====================================================================//
//                       TIMER CONFIGURATION                           //
//                    ─────────────────────────                        //
//                                                                     //
// - timer_clash_               - Debounce timer for physical Clashes. //
//                                Prevents false Clashes.              //
// - timer_post_death_cooldown_ - Cooldown after user revives.         //
//                                Blocks Hazards until timer is done.  //
// - timer_random_event_        - Interval timer for Random Hazards.   //
//                                Controls how often Hazards can occur.//
// - timer_hazard_delay_        - Delay between Hazard damage ticks.   //
//                                Manages Hazard damage over time.     //
// - timer_health_increase_     - Interval for Health recharge.        //
//                                Controls healing rate.               //
// - timer_armor_increase_      - Interval for Armor recharge.         //
//                                Controls Armor recharge rate.        //
//=====================================================================//

//=====================================================================//
//                                                                     //
//                        BEHAVIOR BREAKDOWN                           //
//                    ─────────────────────────                        //
//                                                                     //
//------------------------- Health Alerts -----------------------------//
//                                                                     //
// ▪ When Health reaches the below thresholds, the suit will           //
//   randomly say different (HEV VOICE LINES):                         //
//     - ≤= 50: "Seek Medical Attention"        (50% chance)           //
//     - ≤= 30: "Vital Signs Critical"          (60% chance)           //
//     - ≤= 6:  "User Death Imminent"           (100% chance)          //
//                                                                     //
// ▪ The chances have been configured directly in the sound files,     //
//   with some wavs as blanks to ensure.                               //
// ▪ When Health drops to a lower 10th decimal (e.g., from 43 → 36),   //
//   a Health Alert (health**.wav) will play.                          //
// ▪ There are no Health Alerts when Health is above 50.               //
// ▪ When Health is 0, death.wav will trigger.                         //
//                                                                     //
//------------------------- Armor Alerts ------------------------------//
//                                                                     //
// ▪ When Clash damage ≥ 30:                                           //
//     - armor_alarm.wav will play. (HEV UI SOUND)                     //
// ▪ If Armor reaches 0:                                               //
//     - 50% chance to play armor_compromised.wav. (HEV VOICE LINE)    //
//                                                                     //
//-------------------- Physical Clash System --------------------------//
//                                                                     //
// ▪ Each Clash deals between 1–50 damage based on impact strength.    //
// ▪ Currently 8 (ENVIRONMENTAL) Clash sounds available (clsh**.wav):  //
//     → Divided by Clash Impact and Injury type.                      //
// ▪ Impact categorised by damage value:                               //
//     - < 25: Minor Clash sounds                                      //
//     - ≥ 25: Major Clash sounds                                      //
// ▪ Injury subcategories for both Impact types:                       //
//     - Lacerations                                                   //
//     - Fractures                                                     //
//                                                                     //
//-------------------- Clash Sound Index Table ------------------------//
//                                                                     //
// ▪ Minor Clash sounds:                                               //
//     - clsh00.wav  → Laceration (Low)                                //
//     - clsh01.wav  → Laceration (Med)                                //
//     - clsh02.wav  → Laceration (High)                               //
//     - clsh03.wav  → Fracture (Minor)                                //
// ▪ Major Clash sounds:                                               //
//     - clsh04.wav  → Laceration (Low)                                //
//     - clsh05.wav  → Laceration (Med)                                //
//     - clsh06.wav  → Laceration (High)                               //
//     - clsh07.wav  → Fracture (Major)                                //
//                                                                     //
// ▪ 44% chance to trigger a (HEV VOICE LINE):                         //
//     - e.g. ”Major Fracture Detected!"                               //
//                                                                     //
//-------------------- Morphine Auto-Injection ------------------------//
//                                                                     //
// ▪ Only if Major Clash (HEV VOICE LINE) plays:                       //
//     → 40% chance to follow-up with another (HEV VOICE LINE):        //
//       - morphine.wav                                                //
//       - This line will be on a cooldown, so as not to be spammed.   //
//                                                                     //
//------------------------ Hazard System ------------------------------//
//                                                                     //
// ▪ When triggered, randomly selects one of 6 Hazards:                //
//     → Heat, Shock, Bio, Blood Toxins, Chemical, Radiation.          //
// ▪ If Bio, Blood Toxins, Chem, or Radiation triggered,               //
//   their damage lingers over time.                                   //
//     - These 4 Hazards have a 50% chance to let the user know        //
//       that damage is lingering via a (HEV VOICE LINE).              //
//     - This has been configured directly in the sound font files.    //
// ▪ Heat and Shock do not have lingering damage and should be         //
//   cleared immediately by the user by clicking AUX.                  //
//                                                                     //
//----------------- Quick Healing & Recharging ------------------------//
//                                                                     //
// ▪ Two recovery options are available, which replicate the small     //
//   health and armor pickups in-game, the Medkits and Batteries.      //
//     - Medkit  → Immediately recovers 15 Health.                     //
//     - Battery → Immediately recovers 15 Armor.                      //
// ▪ Both have their own distinct (HEV UI SOUND):                      //
//     - Medkit → medkit.wav                                           //
//     - Battery → battery.wav                                         //
// ▪ Medkit sound feedback:                                            //
//     - Plays medkit.wav on trigger.                                  //
//     - endlock.wav plays if already at max Health.                   //
// ▪ Battery sound feedback:                                           //
//     - Plays battery.wav on trigger.                                 //
//     - endlb.wav plays if already at max Armor.                      //
//     - Plays Armor Readout function, and rounds the value to         //
//       the nearest multiple of 5.                                    //
//                                                                     //
//----------------------- Armor Readout -------------------------------//
//                                                                     //
// ▪ Armor Readout is used to hear the current Armor value.            //
// ▪ Triggered by double-clicking AUX.                                 //
// ▪ Activates a sequence of (HEV UI SOUND) then (HEV VOICE LINE):     //
//   → Subtle alert fuzz**.wav → Armor Readout armor**.wav.            //
// ▪ If Armor = 0:                                                     //
//     - Plays warning.wav (HEV UI SOUND)                              //
//     - No (HEV VOICE LINE)                                           //
//                                                                     //
//=====================================================================//

#ifndef PROPS_HEV_H
#define PROPS_HEV_H
#define PROP_TYPE Hev
#define LOCKUP_HEALING SaberBase::LOCKUP_NORMAL
#define LOCKUP_FILL_ARMOR SaberBase::LOCKUP_LIGHTNING_BLOCK

#include "prop_base.h"
#include <cmath>

EFFECT(health);
EFFECT(armor);
EFFECT(hazard);
EFFECT(stun);
EFFECT(death);
EFFECT(armor_alarm);
EFFECT(armor_compromised);
EFFECT(major);
EFFECT(minor);
EFFECT(morphine);

struct HEVTimerBase {
  uint32_t start_ = 0;

  uint32_t interval_ = 0;

  bool active_ = false;

  void reset() { active_ = false; }
  
  void start() { active_ = true; start_ = millis(); }

  void configure(uint32_t interval) { interval_ = interval; }

  // Returns true if timer is inactive or timeout exceeded.
  bool check() {
    return !active_ || (millis() - start_ > interval_);
  }

  bool running() const {
    return active_ && (millis() - start_) <= interval_;
  }

  bool done() {
    return active_ && (millis() - start_) >= interval_;
  }
};

class Hev : public PROP_INHERIT_PREFIX PropBase {
public:

  HEVTimerBase timer_clash_;
  HEVTimerBase timer_post_death_cooldown_;
  HEVTimerBase timer_random_event_;
  HEVTimerBase timer_hazard_delay_;
  HEVTimerBase timer_hazard_damage_;
  HEVTimerBase timer_health_increase_;
  HEVTimerBase timer_armor_increase_;

  Hev() : PropBase() {
    // Configure all timers with their respective timings from hev_config.h
    // Clash timer uses clash_timeout_ from PropBase for debounce
    timer_clash_.configure(this->clash_timeout_);
    timer_post_death_cooldown_.configure(HEV_POST_DEATH_COOLDOWN_MS);
    timer_random_event_.configure(HEV_RANDOM_EVENT_INTERVAL_MS);
    timer_hazard_delay_.configure(HEV_HAZARD_DELAY_MS);
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
    }
    else if (armor_ > 0) {
      int excess_physical = damage - armor_;
      health_ -= excess_physical;
      // Make armor compromised sound play before setting armor to 0
      SaberBase::DoEffect(EFFECT_USER2, 0.0);
      PVLOG_NORMAL << "Armor Compromised!\n";
      armor_ = 0;
    }
    else {
      health_ -= damage;
    }

    // Enforce minimum values
    if (health_ < 0) health_ = 0;
    if (armor_ < 0) armor_ = 0;

    // Damage
    if (!quiet) SaberBase::DoEffect(EFFECT_STUN, 0.0);
    
    // Death Sound
    if (health_ == 0) {
      SaberBase::DoEffect(EFFECT_EMPTY, 0.0);
      return;
    }
    
    // Health Alert - only plays when health enters a new multiple of 10
    int new_tens = health_ / 10;
    if (tens != new_tens) {
      SaberBase::DoEffect(EFFECT_USER1, 0.0);
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
    if (health_ == 0 || (timer_clash_.active_ && !timer_clash_.check())) {
      return;
    }

    PropBase::Clash(false, strength);

    int damage = std::min((int)(strength * 4), 50);
    float v = (strength - GetCurrentClashThreshold()) / 3;

    SFX_clash.SelectFloat(v);
    SFX_clsh.SelectFloat(v);
    SFX_stab.SelectFloat(v);

    if (damage >= 30) {
      hybrid_font.PlayPolyphonic(&SFX_armor_alarm);
    }

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
      timer_hazard_delay_.configure(HEV_HAZARD_DELAY_MS);
      timer_hazard_delay_.start();
      return;
    }

    // Check sequence and apply damage
    if (timer_hazard_delay_.done()) {
      if (armor_ > 0) {
        armor_--;
        SaberBase::DoEffect(EFFECT_STUN, 0.0);
        if (armor_ == 0) {
          SaberBase::DoEffect(EFFECT_USER2, 0.0);
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
        return;
      }

      timer_hazard_delay_.configure(HEV_HAZARD_DECREASE_MS);
      timer_hazard_delay_.start();
    }
  }

  // Reset random event timer after revive.
  void Revive() {
    if (health_ == 0) {
      timer_random_event_.reset();
    }
  }

  // Increase health (Hold AUX).
  void IncreaseHealth() {
    if (SaberBase::Lockup() != LOCKUP_HEALING) return;

    if (health_ >= 100) {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      return;
    }

    if (timer_health_increase_.running()) return;

    Revive();

    health_++;
    timer_health_increase_.start();
    PVLOG_NORMAL << "Health: " << health_ << "\n";
  }

  // Increase armor (Hold POWER).
  void IncreaseArmor() {
    if (SaberBase::Lockup() != LOCKUP_FILL_ARMOR) return;

    if (armor_ >= 100) {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
      return;
    }

    if (timer_armor_increase_.running()) return;

    armor_++;
    timer_armor_increase_.start();
    PVLOG_NORMAL << "Armor: " << armor_ << "\n";
  }

  // Main Loop
  void Loop() override {
    CheckRandomEvent();
    HazardDecrease();
    IncreaseHealth();
    IncreaseArmor();
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
          SaberBase::SetLockup(SaberBase::LOCKUP_HEALING);
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
          SaberBase::SetLockup(SaberBase::LOCKUP_FILL_ARMOR);
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
      case EFFECT_ALT_SOUND:
        SOUNDQ->Play(SoundToPlay(&SFX_hazard));
        return;

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
