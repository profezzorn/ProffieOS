// JetpackOliButtons Revision 63

/* Created by OlivierFlying747-8,
  https://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2025 Fredrik Hubinette
  Copyright (c) 2025 OlivierFlying747-8 with contributions by:
  Fredrik Hubinette aka profezzorn,
  Bryan Conner aka NoSloppy who helped with his sound knowledge and experimentation on Alt-sound,
  Ryan Ogurek aka ryryog25 who helped me get this project "off the ground" (no pun intended),
  In case of problems, you can find us at: https://crucible.hubbe.net where somebody will be there to help.
  Distributed under the terms of the GNU General Public License v3.
  https://www.gnu.org/licenses/

This jetpack prop was created to be compatible with ProffieOS v8.x

Includes 1 and 2 button controls. However, I would recommend a 2
buttons setup because I think a 1 button setup is cumbersome to use!

You can find my free/royalty free test font here (It's not great, but it's a start!):
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
  Experimentation on Alt-sounds by Bryan Connor aka NoSloppy can be found here:
  https://crucible.hubbe.net/t/jetpack-prop-update-and-questions/6648/29

  But for now my only Proffie prop has a two buttons setup and I can't see any sensible way to have more than 3 modes.
  I have considered "Alt mode" for more modes but I do not believe that Alt mode is the tool for this job.
  There would be too many button combinations to attempt to go from one "random" mode to another (that is not next up
  or next down). Going from off to flight mode would need 6 (or more!) clicks and I doubt it would make for an enjoyable
  user experience to constantly having to count and keep track of your clicks. It would be barely achievable on a two
  buttons setup (POW for up and AUX for down) but forget the "fun" malfunctions or a one button prop.

  "How does this jetpack prop work?"
  ==================================

  First Power Short Click Button Press:
    Plays the jetpack idle starting sound. (low volume & low light intensity on the blade(s),
    like a jet engine initial start)
    Loops the idle sound until 1 minute passes, the jetpack is started, or
    the aux button is pressed (plays a false start sound and restarts the 1-minute idle loop).
    If no "Second Power Button Press" within 1 minute, the jetpack will switch off automatically.

  Second Power Short Click Button Press:
    Plays the jetpack "going to in flight" starting sound. (high volume & high light intensity on the blade(s),
    like a jet engine applying take-off power)
    Loops the jetpack flying sound until the power button is pressed again.
    To do (if there is a demand): add a limit to this. You can't fly for ever without "refueling"!

  Third Power Short Click Button Press:
    Plays the jetpack slowing down to idle sound. (low volume & low light intensity on the blade(s) again)
    Loops the idle sound until 1 minute passes, the jetpack is restarted, or the Aux button is pressed
    (restarts the 1-minute idle loop).

  Medium Power Button Press:
    When flying: Plays a stuttering sound.
    When idle: Plays a false start sound and restarts the 1-minute idle loop.
    When off: Plays a self-destruct, melt-down or dud sound randomly! (surprise!) The idea behind those 3
      effects: you might want to use the jetpack as a detonator and rig it to explode but it may or
      may not go as planned (it might explode, melt or it's a dud)!

  Double Click Power Button Press:
    Will do the "jetpack missile launch sequence".
    While OFF or in IDLEMODE, if present, mandotalk.wav will play.
    While in FLIGHTMODE, mandotalk.wav will not play.

  The jetpack will turn off automatically if in "idle mode" for more than 1 minute. This can be changed with
  "#define JETPACK_IDLE_TIME 1000 * 60 * 1" (millisec * sec * minutes) in your config.
  You can change the time to what you want, but don't make it less than your startidlemode.wav + a few seconds,
  or you will not be able to make the jetpack go into "flight mode". I have it by default set to 1 min
  for testing/debugging purposes.
  I guess a good number, once I know everything is working as it should be, would be around 30 to
  35 seconds. Enough for various sound effects to complete (if your "engine start sound is short enough),
  enough to give you time to restart (when coming from "flight mode") or enough to "play" with "false start" function.

  Loop Function:
    Monitors the 1-minute timer (or user defined) during idle mode to turn off the jetpack
    completely if the time expires.

============= LIST OF BUTTON & MOTION COMMANDS: =================================================================================
1 Button: !!! NEEDS RE-TESTING !!!

POWER Button:
  - Short click (while OFF or ON):
    - If jetpack is OFF → Start jetpack to IDLE mode (rev up)
    - If in IDLE mode   → Engage FLIGHT mode   (rev up again)
    - If in FLIGHT mode → Return to IDLE mode      (rev down)
    - If in Volume Menu → Adjust Volume:
      - Pointing UP   → Volume Up
      - Pointing DOWN → Volume Down
  - Double click (while ON or OFF): Launch missile
  - Triple click (while ON): Turn OFF jetpack (stops idle/flight, not missile)
  - Hold medium (while ON or OFF): Trigger Jetpack Mishaps
  - Quadruple click (while OFF): Announce Battery Level
  - Double click & Hold Medium (while ON or OFF): Enter/Exit Volume Menu
  - Double click (while OFF), pointing:
    - UP   → Next Preset
    - DOWN → Previous Preset
  - Triple click & Hold Medium (while ON or OFF): Start/Stop Track
  - Triple click (while ON or OFF), pointing:
    - UP   → Next Track
    - DOWN → Previous Track

               Activate: EVENT_FIRST_SAVED_CLICK_SHORT Short-click while OFF, in idlemode or flightmode. | tested (works)
        Jetpack Mishaps: EVENT_FIRST_HELD_MEDIUM Hold medium click while ON or OFF.                      | tested (works)
Missile Launch Sequence: EVENT_SECOND_SAVED_CLICK_SHORT Double-click while ON or OFF.                    | tested (works)
       Turn Off Jetpack: EVENT_THIRD_SAVED_CLICK_SHORT while ON.                                         | untested (works but current sound keeps playing until done!
                         It will turn off automatically after JETPACK_IDLE_TIME expires (default 1 min)|         I am not yet sure if I want to change that or not?)
          Battery Level: EVENT_FOURTH_SAVED_CLICK_SHORT, while OFF.                                      | un-tested
            Next Preset: EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF while pointing towards up.              | un-tested
        Previous Preset: EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF while pointing towards down.            | un-tested
              Volume Up: EVENT_FIRST_SAVED_CLICK_SHORT while in Volume Menu pointing towards up.         | un-tested
            Volume Down: EVENT_FIRST_SAVED_CLICK_SHORT while in Volume Menu pointing towards down.       | un-tested
 Enter/Exit Volume Menu: EVENT_SECOND_HELD_MEDIUM while ON or OFF.                                       | un-tested
     Play/Stop Track(s): EVENT_THIRD_HELD_MEDIUM, while ON or OFF.                                       | un-tested
             Next track: EVENT_THIRD_SAVED_CLICK_SHORT, while ON or OFF pointing towards up.             | un-tested
         Previous track: EVENT_THIRD_SAVED_CLICK_SHORT, while ON or OFF pointing towards down.           | un-tested

=================================================================================================================================
2 Buttons: !!! NEEDS RE-WRITING & RE-TESTING !!!

POWER Button:
  - Short click (while OFF or ON):
    - If jetpack is OFF → Start jetpack to IDLE mode (rev up)
    - If in IDLE mode   → Engage FLIGHT mode (rev up again)
    - If in FLIGHT mode → Return to IDLE mode (rev down)
  - Double click (ON or OFF): Launch missile (sets missilelaunch_ = true)
  - Triple click (ON): Turn OFF jetpack (stops idle/flight, not missile)
  - Hold medium (ON or OFF): Trigger Jetpack Mishaps (custom function)
  - Quadruple click (OFF): Announce Battery Level

               Activate: EVENT_FIRST_SAVED_CLICK_SHORT Short-click while OFF, in idlemode or flightmode. | tested (works)
        Jetpack Mishaps: EVENT_FIRST_HELD_MEDIUM Medium-held-click while ON or OFF.                      | tested (works)
Missile Launch Sequence: EVENT_SECOND_SAVED_CLICK_SHORT Double-click while ON or OFF.                    | tested (works)
       Turn Off Jetpack: EVENT_THIRD_SAVED_CLICK_SHORT while ON.                                         | tested (works but current sound keeps playing until done! I want to change that!)
                         It should turn off automatically after JETPACK_IDLE_TIME expires (default 1 min)| tested (works)
          Battery Level: EVENT_FOURTH_SAVED_CLICK_SHORT while OFF.                                       | tested something is missing

AUX Button:
  - Short click (OFF):
    - Not in Volume Menu →
      - Pointing UP   → Next Preset
      - Pointing DOWN → Previous Preset
  - Short click (ON or OFF):
    - In Volume Menu →
      - Pointing UP   → Volume Up
      - Pointing DOWN → Volume Down
      - Level         → No action
  - Double click (ON or OFF): Enter/Exit Volume Menu
  - Triple click (ON or OFF): Start/Stop Track
  - Medium Hold (ON or OFF):
    - Pointing UP   → Next Track
    - Pointing DOWN → Previous Track

            Next Preset: EVENT_FIRST_SAVED_CLICK_SHORT while OFF.                                        | tested (works)
        Previous Preset: EVENT_FIRST_SAVED_CLICK_SHORT while OFF.                                        | tested (works)
              Volume Up: EVENT_FIRST_SAVED_CLICK_SHORT Short-click while in Volume Menu while ON or OFF. | untested
            Volume Down: EVENT_FIRST_SAVED_CLICK_SHORT Short-click while in Volume Menu while ON or OFF. | untested
 Enter/Exit Volume Menu: EVENT_SECOND_SAVED_CLICK_SHORT while ON or OFF.                                 | tested (works)
        Play/Stop Track: EVENT_THIRD_SAVED_CLICK_SHORT while OFF.                                        | tested (works)
             Next track: EVENT_FIRST_HELD_MEDIUM Medium-held-click while ON or OFF.                      | untested
         Previous track: EVENT_FIRST_HELD_MEDIUM Medium-held-click while ON or OFF.                      | untested

============= LIST OF .wav USED in this jetpack: ================================================================================
This prop version REQUIRES a ProffieOS Voicepack V1 or V2 for some of the sounds to work.
Typically, that is a folder named "common" on the root level of the SD card.

Download your choice of language and variation here:
https://fredrik.hubbe.net/lightsaber/sound/
Also, a large variety of FREE in-universe character Voicepacks available here:
https://crucible.hubbe.net/t/additional-voicepacks/4227
If you'd care to make a donation for Brian Conner's time making these Voicepacks:
https://www.buymeacoffee.com/brianconner

Your sound font should contain the below listed files to use jetpack_Oli_buttons.h to it's full potential:

== sounds for jetpack engine ==
startidlemode.wav    jetpack starting to idle (0 to idle)
idlemode.wav         jetpack idle sound       (looping idle sound)
startflightmode.wav  jetpack starting sound   (idle to flight)
flightmode.wav       jetpack flying sound     (looping flight sound)
stopflightmode.wav   jetpack shutdown sound   (flight to idle)
stopidlemode.wav     jetpack stopping sound   (idle to 0)

== sounds for mishaps ==
falsestart.wav       jetpack false start sound (pre-flight)
stuttering.wav       jetpack stuttering sound  (in flight)
selfdestruct.wav     jetpack exploding sound   (pre-idle)
meltdown.wav         jetpack melt-down sound   (pre-idle)
dud.wav              jetpack not exploding or melting down sound "Dank farrik, it's a dud!"

== sounds for missile ==
arming.wav           viewfinder coming down "click" sound
targeting.wav        viewfinder search/finds target sound
missilelaunch.wav    missile launch sound
missilegoesboom.wav  double explosion in the distance sound
mandotalk.wav        "Nice shot! I was aiming for the other one!
disarming.wav        viewfinder going back up sound "shlack" (reverse "click")

== sounds for other weapons == (only coded partialy, no code is using them yet)
whistlingbirds.wav
mandosrifle.wav

============= TO DO: ============================================================================================================
  Create animations for OLED for Missile Launch (based on Mando S02E06 - if you know, you know!)
  & make a fun jetpack sound font.
  Create DIM "mode" for "blades/presets" in idle mode.

============= List of optional jetpack defines that you can add to your config. =================================================
#define JETPACK_IDLE_TIME 1000 * 60 * 1 // Jetpack max idle mode time in millisec (default 1 min) before auto-shutdown.
#define JETPACK_TRACK_PLAYER            // Adds jetpack prop track player functionality - almost identical to Fett263's saber
                                        // track player. Tracks have to be located in "font_name\tracks\" or in "common\tracks\"

============= Future possible development ideas =================================================================================
Add functionality for a rotary encoder and add intermediate flight modes with adequately smooth up/down sound transitions.
    (That is my n°1 priority for the future of this prop!)
Add support for a smoke generator? (will probably be controlled via blade styles)
Add support for motor function(s) to make something turn on your jetpack? (will probably be controlled via blade styles)

// TBA #define JETPACK_ALLOW_CRASHING   // If "shot at" - I could create it if there is interest!
// TBA #define MAX_JETPACK_FLIGHT_TIME  // You can't "fly" forever. Eventually, you will run out of fuel
                                        // (or battery, in the case of your prop).
// TBA #define JETPACK_USES_MOTION      // For the most basic functions, but your ProffieBoard would have to be located in your
// for a "zero-ish" button setup        // gauntlet for the ability to do swing left/right (mode up/down), spin (malfunction), stab/force push (missile launch)?
                                        // Only the most "unique" functions of this prop would have a movement associated to them.
Add support for a touch screen to supplement a "zero-ish" button setup for the other functions. ProffieOS has no support for that yet! (idea for POS9.x?)

=================================================================================================================================
Please feel free to submit more fun ideas on The Crucible or to this GitHub page:
https://github.com/olivierflying747-8/Olis-ProffieOS/tree/06_props_Oli_buttons_SUGGESTIONS_WELCOME

=================================================================================================================================

Thank you for reading!
======================
I hope you will have fun with my vision of a jetpack.
*/

#ifndef PROPS_JETPACK_OLI_BUTTONS_H
#define PROPS_JETPACK_OLI_BUTTONS_H

#if NUM_BUTTONS < 1
#error jetpack_Oli_buttons requires 1 or 2 Buttons for operation, but 2 are recommended for simplicity of handling!
#endif

#include "prop_base.h"
#include "../sound/sound_library.h"

#define PROP_TYPE JetpackOliButtons

#ifndef JETPACK_IDLE_TIME
// Jetpack max idle mode time in millisec (for now, default is 1 min) before auto-shutdown
#define JETPACK_IDLE_TIME 1000 * 60 * 1
#endif

// ========== Sounds for jetpack ================================================================================================
EFFECT2(idlemode,idlemode);          // jetpack idle sound        (looping idle sound)
EFFECT2(startidlemode,idlemode);     // jetpack starting to idle  (0 to idle)
EFFECT2(flightmode,flightmode);      // jetpack flying sound      (looping flight sound)
EFFECT2(startflightmode,flightmode); // jetpack starting sound    (idle to flight)
EFFECT2(stopflightmode,idlemode);    // jetpack shutdown sound    (flight to idle)
EFFECT(stopidlemode);                // jetpack stopping sound    (idle to 0)
// ========== Sounds for mishaps ================================================================================================
EFFECT2(falsestart,idlemode);        // jetpack false start sound (pre-flight)
EFFECT2(stuttering,flightmode);      // jetpack stuttering sound  (in flight)
EFFECT(selfdestruct);                // jetpack exploding sound   (pre-idle)
EFFECT(meltdown);                    // jetpack melt-down sound   (pre-idle)
EFFECT(dud);                         // jetpack not exploding or melting down sound "Dank farrik, it's a dud!"
// ========== Sounds for missile ================================================================================================
EFFECT(arming);                      // viewfinder coming down "click" sound
EFFECT(targeting);                   // viewfinder search/finds target sound
EFFECT(missilelaunch);               // missile launch sound
EFFECT(missilegoesboom);             // double explosion in the distance sound
EFFECT(mandotalk);                   // "Nice shot! I was aiming for the other one!"
EFFECT(disarming);                   // viewfinder going back up sound "shlack" (reverse "click")
// ========== Other weapons =====================================================================================================
EFFECT(whistlingbirds);              // I would like to expend this into another state_machine:
                                     // target, shoot, sound of Storm Troopers or pirates going "extinct".
EFFECT(mandosrifle);                 // I would like to expend this into another state_machine:
                                     // load, shoot, sound of Jawas going "extinct".
// ==============================================================================================================================

class JetpackOliButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  JetpackOliButtons() : PropBase() {}
  const char* name() override { return "JetpackOliButtons"; }

  void PerformMissileLaunchSequence() {
    STATE_MACHINE_BEGIN();
    while (true) {
      if (missilelaunch_) {
        PVLOG_STATUS << "** Starting Missile Launch Sequence\n";
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
          PVLOG_STATUS << "** Mando: Nice shot!\nBoba: I was aiming for the other one!\n";
        }
        // create animation for OLED of viewfinder going back up
        SaberBase::DoEffect(EFFECT_DISARMING,0);
        SLEEP(SaberBase::sound_length * 1000);
        PVLOG_NORMAL << "** Missile Launch Sequence Completed!\n";
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
      //default: SaberBase::DoEffect(EFFECT_DUD,0); break; // <-- Is a "default" even needed ???
    }
  }

  void StopPlayNextLoopingSound(Effect* sound_name,const char* message1,EffectType effect,const char* message2) {
    RefPtr<BufferedWavPlayer> idlePlayer = GetWavPlayerPlaying(sound_name);
    if (idlePlayer) {
      idlePlayer->set_fade_time(0.5);
      idlePlayer->FadeAndStop();
      idlePlayer.Free();
      PVLOG_NORMAL << message1 << "\n";
      }
    if (jetpack_wav_player_.isPlaying()) {
      return;
    }
    SaberBase::DoEffect(effect, 0);
    PVLOG_STATUS << message2 << "\n";
  }

  // Initial start, transition to Idle Mode (from 0 to idle)
  void StartIdleMode() {
    StopPlayNextLoopingSound(&SFX_hum,"**** Stopped hum wav",EFFECT_STARTIDLEMODE,"**** Jetpack Idling");
    flight_ = false;
    idle_   = true;
    idle_timer_  = millis(); // Reset idle timer
    FastOn();
  }

  // Transition to Flying Mode (from idle to flying)
  void StartFlightMode() {
    StopPlayNextLoopingSound(&SFX_idlemode,"**** Stopped idlemode wav",EFFECT_STARTFLIGHTMODE,"**** Jetpack Flying");
    flight_ = true;
    idle_   = false;
  }

  // Transition from Flying Mode (from flying to idle)
  void StopFlightMode() {
    StopPlayNextLoopingSound(&SFX_flightmode,"**** Stopped flightmode wav",EFFECT_STOPFLIGHTMODE,"**** Jetpack Idling");
    flight_ = false;
    idle_   = true;
    idle_timer_  = millis(); // Reset idle timer
  }

  // Stop Idle Mode (Jetpack completely off - from flight or idle to off)
  void StopIdleMode() {
    StopPlayNextLoopingSound(&SFX_idlemode,"**** Stopped idlemode wav",EFFECT_STOPIDLEMODE,"**** Jetpack Stopped");
    flight_ = false; // Should already be false but StopIdleMode can also be used to turn the
                     // jetpack completely off from any state (with a 2 buttons setup).
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

  void Setup() {
   sound_library_v2.init();
  }

  void FusorWeapons() {
    if (!idle_ && !flight_) { // Jetpack is OFF
      if (fusor.angle1() > M_PI / 9) {
        // Pointing towards UP (above +20°)
        SaberBase::DoEffect(EFFECT_MANDOSRIFLE, 0);
      } else {
        if (fusor.angle1() < -M_PI / 9) {
          // Pointing towards DOWN (below -20°)
          missilelaunch_ = true;
        } else {
          // Not pointing UP or DOWN
          SaberBase::DoEffect(EFFECT_WHISTLINGBIRDS, 0);
        }
      }
    } else {
      missilelaunch_ = true;
    }
  }

  void SetIdleOrFlight() {
    if (idle_) StartFlightMode();    // Transition from idle to flight  (rev more up)
    else
      if (flight_) StopFlightMode(); // Transition from flight to idle     (rev down)
      else StartIdleMode();          // Jetpack initial start from 0 to idle (rev up)
  }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

      // Completely turn jetpack off from any state (flight or idle):
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
        StopIdleMode(); // This will stop the jetpack but not the missile.
        return true;

      // Jetpack mishaps:
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        JetpackMishaps();
        return true;

      // Missile Launch: (& other weapons based on fusor)
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        //missilelaunch_ = true;
        FusorWeapons();
        return true;

#if NUM_BUTTONS >= 2
      // Jetpack ON: - if not in idle or in flight, it revs up to idle.
      //             - if in idle, it revs "more up" to flight.
      //             - if in flight, it revs down to idle.
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        SetIdleOrFlight();
        return true;

      // Volume Up/Down (ON or OFF) & Next/Previous Preset (OFF):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        if (mode_volume_) FusorVolume();
        else FusorPreset();
        return true;
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        if (mode_volume_) FusorVolume();
        return true;

      // Enter/Exit Volume Menu:
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        EnterExit_SA22C_VolumeMenu();
        return true;

      // Start or Stop Track
      case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
        StartAndStopTrack();
        return true;

#ifdef JETPACK_TRACK_PLAYER
      // Play Next/Previous Track
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        FusorTrack();
        return true;
#endif // JETPACK_TRACK_PLAYER

#else // NUM_BUTTONS = 1
      // Jetpack ON: - if not in idle or in flight, it revs up to idle.
      //             - if in idle, it revs "more up" to flight.
      //             - if in flight, it revs down to idle.
      // Volume Up/Down (ON or OFF)
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        if (!mode_volume_) SetIdleOrFlight();
        else FusorVolume();
        return true;

      // Next/Previous Preset (OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        FusorPreset();
        return true;

      // Enter/Exit Volume Menu:
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
        EnterExitVolumeMenu();
        return true;

      // Start or Stop Track
      case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD_MEDIUM, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD_MEDIUM, MODE_OFF):
        StartAndStopTrack();
        return true;

#ifdef JETPACK_TRACK_PLAYER
      // Play Next/Previous Track
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
        FusorTrack();
        return true;
#endif // JETPACK_TRACK_PLAYER
#endif // NUM_BUTTONS >= 2

      // Battery level:
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):
        FusorBatteryLevel();
        return true;

/* For multiple blade detect part 3A/5
(Need to add part 1/5 to events.h 2/5 to config, part 3A/5 & 3B/5 to props,
 part 4/5 to to ProffieOS.ino,part 5A/5 & 5B/5 to prop_base.h) */
#ifndef PROPS_MULTI_PROP_H
#ifdef BLADE_DETECT_PIN
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        UpdateBladeInserted (1, true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        UpdateBladeInserted (1, false);
        return true;
#endif // BLADE_DETECT_PIN

#ifdef BLADE_DETECT_PIN2
      case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        UpdateBladeInserted (2, true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT2, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        UpdateBladeInserted (2, false);
        return true;
#endif // BLADE_DETECT_PIN2

#ifdef BLADE_DETECT_PIN3
      case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        UpdateBladeInserted (3, true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT3, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        UpdateBladeInserted (3, false);
        return true;
#endif // BLADE_DETECT_PIN3
#endif // PROPS_MULTI_PROP_H

    }  // switch (EVENTID)
    return false;
  }  // Event2

/* For multiple blade detect part 3B/5
(Need to add part 1/5 to events.h 2/5 to config, part 3A/5 & 3B/5 to props,
 part 4/5 to to ProffieOS.ino,part 5A/5 & 5B/5 to prop_base.h) */
#ifndef PROPS_MULTI_PROP_H
  void UpdateBladeInserted (int blade_detect_index, bool blade_inserted) {
    switch (blade_detect_index) { // blade_detect_index = 1, 2 or 3
#ifdef BLADE_DETECT_PIN
      case 1:
        blade_detected_ = blade_inserted;
        break;
#endif
#ifdef BLADE_DETECT_PIN2
      case 2:
        blade2_detected_ = blade_inserted;
        break;
#endif
#ifdef BLADE_DETECT_PIN3
      case 3:
        blade3_detected_ = blade_inserted;
        break;
#endif
    }
    FindBladeAgain();
    SaberBase::DoBladeDetect(blade_inserted);
    PVLOG_DEBUG << "****** Blade detect/undetect done from jetpack_Oli_buttons.\n";
  }
#endif // PROPS_MULTI_PROP_H

  // Helper (to avoid repetition)
  void EffectHelper (Effect* effect1,const char* msg1) {
    hybrid_font.PlayPolyphonic(effect1);
    PVLOG_NORMAL << msg1 << "\n";
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      default: return;
      // Missile effects:
      //=================
      case EFFECT_ARMING:          EffectHelper(&SFX_arming,                   "** Arming"); return; // Must not be
      case EFFECT_TARGETING:       EffectHelper(&SFX_targeting,             "** Targeting"); return; // interrupted
      case EFFECT_MISSILELAUNCH:   EffectHelper(&SFX_missilelaunch,    "** Missile Launch"); return; // and must be
      case EFFECT_MISSILEGOESBOOM: EffectHelper(&SFX_missilegoesboom,"** Missile Explodes"); return; // able to play in
      case EFFECT_MANDOTALK:       EffectHelper(&SFX_mandotalk,  "** Mando & Boba Talking"); return; // sequence with
      case EFFECT_DISARMING:       EffectHelper(&SFX_disarming,      "** Disarm Targeting"); return; // one another!
      // Other weapon(s) effects:
      // ========================
      case EFFECT_WHISTLINGBIRDS:  EffectHelper(&SFX_whistlingbirds,  "** Whistling birds, use them wisely!"); return;
      case EFFECT_MANDOSRIFLE:     EffectHelper(&SFX_mandosrifle,        "**Mando's rifle"); return;
      // Engine mishap effects:
      // ======================
      case EFFECT_FALSESTART:      EffectHelper(&SFX_falsestart,          "** False Start"); return;
      case EFFECT_STUTTERING:      EffectHelper(&SFX_stuttering,           "** Stuttering"); return;
      case EFFECT_SELFDESTRUCT:    EffectHelper(&SFX_selfdestruct,      "** Self Destruct"); return;
      case EFFECT_MELTDOWN:        EffectHelper(&SFX_meltdown,               "** Meltdown"); return;
      case EFFECT_DUD:             EffectHelper(&SFX_dud,   "** Dank Farrik!\nIt's a Dud!"); return;
      // Engine normal effects:
      // ======================
      case EFFECT_STARTIDLEMODE:   EffectHelper(&SFX_startidlemode,     "** Jetpack Starting to Idle."); return; //after startidlemode, play idle mode on loop
      case EFFECT_IDLEMODE:        PVLOG_DEBUG << "** Jetpack in Idle Loop Mode." << "\n";               return; //plays on loop
      case EFFECT_STARTFLIGHTMODE: EffectHelper(&SFX_startflightmode, "** Jetpack Starting to Flight."); return; //after startflightmode, play flight mode on loop
      case EFFECT_FLIGHTMODE:      PVLOG_DEBUG << "** Jetpack in Flight Loop Mode." << "\n";             return; //plays on loop
      case EFFECT_STOPFLIGHTMODE:  EffectHelper(&SFX_stopflightmode,"** Jetpack Slowing Down to Idle."); return; //after stopflightmode, play idle on loop
      case EFFECT_STOPIDLEMODE:    EffectHelper(&SFX_stopidlemode,        "** Jetpack Completely Off."); return;
      // System effects: (info found in config/styles/blade-effects.md, https://pod.hubbe.net/config/styles/blade-effects.html)
      // ===============
        // Shows volume level visually on blade, great for using with volume menu feature.
      case EFFECT_VOLUME_LEVEL:    PVLOG_DEBUG << "*** Blade Effect Volume Level\n";           return;
        // Typically used to apply a dimming effect on blade to conserve battery.
        // Here it will be used for idle mode.
      case EFFECT_POWERSAVE:       sound_library_v2.SayDim();
                                   PVLOG_DEBUG << "*** Effect Powersave/Dim\n";                return;
        // On-Demand Battery Level. Shows battery level visually on blade.
      case EFFECT_BATTERY_LEVEL:   sound_library_.SayBatteryLevel();
                                   PVLOG_DEBUG << "*** Blade Effect Battery Level\n";
                                   PVLOG_NORMAL << "***** May the Force be with you. *****\n"; return;
        // Triggers the change for sets of sounds within the font from one alternative to another.
      case EFFECT_ALT_SOUND:                                                                   return;
        // https://pod.hubbe.net/sound/alt_sounds.html#have-the-prop-control-the-alt "SaberBase::DoEffect(EFFECT_ALT_SOUND, 0.0, N);" N is the alt value!
#ifdef JETPACK_TRACK_PLAYER
      case EFFECT_TRACK:           PlayEffectTrack();                                          return;
#endif
    }  // switch (effect)
  }  // SB_Effect

  void FusorPreset() {
    if (fusor.angle1() > M_PI / 9) {
      // Pointing towards UP (above +20°)
      next_preset();
      PVLOG_DEBUG << "** Next preset\n";
    } else {
      if (fusor.angle1() < -M_PI / 9) {
        // Pointing  towards DOWN (below -20°)
        previous_preset();
        PVLOG_DEBUG << "** Previous preset\n";
      } else {
        // Not pointing towards UP or DOWN (between -20° & +20°)
        first_preset();
        PVLOG_DEBUG << "** Jumped to first preset\n";
      }
    }
  }

  void FusorVolume() {
    if (fusor.angle1() > M_PI / 9) {
      // Pointing towards UP (above +20°)
      VolumeUp();
    } else {
      if (fusor.angle1() < -M_PI / 9) {
        // Pointing towards DOWN (below -20°)
        VolumeDown();
      } else {
        //Not pointing towards UP or DOWN (between -20° & +20°)
        //MinimumVolume(); // <- ToDo !!!
      }
    }
  }

  // Based on SA22C Volume Menu
  void EnterExit_SA22C_VolumeMenu() {
    if (!mode_volume_) {
      mode_volume_ = true;
      sound_library_.SayEnterVolumeMenu();
      PVLOG_DEBUG << "* Enter Volume Menu\n";
    } else {
      mode_volume_ = false;
      sound_library_.SayVolumeMenuEnd();
      PVLOG_DEBUG << "* Exit Volume Menu\n";
    }
  }

  // Based on SA22C Volume Menu
  void VolumeUp() {
    PVLOG_DEBUG << "* Volume Up\n";
    if (dynamic_mixer.get_volume() < VOLUME) {
      dynamic_mixer.set_volume(std::min<int>(VOLUME + VOLUME * 0.1, dynamic_mixer.get_volume() + VOLUME * 0.1));
      sound_library_.SayVolumeUp();
      PVLOG_DEBUG << "* Volume Up - Current Volume: " << dynamic_mixer.get_volume() << "\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    } else {
      // Cycle through ends of Volume Menu option
#ifdef VOLUME_MENU_CYCLE
      if (!max_vol_reached_) {
        sound_library_.SayMaximumVolume();
        PVLOG_DEBUG << "* Maximum Volume\n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        max_vol_reached_ = true;
      } else {
        dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1,
        dynamic_mixer.get_volume() - VOLUME * 0.9));
        sound_library_.SayMinimumVolume();
        PVLOG_DEBUG << "* Minimum Volume\n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        max_vol_reached_ = false;
      }
#else
      sound_library_.SayMaximumVolume();
      PVLOG_DEBUG << "* Maximum Volume\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
#endif
    }
  }

  // Based on SA22C Volume Menu
  void VolumeDown() {
    PVLOG_DEBUG << "* Volume Down\n";
    if (dynamic_mixer.get_volume() > (0.1 * VOLUME)) {
      dynamic_mixer.set_volume(std::max<int>(VOLUME * 0.1, dynamic_mixer.get_volume() - VOLUME * 0.1));
      sound_library_.SayVolumeDown();
      PVLOG_DEBUG << "* Volume Down - Current Volume: " << dynamic_mixer.get_volume() << "\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    } else {
      // Cycle through ends of Volume Menu option
#ifdef VOLUME_MENU_CYCLE
      if (!min_vol_reached_) {
        sound_library_.SayMinimumVolume();
        PVLOG_DEBUG << "* Minimum Volume\n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        min_vol_reached_ = true;
      } else {
        dynamic_mixer.set_volume(VOLUME);
        sound_library_.SayMaximumVolume();
        PVLOG_DEBUG << "* Maximum Volume\n";
        SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
        min_vol_reached_ = false;
      }
#else
      sound_library_.SayMinimumVolume();
      PVLOG_DEBUG << "* Minimum Volume\n";
      SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
#endif
    }
  }

  // Based on BlasterBC DoSpokenBatteryLevel()
  void FusorBatteryLevel() {
    // Avoid weird battery readings when using USB
    if (battery_monitor.battery() < 0.5) {
      sound_library_.SayTheBatteryLevelIs();
      sound_library_.SayDisabled();
      return;
    }
    SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
    sound_library_.SayTheBatteryLevelIs();
    // Pointing towards UP (above +20°)
    if (fusor.angle1() > M_PI / 9) {
      // Battery Level in Percent
      sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
      sound_library_.SayPercent();
      PVLOG_DEBUG << "*** Battery Percentage: " <<battery_monitor.battery_percent() << "%\n";
      //is_speaking_ = true;
    } else {
      // Pointing towards DOWN (below -20°)
      if (fusor.angle1() < -M_PI / 9) {
        // Battery Level in Volts
        sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
        sound_library_.SayVolts();
        PVLOG_DEBUG << "*** Battery Voltage: " << battery_monitor.battery() << "\n";
        //is_speaking_ = true;
      } else {
        // Not pointing towards UP or DOWN (between -20° & +20°)
        // Battery Level in Percent & Volts
        sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
        sound_library_.SayPercent();
        sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
        sound_library_.SayVolts();
        PVLOG_DEBUG << "*** Battery Percentage: " <<battery_monitor.battery_percent() << "%, Battery Voltage: " << battery_monitor.battery() << "\n";
      }
    }
  }

  // From BlasterBC
  void DoQuote() {
    if (SFX_quote) {
      if (GetWavPlayerPlaying(&SFX_quote)) return;  // Simple prevention of quote overlap
      sequential_quote_ ? SFX_quote.SelectNext() : SFX_quote.Select(-1);
      hybrid_font.PlayCommon(&SFX_quote);
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
      PVLOG_ERROR << "* No tracks available to play.\n";
      return;
    }
    char playtrack[128];
    RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, playtrack, sizeof(playtrack));
    MountSDCard();
    EnableAmplifier();
    track_player_ = GetFreeWavPlayer();
    if (track_player_) track_player_->Play(playtrack);
    else PVLOG_ERROR << "* No available WAV players.\n";
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
      PVLOG_ERROR << "* No tracks available.\n";
      return;
    }
    track_num_ = (track_num_ % num_tracks_) + 1; // Cycle to the next track
    PlayTrack();
  }

  void PreviousTrack() {
    if (num_tracks_ <= 0) {
      PVLOG_ERROR << "* No tracks available.\n";
      return;
    }
    track_num_ = (track_num_ - 2 + num_tracks_) % num_tracks_ + 1; // Cycle to the previous track
    PlayTrack();
  }

  void FusorTrack() {
    if (fusor.angle1() > M_PI / 9) {
      // Pointing towards UP (above +20°)
      NextTrack();
      PVLOG_DEBUG << "** Next track\n";
    } else {
      //if (fusor.angle1() < -M_PI / 9) {
        // Pointing  towards DOWN (below -20°)
        PreviousTrack();
        PVLOG_DEBUG << "** Previous track\n";
      //}
      // "else" can be added here for: Not pointing towards UP or DOWN (between -20° & +20°)
    }
  }

  bool chdir(const StringPiece dir) override {
    bool ret = PropBase::chdir(dir);
    num_tracks_ = RunCommandAndGetSingleLine("list_current_tracks", nullptr, 0, 0, 0);
    track_num_ = 0;
    return ret;
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (PropBase::Parse(cmd, arg)) return true;
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
#endif // JETPACK_TRACK_PLAYER

  void StartAndStopTrack() {
#ifdef JETPACK_TRACK_PLAYER
    if (!istrack_playing_)
      StartTrackPlayer();
    else
      StopTrackPlayer();
#else
    StartOrStopTrack();
#endif
  }

private:
  // State variables
  unsigned long idle_timer_     = 0;        // Timer for idle mode timeout
  bool idle_                    = false;    // Whether the jetpack is in  idle  mode
  bool flight_                  = false;    // Whether the jetpack is in flight mode
  bool missilelaunch_           = false;    // Whether the missile is launching
  StateMachineState state_machine_;         // Required for the SLEEP macro

  // Sound effects handling
  PlayWav jetpack_wav_player_;              // Handles idle, flight, and mishap sounds
  //bool is_speaking_           = false;    // from blasterBC for prevent overlap
  bool sequential_quote_        = false;    // from blasterBC for quote

  // Track player handling
#ifdef JETPACK_TRACK_PLAYER
  bool istrack_playing_         = false;    // Tracks if track is currently playing (for Event2 switch)
  int  track_num_               = 0;        // Current track number
  int  num_tracks_              = 0;        // Total number of tracks available
  TrackMode track_mode_     = PLAYBACK_OFF; // Current playback mode
#endif

  // Volume handling
  bool mode_volume_             = false;    // from sa22c for volume menu
  bool max_vol_reached_         = false;    // from sa22c for volume menu
  bool min_vol_reached_         = false;    // from sa22c for volume menu

}; // class JetpackOliButtons

#endif // PROPS_JETPACK_OLI_BUTTONS_H

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
  X(disarming)                      \
/* == other weapons == */           \
  X(whistlingbirds)                 \
  X(mandosrifle)

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
    // == other weapon(s) ==
    CONFIG_VARIABLE2(ProffieOSWhistlingBirdsImageDuration,  1000.0f);
    CONFIG_VARIABLE2(ProffieOSMandosRifleImageDuration,     1000.0f);
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
  // for OLED displays, the time a disarming.bmp       will play.
  float ProffieOSDisarmingImageDuration;
  // == other weapons ==
  // for OLED displays, the time a whistlingbirds.bmp  will play.
  float ProffieOSWhistlingBirdsImageDuration;
  // for OLED displays, the time a mandosrifle.bmp     will play.
  float ProffieOSMandosRifleImageDuration;
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
        if (img_.dud) {                                      // animation TBD
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
          ShowFileWithSoundLength(&img_.IMG_arming,          jetpack_font_config.ProffieOSArmingImageDuration);
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

      // other weapons display effects:
      // ==============================
      case EFFECT_WHISTLINGBIRDS:
        if (img_.IMG_whistlingbirds) {                       // animation of whistling birds firing
          ShowFileWithSoundLength(&img_.IMG_whistlingbirds,  jetpack_font_config.ProffieOSWhistlingBirdsImageDuration);
        } else {
          this->SetMessage("whistling\n birds");
          this->SetScreenNow(SCREEN_MESSAGE);
        }
        break;

      case EFFECT_MANDOSRIFLE:
        if (img_.IMG_mandosrifle) {                          // animation of rifle slow motion firing
          ShowFileWithSoundLength(&img_.IMG_mandosrifle,     jetpack_font_config.ProffieOSMandosRifleImageDuration);
        } else {
          this->SetMessage("mando's\n rifle");
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
      // other weapons display effects:
      // ==============================
      case EFFECT_WHISTLINGBIRDS:  this->scr_.Play(&SCR_whistlingbirds);  break;
      case EFFECT_MANDOSRIFLE:     this->scr_.Play(&SCR_mandosrifle);     break;
      default:
        StandarColorDisplayController<W, H, PREFIX>::SB_Effect2(effect, location);
    }
  }

protected:
  ONCE_PER_JETPACK_EFFECT(DEF_SCR);
};

#undef ONCE_PER_JETPACK_EFFECT

#endif  // PROP_BOTTOM


