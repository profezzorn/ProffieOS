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
};

#endif // COMMON_ERROR_H_DECLARED

#ifdef PROFFIEOS_DEFINE_FUNCTION_STAGE

void ProffieOSErrors::sd_card_not_found() {
#ifdef ENABLE_AUDIO
  talkie.Say(talkie_sd_card_15, 15);
  talkie.Say(talkie_not_found_15, 15);
#endif
}

void ProffieOSErrors::font_directory_not_found() {
#ifdef ENABLE_AUDIO
  talkie.Say(talkie_font_directory_15, 15);
  talkie.Say(talkie_not_found_15, 15);
#endif
}

void ProffieOSErrors::error_in_blade_array() {
  STDOUT.println("BAD BLADE");
#ifdef ENABLE_AUDIO
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_blade_array_15, 15);
#endif
}

void ProffieOSErrors::error_in_font_directory() {
#ifdef ENABLE_AUDIO
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_font_directory_15, 15);
#endif
}

void ProffieOSErrors::low_battery() {
#ifdef ENABLE_AUDIO
    // play the fonts low battery sound if it exists
    if (SFX_lowbatt) {
      hybrid_font.PlayCommon(&SFX_lowbatt);
      return;
    }

    talkie.Say(talkie_low_battery_15, 15);
#endif
}


#endif // PROFFIEOS_DEFINE_FUNCTION_STAGE
