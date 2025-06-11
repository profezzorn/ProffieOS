/*
saber_BC_buttons.h
  http://fredrik.hubbe.net/lightsaber/proffieos.html
  Copyright (c) 2016-2019 Fredrik Hubinette
  Copyright (c) 2023-2025 Brian Conner with contributions by:
  Fredrik Hubinette, Fernando da Rosa, Matthew McGeary, and Scott Weber.
  Distributed under the terms of the GNU General Public License v3.
  http://www.gnu.org/licenses/

Includes 1 or 2 button controls. (3rd button for power control of Dual Blades supported as well).
Incorporates an intuitive control scheme so button actions are consistant
whether blade is on or off.

** This prop version REQUIRES a V2 ProffieOS Voicepack for menus to work right.
** Menus will have no sounds if the contents are not somewhere in the font search path.
** Typically, that is a folder named "common" on the root level of the SD card.

Download your choice of language and variation here:
http://fredrik.hubbe.net/lightsaber/sound/
Also, a large variety of FREE in-universe character Voicepacks available here:
https://crucible.hubbe.net/t/additional-voicepacks/4227
If you'd care to make a donation for my time:
https://www.buymeacoffee.com/brianconner

Features:
- Live preset changing while blade is running (skips font.wav and preon).
- Battle Mode, gesture ignitions, and multi-blast based on fett263's work.
- Auto enter/exit multi-blaster block mode.
- Spoken Battery level in volts OR percentage. Both always available.
- Dedicated Quote sounds - Always available, blade ON or OFF. Force.wavs can remain as force.
                           Add quote.wav files to font to use.
                           Live toggle between sequential or random quote play.
                           Quotes can be triggered while blade is off.
- Play / Stop track control while blade is OFF OR ON.
- Force Push is always available, not just in Battle Mode.
- Melt is always available as no button, with pull-away or button to end.
- Drag is always clash with button pressed while pointing DOWN.
- No gestures if No blade is inserted - available option if Blade Detect is used.
- Custom Volume menu with Quick MIN and MAX levels.
- Bypass preon and/or postoff based on blade angle.
- Spam Blast - Enter this mode to make the button super sensitive for
                            multiple blaster blocks. Presses are prioritized over
                            other features. No limits, no lag when "rapid firing".
- No inadvertant effects during preon.
- Rotary control of Volume and Scroll Presets. (Rotate hilt like turning a dial)
- * NEW for OS8 *
  - Dual blade independent ignition and retraction control with a single Proffieboard. (Such as for a staff saber)
  - Scroll Presets mode.
  - Quotes play with blade ON or OFF, and will not overlap / interrupt one another.
  - Mute sound plays before igniting muted.(optional mute.wav used)
  - Blade Length editing menu.
  - OS System Menu system available with a #define.
* Deprecated features:
- No Volume UP/DOWN with buttons. Use rotary control instead (see above),
  but QuickMinVolume and QuickMaxVolume are still available.
- swap and monoforce features of yore are now just assignable User Effects.
- #define NO_VOLUME_MENU removed.
---------------------------------------------------------------------------
Optional Blade style elements:
On-Demand battery level - A layer built into the blade styles that reacts
                          as the battery gets weaker, changing blade color
                          from Green to Red, and the blade length shortens.
User Effects: 
These equate to Fett263 "Special Abilities" 1-8.
- Can be built into blade style code to trigger anything that takes an EFFECT_XXXXX argument.
EFFECT_USER1 through 4 work when blade is ON.
EFFECT_USER5 through 8 work when blade is OFF.
USER 3, 4, 7, 8 are done "while pointing up" on 1 button setups, or the AUX button for 2 button sabers.

EFFECT_POWERSAVE        - PowerSave Dim Blade. Blade style should use a Mix or an AlphaL that applies
                          a transparent amount of Black to the base blade color.
                          Layer example: 50% brightness reduction
                          EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<0>>>

---------------------------------------------------------------------------
Optional #defines:
#define BC_ENABLE_AUTO_SWING_BLAST - Multi-blast initiated by simply swinging
                                     within 1 second of last blast.
                                     Exit by not swinging for 1 second.
#define FEMALE_TALKIE_VOICE        - To use a female voice version of onboard Talkie.

* NEW as of OS8:
#define MENU_SPEC_TEMPLATE         - BC Volume Menu, Scroll Presets, ColorChange, and BC Blade Length adjust are used by default.
                                     Use this define to override and add access the the OS System Menu for editing presets, colors etc...
                                     Several template choices are available to set how the menus are used. See https://pod.hubbe.net/howto/menus.html.
#define DYNAMIC_BLADE_LENGTH       - This is required for onboard menu driven blade length adjustments.
                                     The blade length maximum should be set in the user config file's BladeConfig section.

// Adding the following define activates Dual Blade code in the prop.
#define BC_DUAL_BLADES             - Use Dual Blades mode for a staff saber setup.
                                     Defaults are BLADE 1 and BLADE 2 as MAIN and SECOND blades respectively.
                                     BLADE # corresponds to the blades list in the BladeConfig section of the config file,
                                     and therefore the position of the blade styles in a preset.
                                     If your MAIN and SECOND blades are not in position 1 and 2, you can use the following
                                     optional defines to manually set BLADE # to the appropriate blade from the BladeConfig list.
#define BC_MAIN_BLADE                * Example: #define BC_MAIN_BLADE 2
#define BC_SECOND_BLADE              * Example: #define BC_SECOND_BLADE 4

---------------------------------------------------------------------------
Gesture Controls:
- There are three gesture types: swing, thrust and twist.
  For simplicity, using gesture ignition will automatically skip the preon effect.
  * NOTE * If using Dual Blades, Swing and Twist ignitions will turn on all blades.
  Below are the options to add to the config to enable the various gestures.
#define BC_SWING_ON
#define BC_THRUST_ON
#define BC_TWIST_ON
#define BC_TWIST_OFF

#define NO_BLADE_NO_GEST_ONOFF
- If using Blade Detect, this define disables gesture ignitions or retractions when no blade is inserted.
  * Only works when a BLADE_DETECT_PIN is defined.

#define BC_FORCE_PUSH
- This gesture plays a force sound (or force push sound if the font contains push.wav) with
  a controlled pushing gesture, and is always available, not just in Battle Mode.

#define BC_FORCE_PUSH_LENGTH
- Used for adjustment to the Push gesture length in millis needed to trigger
  Force Push. Recommended range 1 ~ 10,
  1 = shortest, easiest to trigger, 10 = longest. Default value is 5.

#define BC_NO_BM
- Disable battle mode features.

#define BC_GESTURE_AUTO_BATTLE_MODE
- Makes gesture ignition ALSO enter battle mode automatically on ignition.
- *Note* - Cannot be used if #define BC_NO_BM is active.

"Battle Mode 1.0" by fett263, BC modified version:
- Once you enter battle mode, buttons are not used for lockup.
  Clashing the blade against something will automatically do lockup and then end
  when you pull away.
- Automatic lockup and grazing clash (swinging through) detection works
  by measuring delay of the saber blade pulling back from the clash.
  If you clash the blade and do not pull away during the delay period,
  it is assumed to be a lockup and the lockup effect will show on the blade.
  If you clash the blade and pull away, only the bgn/end lockup effects will show.
  You can adjust the threshold of this detection by using:
  #define BC_LOCKUP_DELAY (insert number here)
  Default value is 200.
  If you don't rebound off the object (opponent's blade) but instead clash while
  swinging through, clash will automatically trigger.
- To manually override the auto-lockup temporarily and get a regular clash,
  hold any button while clashing.
- Automatic clash/lockup uses the bgnlock and endlock effects
  so your blade style and font MUST have those capabilities to support
  battle mode.

- Melt will automatically trigger with no buttons when you physically
  stab something, and end when you pull away or click any button.

- Stab will trigger with no buttons and thrusting forward.

------------------------ GENERAL RULES ------------------------

Pointing the blade UP during ignition or retraction will bypass preon and postoff effects.
Gesture Ignitions bypass preon.

Rotation gestures LEFT and RIGHT are single direction "twists".
TWIST gesture is a 2-direction rotation, like revving a motorcycle. Either direction first works.

OS8 System Menus:
System menu will allow for editing of presets, colors and styles, similar to way the ProffieOS Workbench webpage and Edit mode work.
There's also an Edit Settings mode that has a volume level menu, blade length editing and more.
In this BC prop, the default mode is direct entry to BC Volume and BC Blade Length mode menus.
If you want to use the OS System Menu instead, you need to define a Menu Spec Template such as `#define MENU_SPEC_TEMPLATE DefaultMenuSpec`
While in any menu mode the following controls apply:
Save                - Click POW
Cancel / Exit       - Click AUX or Double Click POW
For more info, see https://pod.hubbe.net/howto/menus.html

Each section for controls have a descriptive version listed by feature and somwhat in the order
of using the saber, and a second summary list that is sorted by button clicks.


====================== 1 BUTTON CONTROLS ========================
| Sorted by ON or OFF state: (what it's like while using saber) |
=================================================================

* See below for modifications to these controls if BC_DUAL_BLADES is used.

-------- When blade is OFF -------
Turn Blade ON             - Short Click POW, or use a Gesture Ignition (see Gesture Controls above).
                            * Gesture ignitions will bypass preon, and optional fastout.wav can be used instead of out.wav.
Turn ON without Preon     - Short Click POW (while pointing UP), or use a Gesture Ignition.
                            * Uses fastout.wav if available.
Turn Blade ON Muted       - Hold POW then Twist.
                            or
                            Double Click POW.
                            * Muted ignitions will bypass preon.
                              Optional mute.wav will play before silent ignition and operation.
                              Saber unmutes when blade shuts off.
Scroll Presets            - Hold POW. Beeps will be heard when entering mode, and font.wav will play.
                            Rotate hilt to cycle through presets. Right = Next, Left = Previous.
                            Click POW to ignite with chosen preset, or Click and Hold POW again to Exit.
                            * TWIST_ON not available in this mode.
Next Preset               - Long Click then release POW (NOT pointing UP or DOWN).
Prev Preset               - Long Click then release POW (while pointing DOWN).
Jump to First Preset      - Long Click then release POW (while pointing UP).
Play/Stop Track           - 4x Click POW.
BC Volume Menu:
        Enter Menu        - Hold POW and Clash.
        Volume UP         - Rotate Right.
        Volume DOWN       - Rotate Left.
        Quick MAX Vol     - Hold POW while in Volume Menu.
        Quick MIN Vol     - Double Click and Hold POW while in Volume Menu.
        Save and Exit     - Click POW.
        Cancel and Exit   - Double Click POW.
BC Blade Length Edit:
        Enter Mode        - Double Click and Hold POW.
        Adjust            - Rotate Right or Left
        Save and Exit     - Click POW.
        Cancel and Exit   - Double Click POW.
** OS System Menu           * To use the OS menu system instead of the default BC Volume and BC Blade Length options,
                              use #define MENU_SPEC_TEMPLATE
Spoken Battery Level
        in percentage     - 3x Click and Hold POW.
        in volts          - 3x Click and Hold POW (while pointing DOWN).
                            * Will show On-blade display if EFFECT_BATTERY_LEVEL is used in blade style.
On-Demand Batt Level      - 3x Click and Hold POW, release after a second. (Double Click then Long Click)
                            * Requires EFFECT_BATTERY_LEVEL to be in blade style.
                            * Plays battery.wav sound effect if it exists in font or common folder,
                              otherwise a familiar tune of beeps :)


Quote Player              - 3x Click POW.
                            * Does Force effect if no quote(s) exist.
Toggle Sequential or
       Random quotes      - 3x Click POW (while pointing DOWN).
User Effect 5             - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
User Effect 6             - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
                            * Requires EFFECT_USER in blade style.
                            * Note the same controls when blade is ON are USER 1 and 2.
Trigger Blade ID Scan     - 4x Click POW and Hold, release after one second (3x Click then Long Click)
Next Blade Array          - 4x Click POW and Hold.
                            * Cycles to the next blade array in BladeConfig

-------- When blade is ON -------
Play/Stop Track           - 4x Click POW.
Next Preset Fast          - Hold POW and Twist (NOT pointing UP or DOWN).
                            * Fast switching presets bypasses preon and font.wav.
Previous Preset Fast      - Hold POW and Twist (while pointing DOWN).
First Preset Fast         - Hold POW and Twist (while pointing UP).

Clash                     - No button, just hit the blade against something.
Stab                      - Thrust forward with a stabbing motion. Works in Battle Mode.
Blaster Deflection        - Click or Double click POW.
Spam Blaster Blocks       - 4x Click and Hold POW (while pointing DOWN). Toggles SPAM BLAST mode ON/OFF.
                            * This makes the POW button super sensitive for multiple blaster blocks,
                              but gets in the way of controls for some other features, so deactivate when you're done spamming.
                              Plays mzoom.wav for activating/deactivating this mode.
                              Exits automatically when blade shuts OFF in case you leave it active.
Auto Swing Blast          - Swinging within 1 second of doing a button activated Blaster Block will start this timed mode.
                              To trigger auto blaster blocks, swing saber within 1 second of last block.
                              To exit, stop swinging for 1 second.
                              * Requires #define BC_ENABLE_AUTO_SWING_BLAST.
Toggle Battle Mode        - Double Click and Hold POW, release after a second (while pointing UP). (Click then Long Click)
                            * Power OFF is disabled while in Battle Mode. YOU MUST EXIT THE MODE FIRST.
Force Effect              - Double Click and Hold POW, release after a second (while NOT pointing UP). (Click then Long Click)

Lockup                    - Hold POW + Clash. Release button to end.
                            * In Battle Mode:
                              Just Clash and stay there to Lockup.
                              Holding POW while Clashing will do regular Clash without Locking up.
                              Pull away or press POW to end Lockup.
Drag                      - Hold POW + Clash (while pointing DOWN). Release button to end.
Melt                      - No button, just stab something. Rotate hilt to modify melting. Pull away or press POW to end.

Lightning Block           - Double Click and Hold POW. Release button to end.

Color Change Mode         - 3x Click and Hold POW.
                              Rotate hilt to cycle through all available colors.
                              Click POW to save color selection and Exit.
                              Double Click POW to Exit without saving.
Revert ColorChange        - 3x Click and Hold POW, release after a second. (Double Click then Long Click)
                              Reverts ColorChanged blade color back to the uploaded config color.
                              * This is done outside ColorChange Mode
        ColorChange explained:
          If RotateColorsX<Variation,COLOR> is used in the blade style:
              Rotate hilt to cycle through all available colors,
              Hold POW to save color selection and exit.
          If ColorChange<> is used in the blade style:
              There are up to 12 colors per rotation with a ccchange.wav sound at each step.
              If also using #define COLOR_CHANGE_DIRECT in the config,
              simply entering Color Change Mode will select the next color in the list and exit Color Change Mode.

Quote Player              - 3x Click POW.
                            * Does Force effect if no quote(s) exist.
Toggle Sequential or
       Random quotes      - 3x Click POW (while pointing DOWN).
Force Push                - Push hilt perpendicularly from a stop.
                            Plays push.wav if it exists, otherwise force.wav.
PowerSave Dim Blade       - 4x Click and Hold POW (while pointing UP).

User Effect 1             - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
User Effect 2             - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
                            * Require EFFECT_USER in blade style.
                            * Note the same controls when blade is OFF are USER 5 and 6.

Turn OFF blade            - Hold POW until off -or - Twist if using #define BC_TWIST_OFF.
Turn OFF bypass postoff   - Turn OFF (while pointing UP).

---------------------------------------
      1 button summary by clicks
---------------------------------------
--------- When blade is OFF ---------
twist                   - turn both blades ON (requires Gesture defines)
stab
swing
1 click                 - turn blade ON
                          turn blade ON bypass preon (pointing up)
1 click long            - first preset (pointing up)
                          next preset
                          previous preset (pointing down)
1 click held            - enter/exit scroll presets
    then twist          - turn blade ON muted (back and forth twist)
    then clash          - enter BC volume menu
    then rotate left    - user effect 5 (keep holding POW until executed)
    then rotate right   - user effect 6 (keep holding POW until executed)
2 clicks                - turn blade ON muted
2 clicks held           - blade length edit, or
                          OS system menu instead (requires #define MENU_SPEC_TEMPLATE)
3 clicks                - quote
                          toggle sequential or random quotes (pointing down)
3 clicks long           - on-demand battery level
3 clicks held           - spoken battery level in percentage
                          spoken battery level in volts (pointing down)
4 clicks                - play / stop track
4 clicks long           - manually trigger blade ID scan
4 clicks held           - manually switch to next blade array

twist                   - turn blade ON (requires #define BC_TWIST_ON)
- BC volume menu:
    rotate right        - volume UP
    rotate left         - volume DOWN
    1 click held        - quick MAX volume
    2 clicks held       - quick MIN volume
- OS system menu:
    rotate right/left   - choose options / adjust
    1 click             - select / save
    2 clicks            - exit / cancel

--------- When blade is ON ---------
1 click                 - blaster deflection
1 click long
1 click held            - turn blade OFF
                          turn blade OFF bypass postoff (pointing up)
    then twist          - first preset fast (pointing up)
                          next preset fast
                          previous preset fast (pointing down)
    then clash          - lockup
                          drag (pointing down)
    then rotate left    - user effect 1 (keep holding POW until executed)
    then rotate right   - user effect 2 (keep holding POW until executed)
2 clicks                - blaster deflection
2 clicks long           - force (not pointing up)
                        - toggle battle mode (pointing up)
2 clicks held           - lightning block (release to end)
3 clicks                - quote
                          toggle sequential or random quotes (pointing down)
3 clicks long           - revert colorchange to config
3 clicks held           - color change mode
4 clicks                - play / stop track
4 clicks long
4 clicks held           - dim (pointing up)
                          toggle spam blasts (pointing down)

hit object              - clash
twist                   - turn blade OFF (requires #define BC_TWIST_OFF)
thrust (air)            - stab
stab object             - melt
push                    - force push


====================== 2 BUTTON CONTROLS ========================
| Sorted by ON or OFF state: (what it's like while using saber) |
=================================================================

* See below for modifications to these controls if BC_DUAL_BLADES is used.

-------- When blade is OFF -------
Turn Blade ON             - Short Click POW, or use a Gesture Ignition (see Gesture Controls above).
                            * Gesture ignitions will bypass preon, and optional fastout.wav can be used instead of out.wav.
Turn ON without Preon     - Short Click POW (while pointing UP), or use a Gesture Ignition.
                            * Uses fastout.wav if available.
Turn Blade ON Muted       - Hold POW then Twist.
                            or
                            Double Click POW.
                            * Muted ignitions will bypass preon.
                              Optional mute.wav will play before silent ignition and operation.
                              Saber unmutes when blade shuts off.
Scroll Presets            - Hold POW. Beeps will be heard when entering mode, and font.wav will play.
                            Rotate hilt to cycle through presets. Right = Next, Left = Previous.
                            Click POW to ignite with chosen preset, or Click and Hold POW again to Exit.
                            * TWIST_ON not available in this mode.
Next Preset               - Click AUX (NOT pointing UP or DOWN).
Prev Preset               - Click AUX (while pointing DOWN).
Jump to First Preset      - Click AUX (while pointing UP).
Play/Stop Track           - Long Click then release POW.
BC Volume Menu:
        Enter Menu        - Hold POW + Click AUX.
        Volume UP         - Rotate Right.
        Volume DOWN       - Rotate Left.
        Quick MAX Vol     - Hold POW while in Volume Menu.
        Quick MIN Vol     - Double Click and Hold POW while in Volume Menu.
        Save and Exit     - Click POW.
        Cancel and Exit   - Double Click POW.
BC Blade Length Edit:
        Enter Mode        - Double Click and Hold POW.
        Adjust            - Rotate Right or Left
        Save and Exit     - Click POW.
        Cancel and Exit   - Double Click POW.
** OS System Menu           * To use the OS menu system instead of the default BC Volume and BC Blade Length options,
                              use #define MENU_SPEC_TEMPLATE
Spoken Battery Level
        in percentage     - Hold AUX + Click POW.
        in volts          - Hold AUX + Click POW (while pointing DOWN).
                            * Will show On-blade display if EFFECT_BATTERY_LEVEL is used in blade style.
On-Demand Batt Level      - Hold AUX + Long Click then release POW.
                            * Requires EFFECT_BATTERY_LEVEL to be in blade style. Uses battery.wav sound effect.

Quote Player              - 3x Click POW.
                            * Does Force effect if no quote(s) exist.
Toggle Sequential or
       Random quotes      - 3x Click POW (while pointing DOWN).

User Effect 5             - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
User Effect 6             - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
User Effect 7             - Hold AUX then Rotate Left 60 degrees. (keep holding AUX until executed)
User Effect 8             - Hold AUX then Rotate Right 60 degrees. (keep holding AUX until executed)
                            * Requires EFFECT_USER in blade style.
                            * Note the same controls when blade is ON are USER 1,2,3,4.
Trigger Blade ID Scan     - 4x Click POW and Hold, release after one second (3x Click then Long Click)
Next Blade Array          - 4x Click POW and Hold.
                            * Cycles to the next blade array in BladeConfig

-------- When blade is ON -------
Play/Stop Track           - Long Click then release POW.
Next Preset Fast          - Hold POW and Twist (NOT pointing UP or DOWN).
                            * Fast switching presets bypasses preon and font.wav.
Previous Preset Fast      - Hold POW and Twist (while pointing DOWN).
First Preset Fast         - Hold POW and Twist (while pointing UP).

Clash                     - No buttons, just hit the blade against something.
                            In Battle Mode, Hold any button and Clash to
                            temporarily override the auto-lockup and do regular Clash.
Stab                      - Thrust forward with a stabbing motion. Works in Battle Mode.
Blaster Deflection        - Click or Double click POW or AUX.
Spam Blaster Blocks       - 4x Click and Hold POW (while pointing DOWN). Toggles SPAM BLAST mode ON/OFF.
                            * This makes the POW and AUX buttons super sensitive for multiple blaster blocks,
                              but gets in the way of normal features, so deactivate when you're done spamming.
                              Plays mzoom.wav for activating/deactivating this mode.
                              Exits automatically when blade shuts OFF in case you leave it active.
Auto Swing Blast          - Swinging within 1 second of doing a button activated Blaster Block will start this timed mode.
                              To trigger auto blaster blocks, swing saber within 1 second of last block.
                              To exit, stop swinging for 1 second.
                              * Requires #define BC_ENABLE_AUTO_SWING_BLAST.
Toggle Battle Mode        - Double Click and Hold POW, release after a second (while pointing UP). (Click then Long Click)
                            * Power OFF is disabled while in Battle Mode. YOU MUST EXIT THE MODE FIRST.
Force Effect              - Double Click and Hold POW, release after a second (while NOT pointing UP). (Click then Long Click)

Lockup                    - Hold any button + Clash. Release button to end.
                            In Battle Mode:
                              Just Clash and stay there to Lockup.
                              Holding a button while Clashing will do regular Clash without Locking up.
                              Pull away or press a button to end Lockup.
Drag                      - Hold any button + Clash (while pointing DOWN). Release button to end.
Melt                      - No button, just stab something. Rotate hilt to modify melting. Pull away or press POW to end.

Lightning Block           - Hold POW + Click AUX. Release button to end.

Color Change Mode         - 3x Click and Hold POW.
                              Rotate hilt to cycle through all available colors.
                              Click POW to save color selection and Exit.
                              Double Click POW to Exit without saving.
Revert ColorChange        - 3x Click and Hold POW, release after a second. (Double Click then Long Click)
                              Reverts ColorChanged blade color back to the uploaded config color.
                              * This is done outside ColorChange Mode
        ColorChange explained:
          If RotateColorsX<Variation,COLOR> is used in the blade style:
              Rotate hilt to cycle through all available colors,
              Hold POW to save color selection and exit.
          If ColorChange<> is used in the blade style:
              There are up to 12 colors per rotation with a ccchange.wav sound at each step.
              If also using #define COLOR_CHANGE_DIRECT in the config,
              simply entering Color Change Mode will select the next color in the list and exit Color Change Mode.

Quote Player              - 3x Click POW.
                            * Does Force effect if no quote(s) exist.
Toggle Sequential or
       Random quotes      - 3x Click POW (while pointing DOWN).
Force Push                - Push hilt perpendicularly from a stop.
                            Plays push.wav if it exists, otherwise force.wav.
PowerSave Dim Blade       - Hold AUX then Twist (while pointing UP).

User Effect 1             - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
User Effect 2             - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
User Effect 3             - Hold AUX then Rotate Left 60 degrees. (keep holding AUX until executed)
User Effect 4             - Hold AUX then Rotate Right 60 degrees. (keep holding AUX until executed)
                            * Require EFFECT_USER in blade style.
                            * Note the same controls when blade is OFF are USER 5,6,7,8.

Turn OFF blade            - Hold POW until off -or - Twist if using #define BC_TWIST_OFF.
Turn OFF bypass postoff   - Turn OFF (while pointing UP).

---------------------------------------
      2 button summary by clicks
---------------------------------------
--------- When blade is OFF ---------
twist                   - turn both blades ON (requires Gesture defines)
stab
swing
1 click POW             - turn blade ON
                          turn blade ON bypass preon (pointing up)
1 click POW long        - play / stop track
1 click POW held        - scroll presets
    then twist          - turn blade ON muted (back and forth twist)
    then clash
    then rotate left    - user effect 5 (keep holding POW until executed)
    then rotate right   - user effect 6 (keep holding POW until executed)
1 click AUX             - first preset (pointing up)
                          next preset
                          previous preset (pointing down)
1 click AUX held
    then rotate left    - user effect 7 (keep holding AUX until executed)
    then rotate right   - user effect 8 (keep holding AUX until executed)
2 clicks POW            - turn blade ON muted
2 clicks held           - blade length edit, or
                          OS system menu instead (requires #define MENU_SPEC_TEMPLATE)
3 clicks POW            - quote
                          toggle sequential or random quotes (pointing down)
4 clicks long           - manually trigger blade ID scan
4 clicks held           - manually switch to next blade array

- BC Volume Menu
    rotate right        - volume UP
    rotate left         - volume DOWN
    1 click POW held    - quick MAX volume
    1 click AUX held    - quick MIN volume
- OS system menu:
    rotate right/left   - choose options / adjust
    1 click             - select / save
    2 clicks            - exit / cancel

OFF COMBOS:
Hold AUX
    then click POW      - spoken battery level in percentage
                          spoken battery level in volts (pointing down)
    then click POW long - on-demand battery level
Hold POW
    then click AUX      - enter BC volume menu

--------- When blade is ON ---------
1 click POW             - blaster deflection
1 click POW long        - play / stop track
1 click POW held        - turn blade OFF
                          turn blade OFF bypass postoff (pointing up)
    then clash          - lockup
    then twist          - first preset fast (pointing up)
                          next preset fast
                          previous preset fast (pointing down)
    then rotate left    - user effect 1 (keep holding POW until executed)
    then rotate right   - user effect 2 (keep holding POW until executed)
1 click AUX             - blaster deflection
1 click AUX held
    then clash          - lockup
    then rotate left    - user effect 3 (keep holding AUX until executed)
    then rotate right   - user effect 4 (keep holding AUX until executed)
2 clicks POW            - blaster deflection
2 clicks POW long       - force (not pointing up)
                        - toggle battle mode (pointing up)
2 clicks AUX            - blaster deflection
3 clicks POW            - quote
                          toggle sequential or random quotes (pointing down)
3 clicks POW long       - revert colorchange to config
3 clicks POW held       - color change mode
4 clicks POW held       - toggle spam blasts (pointing down)

ON COMBOS:
Hold POW
    then click AUX      - lightning block (keep holding POW, release to end)
    then twist          - first preset fast (pointing up)
                          next preset fast
                          previous preset fast (pointing down)
Hold AUX
    then twist          - dim (pointing up)

hit object              - clash
twist                   - turn blade OFF (requires #define BC_TWIST_OFF)
thrust (air)            - stab
stab object             - melt
push                    - force push


|==================================================== DUAL BLADES CONTROLS ==============================================
|
|   I attempted to keep the controls for this Dual Blade version familiar from the previous version of this prop.
|   Much of it was rewritten from ground up, but I have tested extensively and I think the layout makes sense intuitively.
|   In the spitit of comradery, I adopted many of the 2 button controls from the saber_fett263_buttons prop file.
|   I never had really thought out much for the 2 button portion of this BC prop as I'm mostly a 1 button kind of guy.
|   So, not being too concerned, and also knowing how much time and effort was put into crafting the fett263 prop,
|   I'm pretty sure it's the right controls to provide for any 2 button users.
|   Especially for Dual Blades controls! Makes so much sense.
|
|   **** Dual Blades requires defining BC_DUAL_BLADES and optionally
|        BC_MAIN_BLADE and BC_SECOND_BLADE. See "Optional defines" above.
|   **** Pointing UP/DOWN specific controls, such as sequential quotes or spoken battery level are still based on
|        the MAIN blade's orientation, except for turning blade ON/OFF as listed below.
|   **** Optional BC_TWIST_ON and BC_TWIST_OFF Gesture defines are strongly recommended,
|        practically required to get the right amount of controls.
|
|   Changes to 1 button controls for Dual Blade use:
|     - No Double Click Blaster Blocks. Use Spam Blast if needed.
|     - Lightning Block is now Double Click (was Double Click and Hold POW)
|     - There are other differences, but it's easiest just to use the full instructions below for your button setup.
|
|   For an example pair of blade styles to use for main and second blades (distributed effects) see the following link:
|   Dual_Blade_Example - https://pastebin.com/MFscr2Uf
|
|==================================================== 1 BUTTON DUAL BLADES ===============================================
|
|---------- ON/OFF control ----------
| Turn Main Blade ON                   - Click POW or Thrust main blade forward.
| Turn Main Blade ON First Muted       - Hold POW then Twist.
| Turn Main Blade OFF                  - Hold POW when saber is ON.
|
| Turn 2nd Blade ON                    - Double click POW or Thrust main blade forward.
| Turn 2nd Blade ON First Muted        - 4x Click and Hold POW, release after a second. (Triple Click then Long Click)
| Turn 2nd Blade OFF                   - Double click and Hold POW when saber is ON.
|
| Thrust ON                            - Thrust either blade in its pointed direction to turn it ON.
|
| Turn Both Blades ON                  - Double Click and Hold POW, release after a second. (Double Click then Long Click)
|                                        This turns on both blades if none are ignited.
|                                        or
|                                        Use Gesture ignitions. Plays fastout.wav if available, bypassing font.wav and preon.wav.
| Turn Both Blades ON Muted            - Hold POW then Swing.
|                                        * Optional mute.wav will play before silent ignition and operation. Saber unmutes on retraction.
| Turn Both Blades OFF                 - Twist. Twist is a back-and-forth pair of motions, like revving a motorcycle.
|                                        * Tries to turns off both blades. Therefore even if only one is ON, this will turn it off.
| Turn any Blade ON bypassing preon    - Point either blade up while turning blade(s) ON.
|                                        * Uses fastout.wav if available, bypassing font.wav and preon.wav.
|                                        If no blade is pointing up, both blades will ignite normally with preon if one exists.
| Turn any Blade OFF bypassing postoff - Point either blade up while turning blade(s) OFF.
|
|-- Non-Ignition / Retraction related features:
|-------- When both blades are OFF -------
| Scroll Presets                       - Click and Hold POW.
|                                        Rotate hilt to cycle through presets. Right = Next, Left = Previous.
|                                        Click POW to ignite with chosen preset, or Click and Hold POW again to Exit.
|                                        * TWIST_ON not available in this mode.
| Next Preset                          - Long Click and release POW (NOT pointing MAIN Blade UP or DOWN).
| Prev Preset                          - Long Click and release POW (while pointing MAIN Blade DOWN).
| Jump to First Preset                 - Long Click and release POW (while pointing MAIN Blade UP).
| Play/Stop Track                      - 4x Click POW.
| BC Volume Menu:
|         Enter Menu                   - Hold POW and Clash.
|         Volume UP                    - Rotate Right.
|         Volume DOWN                  - Rotate Left.
|         Quick MAX Vol                - Hold POW while in Volume Menu.
|         Quick MIN Vol                - Double Click and Hold POW while in Volume Menu.
|         Save and Exit                - Click POW.
|         Cancel and Exit              - Double Click POW.
| BC Blade Length Edit:
|         Enter Mode                   - Double Click and Hold POW.
|         Adjust                       - Rotate Right or Left
|         Save and Exit                - Click POW.
|         Cancel and Exit              - Double Click POW.
| ** OS System Menu                      * To use the OS menu system instead of the default BC Volume and BC Blade Length options,
|                                        use #define MENU_SPEC_TEMPLATE
| Spoken Battery Level:
|        in percentage                 - 3x Click and Hold POW.
|        in volts                      - 3x Click and Hold POW (while pointing DOWN).
|                                        * Will show On-blade display if EFFECT_BATTERY_LEVEL is used in blade style.
| On-Demand Battery Level              - 3x Click and Hold POW, release after a second. (Double Click then Long Click)
|
| Quote Player                         - 3x Click POW.
|                                        * Does Force effect if no quote(s) exist.
| Toggle Sequential or Random quotes   - 3x Click and Hold POW (while pointing Main Blade DOWN).
|
| User Effect 5                        - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
| User Effect 6                        - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
|                                        * Requires EFFECT_USER in blade style.
|                                        * Note the same controls when blade is ON are USER 1 and 2.
||||||| NOT AVAILABLE Trigger Blade ID Scan     - 4x click POW and Hold, release after one second (3x Click then Long Click)
| Next Blade Array          - 4x click POW and Hold.
|                             * Cycles to the next blade array in BladeConfig
|
|-------- When a blade is ON --------
| Play/Stop Track                      - 4x Click POW.
| Next Preset Fast                     - Hold POW and Twist (NOT pointing UP or DOWN).
|                                        * Fast switching presets bypasses preon and font.wav.
| Previous Preset Fast                 - Hold POW and Twist (while pointing DOWN).
| First Preset Fast                    - Hold POW and Twist (while pointing UP).
| Clash                                - No button, just hit the blade against something.
|                                        In Battle Mode, Hold POW and Clash to temporarily override the auto-lockup and do regular Clash.
| Stab                                 - Thrust either blade forward with a stabbing motion. Works in Battle Mode.
| Blaster Deflection                   - Click POW.
| Spam Blaster Blocks                  - 4x Click and Hold POW (while pointing DOWN). Toggles SPAM BLAST mode ON/OFF.
|                                        * This makes the POW button super sensitive for multiple blaster blocks,
|                                          but gets in the way of controls for some other features, so deactivate when you're done spamming.
|                                          Plays mzoom.wav for activating/deactivating this mode.
|                                          Exits if either blade shuts OFF in case you leave it active.
| Auto Swing Blast                     - Swinging within 1 second of doing a button activated Blaster Block will start this timed mode.
|                                          To trigger auto blaster blocks, swing saber within 1 second of last block.
|                                          To exit, stop swinging for 1 second.
|                                          * Requires #define BC_ENABLE_AUTO_SWING_BLAST.
| Lockup                               - Hold POW then Clash. Release button to end.
|                                        In Battle Mode:
|                                          Just Clash and stay there to Lockup.
|                                          Holding POW while Clashing will do regular Clash without Locking up.
|                                          Pull away or press POW to end Lockup.
| Drag                                 - Hold POW then Clash (while pointing Main Blade DOWN). Release button to end.
| Melt                                 - No button, just stab something with Main Blade. Pull away or press POW to end.
| Toggle Battle Mode                   - Double Click POW (while pointing UP) to enter and exit Battle Mode.
|                                        * Resets to inactive when either blade shuts OFF.
| Lightning Block                      - Double Click POW (while NOT pointing UP). Click to end.
| Force Effect                         - Double Click and Hold POW, release after a second (Click then Long Click).
| Color Change Mode                    - 3x Click and Hold POW to enter ColorChange mode.
|                                         Rotate hilt to cycle through all available colors, or
|                                         Click POW to save color selection and exit.
|                                         Double Click POW to exit without saving.
| Revert ColorChange                   - 3x Click and Hold POW, release after a second. (Double Click then Long Click)
|                                        Reverts ColorChanged blade color back to the uploaded config color.
|                                        * This is done outside ColorChange Mode
|     ColorChange explained:
|       If RotateColorsX<Variation,COLOR> is used in the blade style:
|           Rotate hilt to cycle through all available colors,
|           Hold POW to save color selection and exit.
|       If ColorChange<> is used in the blade style:
|           There are up to 12 colors per rotation with a ccchange.wav sound at each step.
|           If also using #define COLOR_CHANGE_DIRECT in the config,
|           simply entering Color Change Mode will select the next color in the list and exit Color Change Mode.
|
| Quote Player                         - 3x Click POW.
|                                        * Does Force effect if no quote(s) exist.
| Toggle Sequential or Random quotes   - 3x Click and Hold POW (while pointing Main Blade DOWN).
| Force Push                           - Push hilt perpendicularly from a stop.
| PowerSave Dim Blade                  - 4x Click and Hold POW (while pointing Main Blade UP)
|
| User Effect 1                        - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
| User Effect 2                        - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
|                                        * Require EFFECT_USER in blade style.
|                                        * Note the same controls when blade is OFF are USER 5 and 6.
|
| ---------------------------------------
|  1 button dual blade summary by clicks
| ---------------------------------------
| --------- When both blades are OFF ---------
|
| gestures: (require gesture defines)
| twist or swing          - turn both blades ON
| stab / thrust           - turn stabbed / thrusted blade ON
|
| buttons:
| 1 click                 - turn main blade ON
|                           turn main blade ON first bypass preon (either blade pointing up)
| 1 click long            - first preset (main blade pointing up)
|                           next preset
|                           previous preset (main blade pointing down)
| 1 click held            - enter/exit scroll presets
|     then twist          - turn main blade ON first muted  (back and forth twist)
|     then clash          - enter BC volume menu
|     then rotate left    - user effect 5 (keep holding POW until executed)
|     then rotate right   - user effect 6 (keep holding POW until executed)
|     then swing          - turn both blades ON muted
| 2 clicks                - turn second blade ON
| 2 clicks long           - turn both blades ON
| 2 clicks held           - blade length edit, or
|                           OS system menu instead (requires #define MENU_SPEC_TEMPLATE)
| 3 clicks                - quote
|                           toggle sequential or random quotes (pointing down)
| 3 clicks long           - on-demand battery level
| 3 clicks held           - spoken battery level in percentage
|                           spoken battery level in volts (pointing down)
| 4 clicks                - play / stop track
| 4 clicks long           - turn second blade ON first muted
||||||||| NOT AVAILABLE     4 clicks long           - manually trigger blade ID scan
| 4 clicks held           - manually switch to next blade array
| - BC Volume menu:
|     rotate right        - volume UP
|     rotate left         - volume DOWN
|     1 click POW held    - quick MAX volume
|     1 click AUX held    - quick MIN volume
| - OS system menu:
|     rotate right/left   - choose options / adjust
|     1 click             - select / save
|     2 clicks            - exit / cancel
|
| --------- When a blade is ON ---------
| twist                   - turn both blades OFF (requires Gesture defines)
| 1 click                 - turn main blade ON (if second blade is ON), otherwise blaster deflection
| 1 click held            - turn main blade OFF
|                           turn main blade OFF bypass postoff (pointing either blade UP)
|     then twist          - first preset fast (main blade pointing up)
|                           next preset fast
|                           previous preset fast (main blade pointing down)
|     then clash          - lockup
|     then rotate left    - user effect 1 (keep holding POW until executed)
|     then rotate right   - user effect 2 (keep holding POW until executed)
| 2 clicks                - lightning block (not pointing either blade up).click to end.
|                         - toggle battle mode (pointing either blade up)
| 2 clicks long           - force
| 2 clicks held           - turn second blade OFF
|                           turn second blade OFF bypass postoff (pointing either blade UP)
| 3 clicks                - quote
|                           toggle sequential or random quotes (main blade pointing down)
| 3 clicks long           - revert colorchange to config
| 3 clicks held           - color change mode
| 4 clicks                - play / stop track
| 4 clicks long
| 4 clicks held           - dim (pointing up)
|                           toggle spam blasts (pointing down)
| hit object              - clash
| twist                   - turn all blades OFF (requires #define BC_TWIST_OFF)
| thrust (air)            - stab
| stab object             - melt
| push                    - force push
|
|
|==================================================== 2 BUTTON DUAL BLADES============================================
|
|---------- ON/OFF control ----------
| Turn Main Blade ON                   - Click POW.
| Turn Main Blade ON First Muted       - Double Click POW.
| Turn Main Blade OFF                  - Hold POW when saber is ON.
|
| Turn 2nd Blade ON                    - Click AUX.
| Turn 2nd Blade ON First Muted        - Double Click AUX.
| Turn 2nd Blade OFF                   - Click and Hold AUX when saber is ON.
|
| Thrust ON                            - Thrust either blade in its pointed direction to turn it ON.
|
| Turn Both Blades ON                  - Double Click and Hold AUX, release after a second. (Double Click then Long Click)
|                                        This turns on both blades if none are ignited.
|                                        or
|                                        Use Gesture ignitions. Plays fastout.wav if available, bypassing font.wav and preon.wav.
| Turn Both Blades ON Muted            - Hold POW then Twist.
|                                        * Optional mute.wav will play before silent ignition and operation. Saber unmutes on retraction.
| Turn Both Blades OFF                 - Twist. Twist is a back-and-forth pair of motions, like revving a motorcycle.
|                                        * Tries to turns off both blades. Therefore even if only one is ON, this will turn it off.
| Turn any Blade ON bypassing preon    - Point either blade up while turning blade(s) ON.
|                                        * Uses fastout.wav if available, bypassing font.wav and preon.wav.
|                                        If no blade is pointing up, both blades will ignite normally with preon if one exists.
| Turn any Blade OFF bypassing postoff - Point either blade up while turning blade(s) OFF.
|
|-- Non-Ignition / Retraction related features:
|-------- When both blades are OFF -------
| Scroll Presets                       - Click and Hold POW to toggle this mode ON/OFF. Rotate hilt to cycle next and previous presets.
|                                        * TWIST_ON not available in this mode. Use any ON/OFF control above to ignite with chosen preset.
| Next Preset                          - Click and Hold AUX, release after a second (NOT pointing UP or DOWN).
| Prev Preset                          - Click and Hold AUX, release after a second (while pointing DOWN).
| Jump to First Preset                 - Click and Hold AUX, release after a second (while pointing UP).
| BC Volume Menu:
|         Enter Menu                   - Hold POW then Click AUX.
|         Volume UP                    - Rotate Right.
|         Volume DOWN                  - Rotate Left.
|         Quick MAX Vol                - Hold POW while in Volume Menu.
|         Quick MIN Vol                - Double Click and Hold POW while in Volume Menu.
|         Save and Exit                - Click POW.
|         Cancel and Exit              - Double Click POW.
| BC Blade Length Edit:
|         Enter Mode                   - Double Click and Hold POW.
|         Adjust                       - Rotate Right or Left
|         Save and Exit                - Click POW.
|         Cancel and Exit              - Double Click POW.
| ** OS System Menu                      * To use the OS menu system instead of the default BC Volume and BC Blade Length options,
|                                        use #define MENU_SPEC_TEMPLATE
| Spoken Battery Level:
|         in volts                     - Hold AUX then Click POW.
|         in percentage                - Hold AUX then Click POW (while pointing Main Blade DOWN).
|                                        * Will show On-blade display if EFFECT_BATTERY_LEVEL is used in blade style.
| On-Demand Battery Level              - Hold AUX, then Long Click POW, release POW after a second.
|                                        * Requires EFFECT_BATTERY_LEVEL to be in blade style. Uses battery.wav sound effect.
|
| Quote Player                         - 3x Click POW.
|                                        * Does Force effect if no quote(s) exist.
| Toggle Sequential or Random quotes   - 3x Click and Hold POW (while pointing Main Blade DOWN).
|
| User Effect 5                        - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
| User Effect 6                        - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
| User Effect 7                        - Hold AUX then Rotate Left 60 degrees. (keep holding AUX until executed)
| User Effect 8                        - Hold AUX then Rotate Right 60 degrees. (keep holding AUX until executed)
|                                        * Requires EFFECT_USER in blade style.
|                                        * Note the same controls when blade is ON are USER 1,2,3,4.
| Trigger Blade ID Scan     - 4x click POW and Hold, release after one second (3x Click then Long Click)
| Next Blade Array          - 4x click POW and Hold.
|                             * Cycles to the next blade array in BladeConfig
|
|-------- When a blade is ON -------
| Play/Stop Track                    - Long Click POW.
| Next Preset Fast                     - Hold POW and Twist (NOT pointing UP or DOWN).
|                                        * Fast switching presets bypasses preon and font.wav.
| Previous Preset Fast                 - Hold POW and Twist (while pointing DOWN).
| First Preset Fast                    - Hold POW and Twist (while pointing UP).
| Clash                                - No button, just hit the blade against something.
|                                        In Battle Mode, Hold POW and Clash to temporarily override the auto-lockup and do regular Clash.
| Stab                                 - Thrust either blade forward with a stabbing motion. Works in Battle Mode.
| Blaster Deflection                   - Click or Double click POW or AUX.
| Spam Blaster Blocks                  - 4x Click and Hold POW (while pointing DOWN). Toggles SPAM BLAST mode ON/OFF.
|                                        * This makes the POW button super sensitive for multiple blaster blocks,
|                                          but gets in the way of controls for some other features, so deactivate when you're done spamming.
|                                          Plays mzoom.wav for activating/deactivating this mode.
|                                          Exits if either blade shuts OFF in case you leave it active.
| Auto Swing Blast                     - Swinging within 1 second of doing a button activated Blaster Block will start this timed mode.
|                                         To trigger auto blaster blocks, swing saber within 1 second of last block.
|                                         To exit, stop swinging for 1 second.
|                                         * Requires #define BC_ENABLE_AUTO_SWING_BLAST.
| Lockup                               - Hold AUX then Clash. Release button to end.
|                                        In Battle Mode:
|                                          Just Clash and stay there to Lockup.
|                                          Holding a button while Clashing will do regular Clash without Locking up.
|                                          Pull away or press a button to end Lockup.
| Drag                                 - Hold AUX then Clash (while pointing Main Blade DOWN). Release button to end.
| Melt                                 - No button, just stab something with Main Blade. Pull away or press a button to end.
| Lightning Block                      - Hold POW, Click and release AUX (keep holding power to block, release POW to end.
| Toggle Battle Mode                   - Double Click and Hold POW, release after a second (while pointing UP). (Click then Long Click)
|                                        Resets to inactive when either blade shuts OFF.
| Force Effect                         - Double Click and Hold POW, release after a second (while NOT pointing UP). (Click then Long Click)
| Color Change Mode                    - 3x Click and Hold POW.
|                                          Rotate hilt to cycle through all available colors.
|                                          Click POW to save color selection and Exit.
|                                          Double Click POW to Exit without saving.
| Revert ColorChange                   - 3x Click and Hold POW, release after a second. (Double Click then Long Click)
|                                        Reverts ColorChanged blade color back to the uploaded config color.
|                                        * This is done outside ColorChange Mode
|     ColorChange explained:
|       If RotateColorsX<Variation,COLOR> is used in the blade style:
|           Rotate hilt to cycle through all available colors,
|           Hold POW to save color selection and exit.
|       If ColorChange<> is used in the blade style:
|           There are up to 12 colors per rotation with a ccchange.wav sound at each step.
|           If also using #define COLOR_CHANGE_DIRECT in the config,
|           simply entering Color Change Mode will select the next color in the list and exit Color Change Mode.
|
| Quote Player                         - 3x Click POW.
|                                        * Does Force effect if no quote(s) exist.
| Toggle Sequential or Random quotes   - 3x Click and Hold POW (while pointing Main Blade DOWN).
|
| Force Push                           - Push hilt perpendicularly from a stop.
| PowerSave Dim Blade                  - Hold AUX then Twist (while pointing Main Blade UP).
|
| User Effect 1                        - Hold POW then Rotate Left 60 degrees. (keep holding POW until executed)
| User Effect 2                        - Hold POW then Rotate Right 60 degrees. (keep holding POW until executed)
| User Effect 3                        - Hold AUX then Rotate Left 60 degrees. (keep holding AUX until executed)
| User Effect 4                        - Hold AUX then Rotate Right 60 degrees. (keep holding AUX until executed)
|                                        * Require EFFECT_USER in blade style.
|                                        * Note the same controls when blade is OFF are USER 5,6,7,8.
|
| ---------------------------------------
|  2 button dual blade summary by clicks
| ---------------------------------------
| --------- When both blades are OFF ---------
|
| gestures: (require gesture defines)
| twist or swing          - turn both blades ON
| stab / thrust           - turn stabbed / thrusted blade ON
|
| buttons:
| 1 click POW             - turn main blade ON
|                           turn main blade ON first bypass preon (either blade pointing up)
| 1 click POW long        - play / stop track
| 1 click POW held        - scroll presets
|     then twist          - turn both blades blade ON muted (back and forth twist)
|     then rotate left    - user effect 5 (keep holding POW until executed)
|     then rotate right   - user effect 6 (keep holding POW until executed)
| 1 click AUX             - turn second blade ON
|                           turn second blade ON first bypass preon (either blade pointing up)
| 1 click AUX long        - first preset (pointing up)
|                           next preset
|                           previous preset (pointing down)
| 1 click AUX held
|     then rotate left    - user effect 7 (keep holding AUX until executed)
|     then rotate right   - user effect 8 (keep holding AUX until executed)
| 2 clicks POW            - turn main blade ON first muted
| 2 clicks POW held       - blade length edit, or
|                           OS system menu instead (requires #define MENU_SPEC_TEMPLATE)
| 2 clicks AUX            - turn second blade ON first muted
| 2 clicks AUX long       - turn both blades ON
|                         - turn both blades ON bypass preon (either blade pointing up)
| 3 clicks POW            - quote
|                           toggle sequential or random quotes (pointing down)
| 4 clicks long           - manually trigger blade ID scan
| 4 clicks held           - manually switch to next blade array
| - BC Volume menu:
|     rotate right        - volume UP
|     rotate left         - volume DOWN
|     1 click POW held    - quick MAX volume
|     1 click AUX held    - quick MIN volume
| - OS system menu:
|     rotate right/left   - choose options / adjust
|     1 click             - select / save
|     2 clicks            - exit / cancel
|
| OFF COMBOS:
| Hold AUX
|     then click POW      - spoken battery level in percentage
|                           spoken battery level in volts (pointing down)
|     then click POW long - on-demand battery level
| Hold POW
|     then click AUX      - enter BC volume menu
|
| --------- When a blade is ON ---------
| 1 click POW             - turn main blade ON (if second blade is ON), otherwise blaster deflection
| 1 click POW long        - play / stop track
| 1 click POW held        - turn main blade OFF
|                           turn main blade OFF bypass postoff (pointing either blade UP)
|     then twist          - first preset fast (main blade pointing up)
|                           next preset fast
|                           previous preset fast (main blade pointing down)
|     then clash          - lockup
|     then rotate left    - user effect 1 (keep holding POW until executed)
|     then rotate right   - user effect 2 (keep holding POW until executed)
| 1 click AUX             - turn second blade ON (if main blade is ON), otherwise blaster deflection
| 1 click AUX held        - turn second blade OFF
|                           turn second blade OFF bypass postoff (pointing either blade UP)
|     then clash          - lockup
|                           drag (main blade pointing down)
|     then rotate left    - user effect 3 (keep holding AUX until executed)
|     then rotate right   - user effect 4 (keep holding AUX until executed)
| 2 clicks POW            - blaster deflection
| 2 clicks POW long       - force (not pointing up)
|                         - toggle battle mode (pointing up)
| 2 clicks AUX            - blaster deflection
| 2 clicks AUX long       - turn all blades OFF (any remaining blade)
| 3 clicks POW            - quote
|                           toggle sequential or random quotes (main blade pointing down)
| 3 clicks POW long       - revert colorchange to config
| 3 clicks POW held       - color change mode
| 4 clicks POW held       - toggle spam blasts (pointing down)
|
| ON COMBOS:
| Hold POW
|     then click AUX      - lightning block (keep holding POW, release to end)
|     then twist          - first preset fast (pointing up)
|                           next preset fast
|                           previous preset fast (pointing down)
| Hold AUX
|     then twist          - dim (pointing up)
|
| hit object              - clash
| twist                   - turn all blades OFF (requires #define BC_TWIST_OFF)
| thrust (air)            - stab
| stab object             - melt
| push                    - force push
|
|
|==================================================== 3 BUTTON DUAL BLADES============================================
| * Note * - The main difference for 3 button control is the independent control for a second blade. All else is the same as 2 button above.
|
|---------- ON/OFF control ----------
| Turn 2nd Blade ON                    - Click AUX2 (button 3).
| Turn 2nd Blade ON First Muted        - Double Click AUX2.
| Turn 2nd Blade OFF                   - Click and Hold AUX2.
| Blaster Blocks                       - Click or Double Click AUX2
|      (Blaster Blocks are single or Double Clicks on all buttons)
| ---------------------------------------
|  3 button dual blade summary by clicks
| ---------------------------------------
| --------- When blade is OFF ---------
|
| 1 click AUX2            - turn second blade ON
|                           turn second blade ON first bypass preon (either blade pointing up)
| --------- When blade is ON ---------
|
| 1 click AUX2 held       - turn second blade OFF
|                           turn second blade OFF bypass postoff (pointing either blade UP)
|
| If you have any questions feel free to post them on The Crucible forum
| https://crucible.hubbe.net. Tag me @NoSloppy.
*/

#ifndef PROPS_SABER_BC_BUTTONS_H
#define PROPS_SABER_BC_BUTTONS_H

#include "prop_base.h"
#include "../sound/hybrid_font.h"
#include "../sound/sound_library.h"
#include "../modes/select_cancel_mode.h"
#include "../modes/settings_menues.h"

#undef PROP_TYPE
#define PROP_TYPE SaberBCButtons

#ifndef MOTION_TIMEOUT
#define MOTION_TIMEOUT 60 * 15 * 1000
#endif

#ifndef BC_SWING_ON_SPEED
#define BC_SWING_ON_SPEED 250
#endif

#ifndef BC_LOCKUP_DELAY
#define BC_LOCKUP_DELAY 200
#endif

#ifndef BC_FORCE_PUSH_LENGTH
#define BC_FORCE_PUSH_LENGTH 5
#endif

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
#define BUTTON_HELD_MEDIUM_TIMEOUT 800
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif

#ifndef BC_MAIN_BLADE
#define BC_MAIN_BLADE 1
#endif

#ifndef BC_SECOND_BLADE
#define BC_SECOND_BLADE 2
#endif

#ifdef BC_DUAL_BLADES
#if BC_MAIN_BLADE == BC_SECOND_BLADE
#error BC_MAIN_BLADE and BC_SECOND_BLADE cannot be the same
#endif
#endif

#if defined(NO_BLADE_NO_GEST_ONOFF) && !defined(BLADE_DETECT_PIN) && !defined(BLADE_ID_SCAN_MILLIS)
#error Using NO_BLADE_NO_GEST_ONOFF requires either BLADE_DETECT_PIN or BLADE_ID_SCAN_MILLIS to be defined.
#endif

#if defined(BC_NO_BM) && defined(BC_GESTURE_AUTO_BATTLE_MODE)
#error You cannot define both BC_NO_BM and BC_GESTURE_AUTO_BATTLE_MODE
#endif

#ifdef BC_DUAL_BLADES
  static constexpr BladeSet BC_MAIN_BLADE_SET = BladeSet::fromBlade(BC_MAIN_BLADE);
  static constexpr BladeSet BC_SECOND_BLADE_SET = BladeSet::fromBlade(BC_SECOND_BLADE);
#endif

EFFECT(dim);        // for EFFECT_POWERSAVE
EFFECT(battery);    // for EFFECT_BATTERY_LEVEL
EFFECT(bmbegin);    // for Begin Battle Mode
EFFECT(bmend);      // for End Battle Mode
EFFECT(push);       // for Force Push gesture
EFFECT(tr);         // for EFFECT_TRANSITION_SOUND, use with User Effects.
EFFECT(mute);       // Notification before muted ignition to avoid confusion.
EFFECT(array);      // for Manual Blade Array switching

template<class SPEC>
struct BCScrollPresetsMode : public SPEC::SteppedMode {

  void next() override {
    beeper.Beep(0.05, 4000);
    prop_next_preset();
  }

  void prev() override {
    beeper.Beep(0.05, 3000);
    prop_previous_preset();
  }

  void update() override {  // Overridden to substitute the tick sound
#ifdef ENABLE_IDLE_SOUND
    SFX_font.SetFollowing(nullptr);
#endif
  }

  void exit() override {
    PVLOG_NORMAL << "** Exit Scroll Presets\n";
    beeper.Beep(0.05, 3000);
    beeper.Silence(0.05);
    beeper.Beep(0.05, 3000);
    beeper.Silence(0.05);
    beeper.Beep(0.10, 2000);
    SPEC::SteppedMode::exit();
  }

  // Custom button controls for Scroll Presets mode.
  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, 0)) {

      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, 0):
        SaberBase::TurnOn();
        exit();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, 0):
        exit();
        return true;
    }
    return false;
  }
};

template<class SPEC>
struct BCVolumeMode : public SPEC::SteppedMode {
  const int max_volume_ = VOLUME;
  const int min_volume_ = VOLUME * 0.10;
  float initial_volume_ = 0.0;
  int initial_percentage_ = 0;
  int percentage_ = 0;

  void mode_activate(bool onreturn) override {
    PVLOG_NORMAL << "** Enter Volume Menu\n";
    initial_volume_ = dynamic_mixer.get_volume();
    initial_percentage_ = round((initial_volume_ / max_volume_) * 10) * 10;
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
    mode::getSL<SPEC>()->SayEditVolume();
    announce_volume();
    SPEC::SteppedMode::mode_activate(onreturn);
  }

  void announce_volume() {
    if (percentage_ <= 10) {
      mode::getSL<SPEC>()->SayMinimumVolume();
    } else if (percentage_ >=100) {
      mode::getSL<SPEC>()->SayMaximumVolume();
    } else {
      mode::getSL<SPEC>()->SayWhole(percentage_);
      mode::getSL<SPEC>()->SayPercent();
    }
  }

  void mode_deactivate() override {
    announce_volume();
    mode::getSL<SPEC>()->SayVolumeMenuEnd();
    SPEC::SteppedMode::mode_deactivate();
  }

  void next() override {
    int current_volume_ = dynamic_mixer.get_volume();
    if (current_volume_ < max_volume_) {
      current_volume_ += max_volume_ * 0.10;
      if (current_volume_ >= max_volume_) {
        current_volume_ = max_volume_;
        QuickMaxVolume();
      } else {
        mode::getSL<SPEC>()->SayVolumeUp();
      }
      dynamic_mixer.set_volume(current_volume_);
    }
  }

  void prev() override {
    int current_volume_ = dynamic_mixer.get_volume();
    if (current_volume_ > min_volume_) {
      current_volume_ -= max_volume_ * 0.10;
      if (current_volume_ <= min_volume_) {
        current_volume_ = min_volume_;
        QuickMinVolume();
      } else {
        mode::getSL<SPEC>()->SayVolumeDown();
      }
      dynamic_mixer.set_volume(current_volume_);
    }
  }

  void QuickMaxVolume() {
    dynamic_mixer.set_volume(max_volume_);
    PVLOG_NORMAL << "** Maximum Volume\n";
    mode::getSL<SPEC>()->SayMaximumVolume();
  }

  void QuickMinVolume() {
    dynamic_mixer.set_volume(min_volume_);
    PVLOG_NORMAL << "** Minimum Volume\n";
    mode::getSL<SPEC>()->SayMinimumVolume();
  }

  void update() override {  // Overridden to substitute the tick sound
    float volume = dynamic_mixer.get_volume();
    percentage_ = round((volume / max_volume_) * 10) * 10;
    SaberBase::DoEffect(EFFECT_VOLUME_LEVEL, 0);
  }

  void select() override {
    PVLOG_NORMAL << "** Saved - Exit Volume Menu\n";
    mode::getSL<SPEC>()->SaySave();
    SPEC::SteppedMode::select();
  }

  void exit() override {
    PVLOG_NORMAL << "** Cancelled - Exit Volume Menu\n";
    percentage_ = initial_percentage_;
    dynamic_mixer.set_volume(initial_volume_);
    mode::getSL<SPEC>()->SayCancel();
    SPEC::SteppedMode::exit();
  }

  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, 0)) {
      // Custom button controls for BCVolumeMode
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, 0):
        QuickMaxVolume();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, 0):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, 0):
        QuickMinVolume();
        return true;
    }
    // Use the select and exit controls from SelectCancelMode
    return SPEC::SelectCancelMode::mode_Event2(button, event, modifiers);
  }
};

#ifdef DYNAMIC_BLADE_LENGTH

template<class SPEC>
struct BCSelectBladeMode : public SPEC::MenuBase {
  int current_blade() { return this->pos_ + 1; }
  ShowColorSingleBladeTemplate<Pulsing<White,Black,800>,Pulsing<White,Black,800>> highlighted_blade_;
  bool single_blade_adjusted_ = false;
  uint16_t size() override { return NUM_BLADES; }

  void mode_activate(bool onreturn) override {
    mode::getSL<SPEC>()->SaySelectBlade();
    SPEC::SteppedMode::mode_activate(onreturn);
    highlighted_blade_.Start(current_blade());
    PVLOG_NORMAL << "** Highlighting blade to edit: " << current_blade() << "\n";
  }

  void mode_deactivate() {
    prop_UpdateStyle();
  }

  void next() override {
    highlighted_blade_.Stop(current_blade());
    SPEC::MenuBase::next();
    highlighted_blade_.Start(current_blade());
    PVLOG_NORMAL << "** Highlighting blade to edit: " << current_blade() << "\n";
  }

  void prev() override {
    highlighted_blade_.Stop(current_blade());
    SPEC::MenuBase::prev();
    highlighted_blade_.Start(current_blade());
    PVLOG_NORMAL << "** Highlighting blade to edit: " << current_blade() << "\n";
  }

  void say() override {
    mode::getSL<SPEC>()->SayBlade();
    mode::getSL<SPEC>()->SayWhole(current_blade());
  }

  void select() override {
    // Set the current blade to send and push to ChangeBladeLengthMode
    PVLOG_NORMAL << "** Editing blade length.\n";
    mode::menu_current_blade = current_blade();
    highlighted_blade_.Stop(current_blade());
    popMode();
    pushMode<typename SPEC::ChangeBladeLengthMode>();
  }
};

template<class SPEC>
struct BCChangeBladeLengthBlade1 : public mode::ChangeBladeLengthBlade1<SPEC> {

  int steps_per_revolution() override {
    return 30;  // adjust for sensitivity
  }

  void select() override {
    PVLOG_NORMAL << "** Saved blade length : " << this->getLength() << "\n";
    prop_SetBladeLength(this->blade(), this->getLength());
    prop_SaveState();
    mode::getSL<SPEC>()->SaySave();
    popMode();
  }

  void update() override {
    hybrid_font.PlayPolyphonic(&SFX_mclick);
    this->say_time_ = Cyclint<uint32_t>(millis()) + (uint32_t)(SaberBase::sound_length * 1000) + 300;
    if (!this->say_time_) this->say_time_ += 1;
    this->fadeout(SaberBase::sound_length);
  }
};
#endif  // DYNAMIC_BLADE_LENGTH

template<class SPEC>
struct BCMenuSpec {
  typedef BCVolumeMode<SPEC> BCVolumeMenu;
  typedef BCScrollPresetsMode<SPEC> BCScrollPresetsMenu;
#ifdef DYNAMIC_BLADE_LENGTH
  typedef BCSelectBladeMode<SPEC> BCSelectBladeMenu;
  typedef mode::ShowLengthStyle<SPEC> ShowLengthStyle;
  typedef BCChangeBladeLengthBlade1<SPEC> ChangeBladeLengthMode;
#endif  // DYNAMIC_BLADE_LENGTH
  typedef mode::SelectCancelMode SelectCancelMode;
  typedef mode::SteppedMode<SPEC> SteppedMode;
  typedef mode::SteppedModeBase<SPEC> SteppedModeBase;
  typedef mode::MenuBase<SPEC> MenuBase;
  typedef SoundLibraryV2 SoundLibrary;
};

class DelayTimer {
public:
  DelayTimer() : triggered_(false), trigger_time_(0), duration_(0) {}

  void trigger(uint32_t duration) {
    triggered_ = true;
    trigger_time_ = millis();
    duration_ = duration;
  }

  void stopTimer() {
    PVLOG_DEBUG << "** STOPPING timer.\n";
    triggered_ = false;
  }

  bool isTimerExpired() {
    if (!triggered_) return false;
    if (millis() - trigger_time_ > duration_) {
      stopTimer();
      return true;  // Timer has elapsed
    }
    return false;  // Timer is still running
  }

private:
  bool triggered_;
  uint32_t trigger_time_;
  uint32_t duration_;
};

// The Saber class implements the basic states and actions
// for the saber.
class SaberBCButtons : public PROP_INHERIT_PREFIX PropBase {
public:
  SaberBCButtons() : PropBase() {}
  const char* name() override { return "SaberBCButtons"; }

#ifndef MENU_SPEC_TEMPLATE
  void Setup() override{
    MKSPEC<BCMenuSpec>::SoundLibrary::init();
  }
#endif

#if defined(DYNAMIC_BLADE_LENGTH) && !defined(MENU_SPEC_TEMPLATE)
  void EnterBladeLengthMode() {
    if (!current_style()) return;
    if (current_mode == this) {
      sound_library_.SayEditBladeLength();

#if NUM_BLADES > 1
        pushMode<MKSPEC<BCMenuSpec>::BCSelectBladeMenu>();
#else
        pushMode<MKSPEC<BCMenuSpec>::ChangeBladeLengthMode>();
#endif

    }
  }
#endif  // DYNAMIC_BLADE_LENGTH

  void EnterVolumeMenu() {
    if (!current_style()) return;
    if (current_mode == this) {
#ifdef MENU_SPEC_TEMPLATE
      sound_library_.SayEditVolume();
      pushMode<MKSPEC<MENU_SPEC_TEMPLATE>::ChangeVolumeMode>();
#else
      pushMode<MKSPEC<BCMenuSpec>::BCVolumeMenu>();
#endif
    }
  }

  void EnterScrollPresets() {
    if (current_mode == this) {
      PVLOG_NORMAL << "** Enter Scroll Presets\n";
      BeepEnterFeature();
#ifdef ENABLE_IDLE_SOUND
      hybrid_font.StopIdleSound();
      SFX_font.SetFollowing(nullptr);
#endif
      scroll_presets_beep_delay_timer_.trigger(350);
      pushMode<MKSPEC<BCMenuSpec>::BCScrollPresetsMenu>();
    }
  }

  void Loop() override {
    PropBase::Loop();
    DetectTwist();
    Vec3 mss = fusor.mss();
    sound_library_.Poll(wav_player);
    if (wav_player && !wav_player->isPlaying()) wav_player.Free();
    if (SaberBase::IsOn()) {
      DetectSwing();
      if (auto_lockup_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 120 &&
          millis() - clash_impact_millis_ > BC_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_lockup_on_ = false;
      }
      if (auto_melt_on_ &&
          !swinging_ &&
          fusor.swing_speed() > 60 &&
          millis() - clash_impact_millis_ > BC_LOCKUP_DELAY &&
          SaberBase::Lockup()) {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        auto_melt_on_ = false;
      }
//EVENT_PUSH
      if (fabs(mss.x) < 3.0 &&
          mss.y * mss.y + mss.z * mss.z > 70 &&
          fusor.swing_speed() < 30 &&
          fabs(fusor.gyro().x) < 10) {
        if (millis() - push_begin_millis_ > BC_FORCE_PUSH_LENGTH) {
          Event(BUTTON_NONE, EVENT_PUSH);
          push_begin_millis_ = millis();
        }
      } else {
        push_begin_millis_ = millis();
      }
    } else {  // SaberBase is not ON
// EVENT_SWING - Swing On gesture control to allow fine tuning of speed needed to ignite
      if (millis() - saber_off_time_ < MOTION_TIMEOUT) {
        SaberBase::RequestMotion();
        if (swinging_ && fusor.swing_speed() < 90) {
          swinging_ = false;
        }
        if (!swinging_ && fusor.swing_speed() > BC_SWING_ON_SPEED) {
          swinging_ = true;
          Event(BUTTON_NONE, EVENT_SWING);
        }
      }
    }
// EVENT_THRUST
    if (mss.y * mss.y + mss.z * mss.z < 20.0 &&  // slightly more forgiving than 16.0?
       (fabs(mss.x) > 14) &&  // Check for both positive and negative x-motion
        fusor.swing_speed() < 150) {
      if (millis() - thrust_begin_millis_ > 15) {
        Event(BUTTON_NONE, EVENT_THRUST);
        thrust_begin_millis_ = millis();
      }
    } else {
      thrust_begin_millis_ = millis();
    }
// Timers
    // Play optional mute.wav first.
    if (mute_all_delay_timer_.isTimerExpired()) {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnOnHelper();
      }
    }
#ifdef BC_DUAL_BLADES
    if (mute_mainBlade_delay_timer_.isTimerExpired()) {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnBladeOn(BC_MAIN_BLADE_SET);
        PVLOG_NORMAL << "** Main Blade Turned ON Muted\n";
      }
    }
    if (mute_secondBlade_delay_timer_.isTimerExpired()) {
      if (SetMute(true)) {
        unmute_on_deactivation_ = true;
        muted_ = true;
        TurnBladeOn(BC_SECOND_BLADE_SET);
        PVLOG_NORMAL << "** Second Blade Turned ON Muted\n";
      }
    }
#endif
    // Delaying playing font.wav so that beep can be heard first.
    if (scroll_presets_beep_delay_timer_.isTimerExpired()) {
        SaberBase::DoEffect(EFFECT_NEWFONT, 0);
    }
  }  // Loop()

#ifdef SPEAK_BLADE_ID
  void SpeakBladeID(float id) override {
    sound_library_v2.SayBlade();
    sound_library_.SayNumber(id, SAY_WHOLE);
  }
#endif

void DetectTwist() {
  bool process = DetectTwistStrokes();
  if (process) {
    if (ProcessTwistEvents()) {
      // Normal twist event happened, clear strokes
      strokes[NELEM(strokes)-1].type = UNKNOWN_GESTURE;
      strokes[NELEM(strokes)-2].type = UNKNOWN_GESTURE;
      return;
    }
  }
  DoSavedTwist();
}

void DoSavedTwist() {
  Stroke* stroke = &strokes[NELEM(strokes)-1];
  switch (stroke->type) {
    case TWIST_LEFT:
    case TWIST_RIGHT:
      if (stroke->end_millis == 0) return;
      if (stroke->length() < 100) return;
      if (millis() - stroke->end_millis < 300) return;
      // Add another check for separation from previous stroke?
      break;
    default:
      return;
  }
  // Emit single twist event
  Event(BUTTON_NONE, stroke->type == TWIST_LEFT ? EVENT_TWIST_LEFT : EVENT_TWIST_RIGHT);
  PVLOG_DEBUG << (stroke->type == TWIST_LEFT ? "EVENT_TWIST_LEFT" : "EVENT_TWIST_RIGHT") << "\n";
  // Prevent re-triggering
  stroke->type = UNKNOWN_GESTURE;
}

  void BeepEnterFeature() {
    beeper.Beep(0.05, 2000);
    beeper.Silence(0.05);
    beeper.Beep(0.05, 2000);
    beeper.Silence(0.05);
    beeper.Beep(0.10, 3000);
  }

  void BeepExitFeature() {
    beeper.Beep(0.05, 3000);
    beeper.Silence(0.05);
    beeper.Beep(0.05, 3000);
    beeper.Silence(0.05);
    beeper.Beep(0.10, 2000);
  }

  bool isPointingUp() {
    if (fusor.angle1() > M_PI / 3) return true;
#ifdef BC_DUAL_BLADES
    if (fusor.angle1() < -M_PI / 3) return true;
#endif
    return false;
  }

  void TurnOnHelper() {
    if (isPointingUp() || muted_) {
      FastOn();
    } else {
      On();
    }
  }

  void TurnOffHelper() {
    if (SaberBase::Lockup() || battle_mode_) return;
    if (isPointingUp()) {
      Off(OFF_FAST);
    } else {
      Off();
    }
    saber_off_time_ = millis();
    battle_mode_ = false;
    spam_blast_ = false;
    muted_ = false;
  }

  void MuteAll() {
    if (hybrid_font.PlayPolyphonic(&SFX_mute)) {
      mute_all_delay_timer_.trigger(SaberBase::sound_length * 1000);
    } else {
      mute_all_delay_timer_.trigger(0);
    }
  }

  // Previous, next, or first preset, depending on blade angle
  void DoChangePreset() {
    if (fusor.angle1() > M_PI / 3) {
    // Main Blade pointing UP
      first_preset();
      PVLOG_NORMAL << "** Jumped to first preset\n";
    } else if (fusor.angle1() < -M_PI / 3) {
    // Main Blade pointing DOWN
      previous_preset();
      PVLOG_NORMAL << "** Previous preset\n";
    } else {
    // Main Blade NOT pointing UP or DOWN
      next_preset();
      PVLOG_NORMAL << "** Next preset\n";
    }
  }

  // When ON, use FastOn() for previous, next, or first preset, depending on blade angle
  void DoChangePresetFast() {
    if (fusor.angle1() > M_PI / 3) {
      // Main Blade pointing UP
      first_preset_fast();
      PVLOG_NORMAL << "** Jumped to first preset fast\n";
    } else if (fusor.angle1() < -M_PI / 3) {
      // Main Blade pointing DOWN
      previous_preset_fast();
      PVLOG_NORMAL << "** Previous preset fast\n";
    } else {
      next_preset_fast();
      PVLOG_NORMAL << "** Next preset fast\n";
    }
    // Trying to maintain mute through Fast preset change
    if (muted_) {
      unmute_on_deactivation_ = true;
      SetMute(true);
    }
  }

  void DoSpokenBatteryLevel() {
    // Avoid weird battery readings when using USB
    if (battery_monitor.battery() < 0.5) {
      sound_library_.SayTheBatteryLevelIs();
      sound_library_.SayDisabled();
      return;
    }
    sound_library_.SayTheBatteryLevelIs();
    // pointing DOWN
    if (fusor.angle1() < -M_PI / 4) {
      sound_library_.SayNumber(battery_monitor.battery(), SAY_DECIMAL);
      sound_library_.SayVolts();
      PVLOG_NORMAL << "Battery Voltage: " << battery_monitor.battery() << "\n";
      is_speaking_ = true;
      SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
    } else {
      sound_library_.SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
      sound_library_.SayPercent();
      PVLOG_NORMAL << "Battery Percentage: " <<battery_monitor.battery_percent() << "%\n";
      is_speaking_ = true;
      SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
    }
  }

  void OnDemandBatteryLevel() {
    PVLOG_NORMAL << "Battery Voltage: " << battery_monitor.battery() << "\n";
    PVLOG_NORMAL << "Battery Percentage: " <<battery_monitor.battery_percent() << "\n";
    SaberBase::DoEffect(EFFECT_BATTERY_LEVEL, 0);
  }

  void DoTrackStartOrStop() {
    if (spam_blast_) return;
    PVLOG_NORMAL << "** Track playback Toggled\n";
    StartOrStopTrack();
  }

  void DoLockup() {
    if (!SaberBase::Lockup() && SaberBase::IsOn()) {

#ifdef BC_DUAL_BLADES
      if (isMainBladeOn() && !isSecondBladeOn()) {
        MainBladeLockupOrDrag();
      } else if (!isMainBladeOn() && isSecondBladeOn()) {
        SecondBladeLockupOrDrag();
      } else {  // Both blades must be ON
        DualBladesLockupOrDrag();
      }
#else
      // Single blade scenario (non-dual blades)
      if (fusor.angle1() < -M_PI / 4) {
        SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
      } else {
        if (!battle_mode_) {
          SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        } else {
          // Overrides Auto-lockup if holding Button during clash, NOT pointing DOWN
          return;
        }
      }
#endif

      SaberBase::DoBeginLockup();
    }
  }

  void DoLightningBlock() {
    if (spam_blast_ || on_pending_) return;
    SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
    SaberBase::DoBeginLockup();
  }

  void DoBlasterBlock() {
    if (spam_blast_ || on_pending_) return;
#ifdef BC_DUAL_BLADES
      SaberBase::DoEffect(EFFECT_BLAST, EffectLocation(0, GetBladeAboveHorizon()));
#else
      SaberBase::DoBlast();
#endif
      last_blast_millis_ = millis();
  }

  void DoQuote() {
    if (spam_blast_) return;
    if (SFX_quote) {
      if (GetWavPlayerPlaying(&SFX_quote)) return;  // Simple prevention of quote overlap

      sequential_quote_ ? SFX_quote.SelectNext() : SFX_quote.Select(-1);
      SaberBase::DoEffect(EFFECT_QUOTE, 0);
    } else {
      if (GetWavPlayerPlaying(&SFX_force)) return;
      SaberBase::DoForce();
    }
  }

  void DoSpamBlast() {
    if (!spam_blast_) return;
    SaberBase::DoBlast();
    last_blast_millis_ = millis();
  }

  void ToggleSequentialQuote() {
    if (spam_blast_) return;
    sequential_quote_ = !sequential_quote_;
    PVLOG_NORMAL << (sequential_quote_ ? "** Quotes play sequentially\n" : "** Quotes play randomly\n");
    if (SFX_mnum) {
      sequential_quote_ ? sound_library_v2.SaySequential() : sound_library_.SayRandom();
    } else {
      beeper.Beep(0.5, sequential_quote_ ? 3000 : 1000);
    }
  }

  void ToggleSpamBlast() {
    spam_blast_ = !spam_blast_;
    PVLOG_NORMAL << (spam_blast_ ? "** Entering" : "** Exiting") << " Spam Blast Mode\n";
    sound_library_.SayZoomingIn();
  }

  void ToggleBattleMode() {
#ifndef BC_NO_BM
    battle_mode_ = !battle_mode_;
    PVLOG_NORMAL << (battle_mode_ ? "** Entering" : "** Exiting") << " Battle Mode\n";
    if (battle_mode_) {
      if (!hybrid_font.PlayPolyphonic(&SFX_bmbegin)) {
        hybrid_font.PlayPolyphonic(&SFX_force);
      }
    } else {
      if (!hybrid_font.PlayPolyphonic(&SFX_bmend)) {
        beeper.Beep(0.5, 3000);
      }
    }
#endif
  }

  void GestureEnableBattleMode() {
#ifdef BC_GESTURE_AUTO_BATTLE_MODE
    PVLOG_NORMAL << "** Auto Entering Battle Mode\n";
    battle_mode_ = true;
#endif
  }

  void NoBladeDisableGestures() {
#ifdef NO_BLADE_NO_GEST_ONOFF
    if (!blade_present()) return;
#endif
  }

#ifdef BC_DUAL_BLADES

  static const BladeSet controlled_blades_;

  bool isMainBladeOn() { return SaberBase::OnBlades()[BC_MAIN_BLADE]; }
  bool isSecondBladeOn() { return SaberBase::OnBlades()[BC_SECOND_BLADE]; }

  // Get blade location to apply effect to
  BladeSet GetBladeAboveHorizon() {
    return fusor.angle1() > 0 ? BC_MAIN_BLADE_SET : BC_SECOND_BLADE_SET;
  }

  void TurnBladeOn(BladeSet target_blade) {
    // Add in all non-controlled blades, effectively excluding the "other" blade.
    target_blade = target_blade | ~controlled_blades_;
    if (SaberBase::OnBlades().off()) {
      PVLOG_DEBUG << "**** No blades are currently ON, turning on the "
                   << (target_blade[BC_MAIN_BLADE] ? "MAIN" : "SECOND")
                   << " blade and all others, excluding the "
                   << (target_blade[BC_MAIN_BLADE] ? "SECOND" : "MAIN")
                   << " blade\n";
      if (isPointingUp() || muted_) {
        FastOn(EffectLocation(0, target_blade));
     } else {
        On(EffectLocation(0, target_blade));
      }
    } else {
      PVLOG_DEBUG << "**** Turning on the "
                   << (target_blade[BC_MAIN_BLADE] ? "MAIN" : "SECOND")
                   << " blade\n";
      SaberBase::TurnOn(EffectLocation(0, target_blade));
    }
  }

  void TurnBladeOff(BladeSet target_blade) {
    SaberBase::OffType off_type = isPointingUp() ? SaberBase::OFF_FAST : SaberBase::OFF_NORMAL;
    PVLOG_DEBUG << "***** Off type: " << (off_type == SaberBase::OFF_FAST ? "OFF_FAST" : "OFF_NORMAL") << "\n";

    // Check if this is the only blade ON (of MAIN or SECOND blades)
    if ((SaberBase::OnBlades() & ~target_blade & controlled_blades_).off()) {
      // Other blade is not on, so just do normal Off() with appropriate off_type
      PVLOG_DEBUG << "**** Turning OFF all blades\n";
      Off(off_type);
      muted_ = false;
    } else {
      // Only Turn OFF this blade, leave the other one ON.
      PVLOG_DEBUG << "**** Turning OFF only the "
                   << (target_blade[BC_MAIN_BLADE] ? "MAIN" : "SECOND")
                   << " blade\n";
      SaberBase::TurnOff(off_type, EffectLocation(0, target_blade));
    }
    battle_mode_ = false;
    spam_blast_ = false;
    saber_off_time_ = millis();
  }

  void TurnMainBladeOnMuted() {
    if (hybrid_font.PlayPolyphonic(&SFX_mute)) {
      mute_mainBlade_delay_timer_.trigger(SaberBase::sound_length * 1000);
    } else {
      mute_mainBlade_delay_timer_.trigger(0);
    }
  }

  void TurnSecondBladeOnMuted() {
    if (hybrid_font.PlayPolyphonic(&SFX_mute)) {
      mute_secondBlade_delay_timer_.trigger(SaberBase::sound_length * 1000);
    } else {
      mute_mainBlade_delay_timer_.trigger(0);
    }
  }

  // Determine the active blade based on x-axis motion - for thrust effects
  EffectLocation GetThrustBladeLocation() {
    mss = fusor.mss();
    if (mss.x < 14) {
      thrusting_blade_ = BC_MAIN_BLADE_SET;
    } else if (mss.x > -14) {
      thrusting_blade_ = BC_SECOND_BLADE_SET;
    }
    return EffectLocation(0, thrusting_blade_);
  }

  void DoAccel(const Vec3& accel, bool clear) override {
    fusor.DoAccel(accel, clear);
    accel_loop_counter_.Update();
    Vec3 diff = fusor.clash_mss();
    float v;
    if (clear) {
      accel_ = accel;
      diff = Vec3(0, 0, 0);
      v = 0.0;
    } else {
  #ifndef PROFFIEOS_DONT_USE_GYRO_FOR_CLASH
      v = (diff.len() + fusor.gyro_clash_value()) / 2.0;
  #else
      v = diff.len();
  #endif
    }
    // If we're spinning the saber or if loud sounds are playing,
    // require a stronger acceleration to activate the clash.
    if (v > (CLASH_THRESHOLD_G * (1
          + fusor.gyro().len() / 500.0
#if defined(ENABLE_AUDIO) && defined(AUDIO_CLASH_SUPPRESSION_LEVEL)
          + dynamic_mixer.audio_volume() * (AUDIO_CLASH_SUPPRESSION_LEVEL * 1E-10) * dynamic_mixer.get_volume()
#endif
         ))) {
      if ( (accel_ - fusor.down()).len2() > (accel - fusor.down()).len2() ) {
        diff = -diff;
      }

      bool stab = (fabs(diff.x) > 2.0 * sqrtf(diff.y * diff.y + diff.z * diff.z)) &&
            fusor.swing_speed() < 150;

      if (!clash_pending1_) {
        forward_stab_ = (stab && (diff.x < 0));
        if (stab) PVLOG_DEBUG << "**** DoAccel >>>>>>>> - forward_stab_ = " << (forward_stab_ = (diff.x < 0)) << "\n";
        clash_pending1_ = true;
        pending_clash_is_stab1_ = stab;
        pending_clash_strength1_ = v;
      } else {
        pending_clash_strength1_ = std::max<float>(v, (float)pending_clash_strength1_);
      }
    }
    accel_ = accel;
  }

  // Handle lockup or drag for the main blade when only the main blade is on
  void MainBladeLockupOrDrag() {
    if (fusor.angle1() < -M_PI / 4) {
      SaberBase::SetLockup(SaberBase::LOCKUP_DRAG, BC_MAIN_BLADE_SET);
    } else if (!battle_mode_) {
      SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL, BC_MAIN_BLADE_SET);
    }
  }

  // Handle lockup or drag for the second blade when only the second blade is on
  void SecondBladeLockupOrDrag() {
    if (fusor.angle1() > M_PI / 4) {
      SaberBase::SetLockup(SaberBase::LOCKUP_DRAG, BC_SECOND_BLADE_SET);
    } else if (!battle_mode_) {
     SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL, BC_SECOND_BLADE_SET);
    }
  }

  // Handle lockup or drag for dual blades when both blades are on
  void DualBladesLockupOrDrag() {
    if (fusor.angle1() < -M_PI / 4) {
      // Main blade is pointing down, and within drag range
      SaberBase::SetLockup(SaberBase::LOCKUP_DRAG, BC_MAIN_BLADE_SET);
    } else if (fusor.angle1() > M_PI / 4) {
      // Main blade is pointing up, and within drag range for the second blade
      SaberBase::SetLockup(SaberBase::LOCKUP_DRAG, BC_SECOND_BLADE_SET);
    } else {
      // Otherwise, apply normal lockup based on which blade is above the horizon
      SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL, GetBladeAboveHorizon());
    }
  }

#endif  // BC_DUAL_BLADES

  bool use_fake_id_ = false;
  float fake_id_ = 0;
  size_t best_config_before_faking_ = SIZE_MAX;

#ifdef BLADE_ID_SCAN_MILLIS
  float id(bool announce = false) override {
    if (use_fake_id_) {
      float real_id = PropBase::id(announce);
      size_t real_best_config = FindBestConfigForId(real_id);

      if (real_best_config != best_config_before_faking_) {
        // Real blade has changed, exit manual mode
        use_fake_id_ = false;
        find_blade_again_pending_ = true;
        return real_id;
      }
      return fake_id_;
    }
    return PropBase::id(announce);
  }

  int GetNoBladeLevelBefore() override {
    int ohm;
    if (best_config_before_faking_ != SIZE_MAX) {
      ohm = blades[best_config_before_faking_].ohm;
      best_config_before_faking_ = SIZE_MAX;
    } else {
      ohm = current_config->ohm;
    }
    return ohm / NO_BLADE;
  }

#else
  float id(bool announce = false) override {
    return use_fake_id_ ? fake_id_ : PropBase::id(announce);
  }
#endif  // BLADE_ID_SCAN_MILLIS

  void TriggerBladeID() {
    use_fake_id_ = false;
    best_config_before_faking_ = SIZE_MAX;
    FindBladeAgain();
    PlayArraySound();
  }

  void NextBladeArray() {
    if (!use_fake_id_) best_config_before_faking_ = current_config - blades;
    size_t next_array = (current_config - blades + 1) % NELEM(blades);
    fake_id_ = blades[next_array].ohm;
    use_fake_id_ = true;

    FindBladeAgain();
    PlayArraySound();
  }

  void PlayArraySound() {
    if (SFX_array) {
      SFX_array.Select(current_config - blades);
      hybrid_font.PlayPolyphonic(&SFX_array);
    } else if (!hybrid_font.PlayPolyphonic(&SFX_bladein)) {
      hybrid_font.PlayCommon(&SFX_font);
    }
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (PropBase::Parse(cmd, arg)) return true;

    if (!strcmp(cmd, "twist")) {
        Event(BUTTON_NONE, EVENT_TWIST);
        return true;
    }
    if (!strcmp(cmd, "left")) {
        Event(BUTTON_NONE, EVENT_TWIST_LEFT);
        return true;
    }
    if (!strcmp(cmd, "right")) {
        Event(BUTTON_NONE, EVENT_TWIST_RIGHT);
        return true;
    }
    return false;
  }

  RefPtr<BufferedWavPlayer> wav_player;

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      // storage of unused cases
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_ON):
        return true;

/*
Case structure below
1btn
    single blade
    dual blades
    any blades

2 or 3btn
    single blade
    dual blades
    any blades

any # of buttons
    single blade
    dual blades
    any blades
*/

// ----------------------------------------------- 1 btn section
#if NUM_BUTTONS == 1
#ifndef BC_DUAL_BLADES
// -------------------- 1 btn single blade

// Blaster Deflection
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        DoBlasterBlock();
        return true;

// Lightning Block
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
        DoLightningBlock();
        return true;

#else  // else BC_DUAL_BLADES is defined

// -------------------- 1 btn dual blades

// Turn Main Blade ON First Muted
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF | BUTTON_POWER):
        TurnMainBladeOnMuted();
        return true;

// Turn Second Blade ON First Muted
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_CLICK_LONG, MODE_OFF):
        TurnSecondBladeOnMuted();
        return true;

// Turn Second Blade ON
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        TurnBladeOn(BC_SECOND_BLADE_SET);
        return true;

// Turn Second Blade OFF
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_ON):
        if (isSecondBladeOn()) {
          TurnBladeOff(BC_SECOND_BLADE_SET);
        }
        return true;

// Turn Dual Blades ON
      case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_OFF):
        TurnOnHelper();
        return true;

// Turn Dual Blades ON Muted
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF | BUTTON_POWER):
        MuteAll();
        return true;

// Lightning Block (Double Click POW to start, Click POW to Stop)
// Toggle Battle Mode
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        if (isPointingUp()) {
          ToggleBattleMode();
        } else {
          if (!isSecondBladeOn() && !on_pending_) {
            TurnBladeOn(BC_SECOND_BLADE_SET);
            return true;
          } else {
            DoLightningBlock();
          }
        }
        return true;

#endif  // BC_DUAL_BLADES

// -------------------- 1 btn any blades

// Enter / Exit Volume MENU
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
        EnterVolumeMenu();
        return true;

// Spoken Battery Level in percentage
// Spoken Battery Level in volts - pointing DOWN
      case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD_MEDIUM, MODE_OFF):
        DoSpokenBatteryLevel();
        return true;

// On-Demand Battery Level
      case EVENTID(BUTTON_POWER, EVENT_THIRD_CLICK_LONG, MODE_OFF):
        OnDemandBatteryLevel();
        return true;

// Change Preset
// Pointing UP = First
// Pointing DOWN = Previous
// NOT Pointing UP or DOWN = Next
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        DoChangePreset();
        return true;

// Start or Stop Track
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT, MODE_ON):
        DoTrackStartOrStop();
        return true;

// Spam Blaster Blocks
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
        DoSpamBlast();
        return true;

// Lockup / Drag
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
       DoLockup();
       return true;

// Power Save blade dimming - pointing UP
// Spam Blast toggle - pointing DOWN
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_MEDIUM, MODE_ON):
        // pointing DOWN
        if (fusor.angle1() < -M_PI / 3) {
          ToggleSpamBlast();
          return true;
        }
        // pointing UP
        if (isPointingUp()) {
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
          return true;
        }
        return true;

#endif  // NUM_BUTTONS == 1

// ----------------------------------------------- 2 or 3 btn section
#if NUM_BUTTONS == 2 || NUM_BUTTONS == 3
#ifndef BC_DUAL_BLADES

// -------------------- 2 btn single blade

// Change Preset
// Pointing UP = First
// Pointing DOWN = Previous
// NOT Pointing UP or DOWN = Next
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        DoChangePreset();
        return true;

// Blaster Deflection
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        DoBlasterBlock();
        return true;

#else  // else BC_DUAL_BLADES is defined

// -------------------- 2 or 3 btn dual blades

// Change Preset
// Pointing UP = First
// Pointing DOWN = Previous
// NOT Pointing UP or DOWN = Next
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_LONG, MODE_OFF):
        DoChangePreset();
        return true;

// Turn Second Blade ON
#if NUM_BUTTONS == 3
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
#else
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
#endif
          TurnBladeOn(BC_SECOND_BLADE_SET);
          return true;

// Turn Second Blade ON additionally
// Blaster Deflection
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        if (!isSecondBladeOn() && !on_pending_) {
          TurnBladeOn(BC_SECOND_BLADE_SET);
          return true;
        }  // fall through
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
#if NUM_BUTTONS == 2
        if (!isSecondBladeOn() && !on_pending_) {
          TurnBladeOn(BC_SECOND_BLADE_SET);
          return true;
        }  // fall through
#endif
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_ON):
        DoBlasterBlock();
        return true;

// Turn Main Blade ON First Muted
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        TurnMainBladeOnMuted();
        return true;

// Turn Second Blade ON First Muted
#if NUM_BUTTONS == 3
      case EVENTID(BUTTON_AUX2, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
#else
      case EVENTID(BUTTON_AUX, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
#endif
        TurnSecondBladeOnMuted();
        return true;

// Turn Dual Blades ON
#if NUM_BUTTONS == 3
      // Use AUX (middle ideally) for dual blade ON/OFF when 3btns
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
#else
      case EVENTID(BUTTON_AUX, EVENT_SECOND_CLICK_LONG, MODE_OFF):
#endif
        TurnOnHelper();
        return true;

// Turn Dual Blades ON Muted
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF | BUTTON_POWER):
        MuteAll();
        return true;

// Turn Second Blade OFF
#if NUM_BUTTONS == 3
      case EVENTID(BUTTON_AUX2, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
#else
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
#endif
        if (isSecondBladeOn()) {
          TurnBladeOff(BC_SECOND_BLADE_SET);
        }
        return true;

// Turn Dual Blades OFF (will also turn OFF any single blade that's ON)
#if NUM_BUTTONS == 3
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
#else
      case EVENTID(BUTTON_AUX, EVENT_SECOND_CLICK_LONG, MODE_ON):
#endif
        TurnOffHelper();
        return true;

#endif  // BC_DUAL_BLADES

// -------------------- 2 or 3 btn any blades

// Enter / Exit Volume MENU
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_POWER):
        EnterVolumeMenu();
        return true;

// Spoken Battery Level in percentage
// Spoken Battery Level in volts - pointing DOWN
     case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF | BUTTON_AUX):
        DoSpokenBatteryLevel();
        return true;

// On-Demand Battery Level hold AUX, long click POW...?
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF | BUTTON_AUX):
        OnDemandBatteryLevel();
        return true;

// Start or Stop Track
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_OFF):
      case EVENTID(BUTTON_POWER, EVENT_FIRST_CLICK_LONG, MODE_ON):
        DoTrackStartOrStop();
        return true;

// Spam Blaster Blocks
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        DoSpamBlast();
        return true;

// Lockup / Drag
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_AUX):
        DoLockup();
        return true;

// Lightning Block
      case EVENTID(BUTTON_AUX, EVENT_CLICK_SHORT, MODE_ON | BUTTON_POWER):
        DoLightningBlock();
        return true;

// Power Save blade dimming - pointing UP
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_AUX):
        if (isPointingUp()) {
          SaberBase::DoEffect(EFFECT_POWERSAVE, 0);
        }
        return true;

// Spam Blast toggle - pointing DOWN
      case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_MEDIUM, MODE_ON):
        // pointing DOWN
        if (fusor.angle1() < -M_PI / 3) {
          ToggleSpamBlast();
        }
        return true;

#endif  // NUM_BUTTONS == 2 || NUM_BUTTONS == 3

// ----------------------------------------------- Any btn section
#ifndef BC_DUAL_BLADES

// -------------------- Any btn single blade

// Turn Saber ON
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        TurnOnHelper();
        return true;

// Turn Blade OFF
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        TurnOffHelper();
        return true;

// Turn Blade ON Muted
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF | BUTTON_POWER):
      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, MODE_OFF):
        MuteAll();
        return true;

// Stab
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_ON):
        if (on_pending_ || millis() - saber_on_time_ < 2000) return false;
        SaberBase::SetClashStrength(2.0);
        SaberBase::DoStab();
        return true;

// Melt
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
        if (on_pending_) return false;
        clash_impact_millis_ = millis();
        if (!SaberBase::Lockup() && !swinging_) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
          auto_melt_on_ = true;
          SaberBase::DoBeginLockup();
          }
        return true;

#else  // else BC_DUAL_BLADES is defined

// -------------------- Any btn dual blades

// Turn Main Blade ON
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        TurnBladeOn(BC_MAIN_BLADE_SET);
        return true;

// Turn Main Blade ON additionally
// Blaster Deflection
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        if (!isMainBladeOn() && !on_pending_) {
          TurnBladeOn(BC_MAIN_BLADE_SET);
        } else {
          DoBlasterBlock();
        }
        return true;

// Turn Main Blade OFF
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM, MODE_ON):
        if (isMainBladeOn()) {
          TurnBladeOff(BC_MAIN_BLADE_SET);
        }
        return true;

// Thrust ON either non-ignited blade
// Stab
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_ON):
        if (millis() - last_thrust_millis_ < 200 ||
            millis() - saber_on_time_ < 2000)
          return false;

          GetThrustBladeLocation();
        // Check if both blades are already on
        if (isMainBladeOn() && isSecondBladeOn()) {
          if (on_pending_) return false;
          SaberBase::SetClashStrength(2.0);
          PVLOG_DEBUG << "**** Doing STAB on " << (thrusting_blade_[BC_MAIN_BLADE] ? "MAIN blade.\n" : "SECOND blade.\n");
          SaberBase::DoEffect(EFFECT_STAB, GetThrustBladeLocation());
        } else {
          if (thrusting_blade_ == BC_MAIN_BLADE_SET && isSecondBladeOn()) {
            TurnBladeOn(BC_MAIN_BLADE_SET);
          } else if (thrusting_blade_ == BC_SECOND_BLADE_SET && isMainBladeOn()) {
            TurnBladeOn(BC_SECOND_BLADE_SET);
          }
        }
        last_thrust_millis_ = millis();
        return true;

// Melt
      case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON):
        if (on_pending_) return false;
        clash_impact_millis_ = millis();
        if (!SaberBase::Lockup() && !swinging_) {
          SaberBase::SetLockup(SaberBase::LOCKUP_MELT, forward_stab_ ? BC_MAIN_BLADE_SET : BC_SECOND_BLADE_SET);
          PVLOG_DEBUG << "**** Doing MELT on " << (forward_stab_ ? "MAIN blade.\n" : "SECOND blade.\n");
          auto_melt_on_ = true;
          SaberBase::DoBeginLockup();
          }
        return true;

#endif  // BC_DUAL_BLADES

// -------------------- Any btn any blades

// Gesture Ignition Controls
#ifdef BC_SWING_ON
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_OFF):
        NoBladeDisableGestures();
        if (millis() - saber_off_time_ > 500) {
          FastOn();
          GestureEnableBattleMode();
        }
        return true;
#endif  // BC_SWING_ON

#ifdef BC_TWIST_ON
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
        NoBladeDisableGestures();
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_millis_ > 300 &&
            millis() - saber_off_time_ > 500) {
          FastOn();
          GestureEnableBattleMode();
          last_twist_millis_ = millis();
        }
        return true;
#endif  // BC_TWIST_ON

#ifdef BC_TWIST_OFF
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
        NoBladeDisableGestures();
        // Delay twist events to prevent false trigger from over twisting
        if (millis() - last_twist_millis_ > 500) {
          last_twist_millis_ = millis();
          TurnOffHelper();
        }
        return true;
#endif  // BC_TWIST_OFF

#ifdef BC_THRUST_ON
      case EVENTID(BUTTON_NONE, EVENT_THRUST, MODE_OFF):
        NoBladeDisableGestures();
        // Delay Thrust On at boot
        if (millis() - saber_off_time_ > 1000) {
          GestureEnableBattleMode();
#ifdef BC_DUAL_BLADES
          GetThrustBladeLocation();
          thrusting_blade_ = thrusting_blade_ | ~controlled_blades_;
          PVLOG_DEBUG << "**** " << (thrusting_blade_[BC_MAIN_BLADE] ? "MAIN" : "SECOND") << " Blade THRUST ON\n";
          FastOn(EffectLocation(0, thrusting_blade_));
#else
          FastOn();
#endif
        }
        return true;
#endif  // BC_THRUST_ON

#ifdef BC_FORCE_PUSH
      case EVENTID(BUTTON_NONE, EVENT_PUSH, MODE_ON):
        // Delay Force Push from previous Push
        if (millis() - last_push_millis_ > 2000) {
          if (!hybrid_font.PlayPolyphonic(&SFX_push)) {
            hybrid_font.DoEffect(EFFECT_FORCE, 0);
          }
          last_push_millis_ = millis();
        }
        return true;
#endif  // BC_FORCE_PUSH

// Manually cycle to Next Blade Array
  case EVENTID(BUTTON_POWER, EVENT_FOURTH_HELD_MEDIUM, MODE_OFF):
    PVLOG_NORMAL << "**** BUTTON EVENT Manually Cycling Blade Array\n";
    NextBladeArray();
    return true;

// Manually trigger Blade ID scan
#if (NUM_BUTTONS != 1) || !defined(BC_DUAL_BLADES)  // only not available for 1 btn dual blades.
  case EVENTID(BUTTON_POWER, EVENT_FOURTH_CLICK_LONG, MODE_OFF):
    PVLOG_NORMAL << "**** BUTTON EVENT Manually triggering native Blade ID\n";
    TriggerBladeID();
    return true;
#endif

// Enter OS System Menu
// Enter BC Blade Length Mode
      case EVENTID(BUTTON_POWER, EVENT_SECOND_HELD_MEDIUM, MODE_OFF):
#ifdef ENABLE_IDLE_SOUND
        hybrid_font.StopIdleSound();
#endif
#ifdef MENU_SPEC_TEMPLATE
        PVLOG_NORMAL << "** Entering ProffieOS Menu System\n";
        EnterMenu();
#elif defined(DYNAMIC_BLADE_LENGTH)
        EnterBladeLengthMode();
#endif
        return true;

// Enter Scroll Presets
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_OFF):
        EnterScrollPresets();
        return true;

// Auto Swing Blast
  // Do swings within 1sec of blaster block to do swing blocks.
  // No swing for 1sec to auto exit.
#ifdef BC_ENABLE_AUTO_SWING_BLAST
      case EVENTID(BUTTON_NONE, EVENT_SWING, MODE_ON):
        if (millis() - last_blast_millis_ < 1000) {
          SaberBase::DoBlast();
          last_blast_millis_ = millis();
          PVLOG_NORMAL << "** Auto Swing Blast mode\n";
        }
        break;
#endif

// Force
// Toggle Battle Mode
#if (NUM_BUTTONS != 1) || !defined(BC_DUAL_BLADES)  // 1 btn with dual blades has different control
      case EVENTID(BUTTON_POWER, EVENT_SECOND_CLICK_LONG, MODE_ON):
        if (spam_blast_ || on_pending_) return false;
        if (isPointingUp()) {
          ToggleBattleMode();
        } else {
          if (GetWavPlayerPlaying(&SFX_force)) return false;  // Simple prevention of force overlap
          SaberBase::DoForce();
        }
        return true;
#endif

  // Auto Lockup Mode
      case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON):
        if (!battle_mode_ || swinging_) return false;
        clash_impact_millis_ = millis();
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        auto_lockup_on_ = true;
        SaberBase::DoBeginLockup();
        return true;

// First Preset Fast - pointing UP
// Next Preset Fast - NOT pointing UP or down
// Previous Preset Fast - pointing DOWN
      case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
        DoChangePresetFast();
        return true;

// Enter ColorChange Mode
#ifndef DISABLE_COLOR_CHANGE
      case EVENTID(BUTTON_POWER, EVENT_THIRD_HELD_MEDIUM, MODE_ON):
        if (spam_blast_) return false;
          ToggleColorChangeMode();
        return true;
#endif

// Revert ColorChange to uploaded config color (reset Variation)
      case EVENTID(BUTTON_POWER, EVENT_THIRD_CLICK_LONG, MODE_ON):
        if (spam_blast_) return false;
        PVLOG_NORMAL << "** Reverted Color Variation to uploaded config color. Variation = " << SaberBase::GetCurrentVariation() << "\n";
        SaberBase::SetVariation(0);
        sound_library_v2.SayResetToDefaultColor();
        return true;

// Quote - NOT pointing DOWN
// Toggle Sequential/Random quotes - pointing DOWN
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):
        if (fusor.angle1() < -M_PI / 4) {
          // pointing DOWN
          ToggleSequentialQuote();
        } else {
          DoQuote();
        }
        return true;

// User Effects a.k.a. "Special Abilities" ©Fett263
#if NUM_BUTTONS == 1

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON | BUTTON_POWER):
        if (isPointingUp()) {
          PVLOG_DEBUG << "**** EFFECT_USER3 **\n";
          SaberBase::DoEffect(EFFECT_USER3, 0);
        } else {
          PVLOG_DEBUG << "**** EFFECT_USER1 **\n";
          SaberBase::DoEffect(EFFECT_USER1, 0);
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON | BUTTON_POWER):
        if (isPointingUp()) {
          PVLOG_DEBUG << "**** EFFECT_USER4 **\n";
          SaberBase::DoEffect(EFFECT_USER4, 0);
        } else {
          PVLOG_DEBUG << "**** EFFECT_USER2 **\n";
          SaberBase::DoEffect(EFFECT_USER2, 0);
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF | BUTTON_POWER):
        if (isPointingUp()) {
          PVLOG_DEBUG << "**** EFFECT_USER7 **\n";
          SaberBase::DoEffect(EFFECT_USER7, 0);
        } else {
          PVLOG_DEBUG << "**** EFFECT_USER5 **\n";
          SaberBase::DoEffect(EFFECT_USER5, 0);
        }
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF | BUTTON_POWER):
        if (isPointingUp()) {
          PVLOG_DEBUG << "**** EFFECT_USER8 **\n";
          SaberBase::DoEffect(EFFECT_USER8, 0);
        } else {
          PVLOG_DEBUG << "**** EFFECT_USER6 **\n";
          SaberBase::DoEffect(EFFECT_USER6, 0);
        }
        return true;

# else  // more than 1-button

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON | BUTTON_POWER):
        PVLOG_DEBUG << "**** EFFECT_USER1 **\n";
        SaberBase::DoEffect(EFFECT_USER1, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON | BUTTON_POWER):
        PVLOG_DEBUG << "**** EFFECT_USER2 **\n";
        SaberBase::DoEffect(EFFECT_USER2, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_ON | BUTTON_AUX):
        PVLOG_DEBUG << "**** EFFECT_USER3 **\n";
        SaberBase::DoEffect(EFFECT_USER3, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_ON | BUTTON_AUX):
        PVLOG_DEBUG << "**** EFFECT_USER4 **\n";
        SaberBase::DoEffect(EFFECT_USER4, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF | BUTTON_POWER):
        PVLOG_DEBUG << "**** EFFECT_USER5 **\n";
        SaberBase::DoEffect(EFFECT_USER5, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF | BUTTON_POWER):
        PVLOG_DEBUG << "**** EFFECT_USER6 **\n";
        SaberBase::DoEffect(EFFECT_USER6, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_RIGHT, MODE_OFF | BUTTON_AUX):
        PVLOG_DEBUG << "**** EFFECT_USER7 **\n";
        SaberBase::DoEffect(EFFECT_USER7, 0);
        return true;

      case EVENTID(BUTTON_NONE, EVENT_TWIST_LEFT, MODE_OFF | BUTTON_AUX):
        PVLOG_DEBUG << "**** EFFECT_USER8 **\n";
        SaberBase::DoEffect(EFFECT_USER8, 0);
        return true;
#endif  // NUM_BUTTONS == 1

// Blade Detect
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
#endif

// Events that need to be handled regardless of what other buttons are pressed.
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_OFF):
      case EVENTID(BUTTON_AUX2, EVENT_PRESSED, MODE_OFF):
        saber_off_time_ = millis();
        SaberBase::RequestMotion();
        return true;

      case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
      case EVENTID(BUTTON_AUX2, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          return true;
        }
    }  // switch
    return false;
  }  // Event2

  void SB_Effect(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_POWERSAVE:
        if (!hybrid_font.PlayPolyphonic(&SFX_dim)) {
          beeper.Beep(0.1, 1300);
          beeper.Beep(0.1, 900);
          beeper.Beep(0.1, 600);
          beeper.Beep(0.1, 300);
        }
        return;
      // On-Demand Battery Level
      case EFFECT_BATTERY_LEVEL:
        if (is_speaking_) {
          is_speaking_ = false;
          return;
        }
        if (!hybrid_font.PlayPolyphonic(&SFX_battery)) {
          beeper.Beep(1.0, 475);
          beeper.Beep(0.5, 693);
          beeper.Beep(0.16, 625);
          beeper.Beep(0.16, 595);
          beeper.Beep(0.16, 525);
          beeper.Beep(1.1, 950);
          beeper.Beep(0.5, 693);
          PVLOG_VERBOSE << "The Force will be with you...always.\n";
        }
        return;
      case EFFECT_IGNITION:
        saber_on_time_ = millis();
        return;
      case EFFECT_TRANSITION_SOUND:
        if (SFX_tr) hybrid_font.PlayPolyphonic(&SFX_tr);
        return;

    }
  }  // SB_Effect

  void Clash2(bool stab, float strength) override {
    SaberBase::SetClashStrength(strength);
    if (Event(BUTTON_NONE, stab ? EVENT_STAB : EVENT_CLASH)) {
      IgnoreClash(400);
    } else {
      IgnoreClash(100);
      // Saber must be on and not in lockup or colorchange mode for stab/clash/melt.
      if (SaberBase::IsOn() && !SaberBase::Lockup()
          && SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE) {
        if (stab) {
          SaberBase::DoStab();
        } else {
#ifdef BC_DUAL_BLADES
      SaberBase::DoEffect(EFFECT_CLASH, EffectLocation(0, GetBladeAboveHorizon()));
#else
      SaberBase::DoClash();
#endif
        }
      }
    }
  }

private:
  DelayTimer mute_all_delay_timer_;
  DelayTimer mute_mainBlade_delay_timer_;
  DelayTimer mute_secondBlade_delay_timer_;
  DelayTimer scroll_presets_beep_delay_timer_;

  bool battle_mode_ = false;
  bool auto_lockup_on_ = false;
  bool auto_melt_on_ = false;
  bool sequential_quote_ = false;
  bool spam_blast_ = false;
  bool is_speaking_ = false;  // Don't play battery.wav when doing Spoken Battery Level
  bool muted_ = false;
  bool forward_stab_ = false;

  uint32_t thrust_begin_millis_ = millis();
  uint32_t push_begin_millis_ = millis();
  uint32_t clash_impact_millis_ = millis();
  uint32_t last_thrust_millis_ = millis();
  uint32_t last_twist_millis_ = millis();
  uint32_t last_push_millis_ = millis();
  uint32_t last_blast_millis_ = millis();
  uint32_t saber_off_time_ = millis();
  uint32_t saber_on_time_ = millis();

  Vec3 mss;
  EffectLocation location;
  BladeSet thrusting_blade_;
};

#ifdef BC_DUAL_BLADES
  const BladeSet SaberBCButtons::controlled_blades_ = BC_MAIN_BLADE_SET | BC_SECOND_BLADE_SET;
#endif

#endif
