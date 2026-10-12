// OS-level errors, with multiple possible ways to signal them.
#ifndef COMMON_ERROR_H_DECLARED
#define COMMON_ERROR_H_DECLARED

#ifdef ENABLE_AUDIO
void DodgeSound(uint32_t millis);
#else
#define DodgeSound(X) X
#endif

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

#ifdef ENABLE_AUDIO
namespace {
#ifndef DISABLE_TALKIE
void SaySdCardNotFound();
void SayFontDirectoryNotFound();
void SayVoicePackNotFound();
void SayErrorInBladeArray();
void SayErrorInFontDirectory();
void SayErrorInVoicePackVersion();
void SayLowBattery();
#else
void BeepSdCardNotFound();
void BeepFontDirectoryNotFound();
void BeepVoicePackNotFound();
void BeepErrorInBladeArray();
void BeepErrorInFontDirectory();
void BeepErrorInVoicePackVersion();
void BeepLowBattery();
#endif
}  // namespace
#endif  // ENABLE_AUDIO


void ProffieOSErrors::sd_card_not_found() {
  SaberBase::DoEffect(EFFECT_SD_CARD_NOT_FOUND, 0);
  PVLOG_ERROR << "** ERROR - SD card not found.\n"
                 "** See https://pod.hubbe.net/troubleshooting/what-is-it-saying.html#sd-card-not-found\n";
#ifdef ENABLE_AUDIO
#ifndef DISABLE_TALKIE
  SOUNDQ->Play(SaySdCardNotFound);
#else
  SOUNDQ->Play(BeepSdCardNotFound);
#endif
#endif
}

void ProffieOSErrors::font_directory_not_found() {
  SaberBase::DoEffect(EFFECT_FONT_DIRECTORY_NOT_FOUND, 0);
  PVLOG_ERROR << "** ERROR - Font directory not found.\n"
                 "** See https://pod.hubbe.net/troubleshooting/what-is-it-saying.html#font-directory-not-found\n";
#ifdef ENABLE_AUDIO
  if (SaberBase::sound_length > 0) return;
#ifndef DISABLE_TALKIE
  SOUNDQ->Play(SayFontDirectoryNotFound);
#else
  SOUNDQ->Play(BeepFontDirectoryNotFound);
#endif
#endif
}

void ProffieOSErrors::voice_pack_not_found() {
  SaberBase::DoEffect(EFFECT_VOICE_PACK_NOT_FOUND, 0);
  PVLOG_ERROR << "** ERROR - Voice pack not found.\n"
                 "** See https://pod.hubbe.net/troubleshooting/what-is-it-saying.html#voice-pack-not-found\n";
#ifdef ENABLE_AUDIO
  if (SaberBase::sound_length > 0) return;
#ifndef DISABLE_TALKIE
  SOUNDQ->Play(SayVoicePackNotFound);
#else
  SOUNDQ->Play(BeepVoicePackNotFound);
#endif
#endif
}

void ProffieOSErrors::error_in_blade_array() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_BLADE_ARRAY, 0);
  PVLOG_ERROR << "** ERROR - Error in blade array\n"
                 "** See https://pod.hubbe.net/troubleshooting/what-is-it-saying.html#error-in-blade-array\n";
#ifdef ENABLE_AUDIO
  if (SaberBase::sound_length > 0) return;
#ifndef DISABLE_TALKIE
  SOUNDQ->Play(SayErrorInBladeArray);
#else
  SOUNDQ->Play(BeepErrorInBladeArray);
#endif
#endif
}

void ProffieOSErrors::error_in_font_directory() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_FONT_DIRECTORY, 0);
  PVLOG_ERROR << "** ERROR - Error in font directory.\n"
                 "** See https://pod.hubbe.net/troubleshooting/what-is-it-saying.html#error-in-font-directory\n";
#ifdef ENABLE_AUDIO
  if (SaberBase::sound_length > 0) return;
#ifndef DISABLE_TALKIE
  SOUNDQ->Play(SayErrorInFontDirectory);
#else
  SOUNDQ->Play(BeepErrorInFontDirectory);
#endif
#endif
}

void ProffieOSErrors::error_in_voice_pack_version() {
  SaberBase::DoEffect(EFFECT_ERROR_IN_VOICE_PACK_VERSION, 0);
  PVLOG_ERROR << "** ERROR - Error in voice pack version.\n"
                 "** See https://pod.hubbe.net/troubleshooting/what-is-it-saying.html#error-in-voice-pack-version\n";
#ifdef ENABLE_AUDIO
  if (SaberBase::sound_length > 0) return;
#ifndef DISABLE_TALKIE
  SOUNDQ->Play(SayErrorInVoicePackVersion);
#else
  SOUNDQ->Play(BeepErrorInVoicePackVersion);
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
  SOUNDQ->Play(SayLowBattery);
#else
  SOUNDQ->Play(BeepLowBattery);
#endif
#endif
}

#ifdef ENABLE_AUDIO
namespace {
#ifndef DISABLE_TALKIE
void SaySdCardNotFound() {
  talkie.Say(talkie_sd_card_15, 15);
  talkie.Say(talkie_not_found_15, 15);
}
void SayFontDirectoryNotFound() {
  talkie.Say(talkie_font_directory_15, 15);
  talkie.Say(talkie_not_found_15, 15);
}
void SayVoicePackNotFound() {
  talkie.Say(talkie_voice_pack_15, 25);
  talkie.Say(talkie_not_found_15, 15);
}
void SayErrorInBladeArray() {
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_blade_array_15, 15);
}
void SayErrorInFontDirectory() {
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_font_directory_15, 15);
}
void SayErrorInVoicePackVersion() {
  talkie.Say(talkie_error_in_15, 15);
  talkie.Say(talkie_voice_pack_15, 15);
  talkie.Say(talkie_version_15, 15);
}
void SayLowBattery() {
  talkie.Say(talkie_low_battery_15, 15);
}
#else
void BeepSdCardNotFound() {
  beeper.Beep(0.5, 261.63 * 2); // C5 - S
  beeper.Beep(0.5, 293.66 * 2); // D5 - D
  beeper.Beep(0.5, 261.63 * 2); // C5 - card
  beeper.Beep(0.5, 196.00 * 2); // G4 - not
  beeper.Beep(1.0, 130.81 * 2); // C4 - found
}
void BeepFontDirectoryNotFound() {
  beeper.Beep(0.5,   261.63 * 2); // C5 - Font
  beeper.Beep(0.5/3, 246.94 * 2); // B4 - di
  beeper.Beep(0.5/3, 220.00 * 2); // A4 - rec
  beeper.Beep(0.5/3, 196.00 * 2); // G4 - tor
  beeper.Beep(0.5,   174.61 * 2); // F4 - y
  beeper.Beep(0.5,   146.83 * 2); // D4 - not
  beeper.Beep(0.5,   130.81 * 2); // C4 - found
}
void BeepVoicePackNotFound() {
  beeper.Beep(1.0, 220.00 * 2); // A4 - Voice
  beeper.Beep(0.5, 130.81 * 2); // C4 - pack
  beeper.Beep(0.5, 146.83 * 2); // D4 - not
  beeper.Beep(1.0, 130.81 * 2); // C4 - found
}
void BeepErrorInBladeArray() {
  beeper.Beep(0.25, 174.61 * 2); // F4 - Err
  beeper.Beep(0.25, 196.00 * 2); // G4 - or
  beeper.Beep(0.25, 174.61 * 2); // F4 - in
  beeper.Beep(0.25, 164.81 * 2); // E4 - the
  beeper.Beep(0.3,  146.83 * 2); // D4 - blade
  beeper.Beep(0.2,  0);          // Silence
  beeper.Beep(0.5,  146.83 * 2); // D4 - ar
  beeper.Beep(1.0,  130.81 * 2); // C4 - ray
}
void BeepErrorInFontDirectory() {
  beeper.Beep(0.25, 174.61 * 2); // F4 - Err
  beeper.Beep(0.25, 196.00 * 2); // G4 - or
  beeper.Beep(0.25, 174.61 * 2); // F4 - in
  beeper.Beep(0.25, 164.81 * 2); // E4 - the
  beeper.Beep(0.5,  146.83 * 2); // D4 - font
  beeper.Beep(0.5,  164.81 * 2); // E4 - di
  beeper.Beep(0.5,  196.00 * 2); // G4 - rec
  beeper.Beep(0.5,  246.94 * 2); // B4 - tor
  beeper.Beep(0.5,  261.63 * 2); // C5 - y
}
void BeepErrorInVoicePackVersion() {
  beeper.Beep(0.25, 174.61 * 2); // F4 - Err
  beeper.Beep(0.25, 196.00 * 2); // G4 - or
  beeper.Beep(0.25, 174.61 * 2); // F4 - in
  beeper.Beep(0.25, 164.81 * 2); // E4 - the
  beeper.Beep(0.5,  220.00 * 2); // A4 - voice
  beeper.Beep(0.5,  146.83 * 2); // D4 - pack
  beeper.Beep(1.0,  196.00 * 2); // G4 - ver
  beeper.Beep(0.5,  130.81 * 2); // C4 - sion
}
void BeepLowBattery() {
  beeper.Beep(1.0, 261.63 * 2); // C4
  beeper.Beep(1.0, 130.81 * 2); // C3
}
#endif
}  // namespace
#endif  // ENABLE_AUDIO

#endif // PROFFIEOS_DEFINE_FUNCTION_STAGE
