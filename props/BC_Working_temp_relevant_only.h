
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

  void Loop() override {
    PropBase::Loop();
    DetectMenuTurn();
    DetectTwist();
    DetectShake();
    Vec3 mss = fusor.mss();
    sound_library_.Poll(wav_player);
    if (wav_player && !wav_player->isPlaying()) wav_player.Free();
    if (SaberBase::IsOn()) {
      DetectSwing();
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > BC_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > BC_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }
//EVENT_PUSH
      if (fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 70 &&
          fusor.swing_speed() < 30 &&
          fabs(fusor.gyro().x) < 10) {
        if (millis() - push_begin_millis_ > BC_FORCE_PUSH_LENGTH) {
          Event(BUTTON_NONE, EVENT_PUSH);
          push_begin_millis_ = millis();
        }
      } else {
        push_begin_millis_ = millis();
      }
    } else {  // SaberBase is not ON
// EVENT_SWING - Swing On gesture control to allow fine tuning of speed needed to ignite
      if (millis() - saber_off_time_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (swinging_ && fusor.swing_speed() < 90) {
          swinging_ = false;
        }
        if (!swinging_ && fusor.swing_speed() > BC_SWING_ON_SPEED) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
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
// Timers
    // Mute All timer - play optional mute.wav first.
    if (mute_all_timer_.timerCheck()) {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnOnHelper();
      }
    }
    // Mute Main Blade timer
    if (mute_mainBlade_timer_.timerCheck()) {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnBladeOn(BladeSet::fromBlade(BC_SECOND_BLADE));
        PVLOG_NORMAL << "** Main Blade Turned ON Muted\n";
      }
    }
    // Mute Second Blade timer
    if (mute_secondBlade_timer_.timerCheck()) {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnBladeOn(BladeSet::fromBlade(BC_MAIN_BLADE));
        PVLOG_NORMAL << "** Second Blade Turned ON Muted\n";
      }
    }
    // Scroll Presets timer - avoid beep/wav overlap
    if (scroll_presets_timer_.timerCheck() && scroll_presets_) {
        SaberBase::DoEffect(EFFECT_NEWFONT, 0);
    }

  }  // Loop()

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

  void MuteAll() {
    if (SFX_mute) {
      hybrid_font.PlayCommon(&SFX_mute);
      mute_all_timer_.trigger(SaberBase::sound_length * 1000);
    } else {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnOnHelper();
      }
    }
  }

  void DoChangePreset() {
    if (mode_volume_) return;
      if (fusor.angle1() > M_PI / 3) {
      // Main Blade pointing UP
        first_preset();
      } else if (fusor.angle1() < -M_PI / 3) {
      // Main Blade pointing DOWN
        previous_preset();
      } else {
      // Main Blade NOT pointing UP or DOWN
        next_preset();
      }
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
// more stuff



#ifdef BC_DUAL_BLADES

  static const BladeSet controlled_blades_;

  
   void TurnBladeOn(BladeSet target_blade) {
    // Add in all non-controlled blades, effectively excluding the "other" blade.
    target_blade = target_blade | ~controlled_blades_;
    if (SaberBase::OnBlades().off()) {
      PVLOG_NORMAL << "** No blades are currently ON, turning on the " 
                   << (target_blade[BC_MAIN_BLADE] ? "SECOND" : "MAIN")
                   << " blade and all others, excluding the " 
                   << (target_blade[BC_MAIN_BLADE] ? "MAIN" : "SECOND")
                   << " blade\n";
      if (is_pointing_up()) {
        FastOn(EffectLocation(0, target_blade));
      } else {
        On(EffectLocation(0, target_blade));
      }
    } else {
      PVLOG_NORMAL << "** The " 
                   << (SaberBase::OnBlades()[BC_MAIN_BLADE] ? "MAIN" : "SECOND") 
                   << " blade is already ON, turning on the " 
                   << (target_blade[BC_MAIN_BLADE] ? "SECOND" : "MAIN")
                   << " blade\n";
      SaberBase::TurnOn(EffectLocation(0, target_blade));
    }
  }

  void TurnBladeOff(BladeSet target_blade) {
    SaberBase::OffType off_type = is_pointing_up() ? SaberBase::OFF_FAST : SaberBase::OFF_NORMAL;
    PVLOG_DEBUG << "***** Off type: " << (off_type == SaberBase::OFF_FAST ? "OFF_FAST" : "OFF_NORMAL") << "\n";

    // Check if this is the only blade ON (of MAIN or SECOND blades)
    if ((SaberBase::OnBlades() & ~target_blade & controlled_blades_).off()) {
      // Other blade is not on, so just do normal Off() with appropriate off_type
      PVLOG_NORMAL << "** Turning OFF all blades\n";
      Off(off_type);
      muted_ = false;
    } else {
      // Only Turn OFF this blade, leave the other one ON.
      PVLOG_NORMAL << "** Turning OFF only the " 
                   << (SaberBase::OnBlades()[BC_MAIN_BLADE] ? "MAIN" : "SECOND") 
                   << " blade\n";
      SaberBase::TurnOff(off_type, EffectLocation(0, target_blade));
    }
    battle_mode_ = false;
    spam_blast_ = false;
    saber_off_time_ = millis();
  }

  void TurnMainBladeOnOff() {
    PVLOG_DEBUG << "***** TurnMainBladeOnOff called\n";
    if (SaberBase::OnBlades()[BC_MAIN_BLADE]) {
      TurnBladeOff(BladeSet::fromBlade(BC_MAIN_BLADE));
      PVLOG_NORMAL << "** Main Blade Turned OFF\n";
    } else {
      TurnBladeOn(BladeSet::fromBlade(BC_MAIN_BLADE));
      PVLOG_NORMAL << "** Main Blade Turned ON\n";
    }
  }

  void TurnMainBladeOnMuted() {
    if (SFX_mute) {
      hybrid_font.PlayCommon(&SFX_mute);
      mute_mainBlade_timer_.trigger(SaberBase::sound_length * 1000);
    } else {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnBladeOn(BladeSet::fromBlade(BC_MAIN_BLADE));
        PVLOG_NORMAL << "** Main Blade Turned ON Muted\n";
      }
    }
  }

  void TurnSecondBladeOnOff() {
    PVLOG_DEBUG << "***** TurnSecondBladeOnOff called\n";
    if (SaberBase::OnBlades()[BC_SECOND_BLADE]) {
      TurnBladeOff(BladeSet::fromBlade(BC_SECOND_BLADE));
      PVLOG_NORMAL << "** Second Blade Turned OFF\n";
    } else {
      TurnBladeOn(BladeSet::fromBlade(BC_SECOND_BLADE));
      PVLOG_NORMAL << "** Second Blade Turned ON\n";
    }
  }

  void TurnSecondBladeOnMuted() {
    if (SFX_mute) {
      hybrid_font.PlayCommon(&SFX_mute);
      mute_secondBlade_timer_.trigger(SaberBase::sound_length * 1000);
    } else {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnBladeOn(BladeSet::fromBlade(BC_SECOND_BLADE));
        PVLOG_NORMAL << "** Second Blade Turned ON Muted\n";
      }
    }
  }

  void TurnDualBladesOn() {
    PVLOG_DEBUG << "***** TurnDualBladesOn called\n";
    TurnOnHelper();
    PVLOG_NORMAL << "**** Dual Blades Turned ON\n";
  }

  void TurnDualBladesOff() {
    PVLOG_DEBUG << "***** TurnDualBladesOff called\n";
    TurnOffHelper();
    PVLOG_NORMAL << "** Dual Blades Turned OFF\n";
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

// Turn Main Blade ON Muted
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_MEDIUM, MODE_OFF):
        if (mode_volume_) return false;
        TurnMainBladeOnMuted();
        return true;

// Turn Second Blade ON Muted
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_CLICK_LONG, MODE_OFF):
        if (mode_volume_) return false;
        TurnSecondBladeOnMuted();
        return true;

// Turn Both Blades ON Muted
      case EVENTID(BUTTON_POWER, EVENT_THIRD_CLICK_LONG, MODE_OFF):
        if (mode_volume_) return false;
        MuteAll();
        return true;




// rest of file
