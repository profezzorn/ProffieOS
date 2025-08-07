//=====================================================================//
//                        H A L F - L I F E                            //
//                      ─────────────────────                          //
//              Hazardous Environment Suit Diagnostics                 //
//                H.E.V. Mk V - Configuration Protocol                 //
//---------------------------------------------------------------------//
//                      PROPERTY OF BLACK MESA                         //
//---------------------------------------------------------------------//
//               Created by James Nash with fundamental                //
//          support and contributions of Fredrik Hubinette             //
//=====================================================================//

//=====================================================================//
//                    H.E.V. SUIT - USAGE GUIDE                        //
//                    ─────────────────────────                        //
// --2 BUTTON SETUP--                                                  //
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
//     - Both have a maximum value of 100.                             //
//     - Both active only while the suit is ON.                        //
// - Random Hazards:                                                   //
//     - Active only while the suit is ON.                             //
//     - Always damages Armor first, then Health.                      //
//     - Triggered at intervals with a chance-based system.            //
// - When Armor is depleted:                                           //
//     - Hazards begin directly damaging Health.                       //
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
// - minor_laceration.wav    - Plays minor lacerations detected quote  //
// - minor_fracture.wav      - Plays minor fractures detected quote    //
// - major_laceration.wav    - Plays major lacerations detected quote  //
// - major_fracture.wav      - Plays major fractures detected quote    //
//                                                                     //
//------------------------ HEV UI SOUNDS ------------------------------//
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
// - armor00.wav             - No Armor SFX // Armor = 0               //
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
// ▪ Create 7 directories in root of sound font: alt00 through alt06   //
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
//    - Include the (ENVIRONMENTAL FX) that match                      //
//      the Hazard type (e.g., spark, burn, Geiger counter)            //
//                                                                     //
// ▪ All alt**/ folders (alt00–alt06) must have the **same number** of //
//    WAVs inside including **all** sub folders.                       //
//                                                                     //
//=====================================================================//

//=====================================================================//
//                        BEHAVIOR BREAKDOWN                           //
//                    ─────────────────────────                        //
//                                                                     //
//------------------------- Health Alerts -----------------------------//
//                                                                     //
// ▪ When Health drops to a lower 10th decimal (e.g., from 43 → 36),   //
//   a Health Alert (health**.wav) will play, (HEV VOICE LINE).        //
//                                                                     //
// ▪ The suit will say state different Health Alerts, depending on     //
//   what health dropped down to. Below are the health thresholds:     //
//                                                                     //
//   - ≤ 10: "User Death Imminent"    - Range: health01 - health10     //
//   - ≤ 30: "Vital Signs Critical"   - Range: health11 - health30     //
//   - ≤ 50: "Seek Medical Attention" - Range: health31 - health50     //
//                                                                     //
// ▪ Not all wavs have audio, therefore some wavs are blank, which     //
//   creates an **element of chance** for Health Alerts to play.       //
//   This has been configured directly in the health folder in the     //
//   sound font.                                                       //
//                                                                     //
// ▪ The following table shows which health** files in the font        //
//   have actual voice lines (✓ = has audio, blank = no audio).       //
//
//   1–10: "User Death Imminent"
//   health01  ✓    health06  ✓
//   health02  ✓    health07
//   health03  ✓    health08  ✓
//   health04  ✓    health09  ✓
//   health05  ✓    health10
//
//   11–30: "Vital Signs Critical"
//   health11  ✓    health18  ✓    health25
//   health12  ✓    health19       health26  ✓
//   health13       health20  ✓    health27  ✓
//   health14  ✓    health21  ✓    health28
//   health15  ✓    health22       health29  ✓
//   health16       health23  ✓    health30  ✓
//   health17  ✓    health24
//
//   31–50: "Seek Medical Attention"
//   health31       health38  ✓    health45
//   health32  ✓    health39       health46  ✓
//   health33       health40  ✓    health47
//   health34  ✓    health41       health48  ✓
//   health35       health42  ✓    health49
//   health36  ✓    health43       health50  ✓
//   health37       health44  ✓
//
// ▪ There are no Health Alerts when Health is above 50. So,           //
//   health51 - health100 are blank.                                   //
//                                                                     //
// ▪ health00 is blank. When Health is 0, death.wav will play.         //
//                                                                     //
//-------------------------- Armor Alerts -----------------------------//
//                                                                     //
// ▪ When Clash damage ≥ 30:                                           //
//     - armor_alarm.wav will play. (HEV UI SOUNDS)                    //
// ▪ If Armor reaches 0:                                               //
//     - 50% chance to play armor_compromised.wav. (HEV VOICE LINE)    //
//                                                                     //
//--------------------- Physical Clash System -------------------------//
//                                                                     //
// ▪ Each Clash deals between 1–50 damage based on impact strength.    //
// ▪ 8 (ENVIRONMENTAL FX) Clash sounds available (clsh**.wav):         //
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
//------------------- Clash Detected Voice Line -----------------------//
//                                                                     //
// ▪ 44% chance to trigger Clash Detected wav (HEV VOICE LINE):        //
//     → minor_laceration.wav (Minor Laceration)                       //
//     → minor_fracture.wav (Minor Fracture)                           //
//     → major_laceration.wav (Major Laceration)                       //
//     → major_fracture.wav (Major Fracture)                           //
//       - e.g. ”Major Fracture Detected!"                             //
//                                                                     //
//-------------------- Morphine Auto-Injection ------------------------//
//                                                                     //
// ▪ Only if major_laceration or major_fracture (HEV VOICE LINE) plays://
//     → 40% chance to follow-up with another (HEV VOICE LINE):        //
//       → morphine.wav                                                //
//         - This line will be on a cooldown, so as not to be spammed. //
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
// ▪ Both have their own distinct (HEV UI SOUNDS):                     //
//     - Medkit → medkit.wav                                           //
//     - Battery → battery.wav                                         //
// ▪ Medkit sound feedback:                                            //
//     - Plays medkit.wav on trigger.                                  //
//     - endlock.wav plays if already at max Health.                   //
// ▪ Battery sound feedback:                                           //
//     - Plays battery.wav on trigger.                                 //
//     - endlb.wav plays if already at max Armor.                      //
//     - Plays Armor Readout function, but rounds the value to         //
//       the nearest multiple of 5.                                    //
//                                                                     //
//----------------------- Armor Readout -------------------------------//
//                                                                     //
// ▪ Armor Readout is used to hear the current Armor value.            //
// ▪ Triggered by double-clicking AUX.                                 //
// ▪ Activates a sequence of (HEV UI SOUNDS) then (HEV VOICE LINE):    //
//   → Subtle alert fuzz**.wav → Armor Readout armor**.wav.            //
// ▪ If Armor = 0:                                                     //
//     - Plays armor00.wav (HEV UI SOUNDS)                             //
//     - No (HEV VOICE LINE)                                           //
//                                                                     //
//=====================================================================//

#ifndef PROPS_HEV_H
#define PROPS_HEV_H
#define PROP_TYPE Hev
#define LOCKUP_HEALING SaberBase::LOCKUP_NORMAL
#define LOCKUP_FILL_ARMOR SaberBase::LOCKUP_LIGHTNING_BLOCK

//=====================================================================//
//                     HEV SUIT TIMING DEFINES                         //
//                    ─────────────────────────                        //
//                                                                     //
//  Below are the HEV_ #defines which control the timings of how       //
//  often Hazards, Healing and Armor recharge work in the HEV suit.    //
//  All times are in (ms). Below values are the default.               //
//  To fine-tune the behaviour, adjust the values in hev_config.h      //
//                                                                     //
//  HEV_RANDOM_EVENT_INTERVAL_MS                                       //
//      - How often (ms) to check for a Random Hazard.                 //
//        Higher = less frequent checks.                               //
//                                                                     //
//  HEV_RANDOM_HAZARD_CHANCE                                           //
//      - Chance (0–100) for a Random Hazard to occur each check.      //
//        Higher = more frequent Hazards.                              //
//                                                                     //
//  HEV_HAZARD_DELAY_MS                                                //
//      - Delay (ms) before Hazard damage starts after checking and    //
//        triggering Hazard. Allows time for HEV Voice Line to finish. //
//                                                                     //
//  HEV_HAZARD_DECREASE_MS                                             //
//      - Time (ms) between each tick of Hazard damage.                //
//        Lower = faster damage-over-time.                             //
//                                                                     //
//  HEV_HAZARD_AFTER_REVIVE_MS                                         //
//      - Time (ms) after reviving before Hazards can happen again.    //
//                                                                     //
//  HEV_HEALTH_INCREASE_MS                                             //
//      - Time (ms) between each Health recharge tick (hold AUX).      //
//        Default from 0 - 100 Health is 10 seconds.                   //
//        Lower = faster healing.                                      //
//                                                                     //
//  HEV_ARMOR_INCREASE_MS                                              //
//      - Time (ms) between each armor recharge tick (hold POWER).     //
//        Default from 0 - 100 Armor is 10 seconds.                    //
//        Lower = faster recharge.                                     //
//=====================================================================//

#ifndef HEV_RANDOM_EVENT_INTERVAL_MS
#define HEV_RANDOM_EVENT_INTERVAL_MS 60000
#endif
#ifndef HEV_RANDOM_HAZARD_CHANCE
#define HEV_RANDOM_HAZARD_CHANCE 15
#endif
#ifndef HEV_HAZARD_DELAY_MS
#define HEV_HAZARD_DELAY_MS 6000
#endif
#ifndef HEV_HAZARD_DECREASE_MS
#define HEV_HAZARD_DECREASE_MS 1000
#endif
#ifndef HEV_HAZARD_AFTER_REVIVE_MS
#define HEV_HAZARD_AFTER_REVIVE_MS 60000
#endif
#ifndef HEV_HEALTH_INCREASE_MS
#define HEV_HEALTH_INCREASE_MS 100
#endif
#ifndef HEV_ARMOR_INCREASE_MS
#define HEV_ARMOR_INCREASE_MS 100
#endif

#include "prop_base.h"
#include <cmath>

// HEV VOICE LINES
EFFECT(armor);
EFFECT(health);
EFFECT(armor_compromised);
EFFECT(hazard);
EFFECT(minor_laceration);
EFFECT(minor_fracture);
EFFECT(major_laceration);
EFFECT(major_fracture);
EFFECT(morphine);

// HEV UI SOUNDS
EFFECT(armor_alarm);
EFFECT(battery);
EFFECT(death);
EFFECT(fuzz);
EFFECT(medkit);

// ENVIRONMENTAL EFFECTS
EFFECT(stun);

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
};

class Hev : public PROP_INHERIT_PREFIX PropBase {
public:

  //=====================================================================//
  //                       TIMER CONFIGURATION                           //
  //                    ─────────────────────────                        //
  //                                                                     //
  // - timer_clash_               - Uses clash_timeout from PropBase.    //
  //                                Debounce to prevent false Clashes.   //
  // - timer_random_event_        - Interval timer for Random Hazards.   //
  //                                Controls how often Hazards can occur.//
  // - timer_hazard_delay_        - Delay inbetween event trigger and    //
  //                                Hazard dps. Gap for voice to finish. //
  // - timer_hazard_after_revive_ - Cooldown after user revives.         //
  //                                Blocks Hazards until timer is done.  //
  // - timer_health_increase_     - Interval for Health recharge.        //
  //                                Controls healing rate.               //
  // - timer_armor_increase_      - Interval for Armor recharge.         //
  //                                Controls Armor recharge rate.        //
  //=====================================================================//

  HEVTimerBase timer_clash_;
  HEVTimerBase timer_random_event_;
  HEVTimerBase timer_hazard_delay_;
  HEVTimerBase timer_hazard_after_revive_;
  HEVTimerBase timer_health_increase_;
  HEVTimerBase timer_armor_increase_;

  Hev() : PropBase() {
    timer_clash_.configure(this->clash_timeout_);
    timer_random_event_.configure(HEV_RANDOM_EVENT_INTERVAL_MS);
    timer_hazard_delay_.configure(HEV_HAZARD_DELAY_MS);
    timer_hazard_after_revive_.configure(HEV_HAZARD_AFTER_REVIVE_MS);
    timer_health_increase_.configure(HEV_HEALTH_INCREASE_MS);
    timer_armor_increase_.configure(HEV_ARMOR_INCREASE_MS);
  }

  const char* name() override { return "Hev"; }

  int health_ = 100;
  int armor_ = 100;

  enum DamageType {
    DAMAGE_PHYSICAL,
    DAMAGE_HAZARD,
  };

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
  void DoDamage(int damage, bool quiet = false, DamageType type = DAMAGE_PHYSICAL) {
    int previous_health = health_;
    int previous_armor = armor_;
    int tens = health_ / 10;

    // Damage type and calculation
    switch (type) {
      case DAMAGE_PHYSICAL:
        if (armor_ >= damage) {
          armor_ -= ceilf(damage * 0.80 / 2);
          health_ -= ceilf(damage * 0.20);
        } else if (armor_ > 0) {
          int excess_physical = damage - armor_;
          health_ -= excess_physical;
          armor_ = 0;
        } else { health_ -= damage; }
        break;

      case DAMAGE_HAZARD:
        if (armor_ > 0) {
          armor_ -= damage;
        } else { health_ -= damage; }
        break;
    }

    // Enforce minimum values
    if (health_ < 0) health_ = 0;
    if (armor_ < 0) armor_ = 0;

    // (HEV VOICE LINE) Logic for Armor Compromised
    if (previous_armor > 0 && armor_ == 0) {
      SaberBase::DoEffect(EFFECT_USER2, 0.0);
      PVLOG_NORMAL << "Armor Compromised!\n";
    }

    // (ENVIRONMENTAL FX) Damage Sounds
    if (!quiet) SaberBase::DoEffect(EFFECT_STUN, 0.0);
    
    // (HEV UI SOUNDS) Logic for Death Sound
    if (health_ == 0 && previous_health > 0) {
      SaberBase::DoEffect(EFFECT_EMPTY, 0.0);
      return;
    }
    
    // (HEV VOICE LINE) Logic for Health Alert
    // Only plays when Health enters a new multiple of 10
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
    PVLOG_NORMAL << "Current Armor: " << armor_ << "\n";

    // Play random "fuzz" sound only if armor is above 0.
    if (armor_ > 0) {
      SOUNDQ->Play(&SFX_fuzz);

      // Play Armor Readout
      SFX_armor.SelectFloat(armor_ / 100.0);
      SOUNDQ->Play(&SFX_armor);

    } else {
      // If Armor is 0, immediately plays a warning sound.
      SFX_armor.SelectFloat(armor_ / 0.0);
      hybrid_font.PlayCommon(&SFX_armor);
    }
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
    // Skip Hazard check if dead or during revive cooldown
    if (health_ == 0 || !timer_hazard_after_revive_.check()) {
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
        PVLOG_NORMAL << "Activating Hazard.\n";
        current_hazard_ = (Hazard)(1 + random(6));
        SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, current_hazard_);

      } else {
        PVLOG_NORMAL << "Skipping Hazard.\n";
        timer_random_event_.start();
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
    if (!timer_hazard_delay_.running()) {
      DoDamage(1, false, DAMAGE_HAZARD);

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

      // (ENVIRONMENTAL FX) Hazard SFX
      case EFFECT_STUN:
        hybrid_font.PlayCommon(&SFX_stun);
        return;

      // (HEV VOICE LINE) Armor Compromised
      case EFFECT_USER2:
        SOUNDQ->Play(&SFX_armor_compromised);
        return;
    
      // (HEV VOICE LINE) Health Alert
      case EFFECT_USER1:
        if (health_ == 0) return; // Don't queue health sounds if dead
        SFX_health.SelectFloat(health_ / 100.0);
        SOUNDQ->Play(&SFX_health);
        return;
    
      // (HEV UI SOUNDS) Death Sound
      case EFFECT_EMPTY:
        if (health_ == 0) {
          SOUNDQ->clear_pending();
        }
        hybrid_font.PlayCommon(&SFX_death);
        return;
    }
  }

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;

      // (HEV VOICE LINE) Hazard Alert
      case EFFECT_ALT_SOUND:
        SOUNDQ->Play(SoundToPlay(&SFX_hazard));
        return;
    }
  }
};

#endif
