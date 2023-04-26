/* Fett263 Buttons for use with 1, 2 or 3 Button Sabers*
*1 Button Controls based on SA22C prop
Includes Gesture Controls, Battle Mode 2.0, Edit Mode, Track Player, Quote/Force Player, Real Clash, Choreography Mode
   Dual Mode Ignition Sounds, Multi-Phase Control, Multi-Blast

 ProffieOS: Control software for lightsabers and other props.
 http://fredrik.hubbe.net/lightsaber/teensy_saber.html
 Copyright (c) 2016-2019 Fredrik Hubinette

 Fett263 Button (prop) file, "Battle Mode 2.0", "Edit Mode", "Track Player", "Real Clash", "Choreography Mode", "Dual Mode Ignition",
 "Multi-Phase", "Multi-Blast"
 Copyright (c) 2022-2023 Fernando da Rosa
 Visit https://www.fett263.com/proffieOS7-fett263-prop-file.html for required set up and additional information
 
 Voice Prompts and sounds required for certain features and should be included in /common folder or /font folder on SD card.
   Free prompts (courtesy of Brian Conner) available here: http://fredrik.hubbe.net/lightsaber/sound/

 Track Player requires track files to be located in /font/tracks for font specific tracks or /common/tracks for universal (all presets) or a combination of the two.
 
  ----- This prop enables the following EFFECTs for use in menus, Special Abilities* and/or chained effects controlled at the style level in each preset -----
  *requires FETT263_SPECIAL_ABILITIES define

   EFFECT_BATTERY_LEVEL (to display/say* battery level) *requires FETT263_SAY_BATTERY_VOLTS or FETT263_SAY_BATTERY_PERCENT
   EFFECT_VOLUME_LEVEL (to display current volume level)
   EFFECT_POWERSAVE (blade dimming / powersave option)
   EFFECT_QUOTE (random quote* cannot be mixed with EFFECT_NEXT_QUOTE)
   EFFECT_NEXT_QUOTE (sequential quote)
   EFFECT_TRACK (plays previously selected Track (via Track Player) -or- preset's default track if "tracks" folder is not found)
   EFFECT_ALT_SOUND (enables Alt font selection from style)
   EFFECT_TRANSITION_SOUND (enable custom transition sounds)
   EFFECT_SOUND_LOOP (enable custom looped sound toggled on/off with each call)
   EFFECT_BEGIN_BATTLE_MODE (begin Battle Mode)
   EFFECT_END_BATTLE_MODE (end Battle Mode)
   EFFECT_BEGIN_AUTO_BLAST (begin Multi-Blast Mode)
   EFFECT_END_AUTO_BLAST (end Multi-Blast Mode)
   EFFECT_INTERACTIVE_PREON (allows prop to recognize Interactive Preon effects for primary control)
   EFFECT_INTERACTIVE_BLAST (allows prop to recognize Interactive Blast effects for primary control)
   EFFECT_ON (enable normal ignition from chained effect while OFF)
   EFFECT_FAST_ON (enable ignition without preon from chained effect while OFF)
   EFFECT_OFF (enable normal retraction from chained effect while ON)
   EFFECT_FAST_OFF (enable retraction without pstoff from chained effect while ON)
   EFFECT_SECONDARY_IGNITION (to enable separate "secondary" blade ignition control)
   EFFECT_SECONDARY_RETRACTION (to enable separate "secondary" blade retraction control)

----- Available Mini Game EFFECTs -----

    EFFECT_GAME_START (begin game effects)
    EFFECT_GAME_ACTION1 (game action 1)
    EFFECT_GAME_ACTION2 (game action 2)
    EFFECT_GAME_CHOICE (interactive choice based on action)
    EFFECT_GAME_RESPONSE1 (available response for action 1)
    EFFECT_GAME_RESPONSE2 (available response for action 2)
    EFFECT_GAME_RESULT1 (result of choice for action 1)
    EFFECT_GAME_RESULT2 (result of choice for action 2)
    EFFECT_GAME_WIN (end game with win)
    EFFECT_GAME_LOSE (end game with loss)

---------- 2 / 3 Button Controls ----------
NOTE: 
  Click = do short click
  Long Click = hold button for 1 second and release
  Hold = hold button down
  
Standard Controls While Blade is OFF
  Turn On / Ignite Saber* = Click PWR
    *If FETT263_MOTION_WAKE_POWER_BUTTON defined first Click will Wake up motion detection and boot sound will play
  Turn On / Ignite Saber (Muted) = Double Click PWR
  Change Preset (one at a time*) = Click AUX
    *if pointing down will go to previous
  NEW! Scroll Presets (using twist menu) = Long Click AUX
    Turn Right (Stepped) = Next Preset
      Increment by 5 = Hold PWR + Turn Right
    Turn Left (Stepped) = Previous Preset
      Increment by 5 = Hold PWR + Turn Left
    Click PWR = Select Preset
    NEW! Hold PWR = Select and Ignite Preset
    Click AUX = go to First Preset
  Play Track = Long Click PWR pointing up
  NEW! Track Player* = Long Click PWR parallel
  *requires tracks in either font/tracks/ or common/tracks/
  *if no tracks in font or common will "Loop" default track
    Turn Right (Stepped) = Next Track
    Turn Left (Stepped) = Previous Track
    Click PWR = Play Current Track Once
    Click AUX = Random (will play current track and then randomly select next tracks)
    Hold PWR + Turn Right = Rotate (will play current track and then next sequential tracks)
    Hold PWR + Turn Left = Loop Current Track
    Long Click PWR = Stop Track Player
  NEW! Force/Quote Player = Hold PWR 
    If quotes exist in current font pointing straight down will toggle between Force/Quote and play
    *Quotes play sequentially 1,2,3...
    If parallel will do Force/Quote based on current mode
  NEW! Toggle Gesture Sleep* = Hold PWR + Clash
    *toggles gesture controls on/off
    *gestures sleep automatically if Blade Detect is enabled and blade is missing
  NEW! Toggle Spin Mode* = Hold PWR + Swing
    Disables Clash, Stab and Lockup effects to allow for spinning and flourishes
    Will play bmbegin.wav or force.wav when toggled ON/OFF
    *requires FETT263_SPIN_MODE define
  Special Abilities (Style Controlled) (requires FETT263_SPECIAL_ABILITIES)
    Hold PWR + Turn Right = Special Ability 5 (USER5)
    Hold PWR + Turn Left = Special Ability 6 (USER6)
    Hold AUX + Turn Right = Special Ability 7 (USER7)
    Hold Aux + Turn Left = Special Ability 8 (USER8)
  NEW Control! Volume Menu = Hold PWR, Click AUX
    Turn Right (Stepped) = Increase Volume (to max)
    Turn Left (Stepped) = Decrease Volume (to min)
    Click PWR or AUX = Exit
  Check Battery Level*  = Hold AUX, Click PWR
    *requires EFFECT_BATTERY_LEVEL style and/or FETT263_SAY_BATTERY_VOLTS or FETT263_SAY_BATTERY_PERCENT define
    *if using FETT263_BC_SAY_BATTERY_VOLTS_PERCENT
    Point down for volts, parallel or up for percent
  NEW! Change Font
    Next Font = Hold AUX + Long Click PWR (parallel or up)
    Previous Font= Hold AUX + Long Click PWR (down)
  NEW! Copy Preset = Hold PWR + Long Click AUX

Optional Gesture Controls (if enabled and Gesture Sleep is deactivated)
  Ignite Saber
    Swing On
    Stab On
    Twist On
    Thrust On

Standard Controls While Blade is ON
  Turn Off / Retract Blade* = Click PWR (Hold PWR**)
    *if PowerLock is disabled
    **using FETT263_HOLD_BUTTON_OFF define
  Turn Off / Retract Blade (PowerLock Mode) = Hold PWR + Hold AUX
  Blast Effect = Click Aux
  Multi-Blast Mode = Long Click Aux
    Each Swing in Multi-Blast Mode will deflect Blast effect
    To exit, click AUX or do Clash
  Clash Effect = Clash Saber
  Stab Effect = Stab (thrust and impact tip of blade on object)
  Lockup Effect = Hold PWR + Clash Saber (Hold AUX**)
      **using FETT263_HOLD_BUTTON_LOCKUP
      **Battle Mode changes to Hold AUX + Swing with this define
  Drag Effect = Hold AUX + Stab Down
  Melt Effect = Hold AUX + Stab Parallel or Up
  Lightning Block Effect = Hold PWR + click AUX
  NEW! Force/Quote = Long Click PWR (parallel or down)
    If quotes exist in current font pointing straight down will toggle between Force/Quote and play
    *Quotes play sequentially 1,2,3...
    If parallel will do Force/Quote based on current mode
  Start/Stop Tracks = Long Click PWR (pointing straight up)
    *default track only (use Track Player while OFF to select tracks or playback modes)
  Color Change = Hold AUX + Click PWR (parallel or down)
    Rotate Hilt to select color (unless ColorChange<> style is used with COLOR_CHANGE_DIRECT*)
      If styles use Edit Mode Color Editing styles, Color List is used
      If styles use ColorChange<> then colors within the style are used
        *if COLOR_CHANGE_DIRECT is defined then each click will change color instead of turn
      Otherwise ColorWheel is used per style set up.
    Click PWR to save
    Click AUX to revert
    NEW! Color Zoom* = Hold PWR, Release to Save
      *For Color List or ColorWheel you can Hold PWR down to zoom in color for easier selection
       Release PWR to save
  Power Save* = Hold AUX + Click PWR (pointing straight up)
    *requires EFFECT_POWERSAVE in style
  NEW! Change Style (All Blades)
    Next Style = Hold AUX + Long Click PWR (parallel or up)
    Previous Style = Hold AUX + Long Click PWR (down)
  Multi-Phase Preset Change*
    *requires FETT263_MULTI_PHASE define, cannot be used with FETT263_SPECIAL_ABILITIES
    Hold AUX + Twist =  Next Preset
    Hold PWR + Twist = Previous Preset
  Special Abilities (Style Controlled) (requires FETT263_SPECIAL_ABILITIES)
    Hold PWR + Turn Right = Special Ability 1 (USER1)
    Hold PWR + Turn Left = Special Ability 2 (USER2)
    Hold AUX + Turn Right = Special Ability 3 (USER3)
    Hold Aux + Turn Left = Special Ability 4 (USER4)
Optional Gesture Controls (if enabled)
  Retract Blade
    Twist Off

"Battle Mode" Controls* - While ON
    *may vary by defines
  Enter/Exit Battle Mode = Hold AUX (Hold AUX + Swing**)
      **if FETT263_HOLD_BUTTON_LOCKUP defined
  Clash / Lockup = controlled by gesture
    Clash blade
      If blade swings through the clash it will do a "glancing Clash"
      If using FETT263_BM_CLASH_DETECT 6 define (Battle Mode 2.0) normal clash used for hits below the
        FETT263_BM_CLASH_DETECT value (1 ~ 8), if undefined or equal to 0 then Battle Mode 1.0 is used.
      If blade stops/slows on clash the saber will initiate Begin Lockup
      To perform a "clash" do an immediate Pull Away this will transition from Begin Lockup to End Lockup in quick succession
      To Lockup, steady the blade after Clash
      To end Lockup do Pull Away
  Drag / Melt = controlled by gesture
    Stab (thrust with impact at tip of blade)
      If pointing down Drag will initiate
      To end Drag pull blade up from floor at an angle
      If parallel or up Melt will initiate
      To end Melt pull blade away from object at an angle
  Blast Effect = Click AUX
    After Blast, swing within 2 seconds to enter Multi-Blast Mode
  Multi-Blast Mode = Long Click AUX
    Each Swing in Multi-Blast Mode will deflect Blast effect
    To exit, click AUX or do Clash
  Lightning Block = Hold PWR, Click AUX
  Force Push* = Push Saber
    *requires FETT263_FORCE_PUSH
  NEW! Force/Quote = Long Click PWR (parallel or down)
    If pointing down will toggle Force/Quote mode and do Force Effect or play Quote accordingly
    *Quote plays sequentially
    If parallel will do Force/Quote
  Start/Stop Tracks = Long Click PWR (pointing up)
    *default track only (use Track Player while OFF to select tracks or playback modes)

Rehearsal / Choreography Modes*
  *requires FETT263_SAVE_CHOREOGRAPHY define, cannot be used with FETT263_SPECIAL_ABILITIES
  Begin Rehearsal** = While Off, Hold AUX + Twist
      **If a Saved Rehearsal Exists it will prompt you to "Replace?"
        To confirm Turn the hilt Right (Clockwise) to "Accept" and Click PWR to begin a new Rehearsal
        To keep saved rehearsal Click AUX and Rehearsal Mode will be cancelled.
    Saber will Ignite in Rehearsal Mode
    In Rehearsal Mode, standard Clash and Lockup controls are used to record sequence
  Clash = Clash
  Hold PWR + Clash = Lockup
    Rehearsal will also record the sound files used for each effect to repeat in Choreography
  Cancel Rehearsal Mode = Hold AUX
  Save Rehearsal = Hold PWR
  Begin Choreography = While Off, Hold AUX - or - Hold AUX + Swing
    During Choreography Mode Clashes, Lockups and sound files are replayed in sequence
    When recorded sequence completes the saber goes into Battle Mode automatically
    If no saved rehearsal is available for font saber will ignite in Battle Mode*
    *may vary by define
    During Choreography PWR button is disabled
  Turn Off = Hold AUX + Hold PWR

Edit Mode*
    *requires FETT263_EDIT_MODE_MENU & ENABLE_ALL_EDIT_OPTIONS defines
    *requires /common folder with all menu prompt sounds
  Enter Edit Mode = While Off, Hold AUX + Hold PWR
    If menu prompt wav files are missing from preset you will get "Error in Font Directory" warning, refer to Edit Mode setup and requirements

  While in Edit Mode controls are as follows:
    Rotate Forward, Increase Value, Confirm "Yes" = Turn Right (Stepped)
      Increment by 5 (Fonts, Tracks, Blade Length) = Hold PWR + Turn Right
      Increment by 500 (Ignition Time, Ignition Delay, Retraction Time, Retraction Delay) = Hold PWR + Turn Right
      Increment by 5000 (Ignition Option2, Retraction Option2) = Hold PWR + Turn Right
    Rotate Back, Decrease Value, Confirm "No" = Turn Left (Stepped)
      Increment by 5 (Fonts, Tracks, Blade Length) = Hold PWR + Turn Left
      Increment by 500 (Ignition Time, Ignition Delay, Retraction Time, Retraction Delay) = Hold PWR + Turn Left
      Increment by 5000 (Ignition Option2, Retraction Option2) = Hold PWR + Turn Left
    Select, Save, Enter = Click PWR
    Cancel, Revert, Go Back = Click AUX
    Go to Main Menu (from sub-menu) - Hold AUX
    Exit Edit Mode - Hold AUX (or rotate to "Exit") while in Main Menu

  "Edit Color" Additional Control
    "Color List" and "Adjust Color Hue" Zoom Mode = Hold PWR while turning to Zoom color in, release to save

Edit Settings* (Settings Only version of Edit Mode)
    *requires FETT263_EDIT_SETTINGS_MENU & ENABLE_ALL_EDIT_OPTIONS defines
    *requires /common folder with all menu prompt sounds
  Enter Edit Settings = While Off, Hold AUX + Hold PWR
    If menu prompt wav files are missing from preset you will get "Error in Font Directory" warning, refer to Edit Mode setup and requirements

  While in Edit Mode controls are as follows:
    Rotate Forward, Increase Value, Confirm "Yes" = Turn Right (Stepped)
      Increment by 5 (Blade Length) = Hold PWR + Turn Right
    Rotate Back, Decrease Value, Confirm "No" = Turn Left (Stepped)
      Increment by 5 (Blade Length) = Hold PWR + Turn Left
    Select, Save, Enter = Click PWR
    Cancel, Revert, Go Back = Click AUX
    Exit Edit Settings - Hold AUX

---------- 1 Button Controls (based on SA22C prop) ----------
NOTE: 
  Click = do short click (so Double Click is two short clicks in quick succession)
  Long Click = hold button for 1 second and release
  Hold = hold button down
  Click and Hold = hold on Xth click (so Double Click and Hold would be click twice and hold on second)
  Click + Long Click = do X clicks then do long click (so Double Click + Long Click would be click twice then do a long click)

Standard Controls While Blade is OFF
  Turn On / Ignite Saber* = Click PWR
    *If FETT263_MOTION_WAKE_POWER_BUTTON defined first Click will Wake up motion detection and boot sound will play  
  NEW Control! Turn On / Ignite Saber (Muted) = Click + Long Click PWR
  NEW Control! Start / Stop Tracks = Double Click PWR (pointing straight up)
  NEW! Track Player* = Double Click PWR (parallel or down)
    *if only default track exists in current preset, track will "Loop"
    Turn Right (Stepped) = Next Track
    Turn Left (Stepped) = Previous Track
    Click PWR = Play Current Track Once
    Hold PWR = Random (will play current track and then randomly select next tracks)
    Hold PWR + Turn Right = Rotate (will play current track and then next sequential tracks)
    Hold PWR + Turn Left = Loop Current Track
  NEW! Force/Quote Player - Triple Click PWR
    If quotes exist in current font pointing straight down will toggle between Force/Quote and play
    *Quotes play sequentially 1,2,3...
    If parallel will do Force/Quote based on current mode
  Special Abilities (Style Controlled) (requires FETT263_SPECIAL_ABILITIES)
    Hold PWR + Turn Right (parallel or up) = Special Ability 5 (USER5)
    Hold PWR + Turn Left (parallel or up) = Special Ability 6 (USER6)
    Hold PWR + Turn Right (pointing down) = Special Ability 7 (USER7)
    Hold PWR + Turn Left (pointing down) = Special Ability 8 (USER8)
  NEW! Toggle Spin Mode* = Hold PWR + Swing
    Disables Clash, Stab and Lockup effects to allow for spinning and flourishes
    Will play bmbegin.wav or force.wav when toggled ON/OFF
    *requires FETT263_SPIN_MODE define
  NEW! Toggle Gesture Sleep* = Hold PWR + Clash (pointing down)
    *toggles gesture controls on/off
    *gestures sleep automatically if Blade Detect is enabled and blade is missing
  Next Preset = Long Click PWR (parallel or up)
  NEW Control! Previous Preset = Long Click PWR (pointing down)
  NEW! Scroll Presets (using twist menu) = Hold PWR
    Turn Right (Stepped) = Next Preset
      Increment by 5 = Hold PWR + Turn Right
    Turn Left (Stepped) = Previous Preset
      Increment by 5 = Hold PWR + Turn Left
    Click PWR = Select Preset
    NEW! Hold PWR = Select and Ignite Preset
    Long Click PWR = First Preset
  NEW Control! Volume Menu = Hold PWR + Clash (parallel or up)
    Turn Right (Stepped) = Increase Volume (to max)
    Turn Left (Stepped) = Decrease Volume (to min)
    Click PWR = Exit
  NEW Control! Battery Level* = Double Click + Long Click PWR
    *requires EFFECT_BATTERY_LEVEL style and/or FETT263_SAY_BATTERY_PERCENT or FETT263_SAY_BATTERY_VOLTS define
    *if using FETT263_BC_SAY_BATTERY_VOLTS_PERCENT
    Point down for volts, parallel or up for percent
  NEW! Change Font
    Next Font = Triple Click + Long Click PWR (parallel or up)
    Previous Font = Triple Click + Long Click PWR (down)
  NEW! Copy Preset = Quadruple (Four) Click + Hold PWR

Optional Gesture Controls (if enabled and Gesture Sleep is deactivated)
  Ignite Saber
    Swing On
    Stab On
    Twist On
    Thrust On

Standard Controls While Blade is ON
  Turn Off / Retract Blade = Hold PWR
  Clash Effect = Clash Saber
  Lockup Effect = Hold PWR + Clash
  Stab Effect = Stab (thrust with impact at tip of blade)
  NEW Control! Drag Effect = Hold PWR + Stab (pointing straight down)
  Melt Effect = Hold PWR + Stab (parallel or up)
  Lightning Block = Double Click and Hold PWR
  Blast Effect = Click / Double Click / Triple Click PWR
  Multi-Blast Mode = Hold PWR + Swing
    Each Swing in Multi-Blast Mode will deflect Blast effect
    To exit, click PWR or do Clash
  NEW! Force/Quote = Long Click PWR
    If pointing down will toggle Force/Quote mode and do Force Effect or play Quote accordingly
    *Quote plays sequentially
    If parallel will do Force/Quote
  NEW! Stop Track* - Double Click PWR
    *if track is playing while ON
    To start/select track saber must be OFF
  NEW Control! Color Change = 4 Clicks PWR (parallel or down)
    Rotate Hilt to select color (unless ColorChange<> style is used with COLOR_CHANGE_DIRECT*)
      If styles use Edit Mode Color Editing styles, Color List is used
      If styles use ColorChange<> then colors within the style are used
        *if COLOR_CHANGE_DIRECT is defined then each click will change color instead of turn
      Otherwise ColorWheel is used per style set up.
    Click PWR to save
    NEW! Color Zoom* = Double Click and Hold PWR, Release to Save
      *For Color List or ColorWheel you can Hold PWR down to zoom in color for easier selection
       Release PWR to save
  NEW! Power Save* = 4 Clicks PWR (pointing straight up)
    *requires EFFECT_POWERSAVE in style
  Multi-Phase Preset Change*
    *requires FETT263_MULTI_PHASE define, cannot be used with FETT263_SPECIAL_ABILITIES
    Hold PWR + Twist (parallel or up) =  Next Preset 
    Hold PWR + Twist (pointing down) = Previous Preset
  Special Abilities (Style Controlled) (requires FETT263_SPECIAL_ABILITIES)
    Hold PWR + Turn Right (parallel or up) = Special Ability 1 (USER1)
    Hold PWR + Turn Left (parallel or up) = Special Ability 2 (USER2)
    Hold PWR + Turn Right (pointing down) = Special Ability 3 (USER3)
    Hold PWR + Turn Left (pointing down) = Special Ability 4 (USER4)
  NEW! Change Style (All Blades)
    Next Style = Triple Click + Long Click PWR (parallel or up)
    Previous Style = Triple Click + Long Click PWR (down)

Optional Gesture Controls (if enabled)
  Retract Blade
    Twist Off

"Battle Mode" Controls* - While ON
    *may vary by defines
  Enter/Exit Battle Mode = Triple Click and Hold PWR
  Clash / Lockup = controlled by gesture
    Clash blade
      If blade swings through the clash it will do a "glancing Clash"
      If using FETT263_BM_CLASH_DETECT 6 define (Battle Mode 2.0) normal clash used for hits below the
        FETT263_BM_CLASH_DETECT value (1 ~ 8), if undefined or equal to 0 then Battle Mode 1.0 is used.
      If blade stops/slows on clash the saber will initiate Begin Lockup
      To perform a "clash" do an immediate Pull Away this will transition from Begin Lockup to End Lockup in quick succession
      To Lockup, steady the blade after Clash
      To end Lockup do Pull Away
  Drag / Melt = controlled by gesture
    Stab (thrust with impact at tip of blade)
      If pointing down Drag will initiate
      To end Drag pull blade up from floor at an angle
      If parallel or up Melt will initiate
      To end Melt pull blade away from object at an angle
  Blast Effect = Click PWR
    After Blast, swing within 2 seconds to enter Multi-Blast Mode
  Lightning Block = Double Click and Hold PWR
  Force Push* = Push Saber
    *requires FETT263_FORCE_PUSH

Rehearsal / Choreography Modes*
  *requires FETT263_SAVE_CHOREOGRAPHY define
  Begin Rehearsal** = While Off, Triple Click and Hold PWR
      **If a Saved Rehearsal Exists it will prompt you to "Replace?"
        To confirm Turn the hilt Right (Clockwise) to "Accept" and Click PWR to begin a new Rehearsal
        To keep saved rehearsal Click AUX and Rehearsal Mode will be cancelled.
    Saber will Ignite in Rehearsal Mode
    In Rehearsal Mode, standard Clash and Lockup controls are used to record sequence
  Clash = Clash
  Hold PWR + Clash = Lockup
    Rehearsal will also record the sound files used for each effect to repeat in Choreography
  Cancel Rehearsal Mode = Triple Click and Hold PWR
  Save Rehearsal = Hold PWR
  Begin Choreography = While Off, Hold PWR + Swing
    During Choreography Mode Clashes, Lockups and sound files are replayed in sequence
    When recorded sequence completes the saber goes into Battle Mode automatically
    If no saved rehearsal is available for font saber will ignite in Battle Mode*
    *may vary by define
  End Choreography = Hold PWR

Edit Mode*
    *requires FETT263_EDIT_MODE_MENU & ENABLE_ALL_EDIT_OPTIONS defines
    *requires /common folder with all menu prompt sounds
  Enter Edit Mode = While Off, Double Click and Hold PWR
    If menu prompt wav files are missing from preset you will get "Error in Font Directory" warning, refer to Edit Mode setup and requirements

  While in Edit Mode controls are as follows:
    Rotate Forward, Increase Value, Confirm "Yes" = Turn Right (Stepped)
      Increment by 5 (Fonts, Tracks, Blade Length) = Hold PWR + Turn Right
      Increment by 500 (Ignition Time, Ignition Delay, Retraction Time, Retraction Delay) = Hold PWR + Turn Right
      Increment by 5000 (Ignition Option2, Retraction Option2) = Hold PWR + Turn Right
    Rotate Back, Decrease Value, Confirm "No" = Turn Left (Stepped)
      Increment by 5 (Fonts, Tracks, Blade Length) = Hold PWR + Turn Left
      Increment by 500 (Ignition Time, Ignition Delay, Retraction Time, Retraction Delay) = Hold PWR + Turn Left
      Increment by 5000 (Ignition Option2, Retraction Option2) = Hold PWR + Turn Left   
    Select, Save, Enter = Click PWR
    Cancel, Revert, Go Back = Long Click PWR
    Go to Main Menu (from sub-menu) - Hold PWR
    Exit Edit Mode - Hold PWR (or rotate to "Exit") while in Main Menu

  "Edit Color" Additional Control
    "Color List" and "Adjust Color Hue" Zoom Mode = Double Click and Hold PWR while turning to Zoom color in, release to save

Edit Settings* (Settings Only version of Edit Mode)
    *requires FETT263_EDIT_SETTINGS_MENU & ENABLE_ALL_EDIT_OPTIONS defines
    *requires /common folder with all menu prompt sounds
  Enter Edit Mode = While Off, Double Click and Hold PWR
    If menu prompt wav files are missing from preset you will get "Error in Font Directory" warning, refer to Edit Mode setup and requirements

  While in Edit Mode controls are as follows:
    Rotate Forward, Increase Value, Confirm "Yes" = Turn Right (Stepped)
      Increment by 5 (Blade Length) = Hold PWR + Turn Right
    Rotate Back, Decrease Value, Confirm "No" = Turn Left (Stepped)
      Increment by 5 (Blade Length) = Hold PWR + Turn Left    
    Select, Save, Enter = Click PWR
    Cancel, Revert, Go Back = Long Click PWR
    Exit Edit Settings - Hold PWR

---------- || ----------

OPTIONAL DEFINES (added to CONFIG_TOP in config.h file)

  FETT263_EDIT_MODE_MENU
  Enable Edit Mode Menu System
  Requires ENABLE_ALL_EDIT_OPTIONS

  FETT263_EDIT_SETTINGS_MENU
  Enable Edit Settings Menu (Volume, Clash Threshold, Blade Length, Gestures/Controls, Brightness)
  I recommend setting USB Type = "Serial + WebUSB" under Arduino > Tools to allow for style, font, track, color editing via ProffieOS Workbench
  Cannot be combined with FETT263_EDIT_MODE_MENU
  Requires ENABLE_ALL_EDIT_OPTIONS

  FETT263_SAVE_CHOREOGRAPHY
  Enables Enhanced Battle Mode with Saved Choreography, cannot be used with FETT263_SPECIAL_ABILITIES

  FETT263_DUAL_MODE_SOUND
  Enables odd/even out.wav ignition sound selection based on blade angle
  Up = odd number sounds, Down = even numbered sounds

  FETT263_CLASH_STRENGTH_SOUND
  Enables selection of clash, stab and lockup sounds based on clash strength
  Light clash = 01.wav, Hard clash = highest number.wav

  FETT263_MAX_CLASH 16
  The value for hardest clash level to select clash sound
  Range 8 ~ 16
  
  FETT263_QUICK_SELECT_ON_BOOT
  Enables Preset Selection Menu on Boot (when power is first applied)
  Use Dial Menu to turn to desired preset, click PWR to select or hold PWR to select and ignite

  FETT263_SAY_COLOR_LIST
  Spoken Color Names replace default sounds during Color List Mode (requires .wav files)
  
  FETT263_SAY_COLOR_LIST_CC
  Spoken Color Names replace default sounds during Color Change "CC" Color List Mode (requires .wav files)

  FETT263_SAY_BATTERY_VOLTS
  Spoken Battery Level as volts during On Demand Battery Level effect (requires .wav files)
  
  FETT263_SAY_BATTERY_PERCENT
  Spoken Battery Level as percent during On Demand Battery Level effect (requires .wav files)

  == BATTLE MODE OPTIONS ==
    Battle Mode is enabled via controls by default in this prop, you can customize further with these defines

    FETT263_BATTLE_MODE_ALWAYS_ON - Battle Mode is always on, toggle controls deactivated
      This will disable traditional Clash and Stab effects (cannot be used with FETT263_BATTLE_MODE_START_ON)

    FETT263_BATTLE_MODE_START_ON - Battle Mode is active with each ignition by default but can be toggled using Aux + Swing control
      (cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON)

    FETT263_LOCKUP_DELAY 200
      This is the "delay" in millis to determine Clash vs Lockup

    FETT263_BM_CLASH_DETECT 6
      The max value to use clashes in Battle Mode 2.0, clashes used on clash strength below this value
      This allows light clashes to produce clash effects instead of using Begin/End Lockup
      (above this value Clash is performed by quick pull away using Begin/End Lockup sounds and effect)
      Range 0 ~ 8 (note 0 will use Battle Mode 1.0 with all clashes being Begin/End Lockup)

    FETT263_BM_DISABLE_OFF_BUTTON
      During Battle Mode Power Button Retraction is disabled for normal 2 button control

  == Swing On ==
    Gesture Ignition via Swing
    You can use one of the following defines to enable swing on:

      FETT263_SWING_ON - To enable Swing On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_SWING_ON_PREON - Disables Fast On ignition for Swing On so Preon is used (cannot be used with FETT263_SWING_ON)

    FETT263_SWING_ON_NO_BM - To enable Swing On Ignition control but not activate Battle Mode
        (Combine with FETT263_SWING_ON or FETT263_SWING_ON_PREON,
        cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

    FETT263_SWING_ON_SPEED 250
      Adjust Swing Speed required for Ignition 250 ~ 500 recommended

  == Twist Off ==
    Gesture Retraction via Twist (back and forth)

    FETT263_TWIST_OFF
    To enable Twist Off Retraction control

    FETT263_TWIST_OFF_NO_POSTOFF
    To enable Twist Off Retraction control, skips Postoff

  == Twist On ==
    Gesture Ignition via Twist (back and forth)
    You can use one of the following defines to enable twist on:

      FETT263_TWIST_ON - To enable Twist On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_TWIST_ON_PREON - Disables Fast On ignition for Twist On so Preon is used (cannot be used with FETT263_TWIST_ON)

    FETT263_TWIST_ON_NO_BM - To enable Twist On Ignition control but not activate Battle Mode
      (Combine with FETT263_TWIST_ON or FETT263_TWIST_ON_PREON,
      cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

  == Stab On ==
    Gesture Ignition via Stab (linear movement + clash at tip of blade)
    You can use one of the following defines to enable stab on:

      FETT263_STAB_ON - To enable Stab On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_STAB_ON_PREON - Disables Fast On ignition for Stab On so Preon is used (cannot be used with FETT263_STAB_ON)

    FETT263_STAB_ON_NO_BM - To enable Stab On Ignition control but not activate Battle Mode
    (Combine with FETT263_STAB_ON or FETT263_STAB_ON_PREON,
    cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

  == Thrust On ==
    Gesture Ignition via Thrust (linear movement)
    You can use one of the following defines to enable thrust on:

      FETT263_THRUST_ON - To enable Thrust On Ignition control (automatically enters Battle Mode, uses Fast On)

      FETT263_THRUST_ON_PREON - Disables Fast On ignition for Thrust On so Preon is used (cannot be used with FETT263_THRUST_ON)

    FETT263_THRUST_ON_NO_BM - To enable Thrust On Ignition control but not activate Battle Mode
      (Combine with FETT263_THRUST_ON or FETT263_THRUST_ON_PREON,
      cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

  == Gesture Sleep ==
  Toggle Gesture Ignition and Retraction detection to disable or enable gesture options

  FETT263_SAVE_GESTURE_OFF - Save "Gesture Sleep" setting to turn gesture ignitions and retractions off on boot

  == Force Push ==
    Push movement triggers push.wav (or force.wav if push.wav is not present)
    You can use one of the following defines to enable force push:

      FETT263_FORCE_PUSH - To enable gesture controlled Force Push during Battle Mode
        (will use push.wav or force.wav if not present)

      FETT263_FORCE_PUSH_ALWAYS_ON - To enable gesture controlled Force Push full time
        (will use push.wav or force.wav if not present)

    FETT263_FORCE_PUSH_LENGTH 5 - Allows for adjustment to Push gesture length in millis needed to trigger Force Push
      Recommended range 1 ~ 10, 1 = shortest, easiest to trigger, 10 = longest

  FETT263_SPECIAL_ABILITIES
  This enables 8 "Special Ability" controls (style controlled), 4 while ON, 4 while OFF.
  Special Abilities are controlled by the style and can vary in every preset, they are "user" defined effects/capabilities.
  Allows "Multi-Phase" to be style based, replaces FETT263_MULTI_PHASE.
  Cannot be used with FETT263_MULTI_PHASE or FETT263_SAVE_CHOREOGRAPHY

  FETT263_SPIN_MODE
  Enables toggle for "Spin" Mode* which disables all clash/stab/lockup effects to allow for spinning and flourishes.
  Cannot be used with FETT263_SAVE_CHOREOGRAPHY or FETT263_HOLD_BUTTON_LOCKUP
  *Not the same as ENABLE_SPINS

  FETT263_MULTI_PHASE
  This will enable a preset change while ON to create a "Multi-Phase" saber effect

  MOTION_TIMEOUT 60 * 15 * 1000
  This extends the motion timeout to 15 minutes to allow gesture ignition to remain active
  Increase/decrease the "15" value as needed
  
  FETT263_MOTION_WAKE_POWER_BUTTON
  Enables a click on POWER Button to Wake Up Gestures after MOTION_TIMEOUT without igniting blade.  
  Saber will play boot sound and gestures will be active.
  
  FETT263_QUOTE_PLAYER_START_ON
  This will set Force / Quote Player to play Quote by default (if in font)
  
  FETT263_RANDOMIZE_QUOTE_PLAYER
  This will set Quote Player to randomly select quote.wav instead of playing sequentially
  
  FETT263_CIRCULAR_VOLUME_MENU
  Changes Volume Menu to Circular Control

  FETT263_CIRCULAR_DIM_MENU
  Changes Brightness Menu to Circular Control
  
== Disable Features ==
  DISABLE_TALKIE - saves memory by replacing spoken error messages with beep sequences - 

  FETT263_DISABLE_CHANGE_FONT - Disables the "on-the-fly" Change Font option
  
  FETT263_DISABLE_CHANGE_STYLE - Disables the "on-the-fly" Change Style option
  
  FETT263_DISABLE_COPY_PRESET - Disables the "on-the-fly" Copy Preset option
  
  FETT263_DISABLE_BM_TOGGLE - Disable button control for Battle Mode, use gesture ignition or Special Abilities and/or style to toggle.
  
  FETT263_DISABLE_MULTI_BLAST_TOGGLE - Disable button control for Multi-Blast Mode, use Special Abilities and/or style to toggle.
  
  FETT263_DISABLE_MULTI_BLAST - Disables "Multi-Blast" Mode

  FETT263_TRACK_PLAYER_NO_PROMPTS - Disables spoken voice prompts in Track Player
  
  FETT263_DISABLE_QUOTE_PLAYER - Disables Force/Quote player, only uses Force. This will allow Quotes to be controlled by style.
    Use FETT263_SPECIAL_ABILITIES to set EFFECT_QUOTE or EFFECT_NEXT_QUOTE in style
    Cannot be used with FETT263_RANDOMIZE_QUOTE_PLAYER and FETT263_QUOTE_PLAYER_START_ON

== SA22C 2 Button Variations ==
  FETT263_HOLD_BUTTON_OFF - Changes to Hold PWR to turn Off / Retract
  
  FETT263_HOLD_BUTTON_LOCKUP - Enables Hold AUX for Lockup*
    Cannot be used with FETT263_SAVE_CHOREOGRAPHY
    *Clash Strength / Clash Impact effects and sounds for Lockup negated
    *Battle Mode control changes to hold AUX + Swing
    
== BC Variations ==
  FETT263_USE_BC_MELT_STAB
  Allows MELT to be gesture controlled full-time, uses Thrust for Stab effect

  FETT263_BC_SAY_BATTERY_VOLTS_PERCENT
  Spoken Battery Level in volts and percent (point down for volts, parallel or up for percent)

CUSTOM SOUNDS SUPPORTED (add to font to enable):

  On Demand Power Save - dim.wav
  On Demand Battery Level - battery.wav
  Battle Mode On (on toggle) - bmbegin.wav
  Battle Mode Off (on toggle) - bmend.wav
  Enter Volume Menu - vmbegin.wav
  Exit Volume Menu - vmend.wav
  Force Push - push.wav
  Fast On (optional) - faston.wav
  Multi-Blast Mode On - blstbgn.wav
  Multi-Blast Mode Off - blstend.wav
  Quotes - quote01.wav
  Transition Sound - tr.wav
  Transition Sound Loop trloop.wav
*/

#ifndef PROPS_SABER_FETT263_BUTTONS_H
#define PROPS_SABER_FETT263_BUTTONS_H

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 15 * 1000
#endif

#ifndef FETT263_SWING_ON_SPEED
#define FETT263_SWING_ON_SPEED 250
#endif

#ifndef FETT263_LOCKUP_DELAY
#define FETT263_LOCKUP_DELAY 200
#endif

#ifndef FETT263_BM_CLASH_DETECT
#define FETT263_BM_CLASH_DETECT 0
#endif

#ifndef FETT263_FORCE_PUSH_LENGTH
#define FETT263_FORCE_PUSH_LENGTH 5
#endif

#ifdef FETT263_BC_SAY_BATTERY_VOLTS_PERCENT
#define FETT263_SAY_BATTERY_VOLTS
#define FETT263_SAY_BATTERY_PERCENT
#endif

#if defined(FETT263_EDIT_MODE_MENU) || defined(FETT263_EDIT_SETTINGS_MENU)
#define FETT263_USE_SETTINGS_MENU
#endif

#if defined(FETT263_SAY_COLOR_LIST) || defined(FETT263_SAY_COLOR_LIST_CC)
#define SAY_COLOR_LIST
#endif

#if NUM_BUTTONS < 1
#error /props/saber_fett263_buttons.h requires 1, 2 or 3 Buttons for operation
#endif

#if defined(FETT263_HOLD_BUTTON_LOCKUP) && NUM_BUTTONS == 1
#error FETT263_HOLD_BUTTON_LOCKUP cannot be used with 1 Button Controls
#endif

#if defined(FETT263_HOLD_BUTTON_LOCKUP) && defined(FETT263_SAVE_CHOREOGRAPHY)
#error FETT263_HOLD_BUTTON_LOCKUP cannot be used with FETT263_SAVE_CHOREOGRAPHY
#endif

#if defined(FETT263_EDIT_MODE_MENU) && !defined(ENABLE_ALL_EDIT_OPTIONS)
#error ENABLE_ALL_EDIT_OPTIONS must be defined to enable FETT263_EDIT_MODE_MENU
#endif

#if defined(FETT263_EDIT_SETTINGS_MENU) && !defined(ENABLE_ALL_EDIT_OPTIONS)
#error ENABLE_ALL_EDIT_OPTIONS must be defined to enable FETT263_EDIT_SETTINGS_MENU
#endif

#if defined(FETT263_EDIT_MODE_MENU) && defined(FETT263_EDIT_SETTINGS_MENU)
#error FETT263_EDIT_SETTINGS_MENU cannot be combined with FETT263_EDIT_MODE_MENU
#endif

#if defined(FETT263_DISABLE_QUOTE_PLAYER) && defined(FETT263_QUOTE_PLAYER_START_ON)
#error FETT263_QUOTE_PLAYER_START_ON cannot be used with FETT263_DISABLE_QUOTE_PLAYER
#endif

#if defined(FETT263_SPECIAL_ABILITIES) && defined(FETT263_MULTI_PHASE)
#error FETT263_SPECIAL_ABILITIES cannot be used with FETT263_MULTI_PHASE
#endif

#if defined(FETT263_SPECIAL_ABILITIES) && defined(FETT263_SAVE_CHOREOGRAPHY)
#error FETT263_SPECIAL_ABILITIES cannot be used with FETT263_SAVE_CHOREOGRAPHY
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_BATTLE_MODE_START_ON)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_BATTLE_MODE_START_ON
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_SWING_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_SWING_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_TWIST_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_TWIST_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_ALWAYS_ON) && defined(FETT263_STAB_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_ALWAYS_ON and FETT263_STAB_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_SWING_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_SWING_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_TWIST_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_TWIST_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_STAB_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_STAB_ON_NO_BM
#endif

#if defined(FETT263_BATTLE_MODE_START_ON) && defined(FETT263_THRUST_ON_NO_BM)
#error You cannot define both FETT263_BATTLE_MODE_START_ON and FETT263_STAB_ON_NO_BM
#endif

#if defined(FETT263_SPIN_MODE) && defined(FETT263_SAVE_CHOREOGRAPHY)
#error FETT263_SPIN_MODE cannot be used with FETT263_SAVE_CHOREOGRAPHY
#endif

#if defined(FETT263_SPIN_MODE) && defined(FETT263_HOLD_BUTTON_LOCKUP)
#error FETT263_SPIN_MODE cannot be used with FETT263_HOLD_BUTTON_LOCKUP
#endif

#if defined(FETT263_SWING_ON) && defined(FETT263_SWING_ON_PREON)
#error You cannot define both FETT263_SWING_ON and FETT263_SWING_ON_PREON
#endif

#if defined(FETT263_TWIST_ON) && defined(FETT263_TWIST_PREON)
#error You cannot define both FETT263_TWIST_ON and FETT263_TWIST_ON_PREON
#endif

#if defined(FETT263_STAB_ON) && defined(FETT263_STAB_PREON)
#error You cannot define both FETT263_STAB_ON and FETT263_STAB_ON_PREON
#endif

#if defined(FETT263_TWIST_OFF) && defined(FETT263_TWIST_OFF_NO_POSTOFF)
#error You cannot define both FETT263_TWIST_OFF and FETT263_TWIST_OFF_NO_POSTOFF
#endif

#if defined(FETT263_FORCE_PUSH_ALWAYS_ON) && defined(FETT263_FORCE_PUSH)
#error You cannot define both FETT263_FORCE_PUSH_ALWAYS_ON and FETT263_FORCE_PUSH
#endif

#if defined(FETT263_CLASH_STRENGTH_SOUND) && !defined(FETT263_MAX_CLASH)
#define FETT263_MAX_CLASH 16
#endif

#ifdef FETT263_TWIST_OFF
#define TWIST_OFF_GESTURE
#endif

#ifdef FETT263_TWIST_OFF_NO_POSTOFF
#define TWIST_OFF_GESTURE
#endif

#ifdef FETT263_SWING_ON
#define SWING_GESTURE
#endif

#ifdef FETT263_SWING_ON_PREON
#define SWING_GESTURE
#endif

#if defined(FETT263_SWING_ON_NO_BM) && !defined(SWING_GESTURE)
#error FETT263_SWING_ON_NO_BM requires either FETT263_SWING_ON or FETT263_SWING_ON_PREON
#endif

#ifdef FETT263_STAB_ON
#define STAB_GESTURE
#endif

#ifdef FETT263_STAB_ON_PREON
#define STAB_GESTURE
#endif

#if defined(FETT263_STAB_ON_NO_BM) && !defined(STAB_GESTURE)
#error FETT263_STAB_ON_NO_BM requires either FETT263_STAB_ON or FETT263_STAB_ON_PREON
#endif

#ifdef FETT263_TWIST_ON
#define TWIST_GESTURE
#endif

#ifdef FETT263_TWIST_ON_PREON
#define TWIST_GESTURE
#endif

#if defined(FETT263_TWIST_ON_NO_BM) && !defined(TWIST_GESTURE)
#error FETT263_TWIST_ON_NO_BM requires either FETT263_TWIST_ON or FETT263_TWIST_ON_PREON
#endif

#ifdef FETT263_THRUST_ON
#define THRUST_GESTURE
#endif

#ifdef FETT263_THRUST_ON_PREON
#define THRUST_GESTURE
#endif

#if defined(FETT263_THRUST_ON_NO_BM) && !defined(THRUST_GESTURE)
#error FETT263_THRUST_ON_NO_BM requires either FETT263_THRUST_ON or FETT263_THRUST_ON_PREON
#endif

#ifdef FETT263_FORCE_PUSH_ALWAYS_ON
#define FORCE_PUSH_CONDITION true
#define FETT263_FORCE_PUSH
#else
#define FORCE_PUSH_CONDITION battle_mode_
#endif

#if NUM_BUTTONS == 1
/* Tightened click timings
SA22C shortened the timeout for short and double click detection from 500ms
to 300ms.  I think it feels more responsive this way but still gives enough
timeout to ensure all button actions can be achieved consistently
SA22C included all button timings so they can be easily tweaked to suit
individual tastes.
*/

#ifndef BUTTON_DOUBLE_CLICK_TIMEOUT
#define BUTTON_DOUBLE_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_SHORT_CLICK_TIMEOUT
#define BUTTON_SHORT_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_TIMEOUT
#define BUTTON_HELD_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_MEDIUM_TIMEOUT
#define BUTTON_HELD_MEDIUM_TIMEOUT 1000
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif

#endif

#include "prop_base.h"
#include "../sound/hybrid_font.h"
#include "../sound/effect.h"
#include "../common/current_preset.h"
#include "../common/file_reader.h"
#include "../common/malloc_helper.h"

#ifdef FETT263_EDIT_MODE_MENU
#include "../common/color.h"
#include "../styles/edit_mode.h"
#endif

#undef PROP_TYPE
#define PROP_TYPE SaberFett263Buttons

EFFECT(dim); // for EFFECT_POWERSAVE
EFFECT(battery); // for EFFECT_BATTERY_LEVEL
EFFECT(bmbegin); // for Begin Battle Mode
EFFECT(bmend); // for End Battle Mode
EFFECT(vmbegin); // for Begin Volume Menu
EFFECT(vmend); // for End Volume Menu
EFFECT(faston); // for EFFECT_FAST_ON
EFFECT(blstbgn); // for Begin Multi-Blast
EFFECT(blstend); // for End Multi-Blast
EFFECT(push); // for Force Push gesture in Battle Mode
EFFECT(quote); // quote on force effect
EFFECT(tr);
EFFECT2(trloop, trloop);
#ifdef FETT263_USE_SETTINGS_MENU
EFFECT(medit); // Edit Mode
#endif

#include "../sound/sound_library.h"

class GestureControlFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(gestureon, 1);
#ifdef SWING_GESTURE
    CONFIG_VARIABLE2(swingon, 1);
#else
    CONFIG_VARIABLE2(swingon, 0);
#endif
    CONFIG_VARIABLE2(swingonspeed, FETT263_SWING_ON_SPEED);
#ifdef TWIST_GESTURE
    CONFIG_VARIABLE2(twiston, 1);
#else
    CONFIG_VARIABLE2(twiston, 0);
#endif
#ifdef THRUST_GESTURE
    CONFIG_VARIABLE2(thruston, 1);
#else
    CONFIG_VARIABLE2(thruston, 0);
#endif
#ifdef STAB_GESTURE
    CONFIG_VARIABLE2(stabon, 1);
#else
    CONFIG_VARIABLE2(stabon, 0);
#endif
#ifdef TWIST_OFF_GESTURE
    CONFIG_VARIABLE2(twistoff, 1);
#else
    CONFIG_VARIABLE2(twistoff, 0);
#endif
#ifdef FETT263_POWER_LOCK
    CONFIG_VARIABLE2(powerlock, 1);
#else
    CONFIG_VARIABLE2(powerlock, 0);
#endif
#ifdef FORCE_PUSH_CONDITION
    CONFIG_VARIABLE2(forcepush, 1);
#else
    CONFIG_VARIABLE2(forcepush, 0);
#endif
    CONFIG_VARIABLE2(forcepushlen, FETT263_FORCE_PUSH_LENGTH);
    CONFIG_VARIABLE2(lockupdelay, FETT263_LOCKUP_DELAY);
    CONFIG_VARIABLE2(clashdetect, FETT263_BM_CLASH_DETECT);
#ifdef FETT263_MAX_CLASH
    CONFIG_VARIABLE2(maxclash, FETT263_MAX_CLASH);
#else
    CONFIG_VARIABLE2(maxclash, 16);
#endif
}
  bool gestureon; // gesture controls on/off for use with "Gesture Sleep"
  bool swingon; // Swing On ignition
  int swingonspeed; // Swing On speed (200 ~ 500 range)
  bool twiston; // Twist On ignition
  bool thruston; // Thrust On ignition
  bool stabon; // Stab On ignition
  bool forcepush; // Force Push
  uint32_t forcepushlen; // Force Push Length
  uint32_t lockupdelay; // Lockup Delay (for Battle Mode)
  bool twistoff; // Twist Off retraction
  // disable PWR button for retraction, for use with "Power Lock" mode
  // to prevent button turning saber off
  bool powerlock;
  int clashdetect; // maximum Clash Strength to detect Clash during Battle Mode (0 ~ 10 range)
  int maxclash; // maximum Clash Strength for Clash Sound and Detection works with CLASH_THRESHOLD_G to create range of Clash Strength (8 ~ 16 range)
};

#ifdef FETT263_SAVE_CHOREOGRAPHY
// Rehearsal / Choreography
class SavedRehearsal : public ConfigFile {
public:
  virtual ReadStatus Read(FileReader* f, bool reset = false) override {
    int file;
    if (!f || !f->IsOpen()) return ReadStatus::READ_FAIL;
    memset(clash_rec, 0, sizeof(clash_rec));
    STDOUT.println("Reading choreo.ini");
    for (size_t i = 0; i < NELEM(clash_rec); i++) {
      char action[33];
      f->skipwhite();
      f->readVariable(action);
      if (!strcmp(action, "clash")) {
        if (SFX_clsh) {
           file = SFX_clsh.get_min_file();
        } else {
           file = SFX_clash.get_min_file();
        }
        clash_rec[i].stance = STANCE_CLASH;
        f->skipspace();
        clash_rec[i].sound_number1 = (int)(f->readFloatValue() - file);
        f->skipspace();
        continue;
      }
      if (!strcmp(action, "lockup")) {
        if (SFX_lock) {
          file = SFX_lock.get_min_file();
        } else {
          file = SFX_lockup.get_min_file();
        }
        clash_rec[i].stance = STANCE_LOCKUP;
        f->skipspace();
        clash_rec[i].sound_number1 = (int)(f->readFloatValue() - SFX_bgnlock.get_min_file());
        f->skipspace();
        clash_rec[i].sound_number2 = (int)(f->readFloatValue() - file);
        f->skipspace();
        clash_rec[i].sound_number3 = (int)(f->readFloatValue() - SFX_endlock.get_min_file());
        f->skipspace();
        continue;
      }
      clash_rec[i].stance = STANCE_END;
      return ReadStatus:: READ_END;
      break;
    }
    return ReadStatus:: READ_OK;
  }

  void SetVariable(const char* variable, float v) override {
  }

  bool isIntialized() {
    if (clash_rec[0].stance == STANCE_CLASH || clash_rec[0].stance == STANCE_LOCKUP) return true;
      return false;
  }

  enum FormStance {
    STANCE_END = 0,
    STANCE_CLASH,
    STANCE_LOCKUP,
  };

  struct FormEntry {
    FormStance stance;
    int sound_number1;
    int sound_number2;
    int sound_number3;
  };

  FormEntry clash_rec[32];
};
#endif

#ifdef FETT263_USE_SETTINGS_MENU

// Edit Length
int length_edit_length = 0;

// Edit Length Preview for Edit Mode
// LengthEdit uses blade color and creates single white pixel at last pixel, black above last pixel
template<class BASE, class LIGHTUP, class BLACK = BLACK>
class LengthEdit {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    lightup_.run(blade);
    black_.run(blade);
  }

  OverDriveColor getColor(int led) {
    if (led == length_edit_length) return lightup_.getColor(led);
    if (led > length_edit_length) return black_.getColor(led);
    return base_.getColor(led);
  }
private:
  BASE base_;
  LIGHTUP lightup_;
  BLACK black_;
};
#endif
#ifdef FETT263_EDIT_MODE_MENU
// Edit Style Settings

class IntEdit {
  public:
    void run(BladeBase* blade) {  }
    int getInteger(int led) { return int_edit_; }
    static void SetIntValue(int value) { int_edit_ = value; }
 private:
   static int int_edit_;
};

int IntEdit::int_edit_ = 0;
#endif

// Color List
struct ColorListEntry { Color16 color; ColorNumber color_number; };

static constexpr ColorListEntry color_list_[] = {
  { Red::color(), COLOR_RED },
  { OrangeRed::color(), COLOR_ORANGERED },
  { DarkOrange::color(), COLOR_DARKORANGE },
  { Orange::color(), COLOR_ORANGE },
  { { 46260, 33410, 0 }, COLOR_GOLD },
  { Yellow::color(), COLOR_YELLOW },
  { GreenYellow::color(), COLOR_GREENYELLOW },
  { Green::color(), COLOR_GREEN },
  { Aquamarine::color(), COLOR_AQUAMARINE },
  { Cyan::color(), COLOR_CYAN },
  { DeepSkyBlue::color(), COLOR_DEEPSKYBLUE },
  { DodgerBlue::color(), COLOR_DODGERBLUE },
  { Blue::color(), COLOR_BLUE },
  { { 7710, 15420, 51400 }, COLOR_ICEBLUE },
  { { 11102, 92, 53864 }, COLOR_INDIGO },
  { { 24000, 0, 50536 }, COLOR_PURPLE },
  { { 30324, 0, 49768 }, COLOR_DEEPPURPLE },
  { Magenta::color(), COLOR_MAGENTA },
  { DeepPink::color(), COLOR_DEEPPINK },
  { { 25700, 25700, 38550 }, COLOR_SILVER },
  { { 21845, 21845, 51400 }, COLOR_GLACIER },
  { { 46260, 46260, 65535 }, COLOR_ICEWHITE },
  { LightCyan::color(), COLOR_LIGHTCYAN },
  { Moccasin::color(), COLOR_MOCCASIN },
  { LemonChiffon::color(), COLOR_LEMONCHIFFON },
  { NavajoWhite::color(), COLOR_NAVAJOWHITE },
  { White::color(), COLOR_WHITE }
};

// Edit Color Submenu
const int rgb_arg_menu_[] {
  BASE_COLOR_ARG,
  ALT_COLOR_ARG,
  ALT_COLOR2_ARG,
  ALT_COLOR3_ARG,
  BLAST_COLOR_ARG,
  CLASH_COLOR_ARG,
  LOCKUP_COLOR_ARG,
  DRAG_COLOR_ARG,
  LB_COLOR_ARG,
  STAB_COLOR_ARG,
  PREON_COLOR_ARG,
  IGNITION_COLOR_ARG,
  RETRACTION_COLOR_ARG,
  POSTOFF_COLOR_ARG,
  SWING_COLOR_ARG,
  EMITTER_COLOR_ARG,
  OFF_COLOR_ARG
};

// Edit Style Setting Submenu
const int int_arg_menu_[] {
  STYLE_OPTION_ARG,
  STYLE_OPTION2_ARG,
  STYLE_OPTION3_ARG,
  IGNITION_OPTION_ARG,
  IGNITION_OPTION2_ARG,
  IGNITION_TIME_ARG,
  IGNITION_DELAY_ARG,
  IGNITION_POWER_UP_ARG,
  RETRACTION_OPTION_ARG,
  RETRACTION_OPTION2_ARG,
  RETRACTION_TIME_ARG,
  RETRACTION_DELAY_ARG,
  RETRACTION_COOL_DOWN_ARG,
  LOCKUP_POSITION_ARG,
  DRAG_SIZE_ARG,
  MELT_SIZE_ARG,
  SWING_OPTION_ARG,
  EMITTER_SIZE_ARG,
  OFF_OPTION_ARG,
  // preon always needs to be last options to allow check for SFX_preon in menu
  PREON_OPTION_ARG,
  PREON_SIZE_ARG
};

// The Saber class implements the basic states and actions
// for the saber.
class SaberFett263Buttons : public PROP_INHERIT_PREFIX PropBase {
public:
SaberFett263Buttons() : PropBase() {}
  const char* name() override { return "SaberFett263Buttons"; }

  GestureControlFile saved_gesture_control;
#ifdef FETT263_SAVE_CHOREOGRAPHY
  SavedRehearsal saved_choreography;
#endif

  void RestoreGestureState() {
    saved_gesture_control.ReadINIFromDir(NULL, "gesture");
  }

  void SaveGestureState() {
    STDOUT.println("Saving Gesture State");
    saved_gesture_control.WriteToRootDir("gesture");
  }

#ifdef FETT263_SAVE_CHOREOGRAPHY
  void RestoreChoreo() {
    if (saved_choreography.isIntialized()) memset(saved_choreography.clash_rec, 0, sizeof(saved_choreography.clash_rec));
    saved_choreography.ReadInCurrentDir("choreo.ini");
  }

  void WriteChoreo(const char* filename) {
    const char* dir = nullptr;
    int file;
    if (SFX_clsh) {
      dir = SFX_clsh.get_directory();
    } else {
      dir = SFX_clash.get_directory();
    }
    if (dir == nullptr) dir = current_directory;
    PathHelper full_name(dir, filename);
    LOCK_SD(true);
    FileReader out;
    LSFS::Remove(full_name);
    out.Create(full_name);
    for (size_t i = 0; i < NELEM(saved_choreography.clash_rec); i++) {
      char value[64];
      switch (saved_choreography.clash_rec[i].stance) {
        case SavedRehearsal::STANCE_CLASH:
          if (SFX_clsh) {
             file = SFX_clsh.get_min_file();
          } else {
             file = SFX_clash.get_min_file();
          }
          strcpy(value, "clash ");
          itoa(saved_choreography.clash_rec[i].sound_number1 + file, value + strlen(value), 10);
          strcat(value, "\n");
          out.Write(value);
          break;
        case SavedRehearsal::STANCE_LOCKUP:
          if (SFX_lock) {
            file = SFX_lock.get_min_file();
          } else {
            file = SFX_lockup.get_min_file();
          }
          strcpy(value, "lockup ");
          itoa(saved_choreography.clash_rec[i].sound_number1 + SFX_bgnlock.get_min_file(), value + strlen(value), 10);
          strcat(value, " ");
          itoa(saved_choreography.clash_rec[i].sound_number2 + file, value + strlen(value), 10);
          strcat(value, " ");
          itoa(saved_choreography.clash_rec[i].sound_number3 + SFX_endlock.get_min_file(), value + strlen(value), 10);
          strcat(value, "\n");
          out.Write(value);
          break;
        case SavedRehearsal::STANCE_END:
        default:
          break;
      }
    }
    out.Write("end\n");
    out.Close();
    LOCK_SD(false);
  }

  void SaveChoreo() {
    STDOUT.println("Saving Choreography");
    WriteChoreo("choreo.tmp");
    WriteChoreo("choreo.ini");
  }

  void BeginRehearsal() {
    memset(saved_choreography.clash_rec, 0, sizeof(saved_choreography.clash_rec));
    clash_count_ = -1;
    rehearse_ = true;
    sound_library_.SayRehearseBegin();
    FastOn();
  }

  void RehearseLockup() {
    clash_count_ += 1;
    saved_choreography.clash_rec[clash_count_].stance = SavedRehearsal::STANCE_LOCKUP;
    int file_num1;
    int file_num2;
    int file_num3;
    file_num1 = rand() % SFX_bgnlock.files_found();
    SFX_bgnlock.Select(file_num1);
    saved_choreography.clash_rec[clash_count_].sound_number1 = file_num1;
    if (SFX_lock) {
      file_num2 = rand() % SFX_lock.files_found();
      SFX_lock.Select(file_num2);
      saved_choreography.clash_rec[clash_count_].sound_number2 = file_num2;
    } else {
      file_num2 = rand() % SFX_lockup.files_found();
      SFX_lockup.Select(file_num2);
      saved_choreography.clash_rec[clash_count_].sound_number2 = file_num2;
    }
    file_num3 = rand() % SFX_endlock.files_found();
    SFX_endlock.Select(file_num3);
    saved_choreography.clash_rec[clash_count_].sound_number3 = file_num3;
  }

  void RehearseClash() {
    clash_count_ += 1;
    saved_choreography.clash_rec[clash_count_].stance = SavedRehearsal::STANCE_CLASH;
    int file_num1;
    if (SFX_clsh) {
      file_num1 = rand() % SFX_clsh.files_found();
      SFX_clsh.Select(file_num1);
      saved_choreography.clash_rec[clash_count_].sound_number1 = file_num1;
    } else {
      file_num1 = rand() % SFX_clash.files_found();
      SFX_clash.Select(file_num1);
      saved_choreography.clash_rec[clash_count_].sound_number1 = file_num1;
    }
    SaberBase::DoClash();
  }

  void EndRehearsal() {
    sound_library_.SayRehearseEnd();
    clash_count_ += 1;
    saved_choreography.clash_rec[clash_count_].stance = SavedRehearsal::STANCE_END;
    next_event_ = true;
    clash_count_ = 0;
    if (SFX_clsh) {
      SFX_clsh.Select(-1);
    } else {
      SFX_clash.Select(-1);
    }
    if (SFX_lock) {
      SFX_lock.Select(-1);
    } else {
      SFX_lockup.Select(-1);
    }
    SFX_bgnlock.Select(-1);
    SFX_endlock.Select(-1);
    wav_player.Free();
  }

  void BeginChoreo() {
    if (saved_choreography.isIntialized()) {
      sound_library_.SayChoreographyBegin();
      choreo_ = true;
      clash_count_ = 0;
    } else {
      sound_library_.SayNoChoreographyAvailable();
    }
#ifdef FETT263_DUAL_MODE_SOUND
    SelectIgnitionSound();
#endif
    battle_mode_ = true;
    FastOn();
  }

   void ChoreoClash() {
     if (SFX_clsh) {
       SFX_clsh.Select(saved_choreography.clash_rec[clash_count_].sound_number1);
     } else {
       SFX_clash.Select(saved_choreography.clash_rec[clash_count_].sound_number1);
     }
     check_blast_ = false;
     swing_blast_ = false;
     clash_count_ += 1;
     SaberBase::DoClash();
   }

   void ChoreoLockup() {
     SFX_bgnlock.Select(saved_choreography.clash_rec[clash_count_].sound_number1);
     if (SFX_lock) {
       SFX_lock.Select(saved_choreography.clash_rec[clash_count_].sound_number2);
     } else {
       SFX_lockup.Select(saved_choreography.clash_rec[clash_count_].sound_number2);
     }
     SFX_endlock.Select(saved_choreography.clash_rec[clash_count_].sound_number3);
     check_blast_ = false;
     swing_blast_ = false;
     clash_count_ += 1;
     SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
     SaberBase::DoBeginLockup();
     auto_lockup_on_ = true;
   }

  void EndChoreo() {
    choreo_ = false;
    sound_library_.SayChoreographyEnd();
    if (SFX_clsh) {
      SFX_clsh.Select(-1);
    } else {
      SFX_clash.Select(-1);
    }
    if (SFX_lock) {
      SFX_lock.Select(-1);
    } else {
      SFX_lockup.Select(-1);
    }
    SFX_bgnlock.Select(-1);
    SFX_endlock.Select(-1);
  }
#endif

  void GenerateIniFiles() {
    SaveState(current_preset_.preset_num);
    SaveGestureState();
  }

  Color16 GetColorArg(int blade, int arg) {
    char argspace[32];
    if (style_parser.GetArgument(current_preset_.GetStyle(blade), arg + 2, argspace)) {
      char* tmp;
      int r = strtol(argspace, &tmp, 0);
      int g = strtol(tmp+1, &tmp, 0);
      int b = strtol(tmp+1, NULL, 0);
      return Color16(r,g,b);
    }
    return Color16(65535,0,0);
  }
	
  // Color / Style Editing
  HSL hsl_;
  ShowColorAllBladesTemplate<ShowColorStyle> show_color_all_;
#ifdef FETT263_USE_SETTINGS_MENU
  ShowColorSingleBladeTemplate<LengthEdit<ShowColorStyle,Pulsing<White,Rgb<100,100,100>,800>>,LengthEdit<ShowColorStyle,Pulsing<White,Rgb<100,100,100>,800>>> show_length_;
#if NUM_BLADES > 1
  ShowColorSingleBladeTemplate<Pulsing<ShowColorStyle,Black,800>,Pulsing<ShowColorStyle,Black,800>> show_preview_;

    void BladePreview(int blade) {
    if (blade_preview_ > 0) {
      show_preview_.Stop(blade_preview_);
    }
    blade_preview_ = blade;
    show_preview_.Start(blade);
    ShowColorStyle::SetColor(GetColorArg(blade_num_, 1));
  }
#endif

  void LengthPreview(int blade) {
    show_length_.Start(blade);
    length_edit_length = blade_length_ - 1;
    ShowColorStyle::SetColor(GetColorArg(blade_num_, 1));
  }
#endif	

#ifdef FETT263_EDIT_MODE_MENU
  ShowColorSingleBladeTemplate<ShowColorStyle> show_color_;
  ShowColorSingleBladeTemplate<Mix<Bump<Int<16384>,Int<14000>>,RgbArg<1,Rgb<255,0,0>>,ShowColorStyle>> bump_color_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<Int<26000>,Int<8000>>,RgbArg<1,Rgb<255,0,0>>,ShowColorStyle>> tip_color_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<Int<7000>,Int<-6000>>,RgbArg<1,Rgb<255,0,0>>,ShowColorStyle>> hilt_color_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<Int<7000>,Int<-6000>>,Black,ShowColorStyle>> pre_color_;
  ShowColorSingleBladeTemplate<Mix<Bump<IntEdit,Int<10000>>,Black,ShowColorStyle>> show_lockup_position_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<IntEdit,Int<6000>>,Black,ShowColorStyle>> show_drag_size_;
  ShowColorSingleBladeTemplate<Mix<SmoothStep<IntEdit,Int<-6000>>,Black,ShowColorStyle>> show_emitter_size_;

  // Show Full Blade Preview for Color Editing
  void ShowFull() {
    show_color_.Start(blade_num_);
    ShowColorStyle::SetColor(GetColorArg(blade_num_, effect_num_));
    saved_color_ = GetColorArg(blade_num_, effect_num_);
    hsl_ = saved_color_.toHSL();
    hsl_angle_ = fusor.angle2();
  }

  // Show Partial Blade Preview (Clash, Lockup, Drag, Melt, Preon, Emitter) for Color Editing
  void ShowPart() {
    ShowColorStyle::SetColor(GetColorArg(blade_num_, effect_num_));
    saved_color_ = GetColorArg(blade_num_, effect_num_);
    hsl_ = saved_color_.toHSL();
    hsl_angle_ = fusor.angle2();
  }

  // Copy Color Arguments from one blade to another
  void CopyColors() {
    effect_num_ = 16;
    while (true) {
      effect_num_ -= 1;
      if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), effect_num_ + 2)) break;
    }
    saved_color_ = GetColorArg(blade_num_, effect_num_);
    hsl_ = saved_color_.toHSL();
    NewColor(blade_num_, effect_num_);
    current_preset_.Save();
    effect_num_ = 0;
  }

  // Set In/Out (Ignition/Retraction) Arguments
  void SetInOut() {
    char style_arg[10];
    // Reminder - use search and replace to rename "calc_" per PR comments
    itoa(calc_, style_arg, 10);
    current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, style_arg));
    current_preset_.Save();
    switch (menu_type_) {
      case MENU_STYLE_OPTION:
      case MENU_STYLE_OPTION2:
      case MENU_STYLE_OPTION3:
      case MENU_IGNITION_TIME:
      case MENU_IGNITION_OPTION:
      case MENU_IGNITION_OPTION2:
      case MENU_IGNITION_POWER_UP_OPTION:
      case MENU_IGNITION_DELAY:
        SetPresetFast(current_preset_.preset_num);
        break;
      case MENU_PREON_OPTION:
      case MENU_PREON_SIZE:
        UpdatePreon();
        break;
      case MENU_RETRACTION_TIME:
      case MENU_RETRACTION_OPTION:
      case MENU_RETRACTION_OPTION2:
      case MENU_RETRACTION_COOL_DOWN_OPTION:
      case MENU_RETRACTION_DELAY:
        UpdateStyle();
        off_event_ = true;
        restart_millis_ = millis();
        break;
      default:
        break;
    }
  }

  void RevertInOut() {
    char style_arg[10];
    itoa(arg_revert_, style_arg, 10);
    current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, style_arg));
    switch (menu_type_) {
      case MENU_RETRACTION_OPTION:
      case MENU_RETRACTION_TIME:
      case MENU_RETRACTION_COOL_DOWN_OPTION:
      case MENU_RETRACTION_DELAY:
        char ig[10];
        itoa(ignite_time_, ig, 10);
        current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, ig));
        break;
      default:
        break;
    }
    current_preset_.Save();
    SetPresetFast(current_preset_.preset_num);
  }

    
  void SayStyleNumber(int style_num) {
    sound_library_.SayStyle();
    sound_library_.SayNumber(style_num, SAY_WHOLE);
    if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), STYLE_OPTION_ARG + 2)) {
      char argspace[32];
      style_parser.GetArgument(current_preset_.GetStyle(blade_num_), STYLE_OPTION_ARG + 2, argspace);
      int opt = strtol(argspace, NULL, 0);
      sound_library_.SayOption();
      sound_library_.SayNumber(opt, SAY_WHOLE);
    }
  }
	
  // Stop location/size previews in Style Settings Mode
  void StopSettingPreview() {
    switch (set_num_) {
      case LOCKUP_POSITION_ARG:
        show_lockup_position_.Stop(blade_num_);
        break;
      case DRAG_SIZE_ARG:
      case MELT_SIZE_ARG:
        show_drag_size_.Stop(blade_num_);
        break;
      case EMITTER_SIZE_ARG:
      case PREON_SIZE_ARG:
        show_emitter_size_.Stop(blade_num_);
        break;
      default:
        break;
    }
    UpdateStyle();
    menu_type_ = MENU_STYLE_SETTING_SUB;
  }

  // Save IntArg values 16 ~ 25 from Edit Mode Menu selection
  void SaveStyleSetting() {
    char style_arg[10];
    itoa(calc_, style_arg, 10);
    current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, style_arg));
    current_preset_.Save();
    StopSettingPreview();
    MenuSave();
  }

  void CancelStyleSetting() {
    StopSettingPreview();
    MenuCancel();
  }
#endif

// Edit Color
#define H_CHANGE (M_PI / 98304)
#define L_ANGLE (M_PI / 30)
#define H_ANGLE (M_PI / 16384)
#define EDIT_MODE_ZOOM (M_PI * 2 / 2000)
  void EditColor() {
    if (edit_color_ && color_mode_ != COLOR_LIST) {
      float a = fusor.angle2() - hsl_angle_;
      if (a > M_PI) a-=M_PI*2;
      if (a < -M_PI) a+=M_PI*2;
      float angle = 100;
      switch (color_mode_) {
        case EDIT_COLOR: angle = H_ANGLE; break;
        case ZOOM_COLOR:
        case CC_ZOOM_COLOR:
          angle = EDIT_MODE_ZOOM;
          break;
        case EDIT_WHITE:
        case EDIT_BLACK: angle = L_ANGLE; break;
        default: break;
      }
      int steps = 0;
      if (a > angle * 2/3) {
        hsl_angle_ += angle;
        if (hsl_angle_ > M_PI) hsl_angle_ -= M_PI*2;
        steps++;
      }
      if (a < -angle * 2/3) {
        hsl_angle_ -= angle;
        if (hsl_angle_ < M_PI) hsl_angle_ += M_PI*2;
        steps--;
      }
      switch (color_mode_) {
        default: break;
        case EDIT_COLOR:
        case ZOOM_COLOR:
        case CC_ZOOM_COLOR:
          hsl_.H = fract(hsl_.H - H_CHANGE * steps);
          break;
        case EDIT_WHITE:
          if (steps > 0) {
            if (hsl_.L < 1.0) {
              hsl_.L = clamp(hsl_.L + 0.01, 0.5, 1.0);
              if (hsl_.L == 1.0) {
                sound_library_.SayMaximum();
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          if (steps < 0) {
            if (hsl_.L > 0.5) {
              hsl_.L = clamp(hsl_.L - 0.01, 0.5, 1.0);
              if (hsl_.L == 0.5) {
                sound_library_.SayMinimum();
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          break;
        case EDIT_BLACK:
          if (steps > 0) {
            if (hsl_.L < 0.5) {
              hsl_.L = clamp(hsl_.L + 0.01, 0.01, 0.5);
              if (hsl_.L == 0.5) {
                sound_library_.SayMaximum();
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          if (steps < 0) {
            if (hsl_.L > 0.01) {
              hsl_.L = clamp(hsl_.L - 0.01, 0.01, 0.5);
              if (hsl_.L == 0.01) {
                sound_library_.SayMinimum();
                hsl_angle_ = fusor.angle2();
              }
            }
          }
          break;
      }
      ShowColorStyle::SetColor(Color16(hsl_));
    }
  }

  // Saves New Color from Edit Mode Preview Styles to Preset
	
  void NewColor(int blade, int effect) {
    STDOUT << "NewColor(" << blade << "," << effect << ")\n";
    char new_color[32];
    Color16 color_source;
     switch (color_mode_) {
      case COLOR_LIST:
      case CC_COLOR_LIST:
        color_source = Color16(color_list_[dial_].color);
        break;
      default:
        color_source = Color16(hsl_);
        break;
    }
    itoa(Color16(color_source).r, new_color, 10);
    strcat(new_color, ",");
    itoa(Color16(color_source).g, new_color + strlen(new_color), 10);
    strcat(new_color, ",");
    itoa(Color16(color_source).b, new_color + strlen(new_color), 10);
#if NUM_BLADES > 1
    if (color_mode_ == CC_COLOR_LIST  || color_mode_ == CC_ZOOM_COLOR) {
      for (int i = 1; i <= NUM_BLADES; i++) {
        current_preset_.SetStyle(i,style_parser.SetArgument(current_preset_.GetStyle(i), effect + 2, new_color));
      }
    } else {
      current_preset_.SetStyle(blade,style_parser.SetArgument(current_preset_.GetStyle(blade), effect + 2, new_color));
    }
#else
    current_preset_.SetStyle(blade,style_parser.SetArgument(current_preset_.GetStyle(blade), effect + 2, new_color));
#endif
    color_mode_ = NONE;
  }

  // Toggles ColorChange Mode if current style uses RgbArg to CC_COLOR_LIST
  void ToggleCCMode() {
    bool uses_rgb_arg = false;
    for (int i = 1; i <= NUM_BLADES; i++)
      uses_rgb_arg |= style_parser.UsesArgument(current_preset_.GetStyle(i), 3);
    if (!uses_rgb_arg) {
#ifndef DISABLE_COLOR_CHANGE
      ToggleColorChangeMode();
#endif
    } else {
      bool handles_color_change;
    #define USES_COLOR_CHANGE(N) \
      handles_color_change |= current_config->blade##N->current_style() && current_config->blade##N->current_style()->IsHandled(HANDLED_FEATURE_CHANGE_TICKED);
      ONCEPERBLADE(USES_COLOR_CHANGE)
      if (!handles_color_change) {
        color_mode_ = CC_COLOR_LIST;
        show_color_all_.Start();
        for (int i = 1; i <= NUM_BLADES; i++) {
          if (style_parser.UsesArgument(current_preset_.GetStyle(i), BASE_COLOR_ARG + 2)) {
            ShowColorStyle::SetColor(GetColorArg(i, BASE_COLOR_ARG));
            break;
          }
        }
        current_menu_angle_ = fusor.angle2();
        dial_ = -1;
        hybrid_font.PlayCommon(&SFX_ccbegin);
      } else {
#ifndef DISABLE_COLOR_CHANGE
        ToggleColorChangeMode();
#endif
      }
    }	  
  }

  void DoInteractivePreon() {
    if (CheckInteractivePreon()) {
      SaberBase::DoEffect(EFFECT_INTERACTIVE_PREON, 0);
    } else {
      DoIgnition();
    }
  }

  void DoInteractiveBlast() {
    swing_blast_ = false;
    check_blast_ = false;
    if (CheckInteractiveBlast()) {
      SaberBase::DoEffectR(EFFECT_INTERACTIVE_BLAST);
    } else {
      SaberBase::DoBlast();
      if (battle_mode_) ToggleBattleModeMultiBlast();
    }
  }

  void Setup() override {
    RestoreGestureState();
  }

  bool chdir(const char* dir) override {
    bool ret = PropBase::chdir(dir);
    num_tracks_ = RunCommandAndGetSingleLine("list_current_tracks", nullptr, 0, 0, 0);
    track_num_ = 0;
#ifdef FETT263_SAVE_CHOREOGRAPHY
    RestoreChoreo();
    clash_count_ = 0;
#endif
    return ret;
  }

  // Check Event "Delays" for Edit Mode for Ignition/Retraction/Preon Settings Previews and Choreography Save
  void CheckEvent() {
    if (next_event_ && !wav_player->isPlaying()) {
#ifdef FETT263_SAVE_CHOREOGRAPHY
      if (rehearse_) {
        if (SaberBase::IsOn()) {
          rehearse_ = false;
          Off();
          saber_off_time_millis_ = millis();
          SaveChoreo();
        }
        next_event_ = false;
      } else 
#endif
      {
#ifdef FETT263_EDIT_MODE_MENU
        switch (menu_type_) {
          case MENU_IGNITION_TIME:
          case MENU_RETRACTION_TIME:
            next_event_ = false;
            SetInOut();
            break;
          default:
            next_event_ = false;
            break;
        }
#endif
      }
    }
#ifdef FETT263_EDIT_MODE_MENU
    if (off_event_ && millis() - restart_millis_ > 200) {
      Off();
      off_event_ = false;
      restart_ = true;
      restart_millis_ = millis();
    }
    if (restart_ && (int)(millis() - restart_millis_) > ignite_time_ + 1000) {
      restart_ = false;
      FastOn();
    }
#endif
  }

  enum ClashType {
    CLASH_NONE,
    CLASH_CHECK,
    CLASH_BATTLE_MODE,
    CLASH_LOCKUP,
    CLASH_LOCKUP_END,
#ifdef FETT263_CLASH_STRENGTH_SOUND
    CLASH_NORMAL,
    CLASH_STAB,
#endif
  };

#ifdef FETT263_CLASH_STRENGTH_SOUND
  void HandleClash() {
    if (clash_type_ == CLASH_BATTLE_MODE) {
      if (SaberBase::GetClashStrength() < saved_gesture_control.clashdetect) {
        clash_type_ = CLASH_NORMAL;
      } else {
        clash_type_ = CLASH_LOCKUP;
        auto_lockup_on_ = true;
      }
    }
    if (saved_gesture_control.maxclash < 8) saved_gesture_control.maxclash = 8;
    float clash_value = (SaberBase::GetClashStrength() - GetCurrentClashThreshold()) / saved_gesture_control.maxclash;
    switch (clash_type_) {
      default:
        break;
      case CLASH_NORMAL:
        if (SFX_clash) {
          SFX_clash.SelectFloat(clash_value);
        } else {
          SFX_clsh.SelectFloat(clash_value);
        }
        SaberBase::DoClash();
        break;
      case CLASH_STAB:
        if (SFX_stab) {
          SFX_stab.SelectFloat(clash_value);
        } else {
          if (SFX_clash) {
            SFX_clash.SelectFloat(clash_value);
          } else {
            SFX_clsh.SelectFloat(clash_value);
          }
        }
        SaberBase::DoStab();
        break;
      case CLASH_LOCKUP:
        if (battle_mode_) clash_value = (SaberBase::GetClashStrength() - saved_gesture_control.clashdetect) / saved_gesture_control.maxclash;
        if (SFX_bgnlock) {
          SFX_bgnlock.SelectFloat(clash_value);
        }
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        SaberBase::DoBeginLockup();
        break;
      case CLASH_LOCKUP_END:
        if (SFX_endlock) {
          float swing_value = fusor.swing_speed() / 600;
          SFX_endlock.SelectFloat(swing_value);
        }
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        break;
    }
    clash_type_ = CLASH_NONE;
  }
  #endif

  void Clash2(bool stab, float strength) override {
    Event(BUTTON_NONE, stab ? EVENT_STAB : EVENT_CLASH);
    SaberBase::SetClashStrength(strength);
    IgnoreClash(100);
  }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    Vec3 mss = fusor.mss();
    if (clash_type_ != CLASH_NONE && millis() - clash_impact_millis_ > 1) {
      // CHECK PUSH
      if (clash_type_ == CLASH_CHECK) {
        Event(BUTTON_NONE, EVENT_PUSH);
        STDOUT.println("EVENT PUSH");
        clash_type_ = CLASH_NONE;
      }
      if (clash_type_ != CLASH_LOCKUP_END) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
        HandleClash();
#else
        if (clash_type_ == CLASH_BATTLE_MODE && SaberBase::GetClashStrength() > saved_gesture_control.clashdetect) {
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          SaberBase::DoBeginLockup();
          auto_lockup_on_ = true;
	} else {
          SaberBase::DoClash();
        }
        clash_type_ = CLASH_NONE;
#endif
      }
    }
    sound_library_.Poll(wav_player);
#if defined(FETT263_EDIT_MODE_MENU) || defined(FETT263_SAVE_CHOREOGRAPHY)
    CheckEvent();
#endif
    EditColor();
    if (SaberBase::IsOn()) {
      DetectSwing();
#ifdef FETT263_SAVE_CHOREOGRAPHY
      if (choreo_ && saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_END) {
        EndChoreo();
      }
#endif
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > saved_gesture_control.lockupdelay &&
          SaberBase::Lockup()) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
        clash_type_ = CLASH_LOCKUP_END;
        HandleClash();
#else
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
#endif
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > saved_gesture_control.lockupdelay &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }
      // EVENT_PUSH
      if (!menu_ && fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 100 &&
          fusor.swing_speed() < 20 &&
          fabs(fusor.gyro().x) < 5) {
        if (millis() - push_begin_millis_ > saved_gesture_control.forcepushlen) {
          // Checking for Clash at end of movement
          clash_type_ = CLASH_CHECK;
          push_begin_millis_ = millis();
          clash_impact_millis_ = millis();
        }
      } else {
        push_begin_millis_ = millis();
      }
    } else {
      // EVENT_SWING - Swing On gesture control to allow fine tuning of speed needed to ignite
      if (menu_ || millis() - saber_off_time_millis_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (swinging_ && fusor.swing_speed() < 90) {
          swinging_ = false;
        }
        if (!swinging_ && fusor.swing_speed() > saved_gesture_control.swingonspeed) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
	}
      }
    }
    // EVENT_THRUST
    if (mss.y * mss.y + mss.z * mss.z < 16.0 &&
        mss.x > 14  &&
        fusor.swing_speed() < 150) {
      if (millis() - thrust_begin_millis_ > 15) {
        Event(BUTTON_NONE, EVENT_THRUST);
        thrust_begin_millis_ = millis();
      }
    } else {
      thrust_begin_millis_ = millis();
    }
#ifdef FETT263_QUICK_SELECT_ON_BOOT
    if (menu_type_ == MENU_PRESET && millis() < 2000) {
      current_menu_angle_ = fusor.angle2();
    } else {
      DetectMenuTurn();      
    }
#else
    DetectMenuTurn();
#endif
#ifdef ENABLE_AUDIO    
    TrackPlayer();
#else
    STDOUT.println("Audio disabled.");
#endif
  }

  // Fett263 Track Player
  enum TrackMode {
    PLAYBACK_OFF,
    PLAYBACK_LOOP,
    PLAYBACK_ROTATE,
    PLAYBACK_RANDOM,
  };

  void StartTrackPlayer() {
    if (num_tracks_ > 0) {
      sound_library_.SaySelect();
      StartMenu(MENU_TRACK_PLAYER);
      track_num_ = 1;
      PlayTrack();
    } else {
      // Loop default track if tracks not found
#ifndef FETT263_TRACK_PLAYER_NO_PROMPTS
      sound_library_.SayLoop();
#endif
      track_num_ = 0;
      track_mode_ = PLAYBACK_LOOP;
      StartOrStopTrack();
    }
  }

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
    } else {
      if (track_player_ && !track_player_->isPlaying()) {
        track_player_.Free();
      }
    }
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
    char playtrack[128];
    RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, playtrack, sizeof(playtrack));
    MountSDCard();
    EnableAmplifier();
    track_player_ = GetFreeWavPlayer();
    if (track_player_) {
      track_player_->Play(playtrack);
    } else {
      STDOUT.println("No available WAV players.");
    }
  }

  void StopTrackPlayer() {
    track_mode_ = PLAYBACK_OFF;
    if (track_player_) {
      track_player_->Stop();
      track_player_.Free();
    } else {
      StartOrStopTrack();
    }
  }

  void SoundLoop() {
    if (wav_player->isPlaying()) {
      wav_player->set_fade_time(0.2);
      wav_player->FadeAndStop();
      wav_player.Free();
      STDOUT.println("End Wav Player");    
    } else {
      wav_player = GetFreeWavPlayer();
      if (wav_player) {
        wav_player->PlayOnce(&SFX_trloop);
      } else {
        STDOUT.println("Out of WAV players!");
      }
    }
  }

  void SelectPreset(int preset) {
#ifdef SAVE_PRESET
    SaveState(preset);
#endif
    SetPreset(preset, true);
  }

  void DetectMenuTurn() {
    float a = fusor.angle2() - current_menu_angle_;
    if (a > M_PI) a-=M_PI*2;
    if (a < -M_PI) a+=M_PI*2;
    if (a > twist_menu_ * 2/3) {
      if (!swinging_) {
        Event(BUTTON_NONE, EVENT_TWIST_RIGHT);
        STDOUT.println("EVENT MENU TURN RIGHT");
      }
      current_menu_angle_ = fusor.angle2();
    }
    if (a < -twist_menu_ * 2/3) {
      if (!swinging_) {
        Event(BUTTON_NONE, EVENT_TWIST_LEFT);
        STDOUT.println("EVENT MENU TURN LEFT");
      }
      current_menu_angle_ = fusor.angle2();
    }
  }

#ifdef FETT263_USE_SETTINGS_MENU
#define GESTURE_OPTIONS 12
#define SET_SUBMENUS 5
#endif

#ifdef FETT263_EDIT_MODE_MENU
#define SUBMENUS 8
#define COLOR_OPTIONS 3
#endif

  enum MenuType {
    MENU_TOP,
    MENU_PRESET,
    MENU_VOLUME,
    MENU_TRACK_PLAYER,
#ifdef FETT263_USE_SETTINGS_MENU
    MENU_BLADE_LENGTH,
    MENU_LENGTH,
    MENU_GESTURE_SUB,
    MENU_SWINGON,
    MENU_TWISTON,
    MENU_THRUSTON,
    MENU_STABON,
    MENU_FORCEPUSH,
    MENU_TWISTOFF,
    MENU_POWERLOCK,
    MENU_SETTING_SUB,
    MENU_DIM_BLADE,
    MENU_CLASH_THRESHOLD,
    MENU_SWINGON_SPEED,
    MENU_FORCEPUSH_LENGTH,
    MENU_LOCKUP_DELAY,
    MENU_CLASH_DETECT,
    MENU_MAX_CLASH,
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
    MENU_REHEARSE,
#endif
#ifdef FETT263_EDIT_MODE_MENU
    MENU_STYLE,
    MENU_COLOR,
    MENU_COLOR_CHANGE,
    MENU_FONT,
    MENU_TRACK,
    MENU_COPY,
    MENU_DELETE,
    MENU_RESET,
    MENU_EXIT,
    MENU_STYLE_SUB,
    MENU_COLOR_SUB,
    MENU_COLOR_MODE,
    MENU_EFFECT,
    MENU_RESET_COLOR,
#if NUM_BLADES > 1
    MENU_COPY_COLOR,
    MENU_BLADE_STYLE,
    MENU_BLADE_COLOR,
    MENU_BLADE_COPY,
#endif
    MENU_STYLE_SETTING_SUB,
    MENU_STYLE_OPTION,
    MENU_STYLE_OPTION2,
    MENU_STYLE_OPTION3,
    MENU_IGNITION_OPTION,
    MENU_IGNITION_OPTION2,
    MENU_IGNITION_TIME,
    MENU_IGNITION_POWER_UP_OPTION,
    MENU_IGNITION_DELAY,
    MENU_RETRACTION_OPTION,
    MENU_RETRACTION_OPTION2,
    MENU_RETRACTION_TIME,
    MENU_RETRACTION_COOL_DOWN_OPTION,
    MENU_RETRACTION_DELAY,
    MENU_LOCKUP_POSITION,
    MENU_DRAG_SIZE,
    MENU_MELT_SIZE,
    MENU_SWING_OPTION,
    MENU_EMITTER_SIZE,
    MENU_OFF_OPTION,
    MENU_PREON_OPTION,
    MENU_PREON_SIZE,
    MENU_COLOR_BASE,
    MENU_COLOR_ALT,
    MENU_COLOR2_ALT,
    MENU_COLOR3_ALT,
    MENU_COLOR_BLAST,
    MENU_COLOR_CLASH,
    MENU_COLOR_LOCKUP,
    MENU_COLOR_DRAG,
    MENU_COLOR_LB,
    MENU_COLOR_STAB,
    MENU_COLOR_PREON,
    MENU_COLOR_IGNITE,
    MENU_COLOR_RETRACT,
    MENU_COLOR_PSTOFF,
    MENU_COLOR_SWING,
    MENU_COLOR_EMITTER,
    MENU_COLOR_OFF,
#endif
  };

  enum MainMenu {
    EDIT_STYLE = 1,
    EDIT_COLORS = 2,
    EDIT_FONT = 3,
    EDIT_TRACK = 4,
    EDIT_SETTINGS = 5,
    COPY_PRESET = 6,
    DELETE_PRESET = 7,
    EXIT_MENU = 8,
  };

  enum StyleSubMenu {
    EDIT_STYLE_SELECT = 1,
    EDIT_STYLE_SETTINGS = 2,
  };
	
  enum ColorSubMenu {
    EDIT_EFFECT_COLOR = 1,
#if NUM_BLADES == 1
    RESET_COLORS = 2,
#else
    COPY_COLORS = 2,
    RESET_COLORS = 3,
#endif
  };
  enum ColorOptions {
    COLOR_LIST_MODE = 0,
    COLOR_HUE_MODE = 1,
    WHITE_MODE = 2,
    BLACK_MODE = 3,
  };

  enum SettingsMenu {
    EDIT_VOLUME = 1,
    EDIT_CONTROL_SETTINGS = 2,
    EDIT_CLASH_THRESHOLD = 3,
    EDIT_BLADE_LENGTH = 4,
    EDIT_BRIGHTNESS = 5,
  };

  enum GestureControls {
    GESTURE_SWINGON_IGNITION = 1,
    GESTURE_SWINGON_SPEED = 2,
    GESTURE_TWIST_IGNITION = 3,
    GESTURE_THRUST_IGNITION = 4,
    GESTURE_STAB_IGNITION = 5,
    GESTURE_FORCE_PUSH = 6,
    GESTURE_FORCE_PUSH_LENGTH = 7,
    GESTURE_TWIST_RETRACTION = 8,
    GESTURE_LOCKUP_DELAY = 9,
    GESTURE_CLASH_DETECTION = 10,
    GESTURE_POWER_LOCK = 11,
    GESTURE_MAX_CLASH = 12,
  };

  int GetStyleNumber(int blade_num) {
    const char* tmp;
    tmp = current_preset_.GetStyle(blade_num);
    int style_num = FirstWord(tmp, "builtin") ? atoi(SkipWord(tmp)) : 0;
    return style_num;
  }

  void ChangeStyleNumber(int blade_num, int direction) {
    int num_presets = current_config->num_presets;
    int style_num = GetStyleNumber(blade_num);
    style_num += direction;
    if (style_num < 0) style_num = num_presets - 1;
    if (style_num >= num_presets) style_num = 0;
    char style_arg[10];
    itoa(style_num, style_arg, 10);
    current_preset_.SetStyle(blade_num, style_parser.SetArgument(current_preset_.GetStyle(blade_num), 1, style_arg));
  }

  void ChangeStyleNumberAllBlades(int direction) {
    for (int i = 1; i <= NUM_BLADES; i++) {
      ChangeStyleNumber(i, direction);
    }
    current_preset_.Save();
    UpdateStyle();
  }

  // Uses font_num_ value for starting font, not current font
  void ChangeFont(int direction) {
    int num_fonts = RunCommandAndGetSingleLine("list_fonts", nullptr, 0, 0, 0);
    font_num_ += direction;
    if (font_num_ > num_fonts) font_num_ = 1;
    if (font_num_ <= 0) font_num_ = num_fonts;
    char font[128];
    RunCommandAndGetSingleLine("list_fonts", nullptr, font_num_, font, sizeof(font));
    strcat(font, ";common");
    current_preset_.font = mkstr(font);
    current_preset_.Save();
    SetPresetFast(current_preset_.preset_num);
    hybrid_font.SB_Effect(EFFECT_NEWFONT, 0);
  }

  int GetNumberOfPresets() {
    CurrentPreset tmp;
    tmp.SetPreset(-1);
    return tmp.preset_num + 1;
  }

#ifdef FETT263_EDIT_MODE_MENU
// Color Edit Helper Functions
  void SaveColorEdit() {
    menu_type_ = MENU_COLOR_MODE;
    edit_color_ = false;
    twist_menu_ = M_PI / 4;
    NewColor(blade_num_, effect_num_);
    current_preset_.Save();
    color_mode_ = NONE;
  }

  void RevertColorEdit() {
    menu_type_ = MENU_COLOR_MODE;
    edit_color_ = false;
    twist_menu_ = M_PI / 4;
    color_mode_ = NONE;
  }
#endif

#ifdef FETT263_USE_SETTINGS_MENU
// Gesture Control Boolean Menu
  void EnterBooleanMenu(MenuType t, bool current_value) {
    menu_type_ = t;
    choice_ = current_value;
    sound_library_.SayBool(current_value);
  }

  void StartEditMode() {
    if (track_player_) {
      StopTrackPlayer();
    }
    if (!SFX_medit) {
      ProffieOSErrors::error_in_font_directory();
      STDOUT.println("Edit Mode prompts missing");
    } else {
#ifdef FETT263_EDIT_SETTINGS_MENU
      STDOUT.println("Enter Edit Settings Menu");
#else
      STDOUT.println("Enter Edit Mode");
#endif
      GenerateIniFiles();
      menu_top_pos_ = 0;
#ifdef FETT263_EDIT_SETTINGS_MENU
      sound_library_.SaySettingsMenu();
      StartMenu(MENU_SETTING_SUB);
#else
      sound_library_.SayEditMode();
      StartMenu(MENU_TOP);
#endif
      FastOn();
    }
  }
#endif

  void CopyPreset(bool announce) {
    int32_t pos = current_preset_.preset_num;
    current_preset_.preset_num = -1;
    current_preset_.SaveAt(pos);
    if (announce) {
      sound_library_.SayCopyPreset();
      wav_player.Free();
    }
  }

  // Check if ShowColor for ColorChange / Color Editing is active to prevent other events
  bool CheckShowColorCC() {
    if (color_mode_ == CC_COLOR_LIST || color_mode_ == CC_ZOOM_COLOR) return true;
    if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) return true;
    return false;
  }

  // Check to see if ShowColor style is being used and use MenuUndo to properly close if button presses or holds not in menu are used
  bool CancelShowColor() {
    if (color_mode_ != NONE) {
      if (CheckShowColorCC()) {
        EndColorZoom();
        return true;
      } else {
        MenuUndo();
        return true;
      }
    } else {
      switch (menu_type_) {
#ifdef FETT263_EDIT_MODE_MENU
        case MENU_LENGTH:
#if NUM_BLADES > 1
        case MENU_BLADE_STYLE:
        case MENU_BLADE_COLOR:
        case MENU_BLADE_COPY:
        case MENU_BLADE_LENGTH:
        case MENU_COPY_COLOR:
#endif
#elif defined(FETT263_EDIT_SETTINGS_MENU)
        case MENU_LENGTH:
#if NUM_BLADES > 1
        case MENU_BLADE_LENGTH:
#endif   
          MenuUndo();
          return true;
          break;
#endif
        default:
          return false;
          break;
      }
    }
  }

  bool DoColorZoom() {
    if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_SMOOTH) {
      SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_ZOOMED);
      return true;
    }
    if (color_mode_ == COLOR_LIST || color_mode_ == CC_COLOR_LIST) {
      hsl_ = Color16(color_list_[dial_].color).toHSL();
    }
    switch (color_mode_) {
      case COLOR_LIST:
      case EDIT_COLOR:
        color_mode_ = ZOOM_COLOR;
        hsl_angle_ = fusor.angle2();
        return true;
        break;
      case CC_COLOR_LIST:
        color_mode_ = CC_ZOOM_COLOR;
        edit_color_ = true;
        hsl_angle_ = fusor.angle2();
        return true;
        break;
      default:
        return false;
        break;
    }
  }

  // If Color Zoom mode is active save NewColor and end ShowColor style
  bool EndColorZoom() {
#ifndef DISABLE_COLOR_CHANGE
    if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_ZOOMED) {
      ToggleColorChangeMode();
      return true;
    }
#endif
    switch(color_mode_) {
      case ZOOM_COLOR:
      case EDIT_COLOR:
      case COLOR_LIST:
        MenuChoice();
        return true;
        break;
      case CC_COLOR_LIST:
      case CC_ZOOM_COLOR:
        edit_color_ = false;
        hybrid_font.PlayCommon(&SFX_ccend);
        NewColor(1, BASE_COLOR_ARG);
        current_preset_.Save();
        show_color_all_.Stop();
        UpdateStyle();
        wav_player.Free();
        return true;
        break;
      default:
        return false;
        break;
    }
  }

// Start Menu Mode
  void StartMenu(MenuType menu) {
    current_menu_angle_ = fusor.angle2();
    menu_type_ = menu;
    menu_ = true;
  }

// Edit Mode Menu Select (PWR Button)
  void MenuChoice() {
    switch (menu_type_) {
    case MENU_PRESET:
      sound_library_.SaySelect();
      menu_type_ = MENU_TOP;
      menu_ = false;
      break;
#ifdef FETT263_SAVE_CHOREOGRAPHY
    case MENU_REHEARSE:
      if (choice_) {
        menu_ = false;
        BeginRehearsal();
      } else {
        MenuExit();
      }
      break;
#endif
    case MENU_VOLUME:
      if (SaberBase::IsOn()) {
#ifdef FETT263_USE_SETTINGS_MENU
        menu_type_ = MENU_SETTING_SUB;
#endif
        MenuSave();
      } else {
        MenuExit();
      }
      break;
#ifdef FETT263_USE_SETTINGS_MENU
#if NUM_BLADES > 1
    case MENU_BLADE_LENGTH:
      sound_library_.SaySelect();
      show_preview_.Stop(blade_preview_);
      SaveState(current_preset_.preset_num);
      blade_preview_ = 0;
      max_length_ = GetMaxBladeLength(blade_num_);
      blade_length_ = GetBladeLength(blade_num_);
      if (blade_length_ < 1) blade_length_ = max_length_;
      length_revert_ = blade_length_;
      sound_library_.SayNumber(blade_length_, SAY_WHOLE);
      LengthPreview(blade_num_);
      menu_type_ = MENU_LENGTH;
      break;
#endif
    case MENU_LENGTH:
      menu_type_ = MENU_SETTING_SUB;
      current_preset_.Save();
      show_length_.Stop(blade_num_);
      UpdateStyle();
      MenuSave();
      break;
    case MENU_GESTURE_SUB:
      switch (gesture_num_) {
        case GESTURE_SWINGON_IGNITION:
          EnterBooleanMenu(MENU_SWINGON, saved_gesture_control.swingon);
          break;
        case GESTURE_SWINGON_SPEED:
          menu_type_ = MENU_SWINGON_SPEED;
          sound_library_.SaySelect();
          calc_ = saved_gesture_control.swingonspeed;
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case GESTURE_TWIST_IGNITION:
          EnterBooleanMenu(MENU_TWISTON, saved_gesture_control.twiston);
          break;
        case GESTURE_THRUST_IGNITION:
          EnterBooleanMenu(MENU_THRUSTON, saved_gesture_control.thruston);
          break;
        case GESTURE_STAB_IGNITION:
          EnterBooleanMenu(MENU_STABON, saved_gesture_control.stabon);
          break;
        case GESTURE_FORCE_PUSH:
          EnterBooleanMenu(MENU_FORCEPUSH, saved_gesture_control.forcepush);
          break;
        case GESTURE_FORCE_PUSH_LENGTH:
          menu_type_ = MENU_FORCEPUSH_LENGTH;
          sound_library_.SaySelect();
          calc_ = saved_gesture_control.forcepushlen;
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case GESTURE_TWIST_RETRACTION:
          EnterBooleanMenu(MENU_TWISTOFF, saved_gesture_control.twistoff);
          break;
        case GESTURE_LOCKUP_DELAY:
          menu_type_ = MENU_LOCKUP_DELAY;
          sound_library_.SaySelect();
          calc_ = saved_gesture_control.lockupdelay;
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          sound_library_.SayMillis();
          break;
        case GESTURE_CLASH_DETECTION:
          menu_type_ = MENU_CLASH_DETECT;
          sound_library_.SaySelect();
          calc_ = saved_gesture_control.clashdetect;
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case GESTURE_POWER_LOCK:
          EnterBooleanMenu(MENU_POWERLOCK, saved_gesture_control.powerlock);
          break;
        case GESTURE_MAX_CLASH:
          menu_type_ = MENU_MAX_CLASH;
          sound_library_.SaySelect();
          calc_ = saved_gesture_control.maxclash;
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        default:
          break;
      }
      break;
    case MENU_SWINGON:
      if (choice_) {
        saved_gesture_control.swingon = true;
        SaveGestureState();
      } else {
        saved_gesture_control.swingon = false;
        SaveGestureState();
      }
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_SWINGON_SPEED:
      saved_gesture_control.swingonspeed = calc_;
      SaveGestureState();
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_TWISTON:
      if (choice_) {
        saved_gesture_control.twiston = true;
        SaveGestureState();
      } else {
        saved_gesture_control.twiston = false;
        SaveGestureState();
      }
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_THRUSTON:
      if (choice_) {
        saved_gesture_control.thruston = true;
        SaveGestureState();
      } else {
        saved_gesture_control.thruston = false;
        SaveGestureState();
      }
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_STABON:
      if (choice_) {
        saved_gesture_control.stabon = true;
        SaveGestureState();
      } else {
        saved_gesture_control.stabon = false;
        SaveGestureState();
      }
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_FORCEPUSH:
      if (choice_) {
        saved_gesture_control.forcepush = true;
        SaveGestureState();
      } else {
        saved_gesture_control.forcepush = false;
        SaveGestureState();
      }
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_FORCEPUSH_LENGTH:
      saved_gesture_control.forcepushlen = calc_;
      SaveGestureState();
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_TWISTOFF:
      if (choice_) {
        saved_gesture_control.twistoff = true;
        SaveGestureState();
      } else {
        saved_gesture_control.twistoff = false;
        SaveGestureState();
      }
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_LOCKUP_DELAY:
      saved_gesture_control.lockupdelay = calc_;
      SaveGestureState();
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_CLASH_DETECT:
      saved_gesture_control.clashdetect = calc_;
      SaveGestureState();
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_POWERLOCK:
      if (choice_) {
        saved_gesture_control.powerlock = true;
        SaveGestureState();
      } else {
        saved_gesture_control.powerlock = false;
        SaveGestureState();
      }
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_MAX_CLASH:
      saved_gesture_control.maxclash = calc_;
      SaveGestureState();
      menu_type_ = MENU_GESTURE_SUB;
      MenuSave();
      break;
    case MENU_SETTING_SUB:
      switch (menu_sub_pos_) {
      case EDIT_VOLUME:
        menu_type_ = MENU_VOLUME;
        sound_library_.SaySelect();
        break;
      case EDIT_CONTROL_SETTINGS:
        menu_type_ = MENU_GESTURE_SUB;
        sound_library_.SaySelectOption();
        gesture_num_ = 0;
        break;
      case EDIT_CLASH_THRESHOLD:
        menu_type_ = MENU_CLASH_THRESHOLD;
        sound_library_.SaySelect();
        clash_t_ = GetCurrentClashThreshold();
        sound_library_.SayNumber(clash_t_, SAY_DECIMAL);
        break;
      case EDIT_BLADE_LENGTH:
#if NUM_BLADES == 1
        menu_type_ = MENU_LENGTH;
        SaveState(current_preset_.preset_num);
        blade_num_ = 1;
        max_length_ = GetMaxBladeLength(blade_num_);
        blade_length_ = GetBladeLength(blade_num_);
        sound_library_.SaySelect();
        if (blade_length_ < 1) blade_length_ = max_length_;
        length_revert_ = blade_length_;
        LengthPreview(blade_num_);
        sound_library_.SayNumber(blade_length_, SAY_WHOLE);
#else
        menu_type_ = MENU_BLADE_LENGTH;
        blade_num_ = 0;
        sound_library_.SaySelectBlade();
#endif
        break;
      case EDIT_BRIGHTNESS:
        menu_type_ = MENU_DIM_BLADE;
        dim_revert_ = dim = pow(SaberBase::GetCurrentDimming() / 16384.0, 1/2.5);
        sound_library_.SaySelect();
        break;
      }
      break;
    case MENU_CLASH_THRESHOLD:
      SetClashThreshold(clash_t_);
      SaveGlobalState();
      menu_type_ = MENU_SETTING_SUB;
      MenuSave();
      break;
    case MENU_DIM_BLADE:
      SaveGlobalState();
      menu_type_ = MENU_SETTING_SUB;
      MenuSave();
      break;
#endif
#ifdef FETT263_EDIT_MODE_MENU
    case MENU_TOP:
      switch (menu_top_pos_) {
        case EDIT_STYLE:
#if NUM_BLADES == 1
          menu_type_ = MENU_STYLE_SUB;
          menu_sub_pos_ = 0;
          blade_num_ = 1;
          sound_library_.SaySelectOption();         
#else
          menu_type_ = MENU_BLADE_STYLE;
          blade_num_ = 0;
          sound_library_.SaySelectBlade();
#endif
          break;
        case EDIT_COLORS:
          if (style_parser.MaxUsedArgument(current_preset_.GetStyle(1)) == 0) {
#ifndef DISABLE_COLOR_CHANGE
            menu_type_ = MENU_COLOR;
            sound_library_.SaySelect();
            variation_revert_ = SaberBase::GetCurrentVariation();
            ToggleColorChangeMode();
#endif
            break;
          } else {
#if NUM_BLADES == 1
            blade_num_ = 1;
            menu_type_ = MENU_COLOR_SUB;
            menu_sub_pos_ = 0;
            sound_library_.SayColorMenu();
            effect_num_ = 0;
#else
            menu_type_ = MENU_BLADE_COLOR;
            blade_num_ = 0;
            sound_library_.SaySelectBlade();
#endif
  }
          break;
        case EDIT_FONT:
          menu_type_ = MENU_FONT;
          font_num_ = 0;
          sound_library_.SaySelect();
          break;
        case EDIT_TRACK:
          StartOrStopTrack();
          menu_type_ = MENU_TRACK;
          track_num_ = 0;
          sound_library_.SaySelect();
          break;
        case EDIT_SETTINGS:
          menu_type_ = MENU_SETTING_SUB;
          sound_library_.SaySelectOption();
          menu_sub_pos_ = 0;
          break;
        case COPY_PRESET:
          menu_type_ = MENU_COPY;
          sound_library_.SayConfirm();
          break;
        case DELETE_PRESET:
          menu_type_ = MENU_DELETE;
          sound_library_.SayConfirm();
          break;
        case EXIT_MENU:
          sound_library_.SaySelect();
          MenuExit();
          break;
      }
      break;
#if NUM_BLADES > 1
    case MENU_BLADE_STYLE:
      if (blade_num_ == 0) {
        sound_library_.SaySelectBlade();
        break;
      }
      show_preview_.Stop(blade_preview_);
      blade_preview_ = 0;
      menu_sub_pos_ = 0;
      menu_type_ = MENU_STYLE_SUB;
      sound_library_.SaySelectOption();
      break;
    case MENU_BLADE_COLOR:
#if NUM_BLADES > 2
      copy_blade_ = blade_num_;
      sound_library_.SayColorMenu();
#else
      if (blade_num_ == 1) {
        copy_blade_ = 2;
      } else {
        copy_blade_ = 1;
      }
      sound_library_.SaySelectOption();
#endif
      menu_type_ = MENU_COLOR_SUB;
      menu_sub_pos_ = 0;
      show_preview_.Stop(blade_preview_);
      blade_preview_ = 0;
      break;
    case MENU_BLADE_COPY:
      menu_type_ = MENU_COPY_COLOR;
      CopyColors();
      MenuConfirm();
      break;
    case MENU_COPY_COLOR:
      if (choice_) {
        current_preset_.SetStyle(copy_blade_, style_parser.CopyArguments(current_preset_.GetStyle(blade_num_), current_preset_.GetStyle(copy_blade_), int_arg_menu_, NELEM(int_arg_menu_)));
        DOVALIDATE(current_preset_);
        current_preset_.Save();
        DOVALIDATE(current_preset_);
        show_preview_.Stop(blade_preview_);
        UpdateStyle();
        menu_type_ = MENU_COLOR_SUB;
        MenuSelect();
        choice_ = false;
        blade_preview_ = 0;
        break;
      }
      sound_library_.SayConfirm();
      break;
#endif
    case MENU_STYLE_SUB:
      switch (menu_sub_pos_) {
        case EDIT_STYLE_SELECT:
          menu_type_ = MENU_STYLE;
          style_revert_ = GetStyleNumber(blade_num_);
          sound_library_.SaySelect();
          SayStyleNumber(style_revert_);
          break;
        case EDIT_STYLE_SETTINGS:
          effect_num_ = 0;
          menu_type_ = MENU_STYLE_SETTING_SUB;
          sound_library_.SaySelectOption();
          arg_dial_ = -1;
          break;
        default:
          sound_library_.SaySelectOption();
      }
      break;
    case MENU_STYLE:
      restore_point = nullptr;
      current_preset_.Save();
      menu_type_ = MENU_STYLE_SUB;
      MenuSave();
      break;
    case MENU_COLOR:
      break;
    case MENU_COLOR_SUB:
#if NUM_BLADES == 1
      switch (menu_sub_pos_) {
        case EDIT_EFFECT_COLOR:
          menu_type_ = MENU_EFFECT;
          sound_library_.SaySelectEffect();
          arg_dial_ = -1;
          break;
        case RESET_COLORS:
          menu_type_ = MENU_RESET_COLOR;
          sound_library_.SayConfirm();
          break;
        default:
          sound_library_.SaySelectOption();
      }
#elif NUM_BLADES == 2
      switch (menu_sub_pos_) {
        case EDIT_EFFECT_COLOR:
          menu_type_ = MENU_EFFECT;
          sound_library_.SaySelectEffect();
          arg_dial_ = -1;
          break;
        case COPY_COLORS:
          menu_type_ = MENU_COPY_COLOR;
          CopyColors();
          sound_library_.SayConfirm();
          break;
        case RESET_COLORS:
          menu_type_ = MENU_RESET_COLOR;
          sound_library_.SayConfirm();
          break;
        default:
          sound_library_.SaySelectOption();
          break;
      }
#else
      switch (menu_sub_pos_) {
        case EDIT_EFFECT_COLOR:
          menu_type_ = MENU_EFFECT;
          sound_library_.SaySelectEffect();
          arg_dial_ = -1;
          break;
        case COPY_COLORS:
          menu_type_ = MENU_BLADE_COPY;
          CopyColors();
          sound_library_.SaySelectBlade();
          break;
        case RESET_COLORS:
          menu_type_ = MENU_RESET_COLOR;
          sound_library_.SayConfirm();
          break;
        default:
          sound_library_.SaySelectOption();
      }
#endif
      break;
    case MENU_COLOR_MODE:
      edit_color_ = true;
      switch (sub_dial_) {
        case COLOR_LIST_MODE:
          twist_menu_ = M_PI / 6;
          dial_ = -1;
          color_mode_ = COLOR_LIST;
          break;
        case COLOR_HUE_MODE:
          color_mode_ = EDIT_COLOR;
          hsl_.S = 1.0;
          break;
        case WHITE_MODE:
          color_mode_ = EDIT_WHITE;
          twist_menu_ = M_PI / 18;
          hsl_.S = 1.0;
          break;
        case BLACK_MODE:
          color_mode_ = EDIT_BLACK;
          twist_menu_ = M_PI / 36;
          hsl_.S = 1.0;
          break;
        default:
          break;
      }
      switch (effect_num_) {
        case BASE_COLOR_ARG:
          menu_type_ = MENU_COLOR_BASE;
          ShowFull();
          break;
        case ALT_COLOR_ARG:
          menu_type_ = MENU_COLOR_ALT;
          ShowFull();
          break;
        case ALT_COLOR2_ARG:
          menu_type_ = MENU_COLOR2_ALT;
          ShowFull();
          break;
        case ALT_COLOR3_ARG:
          menu_type_ = MENU_COLOR3_ALT;
          ShowFull();
          break;
        case BLAST_COLOR_ARG:
          menu_type_ = MENU_COLOR_BLAST;
          bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case CLASH_COLOR_ARG:
          menu_type_ = MENU_COLOR_CLASH;
          bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case LOCKUP_COLOR_ARG:
          menu_type_ = MENU_COLOR_LOCKUP;
          bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case DRAG_COLOR_ARG:
          menu_type_ = MENU_COLOR_DRAG;
          tip_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case LB_COLOR_ARG:
          menu_type_ = MENU_COLOR_LB;
          bump_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case STAB_COLOR_ARG:
          menu_type_ = MENU_COLOR_STAB;
          tip_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case PREON_COLOR_ARG:
          menu_type_ = MENU_COLOR_PREON;
          pre_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case IGNITION_COLOR_ARG:
          menu_type_ = MENU_COLOR_IGNITE;
          ShowFull();
          break;
        case RETRACTION_COLOR_ARG:
          menu_type_ = MENU_COLOR_RETRACT;
          ShowFull();
          break;
        case POSTOFF_COLOR_ARG:
          menu_type_ = MENU_COLOR_PSTOFF;
          pre_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case SWING_COLOR_ARG:
          menu_type_ = MENU_COLOR_SWING;
          ShowFull();
          break;
        case EMITTER_COLOR_ARG:
          menu_type_ = MENU_COLOR_EMITTER;
          hilt_color_.Start(blade_num_, SkipWord(SkipWord(current_preset_.GetStyle(blade_num_))));
          ShowPart();
          break;
        case OFF_COLOR_ARG:
          menu_type_ = MENU_COLOR_OFF;
          ShowFull();
          break;
        default:
          break;
      }
      sound_library_.SaySelect();
      break;
    case MENU_EFFECT:
      menu_type_ = MENU_COLOR_MODE;
      sub_dial_ = -1;
      sound_library_.SaySelectOption();
      break;
    case MENU_COLOR_BASE:
    case MENU_COLOR_ALT:
    case MENU_COLOR2_ALT:
    case MENU_COLOR3_ALT:
    case MENU_COLOR_IGNITE:
    case MENU_COLOR_RETRACT:
    case MENU_COLOR_SWING:
    case MENU_COLOR_OFF:
      SaveColorEdit();
      show_color_.Stop(blade_num_);
      UpdateStyle();
      MenuSave();
      break;
    case MENU_COLOR_BLAST:
    case MENU_COLOR_CLASH:
    case MENU_COLOR_LOCKUP:
    case MENU_COLOR_LB:
      SaveColorEdit();
      bump_color_.Stop(blade_num_);
      UpdateStyle();
      MenuSave();
      break;
    case MENU_COLOR_DRAG:
    case MENU_COLOR_STAB:
      SaveColorEdit();
      tip_color_.Stop(blade_num_);
      UpdateStyle();
      MenuSave();
      break;
    case MENU_COLOR_PREON:
    case MENU_COLOR_PSTOFF:
      SaveColorEdit();
      pre_color_.Stop(blade_num_);
      UpdateStyle();
      MenuSave();
      break;
    case MENU_COLOR_EMITTER:
      SaveColorEdit();
      hilt_color_.Stop(blade_num_);
      UpdateStyle();
      MenuSave();
      break;
    case MENU_FONT:
      restore_point = nullptr;
      menu_type_ = MENU_TOP;
      MenuSave();
      break;
    case MENU_TRACK:
      restore_point = nullptr;
      if (track_player_) {
        track_player_->Stop();
        track_player_.Free();
      }
      menu_type_ = MENU_TOP;
      MenuSave();
      break;
    case MENU_COPY:
      if (choice_) {
        CopyPreset(false);
        menu_type_ = MENU_TOP;
        MenuSelect();
        choice_ = false;
        break;
      }
      sound_library_.SayConfirm();
      break;
    case MENU_RESET_COLOR:
      if (choice_) {
        SaberBase::SetVariation(0);
        current_preset_.SetStyle(blade_num_, style_parser.CopyArguments("~", current_preset_.GetStyle(blade_num_),  int_arg_menu_, NELEM(int_arg_menu_)));
        current_preset_.Save();
        UpdateStyle();
        menu_type_ = MENU_COLOR_SUB;
        MenuSelect();
        choice_ = false;
        break;
      }
      sound_library_.SayConfirm();
      break;
    case MENU_STYLE_SETTING_SUB:
      char argspace[32];
      style_parser.GetArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2, argspace);
      calc_ = strtol(argspace, NULL, 0);
      IntEdit::SetIntValue(calc_);
      if (NUM_BLADES == 1) blade_num_ = 1;
      switch (set_num_) {
  case PREON_OPTION_ARG:
        case PREON_SIZE_ARG:
          // sound for preon menus chosen below, based on if SFX_preon exists or not
    break;
  default:
    sound_library_.SaySelect();
    break;
      }
      switch (set_num_) {
        case STYLE_OPTION_ARG:
          menu_type_ = MENU_STYLE_OPTION;
          arg_revert_ = strtol (argspace, NULL, 0);
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case STYLE_OPTION2_ARG:
          menu_type_ = MENU_STYLE_OPTION2;
          arg_revert_ = strtol (argspace, NULL, 0);
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case STYLE_OPTION3_ARG:
          menu_type_ = MENU_STYLE_OPTION3;
          arg_revert_ = strtol (argspace, NULL, 0);
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case IGNITION_OPTION_ARG:
        case IGNITION_OPTION2_ARG:
          arg_revert_ = strtol (argspace, NULL, 0);
          if (set_num_ == IGNITION_OPTION2_ARG) {
            menu_type_ = MENU_IGNITION_OPTION2;
          } else {
            menu_type_ = MENU_IGNITION_OPTION;
            sound_library_.SayOption();
          }
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case IGNITION_TIME_ARG:
          menu_type_ = MENU_IGNITION_TIME;
          arg_revert_ = strtol (argspace, NULL, 0);
          break;
        case IGNITION_POWER_UP_ARG:
          menu_type_ = MENU_IGNITION_POWER_UP_OPTION;
          arg_revert_ = strtol (argspace, NULL, 0); 
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case IGNITION_DELAY_ARG:
          menu_type_ = MENU_IGNITION_DELAY;
          arg_revert_ = strtol (argspace, NULL, 0);         
          break;
        case RETRACTION_OPTION_ARG:
        case RETRACTION_OPTION2_ARG:
          char ig[32];
          style_parser.GetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, ig);
          ignite_time_ = strtol(ig, NULL, 0);
          current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, "1"));
          arg_revert_ = strtol(argspace, NULL, 0);
          if (set_num_ == RETRACTION_OPTION2_ARG) {
            menu_type_ = MENU_RETRACTION_OPTION2;
          } else {
            menu_type_ = MENU_RETRACTION_OPTION;
            sound_library_.SayOption();
          }
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case RETRACTION_TIME_ARG:
          menu_type_ = MENU_RETRACTION_TIME;
          char igt[32];
          style_parser.GetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, igt);
          ignite_time_ = strtol(igt, NULL, 0);
          current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, "1"));
          arg_revert_ = strtol(argspace, NULL, 0);
          break;
        case RETRACTION_COOL_DOWN_ARG:
          menu_type_ = MENU_RETRACTION_COOL_DOWN_OPTION;
          char igc[32];
          style_parser.GetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, igc);
          ignite_time_ = strtol(igc, NULL, 0);
          current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, "1"));
          arg_revert_ = strtol(argspace, NULL, 0);
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case RETRACTION_DELAY_ARG:
          menu_type_ = MENU_RETRACTION_DELAY;
          char igd[32];
          style_parser.GetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, igd);
          ignite_time_ = strtol(igd, NULL, 0);
          current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, "1"));
          arg_revert_ = strtol(argspace, NULL, 0);
          break;
        case LOCKUP_POSITION_ARG:
          menu_type_ = MENU_LOCKUP_POSITION;
          ShowColorStyle::SetColor(GetColorArg(blade_num_, LOCKUP_COLOR_ARG));
          show_lockup_position_.Start(blade_num_);
          break;
        case DRAG_SIZE_ARG:
          menu_type_ = MENU_DRAG_SIZE;
          ShowColorStyle::SetColor(GetColorArg(blade_num_, DRAG_COLOR_ARG));
          show_drag_size_.Start(blade_num_);
          break;
        case MELT_SIZE_ARG:
          menu_type_ = MENU_MELT_SIZE;
          ShowColorStyle::SetColor(GetColorArg(blade_num_, DRAG_COLOR_ARG));
          show_drag_size_.Start(blade_num_);
          break;
        case SWING_OPTION_ARG:
          menu_type_ = MENU_SWING_OPTION;
          arg_revert_ = strtol (argspace, NULL, 0);
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case EMITTER_SIZE_ARG:
          menu_type_ = MENU_EMITTER_SIZE;
          ShowColorStyle::SetColor(GetColorArg(blade_num_, EMITTER_COLOR_ARG));
          show_emitter_size_.Start(blade_num_);
          break;
        case OFF_OPTION_ARG:
          menu_type_ = MENU_OFF_OPTION;
          arg_revert_ = strtol (argspace, NULL, 0);
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
          break;
        case PREON_OPTION_ARG:
          if (!SFX_preon) {
            sound_library_.SaySelectOption();
          } else {
            sound_library_.SaySelect();
            menu_type_ = MENU_PREON_OPTION;
            arg_revert_ = strtol (argspace, NULL, 0);
            sound_library_.SayOption();
            sound_library_.SayNumber(calc_, SAY_WHOLE);
          }
          break;
        case PREON_SIZE_ARG:
          if (!SFX_preon) {
            sound_library_.SaySelectOption();
          } else {
            sound_library_.SaySelect();
            menu_type_ = MENU_PREON_SIZE;
            arg_revert_ = strtol (argspace, NULL, 0);
            ShowColorStyle::SetColor(GetColorArg(blade_num_, PREON_COLOR_ARG));
            show_emitter_size_.Start(blade_num_);
          }
          break;
        default:
          break;
      }
      break;
    case MENU_STYLE_OPTION:
    case MENU_STYLE_OPTION2:
    case MENU_STYLE_OPTION3:
    case MENU_IGNITION_OPTION:
    case MENU_IGNITION_OPTION2:
    case MENU_IGNITION_TIME:
    case MENU_IGNITION_POWER_UP_OPTION:
    case MENU_IGNITION_DELAY:
    case MENU_SWING_OPTION:
    case MENU_OFF_OPTION:
    case MENU_PREON_OPTION:
      menu_type_ = MENU_STYLE_SETTING_SUB;
      current_preset_.Save();
      MenuSave();
      break;
    case MENU_RETRACTION_OPTION:
    case MENU_RETRACTION_OPTION2:
    case MENU_RETRACTION_TIME:
    case MENU_RETRACTION_COOL_DOWN_OPTION:
    case MENU_RETRACTION_DELAY:
      menu_type_ = MENU_STYLE_SETTING_SUB;
      char style_arg[10];
      itoa(ignite_time_, style_arg, 10);
      current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), RETRACTION_TIME_ARG, style_arg));
      current_preset_.Save();
      MenuSave();
      break;
    case MENU_LOCKUP_POSITION:
    case MENU_DRAG_SIZE:
    case MENU_MELT_SIZE:
    case MENU_EMITTER_SIZE:
    case MENU_PREON_SIZE:
      SaveStyleSetting();
      break;
    case MENU_DELETE:
      if (choice_) {
        current_preset_.SaveAt(-1);
        choice_ = false;
        menu_type_ = MENU_TOP;
        MenuSelect();
        break;
      }
      sound_library_.SayConfirmSelection();
      break;
    case MENU_EXIT:
      sound_library_.SaySelect();
      MenuExit();
      break;
#endif
    default:
      break;
    }
  }

  void MenuDialIncrement(int direction) {
    switch (menu_type_) {
      case MENU_PRESET:
#ifdef FETT263_USE_SETTINGS_MENU
      case MENU_LENGTH:
#endif
#ifdef FETT263_EDIT_MODE_MENU
      case MENU_FONT:
      case MENU_TRACK:
      case MENU_IGNITION_TIME:
      case MENU_IGNITION_DELAY:
      case MENU_RETRACTION_TIME:
      case MENU_RETRACTION_DELAY:
      case MENU_IGNITION_OPTION2:
      case MENU_RETRACTION_OPTION2:
#endif
        direction *= 5;
        break;
      default:
        break;
    }
    MenuDial(direction);
  }

// Edit Mode Dial
  void MenuDial(int direction) {
    switch (menu_type_) {
      default:
        break;
      case MENU_PRESET:
        if (SaberBase::IsOn()) {
          if (direction > 0) {
            next_preset_fast();
          } else {
            previous_preset_fast();
          }
        } else {
          int num_presets = GetNumberOfPresets();
          int preset = (current_preset_.preset_num + num_presets * 10 + direction) % num_presets;
          SelectPreset(preset);
        }
        break;
      case MENU_VOLUME:
        ChangeVolume(direction);
        break;
      case MENU_TRACK_PLAYER:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        track_num_ += direction;
        if (track_num_ > num_tracks_) track_num_ = 1;
        if (track_num_ <= 0) track_num_ = num_tracks_;
        PlayTrack();
        break;
#ifdef FETT263_USE_SETTINGS_MENU
#if NUM_BLADES > 1
      case MENU_BLADE_LENGTH:
        blade_num_ += direction;
        if (blade_num_ > NUM_BLADES) blade_num_ = 1;
        if (blade_num_ <= 0) blade_num_ = NUM_BLADES;
        BladePreview(blade_num_);
        sound_library_.SayNumber(blade_num_, SAY_WHOLE);
        break;
#endif
      case MENU_LENGTH:
        blade_length_ += direction;
        if (blade_length_ >= max_length_) {
          sound_library_.SayMaximum();
          blade_length_ = max_length_;
        }
        if (blade_length_ <= 1) {
          sound_library_.SayMinimum();
          blade_length_ = 1;
        }
        length_edit_length = blade_length_ - 1;
        sound_library_.SayNumber(blade_length_, SAY_WHOLE);
        SetBladeLength(blade_num_, blade_length_);
        SaveState(current_preset_.preset_num);
        break;
      case MENU_GESTURE_SUB:
        gesture_num_ += direction;
        if (gesture_num_ <= 0) gesture_num_ = GESTURE_OPTIONS;
        if (gesture_num_ > GESTURE_OPTIONS) gesture_num_ = 1;
        switch (gesture_num_) {
          default:
            sound_library_.SaySelectOption();
            break;
          case GESTURE_SWINGON_IGNITION:
            sound_library_.SaySwingIgnition();
            break;
          case GESTURE_SWINGON_SPEED:
            sound_library_.SaySwingOnSpeed();
            break;
          case GESTURE_TWIST_IGNITION:
            sound_library_.SayTwistIgnition();
            break;
          case GESTURE_THRUST_IGNITION:
            sound_library_.SayThrustIgnition();
            break;
          case GESTURE_STAB_IGNITION:
            sound_library_.SayStabIgnition();
            break;
          case GESTURE_FORCE_PUSH:
            sound_library_.SayForcePush();
            break;
          case GESTURE_FORCE_PUSH_LENGTH:
            sound_library_.SayForcePushLength();
            break;
          case GESTURE_TWIST_RETRACTION:
            sound_library_.SayTwistRetraction();
            break;
          case GESTURE_LOCKUP_DELAY:
            sound_library_.SayLockupDelay();
            break;
          case GESTURE_CLASH_DETECTION:
            sound_library_.SayClashDetectionLevel();
            break;
          case GESTURE_POWER_LOCK:
            sound_library_.SayPowerLock();
            break;
          case GESTURE_MAX_CLASH:
            sound_library_.SayMaximumClashStrength();
            break;
        }
        break;
      case MENU_SWINGON:
      case MENU_TWISTON:
      case MENU_THRUSTON:
      case MENU_STABON:
      case MENU_FORCEPUSH:
      case MENU_TWISTOFF:
      case MENU_POWERLOCK:
        choice_ = direction > 0;
        sound_library_.SayBool(choice_);
        break;
      case MENU_SWINGON_SPEED:
        if (direction > 0) {
          if (calc_ < 600) {
            sound_library_.SayUp();
            calc_ += 50;
          }
          if (calc_ >= 600) {
            calc_ = 600;
            sound_library_.SayMaximum();
          }
        } else {
          if (calc_ > 200) {
            sound_library_.SayDown();
            calc_ -= 50;
          }
          if (calc_ <= 200) {
            calc_ = 200;
            sound_library_.SayMinimum();
          }
        }
        sound_library_.SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_FORCEPUSH_LENGTH:
      case MENU_CLASH_DETECT:
        if (direction > 0) {
          if (calc_ < 10) {
            sound_library_.SayUp();
            calc_ += 1;
          }
          if (calc_ >= 10) {
            calc_ = 10;
            sound_library_.SayMaximum();
          }
        } else {
          if (calc_ > 1) {
            sound_library_.SayDown();
            calc_ -= 1;
          }
          if (calc_ <= 1) {
            calc_ = 1;
            sound_library_.SayMinimum();
          }
        }
        sound_library_.SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_MAX_CLASH:
        if (direction > 0) {
          if (calc_ < 16) {
            sound_library_.SayUp();
            calc_ += 1;
          }
          if (calc_ >= 16) {
            calc_ = 16;
            sound_library_.SayMaximum();
          }
        } else {
          if (calc_ > GetCurrentClashThreshold()) {
            sound_library_.SayDown();
            calc_ -= 1;
          }
          if (calc_ <= GetCurrentClashThreshold()) {
            calc_ = GetCurrentClashThreshold();
            sound_library_.SayMinimum();
          }
        }
        sound_library_.SayNumber(calc_, SAY_WHOLE);
        break;
      case MENU_LOCKUP_DELAY:
        if (direction > 0) {
          if (calc_ < 1200) {
            sound_library_.SayUp();
            calc_ += 100;
          }
          if (calc_ >= 1200) {
            calc_ = 1200;
            sound_library_.SayMaximum();
          }
        } else {
          if (calc_ > 200) {
            sound_library_.SayDown();
            calc_ -= 100;
          }
          if (calc_ <= 200) {
            calc_ = 200;
            sound_library_.SayMinimum();
          }
        }
        sound_library_.SayNumber(calc_, SAY_WHOLE);
        sound_library_.SayMillis();
        break;
      case MENU_SETTING_SUB:
        menu_sub_pos_ += direction;
        if (menu_sub_pos_ <= 0) menu_sub_pos_ = SET_SUBMENUS;
        if (menu_sub_pos_ > SET_SUBMENUS) menu_sub_pos_ = 1;
        switch (menu_sub_pos_) {
          case EDIT_VOLUME:
            sound_library_.SayEditVolume();
            break;
          case EDIT_CONTROL_SETTINGS:
            sound_library_.SayEditControlSettings();
            break;
          case EDIT_CLASH_THRESHOLD:
            sound_library_.SayEditClashThreshold();
            break;
          case EDIT_BLADE_LENGTH:
            sound_library_.SayEditBladeLength();
            break;
          case EDIT_BRIGHTNESS:
            sound_library_.SayEditBrightness();
          default:
            break;
        }
        break;
      case MENU_CLASH_THRESHOLD:
        if (direction > 0) {
          if (clash_t_ >= saved_gesture_control.maxclash) {
            sound_library_.SayMaximum();
            clash_t_ = saved_gesture_control.maxclash;
          } else {
            sound_library_.SayUp();
            clash_t_ += 0.25;
            if (clash_t_ > saved_gesture_control.maxclash) clash_t_ = saved_gesture_control.maxclash;
          }
        } else {
          if (clash_t_ <= 1.0) {
            sound_library_.SayMinimum();
            clash_t_ = 1.0;
          } else {
            sound_library_.SayDown();
            clash_t_ -= 0.25;
            if (clash_t_ < 1.0) clash_t_ = 1.0;
          }
        }
        sound_library_.SayNumber(clash_t_, SAY_DECIMAL);
        break;
      case MENU_DIM_BLADE:
        if (direction > 0) {
          dim = std::min<float>(dim + 0.1, 1.0);
#ifdef FETT263_CIRCULAR_DIM_MENU
          if (dim >= 1.0) dim = 0.2;
          sound_library_.SayUp();
#else
          if (dim >= 1.0) {
            dim = 1.0;
            sound_library_.SayMaximum();
          } else {
            sound_library_.SayUp();
          }
#endif
        } else {
          dim = std::max<float>(dim - 0.1, 0.2);
#ifdef FETT263_CIRCULAR_DIM_MENU
          if (dim <= 0.2) dim = 1.0;
          sound_library_.SayDown();
#else
          if (dim <= 0.2) {
            dim = 0.2;
            sound_library_.SayMinimum();
          } else {
            sound_library_.SayDown();
          }
#endif
        }
        SaberBase::SetDimming(pow(dim, 2.2) * 16384);
        break;
#endif
#ifdef FETT263_EDIT_MODE_MENU
      case MENU_TOP:
        menu_top_pos_ += direction;
        if (menu_top_pos_ <= 0) menu_top_pos_ = SUBMENUS;
        if (menu_top_pos_ > SUBMENUS) menu_top_pos_ = 1;
        switch (menu_top_pos_) {
          case EDIT_STYLE:
            sound_library_.SayEditStyle();
            break;
          case EDIT_COLORS:
            sound_library_.SayEditColor();
            break;
          case EDIT_FONT:
            sound_library_.SayEditFont();
            break;
          case EDIT_TRACK:
            sound_library_.SayEditTrack();
            break;
          case EDIT_SETTINGS:
            sound_library_.SayEditSettings();
            break;
          case COPY_PRESET:
            sound_library_.SayCopyPreset();
            break;
          case DELETE_PRESET:
            sound_library_.SayDeletePreset();
            break;
          case EXIT_MENU:
            sound_library_.SayExit();
            break;
       }
       break;
  #if NUM_BLADES > 1
      case MENU_BLADE_STYLE:
      case MENU_BLADE_COLOR:
      case MENU_BLADE_COPY:
        blade_num_ += direction;
        if (blade_num_ > NUM_BLADES) blade_num_ = 1;
        if (blade_num_ <= 0) blade_num_ = NUM_BLADES;
        BladePreview(blade_num_);
        sound_library_.SayNumber(blade_num_, SAY_WHOLE);
        break;
      case MENU_COPY_COLOR:
#endif
      case MENU_RESET_COLOR:
      case MENU_COPY:
      case MENU_DELETE:
#ifdef FETT263_SAVE_CHOREOGRAPHY
      case MENU_REHEARSE:
#endif
        if (direction > 0) {
          choice_ = true;
          sound_library_.SayAccept();
        } else {
          choice_ = false;
          sound_library_.SayCancel();
        }
        break;
      case MENU_STYLE_SUB:
        menu_sub_pos_ += direction;
        if (menu_sub_pos_ <= 0) menu_sub_pos_ = 2;
        if (menu_sub_pos_ > 2) menu_sub_pos_ = 1;
	switch (menu_sub_pos_) {
	  case EDIT_STYLE_SELECT:
            sound_library_.SaySelectStyle();
	    break;
          case EDIT_STYLE_SETTINGS:
            sound_library_.SayEditStyleSettings();
            break;
	  default:
            sound_library_.SaySelectOption();
	    break;
	}
	break;
      case MENU_STYLE:
        ChangeStyleNumber(blade_num_, direction);
        current_preset_.Save();
        UpdateStyle();
        SayStyleNumber(GetStyleNumber(blade_num_));
        break;
      case MENU_COLOR:
        break;
      case MENU_COLOR_SUB:
        menu_sub_pos_ += direction;
  #if NUM_BLADES == 1
        if (menu_sub_pos_ <= 0) menu_sub_pos_ = 2;
        if (menu_sub_pos_ > 2) menu_sub_pos_ = 1;
        switch (menu_sub_pos_) {
          case EDIT_EFFECT_COLOR:
            sound_library_.SayColorOptions();
            break;
          case RESET_COLORS:
            sound_library_.SayResetColors();
            break;
        }
  #else
        if (menu_sub_pos_ <= 0) menu_sub_pos_ = 3;
        if (menu_sub_pos_ > 3) menu_sub_pos_ = 1;
        switch (menu_sub_pos_) {
          case EDIT_EFFECT_COLOR:
            sound_library_.SayColorOptions();
            break;
          case COPY_COLORS:
            sound_library_.SayCopyColors();
            break;
          case RESET_COLORS:
            sound_library_.SayResetColors();
            break;
        }
  #endif
        break;
      case MENU_EFFECT:
        for (int i = 0; i < (int)NELEM(rgb_arg_menu_); i++) {
          arg_dial_ += direction;
          if (arg_dial_ < 0) arg_dial_ = NELEM(rgb_arg_menu_) - 1;
          if (arg_dial_ >= (int)NELEM(rgb_arg_menu_)) arg_dial_ = 0;
          effect_num_ = rgb_arg_menu_[arg_dial_];
          if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), effect_num_ + 2)) break;
        }
        switch (effect_num_) {
          case BASE_COLOR_ARG:
            sound_library_.SayBaseColor();
            break;
          case ALT_COLOR_ARG:
            sound_library_.SayAltColor();
            break;
          case ALT_COLOR2_ARG:
            sound_library_.SayAltColor();
            sound_library_.SayNumber(2, SAY_WHOLE);
            break;
          case ALT_COLOR3_ARG:
            sound_library_.SayAltColor();
            sound_library_.SayNumber(3, SAY_WHOLE);
            break;
          case BLAST_COLOR_ARG:
            sound_library_.SayBlastColor();
            break;
          case CLASH_COLOR_ARG:
            sound_library_.SayClashColor();
            break;
          case LOCKUP_COLOR_ARG:
            sound_library_.SayLockupColor();
            break;
          case DRAG_COLOR_ARG:
            sound_library_.SayDragColor();
            break;
          case LB_COLOR_ARG:
            sound_library_.SayLightningBlockColor();
            break;
          case STAB_COLOR_ARG:
            sound_library_.SayStabColor();
            break;
          case PREON_COLOR_ARG:
            sound_library_.SayPreonColor();
            break;
          case IGNITION_COLOR_ARG:
            sound_library_.SayIgnitionColor();
            break;
          case RETRACTION_COLOR_ARG:
            sound_library_.SayRetractionColor();
            break;
          case POSTOFF_COLOR_ARG:
            sound_library_.SayPostOffColor();
            break;
          case SWING_COLOR_ARG:
            sound_library_.SaySwingColor();
            break;
          case EMITTER_COLOR_ARG:
            sound_library_.SayEmitterColor();
            break;
          case OFF_COLOR_ARG:
            sound_library_.SayOffColor();
            break;
        }
        break;
      case MENU_COLOR_MODE:
        sub_dial_ += direction;
        if (sub_dial_ < 0) sub_dial_ = 3;
        if (sub_dial_ > 3) sub_dial_ = 0;
        switch (sub_dial_) {
          case COLOR_LIST_MODE:
            sound_library_.SayColorList();
            break;
          case COLOR_HUE_MODE:
            sound_library_.SayAdjustColorHue();
            break;
          case WHITE_MODE:
            sound_library_.SayAdjustWhiteLevel();
            break;
          case BLACK_MODE:
            sound_library_.SayAdjustBlackLevel();
            break;
          default:
            sound_library_.SaySelectOption();
            break;
          }
          break;
      case MENU_COLOR_BASE:
      case MENU_COLOR_ALT:
      case MENU_COLOR2_ALT:
      case MENU_COLOR3_ALT:
      case MENU_COLOR_BLAST:
      case MENU_COLOR_CLASH:
      case MENU_COLOR_LOCKUP:
      case MENU_COLOR_DRAG:
      case MENU_COLOR_LB:
      case MENU_COLOR_STAB:
      case MENU_COLOR_PREON:
      case MENU_COLOR_IGNITE:
      case MENU_COLOR_RETRACT:
      case MENU_COLOR_PSTOFF:
      case MENU_COLOR_SWING:
      case MENU_COLOR_EMITTER:
      case MENU_COLOR_OFF:
        // Color List
        if (color_mode_ == COLOR_LIST) {
          dial_ += direction;
          if (dial_ < 0) dial_ = NELEM(color_list_) - 1;
          if (dial_ > (int)NELEM(color_list_) - 1) dial_ = 0;
          ShowColorStyle::SetColor(Color16(color_list_[dial_].color));
#ifdef FETT263_SAY_COLOR_LIST
          sound_library_.SayColor(color_list_[dial_].color_number);
#else
          if (direction > 0) {
            sound_library_.SayUp();
          } else {
	    sound_library_.SayDown();
          }
#endif
          break;
        }
        break;
      case MENU_FONT:
        if (!restore_point.get()) restore_point = std::move(current_preset_.font);
        ChangeFont(direction);
        break;
      case MENU_TRACK:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        if (!restore_point.get()) restore_point = std::move(current_preset_.track);
        track_num_ += direction;
        if (track_num_ > num_tracks_) track_num_ = 1;
        if (track_num_ <= 0) track_num_ = num_tracks_;
        char track[128];
        RunCommandAndGetSingleLine("list_current_tracks", nullptr, track_num_, track, sizeof(track));
        current_preset_.track = mkstr(track);
        current_preset_.Save();
        StartOrStopTrack();
        break;
      case MENU_STYLE_SETTING_SUB:
        for (int i = 0; i < (int)NELEM(int_arg_menu_); i++) {
          arg_dial_ += direction;
          if (direction > 0) {
            // Detect preon.wav for setting preview (cannot be shown without so skip over)
            if (!SFX_preon && (int_arg_menu_[arg_dial_] == PREON_OPTION_ARG || int_arg_menu_[arg_dial_] == PREON_SIZE_ARG)) arg_dial_ = 0;
            if (arg_dial_ > (int)NELEM(int_arg_menu_) - 1) arg_dial_ = 0;
          } else {
            // Detect preon.wav for setting preview (cannot be shown without so skip over)            
            if (!SFX_preon && (int_arg_menu_[arg_dial_] == PREON_OPTION_ARG || int_arg_menu_[arg_dial_] == PREON_SIZE_ARG)) arg_dial_ = NELEM(int_arg_menu_) - 3;
            if (arg_dial_ < 0) arg_dial_ = NELEM(int_arg_menu_) - 1;
          }
          set_num_ = int_arg_menu_[arg_dial_];
          if (style_parser.UsesArgument(current_preset_.GetStyle(blade_num_), set_num_ + 2)) break;
        }
        switch (set_num_) {
          case STYLE_OPTION_ARG:
	    sound_library_.SayStyleOptions();
            break;
          case STYLE_OPTION2_ARG:
            sound_library_.SayStyleOptions();
            sound_library_.SayNumber(2, SAY_WHOLE);
            break;
          case STYLE_OPTION3_ARG:
            sound_library_.SayStyleOptions();
            sound_library_.SayNumber(3, SAY_WHOLE);
            break;
          case IGNITION_OPTION_ARG:
	    sound_library_.SayIgnitionOptions();
            break;
          case IGNITION_OPTION2_ARG:
            sound_library_.SayIgnitionOptions();
            sound_library_.SayNumber(2, SAY_WHOLE);
            break;
          case IGNITION_TIME_ARG:
	    sound_library_.SayIgnitionTime();
            break;
          case IGNITION_POWER_UP_ARG:
            sound_library_.SayPowerUpOptions();
            break;
          case IGNITION_DELAY_ARG:
            sound_library_.SayIgnitionDelay();
            break;
          case RETRACTION_OPTION_ARG:
	    sound_library_.SayRetractionOptions();
            break;
          case RETRACTION_OPTION2_ARG:
            sound_library_.SayRetractionOptions();
            sound_library_.SayNumber(2, SAY_WHOLE);
            break;
          case RETRACTION_TIME_ARG:
	    sound_library_.SayRetractionTime();
            break;
          case RETRACTION_COOL_DOWN_ARG:
            sound_library_.SayCoolDownOptions();
            break;
          case RETRACTION_DELAY_ARG:
            sound_library_.SayRetractionDelay();
            break;
          case LOCKUP_POSITION_ARG:
	    sound_library_.SayClashLockupPosition();
            break;
          case DRAG_SIZE_ARG:
	    sound_library_.SayDragSize();
            break;
          case MELT_SIZE_ARG:
            sound_library_.SayMeltSize();
            break;
          case SWING_OPTION_ARG:
            sound_library_.SaySwingOption();
            break;
          case EMITTER_SIZE_ARG:
	    sound_library_.SayEmitterSize();
            break;
          case OFF_OPTION_ARG:
            sound_library_.SayOffOption();
            break;
          case PREON_OPTION_ARG:
	    sound_library_.SayPreonOptions();
            break;
          case PREON_SIZE_ARG:
	    sound_library_.SayPreonSize();
            break;
          default:
            sound_library_.SaySelectOption();
            break;
        }
        break;
      case MENU_IGNITION_TIME:
      case MENU_RETRACTION_TIME:
      case MENU_IGNITION_DELAY:
      case MENU_RETRACTION_DELAY:
        if (SaberBase::IsOn()) {
          if (direction > 0) {
            calc_ += 100;
            SetInOut();
          } else {
            if (calc_ >= 100) {
              calc_ -= 100;
              if (calc_ >= 100) {
                SetInOut();
	        next_event_ = true;
	        break;
              } 
            } else {
              calc_ = 0;
              if (menu_type_ == MENU_IGNITION_DELAY || menu_type_ == MENU_RETRACTION_DELAY) {
                sound_library_.SayMinimum();
              } else {
                sound_library_.SayAuto();
              }
              next_event_ = true;
            }
          }
	}
        break;
      case MENU_STYLE_OPTION:
      case MENU_STYLE_OPTION2:
      case MENU_STYLE_OPTION3:
      case MENU_IGNITION_OPTION:
      case MENU_IGNITION_POWER_UP_OPTION:
      case MENU_RETRACTION_OPTION:
      case MENU_RETRACTION_COOL_DOWN_OPTION:
      case MENU_SWING_OPTION:
      case MENU_OFF_OPTION:
      case MENU_PREON_OPTION:
        if (SaberBase::IsOn()) {
          calc_ += direction;
          if (calc_ > 32768) calc_ = 0;
          if (calc_ < 0) {
            calc_ = 0;
            sound_library_.SayMinimum();
            break;
          }
          SetInOut();
          sound_library_.SayOption();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
	}
        break;
      case MENU_IGNITION_OPTION2:
      case MENU_RETRACTION_OPTION2:
        if (SaberBase::IsOn()) {
          calc_ += (direction * 1000);
          calc_ &= 0x7fff;
          SetInOut();
          sound_library_.SayNumber(calc_, SAY_WHOLE);
        }
        break;
      case MENU_LOCKUP_POSITION:
      case MENU_EMITTER_SIZE:
      case MENU_PREON_SIZE:
        if (direction > 0) {
          calc_ += 1000;
          sound_library_.SayUp();
        } else {
          if (calc_ > 0) {
            calc_ -= 1000;
            sound_library_.SayDown();
          } else {
            if (calc_ < 1000) {
              calc_ = 0;
              sound_library_.SayMinimum();
            }
          }
        }
        IntEdit::SetIntValue(calc_);
        break;
      case MENU_DRAG_SIZE:
      case MENU_MELT_SIZE:
        if (direction > 0) {
          if (calc_ > 1000) {
            calc_ -= 1000;
            sound_library_.SayUp();
          } else {
            if (calc_ < 1000) {
              calc_ = 0;
              sound_library_.SayMaximum();
            }
          }
        } else {
          if (calc_ < 32768) {
            calc_ += 1000;
            sound_library_.SayDown();
          } else {
            if (calc_ >= 32768) {
              calc_ = 32768;
              sound_library_.SayMinimum();
            }
          }
        }
        IntEdit::SetIntValue(calc_);
        break;
  #endif
      }
    }

// Edit Mode Undo (AUX Button)
  void MenuUndo() {
    switch (menu_type_) {
      case MENU_TOP:
	sound_library_.SayExit();
        MenuExit();
        break;
#ifdef FETT263_USE_SETTINGS_MENU
#if NUM_BLADES > 1
      case MENU_BLADE_LENGTH:
        menu_type_ = MENU_SETTING_SUB;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
#endif
      case MENU_LENGTH:
        menu_type_ = MENU_SETTING_SUB;
        SetBladeLength(blade_num_, length_revert_);
        SaveState(current_preset_.preset_num);
        MenuRevert();
        show_length_.Stop(blade_num_);
        UpdateStyle();
        break;
      case MENU_GESTURE_SUB:
        menu_type_ = MENU_SETTING_SUB;
        MenuCancel();
        break;
      case MENU_SWINGON:
      case MENU_SWINGON_SPEED:
      case MENU_TWISTON:
      case MENU_THRUSTON:
      case MENU_STABON:
      case MENU_FORCEPUSH:
      case MENU_FORCEPUSH_LENGTH:
      case MENU_TWISTOFF:
      case MENU_LOCKUP_DELAY:
      case MENU_CLASH_DETECT:
      case MENU_POWERLOCK:
      case MENU_MAX_CLASH:
        menu_type_ = MENU_GESTURE_SUB;
        MenuCancel();
        break;
      case MENU_CLASH_THRESHOLD:
        menu_type_ = MENU_SETTING_SUB;
        MenuCancel();
        break;
      case MENU_DIM_BLADE:
        SaberBase::SetDimming(pow(dim_revert_, 2.5) * 16384);
        UpdateStyle();
        menu_type_ = MENU_SETTING_SUB;
        SaveGlobalState();
        MenuRevert();
        break;
      case MENU_VOLUME:
        menu_type_ = MENU_SETTING_SUB;
        dynamic_mixer.set_volume(VOLUME);
        MenuRevert();
        break;
      case MENU_SETTING_SUB:
#ifdef FETT263_EDIT_SETTINGS_MENU
        MenuExit();
#else
        menu_type_ = MENU_TOP;
        MenuCancel();
#endif
        break;
#endif
#ifdef FETT263_EDIT_MODE_MENU
#if NUM_BLADES > 1
      case MENU_BLADE_STYLE:
        menu_type_ = MENU_TOP;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
      case MENU_BLADE_COLOR:
        menu_type_ = MENU_TOP;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
      case MENU_BLADE_COPY:
        menu_type_ = MENU_COLOR_SUB;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
      case MENU_COPY_COLOR:
        menu_type_ = MENU_COLOR_SUB;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        MenuCancel();
        break;
#endif
      case MENU_STYLE_SUB:
#if NUM_BLADES == 1
        menu_type_ = MENU_TOP;
#else
        menu_type_ = MENU_BLADE_STYLE;
	blade_num_ = 0;
#endif
	MenuCancel();
	break;
      case MENU_STYLE: {
        char style_arg[10];
        itoa(style_revert_, style_arg, 10);
        current_preset_.SetStyle(blade_num_,style_parser.SetArgument(current_preset_.GetStyle(blade_num_), 1, style_arg));
        current_preset_.Save();
        UpdateStyle();
#if NUM_BLADES == 1
        menu_type_ = MENU_STYLE_SUB;
        MenuRevert();
#else
        menu_type_ = MENU_BLADE_STYLE;
        MenuRevert();
	blade_num_ = 0;
#endif
        break;
      }
      case MENU_COLOR:
        menu_type_ = MENU_TOP;
#ifndef DISABLE_COLOR_CHANGE
        SaberBase::SetVariation(variation_revert_);
        ToggleColorChangeMode();
#endif
        MenuCancel();
        break;
      case MENU_COLOR_SUB:
#if NUM_BLADES == 1
        menu_type_ = MENU_TOP;
        MenuCancel();
        break;
#else
        menu_type_ = MENU_BLADE_COLOR;
        show_preview_.Stop(blade_preview_);
        blade_preview_ = 0;
        blade_num_ = 0;
        MenuCancel();
        break;
#endif
      case MENU_COLOR_MODE:
        edit_color_ = false;
        menu_type_ = MENU_EFFECT;
        MenuCancel();
        break;
      case MENU_EFFECT:
        edit_color_ = false;
        menu_type_ = MENU_COLOR_SUB;
        MenuCancel();
        break;
      case MENU_COLOR_BASE:
      case MENU_COLOR_ALT:
      case MENU_COLOR2_ALT:
      case MENU_COLOR3_ALT:
      case MENU_COLOR_IGNITE:
      case MENU_COLOR_RETRACT:
      case MENU_COLOR_SWING:
      case MENU_COLOR_OFF:
        RevertColorEdit();
        show_color_.Stop(blade_num_);
        MenuRevert();
        break;
      case MENU_COLOR_BLAST:
      case MENU_COLOR_CLASH:
      case MENU_COLOR_LOCKUP:
      case MENU_COLOR_LB:
        RevertColorEdit();
        bump_color_.Stop(blade_num_);
        MenuRevert();
        break;
      case MENU_COLOR_DRAG:
      case MENU_COLOR_STAB:
        RevertColorEdit();
        tip_color_.Stop(blade_num_);
        MenuRevert();
        break;
      case MENU_COLOR_PREON:
      case MENU_COLOR_PSTOFF:
      case MENU_COLOR_EMITTER:
        RevertColorEdit();
        hilt_color_.Stop(blade_num_);
        MenuRevert();
        break;
      case MENU_FONT:
        if (restore_point.get()) current_preset_.font = std::move(restore_point);
        restore_point = nullptr;
        current_preset_.Save();
	// Reload Font Fast
        SetPresetFast(current_preset_.preset_num);
        menu_type_ = MENU_TOP;
        MenuRevert();
        break;
      case MENU_TRACK:
        if (track_player_) {
          track_player_->Stop();
          track_player_.Free();
        }
        if (restore_point.get()) current_preset_.track = std::move(restore_point);
        restore_point = nullptr;
        current_preset_.Save();
        menu_type_ = MENU_TOP;
        MenuRevert();
        break;
      case MENU_COPY:
        choice_ = false;
        menu_type_ = MENU_TOP;
        MenuCancel();
        break;
      case MENU_RESET_COLOR:
        menu_type_ = MENU_COLOR_SUB;
        MenuCancel();
        break;
      case MENU_STYLE_SETTING_SUB:
        menu_type_ = MENU_STYLE_SUB;
        MenuCancel();
        break;
      case MENU_STYLE_OPTION:
      case MENU_STYLE_OPTION2:
      case MENU_STYLE_OPTION3:
      case MENU_IGNITION_OPTION:
      case MENU_IGNITION_OPTION2:
      case MENU_IGNITION_TIME:
      case MENU_IGNITION_POWER_UP_OPTION:
      case MENU_IGNITION_DELAY:
      case MENU_RETRACTION_OPTION:
      case MENU_RETRACTION_OPTION2:
      case MENU_RETRACTION_TIME:
      case MENU_RETRACTION_COOL_DOWN_OPTION:
      case MENU_RETRACTION_DELAY:
      case MENU_SWING_OPTION:
      case MENU_OFF_OPTION:
      case MENU_PREON_OPTION:
      case MENU_PREON_SIZE:
        menu_type_ = MENU_STYLE_SETTING_SUB;
        RevertInOut();
        MenuRevert();
        break;
      case MENU_LOCKUP_POSITION:
      case MENU_DRAG_SIZE:
      case MENU_MELT_SIZE:
      case MENU_EMITTER_SIZE:
        CancelStyleSetting();
        break;
      case MENU_DELETE:
      case MENU_EXIT:
      default:
        choice_ = false;
        menu_type_ = MENU_TOP;
        MenuCancel();
        break;
#endif
    }
  }

  // Exit Edit Mode
  void MenuExit() {
    switch (menu_type_) {
#ifdef FETT263_SAVE_CHOREOGRAPHY
      case MENU_REHEARSE:
	sound_library_.SayKeepRehearsal();
        break;
#endif
      case MENU_PRESET:
        first_preset();
	sound_library_.SaySelect();
        break;
      case MENU_VOLUME:
        if (SFX_vmend) {
	  sound_library_.SayVolumeMenuEnd();
        } else {
	  sound_library_.SayExit();
        }
        break;
#ifdef FETT263_EDIT_SETTINGS_MENU
      case MENU_SETTING_SUB:
        sound_library_.SayExit();
        break;
#endif
      default:
        break;
    }
    menu_type_ = MENU_TOP;
    menu_ = false;
    wav_player.Free();
  }

  void MenuSave() {
    sound_library_.SaySave();
    MenuPrompt();
  }

  void MenuSelect() {
    sound_library_.SaySelect();
    MenuPrompt();
  }

  void MenuConfirm() {
    sound_library_.SayConfirm();
    MenuPrompt();
  }

  void MenuCancel() {
    sound_library_.SayCancel();
    MenuPrompt();
  }

  void MenuRevert() {
    sound_library_.SayRevert();
    MenuPrompt();
  }

  void MenuPrompt() {
    switch (menu_type_) {
      case MENU_TOP:
	sound_library_.SayMainMenu();
        break;
#ifdef FETT263_EDIT_SETTINGS_MENU
#if NUM_BLADES > 1
      case MENU_BLADE_LENGTH:
        sound_library_.SaySelectBlade();
        break;
#endif
      case MENU_GESTURE_SUB:
        sound_library_.SayGestureMenu();
        break;
      case MENU_SETTING_SUB:
        sound_library_.SaySettingsMenu();
        break;
#endif
#ifdef FETT263_EDIT_MODE_MENU
      case MENU_STYLE_SUB:
        sound_library_.SayStyleMenu();
        break;
      case MENU_COLOR_SUB:
	sound_library_.SayColorMenu();
	break;
      case MENU_COLOR_MODE:
        sound_library_.SayColorOptions();
        break;
#if NUM_BLADES > 1
      case MENU_BLADE_COLOR:
      case MENU_BLADE_STYLE:
      case MENU_BLADE_LENGTH:
	sound_library_.SaySelectBlade();
        break;
#endif
      case MENU_GESTURE_SUB:
	sound_library_.SayGestureMenu();
        break;
      case MENU_SETTING_SUB:
	sound_library_.SaySettingsMenu();
        break;
      case MENU_EFFECT:
	sound_library_.SaySelectEffect();
        break;
      case MENU_STYLE_SETTING_SUB:
	sound_library_.SaySelectOption();
        break;
#endif
      default:
        break;
    }
  }

  enum EditColorMode {
    NONE,
    CC_COLOR_LIST,
    CC_ZOOM_COLOR,
    COLOR_LIST,
    EDIT_COLOR,
    EDIT_BLACK,
    EDIT_WHITE,
    ZOOM_COLOR,
  };

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
    if (!strcmp(cmd, "get_gesture") && arg) {
      saved_gesture_control.Print(arg);
      return true;
    }
    if (!strcmp(cmd, "set_gesture") && arg) {
      saved_gesture_control.Set(arg);
      SaveGestureState();
      return true;
    }
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "left") || !strcmp(cmd, "l")) {
      MenuDial(-1);
      return true;
    }
    if (!strcmp(cmd, "right") || !strcmp(cmd, "r")) {
      MenuDial(1);
      return true;
    }
#endif    
    return false;
  }

  void ToggleBattleMode() {
    if (!battle_mode_) {
      battle_mode_ = true;
      if (SFX_bmbegin) {
        hybrid_font.PlayCommon(&SFX_bmbegin);
      } else {
        hybrid_font.DoEffect(EFFECT_FORCE, 0);
      }
    } else {
      battle_mode_ = false;
      if (SFX_bmend) {
        hybrid_font.PlayCommon(&SFX_bmend);
      } else {
        beeper.Beep(0.5, 3000);
      }
    }	  
  }

  void ToggleMultiBlast() {
    swing_blast_ = !swing_blast_;
    if (swing_blast_) {
      if (SFX_blstbgn) {
        hybrid_font.PlayCommon(&SFX_blstbgn);
      } else {
        hybrid_font.SB_Effect(EFFECT_BLAST, 0);
      }
    } else {
      if (SFX_blstend) {
        hybrid_font.PlayCommon(&SFX_blstend);
      } else {
        hybrid_font.SB_Effect(EFFECT_BLAST, 0);
      }
    }
  }

  void ToggleBattleModeMultiBlast() {
    if (battle_mode_ && !swing_blast_) {
      check_blast_ = true;
      last_blast_millis_ = millis();
    }
    swing_blast_ = false;
  }

  void DoAutoMultiBlast() {
    if (check_blast_) {
      if (millis() - last_blast_millis_ < 2000) {
        swing_blast_ = true;
        SaberBase::DoBlast();
      }
      check_blast_ = false;
    }	  
  }
	
  void DoIgnition() {
    wav_player.Free();
#ifdef FETT263_DUAL_MODE_SOUND
    SelectIgnitionSound();
#endif
    if (SFX_preon) {
#ifdef FETT263_DUAL_MODE_SOUND
    SelectPreonSound();
#endif
      On();
    } else {
      FastOn();
    }
#ifdef FETT263_BATTLE_MODE_ALWAYS_ON
    battle_mode_ = true;
#endif
#ifdef FETT263_BATTLE_MODE_START_ON
    battle_mode_ = true;
#endif
  }
	
#ifdef FETT263_DUAL_MODE_SOUND
  // Select preon wav based on blade orientation if more than 1 file, up = odd, down = even
  void SelectPreonSound() {
    SelectSFXEvenOdd(&SFX_preon);
  }

  // Select ignition wav based on blade orientation, up = odd, down = even
  void SelectIgnitionSound() {
    Effect* effect;
    if (SFX_poweron) {
      effect = &SFX_poweron;
    } else {
      effect = &SFX_out;
    }
    SelectSFXEvenOdd(effect);
  }

  // Select retraction wav based on blade orientation, up = odd, down = even
  // Do not mix poweroff.wav and pwroff.wav files in font or selection will not work
  void SelectRetractionSound() {
    Effect* effect;
    if (SFX_poweroff) {
      effect = &SFX_poweroff;
    } else if (SFX_pwroff) {
      effect = &SFX_pwroff;
    } else {
      effect = &SFX_in;
    }
    SelectSFXEvenOdd(effect);
    if (SFX_pstoff) SelectSFXEvenOdd(&SFX_pstoff);
  }

  // Select wav file based on blade orientation, up = odd, down = even
  void SelectSFXEvenOdd(Effect* effect) {
    int f = effect->files_found();
    if (f > 1) {
      if (fusor.angle1() > 0) {
        f = (rand() % ((f + 1)/ 2)) * 2;
      } else {
        f = 1 + (rand() % (f / 2)) * 2;
      }
      effect->Select(f);
    }
  }
#endif

  void CheckQuote() {
#ifndef FETT263_DISABLE_QUOTE_PLAYER
    if (SFX_quote) {      
      if (fusor.angle1() < - M_PI / 3)  {
        force_quote_ = !force_quote_;
      }
      ForceQuote();
    } else {
      SaberBase::DoForce();  
    }
#else
    SaberBase::DoEffect(EFFECT_FORCE, 0);
#endif
  }

  void ForceQuote() {
    if (force_quote_) {
#ifndef FETT263_RANDOMIZE_QUOTE_PLAYER
      SFX_quote.SelectNext();
#endif
      SaberBase::DoEffect(EFFECT_QUOTE, 0);
    } else {
      SaberBase::DoForce();
    }
  }

  // Battery Level
  void DoBattery() {
#if defined (FETT263_SAY_BATTERY_PERCENT) && defined (FETT263_SAY_BATTERY_VOLTS)
    if (fusor.angle1() < - M_PI / 3) {
      sound_library_.SayBatteryVolts();
    } else {
      sound_library_.SayBatteryPercent();
    }
#elif defined(FETT263_SAY_BATTERY_PERCENT)
    sound_library_.SayBatteryPercent();
#elif defined(FETT263_SAY_BATTERY_VOLTS)
    sound_library_.SayBatteryVolts();
#else
    hybrid_font.PlayCommon(&SFX_battery);
#endif
  }

  // Go to first Preset.
  void first_preset() {
#ifdef SAVE_PRESET
    SaveState(0);
#endif
    SetPreset(0, true);
  }

  void ChangeVolume(int v) {
    float current_volume = dynamic_mixer.get_volume();
    float volume = current_volume + (VOLUME * (v * 0.1));
#ifdef FETT263_CIRCULAR_VOLUME_MENU
    if (volume > VOLUME) volume = VOLUME * 0.1;
    if (volume < (VOLUME * 0.1)) volume = VOLUME;
    if (volume > current_volume) {
      STDOUT.println("Volume up");
      sound_library_.SayVolumeUp();      
    } else {
      STDOUT.println("Volume Down");
      sound_library_.SayVolumeDown();
    }
#else
    if (volume > VOLUME) {
      volume = VOLUME;
      sound_library_.SayMaximumVolume();
    } else if (volume < (VOLUME * 0.1)) {
      volume = VOLUME * 0.1;
      sound_library_.SayMininumVolume();
    } else {
      if (v > 0) {
        STDOUT.println("Volume up");
        sound_library_.SayVolumeUp();      
      } else {
        STDOUT.println("Volume Down");
        sound_library_.SayVolumeDown();
      }
    }
#endif
    STDOUT.print("Current Volume: ");
    STDOUT.println(dynamic_mixer.get_volume());
    dynamic_mixer.set_volume(volume);
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
  }

  RefPtr<BufferedWavPlayer> wav_player;

  float current_menu_angle_ = 0.0;
  float current_twist_angle_ = 0.0;
  float clash_t_ = 2.00;
#ifdef FETT263_USE_SETTINGS_MENU
  int blade_length_;
  int max_length_;
  float dim;
  float dim_revert_;
#endif
#ifdef FETT263_EDIT_MODE_MENU
  LSPtr<char> restore_point;
#endif

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {

#ifdef BLADE_DETECT_PIN
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_ON, MODE_ANY_BUTTON | MODE_OFF):
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = true;
#ifdef FETT263_SAVE_GESTURE_OFF
        RestoreGestureState();
#else
        saved_gesture_control.gestureon = true;
#endif
        FindBladeAgain();
        SaberBase::DoBladeDetect(true);
        return true;

      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_BLADE_DETECT, EVENT_LATCH_OFF, MODE_ANY_BUTTON | MODE_OFF):
        // Might need to do something cleaner, but let's try this for now.
        blade_detected_ = false;
#ifdef FETT263_SAVE_GESTURE_OFF
        SaveGestureState();
#endif
        saved_gesture_control.gestureon = false;
        FindBladeAgain();
        SaberBase::DoBladeDetect(false);
        return true;
#endif

      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_OFF):
        if (menu_ && menu_type_ == MENU_TRACK_PLAYER) {
          menu_ = false;
          menu_type_ = MENU_TOP;
          sound_library_.SayUp();
          return true;
        }
        return false;

#ifndef FETT263_MOTION_WAKE_POWER_BUTTON
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
        SaberBase::RequestMotion();
        saber_off_time_millis_ = millis();
        return true;
#endif

#if NUM_BUTTONS == 1
// 1 Button Specific (based on SA22C's prop)

      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
          if (menu_) {
            MenuChoice();
            return true;
#ifdef FETT263_MOTION_WAKE_POWER_BUTTON
          } else if (!SaberBase::MotionRequested()) {
            SaberBase::RequestMotion();
            saber_off_time_millis_ = millis();
            if (SFX_boot) {
              hybrid_font.PlayPolyphonic(&SFX_boot);
            } else {
              sound_library_.SayUp();
            }
            return true;
#endif
          } else {
            DoInteractivePreon();
          }
          return true;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        if (menu_) {
          if (menu_type_ == MENU_PRESET) {
            first_preset();
            MenuExit();
            return true;
          }
          MenuUndo();
        } else {
          if (fusor.angle1() < - M_PI / 3) {
            previous_preset();
          } else {
            next_preset();
          }
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF):
        if (menu_) {
          switch (menu_type_) {
            case MENU_TRACK_PLAYER:
              track_mode_ = PLAYBACK_RANDOM;
#ifndef FETT263_TRACK_PLAYER_NO_PROMPTS
              sound_library_.SayRandom();
#endif
              MenuExit();
              break;
            case MENU_PRESET:
              MenuChoice();
              FastOn();
              break;
            default:
              break;
          }
        } else {
          StartMenu(MENU_PRESET);
          sound_library_.SaySelectPreset();
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_LONG, MODE_OFF):
#if defined(FETT263_EDIT_MODE_MENU) || defined(FETT263_EDIT_SETTINGS_MENU)
        // Enter Edit Mode
        if (!menu_) {
          StartEditMode();
        }
#else
        // If Edit Mode Menu Disabled Save .ini files for SD card editing
        if (menu_) return true;
        GenerateIniFiles();
        sound_library_.SaySave();
#endif
        return true;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_OFF):
        if (menu_) return true;
        if (SetMute(true)) {
          unmute_on_deactivation_ = true;
          FastOn();
        }
        return false;

      case EVENTID(BUTTON_POWER, EVENT_THIRD_CLICK_LONG, MODE_OFF):
        if (menu_) return true;      
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

#ifdef FETT263_SAVE_CHOREOGRAPHY
      // Rehearsal Mode
      case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD_LONG, MODE_OFF):
        if (menu_) return true;
        // Check for existing rehearsal and prompt to overwrite or keep via menu
        if (saved_choreography.isIntialized()) {
          sound_library_.SayRehearseNew();
          StartMenu(MENU_REHEARSE);
          return true;
        } else {
          BeginRehearsal();
        }        
        return true;

      // Choreographed Battle Mode
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_POWER):
        if (menu_) return true;
        BeginChoreo();
        return true;
#endif

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON):
        if (CancelShowColor()) return true;
        if (menu_) {
          MenuUndo();
          return true;
        }
        CheckQuote();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_PRESSED, MODE_ON):
        if (DoColorZoom()) {
          sound_library_.SayZoomingIn();
          return true;
        }
        return false;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_RELEASED, MODE_ON):
        if (EndColorZoom()) {
          return true;
        }
        return false;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_ON):
        if (CancelShowColor()) return true;
        if (menu_) {
#ifdef FETT263_EDIT_SETTINGS_MENU
          if (menu_type_ == MENU_SETTING_SUB) {
#else
          if (menu_type_ == MENU_TOP) {
#endif
            sound_library_.SayExit();
            menu_ = false;
          } else {
            menu_type_ = MENU_TOP;
            MenuCancel();
          }
          return true;
        }
        if (!SaberBase::Lockup()) {
#ifndef DISABLE_COLOR_CHANGE
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            // Just exit color change mode.
            // Don't turn saber off.
            if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_ZOOMED) {
              ToggleColorChangeMode();
            }
            return true;
          }
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          EndRehearsal();
          return true;
        }
        if (choreo_) {
          EndChoreo();
          return true;
        }
#endif
          Off();
          saber_off_time_millis_ = millis();
          battle_mode_ = false;
          swing_blast_ = false;
  }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD_MEDIUM, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          sound_library_.SayCancel();
          rehearse_ = false;
          return true;
        }
#endif
#ifndef FETT263_DISABLE_BM_TOGGLE
        ToggleBattleMode();
#endif
        return true;

#ifdef FETT263_MULTI_PHASE
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
        if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_DUAL_MODE_SOUND
        SelectIgnitionSound();
#endif
        if (fusor.angle1() < - M_PI / 3) {
          previous_preset_fast();
        } else {
          next_preset_fast();
        }
        return true;
#endif

      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
#ifndef DISABLE_COLOR_CHANGE
        if (EndColorZoom()) return true;
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          ToggleColorChangeMode();
          return true;
        }
#endif      
        if (menu_) {
          MenuChoice();
          return true;
        }
        DoInteractiveBlast();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        if (track_player_) {
          StopTrackPlayer();
          return true;
        }
        DoInteractiveBlast();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        DoInteractiveBlast();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_FOURTH_CLICK_SHORT, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        if (fusor.angle1() > M_PI / 3) {
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
        } else {
#ifndef DISABLE_COLOR_CHANGE
          ToggleCCMode();
#endif
#ifdef DISABLE_COLOR_CHANGE
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
#endif
        }
        return true;

#ifndef FETT263_DISABLE_CHANGE_STYLE
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_CLICK_LONG, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        ChangeStyleNumberAllBlades(fusor.angle1() < - M_PI / 3 ? -1 : 1);
        hybrid_font.PlayCommon(&SFX_ccchange);
        return true;
#endif

#ifndef FETT263_DISABLE_CHANGE_FONT
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_CLICK_LONG, MODE_OFF):
        if (menu_) return true;
        ChangeFont(fusor.angle1() < - M_PI / 3 ? -1 : 1);
        return true;
#endif

      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
        CheckQuote();
        return true;

#ifndef FETT263_DISABLE_COPY_PRESET
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_LONG, MODE_OFF):
        if (menu_) return true;
        CopyPreset(true);
        return true;
#endif

#if !defined(FETT263_DISABLE_MULTI_BLAST) && !defined(FETT263_DISABLE_MULTI_BLAST_TOGGLE)
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_POWER):
        if (menu_ || CheckShowColorCC()) return true;
        ToggleMultiBlast();
        return true;
#endif

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
        if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_SPIN_MODE
        if (spin_mode_) return true;
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          RehearseLockup();
        }
#endif
        if (!SaberBase::Lockup()) {
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
        swing_blast_ = false;
        SaberBase::DoBeginLockup();
        return true;
        break;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
        if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_SPIN_MODE
        if (spin_mode_) return true;
#endif
        if (!SaberBase::Lockup()) {
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
          check_blast_ = false;
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        return true;
        break;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        if (menu_) return true;
#ifdef ENABLE_AUDIO
        if (track_player_) {
          StopTrackPlayer();
          track_mode_ = PLAYBACK_OFF;
          return true;
        } else {
          if (fusor.angle1() > M_PI / 3) {
            StartOrStopTrack();
            return true;
          } else {
            StartTrackPlayer();
            return true;
          }
        }
#else
        STDOUT.println("Audio disabled.");
#endif
        return true;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
        if (!menu_) {
          if (fusor.angle1() < - M_PI / 4) {
            if (!saved_gesture_control.gestureon) {
              saved_gesture_control.gestureon = true;
              sound_library_.SayGesturesOn();
            } else {
              saved_gesture_control.gestureon = false;
              sound_library_.SayGesturesOff();
            }
          } else {
            StartMenu(MENU_VOLUME);
            if (SFX_vmbegin) {
              sound_library_.SayEnterVolumeMenu();
            } else {
              sound_library_.SayEditVolume();
            }
          }
        }
        return true;

#else
// 2 Button Controls
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_OFF):
        SaberBase::RequestMotion();
        saber_off_time_millis_ = millis();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_ON, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
        if (menu_) {
          MenuChoice();
          return true;
#ifdef FETT263_MOTION_WAKE_POWER_BUTTON
        } else if (!SaberBase::MotionRequested()) {
          SaberBase::RequestMotion();
          saber_off_time_millis_ = millis();
          if (SFX_boot) {
            hybrid_font.PlayPolyphonic(&SFX_boot);
          } else {
            sound_library_.SayUp();
          }
          return true;
#endif
        } else {
          DoInteractivePreon();
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
        if (!menu_) {
          if (!saved_gesture_control.gestureon) {
            saved_gesture_control.gestureon = true;
            sound_library_.SayGesturesOn();
          } else {
            saved_gesture_control.gestureon = false;
            sound_library_.SayGesturesOff();
          }
        }
        return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_POWER):
        if (!menu_) {
	  StartMenu(MENU_VOLUME);
          if (SFX_vmbegin) {
            sound_library_.SayEnterVolumeMenu();
          } else {
            sound_library_.SayEditVolume();
          }
        }
        return true;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_OFF):
        if (menu_) return true;
        StartMenu(MENU_PRESET);
        sound_library_.SaySelectPreset();
        return true;

#ifndef FETT263_DISABLE_COPY_PRESET
      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_OFF | BUTTON_POWER):
        if (menu_) return true;
        CopyPreset(true);
        return true;
#endif

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF):
        // Track Player - Play Random
        if (menu_) {
          if (menu_type_ == MENU_TRACK_PLAYER) {
            track_mode_ = PLAYBACK_RANDOM;
#ifndef FETT263_TRACK_PLAYER_NO_PROMPTS
            sound_library_.SayRandom();
#endif
          }
          MenuExit();
          return true;
        } else {
          if (fusor.angle1() < - M_PI / 4) {
            previous_preset();
          } else {
            next_preset();
          }
        }
        return true;

        // Handle double-click with preon
      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_OFF):
        if (menu_) return true;
        if (on_pending_) {
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
          }
          return true;
        }
        return false;

      case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        if (millis() - activated_ < 500) {
          if (SetMute(true)) {
            unmute_on_deactivation_ = true;
          }
        }
        return true;

      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF | BUTTON_POWER):
#ifdef FETT263_USE_SETTINGS_MENU
        // Enter Edit Mode
        if (!menu_) {
          StartEditMode();
        }
#else
        // If Edit Mode Menu Disabled Save .ini files for SD card editing
        if (menu_) return true;
        GenerateIniFiles();
        sound_library_.SaySave();
#endif
        return true;

      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
#ifndef DISABLE_COLOR_CHANGE
        if (DoColorZoom()) {
          sound_library_.SayZoomingIn();
          return true;
        }
#endif
        return false;

      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ON):
#ifndef DISABLE_COLOR_CHANGE
        if (EndColorZoom()) return true;
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          ToggleColorChangeMode();
#ifdef FETT263_EDIT_MODE_MENU
          if (menu_type_ == MENU_COLOR) {
            menu_type_ = MENU_TOP;
            MenuSave();
            return true;
          }
#endif
          return true;
        }
#endif
        return false;

#ifdef FETT263_HOLD_BUTTON_LOCKUP
      case EVENTID(BUTTON_AUX, EVENT_HELD_MEDIUM, MODE_ON):
	if (CancelShowColor()) return false;
	if (menu_) {
	  return false;
	  break;
	}
        if (!SaberBase::Lockup()) {
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
	  }
          swing_blast_ = false;
          SaberBase::SetClashStrength(8);
          SaberBase::DoBeginLockup();
          return true;
        } 
#endif

      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_ON):
	if (CancelShowColor()) return false;
        if (menu_) {
#ifdef FETT263_EDIT_SETTINGS_MENU
          if (menu_type_ == MENU_SETTING_SUB) {
#else
          if (menu_type_ == MENU_TOP) {
#endif
            sound_library_.SayExit();
            menu_ = false;
          } else {
            menu_type_ = MENU_TOP;
            MenuCancel();
          }
          return true;
        }
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
	  sound_library_.SayCancel();
          rehearse_ = false;
          return true;
        }
#endif
#if !defined(FETT263_HOLD_BUTTON_LOCKUP) && !defined(FETT263_DISABLE_BM_TOGGLE)
        ToggleBattleMode();
#endif
        return true;

#if defined(FETT263_HOLD_BUTTON_LOCKUP) && !defined(FETT263_DISABLE_BM_TOGGLE)
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON | BUTTON_AUX):
        if (menu_ || CheckShowColorCC()) return true;
        ToggleBattleMode();
        return true;
#endif

#ifndef FETT263_DISABLE_CHANGE_STYLE
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON | BUTTON_AUX):
        if (menu_ || CheckShowColorCC()) return true;
        ChangeStyleNumberAllBlades(fusor.angle1() < - M_PI / 3 ? -1 : 1);
        hybrid_font.PlayCommon(&SFX_ccchange);
        return true;
#endif

#ifndef FETT263_DISABLE_CHANGE_FONT
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF | BUTTON_AUX):
        if (menu_) return true;
        ChangeFont(fusor.angle1() < - M_PI / 3 ? -1 : 1);
        return true;
#endif

      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_ON | BUTTON_POWER):
        if (menu_ || CheckShowColorCC()) return true;
        if (saved_gesture_control.powerlock
#ifdef FETT263_SAVE_CHOREOGRAPHY
	|| choreo_
#endif
        ) {
#ifdef FETT263_SAVE_CHOREOGRAPHY
          choreo_ = false;
#endif
	  wav_player.Free();
          battle_mode_ = false;
#ifdef FETT263_DUAL_MODE_SOUND
          SelectRetractionSound();
#endif
          Off();
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_ON):
	if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          EndRehearsal();
          return true;
        }
#endif
#ifdef FETT263_HOLD_BUTTON_OFF
        if (!swinging_) {
          check_blast_ = false;
          swing_blast_ = false;
#ifdef FETT263_DUAL_MODE_SOUND
          SelectRetractionSound();
#endif
          Off();
          saber_off_time_millis_ = millis();
          battle_mode_ = false;
    return true;
        }
#endif
	return false;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_LATCH_OFF, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_LATCH_OFF, MODE_ON):
        if (CheckShowColorCC()) return true;
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (choreo_) {
          if (saved_choreography.clash_rec[clash_count_].stance != SavedRehearsal::STANCE_END) return true;
        }
#endif
        if (menu_) {
          MenuChoice();
          return true;
        } else {
#ifndef FETT263_HOLD_BUTTON_OFF
          if (!swinging_) {
            check_blast_ = false;
            swing_blast_ = false;
#ifdef FETT263_BM_DISABLE_OFF_BUTTON
            if (!battle_mode_ && !saved_gesture_control.powerlock) {
#ifdef FETT263_DUAL_MODE_SOUND
              SelectRetractionSound();
#endif
              Off();
              saber_off_time_millis_ = millis();
            }
#else
            if (!saved_gesture_control.powerlock) {
#ifdef FETT263_DUAL_MODE_SOUND
              SelectRetractionSound();
#endif
            Off();
            saber_off_time_millis_ = millis();
            battle_mode_ = false;
            }
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
            if (choreo_ && saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_END) {
              EndChoreo();
            }
#endif
#ifdef FETT263_BATTLE_MODE_ALWAYS_ON
            battle_mode_ = true;
#endif
          }
#endif
          return true;
        }

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_DOUBLE_CLICK, MODE_ON):
        if (menu_) {
          MenuUndo();
          return true;
        } else {
          // Avoid the base and the very tip.
          // TODO: Make blast only appear on one blade!
#ifndef DISABLE_COLOR_CHANGE
          if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
            SaberBase::SetVariation(0);
            ToggleColorChangeMode();
            return true;
          }
#endif
          if (color_mode_ == CC_COLOR_LIST) {
            color_mode_ = NONE;
            show_color_all_.Stop();
            sound_library_.SayRevert();
            wav_player.Free();
            return true;
          }
          DoInteractiveBlast();
          return true;
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON):
        if (menu_ || CancelShowColor()) return true;
        if (fusor.angle1() > M_PI / 3) {
          if (track_player_) {
            StopTrackPlayer();
          } else {
            StartOrStopTrack();
          }
          return true;
        } else {
          CheckQuote();
        }
        return true;

#if !defined(FETT263_DISABLE_MULTI_BLAST) && !defined(FETT263_DISABLE_MULTI_BLAST_TOGGLE)
      case EVENTID(BUTTON_AUX, EVENT_CLICK_LONG, MODE_ON):
        if (menu_ || CancelShowColor()) return true;
      	ToggleMultiBlast();
        return true;
#endif

      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON | BUTTON_AUX):
        if (menu_ || CancelShowColor()) return true;
        if (fusor.angle1() > M_PI / 3) {
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
        } else {
#ifndef DISABLE_COLOR_CHANGE
          ToggleCCMode();
#endif
#ifdef DISABLE_COLOR_CHANGE
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
#endif
        }
        return true;

        // Lockup
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_SPIN_MODE
        if (spin_mode_) return true;
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          RehearseLockup();
        }
#endif
        if (!SaberBase::Lockup()) {
          if (fusor.angle1() < - M_PI / 8) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
            SaberBase::DoBeginLockup();
          } else {
#ifdef FETT263_CLASH_STRENGTH_SOUND
            clash_impact_millis_ = millis();
            clash_type_ = CLASH_LOCKUP;
#else
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
            SaberBase::DoBeginLockup();
#endif
          }
          check_blast_ = false;
          swing_blast_ = false;
          return true;
        }
        return true;
        break;

      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
        if (menu_ || CheckShowColorCC()) return true;
        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
        check_blast_ = false;
        swing_blast_ = false;
        SaberBase::DoBeginLockup();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_AUX):
        if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_SPIN_MODE
        if (spin_mode_) return true;
#endif
        if (!SaberBase::Lockup()) {
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
          check_blast_ = false;
          swing_blast_ = false;
          SaberBase::DoBeginLockup();
          return true;
        }
        return true;
        break;

#ifdef FETT263_SPECIAL_ABILITIES
      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON | BUTTON_AUX):
        if (!menu_) {
          SaberBase::DoEffect(EFFECT_USER3, 0);
          return true;
        }
        current_menu_angle_ = fusor.angle2();
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON | BUTTON_AUX):
        if (!menu_) {
          SaberBase::DoEffect(EFFECT_USER4, 0);
          return true;
        }
        current_menu_angle_ = fusor.angle2();
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF | BUTTON_AUX):
        if (!menu_) {
          SaberBase::DoEffect(EFFECT_USER7, 0);
          return true;
        }
        current_menu_angle_ = fusor.angle2();
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF | BUTTON_AUX):
        if (!menu_) {
          SaberBase::DoEffect(EFFECT_USER8, 0);
          return true;
        }
        current_menu_angle_ = fusor.angle2();
        return false;
#endif

        // Off functions
      case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
        if (menu_) return true;
#ifdef ENABLE_AUDIO
        if (track_player_) {
          StopTrackPlayer();
          return true;
        } else {
          if (fusor.angle1() > M_PI / 3) {
            StartOrStopTrack();
            return true;
          } else {
            StartTrackPlayer();
            return true;
          }
        }
#else
        STDOUT.println("Audio disabled.");
#endif
        return true;

      case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_OFF):
        if (menu_) {
          if (menu_type_ == MENU_PRESET) {
            MenuChoice();
            FastOn();
          }
          return true;
        } else {
          CheckQuote();
        }
        return true;

      case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
        if (menu_) return true;
        SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
        return true;

#ifdef FETT263_SAVE_CHOREOGRAPHY
      // Rehearsal Mode
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF | BUTTON_AUX):
        if (!menu_) {
          // Check for existing rehearsal and prompt to overwrite or keep via menu
        if (saved_choreography.isIntialized()) {
            sound_library_.SayRehearseNew();
            StartMenu(MENU_REHEARSE);
            return true;
          } else {
            BeginRehearsal();
          }
	}
        return true;

      // Choreographed Battle Mode
      case EVENTID(BUTTON_AUX, EVENT_HELD_LONG, MODE_OFF):
        if (menu_) return true;
        BeginChoreo();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_AUX):
        if (menu_) return true;
        BeginChoreo();
        return true;
#endif
		    
#ifdef FETT263_MULTI_PHASE
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_AUX):
	if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_DUAL_MODE_SOUND
        SelectIgnitionSound();
#endif
        next_preset_fast();
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
	if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_DUAL_MODE_SOUND
        SelectIgnitionSound();
#endif
        previous_preset_fast();
        return true;
#endif

#endif
// Gesture Controls (not button specific)
      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }
        if (color_mode_ == CC_COLOR_LIST) {
          dial_ = (dial_ + 1) % NELEM(color_list_);
          ShowColorStyle::SetColor(Color16(color_list_[dial_].color));
#ifdef FETT263_SAY_COLOR_LIST_CC
          sound_library_.SayColor(color_list_[dial_].color_number);
#else
          hybrid_font.PlayCommon(&SFX_ccchange);
#endif
          return true;
        }
        if (menu_) {
          MenuDial(1);
          return true;
	}
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }
        if (color_mode_ == CC_COLOR_LIST) {
          if (dial_ <= 0) dial_ = NELEM(color_list_);
          dial_ = dial_ - 1;
          ShowColorStyle::SetColor(Color16(color_list_[dial_].color));
#ifdef FETT263_SAY_COLOR_LIST_CC
          sound_library_.SayColor(color_list_[dial_].color_number);
#else
          hybrid_font.PlayCommon(&SFX_ccchange);
#endif
          return true;
        }
        if (menu_) {
          MenuDial(-1);
          return true;
        }
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }
        if (menu_) {
          MenuDial(1);
          return true;
        }
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }
        if (menu_) {
          MenuDial(-1);
          return true;
        }
        return false;

      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        if (swing_blast_) {
          SaberBase::DoBlast();
          return true;
        } else {
          DoAutoMultiBlast();
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON | BUTTON_POWER):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }
        if (menu_) {
	  MenuDialIncrement(1);
          return true;
        }
        if (swinging_) {
          return false;
        }
#ifdef FETT263_SPECIAL_ABILITIES
        else {
#if NUM_BUTTONS == 1
          if (fusor.angle1() < - ((M_PI / 2) - 0.25)) {
            SaberBase::DoEffect(EFFECT_USER3, 0);
          } else {
            SaberBase::DoEffect(EFFECT_USER1, 0);
          }
#else
          SaberBase::DoEffect(EFFECT_USER1, 0);
#endif
        return true;
	}
#endif
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON | BUTTON_POWER):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }        
        if (menu_) {
	  MenuDialIncrement(-1);
          return true;
	}
        if (swinging_) {
          return false;
	}
#ifdef FETT263_SPECIAL_ABILITIES
        else {
#if NUM_BUTTONS == 1
          if (fusor.angle1() < - ((M_PI / 2) - 0.25)) {
            SaberBase::DoEffect(EFFECT_USER4, 0);
          } else {
            SaberBase::DoEffect(EFFECT_USER2, 0);
          }
#else
          SaberBase::DoEffect(EFFECT_USER2, 0);
#endif
          return true;
	}
#endif
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF | BUTTON_POWER):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }
        if (menu_) {
          if (menu_type_ == MENU_TRACK_PLAYER) {
            track_mode_ = PLAYBACK_ROTATE;
#ifndef FETT263_TRACK_PLAYER_NO_PROMPTS
            sound_library_.SayRotate();
#endif
            return true;
          } else {
            MenuDialIncrement(1);
            return true;
          }
	}
#ifdef FETT263_SPECIAL_ABILITIES
        else {
#if NUM_BUTTONS == 1
          if (fusor.angle1() < - ((M_PI / 2) - 0.25)) {
            SaberBase::DoEffect(EFFECT_USER7, 0);
          } else {
            SaberBase::DoEffect(EFFECT_USER5, 0);
          }
#else
          SaberBase::DoEffect(EFFECT_USER5, 0);
#endif
	  return true;
        }
#endif
        return false;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF | BUTTON_POWER):
        if (wav_player && wav_player->isPlaying()) {
          current_menu_angle_ = fusor.angle2();
          return false;
        }
        if (menu_) {
          if (menu_type_ == MENU_TRACK_PLAYER) {
            track_mode_ = PLAYBACK_LOOP;
#ifndef FETT263_TRACK_PLAYER_NO_PROMPTS
            sound_library_.SayLoop();
#endif
            return true;
          } else {
            MenuDialIncrement(-1);
            return true;
          }
	}
#ifdef FETT263_SPECIAL_ABILITIES
        else {
#if NUM_BUTTONS == 1
          if (fusor.angle1() < - ((M_PI / 2) - 0.25)) {
            SaberBase::DoEffect(EFFECT_USER8, 0);
          } else {
            SaberBase::DoEffect(EFFECT_USER6, 0);
          }
#else
          SaberBase::DoEffect(EFFECT_USER6, 0);
#endif
          return true;
	}
#endif
        return false;

      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF):
        if (menu_) return true;
        SaberBase::DoEffectR(EFFECT_OFF_CLASH);
        return true;

      // Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        if (menu_ || SaberBase::Lockup() || CheckShowColorCC()) return true;
#ifdef FETT263_SPIN_MODE
        if (spin_mode_) return true;
#endif
        clash_impact_millis_ = millis();
        check_blast_ = false;
        swing_blast_ = false;
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (rehearse_) {
          RehearseClash();
          return true;
        }
#endif
        // Allow normal clashes if blade continues to swing after clash detected in Battle Mode
        if (!battle_mode_ || swinging_) {
          clash_impact_millis_ = millis();
#ifdef FETT263_CLASH_STRENGTH_SOUND
          clash_type_ = CLASH_NORMAL;
#else
          SaberBase::DoClash();
#endif
          return true;
        }
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (choreo_) {
          if (saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_CLASH) {
            ChoreoClash();
            return true;
          } else {
            ChoreoLockup();
            return true;
          }
        }
#endif
        if (fusor.angle1() < - ((M_PI / 2) - 0.25)) {
          SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          SaberBase::DoBeginLockup();
          auto_melt_on_ = true;
          return true;
        }
        clash_type_ = CLASH_BATTLE_MODE;
        return true;

      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
        if (menu_ || SaberBase::Lockup() || CheckShowColorCC()) return true;
#ifdef FETT263_SPIN_MODE
        if (spin_mode_) return true;
#endif
        clash_impact_millis_ = millis();
        check_blast_ = false;
        swing_blast_ = false;
        if (!swinging_) {
#ifndef FETT263_USE_BC_MELT_STAB
          if (!battle_mode_) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
            clash_impact_millis_ = millis();
            clash_type_ = CLASH_STAB;
#else
            SaberBase::DoStab();
#endif
            return true;
          }
#endif
          if (fusor.angle1() < - M_PI / 4) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          }
          auto_melt_on_ = true;
          SaberBase::DoBeginLockup();
        }
        return true;

#ifdef FETT263_USE_BC_MELT_STAB
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_ON):
        if (menu_ || SaberBase::Lockup() || CheckShowColorCC()) return true;
        clash_impact_millis_ = millis();
#ifdef FETT263_CLASH_STRENGTH_SOUND
        clash_impact_millis_ = millis();
        clash_type_ = CLASH_STAB;
#else
        SaberBase::DoStab();
#endif
        check_blast_ = false;
        swing_blast_ = false;
        return true;
#endif

      // Optional Gesture Controls (defines listed at top)

#ifdef FETT263_SPIN_MODE
    case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_POWER):
      if (!menu_) {
        spin_mode_ = !spin_mode_;
	if (spin_mode_) {
          if (SFX_bmbegin) {
            hybrid_font.PlayCommon(&SFX_bmbegin);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
	} else {
          if (SFX_bmend) {
            hybrid_font.PlayCommon(&SFX_bmend);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
	}
      }
      return true;
#endif

#ifdef FETT263_SWING_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.swingon) return true;
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
	  wav_player.Free();
          if (SFX_preon) {
#ifdef FETT263_DUAL_MODE_SOUND
            SelectPreonSound();
#endif  
            On();
          } else {
            FastOn();
          }
#ifndef FETT263_SWING_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.swingon) return true;
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
          wav_player.Free();
	  FastOn();
#ifndef FETT263_SWING_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
        if (!saved_gesture_control.twistoff) return true;
        if (menu_ || CheckShowColorCC()) return true;
#ifdef FETT263_SAVE_CHOREOGRAPHY
        if (choreo_) {
          if (saved_choreography.clash_rec[clash_count_].stance == SavedRehearsal::STANCE_END) {
          if (SFX_clsh) {
            SFX_clsh.Select(-1);
          } else {
            SFX_clash.Select(-1);
          }
          if (SFX_lock) {
            SFX_lock.Select(-1);
          } else {
            SFX_lockup.Select(-1);
          }
          SFX_bgnlock.Select(-1);
          SFX_endlock.Select(-1);
          choreo_ = false;
          } else {
            return true;
          }
        }
#endif
#ifdef FETT263_DUAL_MODE_SOUND          
        SelectRetractionSound();
#endif  
#ifdef FETT263_TWIST_OFF_NO_POSTOFF  
        Off(OFF_FAST);
#else
        Off();
#endif
        saber_off_time_millis_ = millis();
#ifndef FETT263_BATTLE_MODE_ALWAYS_ON
        battle_mode_ = false;
#endif
        return true;

#ifdef FETT263_TWIST_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.twiston) return true;
        // Delay twist events to prevent false trigger from over twisting
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
	  wav_player.Free();
          if (SFX_preon) {
#ifdef FETT263_DUAL_MODE_SOUND
            SelectPreonSound();
#endif
            On();
          } else {
            FastOn();
          }

#ifndef FETT263_TWIST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.twiston) return true;
        // Delay twist events to prevent false trigger from over twisting
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
	  wav_player.Free();
          FastOn();
#ifndef FETT263_TWIST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_STAB_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.stabon) return true;
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
          wav_player.Free();
          if (SFX_preon) {
#ifdef FETT263_DUAL_MODE_SOUND
            SelectPreonSound();
#endif
            On();
          } else {
            FastOn();
          }
#ifndef FETT263_STAB_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.stabon) return true;
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
	  wav_player.Free();
          FastOn();
#ifndef FETT263_STAB_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

#ifdef FETT263_THRUST_ON_PREON
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.thruston) return true;
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
	  wav_player.Free();
          if (SFX_preon) {
#ifdef FETT263_DUAL_MODE_SOUND
            SelectPreonSound();
#endif
            On();
          } else {
            FastOn();
          }
#ifndef FETT263_THRUST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#else
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        if (!saved_gesture_control.gestureon) return true;
        if (!saved_gesture_control.thruston) return true;
        if (!menu_) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectIgnitionSound();
#endif
	  wav_player.Free();
          FastOn();
#ifndef FETT263_THRUST_ON_NO_BM
          battle_mode_ = true;
#endif
        }
        return true;
#endif

      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
        if (menu_ || CheckShowColorCC()) return true;
        if (!saved_gesture_control.forcepush) return true;
        if (FORCE_PUSH_CONDITION &&
           millis() - last_push_millis_ > 1000) {
          if (SFX_push) {
            hybrid_font.PlayCommon(&SFX_push);
          } else {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
          last_push_millis_ = millis();
        }
        return true;

      // Events that needs to be handled regardless of what other buttons
      // are pressed.
      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        if (SaberBase::Lockup()) {
#ifdef FETT263_CLASH_STRENGTH_SOUND
          clash_type_ = CLASH_LOCKUP_END;
          HandleClash();
#else
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
#endif
          return true;
        }
    }
    return false;
  }

  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      case EFFECT_INTERACTIVE_BLAST: hybrid_font.PlayCommon(&SFX_blast); return;
      case EFFECT_TRACK: PlayEffectTrack(); return;
      case EFFECT_QUOTE: hybrid_font.PlayCommon(&SFX_quote); return;
      case EFFECT_NEXT_QUOTE:
        SFX_quote.SelectNext();
        hybrid_font.PlayCommon(&SFX_quote);
        return;
      case EFFECT_FAST_OFF:
      case EFFECT_OFF:
        if (SaberBase::IsOn()) {
#ifdef FETT263_DUAL_MODE_SOUND          
          SelectRetractionSound();
#endif
          if (effect == EFFECT_FAST_OFF) {
            Off(OFF_FAST);
          } else {
            Off();
          }
          saber_off_time_millis_ = millis();
        }
        return;
      case EFFECT_SECONDARY_RETRACTION:
        if (SaberBase::IsOn()) {
#ifdef FETT263_DUAL_MODE_SOUND
          SelectRetractionSound();
#endif
          hybrid_font.PlayCommon(&SFX_in);
	}
        return;
      case EFFECT_POWERSAVE:
        if (SFX_dim) {
          hybrid_font.PlayCommon(&SFX_dim);
        } else {
          beeper.Beep(0.5, 3000);
        }
        return;
      case EFFECT_BATTERY_LEVEL:
        DoBattery();
        return;
      case EFFECT_INTERACTIVE_PREON:
        preon_effect_ = true;
        return;
      case EFFECT_FAST_ON:
        if (SaberBase::IsOn()) {
          if (!preon_effect_ && SFX_faston) {
            hybrid_font.PlayCommon(&SFX_faston);
          }
        } else {
#ifdef FETT263_DUAL_MODE_SOUND          
          SelectIgnitionSound();
#endif
          FastOn();
          preon_effect_ = false;
        }
        return;
      case EFFECT_ON:
        if (!SaberBase::IsOn()) {
#ifdef FETT263_DUAL_MODE_SOUND          
          SelectIgnitionSound();
#endif
          On();
        }
        return;
      case EFFECT_SECONDARY_IGNITION:
        if (SaberBase::IsOn()) {
#ifdef FETT263_DUAL_MODE_SOUND          
          SelectIgnitionSound();
#endif 
          hybrid_font.PlayCommon(&SFX_out);
	}
        return;
      case EFFECT_TRANSITION_SOUND: hybrid_font.PlayCommon(&SFX_tr); return;
      case EFFECT_SOUND_LOOP: SoundLoop(); return;
      case EFFECT_BEGIN_BATTLE_MODE:
        if (!battle_mode_) ToggleBattleMode();
        return;
      case EFFECT_END_BATTLE_MODE:
        if (battle_mode_) ToggleBattleMode();
        return;
      case EFFECT_BEGIN_AUTO_BLAST:
        if (!swing_blast_) ToggleMultiBlast();
        return;
      case EFFECT_END_AUTO_BLAST:
        if (swing_blast_) ToggleMultiBlast();
        return;
      default:
        break; // avoid compiler warnings
    }
  }

private:
  bool swing_blast_ = false; // Multi-Blast Mode (Swing = Blast Deflect)
  bool check_blast_ = false; // Check if Multi-Blast mode should be enabled (Battle Mode)
  bool auto_lockup_on_ = false; // Battle Mode Lockup active
  bool auto_melt_on_ = false; // Battle Mode Melt/Drag active
  bool battle_mode_ = false; // Battle Mode active
#ifdef FETT263_SPIN_MODE
  bool spin_mode_ = false;
#endif
  bool preon_effect_ = false; // used for interactive preon, must be reset with EFFECT_FAST_ON
#ifdef FETT263_QUICK_SELECT_ON_BOOT  
  bool menu_ = true; // enable MENU_PRESET on boot
#else
  bool menu_ = false; // Edit Mode / Menu System active
#endif
#ifdef FETT263_QUOTE_PLAYER_START_ON
  bool force_quote_ = true; // Quote Player active (in place of force effect)
#else
  bool force_quote_ = false; // Quote Player active (in place of force effect)
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
  bool rehearse_ = false; // Rehearsal Mode active
  bool choreo_ = false; // Choreography Mode active
#endif
  uint32_t thrust_begin_millis_; // Thrust timer
  uint32_t push_begin_millis_; // Push timer
  uint32_t clash_impact_millis_; // Clash timer
  uint32_t last_push_millis_; // Last Push (to prevent gesture spamming)
  uint32_t last_blast_millis_; // Last Blast (for Battle Mode Multi-Blast detection)
  uint32_t saber_off_time_millis_; // Off timer
  uint32_t restart_millis_; // Used to time restarts to show preon timing.
  ClashType clash_type_ = CLASH_NONE;
#ifdef FETT263_QUICK_SELECT_ON_BOOT  
  MenuType menu_type_ = MENU_PRESET;
#else
  MenuType menu_type_ = MENU_TOP;
#endif
  int menu_top_pos_ = 0; // Top menu dial position
  int menu_sub_pos_ = 0; // Sub menu dial position
  TrackMode track_mode_ = PLAYBACK_OFF;
  int track_num_; // Track Number for Track Player
  int num_tracks_; // Number of Tracks Found
  int ignite_time_; // Ignition timer for Edit Mode retraction preview
  int dial_ = -1; // Menu dial "tick"
  int sub_dial_; // Sub menu dial "tick"
  int arg_dial_; // Argument Sub menu dial "tick"
  int gesture_num_;
  float twist_menu_ = M_PI / 4; // Twist Menu sensitivity
  bool choice_ = false;
  // Edit Mode selection confirmation
  // for True/False control when deleting, disabling/enabling or copying
  bool off_event_ = false; // Do off event in Edit Mode
  bool restart_ = false; // Ignite blade after off event in Edit Mode
  bool next_event_ = false;
  // Do next event in Edit Mode, allows an action/wav to complete before
  // "next event" begins, for use with choreography and ignition/retraction previews where menu sound
  // would otherwise be truncated by change in state
  EditColorMode color_mode_;
  bool edit_color_ = false; // Color Editing Mode active
  float hsl_angle_ = 0.0; // HSL angle for Color Editing
  int font_num_; // Font number from list_fonts array for use in Edit Mode dial
#ifdef FETT263_USE_SETTINGS_MENU
  int blade_preview_ = 0; // Blade number for "preview" style
  int blade_num_; // Active Blade Number for editing
  int length_revert_; // Original Blade Length for Revert
  // Calculated value for IntArg (Settings Arguments)
  // used to add or subtract from original saved value by dial
  int calc_;
#endif
#ifdef FETT263_EDIT_MODE_MENU
  uint32_t variation_revert_; // Variation revert value
  Color16 saved_color_;
  int effect_num_; // Effect Arg Number
  int copy_blade_; // Blade to Copy from
  int set_num_; // Settings Arg Number
  int style_revert_; // Original Style Number for Revert
  int arg_revert_; // Original Arg vale for Revert
#endif
#ifdef FETT263_SAVE_CHOREOGRAPHY
  int clash_count_ = -1; // Choreography Mode Clash counter
#endif
};

#endif
