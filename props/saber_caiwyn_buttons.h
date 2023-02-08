// Caiwyn's Lightsaber Controls
// This config is designed to provide fast, low-latency response to button
// presses for the most basic functions (blaster blocks, lockups, and clashes)
// by omitting, or limiting non-critical features.
//
// At least 2 buttons are required.  A third button can be added, but is not
// necessary.
//
// While saber is OFF:
//               Next Preset: Hold Aux for 1 second when track is not playing
//             Check Battery: Double-click and hold Aux for 1 second when
//                            track is not playing
//          Start/Stop Track: Click Aux
//                Next Track: Hold Aux for 1 second while track is playing
//                            | Tracks must be stored in <font>/tracks/*.wav
//                            | and will be selected in alphabetical order.
//         Track Player Mode: Double-click and hold Aux for 1 second while
//                            track is playing
//                            | This cycles through three playback modes:
//                            | 1. Play a single track and stop (default)
//                            | 2. Repeat a single track in a loop
//                            | 3. Repeat all tracks in a loop
//             Turn Saber On: Press Power
//     Turn On & Start Track: Hold Aux or Aux2 and press Power
//
// While saber is ON:
//             Blaster Block: Click Aux
//                    Lockup: Hold Aux or Aux2 during impact
//                      Drag: Hold Aux or Aux2 during impact with blade pointed
//                            down.
//                      Melt: Hold Aux or Aux2 and stab
//           Lightning Block: Click/Hold Power
//
//         Enter Volume Menu: Double-click and hold Aux for 1 second
//                            | Be aware that the first click will trigger a
//                            | blaster block.
//           Increase Volume: Rotate hilt right while in Volume Menu
//           Decrease Volume: Rotate hilt left while in Volume Menu
//   Save & Exit Volume Menu: Click Power
//  Reset & Exit Volume Menu: Click Aux
//
//   Enter Color Change Mode: Triple-click and hold Aux for 1 second
//                            | Be aware that the first two clicks will
//                            | trigger blaster blocks.
//              Change Color: Rotate hilt while in Color Change Mode
//                Color Zoom: Press & Hold Power and Rotate hilt
//                            | This will fine-tune the selected color before
//                            | saving your change.
//  Save & Exit Color Change: Release Power while in Color Zoom
// Reset & Exit Color Change: Click Aux
//
//            Turn Saber Off: Hold Aux or Aux2 and press Power
//
// If CAIWYN_BUTTON_LOCKUP and/or CAIWYN_BUTTON_CLASH is defined:
//   Non-impact Clash/Lockup: Click/Hold Power
//                            | This generates a clash/lockup effect with no
//                            | impact to the blade; quick press for a short
//                            | clash, hold for a lockup.  This effect
//                            | replaces the Lightning Block for sabers with
//                            | only 2 buttons.
//           Lightning Block: Hold Aux2 and press Aux (requires 3 buttons)
//
// You will need the following sound files in order for menus to work properly:
// vmbegin.wav              - Enter Volume Change Menu
// vmend.wav                - Save Volume Change
// volmax.wav               - Reset to Maximum Volume
// monce.wav                - Set Track Player to play a single track one time
// mloop.wav                - Set Track Player to repeat a single track
// mrotate.wav              - Set Track Player to repeat all tracks
// ccbegin.wav              - Enter Color Change Mode
// ccend01.wav              - Save Color and Exit Color Change Mode
// ccend02.wav              - Reset Color and Exit Color Change Mode
// battlevl.wav             - Announce Current Battery Level
// mnum1.wav to mnum20.wav  - Individually Spoken Numbers
// thirty.wav to ninety.wav
// hundred.wav              - "Hundred"
// mpercent.wav             - "Percent"
// mselect.wav              - Beep to confirm menu selections (can be omitted)
// You can download a package containing all of these files here:
// https://drive.google.com/file/d/1cSBirX5STOVPanOkOlIeb0eofjx-qFmj/view
//
// Options you can add to your config file:
// #define CAIWYN_BUTTON_CLASH    - Enables a clash to be triggered without
//                                  impact to the blade by pressing the power
//                                  button.  This effect replaces lightning
//                                  blocks for two-button sabers, but if a
//                                  saber has three buttons, the lightning
//                                  block can be triggered by holding AUX2 and
//                                  pressing AUX.
//
// #define CAIWYN_BUTTON_LOCKUP   - Enables a lockup to be triggered without
//                                  impact to the blade by pressing and holding
//                                  the power button.
//                                  If both CAIWYN_BUTTON_LOCKUP and
//                                  CAIWYN_BUTTON_CLASH are defined, pressing
//                                  the power button triggers a lockup, but a
//                                  clash sound is overlayed on top of the
//                                  lockup sounds to smooth out the transition
//                                  from bgnlock and endlock sounds for quick
//                                  clashes.  I recommend this configuration.
//
// #define CAIWYN_SAVE_TRACKS     - Automatically saves the selected track for
//                                  each preset. Any time the user holds the
//                                  Aux button for one second to change the
//                                  active track, the preset is updated to
//                                  default to that track. This ONLY happens
//                                  when the user selects the track directly;
//                                  if the Track Player Mode is set to repeat
//                                  all tracks, the saved track is not updated
//                                  when the next track is automatically
//                                  played.
//                                  If SAVE_STATE is defined, then this will
//                                  automatically be defined as well.
//
// #define CAIWYN_SAVE_TRACK_MODE - Saves the selected track mode to a config
//                                  file so that the setting is restored when
//                                  Proffieboard boots up after the battery is
//                                  recharged or replaced.
//                                  If SAVE_STATE is defined, then this will
//                                  automatically be defined as well.
//
// #define DISABLE_COLOR_CHANGE   - Disables the color change menu.

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

#ifdef SAVE_STATE
#define CAIWYN_SAVE_TRACKS
#define CAIWYN_SAVE_TRACK_MODE
#endif

#ifdef CAIWYN_SAVE_TRACK_MODE
class SaveCaiwynStateFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(track_mode, 0);
  }
  int track_mode;
};
#endif

// The Saber class implements the basic states and actions for the saber.
class CaiwynButtons : public PropBase {
public:
  CaiwynButtons() : PropBase() {}
  const char* name() override { return "CaiwynButtons"; }

  void Loop() override {
    PropBase::Loop();
    DetectMenuTurn();
    PollTrackPlayer();
    sound_library_.Poll(wav_player_);
  }

  void SetPreset(int preset_num, bool announce) override {
    PropBase::SetPreset(preset_num, announce);
    strcpy(current_track_,current_preset_.track.get());
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (PropBase::Parse(cmd, arg)) return true;
    if (!strcmp(cmd, "list_current_tracks")) {
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
    current_menu_angle_ = fusor.angle2();
    mode_volume_ = true;
    sound_library_.SaySelect();
    sound_library_.SayEnterVolumeMenu();
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
    sound_library_.SaySelect();
    sound_library_.SayVolumeMenuEnd();
    STDOUT.println("Exit Volume Menu");
  }

  void VolumeReset() {
    dynamic_mixer.set_volume(VOLUME);
    mode_volume_ = false;
    sound_library_.SaySelect();
    sound_library_.SayMaximumVolume();
    STDOUT.println("Volume Reset");
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

  enum TrackMode {
    PLAYBACK_ONCE,
    PLAYBACK_LOOP,
    PLAYBACK_ROTATE,
  };

#ifdef CAIWYN_SAVE_TRACK_MODE
  SaveCaiwynStateFile saved_caiwyn_state;
  void RestoreCaiwynState() {
    saved_caiwyn_state.ReadINIFromDir(NULL, "caiwyn");
    track_mode_ = (TrackMode) saved_caiwyn_state.track_mode;
  }

  void SaveCaiwynState() {
    saved_caiwyn_state.track_mode = (int) track_mode_;
    saved_caiwyn_state.WriteToRootDir("caiwyn");
  }

  void Setup() override {
    RestoreCaiwynState();
  }
#endif

  void PollTrackPlayer() {
    if (track_player_on_ && !track_player_) {
      switch (track_mode_) {
        case PLAYBACK_ONCE:
          StopTrackPlayer();
          break;
        case PLAYBACK_LOOP:
          PlayTrack(current_track_);
          break;
        case PLAYBACK_ROTATE:
          NextTrack(false);
          break;
      }
    }
  }

  bool TrackExists(const char* track) {
    LOCK_SD(true);
    bool exists = LSFS::Exists(track);
    LOCK_SD(false);
    return exists;
  }

  void PlayTrack(const char* track) {
    if (!track_player_) track_player_ = GetFreeWavPlayer();
    if (track_player_) {
      track_player_->Play(track);
      track_player_on_ = true;
    } else {
      STDOUT.println("No available WAV players.");
      track_player_on_ = false;
      STDOUT.println("Track player stopped.");
    }
  }

  void StartTrackPlayer() {
    num_tracks_ = RunCommandAndGetSingleLine("list_current_tracks", nullptr, 0, 0, 0);
    if (!current_track_) strcpy(current_track_,current_preset_.track.get());
    if (!TrackExists(current_track_)) {
      STDOUT.print(current_track_);
      STDOUT.println(" not found.");
      if (num_tracks_ > 0) {
        RunCommandAndFindNextSortedLine<128>("list_current_tracks", nullptr, nullptr, current_track_, false);
      } else {
        STDOUT.println("No tracks found.");
        return;
      }
    }
    STDOUT.println("Track player started.");
    PlayTrack(current_track_);
  }

  void StopTrackPlayer() {
    track_player_on_ = false;
    if (track_player_ && track_player_->isPlaying()) track_player_->Stop();
    STDOUT.println("Track player stopped.");
#ifdef CAIWYN_SAVE_TRACK_MODE
    SaveCaiwynState();
#endif
  }

  void NextTrack(bool manually_selected) {
    if (track_player_ && track_player_->isPlaying()) track_player_->Stop();
    if (num_tracks_ > 0) {
      char next_track[128];
      next_track[0] = 0;
      if (!RunCommandAndFindNextSortedLine<128>("list_current_tracks", nullptr, current_track_, next_track, false)) {
        RunCommandAndFindNextSortedLine<128>("list_current_tracks", nullptr, nullptr, next_track, false);
      }
      strcpy(current_track_, next_track);
      if (manually_selected) {
#ifdef CAIWYN_SAVE_TRACKS
        current_preset_.track = mkstr(current_track_);
        current_preset_.Save();
#endif
        sound_library_.SaySelect();
      }
    }
    PlayTrack(current_track_);
  }

  void ChangeTrackMode() {
    sound_library_.SaySelect();
    switch (track_mode_) {
      case PLAYBACK_ONCE:
        track_mode_ = PLAYBACK_LOOP;
        sound_library_.SayLoop();
        STDOUT.println("Track player set to repeat single track.");
        break;
      case PLAYBACK_LOOP:
        track_mode_ = PLAYBACK_ROTATE;
        sound_library_.SayRotate();
        STDOUT.println("Track player set to repeat all tracks.");
        break;
      case PLAYBACK_ROTATE:
        track_mode_ = PLAYBACK_ONCE;
        sound_library_.Play("monce.wav");
        STDOUT.println("Track player set to play single track.");
        break;
    }
  }

  void CheckBattery() {
    SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
    if (SFX_mnum) {
      sound_library_.SaySelect();
      sound_library_.SayTheBatteryLevelIs();
      sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
      sound_library_.SayPercent();
    } else {
      talkie.Say(spBATTERYLEVEL);
      talkie.SayNumber((int)floorf(battery_monitor.battery_percent()));
      talkie.Say(spPERCENT);
    }
  }

#ifndef DISABLE_COLOR_CHANGE
// Revert color change (reset to Variation == 0)
  void ResetColorChangeMode() {
    if (!current_style()) return;
    if (SFX_ccend) {
      SFX_ccend.Select(1);
      sound_library_.SaySelect();
    }
    STDOUT << "Reset Color Variation" << "\n";
    SetVariation(0);
    STDOUT << "Color change mode done, variation = 0" << "\n";
    SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
  }
#endif

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

// Turn On Saber
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        On();
        return true;

// Turn On Saber and Play Track
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF | BUTTON_AUX):
#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF | BUTTON_AUX2):
#endif
        On();
        if (!track_player_on_) {
          StartTrackPlayer();
        }
        return true;

// Turn Off Saber
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON | BUTTON_AUX):
#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON | BUTTON_AUX2):
#endif
        if (!mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
          Off();
          return true;
        }
        break;

// Start or Stop Track
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        if (track_player_on_) {
          StopTrackPlayer();
        } else {
          StartTrackPlayer();
        }
        return true;

// Next Preset/Track
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
        if (track_player_on_) {
          NextTrack(true);
        } else {
          next_preset();
        }
        return true;

// Change Track Player Mode/Check Battery Level
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
        if (track_player_on_) {
          ChangeTrackMode();
          return true;
        } else if (!wav_player_->isPlaying()) {
          CheckBattery();
          return true;
        }
        break;

// Lightning Block / Non-Impact Clash and Lockup / Zoom Color
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
        if (!SaberBase::Lockup() && !mode_volume_ && (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE)) {
#ifdef CAIWYN_BUTTON_LOCKUP
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          SaberBase::DoBeginLockup();
#ifdef CAIWYN_BUTTON_CLASH
          hybrid_font.PlayPolyphonic(&SFX_clsh);
#endif
#elif defined(CAIWYN_BUTTON_CLASH)
          SaberBase::DoClash();
#else
          SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
          SaberBase::DoBeginLockup();
#endif
          return true;
#ifndef DISABLE_COLOR_CHANGE
        } else if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_SMOOTH) {
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
            SFX_ccend.Select(0);
            sound_library_.SaySelect();
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
            sound_library_.SaySelect();
          }
          ToggleColorChangeMode();
          return true;
        }
        break;
#endif

// Blaster Block / Reset Volume / Reset Color
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
        if (mode_volume_) {
          VolumeReset();
#ifndef DISABLE_COLOR_CHANGE
        } else if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          ResetColorChangeMode();
#endif
        } else {
          SaberBase::DoBlast();
        }
        return true;

// Lockup / Drag
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX2):
#endif
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

// Melt
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX2):
#endif
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
          sound_library_.SaySelect();
          ToggleColorChangeMode();
          return true;
        }
        break;
#endif

// Lightning Block
#if NUM_BUTTONS > 2 && defined(CAIWYN_BUTTON_LOCKUP)
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON | BUTTON_AUX2):
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON | BUTTON_AUX):
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
        SaberBase::DoBeginLockup();
        return true;
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
#if NUM_BUTTONS > 2
      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
#endif
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
  float current_menu_angle_ = 0.0;
  TrackMode track_mode_ = PLAYBACK_ONCE;
  bool track_player_on_ = false;
  char current_track_[128];
  int num_tracks_;
  RefPtr<BufferedWavPlayer> wav_player_;
};

#endif
