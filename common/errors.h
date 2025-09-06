// OS-level errors, with multiple possible ways to signal them.
#ifndef COMMON_ERROR_H_DECLARED
#define COMMON_ERROR_H_DECLARED

class ProffieOSErrors {
public:
  static void sd_card_not_found();
  static void font_directory_not_found();
  static void error_in_blade_array();
  static void error_in_font_directory();
  static void low_battery();
  static void voice_pack_not_found();
  static void error_in_voice_pack_version();
};

#endif // COMMON_ERROR_H_DECLARED

#ifdef PROFFIEOS_DEFINE_FUNCTION_STAGE

#if defined(DISABLE_TALKIE) && defined(KEEP_MINIMUM_TALKIE_ONLY)
#warning Your config file has both DISABLE_TALKIE & KEEP_MINIMUM_TALKIE_ONLY defined in it. DISABLE_TALKIE has priority and no talkie will be included!
#endif

#if !defined(DISABLE_TALKIE) && !defined(KEEP_MINIMUM_TALKIE_ONLY)
#ifdef KEEP_REGULAR_TALKIE
#error Your config file has KEEP_REGULAR_TALKIE defined. Please delete it from your config! For internal use only!
#else
#define KEEP_REGULAR_TALKIE
#endif // KEEP_REGULAR_TALKIE
#endif

void ProffieOSErrors::sd_card_not_found() {
  SaberBase::DoEffect(EFFECT_SD_CARD_NOT_FOUND, 0);
#ifdef ENABLE_AUDIO
  // no need for a ".wav" file for this one
#ifndef DISABLE_TALKIE
#ifdef KEEP_MINIMUM_TALKIE_ONLY
  talkie.Say(spNO);
  talkie.Say(spS);
  talkie.Say(spD);
#else // KEEP_MINIMUM_TALKIE_ONLY
#ifdef OLD_PROFFIE_TALKIE
  talkie.Say(spSDCARD);
  talkie.Say(spNOTFOUND);
#else // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_sd_card_15, 15);
  talkie.Say(talkie_not_found_15, 15);
#endif // OLD_PROFFIE_TALKIE
#endif // KEEP_MINIMUM_TALKIE_ONLY
#else // DISABLE_TALKIE
  beeper.Beep(0.5, 261.63 * 2); // C4 - S
  beeper.Beep(0.5, 293.66 * 2); // D4 - D
  beeper.Beep(0.5, 261.63 * 2); // C4 - card
  beeper.Beep(0.5, 196.00 * 2); // G3 - not
  beeper.Beep(1.0, 130.81 * 2); // C3 - found
#endif // DISABLE_TALKIE
#endif
}

void ProffieOSErrors::font_directory_not_found() {
  SaberBase::DoEffect(EFFECT_FONT_DIRECTORY_NOT_FOUND, 0);
#ifdef ENABLE_AUDIO
  // play the fonts "font directory not found" sound if it exists
  if (SFX_erfntdnf) {
    hybrid_font.PlayCommon(&SFX_erfntdnf);
    return;
  }
#ifndef DISABLE_TALKIE
#ifdef KEEP_MINIMUM_TALKIE_ONLY
  talkie.Say(spNO);
  talkie.Say(spF);
  talkie.Say(spO);
  talkie.Say(spN);
  talkie.Say(spT);
#else // KEEP_MINIMUM_TALKIE_ONLY
#ifdef OLD_PROFFIE_TALKIE
  talkie.Say(spFONTDIRECTORY);
  talkie.Say(spNOTFOUND);
#else // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_font_directory_15, 15);
  talkie.Say(talkie_not_found_15, 15);
#endif // OLD_PROFFIE_TALKIE
#endif // KEEP_MINIMUM_TALKIE_ONLY
#else // DISABLE_TALKIE
  beeper.Beep(0.5, 261.63 * 2);   // C4 - Font
  beeper.Beep(0.5/3, 246.94 * 2); // B3 - di
  beeper.Beep(0.5/3, 220.00 * 2); // A3 - rec
  beeper.Beep(0.5/3, 196.00 * 2); // G3 - to
  beeper.Beep(0.5, 174.61 * 2);   // F3 - ry
  beeper.Beep(0.5, 146.83 * 2);   // D3 - not
  beeper.Beep(0.5, 130.81 * 2);   // C3 - found
#endif // DISABLE_TALKIE
#endif
}

void ProffieOSErrors::error_in_blade_array() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_BLADE_ARRAY, 0);
  STDOUT.println("BAD BLADE"); // should we replace STDOUT.println with PVLOG_ERROR ?
#ifdef ENABLE_AUDIO
  // play the fonts "error in blade array" sound if it exists
  if (SFX_erbladar) {
    hybrid_font.PlayCommon(&SFX_erbladar);
    return;
  }
#ifdef KEEP_REGULAR_TALKIE
#ifdef OLD_PROFFIE_TALKIE
  talkie.Say(spERRORIN);
  talkie.Say(spBLADEARRAY);
#else // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_blade_array_15, 15);
#endif // OLD_PROFFIE_TALKIE
#else // KEEP_REGULAR_TALKIE
  beeper.Beep(0.25, 174.61 * 2); // F3 - Er
  beeper.Beep(0.25, 196.00 * 2); // G3 - ror
  beeper.Beep(0.25, 174.61 * 2); // F3 - in
  beeper.Beep(0.25, 164.81 * 2); // E3 - the
  beeper.Beep(0.3, 146.83 * 2);  // D3 - blade
  beeper.Beep(0.2, 0);
  beeper.Beep(0.5, 146.83 * 2);  // D3 - ar
  beeper.Beep(1.0, 130.81 * 2);  // C3 - ray
#endif // KEEP_REGULAR_TALKIE
#endif
}

void ProffieOSErrors::error_in_font_directory() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_FONT_DIRECTORY, 0);
#ifdef ENABLE_AUDIO
  // play the fonts "error in font directory" sound if it exists
  if (SFX_erinftdr) {
    hybrid_font.PlayCommon(&SFX_erinftdr);
    return;
  }
#ifndef DISABLE_TALKIE
#ifdef KEEP_MINIMUM_TALKIE_ONLY
  talkie.Say(spF);
  talkie.Say(spO);
  talkie.Say(spN);
  talkie.Say(spT);
  talkie.Say(spERROR);
#else // KEEP_MINIMUM_TALKIE_ONLY
#ifdef OLD_PROFFIE_TALKIE
  talkie.Say(spERRORIN);
  talkie.Say(spFONTDIRECTORY);
#else // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_font_directory_15, 15);
#endif // OLD_PROFFIE_TALKIE
#endif // KEEP_MINIMUM_TALKIE_ONLY
#else // DISABLE_TALKIE
  beeper.Beep(0.25, 174.61 * 2); // F3 - Er
  beeper.Beep(0.25, 196.00 * 2); // G3 - ror
  beeper.Beep(0.25, 174.61 * 2); // F3 - in
  beeper.Beep(0.25, 164.81 * 2); // E3 - font
  beeper.Beep(0.5, 146.83 * 2);  // D3 - di
  beeper.Beep(0.5, 164.81 * 2);  // E3 - rec
  beeper.Beep(0.5, 196.00 * 2);  // G3 - to
  beeper.Beep(0.5, 246.94 * 2);  // B3 - r
  beeper.Beep(0.5, 261.63 * 2);  // C4 - y
#endif // DISABLE_TALKIE
#endif
}

void ProffieOSErrors::low_battery() {
#ifdef ENABLE_AUDIO
  // play the fonts low battery sound if it exists
  if (SFX_lowbatt) {
    hybrid_font.PlayCommon(&SFX_lowbatt);
    return;
  }
#ifdef KEEP_REGULAR_TALKIE
#ifdef OLD_PROFFIE_TALKIE
  talkie.Say(spLOWBATTERY);
#else // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_low_battery_15, 15);
#endif // OLD_PROFFIE_TALKIE
#else // KEEP_REGULAR_TALKIE
  beeper.Beep(1.0, 261.63 * 2); // C4
  beeper.Beep(1.0, 130.81 * 2); // C3
#endif // KEEP_REGULAR_TALKIE
#endif
}

void ProffieOSErrors::voice_pack_not_found() {
  STDOUT.println("YOUR VOICE PACK IS MISSING"); // should we replace STDOUT.println with PVLOG_ERROR ?
#ifdef ENABLE_AUDIO
  // play the fonts "voice pack not found" sound if it exists
  if (SFX_ervpntfd) {
    hybrid_font.PlayCommon(&SFX_ervpntfd);
    return;
  }
#ifdef KEEP_REGULAR_TALKIE
  talkie.Say(talkie_voice_pack_15, 15);
#ifdef OLD_PROFFIE_TALKIE
  talkie.Say(spNOTFOUND);
#else // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_not_found_15, 15);
#endif // OLD_PROFFIE_TALKIE
#else // KEEP_REGULAR_TALKIE
  // insert appropriate beeper.Beep codes here // ?? - voi
  // insert appropriate beeper.Beep codes here // ?? - ce
  // insert appropriate beeper.Beep codes here // ?? - pack
  beeper.Beep(0.5, 146.83 * 2);   // D3 - not
  beeper.Beep(0.5, 130.81 * 2);   // C3 - found
#endif // KEEP_REGULAR_TALKIE
#endif
}

void ProffieOSErrors::error_in_voice_pack_version() {
  STDOUT.println("INCORRECT VOICE PACK VERSION"); // should we replace STDOUT.println with PVLOG_ERROR ?
#ifdef ENABLE_AUDIO
  // play the fonts "error in voicepack version" sound if it exists
  if (SFX_erinvpvr) {
    hybrid_font.PlayCommon(&SFX_erinvpvr);
    return;
  }
#ifdef KEEP_REGULAR_TALKIE
#ifdef OLD_PROFFIE_TALKIE
  talkie.Say(spERRORIN);
#else // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_error_in_15, 15);
#endif // OLD_PROFFIE_TALKIE
  talkie.Say(talkie_voice_pack_15, 15);
  talkie.Say(talkie_version_15 ,15);
#else // KEEP_REGULAR_TALKIE
  beeper.Beep(0.25, 174.61 * 2); // F3 - Er
  beeper.Beep(0.25, 196.00 * 2); // G3 - ror
  beeper.Beep(0.25, 174.61 * 2); // F3 - in
  // insert appropriate beeper.Beep codes here // ?? - voi
  // insert appropriate beeper.Beep codes here // ?? - ce
  // insert appropriate beeper.Beep codes here // ?? - pack
  // insert appropriate beeper.Beep codes here // ?? - ver
  // insert appropriate beeper.Beep codes here // ?? - sion
#endif // KEEP_REGULAR_TALKIE
#endif
}

#endif // PROFFIEOS_DEFINE_FUNCTION_STAGE
