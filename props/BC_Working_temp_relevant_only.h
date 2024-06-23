
#ifndef PROPS_SABER_BC_BUTTONS_H
#define PROPS_SABER_BC_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"
#include "../sound/sound_library.h"
#include "../modes/select_cancel_mode.h"
#include "../modes/settings_menues.h"

#ifndef BC_MAIN_BLADE
#define BC_MAIN_BLADE 1
#endif

#ifndef BC_SECOND_BLADE
#define BC_SECOND_BLADE 2
#endif

#if BC_MAIN_BLADE == BC_SECOND_BLADE
#error BC_MAIN_BLADE and BC_SECOND_BLADE cannot be the same
#endif

// The Saber class implements the basic states and actions
// for the saber.
class SaberBCButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  // SaberBCButtons() : PropBase() {}
  SaberBCButtons() : PropBase() {}
  const char* name() override { return "SaberBCButtons"; }

  // bunch of functions here


  bool is_pointing_up() {
    if (fusor.angle1() > M_PI / 3) return true;
#ifdef BC_DUAL_BLADES
    if (fusor.angle1() < -M_PI / 3) return true;
#endif
    return false;
  }

  void TurnOnHelper() {
    if (is_pointing_up()) {
      FastOn();
    } else {
      On();
    }
  }

  void TurnOffHelper() {
      if (is_pointing_up()) {
      Off(OFF_FAST);
    } else {
      Off();
    }
    saber_off_time_ = millis();
    battle_mode_ = false;
    spam_blast_ = false;
    muted_ = false;
  }

  void DoChangePresetFast() {
    if (CheckColorChangeMode()) return;
    if (fusor.angle1() > M_PI / 3) {
      // Main Blade pointing UP
      first_preset_fast();
    } else if (fusor.angle1() < -M_PI / 3) {
      // Main Blade pointing DOWN
      previous_preset_fast();
    } else {
      next_preset_fast();
    }
    // Maintain mute through Fast preset change
    if (muted_) {
      unmute_on_deactivation_ = true;
      SetMute(true);
    }
  }

#ifdef BC_DUAL_BLADES

  static const BladeSet controlled_blades_;

  void PrintBladesStatus() {
    PVLOG_DEBUG << "***** Updated. Current blade on: " 
           << (SaberBase::OnBlades()[BC_MAIN_BLADE] ? " MAIN " : "") 
           << (SaberBase::OnBlades()[BC_SECOND_BLADE] ? " SECOND " : "") 
           << "\n";
  }

  void TurnBladeOn(BladeSet target_blade) {  // receives opposite blade (MAIN or SECOND) so we can exclude it
    BladeSet targetBladeSet = BladeSet::fromBlade(target_blade);

    if (SaberBase::OnBlades().off()) {
      PVLOG_NORMAL << "** No blades are currently ON, turn on the " 
                   << (target_blade == BC_MAIN_BLADE ? "SECOND" : "MAIN")
                   << " blade and all others, exclude the " 
                   << (target_blade == BC_MAIN_BLADE ? "MAIN" : "SECOND")
                   << " blade\n";
      if (is_pointing_up()) {
        FastOn(EffectLocation(0, ~targetBladeSet));
      } else {
        On(EffectLocation(0, ~targetBladeSet));
      }
    } else {
      PVLOG_NORMAL << "** The " 
                   << (SaberBase::OnBlades()[BC_MAIN_BLADE] ? "MAIN" : "SECOND") 
                   << " blade is already ON, turning on the " 
                   << (target_blade == BC_MAIN_BLADE ? "SECOND" : "MAIN")
                   << " blade\n";
      SaberBase::TurnOn(EffectLocation(0, ~targetBladeSet));
    }
    PrintBladesStatus();
  }

  void TurnBladeOff(BladeSet target_blade) {
    BladeSet targetBladeSet = BladeSet::fromBlade(target_blade);
    SaberBase::OffType off_type = is_pointing_up() ? SaberBase::OFF_FAST : SaberBase::OFF_NORMAL;
    PVLOG_DEBUG << "***** Off type: " << (off_type == SaberBase::OFF_FAST ? "OFF_FAST" : "OFF_NORMAL") << "\n";

    // Check if this is the only blade ON (of MAIN or SECOND blades)
    if ((SaberBase::OnBlades() & ~targetBladeSet & controlled_blades_).off()) {
      // Other blade is not on, so just do normal Off() with appropriate off_type
      PVLOG_NORMAL << "** Turning OFF all blades\n";
      Off(off_type);
    } else {
      // Only Turn OFF this blade, leave the other one ON.
      PVLOG_NORMAL << "** Turning OFF only the " 
                   << (SaberBase::OnBlades()[BC_MAIN_BLADE] ? "MAIN" : "SECOND") 
                   << " blade\n";
      SaberBase::TurnOff(off_type, EffectLocation(0, targetBladeSet));
    }
    PrintBladesStatus();
    battle_mode_ = false;
    spam_blast_ = false;
    muted_ = false;
    saber_off_time_ = millis();
  }

  void TurnMainBladeOnOff() {
    PVLOG_DEBUG << "***** TurnMainBladeOnOff called\n";
    if (SaberBase::OnBlades()[BC_MAIN_BLADE]) {
      TurnBladeOff(BC_MAIN_BLADE);
      PVLOG_NORMAL << "** Main Blade Turned OFF\n";
    } else {
      TurnBladeOn(BC_SECOND_BLADE);  // Send the "other" blade so we know what to exclude
      PVLOG_NORMAL << "** Main Blade Turned ON\n";
    }
  }

  void TurnSecondBladeOnOff() {
    PVLOG_DEBUG << "***** TurnSecondBladeOnOff called\n";
    if (SaberBase::OnBlades()[BC_SECOND_BLADE]) {
      TurnBladeOff(BC_SECOND_BLADE);
      PVLOG_NORMAL << "** Second Blade Turned OFF\n";
    } else {
      TurnBladeOn(BC_MAIN_BLADE);  // Send the "other" blade so we know what to exclude
      PVLOG_NORMAL << "** Second Blade Turned ON\n";
    }
  }

  void TurnDualBladesOn() {
    PVLOG_DEBUG << "***** TurnDualBladesOn called\n";
    TurnOnHelper();
    PVLOG_NORMAL << "**** Dual Blades Turned ON\n";
    PrintBladesStatus();
  }

  void TurnDualBladesOff() {
    PVLOG_DEBUG << "***** TurnDualBladesOff called\n";
    TurnOffHelper();
    PVLOG_NORMAL << "** Dual Blades Turned OFF\n";
    PrintBladesStatus();
  }

#endif  // BC_DUAL_BLADES

  // some of the Event2 button events to trigger this stuff - SINGLE button version (to get the hard part out of the way)

// Change Preset
// Pointing UP = First
// Pointing DOWN = Previous
// NOT Pointing UP or DOWN = Next
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        DoChangePreset();
        return true;

// Change Preset Fast
// Pointing UP = First Fast
// Pointing DOWN = Previous Fast
// NOT Pointing UP or DOWN = Next Fast
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON):
        DoChangePresetFast();
        return true;

// Turn Main Blade ON/OFF
// (Click POW for first ON)
// (Click and Hold POW for OFF, or also for ON when SECOND blade is already on.)
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        if (mode_volume_) {
          QuickMaxVolume();
        } else {
          TurnMainBladeOnOff();
        }
        return true;


// Turn Second Blade ON/OFF using POW button
// (Double Click POW for first ON)
// (Double Click and Hold POW for OFF, or also for ON when MAIN blade is already on.)
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
      if (mode_volume_) {
        QuickMinVolume();
      } else {
      TurnSecondBladeOnOff();
      }
      return true;

// Turn Dual Blades ON
// (Double Click and Hold POW for ON)
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):  // pow m2
        if (mode_volume_) return false;
        TurnDualBladesOn();
        return true;

// Turn Dual Blades OFF (will also turn OFF any single blade that's ON)
// (Double Click and Hold POW, release after a second. (Click then long Click)
      case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_ON):
        TurnDualBladesOff();
        return true;

// rest of file
