// OS-level errors, with multiple possible ways to signal them.
#ifndef COMMON_ERROR_H_DECLARED
#define COMMON_ERROR_H_DECLARED

#include "delay_timer.h"

class ProffieOSErrors {
public:
  static void sd_card_not_found();
  static void font_directory_not_found();
  static void voice_pack_not_found();
  static void error_in_blade_array();
  static void error_in_font_directory();
  static void error_in_voice_pack_version();
  static void low_battery();
};

#endif // COMMON_ERROR_H_DECLARED

#ifdef PROFFIEOS_DEFINE_FUNCTION_STAGE

void ProffieOSErrors::sd_card_not_found() {
  SaberBase::DoEffect(EFFECT_SD_CARD_NOT_FOUND, 0);
  PVLOG_ERROR << "** ERROR - SD card not found.\n";
#ifdef ENABLE_AUDIO
#ifndef DISABLE_TALKIE
  talkie.Say(talkie_sd_card_15, 15);
  talkie.Say(talkie_not_found_15, 15);
#else
  beeper.Beep(0.5, 261.63 * 2); // C4
  beeper.Beep(0.5, 293.66 * 2); // D4
  beeper.Beep(0.5, 261.63 * 2); // C4
  beeper.Beep(0.5, 196.00 * 2); // G3
  beeper.Beep(1.0, 130.81 * 2); // C3
#endif
#endif
}

void ProffieOSErrors::font_directory_not_found() {
  SaberBase::DoEffect(EFFECT_FONT_DIRECTORY_NOT_FOUND, 0);
  PVLOG_ERROR << "** ERROR - Font directory not found.\n";
#ifdef ENABLE_AUDIO
#ifndef DISABLE_TALKIE
  talkie.Say(talkie_font_directory_15, 15);
  talkie.Say(talkie_not_found_15, 15);
  StartDelayTimer(2000);
#else
  beeper.Beep(0.5, 261.63 * 2); // C4
  beeper.Beep(0.5/3, 246.94 * 2); // B3
  beeper.Beep(0.5/3, 220.00 * 2); // A3
  beeper.Beep(0.5/3, 196.00 * 2); // G3
  beeper.Beep(0.5, 174.61 * 2); // F3
  beeper.Beep(0.5, 146.83 * 2); // D3
  beeper.Beep(0.5, 130.81 * 2); // C3
  StartDelayTimer(2530);
#endif
#endif
}

void ProffieOSErrors::voice_pack_not_found() {
  SaberBase::DoEffect(EFFECT_VOICE_PACK_NOT_FOUND, 0);
  PVLOG_ERROR << "** ERROR - Voice pack not found.\n";
#ifdef ENABLE_AUDIO
#ifndef DISABLE_TALKIE
  talkie.Say(talkie_voice_pack_15, 25);
  talkie.Say(talkie_not_found_15, 15);
  StartDelayTimer(2000);
#else
  beeper.Beep(1.0, 220.00 * 2); // A4 - Voice
  beeper.Beep(0.5, 130.81 * 2); // C4 - pack
  beeper.Beep(0.5, 146.83 * 2); // D4 - not
  beeper.Beep(1.0, 130.81 * 2); // C4 - found
  StartDelayTimer(3000);
#endif
#endif
}

void ProffieOSErrors::error_in_blade_array() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_BLADE_ARRAY, 0);
  STDOUT.println("BAD BLADE");
  PVLOG_ERROR << "** ERROR - Error in blade array.\n";
#ifdef ENABLE_AUDIO
#ifndef DISABLE_TALKIE
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_blade_array_15, 15);
  StartDelayTimer(2000);
#else
  beeper.Beep(0.25, 174.61 * 2); // F3 - Er
  beeper.Beep(0.25, 196.00 * 2); // G3 - ror
  beeper.Beep(0.25, 174.61 * 2); // F3 - in
  beeper.Beep(0.25, 164.81 * 2); // E3 - the
  beeper.Beep(0.3, 146.83 * 2); // D3 - blade
  beeper.Beep(0.2, 0);
  beeper.Beep(0.5, 146.83 * 2); // D3 - ar
  beeper.Beep(1.0, 130.81 * 2); // C3 - ray
  StartDelayTimer(3000);
#endif
#endif
}

void ProffieOSErrors::error_in_font_directory() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_FONT_DIRECTORY, 0);
  PVLOG_ERROR << "** ERROR - Error in font directory.\n";
#ifdef ENABLE_AUDIO
#ifndef DISABLE_TALKIE
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_font_directory_15, 15);
  StartDelayTimer(1300);
#else
  beeper.Beep(0.25, 174.61 * 2); // F3
  beeper.Beep(0.25, 196.0 * 2); // G3
  beeper.Beep(0.25, 174.61 * 2); // F3
  beeper.Beep(0.25, 164.81 * 2); // E3
  beeper.Beep(0.5, 146.83 * 2); // D3
  beeper.Beep(0.5, 164.81 * 2); // E3
  beeper.Beep(0.5, 196.0 * 2); // G3
  beeper.Beep(0.5, 246.94 * 2); // B3
  beeper.Beep(0.5, 261.63 * 2); // C4
  StartDelayTimer(3500);
#endif
#endif
}

void ProffieOSErrors::error_in_voice_pack_version() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_VOICE_PACK_VERSION, 0);
  PVLOG_ERROR << "** ERROR - Error in voice pack version.\n";
#ifdef ENABLE_AUDIO
#ifndef DISABLE_TALKIE
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_voice_pack_15, 15);
  talkie.Say(talkie_version_15, 15);
  StartDelayTimer(1500);
#else
  beeper.Beep(0.25, 174.61 * 2); // F4 - Err
  beeper.Beep(0.25, 196.00 * 2); // G4 - or
  beeper.Beep(0.25, 174.61 * 2); // F4 - in
  beeper.Beep(0.25, 164.81 * 2); // E4 - the
  beeper.Beep(0.5,  220.00 * 2); // A4 - voice
  beeper.Beep(0.5,  146.83 * 2); // D4 - pack
  beeper.Beep(1.0,  196.00 * 2); // G4 - ver
  beeper.Beep(0.5,  130.81 * 2); // C4 - sion
  StartDelayTimer(3500);
#endif
#endif
}

void ProffieOSErrors::low_battery() {
#ifdef ENABLE_AUDIO
  // play the fonts low battery sound if it exists
  if (SFX_lowbatt) {
    hybrid_font.PlayCommon(&SFX_lowbatt);
    return;
  }

#ifndef DISABLE_TALKIE
  talkie.Say(talkie_low_battery_15, 15);
#else
  beeper.Beep(1.0, 261.63 * 2); // C4
  beeper.Beep(1.0, 130.81 * 2); // C3
#endif
#endif
}


#endif // PROFFIEOS_DEFINE_FUNCTION_STAGE
