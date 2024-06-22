
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

#ifdef BC_DUAL_BLADES
  void FastOn(EffectLocation location = EffectLocation()) override {
    STDOUT << "***** BC prop FastOn() called\n";

    STDOUT << "***** blades_on before update: ";
    printBitmask(blades_on);
    uint16_t saved_blades_on = blades_on;

    if (blades_on == 0) {
        SaberBase::TurnOn(location);
    } else {
      // SaberBase::TurnOn(EffectLocation(0, BladeSet::fromBlade((blades_on & (1 << BC_MAIN_BLADE)) ? BC_MAIN_BLADE : BC_SECOND_BLADE)));
SaberBase::TurnOn(EffectLocation(0, blades_on));

    }
    SaberBase::DoEffect(EFFECT_FAST_ON, location);

    STDOUT << "***** blades_on after preset change: ";
    printBitmask(blades_on);
    STDOUT << "***** recalling saved_blades_on after change and setting blades on to this: ";
    printBitmask(saved_blades_on);
    // Restore what blades we had on before preset change
    blades_on = saved_blades_on;
    STDOUT << "***** blades_on on finish: ";
    printBitmask(blades_on);
  }
#endif


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

  uint16_t blades_on = 0;
  uint16_t controlled_blades = (1 << BC_MAIN_BLADE) | (1 << BC_SECOND_BLADE);

  void printBitmask(uint16_t bitmask) {
    for (int i = 15; i >= 0; --i) {
      PVLOG_DEBUG << ((bitmask >> i) & 1);
    }
    PVLOG_DEBUG << "\n";
  }

void PrintBladesStatus() {
  PVLOG_DEBUG << "***** Updated blades_on_: " 
         << ((blades_on & (1 << BC_MAIN_BLADE)) ? " MAIN " : "") 
         << ((blades_on & (1 << BC_SECOND_BLADE)) ? " SECOND " : "") 
         << "\n";
}

  void OnBlades(uint16_t blades) {  // receives opposite blade so we can exclude it
      PVLOG_DEBUG << "***** OnBlades called with blades: " << blades << "\n";  // Just an integer at the moment, 1 or 2
      PVLOG_DEBUG << "***** blades_on before: ";
      printBitmask(blades_on);
    if (!blades_on) {
      PVLOG_NORMAL << "** No blades are currently ON, turn on the specified blade and all others, exclude the opposite blade\n";

   // Debug the bitmask
    PVLOG_DEBUG << "***** blades Bitmask: ";
    printBitmask(BladeSet::fromBlade(blades).getBlades()); // Adjust this line to fit your debug method
    PVLOG_DEBUG << "***** Inverted blades Bitmask: ";
    printBitmask(~(BladeSet::fromBlade(blades).getBlades()) & 0xFFFF);


      if (is_pointing_up()) {
        FastOn(EffectLocation(0, ~BladeSet::fromBlade(blades)));
      } else {
        On(EffectLocation(0, ~BladeSet::fromBlade(blades)));
      }
    } else {
      PVLOG_NORMAL << "** A blade is already ON, turning on the specified blade\n";

    // Debug the bitmask
    PVLOG_DEBUG << "***** blades Bitmask: ";
    printBitmask(BladeSet::fromBlade(blades).getBlades()); // Adjust this line to fit your debug method
    PVLOG_DEBUG << "***** Inverted blades: ";
    printBitmask(~(BladeSet::fromBlade(blades)).getBlades() & 0xFFFF); // Adjust this line to fit your debug method

      SaberBase::TurnOn(EffectLocation(0, ~BladeSet::fromBlade(blades)));
    }
    // Mark blade bit as now ON
    PVLOG_DEBUG << "***** Marking blades_on that " << ((blades == BC_MAIN_BLADE) ? "SECOND" : "MAIN") << " is now ON\n";
    blades_on |= (blades == (BC_MAIN_BLADE)) ? (1 << BC_SECOND_BLADE) : (1 << BC_MAIN_BLADE);

    PVLOG_DEBUG << "***** blades_on after update: ";
    printBitmask(blades_on);

    PrintBladesStatus();
  }


  void OffBlades(uint16_t blades) {

    SaberBase::OffType off_type = is_pointing_up() ? SaberBase::OFF_FAST : SaberBase::OFF_NORMAL;
    PVLOG_DEBUG << "***** Off type: " << (off_type == SaberBase::OFF_FAST ? "OFF_FAST" : "OFF_NORMAL") << "\n";

    // Check if this is the only blade ON (of Main and Second blades)
    // Check if blades_on will be empty after clearing the specified blade, meaning no controlled blades are on.
    if ((blades_on & controlled_blades) == (1 << blades)) {
      // Other blade is not on, so just do normal Off() with appropriate off_type
      PVLOG_NORMAL << "** Turning OFF all blades\n";
      Off(off_type);
    } else {
      // Only Turn OFF this blade, leave the other one ON.
      PVLOG_NORMAL << "** Turning OFF only the " << ((blades_on & (1 << BC_MAIN_BLADE)) ? "MAIN" : "SECOND") << " blade\n";
      SaberBase::TurnOff(off_type, EffectLocation(0, BladeSet::fromBlade(blades)));
    }
    // Mark blade bit as now OFF
    PVLOG_DEBUG << "***** Marking blades_on that " << ((blades == BC_MAIN_BLADE) ? "MAIN" : "SECOND") << " is now OFF\n";
      // Mark blade bit as now OFF
  PVLOG_DEBUG << "***** blades_on before update: " 
         << ((blades_on & (1 << BC_MAIN_BLADE)) ? " MAIN " : "") 
         << ((blades_on & (1 << BC_SECOND_BLADE)) ? " SECOND " : "") 
         << "\n\n";

  // Debugging: Print the blades parameter and its bitmask
  PVLOG_DEBUG << "***** blades parameter: " << blades << "\n";
  PVLOG_DEBUG << "***** blades Bitmask: ";
  printBitmask(BladeSet::fromBlade(blades).getBlades()); // Adjust this line to fit your debug method

  // Debugging: Print blades_on before and after update
  PVLOG_DEBUG << "***** blades_on Bitmask before update: ";
  printBitmask(blades_on);
  PVLOG_DEBUG << "***** Mask being ANDed: ";
  printBitmask(~BladeSet::fromBlade(blades).getBlades() & 0xFFFF); // Adjust this line to fit your debug method

  blades_on &= ~(1 << blades);

  PVLOG_DEBUG << "***** blades_on after update: ";
  printBitmask(blades_on);

  PrintBladesStatus();

    battle_mode_ = false;
    spam_blast_ = false;
    muted_ = false;
    saber_off_time_ = millis();
  }

  void TurnMainBladeOnOff() {
    PVLOG_DEBUG << "***** TurnMainBladeOnOff called\n";
    if (blades_on & (1 << BC_MAIN_BLADE)) {
      OffBlades(BC_MAIN_BLADE);  // BladeSet::fromBlade will convert to bitmask
      PVLOG_NORMAL << "** Main Blade Turned OFF\n";
    } else {
      OnBlades(BC_SECOND_BLADE);  // Send the "other" blade so we know what to exclude
      PVLOG_NORMAL << "** Main Blade Turned ON\n";
    }
  }

  void TurnSecondBladeOnOff() {
    PVLOG_DEBUG << "***** TurnSecondBladeOnOff called\n";
    if (blades_on & (1 << BC_SECOND_BLADE)) {
      OffBlades(BC_SECOND_BLADE);
      PVLOG_NORMAL << "** Second Blade Turned OFF\n";
    } else {
      OnBlades(BC_MAIN_BLADE);
      PVLOG_NORMAL << "** Second Blade Turned ON\n";
    }
  }

  void TurnDualBladesOn() {
    PVLOG_DEBUG << "***** TurnDualBladesOn called\n";
    TurnOnHelper();
    blades_on |= controlled_blades;
    PVLOG_NORMAL << "**** Dual Blades Turned ON\n";
    PrintBladesStatus();
  }

  void TurnDualBladesOff() {
    PVLOG_DEBUG << "***** TurnDualBladesOff called\n";
    TurnOffHelper();
    blades_on &= ~controlled_blades;
    PVLOG_NORMAL << "** Dual Blades Turned OFF\n";
    PrintBladesStatus();
  }


#endif  // BC_DUAL_BLADES

  // rest of file here