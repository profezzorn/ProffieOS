// Jetpack Revision 51

/* Created by OlivierFlying747-8 with lots of help from Fredrik Hubinette aka profezzorn,
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2019 Fredrik Hubinette
  Copyright (c) 2025 OlivierFlying747-8 with contributions by:
  Fredrik Hubinette aka profezzorn,
  Ryan Ogurek aka ryryog25,
  Bryan Connor aka NoSloppy,
  In case of problem, you can find us at: https://crucible.hubbe.net somebody will be there to help.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/

Includes 1, 2 and 3 button controls. However, I would recommend a 2 or 3
buttons setup because I think a 1 button setup is "cumbersome" to use!

Buttons handeling was initially based on saber_sa22c_buttons.h

Explanation:

    First Power Button Press:
        Plays the jetpack idle starting sound.
        Loops the idle sound until 1 minute passes, the jetpack is started, or
        the aux button is pressed (plays a false start sound and restarts the 1-minute idle loop).
        If no "Second Power Button Press" within 1 minute, the jetpack will switch off.

    Second Power Button Press:
        Plays the jetpack starting sound.
        Loops the jetpack flying sound until the power button is pressed again.

    Third Power Button Press:
        Plays the jetpack shutting down to idle sound.
        Loops the idle sound until 1 minute passes, the jetpack is restarted, or the Aux button is pressed
        (restarts the 1-minute idle loop).

    Aux Button Presses: (for a 2 or 3 buttons setup)
        When flying: Plays a stuttering sound.
        When idle: Plays a false start sound and restarts the 1-minute idle loop.
        When off: Plays a self-destruct, melt-down or dud sound randomly! (surprise!)

    The jetpack turns off automatically if idle for more than 1 minute. This can be changed with
    "#define JETPACK_IDLE_TIME 1000 * 60 * 1" (millisec * sec * minutes) in your config, you can
    change the time to what you want (but don't make it zero or you will not be able to make the
    jetpack go into "flight mode". I have it by default to 1 min for testing/debugging purposes.
    I guess a good number, once I know everything is working as it should be, would be around 15
    to 20sec. Enough for various sound effects to complete, enough to give you time to restart or
    enough to play with "false start" function.

    Loop Function:
        Monitors the 1-minute timer (or user defined) during idle mode to turn off the jetpack
        completely if the time expires.
        Plays idle or flight mode on loop for the required time.

    To do: create animations for OLED for Missile Launch (based on Mando S02E06 - if you know, you know!)
    & make a fun jetpack sound font.
    Create DIM "mode" for "blades" in idle mode.

*/
/* ============= List of optional jetpack defines that you can add to your config. =============
#define JETPACK_IDLE_TIME 1000 * 60 * 1 // Jetpack max idle mode time in millisec (default 1 min) before auto-shutdown.
#define JETPACK_TRACK_PLAYER            // To add jetpack prop track player funtionality - almost identical to Fett263's saber
                                        // Fett263's saber track player, but without the menu option.
// TBA JETPACK_ALLOW_CRASHING           // if "shot at" // I could create it, if there is an interest!
*/
/* ============= CHECK THE BELLOW LIST TO ACTUAL BUTTON(S) COMMANDS! ============= NOT YET FINAL !!! ============= *\
1 Button:
POWER Button:

               Activate: Short-click while OFF.
Missile Launch Sequence: Double-click while ON or OFF.
        Jetpack Mishaps: Hold medium-duration click while ON or OFF.
         Activate Muted: Double-click and hold while OFF.
        Play/Stop Music: Hold and release while OFF.
         Turn Off Blade: Hold and wait until the blade turns off while ON.
            Next Preset: Hold and release while OFF.
        Previous Preset: Hold and wait while OFF.
      Color Change Mode: Triple-click while ON.
 Enter/Exit Volume Menu: Double-click and hold while OFF.
              Volume Up: Short-click while in Volume Menu.
            Volume Down: Long-click while in Volume Menu.
          Battery Level: Triple-click while OFF.

=================================================================================================================
2 Buttons:
POWER Button:

               Activate: Short-click while OFF.
Missile Launch Sequence: Double-click while ON or OFF.
        Jetpack Mishaps: Hold medium-duration click while ON or OFF.
         Activate Muted: Double-click and hold while OFF.
        Play/Stop Music: Hold and release while OFF.
         Turn Off Blade: Hold and wait until the blade turns off while ON.
              Volume Up: Short-click while in Volume Menu.
        Previous Preset: Hold and wait while OFF.

AUX Button:

            Next Preset: Short-click while OFF.
      Enter Volume Menu: Long-click while OFF.
            Volume Down: Short-click while in Volume Menu.
          Battery Level: Hold and release while OFF.
      Color Change Mode: Short-click while ON.
          == Additional: Hold and release while OFF.

=================================================================================================================
3 Buttons: Same as 2 buttons except for the following:
AUX2 Button:

        Previous Preset: Short-click while OFF.
      Color Change Mode: Short-click while ON.
          == Additional: Hold and release while OFF.

\* ============= CHECK THE ABOVE LIST TO ACTUAL BUTTON(S) COMMANDS! ============= NOT YET FINAL !!! ============= */

#ifndef PROPS_JETPACK_PROP_H
#define PROPS_JETPACK_PROP_H

#if NUM_BUTTONS < 1
#error Your prop NEEDS 1 or more buttons to use jetpack_prop (2 is recomended)
#endif

#include "prop_base.h"

#define PROP_TYPE Jetpack

#ifndef JETPACK_IDLE_TIME
#define JETPACK_IDLE_TIME 1000 * 60 * 1 // Jetpack max idle mode time in millisec (default 1 min) before auto-shutdown
#endif

// == sounds for system ==

// The long below list will not use additional memory. It is necessary in case you want to use jetpack_prop.h with
// dual_prop.h or my multi_prop.h to avoid double define errors of identical effects from other props.

// Props in alphabetical order, Sabers First, Blaster(s) Second, everything else, like the thermal detonator, Third!
// I placed the jetpack prop low on the multi_prop.h "food chain" (in Fourth)!

// Looking at all this code below, I am wondering which one was the first chicken and which one was
// the first egg!

#ifdef PROPS_SABER_BC_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#define HAVE_MUTE       // Notification before muted ignition to avoid confusion.
#endif

#ifdef PROPS_CAIWYN_BUTTONS_H
// NO EFFECTs in saber_caiwyn_buttons.h, pfew. That's one off the list (until/if it gets updated)!
#endif

#ifdef PROPS_SABER_FETT263_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#define HAVE_VMBEGIN    // for Begin Volume Menu
#define HAVE_VMEND      // for End Volume Menu
#endif

#ifdef PROPS_SABER_SA22C_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#define HAVE_VMBEGIN    // for Begin Volume Menu
#define HAVE_VMEND      // for End Volume Menu
#define HAVE_VOLDOWN    // for decrease volume
#define HAVE_VOLUP      // for increse volume
#define HAVE_VOLMAX     // for maximum volume reached
#define HAVE_VOLMIN     // for minimum volume reached
#endif

#ifdef PROPS_SABER_SABERSENSE_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#define HAVE_VMBEGIN    // for Begin Volume Menu
#define HAVE_VMEND      // for End Volume Menu
#define HAVE_VOLDOWN    // for decrease volume
#define HAVE_VOLUP      // for increse volume
#define HAVE_VOLMAX     // for maximum volume reached
#define HAVE_VOLMIN     // for minimum volume reached
#endif

#ifdef PROPS_SABER_SHTOK_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#endif

#ifdef PROPS_SABER_SABERTRIO_BUTTONS_H // I know, even them!
// I have been collecting props from every horizon for testing purposes.
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#define HAVE_MUTE       // for Mute Mode
#define HAVE_VMBEGIN    // for Begin Volume Menu
#define HAVE_VMEND      // for End Volume Menu
#define HAVE_VOLDOWN    // for decrease volume
#define HAVE_VOLUP      // for increase volume
#define HAVE_VOLMAX     // for maximum volume reached
#define HAVE_VOLMIN     // for minimum volume reached
#endif

#ifdef PROPS_BLASTER_BC_BUTTONS_H
#ifndef HAVE_BATTERY
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#endif
#ifndef HAVE_VMBEGIN
#define HAVE_VMBEGIN    // for Begin Volume Menu
#endif
#ifndef HAVE_VMEND
#define HAVE_VMEND      // for End Volume Menu
#endif
#ifndef HAVE_VOLDOWN
#define HAVE_VOLDOWN    // for decrease volume
#endif
#ifndef HAVE_VOLUP
#define HAVE_VOLUP      // for increse volume
#endif
#ifndef HAVE_VOLMAX
#define HAVE_VOLMAX     // for maximum volume reached
#endif
#ifndef HAVE_VOLMIN
#define HAVE_VOLMIN     // for minimum volume reached
#endif
#endif

#ifndef HAVE_BATTERY
EFFECT(battery);        // for EFFECT_BATTERY_LEVEL
#endif
#ifndef HAVE_DIM
EFFECT(dim);            // for EFFECT_POWERSAVE (dim blades while in idle mode)
#endif
#ifndef HAVE_MUTE
EFFECT(mute);           // Notification before muted ignition to avoid confusion.
#endif
#ifndef HAVE_VMBEGIN
EFFECT(vmbegin);        // for Begin Volume Menu
#endif
#ifndef HAVE_VMEND
EFFECT(vmend);          // for End Volume Menu
#endif
#ifndef HAVE_VOLDOWN
EFFECT(voldown);        // for decrease volume
#endif
#ifndef HAVE_VOLUP
EFFECT(volup);          // for increase volume
#endif
#ifndef HAVE_VOLMAX
EFFECT(volmax);         // for maximum volume reached
#endif
#ifndef HAVE_VOLMIN
EFFECT(volmin);         // for minimum volume reached
#endif

#ifdef HAVE_BATTERY
#undef HAVE_BATTERY     // for EFFECT_BATTERY_LEVEL
#endif
#ifdef HAVE_DIM
#undef HAVE_DIM         // for EFFECT_POWERSAVE
#endif
#ifdef HAVE_MUTE
#undef HAVE_MUTE        // Notification before muted ignition to avoid confusion.
#endif
#ifdef HAVE_VMBEGIN
#undef HAVE_VMBEGIN     // for Begin Volume Menu
#endif
#ifdef HAVE_VMEND
#undef HAVE_VMEND       // for End Volume Menu
#endif
#ifdef HAVE_VOLDOWN
#undef HAVE_VOLDOWN     // for decrease volume
#endif
#ifdef HAVE_VOLUP
#undef HAVE_VOLUP       // for increse volume
#endif
#ifdef HAVE_VOLMAX
#undef HAVE_VOLMAX      // for maximum volume reached
#endif
#ifdef HAVE_VOLMIN
#undef HAVE_VOLMIN      // for minimum volume reached
#endif

// == sounds for jetpack ==
EFFECT2(startidlemode,idlemode);     // jetpack starting to idle  (0 to idle)
EFFECT2(idlemode,idlemode);          // jetpack idle sound        (looping idle sound)
EFFECT2(startflightmode,flightmode); // jetpack starting sound    (idle to flight)
EFFECT2(flightmode,flightmode);      // jetpack flying sound      (looping flight sound)
EFFECT2(stopflightmode,idlemode);    // jetpack shutdown sound    (flight to idle)
EFFECT(stopidlemode);                // jetpack stopping sound    (idle to 0)
// == sounds for mishaps ==
EFFECT2(falsestart,idlemode);        // jetpack false start sound (pre-flight)
EFFECT2(stuttering,flightmode);      // jetpack stuttering sound  (in flight)
EFFECT(selfdestruct);                // jetpack exploding sound   (pre-idle)
EFFECT(meltdown);                    // jetpack melt-down sound   (pre-idle)
EFFECT(dud);                         // jetpack not exploding or melting down sound "Dank farrik, it's a dud!"
// == sounds for missile ==
EFFECT(aiming);                      // viewfinder coming down "click" sound
EFFECT(targetting);                  // viewfinder search/finds target sound
EFFECT(missilelaunch);               // missile launch sound
EFFECT(missilegoesboom);             // double explosion in the distance sound
EFFECT(mandotalk);                   // "Nice shot! I was aiming for the other one!
EFFECT(disarm);                      // viewfinder going back up (reverse "click") sound "shlack"

class Jetpack : public PROP_INHERIT_PREFIX PropBase {
public:
  Jetpack() : PropBase() {}
  const char* name() override { return "Jetpack"; }

  // Based on SA22C Volume Menu
  void VolumeUp() {
    PVLOG_DEBUG << "Volume Up\n";
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1, dynamic_mixer.get_volume() + VOLUME * 0.1));
      if (!hybrid_font.PlayPolyphonic(&SFX_volup)) beeper.Beep(0.5, 2000);
      PVLOG_DEBUG << "Volume Up - Current Volume: " << dynamic_mixer.get_volume() << "\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    } else {
      // Cycle through ends of Volume Menu option
#ifdef VOLUME_MENU_CYCLE
      if (!max_vol_reached_) {
        if (!hybrid_font.PlayPolyphonic(&SFX_volmax)) beeper.Beep(0.5, 3000);
        PVLOG_DEBUG << "Maximum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        max_vol_reached_ = true;
      } else {
        dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
        dynamic_mixer.get_volume() - VOLUME * 0.9));
        if (!hybrid_font.PlayPolyphonic(&SFX_volmin)) beeper.Beep(0.5, 1000);
          PVLOG_DEBUG << "Minimum Volume: \n";
          SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
          max_vol_reached_ = false;
      }
#else
      if (!hybrid_font.PlayPolyphonic(&SFX_volmax)) beeper.Beep(0.5, 3000);
        PVLOG_DEBUG << "Maximum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
#endif
    }
  }

  // Based on SA22C Volume Menu
  void VolumeDown() {
    PVLOG_DEBUG << "Volume Down\n";
    if (dynamic_mixer.get_volume() > (0.1 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1, dynamic_mixer.get_volume() - VOLUME * 0.1));
      if (!hybrid_font.PlayPolyphonic(&SFX_voldown)) beeper.Beep(0.5, 2000);
      PVLOG_DEBUG << "Volume Down - Current Volume: " << dynamic_mixer.get_volume() << "\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    } else {
      // Cycle through ends of Volume Menu option
#ifdef VOLUME_MENU_CYCLE
      if (!min_vol_reached_) {
        if (!hybrid_font.PlayPolyphonic(&SFX_volmin)) beeper.Beep(0.5, 1000);
        PVLOG_DEBUG << "Minimum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        min_vol_reached_ = true;
      } else {
        dynamic_mixer.set_volume(VOLUME);
        if (!hybrid_font.PlayPolyphonic(&SFX_volmax)) beeper.Beep(0.5, 3000);
        PVLOG_DEBUG << "Maximum Volume: \n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        min_vol_reached_ = false;
      }
#else
      if (!hybrid_font.PlayPolyphonic(&SFX_volmin)) beeper.Beep(0.5, 1000);
      PVLOG_DEBUG << "Minimum Volume: \n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
#endif
    }
  }

#ifdef JETPACK_TRACK_PLAYER
  // Based on Fett263 Track Player Modes
  enum TrackMode {
    PLAYBACK_OFF,
    PLAYBACK_LOOP,
    PLAYBACK_ROTATE,
    PLAYBACK_RANDOM,
  };

  // Start and Stop Track Player
  void StartTrackPlayer() {
    if (num_tracks_ > 0) {
      istrack_playing_ = true;
      track_num_ = 1;
      track_mode_ = PLAYBACK_LOOP;
      PlayTrack();
    } else {
      // Loop default track if tracks not found
      track_num_ = 0;
      track_mode_ = PLAYBACK_LOOP;
      StartOrStopTrack();
    }
  }

  // Rotate Through Tracks
  void TrackPlayer() {
    if (track_mode_ != PLAYBACK_OFF) {
      if (!track_player_) {
        if (track_num_ <= 0 && track_mode_ == PLAYBACK_LOOP) {
          StartOrStopTrack();
          if (!track_player_ || !track_player_->isPlaying()) track_mode_ = PLAYBACK_OFF;
        } else {
          switch (track_mode_) {
            case PLAYBACK_ROTATE:
              track_num_ += 1;
              if (track_num_ > num_tracks_) track_num_ = 1;
              break;
            case PLAYBACK_RANDOM:
              track_num_ = rand() % num_tracks_;
              if (track_num_ <= 0) track_num_ = num_tracks_;
              break;
            default:
              break;
          }
          PlayTrack();
        }
      }
    } else if (track_player_ && !track_player_->isPlaying()) track_player_.Free();
  }

  void PlayEffectTrack() {
    if (!track_player_->isPlaying()) {
      if (num_tracks_ > 0) {
        if (track_num_ <= 0) track_num_ = 1;
        PlayTrack();
      } else {
        StartOrStopTrack();
      }
    }
  }

  void PlayTrack() {
    if (num_tracks_ == 0) {
      PVLOG_DEBUG << "No tracks available to play.\n";
      return;
    }
    char playtrack[128];
    RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, playtrack, sizeof(playtrack));
    MountSDCard();
    EnableAmplifier();
    track_player_ = GetFreeWavPlayer();
    if (track_player_) track_player_->Play(playtrack);
    else PVLOG_DEBUG << "No available WAV players.\n";
  }

  void StopTrackPlayer() {
    track_mode_ = PLAYBACK_OFF;
    if (track_player_) {
      istrack_playing_ = false;
      track_player_->Stop();
      track_player_.Free();
    } else StartOrStopTrack();
  }

  void NextTrack() {
    if (num_tracks_ <= 0) {
      PVLOG_DEBUG << "No tracks available.\n";
      return;
    }
    track_num_ = (track_num_ % num_tracks_) + 1; // Cycle to the next track
    PlayTrack();
  }

  void PreviousTrack() {
    if (num_tracks_ <= 0) {
      PVLOG_DEBUG << "No tracks available.\n";
      return;
    }
    track_num_ = (track_num_ - 2 + num_tracks_) % num_tracks_ + 1; // Cycle to the previous track
    PlayTrack();
  }

// only one prop can have chdir & Parse, so in a dual_prop.h/multi_prop.h "environment" SaberFett263Button already has them.
#ifndef PROPS_SABER_FETT263_BUTTONS_H
  bool chdir(const StringPiece dir) override {
    bool ret = PropBase::chdir(dir);
    num_tracks_ = RunCommandAndGetSingleLine("list_current_tracks", nullptr, 0, 0, 0);
    track_num_ = 0;
    return ret;
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (PropBase::Parse(cmd, arg)) return true;
    if (!strcmp(cmd, "list_current_tracks")) {
    //PVLOG_NORMAL << "Tracks available: " << num_tracks_ << " (including default: " << num_default_tracks_ << ")\n";
      LOCK_SD(true);
      for (const char* dir = current_directory; dir; dir = next_current_directory(dir)) {
        PathHelper path(dir, "tracks");
        ListTracks(path);
      }
      LOCK_SD(false);
      return true;
    }
    if (!strcmp(cmd, "list_current_tracks")) {
      // Tracks must be in: font/tracks/*.wav
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
#endif // PROPS_SABER_FETT263_BUTTONS_H
#endif // JETPACK_TRACK_PLAYER

  // I used SA22C Event2 as a base, and I added my "twist" on it, since a jetpack prop/backpack with a
  // Proffie board in it can't realisticly have twist or stab motion events.
  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
#ifdef BLADE_DETECT_PIN
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        blade_detected_ = true;
        FindBladeAgain();
        SaberBase::DoBladeDetect(true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        blade_detected_ = false;
        FindBladeAgain();
        SaberBase::DoBladeDetect(false);
        return true;
#endif // BLADE_DETECT_PIN
      // Jetpack ON AND Volume Up:
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        if (!mode_volume_) {
          if (idle_)
            StartFlightMode();  // Transition from idle to flight  (rev more up)
          else
            if (flight_)
              StopFlightMode(); // Transition from flight to idle     (rev down)
            else
              StartIdleMode();  // Jetpack initial start from 0 to idle (rev up)
              // if idle, it revs up / if up, it revs down to idle / if not idle or not up, it needs to rev "up" to idle.
        } else {
            VolumeUp();
        }
        return true;
      // Completely turn jetpack off from any state ******** only for 2 & 3 buttons (while on): ********
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_HELD_LONG, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_ON):
        StopIdleMode(); // This will stop the jetpack but not the missile.
        return true;
      // Jetpack mishaps:
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        JetpackMishaps();
        return true;
      // Missile Launch:
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        missilelaunch_ = true;
        return true;

      // Abort missile sequence, maybe? ******** only for (2 & 3 buttons while off): ********
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        missilelaunch_ = false;
        state_machine_.reset_state_machine();
        PVLOG_NORMAL << "Missile Sequence Aborted.\n";
        SaberBase::DoEffect(EFFECT_DISARM,0);
        return true;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      // Next Preset (1 button):
#if NUM_BUTTONS == 1
        if (!mode_volume_)
          next_preset();
#else
        // Start or Stop Track (2 and 3 buttons):
#ifdef JETPACK_TRACK_PLAYER
        if (!istrack_playing_)
          StartTrackPlayer();
        else
          StopTrackPlayer();
#else
        StartOrStopTrack();
#endif
#endif // NUM_BUTTONS == 1
        return true;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON):
        // Volume Down (1 button):
        if (mode_volume_)
          VolumeDown();
        return true;

      // Next Preset and Volume Down (2 and 3 buttons):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT, MODE_OFF):
        if (!mode_volume_)
          next_preset();
        else
          VolumeDown();
        return true;
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT, MODE_ON):
        if (mode_volume_)
          VolumeDown();
        return true;

#if NUM_BUTTONS >= 3
      // Previous Preset (3 buttons):
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_CLICK_SHORT, MODE_OFF):
#else
      // Previous Preset (1 and 2 buttons):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF):
#endif // NUM_BUTTONS >= 3
        if (!mode_volume_)
          previous_preset();
        return true;

      // Activate Muted (Turn's Saber ON Muted):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_OFF):
        if (SetMute(true)) {
          unmute_on_deactivation_ = true;
          if (!hybrid_font.PlayPolyphonic(&SFX_mute)) { // was announcemode(&SFX_mute);
            // Use beeper for fallback sounds
            beeper.Beep(0.05, 2000);
            beeper.Silence(0.05);
            beeper.Beep(0.05, 2000);
          }
          FastOn();
        }
        return true;

#ifndef DISABLE_COLOR_CHANGE
      // Color Change mode (1 button):
#if NUM_BUTTONS == 1
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
      // Color Change mode (2 buttons):
#elif NUM_BUTTONS == 2
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT, MODE_ON):
#elif // Color Change mode (3 buttons):
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_CLICK_SHORT, MODE_ON):
#endif
        ToggleColorChangeMode();
        return true;
#endif // DISABLE_COLOR_CHANGE

#if NUM_BUTTONS == 1
      // Start or Stop Track (1 button):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_ON):
#ifdef JETPACK_TRACK_PLAYER
        if (!istrack_playing_)
          StartTrackPlayer();
        else
          StopTrackPlayer();
#else
        StartOrStopTrack();
#endif
        return true;
#endif // NUM_BUTTONS == 1

#ifdef JETPACK_TRACK_PLAYER
      // Play Next Track (2 & 3 buttons):
#if NUM_BUTTONS > 1
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD_LONG, MODE_OFF):
#else
      // Play Next Track (1 button):
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_LONG, MODE_ON):
#endif
        NextTrack();
        return true;

      // Play Previous Track (2 and 3 buttons):
#if NUM_BUTTONS > 1
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
#else
      // Play Previous Track (1 button):
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_ON):
#endif
        PreviousTrack();
        return true;
#endif // JETPACK_TRACK_PLAYER

#if NUM_BUTTONS == 1
      // Enter/Exit Volume MENU (1 button):
      case EVENTID(BUTTON_NONE, EVENT_SECOND_HELD, MODE_ON):
#else
      // Enter/Exit Volume MENU (2 and 3 buttons):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_HELD, MODE_OFF):
#endif // NUM_BUTTONS == 1
        if (!mode_volume_) {
          mode_volume_ = true;
          if (!hybrid_font.PlayPolyphonic(&SFX_vmbegin)); beeper.Beep(0.5, 3000);
          PVLOG_NORMAL << "Enter Volume Menu\n";
        } else {
          mode_volume_ = false;
          if (!hybrid_font.PlayPolyphonic(&SFX_vmend)); beeper.Beep(0.5, 3000);
          PVLOG_NORMAL << "Exit Volume Menu\n";
        }
        return true;

      // Battery level:
#if NUM_BUTTONS == 1
      // 1 button
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
#else
      // 2 and 3 buttons
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_OFF):
#endif // NUM_BUTTONS == 1
        talkie.SayDigit((int)floorf(battery_monitor.battery()));
        talkie.Say(spPOINT);
        talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
        talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
        talkie.Say(spVOLTS);
        return true;
    }  // switch (EVENTID)
    return false;  // No action
  }  // Event2

  // I want the effects to play one after the other without overlapping or interrupting one another.
  void PerformMissileLaunchSequence() {
    STATE_MACHINE_BEGIN();
    while (true) {
      if (missilelaunch_) {
        PVLOG_NORMAL << "Starting Missile Launch Sequence\n";
        // create animation for OLED of viewfinder coming down
        SaberBase::DoEffect(EFFECT_AIMING,0);
        SLEEP(SaberBase::sound_length * 1000 + 10);
        // create animation for OLED of targetting
        SaberBase::DoEffect(EFFECT_TARGETTING,0);
        SLEEP(SaberBase::sound_length * 1000);
        // create animation for OLED of jetpack launching missile
        SaberBase::DoEffect(EFFECT_MISSILELAUNCH,0);
        SLEEP(SaberBase::sound_length * 1000);
        // create animation for OLED of explosion
        SaberBase::DoEffect(EFFECT_MISSILEGOESBOOM,0);
        SLEEP(SaberBase::sound_length * 1000 + 10);
        if (!flight_) { // Perform "Mando Talk" if not in flight mode!
          // create animation for OLED of mando & boba talking
          SaberBase::DoEffect(EFFECT_MANDOTALK,0);
          SLEEP(SaberBase::sound_length * 1000 + 10);
          PVLOG_NORMAL << "Mando: Nice shot!\nBoba: I was aiming for the other one!\n";
        }
        // create animation for OLED of viewfinder going back up
        SaberBase::DoEffect(EFFECT_DISARM,0);
        SLEEP(SaberBase::sound_length * 1000);
        PVLOG_NORMAL << "Missile Launch Sequence Completed!\n";
        missilelaunch_ = false;
        }
      YIELD();  // this means "sleep one loop"
      }
    STATE_MACHINE_END();
  }

  // Handles Jetpack mishaps events
  void JetpackMishaps() {
    if (jetpack_wav_player_.isPlaying()) {
      return; // Wait for the current sound to finish
    }
    if (flight_) {
      // Jetpack flying, play stuttering sound
      jetpack_wav_player_.StopFromReader();     // Stop flight sound
      SaberBase::DoEffect(EFFECT_STUTTERING,0); // Play stuttering sound
    } else if (idle_) {
      // Jetpack idle, play false start and reset idle timer
      jetpack_wav_player_.StopFromReader();     // Stop idle sound
      SaberBase::DoEffect(EFFECT_FALSESTART,0); // Play false start sound
      timer_ = millis(); // Reset idle timer
    } else ChooseRandomEffect(); // Play random mishap effect
  }

  // Jetpack off, play self destruct, meltdown or dud (randomly between the three)
  void ChooseRandomEffect() {
    int choice = random(3); // Generate a random number: 0, 1, or 2
    switch (choice) {
      case 0:  SaberBase::DoEffect(EFFECT_SELFDESTRUCT,0); break;
      case 1:  SaberBase::DoEffect(EFFECT_MELTDOWN,0);     break;
      case 2:  SaberBase::DoEffect(EFFECT_DUD,0);          break;
      default: SaberBase::DoEffect(EFFECT_SELFDESTRUCT,0); break; // <-- Is a "default" even needed ???
    }
  }

  // Initial start, transition to Idle Mode (from 0 to idle)
  void StartIdleMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return; // Wait for the current sound to finish
    }
    SaberBase::DoEffect(EFFECT_STARTIDLEMODE, 0); // start transition to idle mode
    flight_ = false;
    idle_   = true;
    timer_  = millis(); // Reset idle timer
    FastOn();
    PVLOG_STATUS << "Jetpack Idling\n";
  }

  // Transition to Flying Mode (from idle to flying)
  void StartFlightMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return; // Wait for the current sound to finish
    }
    SaberBase::DoEffect(EFFECT_STARTFLIGHTMODE, 0); // start transition to flight mode
    flight_ = true;
    idle_   = false;
    PVLOG_STATUS << "Jetpack Flying\n";
  }

  // Transition from Flying Mode (from flying to idle)
  void StopFlightMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return; // Wait for the current sound to finish
    }
    SaberBase::DoEffect(EFFECT_STOPFLIGHTMODE, 0); // start transition from flight mode
    flight_ = false;
    idle_   = true;
  }

  // Stop Idle Mode (Jetpack completely off - idle to off)
  void StopIdleMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return; // Wait for the current sound to finish
    }
    SaberBase::DoEffect(EFFECT_STOPIDLEMODE, 0); // start transition from idle mode
    flight_ = false; // Should already be false but StopIdleMode can be used to turn the jetpack completely off from any state.
    idle_   = false;
    Off(OFF_FAST); // Fully stop the jetpack
  }

  void Loop() override {
    unsigned long now = millis();

    // Perform the missile launch sequence
    PerformMissileLaunchSequence();

    // Stop idle mode after timeout
    if (idle_ && (now - timer_ > JETPACK_IDLE_TIME)) {
      StopIdleMode(); // Stop jetpack if idle for more than defined time
      return; // Exit early to avoid further processing
    }

    // Start idle loop after transition sound finishes
    if (idle_)
      IdlePlaying();

    // Start flight loop after transition sound finishes
    if (flight_)
      FlightPlaying();
  }

  void IdlePlaying() {
    if (idle_ && !jetpack_wav_player_.isPlaying()) {
      SaberBase::DoEffect(EFFECT_IDLEMODE,0); // Start idle loop
    }
  }

  void FlightPlaying() {
    if (flight_ && !jetpack_wav_player_.isPlaying()) {
      SaberBase::DoEffect(EFFECT_FLIGHTMODE,0); // Start flight loop
    }
  }

  void DoMotion(const Vec3&, bool) override { }  // jetpack doesn't have "movement effects", you
    // do not want to have to do front/back/side flips at your next cosplay to activate someting!
    // Or just imagine, for an instant, someone trying to replicate stab, aka running face first
    // into the nearest wall with their backpack/jetpack prop.

  // Helper (to avoid repetition)
  void EffectHelper (Effect* effect1,const char* msg1) {
    hybrid_font.PlayPolyphonic(effect1);
    PVLOG_STATUS << msg1 << "\n";
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;
      // Missile effects:
      case EFFECT_AIMING:          EffectHelper(&SFX_aiming,                   "Aiming"); // Must not be
      case EFFECT_TARGETTING:      EffectHelper(&SFX_targetting,           "Targetting"); // interrupted
      case EFFECT_MISSILELAUNCH:   EffectHelper(&SFX_missilelaunch,    "Missile Launch"); // and must
      case EFFECT_MISSILEGOESBOOM: EffectHelper(&SFX_missilegoesboom,"Missile Explodes"); // play in
      case EFFECT_MANDOTALK:       EffectHelper(&SFX_mandotalk,  "Mando & Boba Talking"); // sequence with
      case EFFECT_DISARM:          EffectHelper(&SFX_disarm,        "Disarm Targetting"); // one another!
      // Engine mishap effects:
      case EFFECT_FALSESTART:      EffectHelper(&SFX_falsestart,          "False Start");
      case EFFECT_STUTTERING:      EffectHelper(&SFX_stuttering,           "Stuttering");
      case EFFECT_SELFDESTRUCT:    EffectHelper(&SFX_selfdestruct,      "Self Destruct");
      case EFFECT_MELTDOWN:        EffectHelper(&SFX_meltdown,               "Meltdown");
      case EFFECT_DUD:             EffectHelper(&SFX_dud,   "Dank Farrik!\nIt's a Dud!");
      // Engine normal effects:
      case EFFECT_STARTIDLEMODE:   EffectHelper(&SFX_startidlemode,     "Jetpack Starting to Idle."); //after startidlemode, play idle mode on loop
      case EFFECT_IDLEMODE:        EffectHelper(&SFX_idlemode,         "Jetpack in Idle Loop Mode."); //plays on loop
      case EFFECT_STARTFLIGHTMODE: EffectHelper(&SFX_startflightmode, "Jetpack Starting to Flight."); //after startflightmode, play flight mode on loop
      case EFFECT_FLIGHTMODE:      EffectHelper(&SFX_flightmode,     "Jetpack in Flight Loop Mode."); //plays on loop
      case EFFECT_STOPFLIGHTMODE:  EffectHelper(&SFX_stopflightmode,"Jetpack Slowing Down to Idle."); //after stopflightmode, play idle on loop
      case EFFECT_STOPIDLEMODE:    EffectHelper(&SFX_stopidlemode,        "Jetpack Completely Off.");
      // System effects: (info found in config/styles/blade-effects.md, https://pod.hubbe.net/config/styles/blade-effects.html)
    //case EFFECT_VOLUME_LEVEL: return; // Shows volume level visually on blade, great for using with volume menu feature.
      case EFFECT_POWERSAVE:        if (!hybrid_font.PlayPolyphonic(&SFX_dim)) {
        // Typically used to apply a dimming effect on blade to conserve battery. Here it will be used for idle mode.
                                      beeper.Beep(0.1, 1300);
                                      beeper.Beep(0.1, 900);
                                      beeper.Beep(0.1, 600);
                                      beeper.Beep(0.1, 300);
                                    };
                                    return;
    //case EFFECT_ALT_SOUND:    return; // Triggers the change for sets of sounds within the font from one alternative to another.
      // https://pod.hubbe.net/sound/alt_sounds.html#have-the-prop-control-the-alt "SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, N);" N is the alt value!
#ifdef JETPACK_TRACK_PLAYER
      case EFFECT_TRACK:           PlayEffectTrack();
                                   return;
#endif
      // On-Demand Battery Level
      case EFFECT_BATTERY_LEVEL:    if (!hybrid_font.PlayPolyphonic(&SFX_battery)) { // Shows battery level visually on blade.
                                      beeper.Beep(1.0, 475);
                                      beeper.Beep(0.5, 693);
                                      beeper.Beep(0.16, 625);
                                      beeper.Beep(0.16, 595);
                                      beeper.Beep(0.16, 525);
                                      beeper.Beep(1.1, 950);
                                      beeper.Beep(0.5, 693);
                                      PVLOG_NORMAL << "May the Force be with you...always.\n";
                                    };
                                    return;
    }  // switch (effect)
  }  // SB_Effect

private:
  // State variables
  unsigned long timer_  = 0;            // Timer for idle mode timeout
  bool idle_            = false;        // Whether the jetpack is in  idle  mode
  bool flight_          = false;        // Whether the jetpack is in flight mode
  bool missilelaunch_   = false;        // Whether the missile is launching
  StateMachineState state_machine_;     // Required for the SLEEP macro
  // Sound effects handling
  PlayWav jetpack_wav_player_;          // Handles idle, flight, and mishap sounds
  // Track player handling
  bool istrack_playing_ = false;        // Tracks if track is currently playing (for Event2 switch)
  int  track_num_       = 0;            // Current track number
  int  num_tracks_      = 0;            // Total number of tracks available
  TrackMode track_mode_ = PLAYBACK_OFF; // Current playback mode
  // Volume handling
  bool mode_volume_     = false;        // from sa22c for volume menu
  bool max_vol_reached_ = false;        // from sa22c for volume menu
  bool min_vol_reached_ = false;        // from sa22c for volume menu
}; // class Jetpack

#endif // PROPS_JETPACK_PROP_H
