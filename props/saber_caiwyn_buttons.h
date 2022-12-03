// Caiwyn's Lightsaber Controls
// This config is designed to provide fast, low-latency response to button
// presses for the most basic functions (blaster blocks, lockups, and clashes)
// by omitting, or limiting non-critical features.
//
// At least 2 buttons are required.  A third button can be added, but is not
// necessary.
//
// While saber is OFF:
//          Start/Stop Track: Click Aux
//         Track Player Mode: Double-click Aux
//                            * This cycles through three playback modes:
//                            * 1. Play a single track and stop
//                            * 2. Repeat a single track in a loop
//                            * 3. Repeat all tracks in a loop
//                            * Tracks must be in <font>/tracks/*.wav, and will
//                            * be selected in alphabetical order.
//                Next Track: Hold Aux for 1 second while track is playing
//               Next Preset: Hold Aux for 1 second when track is not playing
//             Check Battery: Double-click and hold Aux for 1 second
//                   Turn On: Press Power
//     Turn On & Start Track: Hold Aux or Aux2 and press Power
//
// While saber is ON:
//   Non-impact Clash/Lockup: Click/Hold Power
//                            * This generates a clash/lockup effect with no
//                            * impact to the blade; quick press for a short
//                            * clash, hold for a lockup
//             Blaster Block: Click Aux
//           Lightning Block: Hold Aux
//                    Lockup: Hold Aux during impact
//                      Drag: Hold Aux during impact with saber pointed down.
//                      Melt: Hold Aux and stab
//
//         Enter Volume Menu: Double-click and hold Aux for 1 second
//                            * Be aware that the first click will trigger a
//                            * blaster block
//           Increase Volume: Rotate hilt right while in Volume Menu
//           Decrease Volume: Rotate hilt left while in Volume Menu
//   Save & Exit Volume Menu: Click Power
// Cancel & Exit Volume Menu: Click Aux
//
//   Enter Color Change Mode: Triple-click and hold Aux for 1 second
//                            * Be aware that the first two clicks will
//                            * trigger blaster blocks
//              Change Color: Rotate hilt while in Color Change Mode
//                Color Zoom: Press & Hold Power and Rotate hilt
//                            * This will fine-tune the selected color before
//                            * saving your change
//  Save & Exit Color Change: Release Power while in Color Zoom
// Reset & Exit Color Change: Click Aux
//
//                  Turn Off: Hold Aux or Aux2 and press Power
//
// You will need the following sound files in order for menus to work properly:
// vmbegin.wav - Enter Volume Change Menu
// vmend01.wav - Save Volume Change
// vmend02.wav - Cancel Volume Change
// monce.wav   - Set Track Player to play a single track one time
// mloop.wav   - Set Track Player to repeat a single track
// mrotate.wav - Set Track Player to repeat all tracks
// ccbegin.wav - Enter Color Change Mode
// ccend01.wav - Save Color and Exit Color Change Mode
// ccend02.wav - Reset Color and Exit Color Change Mode
//
// Options you can add to your config file:
// #define DISABLE_COLOR_CHANGE   - Disables the color change menu.
// #define CAIWYN_DISABLE_BEEPS   - Disables the single beep that occurs when
//                                  selecting fonts, tracks, or entering or
//                                  exiting the volume and color change menus.
//                                  Note that beeps will still play if any
//                                  sound file listed above is missing.
// #define CAIWYN_BATTERY_PROMPTS - To use your own sound files when checking
//                                  battery level instead of Proffie's built-in
//                                  voice.
//                                  This will require additional sound files:
//                                  mbatt.wav, mpercent.wav, mzero.wav,
//                                  mnum1.wav - mnum20.wav,
//                                  thirty.wav - ninety.wav, hundred.wav
// #define CAIWYN_NOCLASH_LOCKUP  - When you click the power button, both a
//                                  clash sound and a lockup are simultaneously
//                                  triggered.  This is because I have found
//                                  that quickly starting and ending a lockup
//                                  often sounds like multiple clashes rather
//                                  than a single clash.
//                                  To smooth everything out, a clash sound is
//                                  mixed in as well.  This works for most
//                                  fonts, but if it sounds weird you can
//                                  disable the clash sound and just use the
//                                  lockup by itself.

#ifndef PROPS_CAIWYN_BUTTONS_H
#define PROPS_CAIWYN_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"
#include "../sound/sound_library.h"

#undef PROP_TYPE
#define PROP_TYPE CaiwynButtons

#if NUM_BUTTONS < 2
#error /props/saber_caiwyn_buttons.h requires 2 or 3 Buttons for operation
#endif

#ifndef BUTTON_DOUBLE_CLICK_TIMEOUT
#define BUTTON_DOUBLE_CLICK_TIMEOUT 500
#endif

#ifndef BUTTON_SHORT_CLICK_TIMEOUT
#define BUTTON_SHORT_CLICK_TIMEOUT 500
#endif

#ifndef BUTTON_HELD_TIMEOUT
#define BUTTON_HELD_TIMEOUT 500
#endif

#ifndef BUTTON_HELD_MEDIUM_TIMEOUT
#define BUTTON_HELD_MEDIUM_TIMEOUT 1000
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif

EFFECT(vmbegin);
EFFECT(vmend);
EFFECT(monce);
EFFECT(mloop);
EFFECT(mrotate);

// The Saber class implements the basic states and actions for the saber.
class CaiwynButtons : public PropBase {
public:
  CaiwynButtons() : PropBase() {}
  const char* name() override { return "CaiwynButtons"; }

  void Loop() override {
    PropBase::Loop();
    DetectMenuTurn();
    TrackPlayer();
    sound_library_.Poll(wav_player_);
  }

  virtual void SetPreset(int preset_num, bool announce) override {
    PropBase::SetPreset(preset_num, announce);
    strcpy(current_track_,current_preset_.track.get());
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (PropBase::Parse(cmd, arg)) return true;
    if (!strcmp(cmd, "list_current_tracks")) {
// Tracks must be in: <font>/tracks/*.wav
      LOCK_SD(true);
      for (const char* dir = current_directory; dir; dir = next_current_directory(dir)) {
        PathHelper path(dir, "tracks");
        ListTracks(path);
      }
      LOCK_SD(false);
      return true;
    }
    return false;
  }

  void VolumeMenu() {
    reset_volume_ = dynamic_mixer.get_volume();
    current_menu_angle_ = fusor.angle2();
    mode_volume_ = true;
    if (SFX_vmbegin) {
#ifndef CAIWYN_DISABLE_BEEPS
      beeper.Beep(0.1, 2000);
#endif
      hybrid_font.PlayPolyphonic(&SFX_vmbegin);
    } else {
      beeper.Beep(0.20,1000.0);
      beeper.Beep(0.20,1414.2);
      beeper.Beep(0.20,2000.0);
    }
    STDOUT.println("Enter Volume Menu");
  }

  void VolumeUp() {
    STDOUT.println("Volume up");
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME, dynamic_mixer.get_volume() + VOLUME * 0.10));
      STDOUT.print("Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    }
  }

  void VolumeDown() {
    STDOUT.println("Volume Down");
    if (dynamic_mixer.get_volume() > (VOLUME * 0.10)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.10, dynamic_mixer.get_volume() - VOLUME * 0.10));
      STDOUT.print("Current Volume: ");
      STDOUT.println(dynamic_mixer.get_volume());
    }
  }

  void VolumeSave() {
    mode_volume_ = false;
    if (SFX_vmend) {
#ifndef CAIWYN_DISABLE_BEEPS
      beeper.Beep(0.1, 2000);
#endif
      SFX_vmend.Select(0);
      hybrid_font.PlayPolyphonic(&SFX_vmend);
    } else {
      beeper.Beep(0.20,2000.0);
      beeper.Beep(0.20,1414.2);
      beeper.Beep(0.20,1000.0);
    }
    STDOUT.println("Exit Volume Menu");
  }

  void VolumeCancel() {
    dynamic_mixer.set_volume(reset_volume_);
    mode_volume_ = false;
    if (SFX_vmend) {
#ifndef CAIWYN_DISABLE_BEEPS
      beeper.Beep(0.1, 2000);
#endif
      SFX_vmend.Select(1);
      hybrid_font.PlayPolyphonic(&SFX_vmend);
    } else {
      beeper.Beep(0.20,2000.0);
      beeper.Beep(0.20,1414.2);
      beeper.Beep(0.20,1000.0);
    }
    STDOUT.println("Volume Change Cancelled");
    STDOUT.print("Current Volume: ");
    STDOUT.println(dynamic_mixer.get_volume());
  }

  void DetectMenuTurn() {
    if (mode_volume_) {
      float a = fusor.angle2() - current_menu_angle_;
      if (a > M_PI) a-=M_PI*2;
      if (a < -M_PI) a+=M_PI*2;
      if (a > M_PI / 6) {
        VolumeUp();
        current_menu_angle_ = fusor.angle2();
      }
      if (a < -M_PI / 6) {
        VolumeDown();
        current_menu_angle_ = fusor.angle2();
      }
    }
  }

// Fett263 Track Player, modified by Caiwyn
  enum TrackMode {
    PLAYBACK_ONCE,
    PLAYBACK_LOOP,
    PLAYBACK_ROTATE,
  };

  void TrackPlayer() {
    if (track_player_on_ && !track_player_->isPlaying()) {
      if (track_player_) track_player_.Free();
      switch (track_mode_) {
        case PLAYBACK_LOOP:
          if (num_tracks_ <= 0) {
            StartOrStopTrack();
          } else {
            PlayTrack();
          }
          break;
        case PLAYBACK_ROTATE:
          NextTrack();
          break;
        default:
          track_player_on_ = false;
      }
    }
  }

  void PlayTrack() {
    if (!current_track_) {
      strcpy(current_track_,current_preset_.track.get());
    }
    MountSDCard();
    EnableAmplifier();
    track_player_ = GetFreeWavPlayer();
    if (track_player_) {
      track_player_->Play(current_track_);
    } else {
      STDOUT.println("No available WAV players.");
    }
  }

  void StartTrackPlayer() {
    num_tracks_ = RunCommandAndGetSingleLine("list_current_tracks", nullptr, 0, 0, 0);
    if (num_tracks_ > 0) {
      PlayTrack();
    } else {
      StartOrStopTrack();
    }
    track_player_on_ = true;
  }

  void StopTrackPlayer() {
    track_player_on_ = false;
    if (track_player_) {
      track_player_->Stop();
      track_player_.Free();
    } else {
      StartOrStopTrack();
    }
  }

  void NextTrack() {
    if (num_tracks_ > 0) {
      if (track_player_->isPlaying()) track_player_->Stop();
      if (track_player_) track_player_.Free();
      char next_track[128];
      next_track[0] = 0;
      if (!RunCommandAndFindNextSortedLine<128>("list_current_tracks", nullptr, current_track_, next_track, false)) {
        RunCommandAndFindNextSortedLine<128>("list_current_tracks", nullptr, nullptr, next_track, false);
      }
      strcpy(current_track_, next_track);
#ifdef SAVE_TRACK
      current_preset_.track = mkstr(current_track_);
      current_preset_.Save();
#endif
      PlayTrack();
    } else {
      StartOrStopTrack();
      StartOrStopTrack();
    }
  }

  void ChangeTrackMode() {
    switch (track_mode_) {
      case PLAYBACK_ONCE:
        track_mode_ = PLAYBACK_LOOP;
        if (SFX_mloop) {
#ifndef CAIWYN_DISABLE_BEEPS
          beeper.Beep(0.1, 2000);
#endif
          hybrid_font.PlayPolyphonic(&SFX_mloop);
        } else {
          beeper.Beep(0.20,1000);
          beeper.Beep(0.20,2000);
          beeper.Beep(0.20,1000);
        }
        break;
      case PLAYBACK_LOOP:
        track_mode_ = PLAYBACK_ROTATE;
        if (SFX_mrotate) {
#ifndef CAIWYN_DISABLE_BEEPS
          beeper.Beep(0.1, 2000);
#endif
          hybrid_font.PlayPolyphonic(&SFX_mrotate);
        } else {
          beeper.Beep(0.20,1000);
          beeper.Beep(0.20,2000);
          beeper.Beep(0.20,1000);
        }
        break;
      default:
        track_mode_ = PLAYBACK_ONCE;
        if (SFX_monce) {
#ifndef CAIWYN_DISABLE_BEEPS
          beeper.Beep(0.1, 2000);
#endif
          hybrid_font.PlayPolyphonic(&SFX_monce);
        } else {
          beeper.Beep(0.20,1000);
          beeper.Beep(0.20,2000);
          beeper.Beep(0.20,1000);
        }
    }
  }

#ifndef DISABLE_COLOR_CHANGE
// Revert color change (reset to Variation == 0)
  void ResetColorChangeMode() {
    if (!current_style()) return;
    if (SFX_ccend) {
#ifndef CAIWYN_DISABLE_BEEPS
      beeper.Beep(0.1, 2000);
#endif
      SFX_ccend.Select(1);
    }
    STDOUT << "Reset Color Variation" << "\n";
    SetVariation(0);
    STDOUT << "Color change mode done, variation = 0" << "\n";
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
  }
#endif

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

// Activate Saber
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        On();
        return true;

// Activate Saber and Play Track
#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF | BUTTON_AUX2):
#else
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF | BUTTON_AUX):
#endif
        On();
        if (!track_player_) {
          StartTrackPlayer();
        }
        return true;

// Deactivate Saber
#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON | BUTTON_AUX2):
#else
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON | BUTTON_AUX):
#endif
        if (!mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
          Off();
          return true;
        }
        break;

// Start or Stop Track
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        if (track_player_) {
          StopTrackPlayer();
        } else {
          StartTrackPlayer();
        }
        return true;

// Next Preset/Track
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
#ifndef CAIWYN_DISABLE_BEEPS
        beeper.Beep(0.1, 2000);
#endif
        if (track_player_) {
          NextTrack();
        } else {
          next_preset();
          strcpy(current_track_,current_preset_.track.get());
          track_mode_ = PLAYBACK_ONCE;
        }
        return true;

// Battery Level
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
        if (!track_player_on_ && !wav_player_->isPlaying()) {
#ifdef CAIWYN_BATTERY_PROMPTS
          sound_library_.SayBatteryLevel();
          sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
          sound_library_.SayPercent();
#else
          talkie.Say(spBATTERYLEVEL);
          talkie.SayNumber((int)floorf(battery_monitor.battery_percent()));
          talkie.Say(spPERCENT);
#endif
          SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
          return true;
        }
        break;

// Track Playback Mode
      case EVENTID(BUTTON_AUX, EVENT_SECOND_CLICK_SHORT, MODE_OFF):
        ChangeTrackMode();
        return true;

// Non-Impact Clash and Lockup / Zoom Color
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
        if (!SaberBase::Lockup() && !mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          SaberBase::DoBeginLockup();
#ifndef CAIWYN_NOCLASH_LOCKUP
          hybrid_font.PlayPolyphonic(&SFX_clsh);
#endif
          return true;
#ifndef DISABLE_COLOR_CHANGE
        } else if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_SMOOTH) {
#ifndef CAIWYN_DISABLE_BEEPS
          if (SFX_ccend) {
            beeper.Beep(0.1, 2000);
          }
#endif
          SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_ZOOMED);
          return true;
#endif;
        }
        break;

// Save Volume / Save Color
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
        if (mode_volume_) {
          VolumeSave();
          return true;
#ifndef DISABLE_COLOR_CHANGE
        } else if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_STEPPED) {
          if (SFX_ccend) {
#ifndef CAIWYN_DISABLE_BEEPS
            beeper.Beep(0.1, 2000);
#endif
            SFX_ccend.Select(0);
          }
          ToggleColorChangeMode();
          return true;
#endif
        }
        break;

#ifndef DISABLE_COLOR_CHANGE
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ON):
        if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_ZOOMED) {
          if (SFX_ccend) {
            SFX_ccend.Select(0);
          }
          ToggleColorChangeMode();
          return true;
        }
        break;
#endif

// Cancel Volume / Reset Color / Blaster Block
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
        if (mode_volume_) {
          VolumeCancel();
#ifndef DISABLE_COLOR_CHANGE
        } else if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          ResetColorChangeMode();
#endif
        } else {
          SaberBase::DoBlast();
        }
        return true;

// Lockup/Drag
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup() && !mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
          if (accel_.x < -0.15) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          }
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Lightning Block
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD, MODE_ON):
        if (!SaberBase::Lockup() && !mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
          SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Melt
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
        if (!SaberBase::Lockup() && !mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

// Enter Volume Menu
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
        if (!mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
          VolumeMenu();
          return true;
        }
        break;

// Color Change
#ifndef DISABLE_COLOR_CHANGE
      case EVENTID(BUTTON_AUX, EVENT_THIRD_HELD_MEDIUM, MODE_ON):
        if (!mode_volume_) {
#ifndef CAIWYN_DISABLE_BEEPS
          if (SFX_ccbegin) {
            beeper.Beep(0.1, 2000);
          }
#endif
          ToggleColorChangeMode();
          return true;
        }
        break;
#endif

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

      // Events that need to be handled regardless of what other buttons are pressed.
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          return true;
        }
      }
      return false;
    }

private:
  bool mode_volume_ = false;
  int32_t reset_volume_ = VOLUME;
  float current_menu_angle_ = 0.0;
  TrackMode track_mode_ = PLAYBACK_ONCE;
  bool track_player_on_ = false;
  char current_track_[128];
  int num_tracks_;
  RefPtr<BufferedWavPlayer> wav_player_;
};

#endif
