// JetpackOliButtons Revision 57

/* Created by OlivierFlying747-8 with lots of help from Fredrik Hubinette aka profezzorn,
  https://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2025 Fredrik Hubinette
  Copyright (c) 2025 OlivierFlying747-8 (Apologies for only adding my username, I would like to remain "anonymous" for the time being)
  With contributions by:
  Fredrik Hubinette aka profezzorn,
  Ryan Ogurek aka ryryog25,
  Bryan Connor aka NoSloppy,
  In case of problems, you can find us at: https://crucible.hubbe.net where somebody will be there to help.
  Distributed under the terms of the GNU General Public License v3.
  https://www.gnu.org/licenses/

Includes 1, 2 and 3 button controls. However, I would recommend a 2 or 3
buttons setup because I think a 1 button setup is cumbersome to use!

Buttons handling was initially based on saber_sa22c_buttons.h

You can find my free/royalty free test font here:
https://www.dropbox.com/scl/fi/jcu9pzaaeqo745y48bi4g/jetpack_Oli_buttons_testfont.zip?rlkey=r7xp96vrt8bhbeksux5rvcsnt&st=e6isx2vz&dl=0

Worth mentioning:
=================
    Unlike a saber, this jetpack doesn't have any "movement effects". The only one I could foresee being useful
    is clash ("tap or smash" your backpack/jetpack prop - but it is not in use for now). You do not want
    to have to do front/back/side flips at your next cosplay to activate something! Or just imagine,
    for an instant, someone trying to replicate a stab, aka running face-first into the nearest wall with
    their backpack/jetpack prop. Call me evil if you want, but this makes me laugh every time I picture it.

Disclaimer:
===========
    The explanations below are not fully 100 percent accurate, I just want to give you, the reader, an idea
    of what it's like. If you want to learn to fly, please go to flight school. I am not a flight instructor!

    Please do not use this as your first flying lesson! And if you do, you will probably crash and most
    likely die! Don't say: "I didn't warn you!"

    This jetpack prop is for fun "pretend-flying" only. Thank you.

Explanations:
=============

    Let me start with a bit of:
    "How does a jet engine work?": (This section will give you an insight into why this jetpack was designed as it is.)
    ============================== (Feel free to skip to "How does this jetpack prop work" if you are not interested!)

    On the Boeing 747 (Yes, I am a captain on the B-747), a minimum of five minutes is necessary between the last
    engine being started and takeoff power being applied. This allows for thermal stabilization in the
    engines, avoids premature and very costly wear and tear and reduces the chances of one of them going
    "boom" down the line.

    Additionally, between landing power and after the engine thrust reversers have been returned to their stowed
    position and the engines have stabilized back to their idle power, another three minutes must pass before it is
    safe and allowed to shut them down. Just in case you have ever wondered, "We have arrived, why has(have) the
    pilot(s) not shut down the engines already?" after a short taxi, that is why!

    I tried to somewhat mimic the functioning of the jetpack prop based on our real-world jetliner jet engines.
    However, I have only included three modes of operation:
    - Off (yes, off is a mode)
    - Idle/low power
    - Flight/high/full power

    Instead of the seven-ish needed (listed in order of operation):
    - Idle (20 to 25%) - between "after engine(s) has(have) been started" and takeoff (it is called ground idle).
    - Takeoff (100%) - can only be applied for five to ten minutes.
    - Climb/cruise (80 to 90%)
    - Descent (+/-40%) - it is called flight idle.
    - Landing (+/-60%)
    - Idle reverse - optional and not needed on a jetpack, but most passenger jetliners use this.
    - Full reverse - also optional and definitely not needed on a jetpack, but big and heavy jets need to use this.
    - Plus everything in between 80 and 40% - for different rates of descent.
    - Plus everything in between 60 and 90% - for different rates of climb or maintaining different speeds at
      or below cruise altitude.

    "Why did I choose 'only' 3 modes?"
    ==================================
    Because I wanted to have at least 2 "power" levels, and in Star Wars, jetpacks seem to only have off or full power.
    One exception to that: in one episode of The Mandalorian season 3 when Mando uses his jetpack to gently descends
    with a "slow-ish burn" - I don't want to add spoilers and I don't know of any other example. If you've seen the
    episode, you know what I am talking about!
    And by having 3 modes of operation, I could introduce more "fun" (if you are "pretend flying only") failures for
    each phases of "flight/non flight".

    The ultimate goal would be to code for a rotary encoder (I suppose this could work like a thrust lever?) or a
    touch-button that could modulate "power" (aka sound) like a thrust lever modulates fuel flow. I don't even know if
    ProffieBoard/ProffieOS can modulate sounds like a thrust lever modulates power on a jet engine? And besides,
    I don't know (yet) how to code for that!

    But for now my only Proffie prop has a two buttons setup and I can't see any sensible way to have more than 3 modes.
    I have considered "Alt mode" for more modes but I do not believe that Alt mode is the tool for this job.
    There would be too may button combinations to attempt to go from one "random" mode to another (that is not next up
    or next down). Going from off to flight mode would need 6 (or more!) clicks and I doubt it would make for an enjoyable
    user experience to constantly having to count and keep track of your clicks. It would be barely achievable on a two
    buttons setup (POW for up and AUX for down) but forget the "fun" malfunctions or a one button prop.

    "How does this jetpack prop work?"
    ==================================

    First Power Button Press:
        Plays the jetpack idle starting sound. (low volume & low light intensity on the blade(s),
        like a jet engine initial start)
        Loops the idle sound until 1 minute passes, the jetpack is started, or
        the aux button is pressed (plays a false start sound and restarts the 1-minute idle loop).
        If no "Second Power Button Press" within 1 minute, the jetpack will switch off.

    Second Power Button Press:
        Plays the jetpack "going to in flight" starting sound. (high volume & high light intensity on the blade(s),
        like a jet engine applying take-off power)
        Loops the jetpack flying sound until the power button is pressed again.
        To do (if there is a demand): add a limit to this. You can't fly for ever without "refueling"!

    Third Power Button Press:
        Plays the jetpack slowing down to idle sound. (low volume & low light intensity on the blade(s) again)
        Loops the idle sound until 1 minute passes, the jetpack is restarted, or the Aux button is pressed
        (restarts the 1-minute idle loop).

    Aux Button Presses: (for a 2 or 3 buttons setup) (effects also available for 1 button setup - see buttons table below)
        When flying: Plays a stuttering sound.
        When idle: Plays a false start sound and restarts the 1-minute idle loop.
        When off: Plays a self-destruct, melt-down or dud sound randomly! (surprise!) The idea behind those 3
            effects: you might want to use the jetpack as a detonator and rig it to explode but it may or
            may not go as planned (it might explode, melt or it's a dud)!

    The jetpack turns off automatically if idle for more than 1 minute. This can be changed with
    "#define JETPACK_IDLE_TIME 1000 * 60 * 1" (millisec * sec * minutes) in your config, you can
    change the time to what you want (but don't make it zero or you will not be able to make the
    jetpack go into "flight mode". I have it by default to 1 min for testing/debugging purposes.
    I guess a good number, once I know everything is working as it should be, would be around 15
    to 20 sec. Enough for various sound effects to complete, enough to give you time to restart or
    enough to play with "false start" function.

    Loop Function:
        Monitors the 1-minute timer (or user defined) during idle mode to turn off the jetpack
        completely if the time expires.
        Plays idle or flight mode on loop for the required time.

    To do: create animations for OLED for Missile Launch (based on Mando S02E06 - if you know, you know!)
    & make a fun jetpack sound font.
    Create DIM "mode" for "blades" in idle mode.

Thank you for reading!
======================
I hope you will have fun with my vision of a jetpack.

============= List of optional jetpack defines that you can add to your config. =============
#define JETPACK_IDLE_TIME 1000 * 60 * 1 // Jetpack max idle mode time in millisec (default 1 min) before auto-shutdown.
#define JETPACK_TRACK_PLAYER            // Adds jetpack prop track player functionality - almost identical to
                                        // Fett263's saber track player, but without the menu option.
// TBA JETPACK_ALLOW_CRASHING           // If "shot at" - I could create it if there is interest!
// TBA MAX_JETPACK_FLIGHT_TIME          // You can't "fly" forever. Eventually, you will run out of fuel
                                        // (or battery, in the case of your prop).

Please feel free to submit more fun ideas on The Crucible or to this GitHub page:
https://github.com/olivierflying747-8/Olis-ProffieOS/tree/Jetpack-suggestions

============= CHECK THE LIST BELOW FOR ACTUAL BUTTON COMMANDS! ============= NOT YET FINAL !!! =============
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
 Enter/Exit Volume Menu: Double-click and hold while OFF.
              Volume Up: Short-click while in Volume Menu.
            Volume Down: Long-click while in Volume Menu.
          Battery Level: Triple-click while OFF.
 Abort missile sequence: not available on a 1 button setup (But why do you want to stop it anyways?)

=================================================================================================================
2 Buttons:
POWER Button:

               Activate: Short-click while OFF, in idlemode or flightmode.  | tested (works)
Missile Launch Sequence: Double-click while ON or OFF.                      | tested (works)
        Jetpack Mishaps: Hold medium-duration click while ON or OFF.        | tested (works)
         Activate Muted: Double-click and hold while OFF.                   | tested (works but doesn't say "mute" from common) & How do I un-mute ??
        Play/Stop Music: Hold long and release while OFF.                   | tested (works)
       Turn Off Jetpack: Hold and wait until the blade turns off while ON.  | tested (works in flight but sound keeps playing until done)
                                                                            |        (doesn't work in idle)
              Volume Up: Short-click while in Volume Menu.                  | untested (can't enter volume menu)
        Previous Preset: Hold and wait while OFF.                           | tested (doesn't work - does play track)

AUX Button:

            Next Preset: Short-click while OFF.                             | tested (works)
      Enter Volume Menu: Long-click while OFF.                              | tested (doesn't work - does battery level)
            Volume Down: Short-click while in Volume Menu.                  | untested (can't enter volume menu)
          Battery Level: Hold and release while OFF.                        |
 Abort missile sequence: Hold and release while OFF.                        | tested (works but you have to be quick because if boom has already started, it will finish!)

=================================================================================================================
3 Buttons: Same as 2 buttons except for the following:
AUX2 Button:

        Previous Preset: Short-click while OFF.                            | tested (works)
 Abort missile sequence: Hold and release while OFF.                       | tested (works but you have to be quick because if boom has already started, it will finish!)

============= CHECK THE LIST ABOVE FOR ACTUAL BUTTON COMMANDS! ============= NOT YET FINAL !!! =============
*/

#ifndef PROPS_JETPACK_OLI_BUTTONS_H
#define PROPS_JETPACK_OLI_BUTTONS_H

#if NUM_BUTTONS < 1
#error Your prop NEEDS 1 button to use jetpack_Oli_buttons, but 2 are recommended for simplicity of handling!
#endif

#include "prop_base.h"

#define PROP_TYPE JetpackOliButtons

#ifndef JETPACK_IDLE_TIME
// Jetpack max idle mode time in millisec (for now, default 1 min) before auto-shutdown
#define JETPACK_IDLE_TIME 1000 * 60 * 1
#endif

/* == sounds for system ==
The long below list will not use additional memory. It is necessary in case you want to use jetpack_prop.h with
dual_prop.h or my multi_prop.h to avoid double define errors of identical effects from other props.

Props in alphabetical order, Sabers First, Blaster(s) Second, everything else, like the thermal detonator, Third!
I placed the jetpack prop low on the multi_prop.h "food chain" (in Fourth)!
*/

// =====================================================================================================================

#ifdef PROPS_SABER_BC_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#define HAVE_MUTE       // Notification before muted ignition to avoid confusion.
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
#define HAVE_VOLUP      // for increase volume
#define HAVE_VOLMAX     // for maximum volume reached
#define HAVE_VOLMIN     // for minimum volume reached
#endif

#ifdef PROPS_SABER_SABERSENSE_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#define HAVE_VMBEGIN    // for Begin Volume Menu
#define HAVE_VMEND      // for End Volume Menu
#define HAVE_VOLDOWN    // for decrease volume
#define HAVE_VOLUP      // for increase volume
#define HAVE_VOLMAX     // for maximum volume reached
#define HAVE_VOLMIN     // for minimum volume reached
#endif

#ifdef PROPS_SABER_SHTOK_BUTTONS_H
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
#define HAVE_DIM        // for EFFECT_POWERSAVE
#endif

#ifdef PROPS_BLASTER_BC_BUTTONS_H
#ifndef HAVE_BATTERY
#define HAVE_BATTERY    // for EFFECT_BATTERY_LEVEL
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

// =====================================================================================================================

// == sounds for jetpack ==
EFFECT2(idlemode,idlemode);          // jetpack idle sound        (looping idle sound)
EFFECT2(startidlemode,idlemode);     // jetpack starting to idle  (0 to idle)
EFFECT2(flightmode,flightmode);      // jetpack flying sound      (looping flight sound)
EFFECT2(startflightmode,flightmode); // jetpack starting sound    (idle to flight)
EFFECT2(stopflightmode,idlemode);    // jetpack shutdown sound    (flight to idle)
EFFECT(stopidlemode);                // jetpack stopping sound    (idle to 0)
// == sounds for mishaps ==
EFFECT2(falsestart,idlemode);        // jetpack false start sound (pre-flight)
EFFECT2(stuttering,flightmode);      // jetpack stuttering sound  (in flight)
EFFECT(selfdestruct);                // jetpack exploding sound   (pre-idle)
EFFECT(meltdown);                    // jetpack melt-down sound   (pre-idle)
EFFECT(dud);                         // jetpack not exploding or melting down sound "Dank farrik, it's a dud!"
// == sounds for missile ==
EFFECT(arming);                      // viewfinder coming down "click" sound
EFFECT(targeting);                   // viewfinder search/finds target sound
EFFECT(missilelaunch);               // missile launch sound
EFFECT(missilegoesboom);             // double explosion in the distance sound
EFFECT(mandotalk);                   // "Nice shot! I was aiming for the other one!
EFFECT(disarming);                   // viewfinder going back up sound "shlack" (reverse "click")

class JetpackOliButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  JetpackOliButtons() : PropBase() {}
  const char* name() override { return "JetpackOliButtons"; }

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

/*
// Only one chdir & Parse function can be used at a time, so in a dual_prop.h/multi_prop.h "environment"
// SaberFett263Button already has them.
// I initially had this in multi_prop.h:
#if defined(PROPS_SABER_FETT263_BUTTONS_H) || defined(PROPS_JETPACK_PROP_H)
  bool chdir(StringPiece dir) override {
    return Saber::chdir(dir);
  }
  bool Parse(const char* key, const char* value) override {
    return Saber::Parse(key, value);
  }
#endif
// This was resolving the ambiguity but was also increasing the size of the compile by around 100 bytes,
// because the code for chdir and Parse was added twice (once in JetpackOli and once in Fett263) and
// extra code had to be added to multi_prop.h to resolve the ambiguity. Solving the ambiguity directly
// in JetpackOli reduces the size of the compile by a little bit.
// Every byte saved counts, right ?
*/

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
  // Proffie board in it can't realistically have twist or stab motion events.
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

      // Abort missile sequence, if you are quick enough and the long click has been released before
      // missilegoesboom has started! ******** only for (2 & 3 buttons while off): ********
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        missilelaunch_ = false;
        state_machine_.reset_state_machine();
        PVLOG_NORMAL << "Missile Sequence Aborted.\n";
        SaberBase::DoEffect(EFFECT_DISARMING,0);
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
          if (!hybrid_font.PlayPolyphonic(&SFX_mute)) {
            beeper.Beep(0.05, 2000);
            beeper.Silence(0.05);
            beeper.Beep(0.05, 2000);
          }
          FastOn();
        }
        return true;

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
    talkie.SayNumber((int)roundf(battery_monitor.battery_percent() * 10) % 100);
    talkie.Say(spPERCENT);
  // I personally prefer %, that is why it is "active" here.
  // If you want Volts instead of percents, comment the two lines above and un-comment the 5 lines below.
  //talkie.SayDigit((int)floorf(battery_monitor.battery()));
  //talkie.Say(spPOINT);
  //talkie.SayDigit(((int)floorf(battery_monitor.battery() * 10)) % 10);
  //talkie.SayDigit(((int)floorf(battery_monitor.battery() * 100)) % 10);
  //talkie.Say(spVOLTS);
        return true;
    }  // switch (EVENTID)
    return false;  // No action
  }  // Event2

  void PerformMissileLaunchSequence() {
    STATE_MACHINE_BEGIN();
    while (true) {
      if (missilelaunch_) {
        PVLOG_NORMAL << "Starting Missile Launch Sequence\n";
        // create animation for OLED of viewfinder coming down
        SaberBase::DoEffect(EFFECT_ARMING,0);
        SLEEP(SaberBase::sound_length * 1000);
        // create animation for OLED of targeting
        SaberBase::DoEffect(EFFECT_TARGETING,0);
        SLEEP(SaberBase::sound_length * 1000);
        // create animation for OLED of jetpack launching missile
        SaberBase::DoEffect(EFFECT_MISSILELAUNCH,0);
        SLEEP(SaberBase::sound_length * 1000);
        // create animation for OLED of explosion
        SaberBase::DoEffect(EFFECT_MISSILEGOESBOOM,0);
        SLEEP(SaberBase::sound_length * 1000);
        if (!flight_) { // Perform "Mando Talk" if not in flight mode!
          // create animation for OLED of Mando & Boba talking
          SaberBase::DoEffect(EFFECT_MANDOTALK,0);
          SLEEP(SaberBase::sound_length * 1000 + 10);
          PVLOG_NORMAL << "Mando: Nice shot!\nBoba: I was aiming for the other one!\n";
        }
        // create animation for OLED of viewfinder going back up
        SaberBase::DoEffect(EFFECT_DISARMING,0);
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
      return;
    }
    if (flight_) {
      // Jetpack flying, play stuttering sound.
      jetpack_wav_player_.StopFromReader();      // Stop flight sound.
      SaberBase::DoEffect(EFFECT_STUTTERING, 0); // Play stuttering sound.
    } else if (idle_) {
      // Jetpack idle, play false start and reset idle timer.
      jetpack_wav_player_.StopFromReader();      // Stop idle sound.
      SaberBase::DoEffect(EFFECT_FALSESTART, 0); // Play false start sound.
      idle_timer_ = millis(); // Reset idle timer
    } else ChooseRandomEffect(); // Play random mishap effect.
  }

  // Jetpack off, play self destruct, meltdown or dud (randomly between the three).
  void ChooseRandomEffect() {
    int choice = random(3); // Generate a random number: 0, 1, or 2.
    switch (choice) {
      case 0:  SaberBase::DoEffect(EFFECT_SELFDESTRUCT,0); break;
      case 1:  SaberBase::DoEffect(EFFECT_MELTDOWN,0);     break;
      case 2:  SaberBase::DoEffect(EFFECT_DUD,0);          break;
      default: SaberBase::DoEffect(EFFECT_DUD,0); break; // <-- Is a "default" even needed ???
    }
  }

  // Initial start, transition to Idle Mode (from 0 to idle)
  void StartIdleMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return;
    }
    SaberBase::DoEffect(EFFECT_STARTIDLEMODE, 0); // start transition to idle mode.
    flight_ = false;
    idle_   = true;
    idle_timer_  = millis(); // Reset idle timer
    FastOn();
    PVLOG_STATUS << "Jetpack Idling\n";
  }

  // Transition to Flying Mode (from idle to flying)
  void StartFlightMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return;
    }
    SaberBase::DoEffect(EFFECT_STARTFLIGHTMODE, 0); // start transition to flight mode.
    flight_ = true;
    idle_   = false;
    PVLOG_STATUS << "Jetpack Flying\n";
  }

  // Transition from Flying Mode (from flying to idle)
  void StopFlightMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return;
    }
    SaberBase::DoEffect(EFFECT_STOPFLIGHTMODE, 0); // start transition from flight mode.
    flight_ = false;
    idle_   = true;
  }

  // Stop Idle Mode (Jetpack completely off - from flight or idle to off)
  void StopIdleMode() {
    if (jetpack_wav_player_.isPlaying()) {
      return;
    }
    SaberBase::DoEffect(EFFECT_STOPIDLEMODE, 0); // start transition from idle mode.
    flight_ = false; // Should already be false but StopIdleMode can also be used to turn the
                     // jetpack completely off from any state (with a 2 or 3 buttons setup).
    idle_   = false;
    Off(OFF_FAST); // Fully stop the jetpack.
  }

  void Loop() override {
    unsigned long now = millis();

    // Perform the missile launch sequence.
    PerformMissileLaunchSequence();

    // Stop idle mode after timeout
    if (idle_ && (now - idle_timer_ > JETPACK_IDLE_TIME)) {
      StopIdleMode(); // Stop jetpack if idle for more than defined time.
      return;
    }
  }

  void DoMotion(const Vec3&, bool) override { }

  // Helper (to avoid repetition)
  void EffectHelper (Effect* effect1,const char* msg1) {
    hybrid_font.PlayPolyphonic(effect1);
    PVLOG_STATUS << msg1 << "\n";
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;
      // Missile effects:
      case EFFECT_ARMING:          EffectHelper(&SFX_arming,                   "Arming"); return; // Must not be
      case EFFECT_TARGETING:       EffectHelper(&SFX_targeting,             "Targeting"); return; // interrupted
      case EFFECT_MISSILELAUNCH:   EffectHelper(&SFX_missilelaunch,    "Missile Launch"); return; // and must
      case EFFECT_MISSILEGOESBOOM: EffectHelper(&SFX_missilegoesboom,"Missile Explodes"); return; // play in
      case EFFECT_MANDOTALK:       EffectHelper(&SFX_mandotalk,  "Mando & Boba Talking"); return; // sequence with
      case EFFECT_DISARMING:       EffectHelper(&SFX_disarming,      "Disarm Targeting"); return; // one another!
      // Engine mishap effects:
      case EFFECT_FALSESTART:      EffectHelper(&SFX_falsestart,          "False Start"); return;
      case EFFECT_STUTTERING:      EffectHelper(&SFX_stuttering,           "Stuttering"); return;
      case EFFECT_SELFDESTRUCT:    EffectHelper(&SFX_selfdestruct,      "Self Destruct"); return;
      case EFFECT_MELTDOWN:        EffectHelper(&SFX_meltdown,               "Meltdown"); return;
      case EFFECT_DUD:             EffectHelper(&SFX_dud,   "Dank Farrik!\nIt's a Dud!"); return;
      // Engine normal effects:
      case EFFECT_STARTIDLEMODE:   EffectHelper(&SFX_startidlemode,     "Jetpack Starting to Idle."); return; //after startidlemode, play idle mode on loop
    //case EFFECT_IDLEMODE:        EffectHelper(&SFX_idlemode,         "Jetpack in Idle Loop Mode."); return; //plays on loop
      case EFFECT_STARTFLIGHTMODE: EffectHelper(&SFX_startflightmode, "Jetpack Starting to Flight."); return; //after startflightmode, play flight mode on loop
    //case EFFECT_FLIGHTMODE:      EffectHelper(&SFX_flightmode,     "Jetpack in Flight Loop Mode."); return; //plays on loop
      case EFFECT_STOPFLIGHTMODE:  EffectHelper(&SFX_stopflightmode,"Jetpack Slowing Down to Idle."); return; //after stopflightmode, play idle on loop
      case EFFECT_STOPIDLEMODE:    EffectHelper(&SFX_stopidlemode,        "Jetpack Completely Off."); return;
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
  unsigned long idle_timer_ = 0;        // Timer for idle mode timeout
  bool idle_            = false;        // Whether the jetpack is in  idle  mode
  bool flight_          = false;        // Whether the jetpack is in flight mode
  bool missilelaunch_   = false;        // Whether the missile is launching
  StateMachineState state_machine_;     // Required for the SLEEP macro
  // Sound effects handling
  PlayWav jetpack_wav_player_;          // Handles idle, flight, and mishap sounds
#ifdef JETPACK_TRACK_PLAYER
  // Track player handling
  bool istrack_playing_ = false;        // Tracks if track is currently playing (for Event2 switch)
  int  track_num_       = 0;            // Current track number
  int  num_tracks_      = 0;            // Total number of tracks available
  TrackMode track_mode_ = PLAYBACK_OFF; // Current playback mode
#endif
  // Volume handling
  bool mode_volume_     = false;        // from sa22c for volume menu
  bool max_vol_reached_ = false;        // from sa22c for volume menu
  bool min_vol_reached_ = false;        // from sa22c for volume menu
}; // class Jetpack

#endif // PROPS_JETPACK_PROP_H

/**********************************************\
|*                                            *|
|*   JETPACK OLI BUTTONS DISPLAY CONTROLLER   *|
|*                                            *|
\**********************************************/

#ifdef PROP_BOTTOM

#define ONCE_PER_JETPACK_EFFECT(X)  \
/* == jetpack == */                 \
  X(startidlemode)                  \
  X(idlemode)                       \
  X(startflightmode)                \
  X(flightmode)                     \
  X(stopflightmode)                 \
  X(stopidlemode)                   \
/* ==  mishaps == */                \
  X(falsestart)                     \
  X(stuttering)                     \
  X(selfdestruct)                   \
  X(meltdown)                       \
  X(dud)                            \
/* == missile == */                 \
  X(arming)                         \
  X(targeting)                      \
  X(missilelaunch)                  \
  X(missilegoesboom)                \
  X(mandotalk)                      \
  X(disarming)

#ifdef INCLUDE_SSD1306

struct JetpackDisplayConfigFile : public ConfigFile {
  JetpackDisplayConfigFile() { link(&font_config); }
  void iterateVariables(VariableOP *op) override {
    // == jetpack ==
    CONFIG_VARIABLE2(ProffieOSStartIdleModeImageDuration,   1000.0f);
    CONFIG_VARIABLE2(ProffieOSIdleModeImageDuration,        1000.0f);
    CONFIG_VARIABLE2(ProffieOSStartFlightModeImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSFlightModeImageDuration,      1000.0f);
    CONFIG_VARIABLE2(ProffieOSStopFlightModeImageDuration,  1000.0f);
    CONFIG_VARIABLE2(ProffieOSStopIdleModeImageDuration,    1000.0f);
    // ==  mishaps ==
    CONFIG_VARIABLE2(ProffieOSFalseStartImageDuration,      1000.0f);
    CONFIG_VARIABLE2(ProffieOSStutteringImageDuration,      1000.0f);
    CONFIG_VARIABLE2(ProffieOSSelfDestructImageDuration,    1000.0f);
    CONFIG_VARIABLE2(ProffieOSMeltdownImageDuration,        1000.0f);
    CONFIG_VARIABLE2(ProffieOSDudImageDuration,             1000.0f);
    // == missile ==
    CONFIG_VARIABLE2(ProffieOSArmingImageDuration,          1000.0f);
    CONFIG_VARIABLE2(ProffieOSTargetingImageDuration,       1000.0f);
    CONFIG_VARIABLE2(ProffieOSMissileLaunchImageDuration,   1000.0f);
    CONFIG_VARIABLE2(ProffieOSMissileGoesBoomImageDuration, 1000.0f);
    CONFIG_VARIABLE2(ProffieOSMandoTalkImageDuration,       1000.0f);
    CONFIG_VARIABLE2(ProffieOSDisarmingImageDuration,       1000.0f);
  }
  // == jetpack ==
  // for OLED displays, the time a idlemode.bmp        will play.
  float ProffieOSIdleModeImageDuration;
  // for OLED displays, the time a startidlemode.bmp   will play.
  float ProffieOSStartIdleModeImageDuration;
  // for OLED displays, the time a startflightmode.bmp will play.
  float ProffieOSStartFlightModeImageDuration;
  // for OLED displays, the time a flightmode.bmp      will play.
  float ProffieOSFlightModeImageDuration;
  // for OLED displays, the time a stopflightmode.bmp  will play.
  float ProffieOSStopFlightModeImageDuration;
  // for OLED displays, the time a stopidlemode.bmp    will play.
  float ProffieOSStopIdleModeImageDuration;
  // ==  mishaps ==
  // for OLED displays, the time a falsestart.bmp      will play.
  float ProffieOSFalseStartImageDuration;
  // for OLED displays, the time a stuttering.bmp      will play.
  float ProffieOSStutteringImageDuration;
  // for OLED displays, the time a selfdestruct.bmp    will play.
  float ProffieOSSelfDestructImageDuration;
  // for OLED displays, the time a meltdown.bmp        will play.
  float ProffieOSMeltdownImageDuration;
  // for OLED displays, the time a dud.bmp             will play.
  float ProffieOSDudImageDuration;
  // == missile ==
  // for OLED displays, the time a arming.bmp          will play.
  float ProffieOSArmingImageDuration;
  // for OLED displays, the time a targeting.bmp       will play.
  float ProffieOSTargetingImageDuration;
  // for OLED displays, the time a missilelaunch.bmp   will play.
  float ProffieOSMissileLaunchImageDuration;
  // for OLED displays, the time a missilegoesboom.bmp will play.
  float ProffieOSMissileGoesBoomImageDuration;
  // for OLED displays, the time a mandotalk.bmp       will play.
  float ProffieOSMandoTalkImageDuration;
  // for OLED displays, the time a disarming.bmp          will play.
  float ProffieOSDisarmingImageDuration;
};

template<typename PREFIX = ByteArray<>>
struct JetpackDisplayEffects  {
  JetpackDisplayEffects() : dummy_(0) ONCE_PER_JETPACK_EFFECT(INIT_IMG) {}
  int dummy_;
  ONCE_PER_JETPACK_EFFECT(DEF_IMG)
};

template<int Width, class col_t, typename PREFIX = ByteArray<>>
class JetpackDisplayController : public StandardDisplayController<Width, col_t, PREFIX> {
public:
  JetpackDisplayEffects<PREFIX> img_;
  JetpackDisplayConfigFile &jetpack_font_config;
  JetpackDisplayController() :
    img_(*getPtr<JetpackDisplayEffects<PREFIX>>()),
    jetpack_font_config(*getPtr<JetpackDisplayConfigFile>()) {}

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      // jetpack normal operations display effects:
      // ==========================================
      case EFFECT_STARTIDLEMODE:
        if (img_.startidlemode) {                            // animation of flame going from zero to small
          ShowFileWithSoundLength(&img_.startidlemode,       jetpack_font_config.ProffieOSStartIdleModeImageDuration);
        } else {
          this->SetMessage("jetpack\nigniting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_IDLEMODE:
        if (img_.idlemode) {                                 // animation of small oscillating flame
          ShowFileWithSoundLength(&img_.idlemode,            jetpack_font_config.ProffieOSIdleModeImageDuration);
        } else {
          this->SetMessage("jetpack\nidling");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STARTFLIGHTMODE:
        if (img_.startflightmode) {                          // animation of flame going from small to large
          ShowFileWithSoundLength(&img_.startflightmode,     jetpack_font_config.ProffieOSStartFlightModeImageDuration);
        } else {
          this->SetMessage("jetpack\nstarting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_FLIGHTMODE:
        if (img_.flightmode) {                               // animation of large oscillating flame
          ShowFileWithSoundLength(&img_.flightmode,          jetpack_font_config.ProffieOSFlightModeImageDuration);
        } else {
          this->SetMessage("jetpack\nflying");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STOPFLIGHTMODE:
        if (img_.stopflightmode) {                           // animation of flame going from large to small
          ShowFileWithSoundLength(&img_.stopflightmode,      jetpack_font_config.ProffieOSStopFlightModeImageDuration);
        } else {
          this->SetMessage("jetpack\nshutdown");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STOPIDLEMODE:
        if (img_.stopidlemode) {                             // animation of flame going from small to zero
          ShowFileWithSoundLength(&img_.stopidlemode,        jetpack_font_config.ProffieOSStopIdleModeImageDuration);
        } else {
          this->SetMessage("  jetpack\ncompletely off");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      // jetpack mishaps display effects:
      // =================================
      case EFFECT_FALSESTART:
        if (img_.IMG_falsestart) {                           // Jetpack with "smoke fart" animation
          ShowFileWithSoundLength(&img_.IMG_falsestart,      jetpack_font_config.ProffieOSFalseStartImageDuration);
        } else {
          this->SetMessage("jetpack\nfalse start");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_STUTTERING:
        if (img_.IMG_stuttering) {                           // Jetpack with trail of "smoke farts" animation
          ShowFileWithSoundLength(&img_.IMG_stuttering,      jetpack_font_config.ProffieOSStutteringImageDuration);
        } else {
          this->SetMessage("jetpack\nstuttering");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_SELFDESTRUCT:
        if (img_.selfdestruct) {                             // Jetpack exploding animation
          ShowFileWithSoundLength(&img_.selfdestruct,        jetpack_font_config.ProffieOSSelfDestructImageDuration);
        } else {
          this->SetMessage("jetpack\nself destruct");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MELTDOWN:
        if (img_.meltdown) {                                 // Jetpack melting animation
          ShowFileWithSoundLength(&img_.meltdown,            jetpack_font_config.ProffieOSMeltdownImageDuration);
        } else {
          this->SetMessage("jetpack\nmelting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_DUD:
        if (img_.dud) {                                      // annimation TBD
          ShowFileWithSoundLength(&img_.dud,                 jetpack_font_config.ProffieOSDudImageDuration);
        } else {
          this->SetMessage("it's a\ndud");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      // jetpack missile display effects: Make animations for OLED from the episode from Mando S02E06
      // ================================
      case EFFECT_ARMING:
        if (img_.IMG_arming) {                               // animation of viewfinder coming down
          ShowFileWithSoundLength(&img_.IMG_arming,          jetpack_font_config.ProffieOSAimingImageDuration);
        } else {
          this->SetMessage("arming");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_TARGETING:
        if (img_.IMG_targeting) {                            // animation of targeting
          ShowFileWithSoundLength(&img_.IMG_targeting,       jetpack_font_config.ProffieOSTargetingImageDuration);
        } else {
          this->SetMessage("targeting");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MISSILELAUNCH:
        if (img_.IMG_missilelaunch) {                        // animation of jetpack launching missile
          ShowFileWithSoundLength(&img_.IMG_missilelaunch,   jetpack_font_config.ProffieOSMissileLaunchImageDuration);
        } else {
          this->SetMessage("launching\nmissile");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MISSILEGOESBOOM:
        if (img_.IMG_missilegoesboom) {                      // animation of explosion
          ShowFileWithSoundLength(&img_.IMG_missilegoesboom, jetpack_font_config.ProffieOSMissileGoesBoomImageDuration);
        } else {
          this->SetMessage("missile\nexplodes");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MANDOTALK:
        if (img_.IMG_mandotalk) {                            // animation of Mando & Boba talking
          ShowFileWithSoundLength(&img_.IMG_mandotalk,       jetpack_font_config.ProffieOSMandoTalkImageDuration);
        } else {
          this->SetMessage("nice shot");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_DISARMING:
        if (img_.IMG_disarming) {                            // animation of viewfinder going back up
          ShowFileWithSoundLength(&img_.IMG_disarming,       jetpack_font_config.ProffieOSDisarmingImageDuration);
        } else {
          this->SetMessage("disarming");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      default:
        StandardDisplayController<Width, col_t, PREFIX>::SB_Effect2(effect, location);
    }
  }

  void SB_Off2(typename StandardDisplayController<Width, col_t, PREFIX>::OffType offtype, EffectLocation location) override {
    if (offtype == StandardDisplayController<Width, col_t, PREFIX>::OFF_BLAST) {
      ShowFileWithSoundLength(img_.IMG_disarming, jetpack_font_config.ProffieOSDisarmingImageDuration);
    } else {
      StandardDisplayController<Width, col_t, PREFIX>::SB_Off2(offtype, location);
    }
  }
};

#endif  // INCLUDE_SSD1306

template<int W, int H, typename PREFIX = ConcatByteArrays<typename NumberToByteArray<W>::type, ByteArray<'x'>,
         typename NumberToByteArray<H>::type>>
class JetpackColorDisplayController : public StandarColorDisplayController<W, H, PREFIX> {
public:
  template<int w, int h>
  explicit JetpackColorDisplayController(SizedLayeredScreenControl<w, h>* screen) :
  StandarColorDisplayController<W, H, PREFIX>(screen) ONCE_PER_JETPACK_EFFECT(INIT_SCR) {
  }
  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      // jetpack normal operations color display effects:
      // ================================================
      case EFFECT_STARTIDLEMODE:   this->scr_.Play(&SCR_startidlemode);   break;
      case EFFECT_IDLEMODE:        this->scr_.Play(&SCR_idlemode);        break;
      case EFFECT_STARTFLIGHTMODE: this->scr_.Play(&SCR_startflightmode); break;
      case EFFECT_FLIGHTMODE:      this->scr_.Play(&SCR_flightmode);      break;
      case EFFECT_STOPFLIGHTMODE:  this->scr_.Play(&SCR_stopflightmode);  break;
      case EFFECT_STOPIDLEMODE:    this->scr_.Play(&SCR_stopidlemode);    break;
      // jetpack mishaps display effects:
      // =================================
      case EFFECT_FALSESTART:      this->scr_.Play(&SCR_falsestart);      break;
      case EFFECT_SELFDESTRUCT:    this->scr_.Play(&SCR_selfdestruct);    break;
      case EFFECT_STUTTERING:      this->scr_.Play(&SCR_stuttering);      break;
      case EFFECT_MELTDOWN:        this->scr_.Play(&SCR_meltdown);        break;
      case EFFECT_DUD:             this->scr_.Play(&SCR_dud);             break;
      // jetpack missile display effects: Make animations for OLED from the episode from Mando S02E06
      // ================================
      case EFFECT_ARMING:          this->scr_.Play(&SCR_arming);          break;
      case EFFECT_TARGETING:       this->scr_.Play(&SCR_targeting);       break;
      case EFFECT_MISSILELAUNCH:   this->scr_.Play(&SCR_missilelaunch);   break;
      case EFFECT_MISSILEGOESBOOM: this->scr_.Play(&SCR_missilegoesboom); break;
      case EFFECT_MANDOTALK:       this->scr_.Play(&SCR_mandotalk);       break;
      case EFFECT_DISARMING:       this->scr_.Play(&SCR_disarming);       break;
      default:
        StandarColorDisplayController<W, H, PREFIX>::SB_Effect2(effect, location);
    }
  }

protected:
  ONCE_PER_JETPACK_EFFECT(DEF_SCR);
};

#undef ONCE_PER_JETPACK_EFFECT

#endif  // PROP_BOTTOM
