// OS-level errors, with multiple possible ways to signal them.
#ifndef COMMON_ERROR_H_DECLARED
#define COMMON_ERROR_H_DECLARED

/* DISABLE_TALKIE define will save memory by replacing the default
   spoken error messages with the following beep sequences:
   - Single Low Beep on Boot = "SD Card Not Found"
   - Low Beep to High Beep on Boot or Change Preset = "Font Directory Not Found"
   - High Beep to Low Beep to High Beep on Boot or Change Preset = "Error in Font Directory"
   - Low Beep to High Beep to Low Beep on Boot = "Error in Blade Array" 
   - Two Quick High Pitched Beeps = "Low Battery" (if lowbatt.wav not present)   
*/

class ProffieOSErrors {
public:
  static void sd_card_not_found();
  static void font_directory_not_found();
  static void error_in_blade_array();
  static void error_in_font_directory();
  static void low_battery();
};

#endif // COMMON_ERROR_H_DECLARED

#ifdef PROFFIEOS_DEFINE_FUNCTION_STAGE

void ProffieOSErrors::sd_card_not_found() {
#ifdef ENABLE_AUDIO
#if !defined(DISABLE_TALKIE)
  talkie.Say(talkie_sd_card_15, 15);
  talkie.Say(talkie_not_found_15, 15);
#else
    beeper.Beep(1.0, 1000);
#endif
#endif
}

void ProffieOSErrors::font_directory_not_found() {
#ifdef ENABLE_AUDIO
#if !defined(DISABLE_TALKIE)
  talkie.Say(talkie_font_directory_15, 15);
  talkie.Say(talkie_not_found_15, 15);
#else
  beeper.Beep(0.5, 500);
  beeper.Beep(0.5, 3000);
#endif
#endif
}

void ProffieOSErrors::error_in_blade_array() {
  STDOUT.println("BAD BLADE");
#ifdef ENABLE_AUDIO
#if !defined(DISABLE_TALKIE)
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_blade_array_15, 15);
#else
    beeper.Beep(0.25, 500);
    beeper.Beep(0.5, 3000);
    beeper.Beep(0.25, 500);
#endif
#endif
}

void ProffieOSErrors::error_in_font_directory() {
#ifdef ENABLE_AUDIO
#if !defined(DISABLE_TALKIE)
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_font_directory_15, 15);
#else
  beeper.Beep(0.5, 2000);
  beeper.Beep(0.25, 500);
  beeper.Beep(0.5, 2000);
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
#if !defined(DISABLE_TALKIE)
    talkie.Say(talkie_low_battery_15, 15);
#else
  beeper.Beep(0.25, 2000);
  beeper.Beep(0.25, 3000);
#endif
#endif
}


#endif // PROFFIEOS_DEFINE_FUNCTION_STAGE
