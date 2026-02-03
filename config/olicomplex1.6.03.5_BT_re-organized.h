// ProffieOS8.x Config File

/*
I ordered this chassis with parts, Proffie v3.9, Bluetooth & OLED.
Saberbay
Imp Sword Crystal Chamber Install Chassis Proffie+OLED+Bluetooth / Yes
Main Chassis Color: Copper Brown (Metallic PLA) Crystal Chamber Color: Classis Gold Sparkle (Sparkle PLA)

Crystal chamber install chassis designed exclusively for the Imp Sword sold here at Saberbay.
Not designed or compatible with any other saber design.
Chassis design is from Jimmy's Saber Junk and 3D printed.This chassis is printed in 3 parts and will...

It came with:
FULL PARTS KIT includes:
-choice of soundboard (CFX, ProffieV3.9, GHV3)
-qty. 1 assembled NPXL V3 shortpin hiltside connector
-qty. 1 single pixel accent LED
-qty. 1 KeepPower 18650 3000mah 15A button top protected cell
-qty. 1 high amp killswitch
-qty. 1 blue TCSS OLED screen (for OLED chassis options)
-qty. 1 Bluetooth BT909 Breakout board module (for Bluetooth chassis options)
-qty. 1 28mm X-BASS 4ohm/3W bass speaker  <-- I broke it!

Standard AV buttons are 12mm

*/

/*
https://pod.hubbe.net/
https://github.com/profezzorn
https://github.com/profezzorn/ProffieOSDocs
https://github.com/profezzorn/ProffieOS/wiki
https://fredrik.hubbe.net/lightsaber/style_editor_beta.html
https://www.fett263.com/index.html
https://www.fett263.com/proffieOS7-fett263-prop-file.html
https://www.fett263.com/fett263-os7-config-helper.html
https://github.com/NoSloppy/SoundFontNamingConverter
https://www.soundfontnamingconverter.com/
https://github.com/NoSloppy/Unlock_Savefiles
https://therebelarmory.com/board/97/profezzorns-lab
https://therebelarmory.com/thread/11354/welcome-profezzorns-lab

LIST OF PROFFIE BOARD PINS:
https://fredrik.hubbe.net/lightsaber/v6/
*/



#ifdef CONFIG_TOP                           // https://pod.hubbe.net/config/the-config_top-section.html
//============================================================================================================================================

#include "proffieboard_v3_config.h"
//PROFFIECONFIG ENABLE_MASS_STORAGE         //Do not remove the "//"
//PROFFIECONFIG ENABLE_WEBUSB               //This is a ProfieConfig setting from @Ryryog25 (Ryan) program.
                                            //=============================================================
                                            //It can also work as a reminder to turn it on in Arduino (if you wish)
                                            //=====================================================================
// MTFBWY

/*******************************\
|*                             *|
|* === LIST OF CATEGORIES: === *|
|*                             *|_____________________________________________________________________________\
|*************************************************************************************************************|
|* == Blade ==                                                                                               *|
|* == Blade ID (if ever needed) ==                                                                           *|
|* == Bluetooth/RFID ==                                                                                      *|
|* == Board Orientation ==                                                                                   *|
|* == Buttons ==                                                                                             *|
|* == Colors ==                                                                                              *|
|* == Editing ==                                                                                             *|
|* == Memory / Save Memory ==                                                                                *|
|* == Motion ==                                                                                              *|
|* == OLED ==                                                                                                *|
|* == Sound/Volume/Audio ==                                                                                  *|
|* == Time Outs == (milisec * sec * min)                                                                     *|
|* ========================================================================================================= *|
|* ========================= PROP SPECIFIC THAT DON'T FIT IN THE CATEGORIES ABOVE: ========================= *|
|* ========================================================================================================= *|
|* ==== Blaster Mode & Bullets count define's ====                                                           *|
|* ==== Crossguard define's ==== (from crossguard_config.h)                                                  *|
|* ==== BC define's ====                                                                                     *|
|* ========================================================================================================= *|
|* ==== Fett263 define's ====                                                                                *|
|* == BATTLE MODE OPTIONS == Battle Mode is enabled via controls by default in Fett263's prop                *|
|* == Force Push == Push movement triggers push.wav (or force.wav if push.wav is not present)                *|
|* == Gesture Sleep == Toggle Gesture Ignition and Retraction detection to disable or enable gesture options *|
|* == Stab On == Gesture Ignition via Stab (linear movement + clash at tip of blade)                         *|
|* == Swing On == Gesture Ignition via Swing                                                                 *|
|* == Thrust On == Gesture Ignition via Thrust (linear movement)                                             *|
|* == Twist Off == Gesture Retraction via Twist (back and forth)                                             *|
|* == Twist On == Gesture Ignition via Twist (back and forth)                                                *|
|* ========================================================================================================= *|
|* ==== SA22C define's ====                                                                                  *|
|* ==== SABERSENSE define's ====                                                                             *|
|* ========================================================================================================= *|
|* ==== More define's ==== that I found but have no idea what they do ???                                    *|
\*************************************************************************************************************/

/***************\
|*             *|
|* == Blade == *|
|*             *|
\***************/

//#define NUM_BLADES 4                      //I have: Main blade + NPXL connector (2 SubBlades) + Crystal chamber
#define NUM_BLADES 5                        //5th blade would be bluetooth

const unsigned int maxLedsPerStrip = 144;   //144 per meter, only change the number if more than 144/m !
//#define DISABLE_MOTION
//#define DISABLE_WS2811
#define SHARED_POWER_PINS
//#define INVERT_WS2811                     // If your electronics inverts the bladePin for some reason, define this.
#define BLADE_DETECT_PIN blade5Pin // 7 / Free 1
// For multi blade detect part 2/4
// (Need to add part 1/4 to events.h 2/4 to config, part 3/4 to props, part 4/4 to to ProffieOS.ino)
//#define BLADE_DETECT_PIN2 blade6Pin // 8 / Free 2
//#define BLADE_DETECT_PIN3 blade7Pin // 9 / Free 3

/***********************************\
|*                                 *|
|* == Blade ID (if ever needed) == *|
|*                                 *|
\***********************************/

/*
#define BLADE_ID_SCAN_MILLIS 1000           //To activate Blade ID constant monitoring
#define BLADE_ID_TIMES 1000                 // https://pod.hubbe.net/howto/blade-id-constant-monitoring.html
#define ENABLE_POWER_FOR_ID PowerPINS<bladePowerPin1,bladePowerPin2,bladePowerPin3>
#define BLADE_ID_CLASS                      // https://pod.hubbe.net/howto/blade-id.html
#define BLADE_ID_CLASS BridgedPullupBladeID<bladeIdentifyPin, BRIDGED_PIN>          //
                                                                                    //Chose one of:
#define BLADE_ID_CLASS ExternalPullupBladeID<bladeIdentifyPin, PULLUP_RESISTANCE>   //
#define SPEAK_BLADE_ID
#define BLADE_ID_SCAN_TIMEOUT 5 * 60 * 1000 //How long active blade scanning will actually be active for
#define BLADE_ID_STOP_SCAN_WHEN_IGNITED     //This define stops ID scanning while the saber is ignited
#define NO_BLADE_ID_RANGE 100,1000          //To be adjusted https://crucible.hubbe.net/t/blade-id-not-playing-font-ident-when-it-switches-should-it/4463/31
*/

/************************\              // https://crucible.hubbe.net/t/deep-sleep/4663/23 <-- Make BT "sleep"
|*                      *|              // Here’s the checklist for what it needs,
|* == Bluetooth/RFID == *|              // - #define ENABLE_SERIAL and #define IDLE_OFF_TIME 60 * 10 * 1000 in config_top
|*                      *|              // - StylePtr<Blue>(), as the blade style in each preset fot BT "fake blade"
\************************/              // - SimpleBladePtr<CH3LED, NoLED, NoLED, NoLED, bladePowerPin6, -1, -1, -1>(),
                                        //   as the BT "fakeblade" in the config
                                        // - wiring: BT voltage to 3.3, BT negative to LED 6,
                                        //   TX on Proffie to RX on BT and RX on Proffie to TX on BT.

#define ENABLE_SERIAL                       //Don't use "Serial + Mass Storage" if not necessary. (To save memory)
                                            //Serial port can be used to talk to a BLE chip. I use BT909 so do not comment out #define ENABLE_SERIAL, unless absolutely necessary!
#define BLE_PASSWORD "000000"               //default is "000000", but I'd prefer "66" or "69", because: "Why not?"
                                            //Max 20 characters
#define BLE_NAME "oli_chassi"               //Max 32 characters
#define BLE_SHORTNAME "olisaber"            //Max 9 characters
//#define RFID_SERIAL Serial3               //Add an RFID reader. To configure, you will need an RFID_Commands array in the CONFIG_PRESETS section.

/***************************\
|*                         *|
|* == Board Orientation == *|
|*                         *|
\***************************/

/*
No need to include any "ORIENTATION" if orientation is default.
#define ORIENTATION_ROTATION 0,20,0       // X,Y,Z      //For Curved hilts, or where the board is not parallel with the blade.
                                                        //This will rotate the orientation of the board 20 degrees around the Y axis.
#define ORIENTATION ORIENTATION_FETS_TOWARDS_BLADE      //Default is USB connector points away from blade
#define ORIENTATION ORIENTATION_USB_TOWARDS_BLADE       // https://pod.hubbe.net/config/the-config_top-section.html#optional-defines
#define ORIENTATION ORIENTATION_USB_CCW_FROM_BLADE
#define ORIENTATION ORIENTATION_USB_CW_FROM_BLADE
#define ORIENTATION ORIENTATION_TOP_TOWARDS_BLADE
#define ORIENTATION ORIENTATION_BOTTOM_TOWARDS_BLADE
*/

/*****************\
|*               *|
|* == Buttons == *|
|*               *|
\*****************/

#define NUM_BUTTONS 3
//#define DUAL_POWER_BUTTONS                //The AUX will also turn the saber on. If not defined, AUX will go to next preset when off.
#define FETT263_HOLD_BUTTON_OFF             //Changes to Hold PWR to turn Off / Retract
//#define FETT263_HOLD_BUTTON_LOCKUP        //Enables Hold AUX for Lockup* Cannot be used with FETT263_SAVE_CHOREOGRAPHY
                                            //*Clash Strength / Clash Impact effects and sounds for Lockup negated
                                            //*Battle Mode control changes to hold AUX + Swing
#define FETT263_POWER_LOCK                  //enable Power Lock Mode to change 2 button Retraction from click PWR to
                                            //hold PWR & AUX (can be enabled/disabled in Edit Mode)
#define FETT263_MOTION_WAKE_POWER_BUTTON    //Enables a click on POWER Button to Wake Up Gestures after MOTION_TIMEOUT without igniting blade. Saber will play boot sound and gestures will be active.
#define FETT263_BM_DISABLE_OFF_BUTTON       //During Battle Mode Power Button Retraction is disabled
//For overwriting value of buttons timeout (defined in "../buttons/button_base.h")
#define BUTTON_DOUBLE_CLICK_TIMEOUT 300     // default 500
#define BUTTON_SHORT_CLICK_TIMEOUT 300      // default 500
#define BUTTON_HELD_TIMEOUT 300             // default 300
#define BUTTON_HELD_MEDIUM_TIMEOUT 800      // default 800
#define BUTTON_HELD_LONG_TIMEOUT 2000       // default 2000
#define BUTTON_LONG_CLICK_TIMEOUT 2500      // default 2500
//#define MULTI_PROP_BUTTON_HELD_XTRA_LONG_TIMEOUT 5000  // default 4000 For multi_prop.h
//#define SPEAK_TOUCH_VALUES                //To calibrate your touch buttons, you can temporarily add this to your config
                                            //to have ProffieOS say the touch values out loud. If you have more than one,
                                            //calibrate one at a time and comment out the rest temporarily.
//#define DETONATOR_BUTTON_POWER_IS_LATCH   //If your detonator pow button is latching and has been defined as such in your config.
// Define this if your power button is a touch button.
//#define POWER_TOUCHBUTTON_SENSITIVITY 1700
//#define AUX_TOUCHBUTTON_SENSITIVITY 1700
//#define AUX2_TOUCHBUTTON_SENSITIVITY 1700

/****************\
|*              *|
|* == Colors == *|
|*              *|
\****************/

#define COLOR_CHANGE_DIRECT                 //This enables "Click to Change" capability for styles with ColorChange<>, used with Dual Phase Library styles and other specialty styles in my library. Does not affect normal styles or Edit Mode functionality
#define EXTRA_COLOR_BUFFER_SPACE 60         //is a define that can speed up processing a bit. Basically, it can calculate the colors for the next blade (or the next frame for the same blade) even though it’s not done feeding out the data for the data in the color buffer yet. It’s the equivalent of just increasing maxLedsPerStrip. @NoSloppy

/*****************\
|*               *|
|* == Editing == *|
|*               *|
\*****************/

#define DISABLE_COLOR_CHANGE
#define ENABLE_ALL_EDIT_OPTIONS             //This will enable the components for Edit Mode used through the Edit Mode Menu or ProffieOS Workbench (WebUSB).
/*
These are not needed since I use ENABLE_ALL_EDIT_OPTIONS
#define DYNAMIC_BLADE_DIMMING               //controllable from ProffieOS Workbench or some props.
#define DYNAMIC_BLADE_LENGTH                //controllable from ProffieOS Workbench or some props.
#define DYNAMIC_CLASH_THRESHOLD             //controllable from ProffieOS Workbench or some props.
#define SAVE_BLADE_DIMMING
#define SAVE_CLASH_THRESHOLD
*/
/*
These are not needed since I use SAVE_STATE
#define SAVE_COLOR_CHANGE                   //to save the color change state
#define SAVE_VOLUME                         //Start with the volume used last
#define SAVE_PRESET                         //Start at the last preset when you turn the saber on
*/
#define SAVE_STATE                          //to encompass SAVE_COLOR_CHANGE, SAVE_BLADE_DIMMING, SAVE_VOLUME and SAVE_PRESET
//#define KEEP_SAVEFILES_WHEN_PROGRAMMING   // https://pod.hubbe.net/config/keeping-edits-when-uploading.html
                                            //This define applies to uploading (programming) the board. If you have this define then all previously saved edits will be used in place of the config you are trying to upload, so if you have any changes in your config that you’re trying to upload they may not appear on the saber. This define has nothing to do with normal editing functions, it only applies to uploading a new config. If you edit your blade length with Edit Mode it will be used until you upload again (or if you delete the .ini files on your SD).
                                            //When you upload a new config you are overwriting everything on the saber, so all settings and edits are replaced with the new config.
                                            //IF you intend to upload a config but don’t want it to overwrite your previous edits then you would apply this define BUT doing so means that any changes you try to make by uploading a config will be ignored and as the article notes you can get odd behavior or errors if you change the presets or other editable options in the config that were previously changed in Edit Mode. Before using the define you need to familiarize yourself with what it’s doing, which is ignoring anything you tried to change via an upload in lieu of the saved edits. This is opposite normal behavior and often confuses users. It also makes it impossible for anyone to troubleshoot issues as your config is no longer “running things”. For this reason, this define is really only for use by users that fully understand it and essentially only ever intend to make any changes through the menu or Workbench. If you use this define and try to upload any changes via your config they will not appear on your saber along with all of the other noted behavior differences in the article.
                                            //If you just want to be able to edit your saber between uploads and/or without needing to upload then you just need to use
//#define FETT263_EDIT_MODE_MENU            //Enable Edit Mode Menu System *Requires ENABLE_ALL_EDIT_OPTIONS define
#define FETT263_EDIT_SETTINGS_MENU          //Enable Edit Settings Menu (Volume, Clash Threshold, Blade Length, Gestures/Controls, Brightness)
                                            //I recommend setting USB Type = "Serial + WebUSB" under Arduino > Tools to allow for style, font, track, color editing via ProffieOS Workbench
                                            //Cannot be combined with FETT263_EDIT_MODE_MENU Requires ENABLE_ALL_EDIT_OPTIONS
#define FETT263_CIRCULAR_DIM_MENU           //Changes Brightness Menu to Circular Control
//#define FETT263_SAVE_CHOREOGRAPHY         //Enables Enhanced Battle Mode with Saved Choreography
//#define FETT263_QUICK_SELECT_ON_BOOT      //Enables Preset Selection Menu on Boot (when power is first applied)
                                            //Use Dial Menu to turn to desired preset, click PWR to select or hold PWR to select and ignite
//#define FETT263_SPECIAL_ABILITIES         //This enables 8 "Special Ability" controls (style controlled), 4 while ON, 4 while OFF.
                                            //Special Abilities are controlled by the style and can vary in every preset, they are "user" defined effects/capabilities.
                                            //Allows "Multi-Phase" to be style based, replaces FETT263_MULTI_PHASE.
                                            //Cannot be used with FETT263_MULTI_PHASE or FETT263_SAVE_CHOREOGRAPHY
/* "Special Abilities are user effects such as EFFECT_USER1 that a prop file can set to do anything,
as opposed to like “EFFECT_CLASH” which would always be a clash.
By using the Fett263 Style Library, many different features and effects can be set to use them with
a ton of flexibility in your blade styles created there." @NoSloppy */
#define FETT263_MULTI_PHASE                 //enable a preset change while blade is ON to create a "Multi-Phase" saber effect
                                            //#define FETT263_MULTI_PHASE or #define FETT263_SPECIAL_ABILITIES
#define FETT263_DISABLE_CHANGE_FONT
#define FETT263_DISABLE_CHANGE_STYLE
#define FETT263_DISABLE_COPY_PRESET
//#define FETT263_DISABLE_MULTI_BLAST_TOGGLE
//#define MENU_SPEC_TEMPLATE DefaultMenuSpec // https://pod.hubbe.net/howto/menus.html
// or
//#define MENUS_SPEC_TEMPLATE SettingsOnlyMenuSpec // https://pod.hubbe.net/config/the-config_top-section.html#menuspectemplate
//#define MENU_SPEC_MENU ChangeVolumeMode
//#define COLOR_MENU_GAMMA 2.2
//#define CLASH_THRESHOLD_GAMMA 2.0
//#define VOLUME_MENU_GAMMA 2.2
#define STEPS_PER_REVOLUTION 15     // Added Feb 2025
#define FONT_PATTERN "*;common"
//#define FONT_PATTERN "override;*;common"  //new with OS8.x //how to use it ???
/* Another feature that you may want to add support for is the FONT_PATTERN define. Basically, it lets you specify what
the font path given a specific directory. The default is of course "*;common", but you could also set it to "*;voice"
if you prefer. Or "*;common;voice" if you feel like you need two common directories. Or "override;*;common" or
"override;*;extras/*;* /Proffie;common".

There are functions in strfun.h that converts back and forth between a single directory, like "TeensySF" and the full
font path, which in the last example above would be "override;TeensySF;extras/TeensySF;TeensySF/Proffie;common".

One possible use case for this is to have font-specific voice packs. You could for instance do: "*;* /voice;common".
You would need to create a “voice” directory in every font directory, and then you can (optionally) stick a voice pack
in there to override the files in “common”. */

/******************************\
|*                            *|
|* == Memory / Save Memory == *| https://pod.hubbe.net/howto/saving-memory.html
|*                            *|
\******************************/

//#define DISABLE_SD
//#define KILL_OLD_PLAYERS  //will be ON by default @OS8.x
                                            //The result is that the new sound always plays.
//#define DISABLE_DIAGNOSTIC_COMMANDS       //This disables some diagnostic commands to save memory
                                            // https://pod.hubbe.net/tools/serial-monitor-commands.html
                                            // https://pod.hubbe.net/tools/serial-monitor-additional-commands.html
//#define ENABLE_DEBUG                      // "reserved for Profezzorn"
//#define ENABLE_DEVELOPER_COMMANDS         // "reserved for Profezzorn" Some commands (only useful for developers are normally not compiled), if you want them, add this define to enable them.
//#define VERBOSE_SD_ERRORS                 // With this define, proffieos will say an error code when you get an “sd card not found”
                                            // error, and that error code can help narrow down the cause of the problem. (maybe)
#define PROFFIEOS_LOG_LEVEL 500             // PVLOG_ERROR 100 = errors
                                            // PVLOG_STATUS 200 = things you should probably know, like blade ID
                                            // PVLOG_NORMAL 300 = normal information about what is happening (default level)
                                            // PVLOG_DEBUG 400 = information helpful for debugging
                                            // PVLOG_VERBOSE 500 = repeated, spammy information you don't normally want
//#define CLASH_RECORDER                    //Define this to record clashes to SD card as CSV files.
#define DISABLE_BASIC_PARSER_STYLES         //Memory Saving. This will disable the "named" legacy default
                                            //styles to allow more room for your custom styles.
                // ^                        //Edit Mode styles replace these.
                // |
                // +--- check in style/style_parser.h for WHAT WILL BE CHANGED / INGORED / DISABLED !!!

//#define DISABLE_TALKIE                    //To save memory, ProffieOS will use short melodies to indicate an error
                                            //instead. Changes talky to simple beeps.
                                            // https://pod.hubbe.net/troubleshooting/beep_codes.html
                                            // https://pod.hubbe.net/troubleshooting/what-is-it-saying.html
                                            // https://pod.hubbe.net/troubleshooting/status-led
//#define POV_INCLUDE_FILE "star_wars_logo_pov_data.h"       //Disable all "POV" entries to save memory (Persistance Of Vision)
//Disable in == Bluetooth == #define ENABLE_SERIAL  //Don't use "Serial + Mass Storage" if not necessary. (To save memory)
                                            //Serial port can be used to talk to a BLE chip. I use BT909 so do not disable
                                            //SERIAL, unless absolutely necessary!
#define MOUNT_SD_SETTING                    // https://pod.hubbe.net/config/the-config_top-section.html#mountsdsetting

/****************\
|*              *|
|* == Motion == *|
|*              *|
\****************/

#define DETONATOR_TWIST                     // if you want twist ON/OFF on your two buttons setup
//#define DISABLE_MOTION                    //ENABLE_MOTION is on by default in latest ProffieOS, to turn it off
//#define FUSOR_VARIANCE                    //Will need #define ENABLE_DEVELOPER_COMMANDS to enable the dumpfusor command.
//#define GYRO_MEASUREMENTS_PER_SECOND 800
//#define PROFFIEOS_ACCELEROMETER_RANGE 16  //This one can be set to 2, 4, 8 or 16, and specifies the range of values
                                            //measured by the accelerometer. Higher values means less precision, but we
                                            //don’t really need high precision, so it doesn’t really matter. However, if
                                            //you want the 3.9-value-fluke, you should set this to 4.
//#define PROFFIEOS_MOTION_FREQUENCY 1600   //This define controls how often we read the accelerometer. The frequency
                                            //can be 200, 400, 800, 1600, 3300 or 6600, but the interesting ones are
                                            //800 and 1600. The default is 1600, 800 is basically what ProffieOS 5
                                            //and below would get.
//#define ACCEL_MEASUREMENTS_PER_SECOND 800 //The default is 800, and the code just creates a box filter of size
                                            //(PROFFIEOS_MOTION_FREQUENCY / ACCCEL_MEASUREMENTS_PER_SECOND = 2) to do
                                            //the averaging. Setting this define to the same value as
                                            //PROFFIEOS_MOTION_FREQUENCY (1600) means that no filtering will occur.
                                            //Setting it to a lower value is also valid, but will cause some (very minor)
                                            //latency in clash detection.
//The other solution is:
//#define AUDIO_CLASH_SUPPRESSION_LEVEL 10  //This define checks the volume of sound currently being emitted and
                                            //increases the CLASH_THRESHOLD_G accordingly. Higher values means more
                                            //suppression when the volumes goes up, but that also means it’s more
                                            //difficult to actually make a clash happen.
//#define PROFFIEOS_DONT_USE_GYRO_FOR_CLASH // https://pod.hubbe.net/explainers/history-of-clash.html
//#define FETT263_MOTION_WAKE_POWER_BUTTON  //Enables a click on POWER Button to Wake Up Gestures after MOTION_TIMEOUT
                                            //without igniting blade. Saber will play boot sound and gestures will be active.

/**************\
|*            *|
|* == OLED == *|                            // https://github.com/profezzorn/ProffieOS/blob/6173b290d05a2b90ada6e00c6ebb6f9362963567/display/ssd1306.h
|*            *|
\**************/

#define INCLUDE_SSD1306                     //INCLUDE instead of ENABLE to make it work for "Bullet Counts" from CONFIG_BOTTOM
//#define ENABLE_SSD1306                    //To activate OLED <-- What does "activate" mean ?
                                            // @profezzorn "Yes, it might not be super obvious because of the wonky C++ syntax,
                                            //but this creates two global objects, one called display_controller and one called
                                            //display. I split it up like this based on the idea that in the future, different
                                            //presets could use different display controllers."

                                            //I still don't know if OLED will display "*.bmp" from font folders if
                                            //"INCLUDE_SSD1306" is used instead of "ENABLE_SSD1306"

//#define OLED_FLIP_180                     //To flip the OLED displayed content 180 degrees.
//#define OLED_MIRRORED                     //Shows the OLED displayed content in reverse
//#define ENABLE_SPIDISPLAY                 //For the color screen (see "CONFIG_BOTTOM")
//#define USE_AUREBESH_FONT                 //To see "presets" (or fonts - if presets are named by their font name) name
                                            //in Aurabesh on OLED

/****************************\
|*                          *|
|* == Sound/Volume/Audio == *|
|*                          *|
\****************************/

//#define BC_VOLUME_MENU
//#define ENABLE_AUDIO         //is ON by default @OS8.x
#define VOLUME 1200
#define BOOT_VOLUME 500
//#define ENABLE_I2S_OUT                    //This enables I2S output, Data3 is SCK, Data4 is FS and Button2/aux is DATA.
//#define ENABLE_SPDIF_OUT                  //This enables SPDIF output. The SPDIF signal will come out at 3.3 volts on Button2/AUX.
//#define LINE_OUT_VOLUME 2000              //If you use ENABLE_I2S_OUT or ENABLE_SPDIF_OUT, you can control the volume with this define.
//#define LINE_OUT_VOLUME dynamic_mixer.get_volume() //The default is 2000. If you want the line out volume to follow the master volume.
#define FILTER_CUTOFF_FREQUENCY 200
#define FILTER_ORDER 8
#define CLASH_THRESHOLD_G 4.0     //was 2.0
#define ENABLE_IDLE_SOUND
//#define FEMALE_TALKIE_VOICE               //only affects built-in error messages
#define VOLUME_MENU_CYCLE                   //"Rotating" volume menu (from SA22C saber) in JetpackOliButtons, DetonatorOliButtons & SaberSA22CButtons
#define JETPACK_TRACK_PLAYER
#define FETT263_DUAL_MODE_SOUND             //Enables odd/even out.wav ignition sound selection based on blade angle
                                            //Up = odd number sounds, Down = even numbered sounds
#define FETT263_CLASH_STRENGTH_SOUND        //Enables selection of clash, stab and lockup sounds based on clash strength
                                            //Light clash = 01.wav, Hard clash = highest number.wav
#define FETT263_SAY_COLOR_LIST              //Spoken Color Names replace default sounds during Color List Mode
#define FETT263_SAY_COLOR_LIST_CC           //Spoken Color Names replace default sounds during Color Change "CC" Color List Mode
//#define FETT263_SAY_BATTERY_PERCENT       //Spoken Battery Level percentage during On Demand Battery Level effect
#define FETT263_BC_SAY_BATTERY_VOLTS_PERCENT//Spoken Battery Level in volts and percent (point down for volts, parallel or up for percent)
#define FETT263_CIRCULAR_VOLUME_MENU        //Changes Volume Menu to Circular Control
//#define FETT263_TRACK_PLAYER_NO_PROMPTS   //Disables spoken voice prompts in Track Player
//#define FETT263_DISABLE_QUOTE_PLAYER      //Disables Force/Quote player, only uses Force. This will allow Quotes to be controlled by style.
                                            //Use FETT263_SPECIAL_ABILITIES to set EFFECT_QUOTE or EFFECT_NEXT_QUOTE in style
                                            //Cannot be used with FETT263_RANDOMIZE_QUOTE_PLAYER and FETT263_QUOTE_PLAYER_START_ON
#define FETT263_QUOTE_PLAYER_START_ON       //This will set Force / Quote to play Quote by default (if in font)
#define FETT263_BM_CLASH_DETECT 4.0         //max value to use clashes in Battle Mode 2.0, clashes used on clash strength below this value
                                            //allows light clashes to produce clash effects instead of using Begin/End Lockup
                                            //(above this value Clash is performed by quick pull away using Begin/End Lockup sounds and effect)
                                            //Range 0 ~ 8 (note 0 will use Battle Mode 1.0 with all clashes being Begin/End Lockup)
#define FETT263_RANDOMIZE_QUOTE_PLAYER      //is specific to using the saber_fett263.buttons.h prop file, and deals with whether quote sound files are randomized or played sequentially. @NoSloppy
//#define NO_REPEAT_RANDOM     //is ON by default @OS8.x
                                            //is OS System level define that prevents the same effect file from playing back-to-back. @NoSloppy
//#define PREONS_MATCH_BLADE_EFFECTS

/*****************************************\
|*                                       *|
|* == Time Outs == (milisec * sec * min) *| //max value 2147483647 (= +/- 25 days)(24 days, 20 hours, 31 min, 23 sec, 647 milisec)
|*                                       *|
\*****************************************/

#define PLI_OFF_TIME 1000 * 60 * 1          //Duration that Power Level Indicator will show on OLED when blade is turned off. Set for 10 seconds.
#define IDLE_OFF_TIME 1000 * 60 * 20        //Timeout for accent LEDs (in milliseconds), this example would set it to 20 minutes
                                            //This is when the MOSFETS (all 6 of them) turn off, so it’s not necessarily just accent LEDs.
#define MOTION_TIMEOUT 1000 * 60 * 15       //keeps motion chip active for 15 minutes while blade is Off
#define JETPACK_IDLE_TIME 1000 * 60 * 1     //Jetpack max idle time in millisec (default 1 min) before auto-shutdown
//#define CONFIG_STARTUP_DELAY 1000 * 5 * 1 //Time delay to boot-up after kill switch on to allow for insert chassis in hilt.
//#define DELAYED_OFF                       //For detonator.h (I prefer without)

#define DETONATOR_TIMER_DURATION 6.0f          // default is 6 seconds during "loud timer" delayed detonation.  (set timing in seconds)
#define DETONATOR_STEALTH_TIMER_DURATION 12.0f // default is 12 seconds during "quiet timer" delayed detonation.  (set timing in seconds)

/*************************************************************************************************************\
|* ========================================================================================================= *|
|*                                                                                                           *|
|* ========================= PROP SPECIFIC THAT DON'T FIT IN THE CATEGORIES ABOVE: ========================= *|
|*                                                                                                           *|
|* ========================================================================================================= *|
\*************************************************************************************************************/

/***************************************************\
|*                                                 *|
|* ==== Blaster Mode & Bullets count defines  ==== *|
|*                                                 *|
\***************************************************/

#define BLASTER_SHOTS_UNTIL_EMPTY 20        //Not defined = unlimited shots.
#define BLASTER_JAM_PERCENTAGE 1            //0 = NO JAM. If this is not defined, random from 0-100%.
#define BLASTER_ENABLE_AUTO
#define BLASTER_DEFAULT_MODE MODE_STUN      //MODE_STUN|MODE_KILL|MODE_AUTO.
//#define RELOAD_BUTTON_IS_POWER
#define OLED_USE_BLASTER_IMAGES
//#define BLASTER_RANGE_LEVELS              //For Baldusi's blaster ??

/***********************************************************\
|*                                                         *|
|* ==== Crossguard #define ==== (from crossguard_config.h) *|
|*                                                         *|
\***********************************************************/

// #define FIRE1_SPEED 2                    // Fire speed, valid values are 1 - 10
// #define FIRE1_DELAY 800                  // How long to wait before firing up crossguards.
                                            // Each of these have three values: A, B, C
                                            // A = non-random intensity
                                            // B = random intensity
                                            // C = cooling
                                            // The first two control temperature, and as we add
                                            // A + rnd(B) to the base of the blade each animation frame.
                                            // The second controls how rapidly the fire cools down
// #define FIRE1_NORMAL 0, 1000, 2          // This is used during normal operation.
// #define FIRE1_CLASH  3000, 0, 0          // This is used when a clash occurs
// #define FIRE1_LOCKUP 0, 5000, 10         // This is used during lockup.
/* #define FIRE1PTR(NUM, DELAY) \           // Helper
  StyleFirePtr<RED, YELLOW, NUM, DELAY, FIRE1_SPEED, \
    FIRE1_NORMAL, FIRE1_CLASH, FIRE1_LOCKUP>()
*/

/*************************\
|*                       *|
|* ==== BC define's ==== *|
|*                       *|
\*************************/

/*
---------------------------------------------------------------------------
Optional #defines:
#define ENABLE_AUTO_SWING_BLAST  // - Multi-blast initiated by simply swinging
                                 //   within 1 second of last blast.
                                 //   Exit by not swinging for 1 second.
#define NO_VOLUME_MENU           // - Option to omit On-the-fly Volume menu control with buttons.
---------------------------------------------------------------------------
//Gesture Controls:
#define BC_SWING_ON
#define BC_STAB_ON
#define BC_THRUST_ON
#define BC_TWIST_ON
#define BC_TWIST_OFF
//#define NO_BLADE_NO_GEST_ONOFF
#define BC_FORCE_PUSH
#define BC_FORCE_PUSH_LENGTH 5              //1 = shortest, easiest to trigger, 10 = longest. Default value is 5.
#define BC_NO_BM
//#define BC_GESTURE_AUTO_BATTLE_MODE       //Cannot be used if #define BC_NO_BM is active.
#define BC_LOCKUP_DELAY 200
*/

/******************************\
|*                            *|
|* ==== Fett263 define's ==== *|
|*                            *|
\******************************/

/**********************************************************************************************\
|*                                                                                            *|
|* == BATTLE MODE OPTIONS == Battle Mode is enabled via controls by default in Fett263's prop *|
|*                                                                                            *|
\**********************************************************************************************/

//#define FETT263_BATTLE_MODE_ALWAYS_ON     //Battle Mode is always on, toggle controls deactivated
                                            //This will disable traditional Clash and Stab effects (cannot be used with FETT263_BATTLE_MODE_START_ON)
//#define FETT263_BATTLE_MODE_START_ON      //Battle Mode is active with each ignition by default but can be toggled using Aux + Swing control
                                            //(cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON)
//#define FETT263_MAX_CLASH 1276800         //optional define to set the hardest clash on saber range 8 ~ 16, defaults to 16 if not defined
//#define FETT263_SPIN_MODE                 //Enables toggle for "Spin" Mode* which disables all clash/stab/lockup effects to allow for spinning and flourishes.
                                            //Cannot be used with FETT263_SAVE_CHOREOGRAPHY or FETT263_HOLD_BUTTON_LOCKUP *Not the same as ENABLE_SPINS
//#define ENABLE_SPIN
#define FETT263_USE_BC_MELT_STAB            //Allows MELT to be gesture controlled full-time, uses Thrust for Stab effect
#define FETT263_LOCKUP_DELAY 1500           //"delay" in millis to determine Clash vs Lockup

/**********************************************************************************************\
|*                                                                                            *|
|* == Force Push == Push movement triggers push.wav (or force.wav if push.wav is not present) *|
|*                                                                                            *|
\**********************************************************************************************/

//#define FETT263_FORCE_PUSH                //To enable gesture controlled Force Push during Battle Mode (will use push.wav or force.wav if not present)
#define FETT263_FORCE_PUSH_ALWAYS_ON        //To enable gesture controlled Force Push full time (will use push.wav or force.wav if not present)
#define FETT263_FORCE_PUSH_LENGTH 5         //Allows for adjustment to Push gesture length in millis needed to trigger Force Push
                                            //Recommended range 1 ~ 10, 1 = shortest, easiest to trigger, 10 = longest

/*************************************************************************************************************\
|*                                                                                                           *|
|* == Gesture Sleep == Toggle Gesture Ignition and Retraction detection to disable or enable gesture options *|
|*                                                                                                           *|
\*************************************************************************************************************/

//#define FETT263_SAVE_GESTURE_OFF            //Save "Gesture Sleep" setting to turn gesture ignitions and retractions off on boot

/*************************************************************************************\
|*                                                                                   *|
|* == Stab On == Gesture Ignition via Stab (linear movement + clash at tip of blade) *|
|*                                                                                   *|
\*************************************************************************************/

//#define FETT263_STAB_ON                   //To enable Stab On Ignition control (automatically enters Battle Mode, uses Fast On)
#define FETT263_STAB_ON_PREON               //Disables Fast On ignition for Stab On so Preon is used (cannot be used with FETT263_STAB_ON)
//#define FETT263_STAB_ON_NO_BM             //To enable Stab On Ignition control but not activate Battle Mode
                                            //(Combine with FETT263_STAB_ON or FETT263_STAB_ON_PREON,
                                            //cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

/*********************************************\
|*                                           *|
|* == Swing On == Gesture Ignition via Swing *|
|*                                           *|
\*********************************************/

//#define FETT263_SWING_ON                  //To enable Swing On Ignition control (automatically enters Battle Mode, uses Fast On)
#define FETT263_SWING_ON_PREON              //Disables Fast On ignition for Swing On so Preon is used (cannot be used with FETT263_SWING_ON)
#define FETT263_SWING_ON_NO_BM              //To enable Swing On Ignition control but not activate Battle Mode
                                            //(Combine with FETT263_SWING_ON or FETT263_SWING_ON_PREON,
                                            //cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)
#define FETT263_SWING_ON_SPEED 250          //Swing Speed required for Ignition 250 ~ 500 recommended

/*****************************************************************\
|*                                                               *|
|* == Thrust On == Gesture Ignition via Thrust (linear movement) *|
|*                                                               *|
\*****************************************************************/

#define FETT263_THRUST_ON                   //To enable Thrust On Ignition control (automatically enters Battle Mode, uses Fast On)
//#define FETT263_THRUST_ON_PREON           //Disables Fast On ignition for Thrust On so Preon is used (cannot be used with FETT263_THRUST_ON)
//#define FETT263_THRUST_ON_NO_BM           //To enable Thrust On Ignition control but not activate Battle Mode
                                            //(Combine with FETT263_THRUST_ON or FETT263_THRUST_ON_PREON,
                                            //cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

/*****************************************************************\
|*                                                               *|
|* == Twist Off == Gesture Retraction via Twist (back and forth) *|
|*                                                               *|
\*****************************************************************/

#define FETT263_TWIST_OFF                   //To enable Twist Off Retraction control <-- I don't like it.

/**************************************************************\
|*                                                            *|
|* == Twist On == Gesture Ignition via Twist (back and forth) *|
|*                                                            *|
\**************************************************************/

#define FETT263_TWIST_ON                    //To enable Twist On Ignition control (automatically enters Battle Mode, uses Fast On)
//#define FETT263_TWIST_ON_PREON            //Disables Fast On ignition for Twist On so Preon is used (cannot be used with FETT263_TWIST_ON)
//#define FETT263_TWIST_ON_NO_BM            //To enable Twist On Ignition control but not activate Battle Mode
                                            //(Combine with FETT263_TWIST_ON or FETT263_TWIST_ON_PREON,
                                            //cannot be used with FETT263_BATTLE_MODE_ALWAYS_ON or FETT263_BATTLE_MODE_START_ON)

/****************************\
|*                          *|
|* ==== SA22C define's ==== *|
|*                          *|
\****************************/

/*
#define SA22C_NO_LOCKUP_HOLD
#define SA22C_STAB_ON
#define SA22C_SWING_ON
#define SA22C_TWIST_ON
#define SA22C_THRUST_ON
#define SA22C_TWIST_OFF
#define SA22C_FORCE_PUSH
#define SA22C_FORCE_PUSH_LENGTH 5
#define GESTURE_AUTO_BATTLE_MODE
#define SA22C_LOCKUP_DELAY 200              //Default value is 200
*/

/*********************************\
|*                               *|
|* ==== SABERSENSE define's ==== *|
|*                               *|
\*********************************/

//#define SABERSENSE_BLADE_ID
  /*
  Replaces regular BladeID with on-demand ID scanning.
  Normal BladeID relies either on continuous ID scanning,
  or it only runs at specific times like bootup
  or when changing fonts. Sabersense BladeID makes
  BladeID scanning available as a button press.
  Triple-click POWER will run the scan, identify the blade
  and switch to its associated array. This system prevents
  spurious ID readings switching blade when you don't want
  it to, especially if using SnapshotID.
  Plays array-specific bladeidX.wav files when switching.
  */

//#define SABERSENSE_ARRAY_SELECTOR
  /*
  Replaces regular BladeID and allows cycling between
  different blade/preset arrays manually, regardless
  of actual BladeID status. Plays array-specific
  arrayX.wav files when switching.
  Requires arrays to be numbered consecutively,
  starting at zero, in the field that would otherwise
  contain BladeID values.
  Not currently compatible with Blade Detect.
  */

//#define SABERSENSE_DISABLE_SAVE_ARRAY
  /*
  By default, SABERSENSE_ARRAY_SELECTOR saves the current
  array so that the saber will always boot into the last
  array used. This define disables that save functionality.
  */

//#define SABERSENSE_ENABLE_ARRAY_FONT_IDENT
  /*
  Plays font ident after array ident when switching arrays.
  Can be used with SABERSENSE_BLADE_ID and
  SABERSENSE_ARRAY_SELECTOR to tell user which font they
  have landed on on multiple-array systems in which each
  array has its own save files.
  */

//#define SABERSENSE_FLIP_AUDIO_PLAYERS
  /*
  Reverses UP/DOWN orientation for playing FORCE, QUOTE and
  MUSIC TRACK audio files. Default (no define) is UP for
  sequential QUOTE with blade ON and OFF, and DOWN for random
  FORCE effect (ON) and music TRACK (OFF). Define acts on
  both ON and OFF states for consistency.
  */

//#define SABERSENSE_BLAST_PWR_AND_AUX
  /*
  Adds blaster block button to POWER button as well as AUX
  in 2-button mode. Improves 1 and 2-button harmonization,
  but makes accidental blasts more likely when double-
  clicking POWER for Quotes or Force Effect.
  */

//#define SABERSENSE_BUTTON_CLICKER
  /*
  Button Clicker to play press/release wav files when
  buttons are pressed. Intended for Scavenger hilt
  where wheel makes tactile feel difficult.
  Requires press.wav and release.wav files to work.
  */

//#define SABERSENSE_DISABLE_RESET
  /*
  By default, all save files can be deleted on demand with
  a button press, effectively restoring 'factory' defaults.
  This define disables that feature.
  */

//#define SABERSENSE_NO_COLOR_CHANGE
  /*
  Use instead of DISABLE_COLOR_CHANGE.
  */

//#define SABERSENSE_NO_LOCKUP_HOLD
  /*
  Applicable to two-button mode only, reverts to lockup
  being triggered by clash while holding aux.
  */

//GESTURE CONTROLS
    //There are four gesture types: Twist, Stab, Swing and Thrust.
    //Gesture controls bypass all preon effects.
//#define SABERSENSE_TWIST_ON
//#define SABERSENSE_TWIST_OFF
//#define SABERSENSE_STAB_ON
//#define SABERSENSE_SWING_ON
//#define SABERSENSE_THRUST_ON
/*============================================================*/

/*********************************************************************\
|*                                                                   *|
|* == More #define == that I found but have no idea what they do ??? *|
|*                                                                   *|
\*********************************************************************/

// #define ENABLE_SNOOZE                    // TeensySaber V2 hardware define?
// #define ENABLE_FASTLED                   // TeensySaber V2 hardware define? FASTLED is experimental and untested right now
// #define ENABLE_WATCHDOG                  // TeensySaber V2 hardware define?
// #define ENABLE_SERIALFLASH               // TeensySaber V2 hardware define?

#endif // CONFIG_TOP


/*
How To: Back Up A Proffieboard            // https://pod.hubbe.net/howto/how-to-back-up-a-proffieboard.html
How to recover from a bad programming     // https://pod.hubbe.net/howto/how-to-recover-from-a-bad-programming.html
                                          // https://profezzorn.github.io/webdfu/dfu-util/
Editing presets.ini by hand               // https://pod.hubbe.net/howto/editing-presets.ini-by-hand.html
Save file format                          // https://pod.hubbe.net/explainers/save-file-format.html
Speeding things up                        // https://pod.hubbe.net/howto/speeding-things-up.html
Faster fonts                              // https://pod.hubbe.net/sound/faster-fonts.html
Sound font config.ini configuration       // https://pod.hubbe.net/sound/sound-font-config.ini-configuration.html
Sound font configuration                  // https://pod.hubbe.net/sound/sound-font-configuration.html

Serial monitor                            // https://pod.hubbe.net/tools/serial-monitor.html
Workbench                                 // https://pod.hubbe.net/tools/workbench.html

Web-Bluetooth                             // https://profezzorn.github.io/lightsaber-web-bluetooth/app.html  (<-- Open in Chrome not Firefox)
Is it uploading                           // https://pod.hubbe.net/troubleshooting/is-it-uploading.html
Usb connection issues                     // https://pod.hubbe.net/troubleshooting/usb-connection-issues.html
Web-USB                                   // https://pod.hubbe.net/troubleshooting/webusb.html
                                          // https://fredrik.hubbe.net/lightsaber/webusb.html
Files and folder structure                // https://pod.hubbe.net/troubleshooting/files-and-folder-structure.html

 MTFBWY
                                          // https://www.reddit.com/r/fpv/comments/ur4zup/failed_to_open_usb_device_while_trying_to_flash/

--------------------------------------------------------------------------------------------------------------------------

ProffieOS7 requires SD card preformance to be 1200kbps read speed with 12 simulatenous audio streams.
Subpar or poor performing SD cards will cause the saber to lag or freeze.
If you experience lag or slow response you need to run ‘sdtest’ in Serial Monitor.
Even if SD is labeled as high performance you should run sdtest to be sure.

    First, remove DISABLE_DIAGNOSTIC_COMMANDS define from your config.
    Upload new config (you may temporatily need to remove some presets if exceeding FLASH memory)
    With saber connected go to Arduino > Tools > Serial Monitor
    Type ‘sdtest’
    If you do not have 1200kbps read speed and 12 simulatenous audio streams you will want to replace your SD
    with a better performing card.

You can also improve SD performance via formatting and file structure, also be sure to ALWAYS ‘Eject’ the SD card
anytime it’s mounted to your computer to prevent file corruption. MTFBWY

--------------------------------------------------------------------------------------------------------------------------

You’ll need to get the latest code from GitHub for this.
Go to GitHub - profezzorn/ProffieOS: Lightsaber Controller Software

    Click the Green “Code” button
    Click “Download ZIP”
    Then extract the ZIP file to your computer
    Save your previous configs to the /config folder of the new extract and then add these styles.
    Update the new ProffieOS.ino to define your config
    Launch ProffieOS.ino for OS8 in Arduino and upload as usual

--------------------------------------------------------------------------------------------------------------------------

To do a full recursive list dump from your font folder (Windows)
Use CMD, navigate to the folder & use:
dir /b /s /a:-D > ..\results.txt

*/



#ifdef CONFIG_PROP                              // https://pod.hubbe.net/howto/making-your-own-prop-file.html
//============================================================================================================================================

//#include "../props/dual_prop.h"               //use SaberBlasterProp <Saber*, Blaster*> "in PROP_TYPE"
#include "../props/multi_prop.h"                //use MultiProp <Saber*, Blaster*, Detonator, JetpackOliButtons, MorseCode>
                                                // (* choose only 1 saber & 1 blaster)

// *** List of sabers (choose one and only one) ***
// ================================================
//#include "../props/saber.h"                   //use Saber                | working with multi_prop.h
//#include "../props/saber_BC_buttons.h"        //use SaberBCButtons       | working with multi_prop.h
//#include "../props/saber_caiwyn_buttons.h"    //use CaiwynButtons        | <-- works alone but NOT compiling with multi_prop.h or dual_prop.h because it is missing "virtual" in "class CaiwynButtons : public PropBase" - easy fix!)
#include "../props/saber_fett263_buttons.h"     //use SaberFett263Buttons  | working with multi_prop.h
//#include "../props/saber_sa22c_buttons.h"     //use SaberSA22CButtons    | working with multi_prop.h
//#include "../props/saber_sabersense_buttons"  //use SabersenseButtons    |  untested with multi_prop.h
//#include "../props/saber_sabertrio_buttons"   //use SaberFett263Buttons  | WTF, it's a rip-off, untested with multi_prop.h
//#include "../props/s3button_config"           //use SaberSabertrioButtons|  untested with multi_prop.h
//#include "../props/s3button_config2"          //use SaberSabertrioButtons|  untested with multi_prop.h
//#include "../props/saber_shtok_buttons.h"     //use SaberShtokButtons    | compiling with multi_prop.h
//#include "../props/saber_SuperGrandor_0buttons"//use SaberSA22CButtons   |  untested with multi_prop.h
//#include "../props/ultrasaber"                //use Ultrasaber           |  untested with multi_prop.h
//#include "../props/ultrasaber_2"              //use Ultrasaber           |  untested with multi_prop.h

// *** List of blasters (choose one and only one) ***
// ==================================================
//#include "../props/blaster.h"                 //use Blaster              | working with multi_prop.h
#include "../props/blaster_BC_buttons.h"        //use BlasterBCButtons     | working with multi_prop.h after a two lines modification
                                                //(for new version)        |
                                                //use BlasterBC            | Compiling with multi_prop.h (after modifications to BlasterBC
                                                //(for old version)        |
//#include "../props/blaster_laser_musket_buttons"//use LaserMusketButtons |  untested with multi_prop.h but it is not stock compatible because it uses pow & aux buttons instead of fire & mode select.
//#include "../props/blaster_Teas_buttons"      //use BlasterTEA           |  untested with multi_prop.h

// IMPORTANT NOTE: blaster_BC_buttons.h (at the time of writing) is not "stock" compatible with multi_prop.h
// A small modification needs to be added to it's code. I will let NoSloppy decide for the best course of action
// to modify his prop. I do have "my version" of blaster_BC_buttons.h working, so it is possible!

// *** List of other prop types (choose one of each) ***
// =====================================================
//#include "../props/detonator.h"               //use Detonator            | Compiling with multi_prop.h
#include "../props/detonator_Oli_buttons.h"     //use DetonatorOliButtons  | Working with multi_prop.h
#include "../props/jetpack_Oli_buttons.h"       //use JetpackOliButtons    | Working with multi_prop.h
//#include "../props/morsecode_prop.h"          //use MorseCode            | compiling with multi_prop.h (type morse code on the buttons & it plays on OLED & either on speaker or LEDs)
//#include "../props/droids_prop.h"             //In progress - but very far from "ready" (plays with droids sounds)
#undef PROP_TYPE
//#define PROP_TYPE SaberBlasterProp <SaberFett263Buttons, BlasterBCButtons>
//#define PROP_TYPE SaberBlasterProp <SaberBCButtons, BlasterBCButtons>
//#define PROP_TYPE SaberBlasterProp <SaberSA22CButtons, Blaster>
//#define PROP_TYPE SaberBlasterProp </*SaberBCButtons*/SaberSA22CButtons, BlasterBCButtons/**/>
#define PROP_TYPE MultiProp <SaberFett263Buttons, BlasterBCButtons/**/, DetonatorOliButtons/**/, JetpackOliButtons>
//#define PROP_TYPE MultiProp <SaberSA22CButtons, BlasterBCButtons/**/, DetonatorOliButtons/**/, JetpackOliButtons>
//#define PROP_TYPE MultiProp <CaiwynButtons, Blaster, DetonatorOliButtons/**/, JetpackOliButtons>
                  //Attempt 025 is one Saber + one Blaster
                  //BlasterBC is modified by Oli (line 179):
/*
#ifndef PROPS_MULTI_PROP_H                             // added by Oli
EFFECT(battery);    // for EFFECT_BATTERY_LEVEL
#endif                                                 // added by Oli
*/
                  //Fett263 is modified by Oli (line 5904):
/* 2 Button Controls
                                                                            // Added by Oli
      case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_OFF):    // Added by Oli
        #ifndef DEGREES_TO_RADIANS                                          // Added by Oli
        #define DEGREES_TO_RADIANS (M_PI / 180)                             // Added by Oli
        #endif                                                              // Added by Oli
         if (fusor.angle1() > 30 * DEGREES_TO_RADIANS) {                    // Added by Oli
           // up                                                            // Added by Oli
        SetPreset(0, true);  // go to first preset                          // Added by Oli
         } else if (fusor.angle1() < -30 * DEGREES_TO_RADIANS) {            // Added by Oli
           // down                                                          // Added by Oli
        SetPreset(-1, true); // go to last preset                           // Added by Oli
         } else {                                                           // Added by Oli
           // horizontal                                                    // Added by Oli
          CurrentPreset tmp;                                                // Added by Oli
          tmp.SetPreset(-1); // go to middle preset                         // Added by Oli
          SetPreset(tmp.preset_num / 2, true);                              // Added by Oli
         }                                                                  // Added by Oli
*/
// === multi_prop.h history (working versions) ===
    //Attempt 025 is Saber + Blaster with bullets count
    //Attempt 028 is Saber + Blaster + bullets count + Detonator & sound effects on transition between props
    //Attempt 029 is Saber + Blaster + bullets count + Detonator + Jetpack & sound effects
    //Attempt 030 is trying to fix/solve button mapping (for Blaster)
    //Version 035 is Saber + Blaster with buttons mapping + Detonator + Jetpack + MorseCode & sound effects
    //Version 044 is Saber + Blaster (with bts mpg & bullets cnt) + TD + JP + MC + sounds eff. + multi-presets
    //Version 045 is same as 044 with trying helper function to reduce repetitions (not yet working)
    //Version 048 is helper functions working except "extern void SetMessageOLED(const char* message);"
    //Version 049 is stop repetition 1
    //Version 050 is stop repetition 2
    //Version 051 is 050 without "#include <functional>" // Added for std::function
    //Version 059 is display_SetMessage working but modifs to ProffieOS.ino & /display/ssd1306.h
/*
// added for messages from multi_prop.h at the bottom of ProffieOS.ino            // *** added by Oli ***
#if defined(INCLUDE_SSD1306) || defined(ENABLE_SSD1306)                           // *** added by Oli ***
void display_SetMessage(const char* message) {                                    // *** added by Oli ***
   display_controller.SetMessage(message); // with ssd1306working02               // *** added by Oli ***
   //display_controller.ops_.op_.SetMessage(message); // with ssd1306working01    // *** added by Oli ***
}                                                                                 // *** added by Oli ***
#endif                                                                            // *** added by Oli ***
// in display\ssd13606.h in class DisplayHelper2<Width, col_t, OP, OPS...> { // line 197:
// === added by Oli (part 1of2) for SetMessage ===                                        // added by Oli
  // Pass-through method for SetMessage                                                   // added by Oli
  void SetMessage(const char* message) {                                                  // added by Oli
    op_.SetMessage(message); // Accessing `op_` internally                                // added by Oli
  }                                                                                       // added by Oli
// in display\ssd13606.h in class DisplayHelper : public DisplayControllerBase<Width, col_t> { // line 246:
// === added by Oli (part 2of2) for SetMessage ===                                        // added by Oli
  void SetMessage(const char* message) {                                                  // added by Oli
    ops_.SetMessage(message);  // Delegates to the `SetMessage` in DisplayHelper2         // added by Oli
  }                                                                                       // added by Oli
    //Version 060 is added SaberBase::DoEffect(EFFECT_...)
*/

#endif // CONFIG_PROP

/*
Boot Proffieboard:
==================
- Do the sequence properly (with USB down):
  - Hold BOOT (left),
  - Press and release RESET (right),
  - Release BOOT (left).
- Check that the computer sees an STM32BOOTLOADER device in Devices and Printers control panel.
- Upload. (you wont see a Proffieboard connected to a com port in Arduino - Edit but it will be there!)

with USB facing up and pointing towards you: (I use two flat end wooden toothpicks to click the buttons on the board, you can also use a 2x4, 2x6 or taller LEGO tile or plate)
You need to be gentle and feel the click and “un-click” of the buttons.

[quote="olivierflying747-8, post:2, topic:6694"]
with USB facing up and pointing towards you

This might be confusing, so I will rephrase: with the USB port pointing towards you (or if North is the direction you are looking to, then South is behind you, so with the usb pointing South)
[/quote]
*/



#ifdef CONFIG_PRESETS
//============================================================================================================================================

  using Purple = Rgb16<24000, 0, 50536>;
  using Orange = Rgb<255, 97, 0>;
  using OrangeRed = Rgb<255, 14, 0>;
  //using colorx = Rgb or Rgb16<...,...,...>;

/* From Profezzorn to Fett263
Another feature that you may want to add support for is the FONT_PATTERN define. Basically, it lets you specify what
the font path given a specific directory. The default is of course "*;common", but you could also set it to "*;voice"
if you prefer. Or "*;common;voice" if you feel like you need two common directories. Or "override;*;common" or
"override;*;extras/*;* /Proffie;common".

There are functions in strfun.h that converts back and forth between a single directory, like "TeensySF" and the full
font path, which in the last example above would be "override;TeensySF;extras/TeensySF;TeensySF/Proffie;common".

One possible use case for this is to have font-specific voice packs. You could for instance do: "*;* /voice;common".
You would need to create a “voice” directory in every font directory, and then you can (optionally) stick a voice pack
in there to override the files in “common”.
*/

/*******************************\
|*                             *|
|* == Saber sound font list == *|
|*                             *|
\*******************************/

/*
   { "crispity;common", "common/tracks/mandotheme.wav",
   "crispity" },

    { "Slave1_2.1;common", "common/tracks/the_book_of_boba_fett.wav",
    "slave1"},

   { "TheMaw;common", "common/tracks/mandotheme.wav",
    "TheMaw\nyellow"},

   { "EctoBuster;common", "common/tracks/mars.wav",
    "ectobuster\ngradient"}

   { "TeensySF;common", "common/tracks/venus.wav",
    "cyan"},

   { "SmthJedi;common", "common/tracks/mars.wav",
    "blue"},

   { "SmthGrey;common", "common/tracks/mercury.wav",
    "fire"},

   { "SmthFuzz;common", "common/tracks/uranus.wav",
    "red"},

   { "RgueCmdr;common", "common/tracks/ImperialMarchROTJ3_long.wav",
    "blue fire"},

   { "TthCrstl;common", "common/tracks/rebels_theme.wav",
    "green"},

   { "altereactor101;common", "common/tracks/mandotheme.wav", (Japaneese)
    "altereactor"},

   {"analog;common", "common/tracks/mandotheme.wav",
    "analog"},

   { "ancient-tech;common",  "common/tracks/mandotheme.wav",
    "ancient\n -tech"},

   { "angry-monkey;common", "common/tracks/mandotheme.wav",
    "angry\nmonkey"},

   { "apocalypse;common", "common/tracks/mandotheme.wav",
    "apocalypse"},

   { "arcane;common", "common/tracks/mandotheme.wav",
    "arcane"},

   { "ashely;common", "common/tracks/mandotheme.wav", (chainsaw)
    "ashely"},

   { "assassin;common", "common/tracks/mandotheme.wav",
    "assassin"},

   { "banished-prince;common", "common/tracks/mandotheme.wav",
    "banished\nprince"},

   { "battle1;common", "common/tracks/mandotheme.wav",
    "battle1"},

   { "battle2;common", "common/tracks/mandotheme.wav",
    "battle2"},

   { "battle3;common", "common/tracks/mandotheme.wav",
    "battle3"},

   { "bode-akuna;common", "common/tracks/mandotheme.wav",
    "bode\nakuna" },

   { "c110p_chopper;common", "c110p_chopper/tracks/rebels_theme.wav", (no hum)
    "c110p\nchopper" },

   { "Cal-Kestis_BlueNormalized;common", "common/tracks/mandotheme.wav",
    "cal-kestis\n blue"},

   { "celestial-travels;common", "common/tracks/mandotheme.wav", (startrek)
    "celestial\n travels"},

   { "centennial;common", "centennial/tracks/centennial.wav",
    "centennial"},

   { "cere-junda;common", "common/tracks/mandotheme.wav",
    "cere\njunda"},

   { "coda;common", "common/tracks/mandotheme.wav",
    "coda"},

   { "crucible;common", "common/tracks/mandotheme.wav",
    "crucible"},

   { "DaganGeraLight;common", "common/tracks/mandotheme.wav",
    "dagangera\nlight"},

   { "DaganGeraDark;common", "common/tracks/mandotheme.wav",
    "dagangera\ndark"},

   { "DarthVader;common", "common/tracks/mandotheme.wav",
    "darth\nvader"},

   { "DarkSouls;common", "common/tracks/mandotheme.wav",
    "dark\nsouls"},

   { "ronin;common", "common/tracks/mandotheme.wav",
    "ronin"},

   { "deadlink;common", "common/tracks/mandotheme.wav",
    "deadlink"},

    {"decay;common", "common/tracks/mandotheme.wav",
    "decay"},

   { "decimate;common", "common/tracks/mercury.wav",
    "decimate\nwhite"},

   { "dragon_witch;common", "common/tracks/venus.wav", (Japanese)
    "dragon_witch\nmagenta"},

   { "EngineGrip;common", "common/tracks/mercury.wav",
    "enginegrip\nrainbow"},

   { "exalted;common", "common/tracks/uranus.wav",
   "exalted\nstrobe"},

   {"LiquidStatic;common", "common/tracks/mandotheme.wav",
    "liquid\nstatic"},

// === Battery Level / Keep as last preset ===
   { "Zombie_Chainsaw;common", "",
    " battery\nlevel 2.0"},
*/

/****************************\
|*                          *|
|* == Saber presets list == *|
|*                          *|
\****************************/

Preset presets_saber[] = {
   { "crispity;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
        //Theory2Button.h "F1Pod"
    //StylePtr<Layers<Layers<AudioFlicker<RotateColorsX<Variation,HotPink>,RotateColorsX<BlinkingF<Int<1000>,Int<500>>,Rgb<50,0,150>>>,TransitionLoopL<TrWaveX<RandomPerLEDFlickerL<White>,Int<250>,Int<100>,Int<200>,Int<0>>>,TransitionLoopL<TrWaveX<StrobeL<White,Int<15>,Int<1>>,Int<350>,Int<100>,Int<300>,Int<0>>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red>,ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<White,40>,TrFade<1000>>,EFFECT_RETRACTION>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipe<300>,TrColorCycle<1000>>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Layers<
        AudioFlicker<RgbArg<BASE_COLOR_ARG,HotPink>,RotateColorsX<BlinkingF<Int<1000>,Int<500>>,Rgb<50,0,150>>>,
        TransitionLoopL<TrWaveX<RandomPerLEDFlickerL<White>,Int<250>,Int<100>,Int<200>,Int<0>>>,
        TransitionLoopL<TrWaveX<StrobeL<White,Int<15>,Int<1>>,Int<350>,Int<100>,Int<300>,Int<0>>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,White>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<RgbArg<STAB_COLOR_ARG,Red>>,
      ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<RgbArg<CLASH_COLOR_ARG,White>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<White,40>,TrFade<1000>>,EFFECT_RETRACTION>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipe<300>,TrColorCycle<1000>>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Orange,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,Orange,0>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === Crystal Chamber: ===
        // Sabersense https://crucible.hubbe.net/t/possible-gremlin/3427
    //StylePtr<IgnitionDelay<3000,Layers<Rgb<180,20,0>,InOutTrL<TrConcat<TrFade<5000>,Rgb<2,0,0>,TrFade<4000>,Rgb<30,4,0>,TrFade<4000>,Rgb<80,10,0>,TrFade<3000>>,TrConcat<TrFade<1500>,Rgb<80,10,0>,TrFade<2000>,Rgb<40,4,0>,TrFade<2500>,Rgb<5,0,0>,TrFade<3000>>>>>>(),
    StylePtr<IgnitionDelayX<Int<3000>,Layers< //Same as above but "Layerized"
      RgbArg<BASE_COLOR_ARG,Rgb<180,20,0>>,
      InOutTrL<TrConcat<TrFade<5000>,Rgb<2,0,0>,TrFade<4000>,Rgb<30,4,0>,TrFade<4000>,Rgb<80,10,0>,TrFade<3000>>,TrConcat<TrFade<1500>,Rgb<80,10,0>,TrFade<2000>,Rgb<40,4,0>,TrFade<2500>,Rgb<5,0,0>,TrFade<3000>>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "crispity" },

    { "Slave1_2.1;common", "common/tracks/the_book_of_boba_fett.wav",
    // === Main Blade: ===
        // Slave 1 with seismic charge preon
    StylePtr<Layers< // Argified from olicomplex1.6.00.h
    Mix<SmoothStep<Int<16384>,Int<4000>>,Stripes<10000,500,RgbArg<BASE_COLOR_ARG,Rgb<0,30,80>>,Rgb<0,60,180>>,Stripes<10000,-500,Rgb<0,30,80>,Rgb<0,60,180>>>,
      // Dim Blade 50% "Power Save" button combo
    EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<0>>>,
      // Clash - 3 stage fading impact spot
    TransitionEffectL<TrConcat<TrInstant,RgbArg<CLASH_COLOR_ARG,GreenYellow>,TrDelay<25>,AlphaL<TransitionEffect<BrownNoiseFlicker<Rgb<255,150,0>,Black,50>,Moccasin,TrInstant,TrFade<300>,EFFECT_CLASH>,Bump<Scale<BladeAngle<>,Int<25000>,Int<8000>>,Int<18000>>>,TrFade<600>>,EFFECT_CLASH>,
      // Stab
    TransitionEffectL<TrConcat<TrInstant,RgbArg<STAB_COLOR_ARG,GreenYellow>,TrDelay<25>,AlphaL<Black,Int<0>>,TrWipeIn<300>,AlphaL<Stripes<5000,1000,DarkOrange,Rgb<80,60,0>,Orange,Rgb<60,30,0>,Black,DarkOrange,Orange>,SmoothStep<Int<20000>,Int<2000>>>,TrJoin<TrSmoothFade<900>,TrWipe<700>>>,EFFECT_STAB>,
      // Blast
    MultiTransitionEffectL<TrConcat<TrDelay<700>,AlphaL<RgbArg<BLAST_COLOR_ARG,Black>,Int<0>>,TrFade<800>,LocalizedClashL<HumpFlicker<RgbArg<BLAST_COLOR_ARG,DarkOrange>,Red,50>,2000,50,EFFECT_BLAST>,TrFade<600>>,EFFECT_BLAST>,
    LocalizedClashL<Blinking<RgbArg<BLAST_COLOR_ARG,White>,Black,130,800>,1560,50,EFFECT_BLAST>,
    LocalizedClashL<Blinking<RgbArg<BLAST_COLOR_ARG,Red>,Black,130,500>,1560,15,EFFECT_BLAST>,
      // Lockup
    LockupTrL<Layers<
    AlphaL<Blinking<RgbArg<LOCKUP_COLOR_ARG,Tomato>,Strobe<Yellow,Black,15,30>,60,500>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SlowNoise<Int<3000>>,Int<8000>,Int<18000>>>>,
    AlphaL<BrownNoiseFlicker<Yellow,Black,50>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,AlphaL<Blinking<White,Strobe<BrownNoiseFlicker<Yellow,Black,500>,Black,15,30>,60,500>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SlowNoise<Int<3000>>,Int<25000>,Int<32000>>>>,TrFade<500>>,TrSmoothFade<900>,SaberBase::LOCKUP_NORMAL>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Strobe<GreenYellow,Black,20,30>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<15000>>>,TrFade<600>>,EFFECT_LOCKUP_BEGIN>,
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<Strobe<AlphaL<White,Int<20000>>,Black,20,30>,30>,TrSmoothFade<250>>,EFFECT_LOCKUP_BEGIN>,
    TransitionEffectL<TrConcat<TrInstant,Strobe<GreenYellow,Black,20,30>,TrFade<100>>,EFFECT_LOCKUP_BEGIN>,
    TransitionEffectL<TrConcat<TrInstant,Strobe<GreenYellow,Black,20,30>,TrFade<200>,BrownNoiseFlickerL<AlphaL<White,Int<16000>>,Int<50>>,TrJoinR<TrWipe<200>,TrWipeIn<200>,TrFade<300>>>,EFFECT_LOCKUP_END>,
      // Non-Responsive Drag
    LockupTrL<AlphaL<AudioFlicker<BrownNoiseFlicker<Strobe<RgbArg<DRAG_COLOR_ARG,Black>,OrangeRed,20,25>,Yellow,200>,White>,SmoothStep<Int<30000>,Int<2000>>>,TrConcat<TrInstant,GreenYellow,TrDelay<25>,AlphaL<Black,Int<0>>,TrFade<150>>,TrColorCycle<1500,-2000,100>,SaberBase::LOCKUP_DRAG>,
      // Lightning Block
    ResponsiveLightningBlockL<RandomFlicker<Strobe<RgbArg<LB_COLOR_ARG,White>,Rgb16<21301,0,65535>,50,10>,BrownNoiseFlicker<Rgb16<21301,0,65535>,Black,500>>,
      //Begin Lightning Transition
    TrConcat<TrInstant,GreenYellow,TrDelay<25>,BrownNoiseFlicker<Rgb16<21301,0,65535>,Black,500>,TrFade<100>>,
      //End Lightning Transition
    TrConcat<TrInstant,GreenYellow,TrDelay<25>,BrownNoiseFlicker<Rgb16<21301,0,65535>,Black,500>,TrFade<150>,BrownNoiseFlickerL<AlphaL<White,Int<16000>>,Int<50>>,TrJoinR<TrWipe<200>,TrWipeIn<200>,TrFade<400>>>>,
      // Melt
    LockupTrL<
      //Melt Colors
    AlphaL<Mix<TwistAngle<>,Yellow,RandomPerLEDFlicker<Orange,OrangeRed>,BrownNoiseFlicker<Rgb16<20095,128,128>,Rgb16<35103,8175,298>,150>,StyleFire<Rgb16<20393,93,93>,Red,0,4,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,4000,0>>>,
      //Melt Shape
    SmoothStep<Scale<TwistAngle<>,Int<24000>,Int<29000>>,Int<2000>>>,
      //Melt Begin and End transitions
    TrConcat<TrInstant,GreenYellow,TrDelay<25>,AlphaL<Black,Int<0>>,TrWipeIn<600>>,TrColorCycle<1500,-2000,100>,SaberBase::LOCKUP_MELT>,
      // Ignition Powerup
    TransitionEffectL<TrConcat<TrInstant,BrownNoiseFlicker<DeepSkyBlue,White,100>,TrFade<800>>,EFFECT_IGNITION>,
      // In/out using Trigger for instantaneous Center-out wipes
    InOutTrL<TrConcat<TrInstant,Layers<
    AlphaL<Black,SmoothStep<Scale<Trigger<EFFECT_IGNITION,Int<300>,Int<1000>,Int<500>>,Int<16385>,Int<0>>,Int<-1>>>,
    AlphaL<Black,SmoothStep<Scale<Trigger<EFFECT_IGNITION,Int<300>,Int<1000>,Int<500>>,Int<16384>,Int<32768>>,Int<0>>>>,TrDelay<1000>>,TrJoin<TrWipe<700>,TrWipeIn<700>>>,
    TransitionEffectL<TrConcat<TrInstant,Cyan,TrDelay<50>>,EFFECT_IGNITION>,
      // Asteroid impacts after detonation - timed sequence
    TransitionEffectL<TrConcat<
    TrDelay<1500>,GreenYellow,TrDelay<25>,AlphaL<BrownNoiseFlicker<Red,White,100>,Bump<Scale<SlowNoise<Int<30000>>,Int<3000>,Int<6000>>,ChangeSlowly<SlowNoise<Int<30000>>,Int<20000>>>>,TrDelay<100>,AlphaL<Red,Int<0>>,
    TrDelay<400>,GreenYellow,TrDelay<25>,AlphaL<BrownNoiseFlicker<Orange,White,100>,Bump<Scale<SlowNoise<Int<30000>>,Int<6000>,Int<10000>>,ChangeSlowly<SlowNoise<Int<30000>>,Int<20000>>>>,TrDelay<100>,AlphaL<Red,Int<0>>,
    TrDelay<200>,GreenYellow,TrDelay<25>,AlphaL<BrownNoiseFlicker<Black,White,100>,Bump<ChangeSlowly<SlowNoise<Int<30000>>,Int<60000>>,ChangeSlowly<SlowNoise<Int<30000>>,Int<20000>>>>,TrDelay<300>,AlphaL<Red,Int<0>>,
    TrDelay<200>,GreenYellow,TrDelay<25>,AlphaL<BrownNoiseFlicker<Yellow,Red,100>,Bump<Scale<SlowNoise<Int<30000>>,Int<10000>,Int<28000>>,ChangeSlowly<SlowNoise<Int<30000>>,Int<20000>>>>,TrDelay<200>,AlphaL<Red,Int<0>>,
    TrDelay<500>,GreenYellow,TrDelay<25>,AlphaL<BrownNoiseFlicker<Orange,White,100>,Bump<ChangeSlowly<SlowNoise<Int<30000>>,Int<60000>>,ChangeSlowly<SlowNoise<Int<30000>>,Int<20000>>>>,TrDelay<200>,AlphaL<Red,Int<0>>,
    TrDelay<100>,GreenYellow,TrDelay<25>,AlphaL<BrownNoiseFlicker<Red,White,100>,Bump<Scale<SlowNoise<Int<30000>>,Int<5000>,Int<13000>>,ChangeSlowly<SlowNoise<Int<30000>>,Int<20000>>>>,TrDelay<100>,AlphaL<Red,Int<0>>,
    TrDelay<500>,GreenYellow,TrDelay<25>>,EFFECT_IGNITION>,
      // Seismic Charge preon effect. use with linked preon sound
    TransitionEffectL<TrConcat<TrDelay<2125>,RgbArg<PREON_COLOR_ARG,Cyan>,TrFade<100>,Black,TrDelay<10>,Cyan,TrFade<150>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrDelay<2125>,AlphaL<RgbArg<PREON_COLOR_ARG,White>,Bump<Int<16384>,Int<32768>>>,TrFade<100>,Black,TrDelay<10>,AlphaL<White,Bump<Int<16384>,Int<32768>>>,TrFade<150>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrDelay<2100>,AlphaL<BrownNoiseFlicker<RgbArg<PREON_COLOR_ARG,DeepSkyBlue>,White,100>,Bump<Int<16384>,Int<3000>>>,TrDelay<100>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<4000>>>,
    TrSmoothFade<50>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<5000>>>,
    TrSmoothFade<50>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<10000>>>,
    TrSmoothFade<50>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<20000>>>,
    TrSmoothFade<175>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<30000>>>,
    TrSmoothFade<125>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<35000>>>,
    TrSmoothFade<150>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<30000>>>,
    TrSmoothFade<75>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<20000>>>,
    TrSmoothFade<50>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<8000>>>,
    TrSmoothFade<25>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<3000>>>,
    TrFade<50>,AlphaL<BrownNoiseFlicker<DeepSkyBlue,White,100>,Bump<Int<16384>,Int<20000>>>,TrDelay<25>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<BrownNoiseFlicker<RgbArg<PREON_COLOR_ARG,DeepSkyBlue>,White,100>,Bump<Scale<Trigger<EFFECT_PREON,Int<2000>,Int<1000>,Int<500>>,Int<-1000>,Int<16500>>,Int<1500>>>,TrDelay<2000>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrDelay<1100>,AlphaL<RgbArg<PREON_COLOR_ARG,Red>,Int<0>>,TrSmoothFade<700>,Black,TrDelay<100>,AlphaL<White,Bump<Int<16384>,Int<3000>>>,TrDelay<100>>,EFFECT_PREON>>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<EASYBLADE(BLUE,MAGENTA),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<EASYBLADE(Purple,BLUE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers<RandomFlicker<RotateColorsX<Variation,DeepSkyBlue>,RotateColorsX<Variation,Rgb<0,20,128>>>,LockupTrL<BrownNoiseFlickerL<White,Int<200>>,TrConcat<TrInstant,White,TrFade<300>>,TrFade<300>,SaberBase::LOCKUP_NORMAL>,LockupTrL<BrownNoiseFlickerL<White,Int<200>>,TrConcat<TrInstant,White,TrFade<300>>,TrFade<300>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,BlastL<Moccasin>,SimpleClashL<Moccasin>,InOutTrL<TrInstant,TrFade<300>,Pulsing<RotateColorsX<Variation,DeepSkyBlue>,RotateColorsX<Variation,Rgb<0,4,10>>,3000>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "slave1"},

   { "TheMaw;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers<
    Mix<SmoothStep<Int<16000>,Int<10000>>,
      StripesX<Scale<TwistAngle<1>,Int<1500>,Int<3000>>,Int<-2000>,RotateColorsX<Variation,Rgb<60,0,255>>,Black>,
      StripesX<Scale<TwistAngle<1>,Int<1500>,Int<3000>>,Int<2000>,RotateColorsX<Variation,Rgb<60,0,255>>,Black>>,
    // Ice Force
    TransitionEffectL<
      TrConcat<TrFade<7000>,Rgb<10,0,75>,// blade turns solid cold 1000ms before ice full coverage
      TrDelay<1000>>,EFFECT_FORCE>,  // hidden by ice overlay at this point, so no reason to have anything more than 1000 before back to base color
    TransitionEffectL< TrConcat<TrWipe<8000>, //wipe in stripes ice freezing effect
    TransitionEffect<Stripes<10000,-1000,Rgb<50,75,165>,Rgb<25,37,82>,Rgb<50,75,165>,Rgb<13,19,41>>,Sparkle<Rgb<100,150,230>,Rgb<100,200,255>,50,300>,TrFade<11500>,TrDelay<20000>,EFFECT_FORCE>, // Stripes fade to solid over remaining 4 seconds from freezing edge's finish until 12000 then hold for next joined delay to end at 15000, so 3000 more.
      TrDelay<7000>,Sparkle<Rgb<100,150,230>,Rgb<100,200,255>,50,300>, //after 15000 (TrConcat makes it 8000 + 7000) from effect start, full blade to this shade of white for wiping away (seamless color from end of stripes fade to solid)
      TrJoin<TrWipe<7000>,TrSmoothFade<9500>>>,EFFECT_FORCE>, // wipe ice away and melt to thinner opacity , but not 100%, leave like 30% by using longer fade duration than the wipe takes.
    TransitionEffectL<TrConcat<
      TrJoin<TrWaveX<BrownNoiseFlicker<Rgb<100,200,255>,DeepSkyBlue,60>,     Int<400>,  Int<50>, Int<1000>,Int<0>>,    // burst
        TrWaveX<BrownNoiseFlicker<Rgb<100,200,255>,SteelBlue,600>,      Int<15000>,Int<400>,Int<8000>,Int<0>>,         // leading freeze edge
        TrWaveX<Sparkle<AlphaL<Rgb<100,200,255>,Int<0>>,Rgb<100,200,255>,3000,200>,Int<15000>,Int<100>,Int<8000>,Int<-7000>>>,    // crystalizing in edge's wake
    AlphaL<Rgb<100,200,255>,Int<0>>,
      TrJoin<TrWaveX<BrownNoiseFlicker<Rgb<100,200,255>,DeepSkyBlue,300>,    Int<300>,  Int<50>, Int<1000>,Int<0>>,  // burst
        TrWaveX<Sparkle<AlphaL<Rgb<100,200,255>,Int<0>>,Rgb<100,200,255>,3000,200>,Int<40000>,Int<300>,Int<7000>,Int<3000>>, // crystalizing in thaw's path
        TrWaveX<BrownNoiseFlicker<DarkOrange,Yellow,80>,     Int<15000>,Int<400>,Int<7000>,Int<0>>>>,EFFECT_FORCE>, // leading thawing edge
    // Dim Blade 50% "Power Save" button combo
    EffectSequence<EFFECT_POWERSAVE,
    AlphaL<Black,Int<16384>>,
    AlphaL<Black,Int<0>>>,
    // Clash
    TransitionEffectL<TrConcat<TrInstant,GreenYellow,TrDelay<25>,
      AlphaL<TransitionEffect<
          TransitionEffect<Strobe<Rgb<255,150,50>,Black,20,20>,BrownNoiseFlicker<Rgb<100,200,255>,Black,300>,TrInstant,TrFade<300>,EFFECT_CLASH>,
        Rgb<100,200,255>,TrInstant,TrDelay<150>,EFFECT_CLASH>,
      Bump<Scale<BladeAngle<>,Int<25000>,Int<8000>>,Int<18000>>>,TrFade<600>>,EFFECT_CLASH>,
    // Stab
    TransitionEffectL<TrConcat<TrInstant,GreenYellow,TrDelay<25>,AlphaL<Black,Int<0>>,TrWipeIn<300>,AlphaL<Stripes<5000,1000,Orange,DarkOrange,Rgb<150,60,0>,Rgb<60,30,0>,Rgb<150,14,0>,OrangeRed>,SmoothStep<Int<20000>,Int<20000>>>,TrJoin<TrSmoothFade<900>,TrWipe<700>>>,EFFECT_STAB>,
    // Blast
    TransitionEffectL<TrConcat<TrInstant,GreenYellow,TrDelay<25>>,EFFECT_BLAST>,
    BlastL<EffectSequence<EFFECT_BLAST,Red,Green,Cyan,GreenYellow>,850,250,351>,
    AlphaL<TransitionEffectL<TrConcat<TrDelay<300>,Red,TrFade<500>>,EFFECT_BLAST>,BlastF<700,250,100000>>,
    BlastL<Rgb<100,200,255>,500,350,100000>,
    // Lockup - BC custom range mid-blade w/random strobe flash
    LockupTrL<Layers<StrobeX<AlphaL<Black,Int<0>>,White,Scale<SlowNoise<Int<1000>>,Int<0>,Int<2>>,Scale<SlowNoise<Int<1000>>,Int<10>,Int<50>>>,
      AlphaL<Blinking<Tomato,Strobe<Yellow,Black,15,30>,60,500>,                                                Bump<Scale<BladeAngle<5000,28000>,Scale<BladeAngle<8000,16000>,Int<3000>,Int<44000>>,Int<3000>>,Scale<SlowNoise<Int<3000>>,Int<8000>,Int<18000>>>>,
      AlphaL<Blinking<BrownNoiseFlicker<Rgb<100,200,255>,Black,50>,BrownNoiseFlicker<Yellow,Tomato,50>,100,500>,Bump<Scale<BladeAngle<5000,28000>,Scale<BladeAngle<8000,16000>,Int<3000>,Int<44000>>,Int<3000>>,Int<9000>>>>,
      TrConcat<TrInstant,
      AlphaL<Blinking<Rgb<100,200,255>,Strobe<BrownNoiseFlicker<Yellow,Black,500>,White,15,30>,60,500>,         Bump<Scale<BladeAngle<5000,28000>,Scale<BladeAngle<8000,16000>,Int<3000>,Int<44000>>,Int<3000>>,Scale<SlowNoise<Int<3000>>,Int<25000>,Int<32000>>>>,TrFade<500>>,TrSmoothFade<900>,SaberBase::LOCKUP_NORMAL>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Strobe<GreenYellow,Black,20,30>,                                Bump<Scale<BladeAngle<5000,28000>,Scale<BladeAngle<8000,16000>,Int<3000>,Int<44000>>,Int<3000>>,Int<15000>>>,TrFade<600>>,EFFECT_LOCKUP_BEGIN>,
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<Strobe<AlphaL<Rgb<100,200,255>,Int<20000>>,Black,20,30>,30>,TrSmoothFade<250>>,EFFECT_LOCKUP_BEGIN>,
    TransitionEffectL<TrConcat<TrInstant,GreenYellow,TrDelay<25>>,EFFECT_LOCKUP_BEGIN>,
    TransitionEffectL<TrConcat<TrInstant,Strobe<GreenYellow,Black,20,30>,TrFade<200>,BrownNoiseFlickerL<AlphaL<Rgb<100,200,255>,Int<16000>>,Int<50>>,TrJoinR<TrWipe<200>,TrWipeIn<200>,TrFade<300>>>,EFFECT_LOCKUP_END>,
    // Non-Responsive Drag
    LockupTrL<AlphaL<AudioFlicker<BrownNoiseFlicker<Strobe<Black,OrangeRed,20,25>,Yellow,200>,Rgb<100,200,255>>,SmoothStep<Int<30000>,Int<2000>>>,TrConcat<TrInstant,GreenYellow,TrDelay<25>,AlphaL<Black,Int<0>>,TrFade<150>>,TrColorCycle<1500,-2000,100>,SaberBase::LOCKUP_DRAG>,
    // Lightning Block - Non-responsive. BC custom color (purple hint w/ random strobe flashes because....lightning)
    LockupTrL<Layers<AlphaL<StrobeX<Black,White,Scale<SlowNoise<Int<1000>>,Int<0>,Int<6>>,Scale<SlowNoise<Int<1000>>,Int<10>,Int<50>>>,Int<22000>>,AlphaL<RandomFlicker<Strobe<Rgb<100,200,255>,Rgb<60,0,255>,50,10>,BrownNoiseFlicker<Rgb<60,0,255>,Black,500>>,LayerFunctions<
      Bump<Scale<SlowNoise<Int<2000>>,Int<3000>,Int<16000>>,Scale<BrownNoiseF<Int<10>>,Int<14000>,Int<8000>>>,
      Bump<Scale<SlowNoise<Int<2300>>,Int<26000>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<10000>>>,
      Bump<Scale<SlowNoise<Int<2300>>,Int<20000>,Int<30000>>,Scale<IsLessThan<SlowNoise<Int<1500>>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<0>>,Int<0>>>>>>,
      // Begin Lightning Transition
      TrConcat<TrInstant,GreenYellow,TrDelay<25>,BrownNoiseFlicker<Rgb<60,0,255>,Black,500>,TrFade<100>>,
      // End Lightning Transition
      TrConcat<TrInstant,GreenYellow,TrDelay<25>,BrownNoiseFlicker<Rgb<60,0,255>,Black,500>,TrFade<150>,BrownNoiseFlickerL<AlphaL<Rgb<100,200,255>,Int<16000>>,Int<50>>,TrJoinR<TrWipe<200>,TrWipeIn<200>,TrFade<400>> >,SaberBase::LOCKUP_LIGHTNING_BLOCK>,
    // Melt
    LockupTrL<
      AlphaL<Mix<TwistAngle<>,Yellow,RandomPerLEDFlicker<Orange,OrangeRed>,BrownNoiseFlicker<Rgb<78,0,0>,Rgb<137,32,1>,150>,StyleFire<Rgb<80,0,0>,Red,0,4,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,4000,0>>>,
      // Melt Shape
      SmoothStep<Scale<TwistAngle<>,Int<24000>,Int<29000>>,Int<2000>>>,
      // Melt Begin and End transitions
      TrConcat<TrInstant,GreenYellow,TrDelay<25>,AlphaL<Black,Int<0>>,TrWipeIn<600>>,TrColorCycle<1500,-2000,100>,SaberBase::LOCKUP_MELT>,
    // Ignition and Retraction
    TransitionEffectL<TrConcat<TrDelay<200>,Strobe<Black,Rgb<100,100,150>,15,40>,TrJoin<TrWipeInSparkTip<Rgb<100,200,255>,800,200>,TrWipeSparkTip<Rgb<100,200,255>,800,200>>>,EFFECT_IGNITION>,
    TransitionEffectL<TrConcat<TrFade<700>,Strobe<Black,Rgb<100,100,150>,15,40>,TrDelay<300>>,EFFECT_RETRACTION>,
    InOutTrL<
      TrJoin<
        TrJoin<TrWipeIn<800>,TrWaveX<White,Int<800>,Int<500>,Int<800>,Int<32768>>>,
        TrJoin<TrWipe<800>,TrWaveX<White,Int<400>,Int<500>,Int<800>,Int<0>>>>,
      TrJoin<
        TrJoin<TrWipeInX<Int<2000>>,TrWaveX<White,Int<1000>,Int<500>,Int<2000>,Int<32768>>>,
        TrJoin<TrWipeX<Int<2000>>,TrWaveX<White,Int<1000>,Int<500>,Int<2000>,Int<0>>>>>,
    TransitionEffectL<TrConcat<TrInstant,White,TrFade<50>>,EFFECT_IGNITION>,
    TransitionEffectL<TrConcat<TrDelayX<Int<1000>>,White,TrFade<50>>,EFFECT_RETRACTION>,
    // Auto-dim blade on Low batt
    AlphaL<Black,Scale<IsLessThan<BatteryLevel,Int<11000>>,Int<0>,Int<16384>>>,
    // On-Demand Battery Level
    AlphaL<TransitionEffectL<TrConcat<TrSmoothFade<500>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<Scale<BatteryLevel,Int<0>,Int<35000>>,Int<-1>>>,TrDelay<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-1>>>,TrSmoothFade<1000>>,EFFECT_BATTERY_LEVEL>,Ifon<Int<0>,Int<32768>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "TheMaw\nyellow"},

   { "EctoBuster;common", "common/tracks/mars.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "ectobuster\ngradient"},

   { "TeensySF;common", "common/tracks/venus.wav",
   // Main Blade
/* copyright Fett263 Rotoscope (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#Rotoscope
OS7.14 v2.52p
This Style Contains 25920 Unique Combinations
Style Options:
Default (0): Hyper Responsive Rotoscope (Original Trilogy)
1: Hyper Responsive Rotoscope (Prequels)
2: Hyper Responsive Rotoscope (Sequels)

Base Color: BaseColorArg (0)

--Effects Included--
Preon Effect Options: Default (0): Disable, 1: Faulty Ignition, 2: Faulty Ignition (Volatile Version), 3: Rey TROS Spark, 4: Sparking, 5: Overload [Color: PreonColorArg]
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Down = Fast) [Color: IgnitionColorArg]
PowerUp Effect Options: Default (0): Disable, 1: Power Flash, 2: Rey TROS Spark, 3: Unstable Power Up Forward, 4: Power Surge (Unstable V1) [Color: IgnitionColorArg]
Retraction Effect Options: Default (0): Standard Retraction, 1: SparkTip Retraction, 2: Dual Mode Wipe In (Up = Fast), 3: Glitch Off [Color: RetractionColorArg]
CoolDown Effect Options: Default (0): Disable, 1: Power Flash, 2: Power Burst Reverse, 3: Unstable Cool Down Reverse [Color: RetractionColorArg]
PostOff Effect Options: Default (0): Disable, 1: Battery Glow (Preon Size), 2: Emitter Glow (Preon Size) [Color: PostOffColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Blast Effect (Randomly Selected): Blast Wave (Random), Responsive Blast Wave (Random), Responsive Blast Fade (Random), Responsive Blast Ripple Fade [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Swing Effect Options: Default (0): Disable, 1: Interactive Power Build-up (Force Toggle) [Color: SwingColorArg]
Battery Level: % Blade (Green to Red)
Display Volume: % Blade [Color: BaseColorArg]
Battery Monitor: Passive Battery Monitor (Retraction)
Swing Option 1- Interactive Power Build-up (Force Toggle) Interactive Control: Force to enable/disable, hold steady to charge up Power and swing to throw.
*/
    //StylePtr<Layers<ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Mix<HoldPeakF<SwingSpeed<250>,Scale<SwingAcceleration<100>,Int<50>,Int<500>>,Scale<SwingAcceleration<>,Int<20000>,Int<10000>>>,RandomFlicker<StripesX<Int<15000>,Scale<HoldPeakF<SwingSpeed<200>,Scale<SwingAcceleration<100>,Int<50>,Int<300>>,Scale<SwingAcceleration<100>,Int<24000>,Int<16000>>>,Int<-3200>,Int<-200>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,Mix<Int<19276>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,Mix<HoldPeakF<SwingSpeed<250>,Scale<SwingAcceleration<100>,Int<50>,Int<500>>,Scale<SwingAcceleration<>,Int<20000>,Int<10000>>>,RandomFlicker<StripesX<Int<24000>,Scale<HoldPeakF<SwingSpeed<200>,Scale<SwingAcceleration<100>,Int<50>,Int<300>>,Scale<SwingAcceleration<100>,Int<24000>,Int<16000>>>,Int<-1400>,Int<-100>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,Mix<Int<19276>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,Mix<HoldPeakF<SwingSpeed<250>,Scale<SwingAcceleration<100>,Int<50>,Int<500>>,Scale<SwingAcceleration<>,Int<20000>,Int<10000>>>,RandomFlicker<StripesX<Int<14000>,Scale<HoldPeakF<SwingSpeed<200>,Scale<SwingAcceleration<100>,Int<50>,Int<300>>,Scale<SwingAcceleration<100>,Int<24000>,Int<16000>>>,Int<-4000>,Int<-400>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,Mix<Int<19276>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>>>,ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,ColorSelect<IncrementF<EffectPulseF<EFFECT_FORCE>,Int<32000>,Int<2>>,TrSmoothFade<500>,AlphaL<RgbArg<BASE_COLOR_ARG,Red>,Int<0>>,AlphaL<StripesX<Int<10000>,Scale<IncrementWithReset<ThresholdPulseF<Ifon<StrobeF<Int<12>,Int<1>>,Int<0>>,Int<32000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>>,Int<0>>,Int<24000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<10000>,Int<100>>,Int<-50>,Int<-8000>>,Mix<Int<10000>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<20000>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<Sum<IncrementWithReset<ThresholdPulseF<StrobeF<Int<10>,Int<1>>,Int<30000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<4000>,Int<100>>,IncrementWithReset<ThresholdPulseF<Ifon<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>,Int<0>>,Int<16000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<38000>,Int<38000>>>,Int<-2000>>>>>,TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelay<200>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<300>,AlphaL<Blue,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<500>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<1200>>>,EFFECT_IGNITION>,TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>>,EFFECT_RETRACTION>,TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaMixL<Bump<Scale<BladeAngle<>,Int<28000>,Int<8000>>,Scale<EffectPosition<EFFECT_BLAST>,Int<9000>,Int<13000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,IgnitionTime<300>,Mult<IgnitionTime<300>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<0>,Int<16384>>,RetractionTime<0>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<0>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>>,Black>,TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<Black,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Rgb<50,50,50>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,RotateColorsX<Int<4000>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>>,White,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<Black,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<6425>,Black,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>>,RotateColorsX<Int<21845>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrJoin<TrDelayX<Int<100>>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrWipe<100>,AlphaL<DodgerBlue,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrDelayX<Sum<WavLen<EFFECT_PREON>,Int<200>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,-200>,Sum<IntArg<PREON_SIZE_ARG,-200>,Int<4000>>>,Int<-2000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,10>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,15>,SmoothStep<Sum<Int<2000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,20>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrBoingX<Mult<Int<8192>,WavLen<EFFECT_PREON>>,3>>>,EFFECT_PREON>,TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelayX<Int<500>>,TrFade<500>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFade<500>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>>,EFFECT_POSTOFF>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>,TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Mix<HoldPeakF<SwingSpeed<250>,Scale<SwingAcceleration<100>,Int<50>,Int<500>>,Scale<SwingAcceleration<>,Int<20000>,Int<10000>>>,RandomFlicker<StripesX<Int<15000>,Scale<HoldPeakF<SwingSpeed<200>,Scale<SwingAcceleration<100>,Int<50>,Int<300>>,Scale<SwingAcceleration<100>,Int<24000>,Int<16000>>>,Int<-3200>,Int<-200>>,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<19276>,Black,RgbArg<BASE_COLOR_ARG,Blue>>>,RgbArg<BASE_COLOR_ARG,Blue>>,RgbArg<BASE_COLOR_ARG,Blue>>,Mix<HoldPeakF<SwingSpeed<250>,Scale<SwingAcceleration<100>,Int<50>,Int<500>>,Scale<SwingAcceleration<>,Int<20000>,Int<10000>>>,RandomFlicker<StripesX<Int<24000>,Scale<HoldPeakF<SwingSpeed<200>,Scale<SwingAcceleration<100>,Int<50>,Int<300>>,Scale<SwingAcceleration<100>,Int<24000>,Int<16000>>>,Int<-1400>,Int<-100>>,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<19276>,Black,RgbArg<BASE_COLOR_ARG,Blue>>>,RgbArg<BASE_COLOR_ARG,Blue>>,RgbArg<BASE_COLOR_ARG,Blue>>,Mix<HoldPeakF<SwingSpeed<250>,Scale<SwingAcceleration<100>,Int<50>,Int<500>>,Scale<SwingAcceleration<>,Int<20000>,Int<10000>>>,RandomFlicker<StripesX<Int<14000>,Scale<HoldPeakF<SwingSpeed<200>,Scale<SwingAcceleration<100>,Int<50>,Int<300>>,Scale<SwingAcceleration<100>,Int<24000>,Int<16000>>>,Int<-4000>,Int<-400>>,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<19276>,Black,RgbArg<BASE_COLOR_ARG,Blue>>>,RgbArg<BASE_COLOR_ARG,Blue>>,RgbArg<BASE_COLOR_ARG,Blue>>>,
      ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,ColorSelect<IncrementF<EffectPulseF<EFFECT_FORCE>,Int<32000>,Int<2>>,TrSmoothFade<500>,AlphaL<RgbArg<BASE_COLOR_ARG,Red>,Int<0>>,AlphaL<StripesX<Int<10000>,Scale<IncrementWithReset<ThresholdPulseF<Ifon<StrobeF<Int<12>,Int<1>>,Int<0>>,Int<32000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>>,Int<0>>,Int<24000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<10000>,Int<100>>,Int<-50>,Int<-8000>>,Mix<Int<10000>,Black,RgbArg<SWING_COLOR_ARG,White>>,Mix<Int<20000>,Black,RgbArg<SWING_COLOR_ARG,White>>,RgbArg<SWING_COLOR_ARG,White>>,SmoothStep<Sum<IncrementWithReset<ThresholdPulseF<StrobeF<Int<10>,Int<1>>,Int<30000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<4000>,Int<100>>,IncrementWithReset<ThresholdPulseF<Ifon<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>,Int<0>>,Int<16000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<38000>,Int<38000>>>,Int<-2000>>>>>,
      TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelay<200>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<300>,AlphaL<Blue,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<500>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<1200>>>,EFFECT_IGNITION>,
      TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
      TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaMixL<Bump<Scale<BladeAngle<>,Int<28000>,Int<8000>>,Scale<EffectPosition<EFFECT_BLAST>,Int<9000>,Int<13000>>>,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,White>>>,TrFade<300>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,IgnitionTime<>,Mult<IgnitionTime<>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,White>,BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<>,Int<16384>>,RetractionTime<>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,TrWipeInX<Mult<RetractionTime<>,Int<16384>>>>>>,
      TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<RgbArg<PREON_COLOR_ARG,Black>,RgbArg<PREON_COLOR_ARG,White>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,White>,Rgb<50,50,50>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,White>,RotateColorsX<Int<4000>,RgbArg<PREON_COLOR_ARG,White>>>,White,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<Black,RgbArg<PREON_COLOR_ARG,White>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,White>,Mix<Int<6425>,Black,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,White>>>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,White>,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,White>>>,RotateColorsX<Int<21845>,RgbArg<PREON_COLOR_ARG,White>>,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrJoin<TrDelayX<Int<100>>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrWipe<100>,AlphaL<DodgerBlue,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrDelayX<Sum<WavLen<EFFECT_PREON>,Int<200>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,-200>,Sum<IntArg<PREON_SIZE_ARG,-200>,Int<4000>>>,Int<-2000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,White>,10>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,White>,15>,SmoothStep<Sum<Int<2000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,White>,20>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrBoingX<Mult<Int<8192>,WavLen<EFFECT_PREON>>,3>>>,EFFECT_PREON>,
      TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,Int<0>>,TrJoin<TrDelayX<Int<500>>,TrFade<500>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFade<500>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>>,EFFECT_POSTOFF>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>,
      TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Blue>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    // === NPXL inner ring: ===
    StylePtr<MarbleStyle>(),
    // === NPXL outer ring: ===
    StylePtr<MarbleStyle>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<CYAN,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Cyan>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Cyan>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "cyan"},

   { "SmthJedi;common", "common/tracks/mars.wav",
    // === Main Blade: ===
      //Theory2Button.h "Kaboom"
    //StylePtr<Layers<Layers<Mix<SmoothStep<Int<16384>,Int<4000>>,Stripes<10000,200,Rgb<0,20,90>,Rgb<0,50,240>>,Stripes<10000,-200,Rgb<0,20,90>,Rgb<0,50,240>>>,TransitionEffectL<TrConcat<TrInstant,White,TrFade<600>>,EFFECT_IGNITION>>,LockupTrL<Layers<AlphaL<AudioFlickerL<White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red>,ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrJoinR<TrWipe<300>,TrWipeIn<300>>,TrJoin<TrWipe<5000>,TrWipeIn<5000>>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<Pulsing<Rgb<0,50,240>,Rgb<0,50,240>,400>,Bump<Scale<Trigger<EFFECT_PREON,Int<3000>,Int<1500>,Int<1500>>,Int<0>,Int<16384>>,Int<8000>>>,TrDelay<3000>,Mix<Bump<Int<16384>,Int<8000>>,Black,Rgb<0,50,240>>,TrJoin<TrWipe<1500>,TrWipeIn<1500>>,Mix<Bump<Int<16384>,Int<8000>>,Black,Rgb<0,50,240>>,TrDelay<160>>,EFFECT_PREON>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Layers<
        Mix<SmoothStep<Int<16384>,Int<4000>>,Stripes<10000,200,RgbArg<BASE_COLOR_ARG,Rgb<0,20,90>>,Rgb<0,50,240>>,Stripes<10000,-200,Rgb<0,20,90>,Rgb<0,50,240>>>,
        TransitionEffectL<TrConcat<TrInstant,White,TrFade<600>>,EFFECT_IGNITION>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,White>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<RgbArg<STAB_COLOR_ARG,Red>>,
      ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<RgbArg<CLASH_COLOR_ARG,White>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrJoinR<TrWipe<300>,TrWipeIn<300>>,TrJoin<TrWipe<5000>,TrWipeIn<5000>>>,
      TransitionEffectL<TrConcat<TrInstant,AlphaL<Pulsing<RgbArg<PREON_COLOR_ARG,Rgb<0,50,240>>,Rgb<0,50,240>,400>,Bump<Scale<Trigger<EFFECT_PREON,Int<3000>,Int<1500>,Int<1500>>,Int<0>,Int<16384>>,Int<8000>>>,TrDelay<3000>,Mix<Bump<Int<16384>,Int<8000>>,Black,Rgb<0,50,240>>,TrJoin<TrWipe<1500>,TrWipeIn<1500>>,Mix<Bump<Int<16384>,Int<8000>>,Black,Rgb<0,50,240>>,TrDelay<160>>,EFFECT_PREON>>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === Crystal Chamber: ===
        // A_Rogue_Child https://crucible.hubbe.net/t/possible-gremlin/3427/4
    //StylePtr<IgnitionDelay<100,Layers<Rgb<180,20,0>,InOutTrL<TrConcat<TrFade<200>,Rgb<2,0,0>,TrFade<300>,Rgb<30,4,0>,TrFade<400>,Rgb<80,10,0>,TrFade<300>>,TrConcat<TrFade<1500>,Rgb<80,10,0>,TrFade<200>,Rgb<40,4,0>,TrFade<100>,Rgb<5,0,0>,TrFade<100>>>>>>(),
    StylePtr<IgnitionDelay<100,Layers< //Same as above but "Layerized"
      RgbArg<BASE_COLOR_ARG,Rgb<180,20,0>>,
      InOutTrL<TrConcat<TrFade<200>,Rgb<2,0,0>,TrFade<300>,Rgb<30,4,0>,TrFade<400>,Rgb<80,10,0>,TrFade<300>>,TrConcat<TrFade<1500>,Rgb<80,10,0>,TrFade<200>,Rgb<40,4,0>,TrFade<100>,Rgb<5,0,0>,TrFade<100>>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "blue"},

   { "SmthGrey;common", "common/tracks/mercury.wav",
    // === Main Blade: ===
/* copyright Fett263 DarkSaber (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#DarkSaber
OS7.14 v2.52p
This Style Contains 1152 Unique Combinations
Style Options:
Default (0): Clone Wars Darksaber
1: Live Action Darksaber (Hyper-Responsive)

Base Color: BaseColorArg (0)

--Effects Included--
Preon Effect Options: Default (0): Disable, 1: Erratic, 2: Power Absorb, 3: Pilot Light [Color: PreonColorArg]
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Gravity Ignition [Color: IgnitionColorArg]
Retraction Effect Options: Default (0): Standard Retraction, 1: SparkTip Retraction, 2: Blink Off, 3: Glitch Off [Color: RetractionColorArg]
CoolDown Effect Options: Default (0): Disable, 1: Power Flash, 2: Power Burst Reverse, 3: Unstable Cool Down Reverse [Color: RetractionColorArg]
PostOff Effect Options: Default (0): Disable, 1: Battery Glow (Preon Size), 2: Emitter Glow (Preon Size) [Color: PostOffColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Blast Effect (Randomly Selected): Blast Wave (Random), Responsive Blast Wave (Random), Responsive Blast Fade (Random), Responsive Blast Ripple Fade [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Battery Level: % Blade (Green to Red)
Display Volume: % Blade [Color: BaseColorArg]
Battery Monitor: Passive Battery Monitor (Retraction)
*/
    //StylePtr<Layers<ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,AudioFlicker<BrownNoiseFlicker<Mix<SwingSpeed<400>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,White>>,Stripes<5000,-300,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<25700>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>>,300>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,White>>,Mix<HoldPeakF<SwingSpeed<400>,Scale<SwingAcceleration<>,Int<2000>,Int<4000>>,Int<8000>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,Stripes<35000,-200,Mix<Int<14000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<24000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<28000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>>,100>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>>,TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>>,EFFECT_RETRACTION>,TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaMixL<Bump<Scale<BladeAngle<>,Int<28000>,Int<8000>>,Scale<EffectPosition<EFFECT_BLAST>,Int<9000>,Int<13000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrSelect<Scale<IsLessThan<BladeAngle<>,Int<18384>>,Scale<IsGreaterThan<SwingAcceleration<>,Int<16384>>,Int<0>,Int<2>>,Int<1>>,TrWipeInX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipe<100>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrJoin<TrBoingX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>,3>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<0>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>>,Black>,TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrBoingX<Mult<Int<10922>,WavLen<EFFECT_PREON>>,5>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<100>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrBoingX<Mult<Int<10922>,WavLen<EFFECT_PREON>>,3>,Black,TrBoingX<Mult<Int<7260>,WavLen<EFFECT_PREON>>,3>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<200>>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrBoingX<Mult<Int<3664>,WavLen<EFFECT_PREON>>,3>>,TrConcat<TrInstant,AlphaL<AudioFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrLoopN<8,TrSparkX<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<200>,Divide<WavLen<EFFECT_PREON>,Int<8>>,Int<32768>>>>,TrConcat<TrExtendX<WavLen<EFFECT_PREON>,TrInstant>,AlphaL<PulsingL<Mix<Int<16384>,White,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>,Int<300>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrLoopN<9,TrBoingX<Mult<WavLen<EFFECT_PREON>,Int<2458>>,3>>>>,EFFECT_PREON>,TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelayX<Int<500>>,TrFade<500>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFade<500>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>>,EFFECT_POSTOFF>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>,TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,AudioFlicker<BrownNoiseFlicker<Mix<SwingSpeed<400>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,White>>,Stripes<5000,-300,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<25700>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<1285>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>>,300>,Mix<Int<6425>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,White>>,Mix<HoldPeakF<SwingSpeed<400>,Scale<SwingAcceleration<>,Int<2000>,Int<4000>>,Int<8000>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,Stripes<35000,-200,Mix<Int<14000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<24000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>,Mix<Int<28000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>>,100>,RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>>>,
      TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
      TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaMixL<Bump<Scale<BladeAngle<>,Int<28000>,Int<8000>>,Scale<EffectPosition<EFFECT_BLAST>,Int<9000>,Int<13000>>>,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,White>>>,TrFade<300>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrSelect<Scale<IsLessThan<BladeAngle<>,Int<18384>>,Scale<IsGreaterThan<SwingAcceleration<>,Int<16384>>,Int<0>,Int<2>>,Int<1>>,TrWipeInX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipe<100>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,White>,BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrJoin<TrBoingX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>,3>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,TrWipeInX<Mult<RetractionTime<>,Int<16384>>>>>>,
      TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrBoingX<Mult<Int<10922>,WavLen<EFFECT_PREON>>,5>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<100>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrBoingX<Mult<Int<10922>,WavLen<EFFECT_PREON>>,3>,Black,TrBoingX<Mult<Int<7260>,WavLen<EFFECT_PREON>>,3>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<200>>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrBoingX<Mult<Int<3664>,WavLen<EFFECT_PREON>>,3>>,TrConcat<TrInstant,AlphaL<AudioFlickerL<RgbArg<PREON_COLOR_ARG,White>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrLoopN<8,TrSparkX<RgbArg<PREON_COLOR_ARG,White>,Int<200>,Divide<WavLen<EFFECT_PREON>,Int<8>>,Int<32768>>>>,TrConcat<TrExtendX<WavLen<EFFECT_PREON>,TrInstant>,AlphaL<PulsingL<Mix<Int<16384>,White,RgbArg<PREON_COLOR_ARG,White>>,Int<300>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrLoopN<9,TrBoingX<Mult<WavLen<EFFECT_PREON>,Int<2458>>,3>>>>,EFFECT_PREON>,
      TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,Int<0>>,TrJoin<TrDelayX<Int<500>>,TrFade<500>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFade<500>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>>,EFFECT_POSTOFF>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>,
      TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Rgb<100,100,150>>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "fire"},

   { "SmthFuzz;common", "common/tracks/uranus.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<OrangeRed,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,OrangeRed>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,OrangeRed>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
        // A_Rogue_Child https://crucible.hubbe.net/t/possible-gremlin/3427/6
    //StylePtr<IgnitionDelay<3000,Layers<StripesX<Int<50>,Int<135>,RotateColorsX<Variation,Black>,RotateColorsX<Variation,Black>,RotateColorsX<Variation,Rgb<180,20,0>>,RotateColorsX<Variation,Rgb<30,4,0>>>>>>(),
    StylePtr<IgnitionDelayX<Int<3000>,Layers< //Same as above but "Layerized"
      StripesX<Int<50>,Int<135>,RgbArg<BASE_COLOR_ARG,Black>,Black,Rgb<180,20,0>,Rgb<30,4,0>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "red"},

   { "RgueCmdr;common", "common/tracks/ImperialMarchROTJ3_long.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,0>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,1>(),
    // === NPXL outer ring: ===
    //StylePtr<Layers<Mix<CircularSectionF<Saw<Int<200>>,Int<2000>>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,InOutTrL<TrColorCycle<10>,TrFade<1>,Mix<LinearSectionF<Saw<Int<20>>,Int<3000>>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Mix<CircularSectionF<Saw<Int<200>>,Int<2000>>,RgbArg<BASE_COLOR_ARG,Black>,RgbArg<BASE_COLOR_ARG,Red>>,
      InOutTrL<TrColorCycle<10>,TrFade<1>,Mix<LinearSectionF<Saw<Int<20>>,Int<3000>>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "blue fire"},

   { "TthCrstl;common", "common/tracks/rebels_theme.wav",
    // === Main Blade: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "green"},

   { "altereactor101;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    //StylePtr<Layers<Layers<ColorChange<TrInstant,DeepSkyBlue,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Rgb<185,212,212>>,RandomL<ColorChange<TrInstant,Rgb<0,107,205>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>>,TransitionEffectL<TrConcat<TrInstant,BrownNoiseFlickerL<AlphaL<White,Int<16000>>,Int<50>>,TrSmoothFade<600>>,EFFECT_LOCKUP_END>,LockupTrL<Strobe<White,Black,25,28>,TrConcat<TrInstant,White,TrFade<200>>,TrFade<300>,SaberBase::LOCKUP_NORMAL>,LockupTrL<AlphaL<White,LayerFunctions<Bump<Scale<SlowNoise<Int<2000>>,Int<3000>,Int<16000>>,Scale<BrownNoiseF<Int<10>>,Int<14000>,Int<8000>>>,Bump<Scale<SlowNoise<Int<2300>>,Int<26000>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<10000>>>,Bump<Scale<SlowNoise<Int<2300>>,Int<20000>,Int<30000>>,Scale<IsLessThan<SlowNoise<Int<1500>>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<0>>,Int<0>>>>>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,ResponsiveStabL<Red>,BlastL<LemonChiffon>,SimpleClashL<Strobe<Yellow,Black,25,28>,200>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipe<800>,TrWipeIn<1100>>,TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Layers<
        ColorChange<TrInstant,DeepSkyBlue,Blue,Cyan,DodgerBlue,Green,PaleGreen,Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Rgb<185,212,212>>,
        RandomL<ColorChange<TrInstant,Rgb<0,107,205>,Rgb<0,0,205>,Rgb<0,205,205>,Rgb<0,60,205>,Rgb<0,205,0>,Rgb<22,205,22>,Rgb<205,0,0>,Rgb<205,0,205>,Rgb<205,62,115>,Rgb<205,205,0>,Rgb<205,79,0>,Rgb<178,205,205>>>>,
      TransitionEffectL<TrConcat<TrInstant,BrownNoiseFlickerL<AlphaL<White,Int<16000>>,Int<50>>,TrSmoothFade<600>>,EFFECT_LOCKUP_END>,
      LockupTrL<Strobe<White,Black,25,28>,TrConcat<TrInstant,White,TrFade<200>>,TrFade<300>,SaberBase::LOCKUP_NORMAL>,
      LockupTrL<AlphaL<White,LayerFunctions<Bump<Scale<SlowNoise<Int<2000>>,Int<3000>,Int<16000>>,Scale<BrownNoiseF<Int<10>>,Int<14000>,Int<8000>>>,Bump<Scale<SlowNoise<Int<2300>>,Int<26000>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<10000>>>,Bump<Scale<SlowNoise<Int<2300>>,Int<20000>,Int<30000>>,Scale<IsLessThan<SlowNoise<Int<1500>>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<0>>,Int<0>>>>>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,
      ResponsiveStabL<Red>,
      BlastL<LemonChiffon>,
      SimpleClashL<Strobe<Yellow,Black,25,28>,200>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,RandomFlicker<Red,Black>,RandomFlicker<Yellow,Red>>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipe<800>,TrWipeIn<1100>>,
      TransitionEffectL<TrConcat<TrWipe<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    //StylePtr<Layers<Layers<ColorChange<TrInstant,DeepSkyBlue,Blue,Cyan,DodgerBlue,Green,Rgb<28,255,28>,Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,RandomL<Black>>,BlastFadeoutL<ColorChange<TrInstant,Red,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,LockupL<Strobe<ColorChange<TrInstant,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,Strobe<ColorChange<TrInstant,White,White,White,White,White,White,White,White,White,Red,Rgb<200,255,255>,Red>,Black,20,16>>,SimpleClashL<Strobe<ColorChange<TrInstant,Yellow,Yellow,Yellow,Yellow,LemonChiffon,Red,Yellow,Yellow,Yellow,Cyan,White,Yellow>,Black,28,25>,260>,InOutHelperL<InOutFuncX<Int<700>,Int<1100>>,Pulsing<ColorChange<TrInstant,Rgb<0,68,125>,Rgb<0,0,125>,Rgb<0,125,125>,Rgb<0,35,125>,Rgb<0,125,0>,Rgb<14,125,14>,Rgb<125,0,0>,Rgb<125,0,125>,Rgb<125,40,77>,Rgb<125,125,0>,Rgb<125,48,0>,Rgb<110,125,125>>,Black,2800>>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Layers<
        ColorChange<TrInstant,DeepSkyBlue,Blue,Cyan,DodgerBlue,Green,PaleGreen,Red,Magenta,Rgb<255,80,154>,Yellow,Orange,Azure>,
        RandomL<Black>>,
      BlastFadeoutL<ColorChange<TrInstant,Red,Red,Red,Red,Red,Red,LightYellow,Blue,Blue,Red,Blue,Red>>,
    LockupL<Strobe<ColorChange<TrInstant,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Rgb<200,255,255>,Yellow>,Black,20,16>,Strobe<ColorChange<TrInstant,White,White,White,White,White,White,White,White,White,Red,Rgb<200,255,255>,Red>,Black,20,16>>,
      SimpleClashL<Strobe<ColorChange<TrInstant,Yellow,Yellow,Yellow,Yellow,LemonChiffon,Red,Yellow,Yellow,Yellow,Cyan,White,Yellow>,Black,28,25>,260>,
      InOutHelperL<InOutFuncX<Int<700>,Int<1100>>,Pulsing<ColorChange<TrInstant,Rgb<0,68,125>,Rgb<0,0,125>,Rgb<0,125,125>,Rgb<0,35,125>,Rgb<0,125,0>,Rgb<14,125,14>,Rgb<125,0,0>,Rgb<125,0,125>,Rgb<125,40,77>,Rgb<125,125,0>,Rgb<125,48,0>,Rgb<110,125,125>>,Black,2800>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "altereactor"},

/* commented out to save space
   {"analog;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // Argified from olicomplex1.6.00.h
    AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb16<0,38402,65535>>,Rgb<0,85,200>>,
      //Ice blue audioflicker
    AlphaL<Rgb16<882,65535,31206>,SwingSpeed<500>>,
      //Bright cyan color swing
    AlphaL<Stripes<2500,-4000,Rgb16<0,38402,65535>,Rgb<0,26,60>,Pulsing<Rgb<0,13,30>,Black,800>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,
      //Hard swing ripple effect
    LockupTrL<Layers<
    AlphaL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,225,0>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
    AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<600>,AlphaL<Mix<SmoothStep<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<1000>>,Stripes<1500,2000,DeepSkyBlue,Rgb16<0,65535,30086>>,Stripes<1500,-2500,DeepSkyBlue,Rgb16<0,65535,30086>>>,Int<18000>>,TrFade<800>,AlphaL<Mix<SmoothStep<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<1000>>,Stripes<1500,1000,Rgb16<1514,65535,52727>,Rgb16<3934,65535,38402>>,Stripes<1500,-1250,Rgb16<1514,65535,52727>,Rgb16<3934,65535,38402>>>,Int<18000>>,TrFade<1100>>,SaberBase::LOCKUP_NORMAL>,
      //Responsive lockup with click calibration effect
      //AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<2100>>,SaberBase::LOCKUP_NORMAL>,
      //Alternate lockup ending with white fade out, replace the second AlphaL line above (ending in LOCKUP_NORMAL)
    ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      //Responsive lightning block
    AlphaL<Rgb16<0,65535,30086>,SmoothStep<Scale<SlowNoise<Int<2300>>,Int<1200>,Int<4500>>,Int<-4000>>>,
      //Sparking emitter flare
    ResponsiveStabL<RgbArg<STAB_COLOR_ARG,Red>>,
      //Responsive stab
    EffectSequence<EFFECT_BLAST,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,LocalizedClashL<RgbArg<BLAST_COLOR_ARG,White>,80,30,EFFECT_BLAST>,ResponsiveBlastWaveL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<500>,Int<200>>,Scale<SwingSpeed<400>,Int<100>,Int<400>>>,BlastL<RgbArg<BLAST_COLOR_ARG,White>,200,200>,ResponsiveBlastFadeL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<6000>,Int<12000>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<400>,Int<100>>,Scale<SwingSpeed<400>,Int<200>,Int<100>>,Scale<SwingSpeed<400>,Int<400>,Int<200>>>>,
      //Multi-blast, blaster reflect cycles through different responsive effects
    ResponsiveClashL<TransitionEffect<RgbArg<CLASH_COLOR_ARG,Rgb<255,240,80>>,LemonChiffon,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Responsive clash
    TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,White,RandomPerLEDFlicker<Rgb<60,60,60>,Black>,BrownNoiseFlicker<White,Rgb<30,30,30>,200>,RandomPerLEDFlicker<Rgb<80,80,80>,Rgb<30,30,30>>>,TrFade<1000>>,EFFECT_IGNITION>,
      //Bright HumpFlicker ignition effect
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<Rgb<90,180,255>,40>,TrFade<1200>>,EFFECT_RETRACTION>,
      //Bright HumpFlicker retraction effect
    LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      //Drag
    LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      //Responsive melt
    EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<8192>>,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<24576>>,AlphaL<Black,Int<0>>>,
      //Power save, if using fett263's prop file hold Aux and click PWR while ON (pointing up) to dim blade in 25% increments.
    InOutTrL<TrConcat<TrWipe<200>,AudioFlicker<DeepSkyBlue,Rgb<0,0,128>>,TrWipe<100>,Black,TrBoing<550,2>>,TrColorCycle<790>>,
      //Glitch out ignition and cycle down retraction
    TransitionEffectL<TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<Rgb16<0,38402,65535>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2200>>,Int<1500>,Int<5000>>,Int<-4000>>>,TrFade<1400>,AlphaL<Rgb16<0,11150,20996>,Bump<Int<0>,Int<10000>>>,TrFade<1500>,AlphaL<Rgb16<20393,20393,20393>,Bump<Int<0>,Int<7000>>>,TrFade<2100>>,EFFECT_RETRACTION>,
      //Retraction cool down effect
    TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_BOOT>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_NEWFONT>,
      //Optional/alternate passive battery monitor, on boot (1st line) or font change (2nd line) you will get a visual indicator at the emitter of your current battery level. This also works without a blade if you have a lit emitter or blade plug. Green is Full, Red is Low (the color will blend from Green to Red as the battery is depleted), the indicator will fade out after 3000 ms and not display again until powered down and back up or fonts change.
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<1000>>,EFFECT_BATTERY_LEVEL>,
      //On demand battery level, if using fett263's prop file Hold AUX and click PWR while OFF, the battery level is represented by the location on the blade; tip = full, hilt = low and color; green = full, yellow = half, red = low
    TransitionEffectL<TrConcat<TrFade<100>,AlphaL<Pulsing<RgbArg<PREON_COLOR_ARG,Rgb<120,120,165>>,Rgb<50,50,80>,500>,Bump<Int<0>,Int<6000>>>,TrBoing<600,9>>,EFFECT_PREON>
      //Clickity preon
    >>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "analog"},
*/

   { "ancient-tech;common",  "common/tracks/mandotheme.wav",
    // === Main Blade: ===
/* copyright Fett263 Greyscale (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#Greyscale
OS7.14 v2.242p
This Style Contains 6 Unique Combinations
Style Options:
Default (0): Crispity (Unstable Blade - AudioFlicker Swing)
1: Mercenary (Smoke Blade with Ripple Swing)
2: Apocalypse (Swing Speed - Split Blade)
3: CODA (Rolling Pulse with Unstable Swing)
4: Masterless (Rotoscope with Color Swing)
5: Decay (Inverted Rolling Pulse with Ripple Swing)

Base Color: BaseColorArg (0)

--Effects Included--
Ignition Effect: Standard Ignition [Color: IgnitionColorArg]
Retraction Effect: Standard Retraction [Color: RetractionColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Blast Effect: Blast Wave (Random) [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
*/
    //StylePtr<Layers<ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<400>>>,Layers<StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,3000>>,AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,1,10,2000,2>,Int<10000>>,AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black,800>>,SwingSpeed<375>>>,Layers<Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,40>>,AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>>,AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,SwingSpeed<525>>>,Layers<Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,SwingSpeed<500>>>>,TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<
        StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,
        AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<400>>>,Layers<
        StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Red>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Red>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Red>>,3000>>,
        AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,1,10,2000,2>,Int<10000>>,
        AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black,800>>,SwingSpeed<375>>>,Layers<
        Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Green>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Green>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Green>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Green>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,40>>,
        AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<
        Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Red>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>>,
        AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,SwingSpeed<525>>>,Layers<
        Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Red>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Red>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,
        AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Red>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<
        StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Red>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Red>,White>,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,
        AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Red>,Black>,Black,RgbArg<BASE_COLOR_ARG,Red>,Black>,SwingSpeed<500>>>>,
      TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>>>(),
    // === NPXL inner ring: ===
/* copyright Fett263 Greyscale (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#Greyscale
OS7.14 v2.242p
This Style Contains 21600 Unique Combinations
Style Options:
Default (0): Crispity (Unstable Blade - AudioFlicker Swing)
1: Mercenary (Smoke Blade with Ripple Swing)
2: Apocalypse (Swing Speed - Split Blade)
3: CODA (Rolling Pulse with Unstable Swing)
4: Masterless (Rotoscope with Color Swing)
5: Decay (Inverted Rolling Pulse with Ripple Swing)

Base Color: BaseColorArg (0)

--Effects Included--
Interactive Preon: Faulty Core [Color: PreonColorArg]
Interactive Preon Control = active Preon (auto times to sound), Clash to ignite. Requires/uses tr00.wav (glitchy ignition)
Note: Allow at least 30 seconds after initial boot for motion/clash detection to stabilize.
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Metal Forge (Heat Up), 3: Lightning Strike, 4: Wipe In, 5: Glitch On [Color: IgnitionColorArg]
PowerUp Effect Options: Default (0): Power Flash, 1: Power Burst Center Out, 2: Power Surge (Stable), 3: Power Surge (Unstable V1), 4: Power Surge (Unstable V2) [Color: IgnitionColorArg]
Retraction Effect Options: Default (0): Standard Retraction, 1: SparkTip Retraction, 2: Metal Forge (Cool Down), 3: Wipe In + Fade, 4: Cycle Down, 5: Run Up [Color: RetractionColorArg]
CoolDown Effect Options: Default (0): Unstable Cool Down Reverse, 1: Unstable Cool Down Forward, 2: Power Burst Forward, 3: Power Burst Reverse, 4: Power Flash [Color: RetractionColorArg]
PostOff Effect Options: Default (0): Exit Hyperspace, 1: Emitter Glow (Emitter Size), 2: Emitter Spark (Preon Size), 3: Emitter Cool Off (Preon Size) [Color: PostOffColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Interactive Blast: Random Blasts - Default Level (Uses Style Option 2 for Editing): 1000 ms
Interactive Blast control requires blast.wav files in font. When Blast is triggered blast.wav will play, to deflect blast (blst.wav and visual effect) swing within 1000ms of blaster sound ending. Random number of blasts from 1 to multiple possible. Only one style per preset can contain.
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Medium), Responsive Blast Wave (Random), Responsive Blast Ripple Fade, Full Blade Blast Fade [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Rain Effect: Rain Spark [Color: EmitterColorArg]
Battery Level: % Blade (Green to Red)
Battery Monitor: Passive Battery Monitor (Retraction)
*/
    //StylePtr<Layers<ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<400>>>,Layers<StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,3000>>,AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,1,10,2000,2>,Int<10000>>,AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black,800>>,SwingSpeed<375>>>,Layers<Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,40>>,AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>>,AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,SwingSpeed<525>>>,Layers<Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,SwingSpeed<500>>>>,TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,Remap<CenterDistF<>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrInstant,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrFade<1200>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<1200>>,TrConcat<TrInstant,HumpFlickerL<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,40>,TrFade<1200>>>,EFFECT_IGNITION>,TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,-3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,-2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>>,EFFECT_RETRACTION>,SparkleL<RgbArg<EMITTER_COLOR_ARG,Rgb<255,255,255>>>,TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<200>,Int<200>,Int<400>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>,TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,TransitionEffectL<TrConcat<TrDelay<100>,AlphaL<White,Int<0>>,TrDoEffect<TrInstant,EFFECT_GAME_CHOICE>>,EFFECT_INTERACTIVE_BLAST>,TransitionEffectL<TrConcat<TrExtendX<Sum<WavLen<EFFECT_INTERACTIVE_BLAST>,IntArg<STYLE_OPTION2_ARG,1000>>,TrInstant>,TransitionPulseL<TrDoEffect<TrInstant,EFFECT_BLAST>,ThresholdPulseF<SwingSpeed<300>,Int<16000>>>,TrConcat<TrDelay<150>,TrRandom<TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrInstant,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>>>>,EFFECT_GAME_CHOICE>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrWipeInX<Mult<IgnitionTime<300>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Black>,TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<19660>>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<19660>>>>,TrWipeInX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrJoin<TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrSmoothFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrColorCycleX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,Layers<Black,TransitionEffectL<TrConcat<TrDoEffectAlways<TrInstant,EFFECT_TRANSITION_SOUND,0>,Layers<TransitionPulseL<TrDoEffectAlways<TrInstant,EFFECT_FAST_ON>,EffectPulseF<EFFECT_OFF_CLASH>>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<100>>,SmoothStep<Scale<NoisySoundLevel,Int<1000>,Int<8000>>,Int<-3000>>>>,TrDelayX<WavLen<EFFECT_TRANSITION_SOUND>>>,EFFECT_INTERACTIVE_PREON>>>,TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrConcat<TrJoin<TrDelayX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,TrCenterWipeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,Remap<CenterDistF<>,Stripes<2000,-1500,Black,Rgb<100,100,150>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,Remap<CenterDistF<>,Stripes<1000,-500,Black,Rgb<100,100,150>,Black>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<White,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Orange,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Red,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-2000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>>>,EFFECT_POSTOFF>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>>>(),
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
/* commented out to save space
    StylePtr<Layers< //Same as above but "Layerized"
      ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<
        StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,
        AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<400>>>,Layers<
        StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Red>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Red>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Red>>,3000>>,
        AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,1,10,2000,2>,Int<10000>>,
        AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black,800>>,SwingSpeed<375>>>,Layers<
        Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Green>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Green>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Green>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Green>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,40>>,
        AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<
        Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Red>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>>,
        AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,SwingSpeed<525>>>,Layers<
        Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Red>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Red>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,
        AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Red>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<
        StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Red>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Red>,White>,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,
        AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Red>,Black>,Black,RgbArg<BASE_COLOR_ARG,Red>,Black>,SwingSpeed<500>>>>,
      TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,Remap<CenterDistF<>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrInstant,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,White>>,TrFade<1200>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<1200>>,TrConcat<TrInstant,HumpFlickerL<RgbArg<IGNITION_COLOR_ARG,White>,40>,TrFade<1200>>>,EFFECT_IGNITION>,
      TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,-3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,-2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,White>,TrFade<1000>>>,EFFECT_RETRACTION>,
      SparkleL<RgbArg<EMITTER_COLOR_ARG,White>>,
      TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Int<200>,Int<200>,Int<400>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>,TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,White>,TrFade<300>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      TransitionEffectL<TrConcat<TrDelay<100>,AlphaL<White,Int<0>>,TrDoEffect<TrInstant,EFFECT_GAME_CHOICE>>,EFFECT_INTERACTIVE_BLAST>,
      TransitionEffectL<TrConcat<TrExtendX<Sum<WavLen<EFFECT_INTERACTIVE_BLAST>,IntArg<STYLE_OPTION2_ARG,1000>>,TrInstant>,TransitionPulseL<TrDoEffect<TrInstant,EFFECT_BLAST>,ThresholdPulseF<SwingSpeed<300>,Int<16000>>>,TrConcat<TrDelay<150>,TrRandom<TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrInstant,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>>>>,EFFECT_GAME_CHOICE>,
      InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrWipeInX<Mult<IgnitionTime<>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,White>>,TrJoin<TrDelayX<Mult<IgnitionTime<>,Int<19660>>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,TrWipeX<Mult<IgnitionTime<>,Int<19660>>>>,TrWipeInX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,TrWipeX<Mult<IgnitionTime<>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,White>,BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrJoin<TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrSmoothFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrColorCycleX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,Layers<
        Black,
        TransitionEffectL<TrConcat<TrDoEffectAlways<TrInstant,EFFECT_TRANSITION_SOUND,0>,Layers<
          TransitionPulseL<TrDoEffectAlways<TrInstant,EFFECT_FAST_ON>,EffectPulseF<EFFECT_OFF_CLASH>>,
          AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<100>>,SmoothStep<Scale<NoisySoundLevel,Int<1000>,Int<8000>>,Int<-3000>>>>,TrDelayX<WavLen<EFFECT_TRANSITION_SOUND>>>,EFFECT_INTERACTIVE_PREON>>>,
      TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrConcat<TrJoin<TrDelayX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,TrCenterWipeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,Remap<CenterDistF<>,Stripes<2000,-1500,Black,Rgb<100,100,150>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,Remap<CenterDistF<>,Stripes<1000,-500,Black,Rgb<100,100,150>,Black>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,White>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<White,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Orange,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Red,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-2000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>>>,EFFECT_POSTOFF>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>>>(),
*/
    // === NPXL outer ring: ===
/* copyright Fett263 Greyscale (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#Greyscale
OS7.14 v2.242p
This Style Contains 21600 Unique Combinations
Style Options:
Default (0): Crispity (Unstable Blade - AudioFlicker Swing)
1: Mercenary (Smoke Blade with Ripple Swing)
2: Apocalypse (Swing Speed - Split Blade)
3: CODA (Rolling Pulse with Unstable Swing)
4: Masterless (Rotoscope with Color Swing)
5: Decay (Inverted Rolling Pulse with Ripple Swing)

Base Color: BaseColorArg (0)

--Effects Included--
Interactive Preon: Faulty Core [Color: PreonColorArg]
Interactive Preon Control = active Preon (auto times to sound), Clash to ignite. Requires/uses tr00.wav (glitchy ignition)
Note: Allow at least 30 seconds after initial boot for motion/clash detection to stabilize.
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Metal Forge (Heat Up), 3: Lightning Strike, 4: Wipe In, 5: Glitch On [Color: IgnitionColorArg]
PowerUp Effect Options: Default (0): Power Flash, 1: Power Burst Center Out, 2: Power Surge (Stable), 3: Power Surge (Unstable V1), 4: Power Surge (Unstable V2) [Color: IgnitionColorArg]
Retraction Effect Options: Default (0): Standard Retraction, 1: SparkTip Retraction, 2: Metal Forge (Cool Down), 3: Wipe In + Fade, 4: Cycle Down, 5: Run Up [Color: RetractionColorArg]
CoolDown Effect Options: Default (0): Unstable Cool Down Reverse, 1: Unstable Cool Down Forward, 2: Power Burst Forward, 3: Power Burst Reverse, 4: Power Flash [Color: RetractionColorArg]
PostOff Effect Options: Default (0): Exit Hyperspace, 1: Emitter Glow (Emitter Size), 2: Emitter Spark (Preon Size), 3: Emitter Cool Off (Preon Size) [Color: PostOffColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Interactive Blast: Random Blasts - Default Level (Uses Style Option 2 for Editing): 1000 ms
Interactive Blast control requires blast.wav files in font. When Blast is triggered blast.wav will play, to deflect blast (blst.wav and visual effect) swing within 1000ms of blaster sound ending. Random number of blasts from 1 to multiple possible. Only one style per preset can contain.
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Medium), Responsive Blast Wave (Random), Responsive Blast Ripple Fade, Full Blade Blast Fade [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Rain Effect: Rain Spark [Color: EmitterColorArg]
Battery Level: % Blade (Green to Red)
Battery Monitor: Passive Battery Monitor (Retraction)
*/
    //StylePtr<Layers<ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<400>>>,Layers<StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,3000>>,AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,1,10,2000,2>,Int<10000>>,AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black,800>>,SwingSpeed<375>>>,Layers<Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,40>>,AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>>,AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,SwingSpeed<525>>>,Layers<Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,SwingSpeed<500>>>>,TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,Remap<CenterDistF<>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrInstant,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrFade<1200>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<1200>>,TrConcat<TrInstant,HumpFlickerL<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,40>,TrFade<1200>>>,EFFECT_IGNITION>,TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,-3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,-2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>>,EFFECT_RETRACTION>,SparkleL<RgbArg<EMITTER_COLOR_ARG,Rgb<255,255,255>>>,TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<200>,Int<200>,Int<400>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>,TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,TransitionEffectL<TrConcat<TrDelay<100>,AlphaL<White,Int<0>>,TrDoEffect<TrInstant,EFFECT_GAME_CHOICE>>,EFFECT_INTERACTIVE_BLAST>,TransitionEffectL<TrConcat<TrExtendX<Sum<WavLen<EFFECT_INTERACTIVE_BLAST>,IntArg<STYLE_OPTION2_ARG,1000>>,TrInstant>,TransitionPulseL<TrDoEffect<TrInstant,EFFECT_BLAST>,ThresholdPulseF<SwingSpeed<300>,Int<16000>>>,TrConcat<TrDelay<150>,TrRandom<TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrInstant,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>>>>,EFFECT_GAME_CHOICE>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrWipeInX<Mult<IgnitionTime<300>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Black>,TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<19660>>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<19660>>>>,TrWipeInX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrJoin<TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrSmoothFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrColorCycleX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,Layers<Black,TransitionEffectL<TrConcat<TrDoEffectAlways<TrInstant,EFFECT_TRANSITION_SOUND,0>,Layers<TransitionPulseL<TrDoEffectAlways<TrInstant,EFFECT_FAST_ON>,EffectPulseF<EFFECT_OFF_CLASH>>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<100>>,SmoothStep<Scale<NoisySoundLevel,Int<1000>,Int<8000>>,Int<-3000>>>>,TrDelayX<WavLen<EFFECT_TRANSITION_SOUND>>>,EFFECT_INTERACTIVE_PREON>>>,TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrConcat<TrJoin<TrDelayX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,TrCenterWipeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,Remap<CenterDistF<>,Stripes<2000,-1500,Black,Rgb<100,100,150>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,Remap<CenterDistF<>,Stripes<1000,-500,Black,Rgb<100,100,150>,Black>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<White,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Orange,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Red,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-2000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>>>,EFFECT_POSTOFF>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>>>(),
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
/* commented out to save space
    StylePtr<Layers< //Same as above but "Layerized"
      ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<
        StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,
        AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<400>>>,Layers<
        StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Red>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Red>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Red>>,3000>>,
        AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,1,10,2000,2>,Int<10000>>,
        AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black,800>>,SwingSpeed<375>>>,Layers<
        Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Green>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Green>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Green>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Green>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,40>>,
        AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<
        Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Red>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>>,
        AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,SwingSpeed<525>>>,Layers<
        Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Red>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Red>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,
        AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Red>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<
        StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Red>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Red>,White>,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,
        AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Red>,Black>,Black,RgbArg<BASE_COLOR_ARG,Red>,Black>,SwingSpeed<500>>>>,
      TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,Remap<CenterDistF<>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrInstant,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,White>>,TrFade<1200>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<1200>>,TrConcat<TrInstant,HumpFlickerL<RgbArg<IGNITION_COLOR_ARG,White>,40>,TrFade<1200>>>,EFFECT_IGNITION>,
      TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,-3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,-2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,White>,TrFade<1000>>>,EFFECT_RETRACTION>,
      SparkleL<RgbArg<EMITTER_COLOR_ARG,White>>,
      TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Int<200>,Int<200>,Int<400>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>,TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,White>,TrFade<300>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      TransitionEffectL<TrConcat<TrDelay<100>,AlphaL<White,Int<0>>,TrDoEffect<TrInstant,EFFECT_GAME_CHOICE>>,EFFECT_INTERACTIVE_BLAST>,
      TransitionEffectL<TrConcat<TrExtendX<Sum<WavLen<EFFECT_INTERACTIVE_BLAST>,IntArg<STYLE_OPTION2_ARG,1000>>,TrInstant>,TransitionPulseL<TrDoEffect<TrInstant,EFFECT_BLAST>,ThresholdPulseF<SwingSpeed<300>,Int<16000>>>,TrConcat<TrDelay<150>,TrRandom<TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrInstant,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>>>>,EFFECT_GAME_CHOICE>,
      InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrWipeInX<Mult<IgnitionTime<>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,White>>,TrJoin<TrDelayX<Mult<IgnitionTime<>,Int<19660>>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,TrWipeX<Mult<IgnitionTime<>,Int<19660>>>>,TrWipeInX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,TrWipeX<Mult<IgnitionTime<>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,White>,BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrJoin<TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrSmoothFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrColorCycleX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,Layers<
        Black,
        TransitionEffectL<TrConcat<TrDoEffectAlways<TrInstant,EFFECT_TRANSITION_SOUND,0>,Layers<
          TransitionPulseL<TrDoEffectAlways<TrInstant,EFFECT_FAST_ON>,EffectPulseF<EFFECT_OFF_CLASH>>,
          AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<100>>,SmoothStep<Scale<NoisySoundLevel,Int<1000>,Int<8000>>,Int<-3000>>>>,TrDelayX<WavLen<EFFECT_TRANSITION_SOUND>>>,EFFECT_INTERACTIVE_PREON>>>,
      TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrConcat<TrJoin<TrDelayX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,TrCenterWipeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,Remap<CenterDistF<>,Stripes<2000,-1500,Black,Rgb<100,100,150>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,Remap<CenterDistF<>,Stripes<1000,-500,Black,Rgb<100,100,150>,Black>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,White>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<White,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Orange,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Red,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-2000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>>>,EFFECT_POSTOFF>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>>>(),
*/
    // === Crystal Chamber: ===
/* copyright Fett263 Greyscale (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#Greyscale
OS7.14 v2.242p
This Style Contains 21600 Unique Combinations
Style Options:
Default (0): Crispity (Unstable Blade - AudioFlicker Swing)
1: Mercenary (Smoke Blade with Ripple Swing)
2: Apocalypse (Swing Speed - Split Blade)
3: CODA (Rolling Pulse with Unstable Swing)
4: Masterless (Rotoscope with Color Swing)
5: Decay (Inverted Rolling Pulse with Ripple Swing)

Base Color: BaseColorArg (0)

--Effects Included--
Interactive Preon: Faulty Core [Color: PreonColorArg]
Interactive Preon Control = active Preon (auto times to sound), Clash to ignite. Requires/uses tr00.wav (glitchy ignition)
Note: Allow at least 30 seconds after initial boot for motion/clash detection to stabilize.
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Metal Forge (Heat Up), 3: Lightning Strike, 4: Wipe In, 5: Glitch On [Color: IgnitionColorArg]
PowerUp Effect Options: Default (0): Power Flash, 1: Power Burst Center Out, 2: Power Surge (Stable), 3: Power Surge (Unstable V1), 4: Power Surge (Unstable V2) [Color: IgnitionColorArg]
Retraction Effect Options: Default (0): Standard Retraction, 1: SparkTip Retraction, 2: Metal Forge (Cool Down), 3: Wipe In + Fade, 4: Cycle Down, 5: Run Up [Color: RetractionColorArg]
CoolDown Effect Options: Default (0): Unstable Cool Down Reverse, 1: Unstable Cool Down Forward, 2: Power Burst Forward, 3: Power Burst Reverse, 4: Power Flash [Color: RetractionColorArg]
PostOff Effect Options: Default (0): Exit Hyperspace, 1: Emitter Glow (Emitter Size), 2: Emitter Spark (Preon Size), 3: Emitter Cool Off (Preon Size) [Color: PostOffColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Interactive Blast: Random Blasts - Default Level (Uses Style Option 2 for Editing): 1000 ms
Interactive Blast control requires blast.wav files in font. When Blast is triggered blast.wav will play, to deflect blast (blst.wav and visual effect) swing within 1000ms of blaster sound ending. Random number of blasts from 1 to multiple possible. Only one style per preset can contain.
Blast Effect (Randomly Selected): Blast Wave (Random), Blast Ripple Fade, Blast Wave (Medium), Responsive Blast Wave (Random), Responsive Blast Ripple Fade, Full Blade Blast Fade [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Rain Effect: Rain Spark [Color: EmitterColorArg]
Battery Level: % Blade (Green to Red)
Battery Monitor: Passive Battery Monitor (Retraction)
*/
    //StylePtr<Layers<ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<400>>>,Layers<StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,3000>>,AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,1,10,2000,2>,Int<10000>>,AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black,800>>,SwingSpeed<375>>>,Layers<Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,40>>,AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>>,AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Black>,SwingSpeed<525>>>,Layers<Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,White>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>>,AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Black>,SwingSpeed<500>>>>,TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,Remap<CenterDistF<>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrInstant,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrFade<1200>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<1200>>,TrConcat<TrInstant,HumpFlickerL<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,40>,TrFade<1200>>>,EFFECT_IGNITION>,TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,-3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,-2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>>,EFFECT_RETRACTION>,SparkleL<RgbArg<EMITTER_COLOR_ARG,Rgb<255,255,255>>>,TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Int<200>,Int<200>,Int<400>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>,TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,TrFade<300>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,TransitionEffectL<TrConcat<TrDelay<100>,AlphaL<White,Int<0>>,TrDoEffect<TrInstant,EFFECT_GAME_CHOICE>>,EFFECT_INTERACTIVE_BLAST>,TransitionEffectL<TrConcat<TrExtendX<Sum<WavLen<EFFECT_INTERACTIVE_BLAST>,IntArg<STYLE_OPTION2_ARG,1000>>,TrInstant>,TransitionPulseL<TrDoEffect<TrInstant,EFFECT_BLAST>,ThresholdPulseF<SwingSpeed<300>,Int<16000>>>,TrConcat<TrDelay<150>,TrRandom<TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrInstant,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>>>>,EFFECT_GAME_CHOICE>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrWipeInX<Mult<IgnitionTime<300>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Black>,TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<19660>>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<19660>>>>,TrWipeInX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrJoin<TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrSmoothFadeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrColorCycleX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,Layers<Black,TransitionEffectL<TrConcat<TrDoEffectAlways<TrInstant,EFFECT_TRANSITION_SOUND,0>,Layers<TransitionPulseL<TrDoEffectAlways<TrInstant,EFFECT_FAST_ON>,EffectPulseF<EFFECT_OFF_CLASH>>,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<100>>,SmoothStep<Scale<NoisySoundLevel,Int<1000>,Int<8000>>,Int<-3000>>>>,TrDelayX<WavLen<EFFECT_TRANSITION_SOUND>>>,EFFECT_INTERACTIVE_PREON>>>,TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrConcat<TrJoin<TrDelayX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,TrCenterWipeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,Remap<CenterDistF<>,Stripes<2000,-1500,Black,Rgb<100,100,150>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,Remap<CenterDistF<>,Stripes<1000,-500,Black,Rgb<100,100,150>,Black>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<White,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Orange,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Red,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-2000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>>>,EFFECT_POSTOFF>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>>>(),
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
/* commented out to save space
    StylePtr<Layers< //Same as above but "Layerized"
      ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,Layers<
        StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<6000>,Black,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<10772>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,
        AlphaL<AudioFlickerL<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<400>>>,Layers<
        StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<1265>,Black,RgbArg<BASE_COLOR_ARG,Red>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Red>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Red>>,3000>>,
        AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,1,10,2000,2>,Int<10000>>,
        AlphaL<Stripes<2500,-3000,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<5654>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Pulsing<Mix<Int<2322>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Black,800>>,SwingSpeed<375>>>,Layers<
        Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<8167>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<4032>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,StaticFire<BrownNoiseFlicker<RgbArg<ALT_COLOR_ARG,Green>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<ALT_COLOR_ARG,Green>>,Mix<Int<8167>,Black,RgbArg<ALT_COLOR_ARG,Green>>>,300>,Mix<Int<4032>,Black,RgbArg<ALT_COLOR_ARG,Green>>,0,6,10,1000,2>>,HumpFlicker<RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>,40>>,
        AlphaL<AlphaL<AudioFlickerL<RotateColorsX<Int<800>,RgbArg<BASE_COLOR_ARG,Red>>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>>,Layers<
        Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,Red>,Stripes<5000,-90,Mix<Int<21200>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<16384>,Black,RotateColorsX<Int<30000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<12632>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,AudioFlicker<RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>,Stripes<5000,-90,Mix<Int<21672>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<31000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<26736>,Black,RotateColorsX<Int<28000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Int<21000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>>,
        AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,Black,RotateColorsX<Int<31600>,RgbArg<BASE_COLOR_ARG,Red>>,Black>,SwingSpeed<525>>>,Layers<
        Stripes<15000,-300,RgbArg<BASE_COLOR_ARG,Red>,Pulsing<Mix<Int<3855>,Mix<Int<21845>,Black,RgbArg<BASE_COLOR_ARG,Red>>,White>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>,800>,Mix<Int<23130>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,
        AlphaL<RotateColorsX<Int<28600>,RgbArg<BASE_COLOR_ARG,Red>>,Scale<IsLessThan<SwingSpeed<300>,Int<13600>>,Scale<SwingSpeed<300>,Int<-19300>,Int<32768>>,Int<0>>>>,Layers<
        StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Red>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Red>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Red>,White>,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Red>>>>,
        AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Red>,Black>,Black,RgbArg<BASE_COLOR_ARG,Red>,Black>,SwingSpeed<500>>>>,
      TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,Remap<CenterDistF<>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrInstant,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,White>>,TrFade<1200>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<1200>>,TrConcat<TrInstant,HumpFlickerL<RgbArg<IGNITION_COLOR_ARG,White>,40>,TrFade<1200>>>,EFFECT_IGNITION>,
      TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,-3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,-2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,White>,TrFade<1000>>>,EFFECT_RETRACTION>,
      SparkleL<RgbArg<EMITTER_COLOR_ARG,White>>,
      TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<3000>,Int<29000>>>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Int<200>,Int<200>,Int<400>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>,TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,White>,TrFade<300>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      TransitionEffectL<TrConcat<TrDelay<100>,AlphaL<White,Int<0>>,TrDoEffect<TrInstant,EFFECT_GAME_CHOICE>>,EFFECT_INTERACTIVE_BLAST>,
      TransitionEffectL<TrConcat<TrExtendX<Sum<WavLen<EFFECT_INTERACTIVE_BLAST>,IntArg<STYLE_OPTION2_ARG,1000>>,TrInstant>,TransitionPulseL<TrDoEffect<TrInstant,EFFECT_BLAST>,ThresholdPulseF<SwingSpeed<300>,Int<16000>>>,TrConcat<TrDelay<150>,TrRandom<TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrInstant,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>,TrDoEffect<TrInstant,EFFECT_INTERACTIVE_BLAST>>>>,EFFECT_GAME_CHOICE>,
      InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>>,TrConcat<TrWipeInX<Mult<IgnitionTime<>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,White>>,TrJoin<TrDelayX<Mult<IgnitionTime<>,Int<19660>>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,TrWipeX<Mult<IgnitionTime<>,Int<19660>>>>,TrWipeInX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,TrWipeX<Mult<IgnitionTime<>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,White>,BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrConcat<TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,White,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,DarkOrange,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Red,TrFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrJoin<TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrSmoothFadeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,TrColorCycleX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,Layers<
        Black,
        TransitionEffectL<TrConcat<TrDoEffectAlways<TrInstant,EFFECT_TRANSITION_SOUND,0>,Layers<
          TransitionPulseL<TrDoEffectAlways<TrInstant,EFFECT_FAST_ON>,EffectPulseF<EFFECT_OFF_CLASH>>,
          AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<100>>,SmoothStep<Scale<NoisySoundLevel,Int<1000>,Int<8000>>,Int<-3000>>>>,TrDelayX<WavLen<EFFECT_TRANSITION_SOUND>>>,EFFECT_INTERACTIVE_PREON>>>,
      TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrConcat<TrJoin<TrDelayX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,TrCenterWipeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,Remap<CenterDistF<>,Stripes<2000,-1500,Black,Rgb<100,100,150>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>,Remap<CenterDistF<>,Stripes<1000,-500,Black,Rgb<100,100,150>,Black>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<8192>>>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,White>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,TrConcat<TrInstant,AlphaL<White,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Orange,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-4000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>,AlphaL<Red,SmoothStep<IntArg<PREON_SIZE_ARG,2000>,Int<-2000>>>,TrFadeX<Mult<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>,Int<10923>>>>>,EFFECT_POSTOFF>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>>>(),
*/
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "ancient\n -tech"},

   { "angry-monkey;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
/* copyright Fett263 BlankBlade (Primary Blade) OS6 Style
https://fett263.com/fett263-proffieOS6-style-library.html#BlankBlade
OS6.5 v2.2
Single Style
Base Style: Blank Blade

--Effects Included--
Ignition Effect: Lightning Strike
Retraction Effect: Run Up
Lockup Effect: Intensity Lockup V1
LightningBlock Effect: Responsive Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Blast Effect: Responsive Blast Wave (Large)
Clash Effect: Random Clash with Wave
Swing Effect: Interactive Fireball (Force Toggle)
Emitter Effect: Unstable Flare
*/
    //StylePtr<Layers<Black,AlphaL<RandomPerLEDFlickerL<RgbArg<EMITTER_COLOR_ARG,Rgb<255,68,0>>>,SmoothStep<Scale<NoisySoundLevel,IntArg<EMITTER_SIZE_ARG,2000>,Sum<IntArg<EMITTER_SIZE_ARG,2000>,Int<6000>>>,Int<-6000>>>,ColorSelect<IncrementF<EffectPulseF<EFFECT_FORCE>,Int<32000>,Int<2>,Int<1>>,TrSmoothFade<500>,AlphaL<RgbArg<BASE_COLOR_ARG,Rgb<255,68,0>>,Int<0>>,AlphaL<AlphaMixL<SmoothStep<IncrementWithReset<Scale<IsGreaterThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<18000>>,Int<0>,ThresholdPulseF<StrobeF<Scale<HoldPeakF<Ifon<SwingAcceleration<>,Int<0>>,Int<150>,Int<32000>>,Int<20>,Int<200>>,Int<1>>,Int<32000>>>,ThresholdPulseF<IsLessThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<8000>>,Int<32000>>,Int<42000>,Int<1500>>,Scale<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<3000>,Int<10000>>>,HumpFlicker<RotateColorsX<Int<1400>,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,RotateColorsX<Int<3200>,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,20>,HumpFlicker<RotateColorsX<Int<1000>,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,RotateColorsX<Int<400>,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,60>,RandomFlicker<Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<SWING_COLOR_ARG,Rgb<255,0,0>>>>,LinearSectionF<IncrementWithReset<Scale<IsGreaterThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<18000>>,Int<0>,ThresholdPulseF<StrobeF<Scale<HoldPeakF<Ifon<SwingAcceleration<>,Int<0>>,Int<150>,Int<32000>>,Int<20>,Int<200>>,Int<1>>,Int<32000>>>,ThresholdPulseF<IsLessThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<8000>>,Int<32000>>,Int<42000>,Int<1500>>,Scale<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<3000>,Int<10000>>>>>,MultiTransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,187,108>>,Int<200>,Int<100>,Int<400>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,187,108>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<ClashImpactF<>,Int<28000>,Int<8000>>>,EFFECT_CLASH>,LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,187,108>>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,187,108>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,187,108>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,187,108>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,187,108>>,Int<16000>>,30>,TrSmoothFade<600>>>,LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,187,108>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,187,108>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>,Mix<TwistAngle<>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>,RotateColorsX<Int<3000>,Mix<Int<12000>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrExtend<4000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,InOutTrL<TrConcat<TrWipeInX<Mult<IgnitionTime<300>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Black>,TrExtendX<Mult<IgnitionTime<300>,Int<19660>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<19660>>>>,TrWipeX<RetractionTime<0>>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Black,
      AlphaL<RandomPerLEDFlickerL<RgbArg<EMITTER_COLOR_ARG,DarkOrange>>,SmoothStep<Scale<NoisySoundLevel,IntArg<EMITTER_SIZE_ARG,2000>,Sum<IntArg<EMITTER_SIZE_ARG,2000>,Int<6000>>>,Int<-6000>>>,
      ColorSelect<IncrementF<EffectPulseF<EFFECT_FORCE>,Int<32000>,Int<2>>,TrSmoothFade<500>,AlphaL<RgbArg<BASE_COLOR_ARG,DarkOrange>,Int<0>>,AlphaL<AlphaMixL<SmoothStep<IncrementWithReset<Scale<IsGreaterThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<18000>>,Int<0>,ThresholdPulseF<StrobeF<Scale<HoldPeakF<Ifon<SwingAcceleration<>,Int<0>>,Int<150>,Int<32000>>,Int<20>,Int<200>>,Int<1>>,Int<32000>>>,ThresholdPulseF<IsLessThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<8000>>,Int<32000>>,Int<42000>,Int<1500>>,Scale<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<3000>,Int<10000>>>,HumpFlicker<RotateColorsX<Int<1400>,RgbArg<SWING_COLOR_ARG,Red>>,RotateColorsX<Int<3200>,RgbArg<SWING_COLOR_ARG,Red>>,20>,HumpFlicker<RotateColorsX<Int<1000>,RgbArg<SWING_COLOR_ARG,Red>>,RotateColorsX<Int<400>,RgbArg<SWING_COLOR_ARG,Red>>,60>,RandomFlicker<Mix<Int<16384>,Black,RgbArg<SWING_COLOR_ARG,Red>>,RgbArg<SWING_COLOR_ARG,Red>>>,LinearSectionF<IncrementWithReset<Scale<IsGreaterThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<18000>>,Int<0>,ThresholdPulseF<StrobeF<Scale<HoldPeakF<Ifon<SwingAcceleration<>,Int<0>>,Int<150>,Int<32000>>,Int<20>,Int<200>>,Int<1>>,Int<32000>>>,ThresholdPulseF<IsLessThan<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<8000>>,Int<32000>>,Int<42000>,Int<1500>>,Scale<HoldPeakF<Ifon<SwingSpeed<400>,Int<0>>,Int<150>,Int<32000>>,Int<3000>,Int<10000>>>>>,
      MultiTransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,NavajoWhite>,Int<200>,Int<100>,Int<400>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,
      TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,NavajoWhite>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<ClashImpactF<>,Int<28000>,Int<8000>>>,EFFECT_CLASH>,
      LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>>,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>>>>,TrConcat<TrExtend<50,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrExtend<3000,TrFade<300>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrFade<3000>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,Int<0>>,TrSelect<Scale<IsLessThan<ClashImpactF<>,Int<20000>>,Int<1>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,Scale<ClashImpactF<>,Int<50>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<500>,Int<300>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,TrSparkX<Remap<CenterDistF<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,Stripes<1200,-3600,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>>,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,NavajoWhite>>>>,Int<30>,Scale<Sum<ClashImpactF<>,SwingSpeed<600>>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,NavajoWhite>,AudioFlicker<RgbArg<LB_COLOR_ARG,NavajoWhite>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,NavajoWhite>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,NavajoWhite>,Int<16000>>,30>,TrSmoothFade<600>>>,
      LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,NavajoWhite>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,NavajoWhite>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,26000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,MoltenCore>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,MoltenCore>>>,Mix<TwistAngle<>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,MoltenCore>>,RotateColorsX<Int<3000>,Mix<Int<12000>,Black,RgbArg<STAB_COLOR_ARG,MoltenCore>>>>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<RgbArg<STAB_COLOR_ARG,MoltenCore>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrExtend<4000,TrFade<300>>,AlphaL<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,MoltenCore>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,MoltenCore>>>,SmoothStep<IntArg<MELT_SIZE_ARG,26000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrConcat<TrWipeInX<Mult<IgnitionTime<>,Int<6553>>>,RandomBlink<30000,RgbArg<IGNITION_COLOR_ARG,White>>,TrExtendX<Mult<IgnitionTime<>,Int<19660>>,TrWipeInX<Mult<IgnitionTime<230>,Int<6553>>>>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,IntArg<EMITTER_SIZE_ARG,1000>,Sum<Int<5000>,IntArg<EMITTER_SIZE_ARG,1000>>>,Int<-3000>>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,TrWipeX<Mult<IgnitionTime<>,Int<19660>>>>,TrWipeX<RetractionTime<>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "angry\nmonkey"},

   { "apocalypse;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // Argified from olicomplex1.6.00.h
    Mix<SmoothStep<Scale<SwingSpeed<400>,Int<6000>,Int<24000>>,Int<10000>>,Mix<Sin<Int<30>>,StyleFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Rgb<128,0,0>,Rgb<60,0,0>>,300>,Rgb<30,0,0>,0,6,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>>,StyleFire<BrownNoiseFlicker<OrangeRed,RandomPerLEDFlicker<Rgb<128,25,0>,Rgb<60,15,0>>,300>,Rgb<30,2,0>,0,6,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>>>,HumpFlicker<Red,Rgb<125,0,0>,40>>,
      //Swing responsive humpflicker/rotating red/orange unstable base, unstable takes over the blade the harder you swing
      //HumpFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>,50>,
      //Alternate plain humpflicker
    AlphaL<AlphaL<AudioFlickerL<Rgb16<65535,8729,0>>,SwingSpeed<350>>,SmoothStep<Scale<SwingSpeed<450>,Int<29000>,Int<9500>>,Int<16000>>>,
      //Responsive edge audioflicker swing - starts at the tip and takes over the blade the harder you swing
    LockupTrL<Layers<
    AlphaL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,150,0>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
    AlphaL<Moccasin,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<500>>,TrConcat<TrInstant,White,TrFade<600>,StyleFire<BrownNoiseFlicker<OrangeRed,RandomPerLEDFlicker<Rgb<128,25,0>,Rgb<60,15,0>>,300>,Rgb<30,2,0>,0,6,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>>,TrFade<50>,Rgb16<65535,8729,0>,TrFade<525>,StyleFire<BrownNoiseFlicker<OrangeRed,RandomPerLEDFlicker<Rgb<128,25,0>,Rgb<60,15,0>>,300>,Rgb<30,2,0>,0,6,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>>,TrFade<50>,Rgb16<35913,7294,0>,TrFade<250>,StyleFire<BrownNoiseFlicker<OrangeRed,RandomPerLEDFlicker<Rgb<128,25,0>,Rgb<60,15,0>>,300>,Rgb<30,2,0>,0,6,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>,FireConfig<10,1000,2>>,TrFade<100>,Rgb16<20393,1818,0>,TrBoing<950,3>>,SaberBase::LOCKUP_NORMAL>,
      //Responsive lockup with unstable cooldown
      //AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrJoinR<TrWipe<100>,TrWipeIn<100>>,White,TrFade<3000>>,SaberBase::LOCKUP_NORMAL>,
      //Alternate normal endlock effect
    ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      //Responsive lightning block
    ResponsiveStabL<RgbArg<STAB_COLOR_ARG,Orange>>,
      //Responsive stab
    EffectSequence<EFFECT_BLAST,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,LocalizedClashL<RgbArg<BLAST_COLOR_ARG,White>,80,30,EFFECT_BLAST>,ResponsiveBlastWaveL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<500>,Int<200>>,Scale<SwingSpeed<400>,Int<100>,Int<400>>>,BlastL<RgbArg<BLAST_COLOR_ARG,White>,200,200>,ResponsiveBlastFadeL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<6000>,Int<12000>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<400>,Int<100>>,Scale<SwingSpeed<400>,Int<200>,Int<100>>,Scale<SwingSpeed<400>,Int<400>,Int<200>>>>,
      //Multi-blast, blaster reflect cycles through different responsive effects
      //ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      //Alternate responsive blast
    ResponsiveClashL<TransitionEffect<RgbArg<CLASH_COLOR_ARG,Rgb<255,150,0>>,Moccasin,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Responsive Clash
      //ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Alternate white clash
    TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,Orange,RandomPerLEDFlicker<Rgb<70,25,5>,Black>,BrownNoiseFlicker<OrangeRed,Rgb<75,20,5>,200>,RandomPerLEDFlicker<Rgb<155,25,0>,Rgb<65,15,0>>>,TrFade<2500>,Stripes<3000,-2250,DarkOrange,RandomPerLEDFlicker<Rgb<195,15,0>,Rgb<50,15,0>>,BrownNoiseFlicker<OrangeRed,Rgb<85,15,0>,200>,RandomPerLEDFlicker<Rgb<155,25,0>,Rgb<95,15,0>>>,TrFade<1000>,Stripes<3000,-1500,Rgb16<65535,8729,0>,RandomPerLEDFlicker<Rgb<195,10,0>,Rgb<55,5,0>>,BrownNoiseFlicker<OrangeRed,Rgb<185,10,0>,200>,RandomPerLEDFlicker<Rgb<255,13,0>,Rgb<105,3,0>>>,TrFade<2500>>,EFFECT_IGNITION>,
      //Unstable ignition
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<Rgb<255,75,0>,40>,TrFade<500>>,EFFECT_RETRACTION>,
      //Humpflicker retraction
    LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      //Drag
    LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      //Responsive melt
    EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<8192>>,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<24576>>,AlphaL<Black,Int<0>>>,
      //Power save, if using fett263's prop file hold Aux and click PWR while ON (pointing up) to dim blade in 25% increments.
    InOutTrL<TrWipeSparkTip<White,175,250>,TrColorCycle<500>>,
      //Spark tip ignition, cycle down retraction
    TransitionEffectL<TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<White,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,Int<500>,Int<5500>>,Int<-4000>>>,TrFade<1100>>,EFFECT_RETRACTION>,
      //White flicker retraction effect
    TransitionEffectL<TrConcat<TrInstant,AlphaL<White,Bump<Int<0>,Int<10000>>>,TrFade<1600>,AlphaL<Orange,Bump<Int<0>,Int<8000>>>,TrFade<1400>,AlphaL<Red,Bump<Int<0>,Int<6000>>>,TrFade<1000>>,EFFECT_RETRACTION>,
      //Emitter cool off retraction effect
      //TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_BOOT>,
      //TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_NEWFONT>,
      //Optional/alternate passive battery monitor, on boot (1st line) or font change (2nd line) you will get a visual indicator at the emitter of your current battery level. This also works without a blade if you have a lit emitter or blade plug. Green is Full, Red is Low (the color will blend from Green to Red as the battery is depleted), the indicator will fade out after 3000 ms and not display again until powered down and back up or fonts change.
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<1000>>,EFFECT_BATTERY_LEVEL>,
      //On demand battery level, if using fett263's prop file Hold AUX and click PWR while OFF, the battery level is represented by the location on the blade; tip = full, hilt = low and color; green = full, yellow = half, red = low
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Int<3000>,Int<2000>,Int<1000>>,BrownNoiseFlicker<RgbArg<PREON_COLOR_ARG,Black>,Red,100>,RandomPerLEDFlicker<Red,Rgb16<14386,0,0>>,BrownNoiseFlicker<Mix<NoisySoundLevel,Red,RotateColorsX<Int<4000>,Red>>,DarkOrange,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-1000>,Int<25000>>,Int<-4000>>>,TrDelay<6500>>,EFFECT_PREON>,
      //Noise responsive preon
    TransitionEffectL<TrConcat<TrDelay<3000>,TransitionLoopL<TrWaveX<RgbArg<PREON_COLOR_ARG,Red>,Int<115>,Int<400>,Int<250>,Int<0>>>,TrDelay<1000>,TransitionLoopL<TrWaveX<Red,Int<215>,Int<300>,Int<395>,Int<0>>>,TrDelay<1500>,TransitionLoopL<TrWaveX<Red,Int<345>,Int<200>,Int<500>,Int<0>>>,TrDelay<1500>,AlphaL<Red,Bump<Int<0>,Int<6000>>>,TrBoing<500,2>>,EFFECT_PREON>,
      //Charge up, wave out preon
    TransitionEffectL<TrConcat<TrFade<1500>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Red>,10>,Bump<Int<0>,Int<4000>>>,TrFade<2000>,AlphaL<HumpFlickerL<Orange,15>,Bump<Int<0>,Int<5000>>>,TrFade<2500>,AlphaL<HumpFlickerL<White,20>,Bump<Int<0>,Int<6000>>>,TrBoing<500,2>>,EFFECT_PREON>
      //Emitter heat up preon
    >>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "apocalypse"},

   { "arcane;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Deceived"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Orange,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<1060>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Orange>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<1060>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers<Layers<Blue,RandomL<Black>>,InOutHelperL<InOutFuncX<Int<300>,Int<800>>,Pulsing<Red,Black,2000>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "arcane"},

   { "ashely;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Chosen_One"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Blue>,RotateColorsX<Variation,Rgb<0,0,128>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Blue>,RotateColorsX<Variation,Rgb<0,0,128>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Red>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,1>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "ashely"},

   { "assassin;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // Argified from olicomplex1.6.00.h
    HumpFlicker<RgbArg<BASE_COLOR_ARG,Green>,Rgb<0,128,0>,50>,
      //Green humpflicker
    AlphaL<Stripes<2500,-2750,Green,Rgb<25,60,0>,Pulsing<Rgb<0,30,0>,Black,800>>,SwingSpeed<375>>,
      //Swing effect, rippling green
    LockupTrL<Layers<
    AlphaL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,240,80>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
    AlphaL<LemonChiffon,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      //Responsive lockup
    ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      //Responsive lightning block
    AlphaL<Rgb16<21301,65535,0>,SmoothStep<Scale<SlowNoise<Int<2500>>,Int<1000>,Int<3000>>,Int<-4000>>>,
      //Sparking emitter flare
    ResponsiveStabL<RgbArg<STAB_COLOR_ARG,Red>>,
      //Responsive stab
    EffectSequence<EFFECT_BLAST,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,BlastF<200,200>>,TrFade<300>>,EFFECT_BLAST>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveBlastWaveL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<500>,Int<200>>,Scale<SwingSpeed<400>,Int<100>,Int<400>>>,ResponsiveBlastFadeL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<6000>,Int<12000>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<400>,Int<100>>,Scale<SwingSpeed<400>,Int<200>,Int<100>>,Scale<SwingSpeed<400>,Int<400>,Int<200>>>>,
      //Multi-blast, blaster reflect cycles through different responsive effects
    ResponsiveClashL<TransitionEffect<RgbArg<CLASH_COLOR_ARG,Rgb<255,240,80>>,LemonChiffon,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Responsive clash
    TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,Rgb16<38402,65535,3934>,RandomPerLEDFlicker<Rgb<60,60,60>,Black>,BrownNoiseFlicker<Rgb16<38402,65535,3934>,Rgb<30,30,30>,200>,RandomPerLEDFlicker<Rgb<80,80,80>,Rgb<30,30,30>>>,TrFade<500>>,EFFECT_IGNITION>,
      //Unstable ignition
    TransitionEffectL<TrConcat<TrInstant,AudioFlickerL<Rgb16<54757,65535,0>>,TrFade<1000>>,EFFECT_RETRACTION>,
      //Bright retraction
    LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      //Drag
    LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      //Responsive melt
    EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<8192>>,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<24576>>,AlphaL<Black,Int<0>>>,
      //Power save, if using fett263's prop file hold Aux and click PWR while ON (pointing up) to dim blade in 25% increments.
    InOutTrL<TrWipeSparkTip<White,250>,TrWipeInSparkTip<White,656>>,
      //Spark tip ignition and retraction
      //TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_BOOT>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_NEWFONT>,
      //Optional/alternate passive battery monitor, on boot (1st line) or font change (2nd line) you will get a visual indicator at the emitter of your current battery level. This also works without a blade if you have a lit emitter or blade plug. Green is Full, Red is Low (the color will blend from Green to Red as the battery is depleted), the indicator will fade out after 3000 ms and not display again until powered down and back up or fonts change.
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<1000>>,EFFECT_BATTERY_LEVEL>
      //On demand battery level, if using fett263's prop file Hold AUX and click PWR while OFF, the battery level is represented by the location on the blade; tip = full, hilt = low and color; green = full, yellow = half, red = low
    >>(),
    // === NPXL inner ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "assassin"},

   { "banished-prince;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Dark_Lord"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,240,80>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<LemonChiffon,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,180,50>,LemonChiffon,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<LemonChiffon,Rgb<255,180,50>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Orange,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,240,80>,LemonChiffon,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,240,80>,LemonChiffon,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,1000>,TrWipeIn<1060>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,240,80>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<LemonChiffon,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,180,50>,LemonChiffon,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<LemonChiffon,Rgb<255,180,50>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Orange>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,240,80>,LemonChiffon,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,240,80>,LemonChiffon,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,1000>,TrWipeIn<1060>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "banished\nprince"},

   { "battle1;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Battle1"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Green>,RotateColorsX<Variation,Rgb<0,128,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,150,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<Moccasin,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<Moccasin,Rgb<255,150,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Green>,RotateColorsX<Variation,Rgb<0,128,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,150,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<Moccasin,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<Moccasin,Rgb<255,150,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Red>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "battle1"},

   { "battle2;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Battle2"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Rgb<180,130,0>>,RotateColorsX<Variation,Rgb<90,65,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,150,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<Moccasin,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<Moccasin,Rgb<255,150,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Rgb<180,130,0>>,RotateColorsX<Variation,Rgb<90,65,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,150,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<Moccasin,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<Moccasin,Rgb<255,150,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Red>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "battle2"},

   { "battle3;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Battle3"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Rgb<95,0,210>>,RotateColorsX<Variation,Rgb<48,0,105>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,150,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<Moccasin,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<Moccasin,Rgb<255,150,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Rgb<95,0,210>>,RotateColorsX<Variation,Rgb<48,0,105>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,150,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<Moccasin,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<Moccasin,Rgb<255,150,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Red>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,150,0>,Moccasin,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "battle3"},

   { "bode-akuna;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
/* copyright Fett263 JediSurvivor (Primary Blade) OS7 Style (A_Rogue_Child 01/01/2025)
https://www.fett263.com/fett263-proffieOS7-style-library.html#JediSurvivor
OS7.14 v2.241p
Single Style
Base Style: Cal Kestis Survivor Green
Base Color: BaseColorArg (0)
--Effects Included--
Ignition Effect: Glitch On [Color: IgnitionColorArg]
Retraction Effect: Glitch Off [Color: RetractionColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Full Blade Flash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Full Blade Flash - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: AudioFlicker - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Fire Melt - End: Wipe Out
[Color: StabColorArg]
Blast Effect: Blast Wave (Sound Based) [Color: BlastColorArg]
Clash Effect: Responsive Clash with Wave [Color: ClashColorArg]
*/
//StylePtr<Layers<StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<4000>,Int<1800>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<2000>,Int<4000>>,Int<-2800>,Int<-4000>>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,255,0>>>,Mix<Sin<Int<20>,Int<22000>,Int<32768>>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,255,0>>>,Mix<Int<8000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<0,255,0>>>>,TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<AudioFlicker<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<8164>,Black,RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Layers<Stripes<2000,4000,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,Mix<Sin<Int<50>>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>,Mix<Int<4096>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,TransitionPulseL<TrConcat<TrExtend<4000,TrInstant>,Stripes<2000,3000,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,Mix<Sin<Int<30>>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>,Mix<Int<8192>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,TrFade<4000>>,LockupPulseF<SaberBase::LOCKUP_MELT>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<0>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>,Black>>>(),
/*
StylePtr<
 Layers<
  StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<4000>,Int<1800>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<2000>,Int<4000>>,Int<-2800>,Int<-4000>>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Green>>,Mix<Sin<Int<20>,Int<22000>,Int<32768>>,Black,RgbArg<BASE_COLOR_ARG,Green>>,Mix<Int<8000>,Black,RgbArg<BASE_COLOR_ARG,Green>>>,
  AudioFlickerL<RgbArg<BASE_COLOR_ARG,Rgb<0,200,0>>>,
  AudioFlickerL<AlphaL<RgbArg<EMITTER_COLOR_ARG,Rgb<50,100,100>>,SmoothStep<IntArg<EMITTER_SIZE_ARG,6000>,Int<-9000>>>>,
  TransitionEffectL<TrConcat<TrFade<300>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Rgb<50,255,100>,RgbArg<SWING_COLOR_ARG,Rgb<200,255,255>>>,TrDelayX<Percentage<WavLen<>,90>>,TrFade<300>>,EFFECT_FORCE>,
  TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,
  TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>,
  LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
  ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,RgbArg<LB_COLOR_ARG,White>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
  LockupTrL<AlphaL<AudioFlicker<RgbArg<DRAG_COLOR_ARG,White>,Mix<Int<8164>,Black,RgbArg<DRAG_COLOR_ARG,White>>>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
  TransitionEffectL<TrSparkX<AlphaL<White,SmoothStep<Int<25000>,Int<5000>>>,Int<100>,Int<1000>,Int<32768>>,EFFECT_DRAG_BEGIN>,
  LockupTrL<AlphaL<Layers<
    Stripes<2000,4000,RgbArg<STAB_COLOR_ARG,DarkOrange>,Mix<Sin<Int<50>>,Black,RgbArg<STAB_COLOR_ARG,DarkOrange>>,Mix<Int<4096>,Black,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,
    TransitionPulseL<TrConcat<TrExtend<4000,TrInstant>,Stripes<2000,3000,RgbArg<STAB_COLOR_ARG,DarkOrange>,Mix<Sin<Int<30>>,Black,RgbArg<STAB_COLOR_ARG,DarkOrange>>,Mix<Int<8192>,Black,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,TrFade<4000>>,LockupPulseF<SaberBase::LOCKUP_MELT>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrJoin<TrInstant,TrSparkX<AlphaL<Red,SmoothStep<Int<25000>,Int<5000>>>,Int<100>,Int<1000>,Int<32768>>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
  InOutTrL<TrConcat<TrJoin<TrDelayX<Mult<IgnitionTime<300>,Int<16384>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<0,255,0>>>,TrWipeX<Mult<IgnitionTime<300>,Int<16384>>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<0>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<0,255,0>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>,Black>>>(),
*/
/* copyright Fett263 FallenOrder (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#FallenOrder
OS7.14 v2.52p
This Style Contains 18 Unique Combinations
Base Style: Fallen Order Cal Kestis

Base Color: Kyber Select - BaseColorArg - AltColorArg - AltColor2Arg - AltColor3Arg - Rgb<0,255,255> - Rgb<255,0,255> - Rgb<115,15,240> - Rgb<100,100,150> (0)
Kyber Select Interactive Control: Before Igniting Rotate Hilt to change Kyber Crystal selection (Twist Angle, changing Preset will reset).

--Effects Included--
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Up = Fast) [Color: IgnitionColorArg]
PowerUp Effect Options: Default (0): Disable, 1: Power Burst Forward [Color: IgnitionColorArg]
Retraction Effect Options: Default (0): Standard Retraction, 1: Dual Mode Wipe In (Down = Fast), 2: SparkTip Retraction [Color: RetractionColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Blast Effect: Blast Wave (Random) [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Battery Level: % Blade (Green to Red)
Display Volume: % Blade [Color: BaseColorArg]
*/
    //StylePtr<Layers<Stripes<16000,-1000,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR2_ARG,Rgb<255,97,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Rgb<0,255,255>,Rgb<255,0,255>,Rgb<115,15,240>,Rgb<100,100,150>>,Pulsing<Mix<Int<11565>,Black,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR2_ARG,Rgb<255,97,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Rgb<0,255,255>,Rgb<255,0,255>,Rgb<115,15,240>,Rgb<100,100,150>>>,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR2_ARG,Rgb<255,97,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Rgb<0,255,255>,Rgb<255,0,255>,Rgb<115,15,240>,Rgb<100,100,150>>,800>,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR2_ARG,Rgb<255,97,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Rgb<0,255,255>,Rgb<255,0,255>,Rgb<115,15,240>,Rgb<100,100,150>>>,TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>>,EFFECT_IGNITION>,TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<IgnitionTime<300>,Int<16384>>,IgnitionTime<300>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,RetractionTime<0>,Mult<RetractionTime<0>,Int<16384>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>>,TrInstant,Mix<SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>,Black,ColorSelect<Scale<TwistAngle<>,Int<0>,Int<8>>,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR2_ARG,Rgb<255,97,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Rgb<0,255,255>,Rgb<255,0,255>,Rgb<115,15,240>,Rgb<100,100,150>>>,Black>>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Stripes<16000,-1000,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Green>,RgbArg<ALT_COLOR2_ARG,Orange>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Cyan,Magenta,Rgb<115,15,240>,Rgb<100,100,150>>,Pulsing<Mix<Int<11565>,Black,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Green>,RgbArg<ALT_COLOR2_ARG,Orange>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Cyan,Magenta,Rgb<115,15,240>,Rgb<100,100,150>>>,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Green>,RgbArg<ALT_COLOR2_ARG,Orange>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Cyan,Magenta,Rgb<115,15,240>,Rgb<100,100,150>>,800>,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Scale<TwistAngle<>,Int<0>,Int<9>>,Scale<TwistAngle<>,Int<0>,Int<8>>>,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Green>,RgbArg<ALT_COLOR2_ARG,Orange>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Cyan,Magenta,Rgb<115,15,240>,Rgb<100,100,150>>>,
      TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,Stripes<5000,-2500,RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>,TrFade<800>>>,EFFECT_IGNITION>,
      TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<IgnitionTime<>,Int<16384>>,IgnitionTime<>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,RetractionTime<>,Mult<RetractionTime<>,Int<16384>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,White>,BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,ColorSelect<IncrementWithReset<ThresholdPulseF<IncrementWithReset<ThresholdPulseF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_PREON>>,Int<30000>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>>,Int<1>>,EffectPulseF<EFFECT_NEWFONT>,Int<1>>,TrInstant,Mix<SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-2000>>,Black,ColorSelect<Scale<TwistAngle<>,Int<0>,Int<8>>,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Green>,RgbArg<ALT_COLOR2_ARG,Orange>,RgbArg<ALT_COLOR3_ARG,Rgb<180,130,0>>,Cyan,Magenta,Rgb<115,15,240>,Rgb<100,100,150>>>,Black>>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,
      TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Blue>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    // === NPXL inner ring: ===
/* copyright Fett263 JediSurvivor (Accent LED / PCB) OS7 Style
https://fett263library.com/early-access/#JediSurvivor
OS7.8 v1.012
Single Style
Base Style: Cal Kestis Survivor Blue

Off Behavior: Off [Color: OffColorArg]

Base Color: Kyber Select - BaseColorArg - AltColorArg - AltColor2Arg - AltColor3Arg - Rgb<255,0,255> - Rgb<255,68,0> - Rgb<115,15,240> - Rgb<100,100,150> - Rgb<180,130,0> (0)
Kyber Select Interactive Control: While Blade is On, use selected Special Ability to toggle, Swing Saber to change color (Phase).

*This style supports Alt Fonts alt000/ to alt008/. Uses altchng.wav on change

--Effects Included--
Ignition Effect: Instant [Color: IgnitionColorArg]
Retraction Effect: Instant [Color: RetractionColorArg]
Special Ability 1: Next Phase
Special Ability 2: Previous Phase
Special Ability 3: Select Random Phase
Special Ability 4: Toggle Swing Change (Random / Party Mode) Uses tr.wav #0 on toggle

*/
    //StylePtr<Layers<StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<8000>,Int<3000>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<-2600>,Int<-3600>>,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<12000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,Pulsing<ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<8000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,1400>>,InOutTrL<TrInstant,TrInstant,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<8000>,Int<3000>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<-2600>,Int<-3600>>,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<12000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,Pulsing<ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<8000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,1400>>,
      InOutTrL<TrInstant,TrInstant>>>(),
    /*StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(), //StyleNormalPtr,
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,                                                  //StyleFirePtr,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,                                                      //StyleRainbowPtr,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,                   //StyleStrobePtr, ...
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,                                //are "Old Legacy Styles"
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),*/
    // === NPXL outer ring: ===
/* copyright Fett263 JediSurvivor (Accent LED / PCB) OS7 Style
https://fett263library.com/early-access/#JediSurvivor
OS7.8 v1.012
Single Style
Base Style: Cal Kestis Survivor Blue

Off Behavior: Off [Color: OffColorArg]

Base Color: Kyber Select - BaseColorArg - AltColorArg - AltColor2Arg - AltColor3Arg - Rgb<255,0,255> - Rgb<255,68,0> - Rgb<115,15,240> - Rgb<100,100,150> - Rgb<180,130,0> (0)
Kyber Select Interactive Control: While Blade is On, use selected Special Ability to toggle, Swing Saber to change color (Phase).

*This style supports Alt Fonts alt000/ to alt008/. Uses altchng.wav on change

--Effects Included--
Ignition Effect: Instant [Color: IgnitionColorArg]
Retraction Effect: Instant [Color: RetractionColorArg]
Special Ability 1: Next Phase
Special Ability 2: Previous Phase
Special Ability 3: Select Random Phase
Special Ability 4: Toggle Swing Change (Random / Party Mode) Uses tr.wav #0 on toggle

*/
    //StylePtr<Layers<StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<8000>,Int<3000>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<-2600>,Int<-3600>>,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<12000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,Pulsing<ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<8000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,1400>>,InOutTrL<TrInstant,TrInstant,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<8000>,Int<3000>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<-2600>,Int<-3600>>,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<12000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,Pulsing<ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<8000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,1400>>,
      InOutTrL<TrInstant,TrInstant>>>(),
    /*StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),*/
    // === Crystal Chamber: ===
/* copyright Fett263 JediSurvivor (Accent LED / PCB) OS7 Style
https://fett263library.com/early-access/#JediSurvivor
OS7.8 v1.012
Single Style
Base Style: Cal Kestis Survivor Blue

Off Behavior: Off [Color: OffColorArg]

Base Color: Kyber Select - BaseColorArg - AltColorArg - AltColor2Arg - AltColor3Arg - Rgb<255,0,255> - Rgb<255,68,0> - Rgb<115,15,240> - Rgb<100,100,150> - Rgb<180,130,0> (0)
Kyber Select Interactive Control: While Blade is On, use selected Special Ability to toggle, Swing Saber to change color (Phase).

*This style supports Alt Fonts alt000/ to alt008/. Uses altchng.wav on change

--Effects Included--
Ignition Effect: Instant [Color: IgnitionColorArg]
Retraction Effect: Instant [Color: RetractionColorArg]
Special Ability 1: Next Phase
Special Ability 2: Previous Phase
Special Ability 3: Select Random Phase
Special Ability 4: Toggle Swing Change (Random / Party Mode) Uses tr.wav #0 on toggle

*/
    //StylePtr<Layers<StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<8000>,Int<3000>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<-2600>,Int<-3600>>,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<12000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,Pulsing<ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<8000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,255>>,RgbArg<ALT_COLOR2_ARG,Rgb<0,255,0>>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Rgb<255,0,255>,Rgb<255,68,0>,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,1400>>,InOutTrL<TrInstant,TrInstant,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      StripesX<Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<8000>,Int<3000>>,Scale<HoldPeakF<Sum<EffectPulseF<EFFECT_IGNITION>,EffectPulseF<EFFECT_ALT_SOUND>>,Int<3000>,Int<3000>>,Int<-2600>,Int<-3600>>,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<12000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,Pulsing<ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>,Mix<Int<8000>,Black,ColorSelect<AltF,TrInstant,RgbArg<BASE_COLOR_ARG,Blue>,RgbArg<ALT_COLOR_ARG,Cyan>,RgbArg<ALT_COLOR2_ARG,Green>,RgbArg<ALT_COLOR3_ARG,Rgb<80,50,210>>,Magenta,DarkOrange,Rgb<115,15,240>,Rgb<100,100,150>,Rgb<180,130,0>>>,1400>>,
      InOutTrL<TrInstant,TrInstant>>>(),
    /*StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),*/
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "bode\nakuna" },

   { "c110p_chopper;common", "c110p_chopper/tracks/rebels_theme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Negotiator"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Blue>,RotateColorsX<Variation,Rgb<0,0,128>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Blue>,RotateColorsX<Variation,Rgb<0,0,128>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Red>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "c110p\nchopper" },

   { "Cal-Kestis_BlueNormalized;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Survivor"
    //StylePtr<Layers<Stripes<16000,-1000,RotateColorsX<Variation,Blue>,Pulsing<RotateColorsX<Variation,Rgb<0,0,90>>,RotateColorsX<Variation,Blue>,800>,RotateColorsX<Variation,Blue>>,TransitionEffectL<TrConcat<TrFade<600>,RandomFlicker<RotateColorsX<Variation,Blue>,RotateColorsX<Variation,Rgb<0,0,160>>>,TrDelay<30000>,RotateColorsX<Variation,Blue>,TrFade<800>>,EFFECT_FORCE>,LockupTrL<Layers<AlphaL<AudioFlickerL<White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>,Int<400>>,ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Stripes<16000,-1000,RotateColorsX<Variation,Blue>,Pulsing<RotateColorsX<Variation,Rgb<0,0,90>>,RotateColorsX<Variation,Blue>,800>,RotateColorsX<Variation,Blue>>,
      TransitionEffectL<TrConcat<TrFade<600>,RandomFlicker<RotateColorsX<Variation,Blue>,RotateColorsX<Variation,Rgb<0,0,160>>>,TrDelay<30000>,RotateColorsX<Variation,Blue>,TrFade<800>>,EFFECT_FORCE>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Red>,
      ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<MarbleStyle>(),
    // === NPXL outer ring: ===
    StylePtr<MarbleStyle>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<CYAN,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Cyan>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Cyan>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "cal-kestis\n blue"},

   { "celestial-travels;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Grandmaster"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Green>,RotateColorsX<Variation,Rgb<0,128,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Green>,RotateColorsX<Variation,Rgb<0,128,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Red>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "celestial\n travels"},

   { "centennial;common", "centennial/tracks/centennial.wav",
    // === Main Blade: ===
/* copyright Fett263 CustomBlade (Primary Blade) OS6 Style
https://fett263.com/fett263-proffieOS6-style-library.html#CustomBlade
OS6.7 v4.018
Single Style
Interactive Kinetic Swing
Default: Fire Blade Medium (Normal)
1: Fire Blade Fastest (Normal)

Base Color: Rgb<255,255,0> (0)
Build up kinetic energy with each swing to apply Style 2 in increasing levels based on inertia, once you've stopped moving the blade it will slowly dissipate and return to Style 1. (stop swinging for approx 5 seconds or clash to dissipate)

--Effects Included--
Ignition Effect: SparkTip Ignition
PowerUp Effect: Power Surge (Unstable V1)
Retraction Effect: SparkTip Retraction
CoolDown Effect: Unstable Cool Down Reverse
PostOff Effect: Emitter Spark (Emitter Size)
Lockup Effect: Intensity Lockup V1, Begin Effect: Real Clash, End Effect: Full Blade Absorb
LightningBlock Effect: Responsive Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Blast Effect: Blast Wave (Random)
Clash Effect: Real Clash V1
Swing Effect: Interactive Ice Blade (Force Effect)
Emitter Effect: Emitter Flare

Swing Option 0- Interactive Ice Blade (Force Effect) Interactive Control: Force to freeze blade, clashes will break ice.
*/
    StylePtr<Layers<
      Mix<HoldPeakF<IncrementWithReset<ThresholdPulseF<SwingSpeed<400>,Int<24000>>,ThresholdPulseF<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>,IncrementWithReset<ThresholdPulseF<BlinkingF<Int<1000>,Int<500>>,Int<32000>>,ThresholdPulseF<SwingSpeed<400>,Int<16000>>,Int<32768>,Int<8000>>>,Int<32000>>,Int<32768>,Scale<SwingAcceleration<>,Int<4000>,Int<8000>>>,Int<50>,Scale<IsLessThan<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>>,Int<32000>>,Int<6000>,Int<32768>>>,StaticFire<RgbArg<BASE_COLOR_ARG,Yellow>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Yellow>>,0,2,0,2000,3>,StaticFire<RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>,Mix<Int<10280>,Black,RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>>,0,5>>,
      AlphaL<RgbArg<EMITTER_COLOR_ARG,Rgb<118,0,194>>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-6000>>>,
      AlphaL<AudioFlickerL<RgbArg<SWING_COLOR_ARG,Rgb<118,0,194>>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,
      ColorSelect<IncrementWithReset<EffectPulseF<EFFECT_FORCE>,ThresholdPulseF<IncrementWithReset<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_STAB>>,EffectPulseF<EFFECT_FORCE>,Int<32768>,Scale<ClashImpactF<>,Int<3000>,Int<14000>>>,Int<32600>>,Int<1>>,TrSmoothFadeX<Int<2000>>,AlphaL<RgbArg<BASE_COLOR_ARG,Yellow>,Int<0>>,RgbArg<BASE_COLOR_ARG,Yellow>>,
      ColorSelect<IncrementWithReset<EffectPulseF<EFFECT_FORCE>,ThresholdPulseF<IncrementWithReset<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_STAB>>,EffectPulseF<EFFECT_FORCE>,Int<32768>,Scale<ClashImpactF<>,Int<3000>,Int<14000>>>,Int<32600>>,Int<1>>,TrSelect<IncrementWithReset<EffectPulseF<EFFECT_FORCE>,Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_STAB>>,Int<1>>,TrSmoothFadeX<Int<3000>>,TrWipeSparkTipX<BrownNoiseFlicker<RgbArg<SWING_COLOR_ARG,Rgb<100,100,150>>,White,200>,WavLen<EFFECT_FORCE>>>,AlphaL<RgbArg<BASE_COLOR_ARG,Yellow>,Int<0>>,AlphaL<Stripes<22000,-20,RgbArg<SWING_COLOR_ARG,Rgb<100,100,150>>,Mix<Int<18000>,Black,RgbArg<SWING_COLOR_ARG,Rgb<100,100,150>>>>,SmoothStep<Sum<Int<33000>,IncrementWithReset<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_STAB>>,EffectPulseF<EFFECT_FORCE>,Int<33000>,Scale<Scale<ClashImpactF<>,Int<3000>,Int<14000>>,Int<-3000>,Int<-14000>>>>,Int<-1>>>>,
      TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>>>>,TrFade<1200>>,EFFECT_IGNITION>,
      TransitionEffectL<TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<118,0,194>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<118,0,194>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<118,0,194>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<118,0,194>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<118,0,194>>>>>,TrFade<800>>,EFFECT_RETRACTION>,
      TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Cyan>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrJoin<TrDelay<8000>,TrInstant>,TrFade<3000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,White>,Int<16000>>,30>,TrSmoothFade<600>>>,
      LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrConcat<TrJoin<TrDelay<4000>,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>,Mix<TwistAngle<>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>,RotateColorsX<Int<3000>,Mix<Int<12000>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrJoin<TrDelay<4000>,TrFade<300>>,AlphaL<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>,IgnitionTime<>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<118,0,194>>,RetractionTime<>>>,
      TransitionEffectL<TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,Rgb<118,0,194>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<EMITTER_SIZE_ARG,2000>,Sum<IntArg<EMITTER_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,EFFECT_POSTOFF>>>(),
    // === NPXL inner ring: ===
/* copyright Fett263 CustomBlade (Accent LED / PCB) OS6 Style
https://fett263.com/fett263-proffieOS6-style-library.html#CustomBlade
OS6.7 v4.018
Single Style
Interactive Kinetic Swing
Default: Fire Blade Medium (Normal)
1: Fire Blade Fastest (Normal)

Off Behavior: Fast Run Forward

Base Color: Rgb<255,255,0> (0)
Build up kinetic energy with each swing to apply Style 2 in increasing levels based on inertia, once you've stopped moving the blade it will slowly dissipate and return to Style 1. (stop swinging for approx 5 seconds or clash to dissipate)

--Effects Included--
Ignition Effect: Standard Ignition
PowerUp Effect: Power Flash
Retraction Effect: Run Up
Lockup Effect: Full Blade Lockup (Dissipate)
LightningBlock Effect: Full Blade Lightning
Blast Effect: Full Blade Blast Fade
Clash Effect: Flash on Clash (Full Blade)
Swing Effect: AudioFlicker Swing (Scaled Full Blade)
*/
    //StylePtr<Layers<Mix<HoldPeakF<IncrementWithReset<ThresholdPulseF<SwingSpeed<400>,Int<24000>>,ThresholdPulseF<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>,IncrementWithReset<ThresholdPulseF<BlinkingF<Int<1000>,Int<500>>,Int<32000>>,ThresholdPulseF<SwingSpeed<400>,Int<16000>>,Int<32768>,Int<8000>>>,Int<32000>>,Int<32768>,Scale<SwingAcceleration<>,Int<4000>,Int<8000>>>,Int<50>,Scale<IsLessThan<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>>,Int<32000>>,Int<6000>,Int<32768>>>,StaticFire<RgbArg<BASE_COLOR_ARG,Rgb<255,255,0>>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,255,0>>>,0,2,0,2000,3>,StaticFire<RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>,Mix<Int<10280>,Black,RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>>,0,5,0,2000,5>>,AlphaL<AudioFlickerL<RgbArg<SWING_COLOR_ARG,Rgb<118,0,194>>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,TransitionEffectL<TrConcat<TrJoin<TrDelayX<IgnitionTime<0>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>,TrFade<1000>>,EFFECT_IGNITION>,TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Rgb<0,255,255>>,TrFade<300>>,EFFECT_BLAST>,TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,TrFade<300>>,EFFECT_CLASH>,LockupTrL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,TrFade<400>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>>,SaberBase::LOCKUP_NORMAL>,LockupTrL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrInstant,TrFade<1000>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,InOutTrL<TrWipeX<IgnitionTime<0>>,TrWipeX<RetractionTime<0>>,TransitionLoop<Black,TrConcat<TrWipe<200>,RgbArg<OFF_COLOR_ARG,Rgb<255,255,0>>,TrWipe<200>>>>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Mix<HoldPeakF<IncrementWithReset<ThresholdPulseF<SwingSpeed<400>,Int<24000>>,ThresholdPulseF<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>,IncrementWithReset<ThresholdPulseF<BlinkingF<Int<1000>,Int<500>>,Int<32000>>,ThresholdPulseF<SwingSpeed<400>,Int<16000>>,Int<32768>,Int<8000>>>,Int<32000>>,Int<32768>,Scale<SwingAcceleration<>,Int<4000>,Int<8000>>>,Int<50>,Scale<IsLessThan<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>>,Int<32000>>,Int<6000>,Int<32768>>>,StaticFire<RgbArg<BASE_COLOR_ARG,Yellow>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Yellow>>,0,2,0,2000,3>,StaticFire<RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>,Mix<Int<10280>,Black,RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>>,0,5>>,
      AlphaL<AudioFlickerL<RgbArg<SWING_COLOR_ARG,Rgb<118,0,194>>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,
      TransitionEffectL<TrConcat<TrJoin<TrDelayX<IgnitionTime<0>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>,TrFade<1000>>,EFFECT_IGNITION>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Cyan>,TrFade<300>>,EFFECT_BLAST>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,TrFade<300>>,EFFECT_CLASH>,
      LockupTrL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,TrFade<400>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>>,SaberBase::LOCKUP_NORMAL>,
      LockupTrL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrInstant,TrFade<1000>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,
      InOutTrL<TrWipeX<IgnitionTime<0>>,TrWipeX<RetractionTime<>>,TransitionLoop<Black,TrConcat<TrWipe<200>,RgbArg<OFF_COLOR_ARG,Yellow>,TrWipe<200>>>>>>(),
    // === NPXL outer ring: ===
    //StylePtr<Layers<Mix<HoldPeakF<IncrementWithReset<ThresholdPulseF<SwingSpeed<400>,Int<24000>>,ThresholdPulseF<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>,IncrementWithReset<ThresholdPulseF<BlinkingF<Int<1000>,Int<500>>,Int<32000>>,ThresholdPulseF<SwingSpeed<400>,Int<16000>>,Int<32768>,Int<8000>>>,Int<32000>>,Int<32768>,Scale<SwingAcceleration<>,Int<4000>,Int<8000>>>,Int<50>,Scale<IsLessThan<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>>,Int<32000>>,Int<6000>,Int<32768>>>,StaticFire<RgbArg<BASE_COLOR_ARG,Rgb<255,255,0>>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,255,0>>>,0,2,0,2000,3>,StaticFire<RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>,Mix<Int<10280>,Black,RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>>,0,5,0,2000,5>>,AlphaL<AudioFlickerL<RgbArg<SWING_COLOR_ARG,Rgb<118,0,194>>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,TransitionEffectL<TrConcat<TrJoin<TrDelayX<IgnitionTime<0>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>,TrFade<1000>>,EFFECT_IGNITION>,TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Rgb<0,255,255>>,TrFade<300>>,EFFECT_BLAST>,TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,TrFade<300>>,EFFECT_CLASH>,LockupTrL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,TrFade<400>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>>,SaberBase::LOCKUP_NORMAL>,LockupTrL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrInstant,TrFade<1000>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,InOutTrL<TrWipeX<IgnitionTime<0>>,TrWipeX<RetractionTime<0>>,TransitionLoop<Black,TrConcat<TrWipe<200>,RgbArg<OFF_COLOR_ARG,Rgb<255,255,0>>,TrWipe<200>>>>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Mix<HoldPeakF<IncrementWithReset<ThresholdPulseF<SwingSpeed<400>,Int<24000>>,ThresholdPulseF<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>,IncrementWithReset<ThresholdPulseF<BlinkingF<Int<1000>,Int<500>>,Int<32000>>,ThresholdPulseF<SwingSpeed<400>,Int<16000>>,Int<32768>,Int<8000>>>,Int<32000>>,Int<32768>,Scale<SwingAcceleration<>,Int<4000>,Int<8000>>>,Int<50>,Scale<IsLessThan<Sum<EffectPulseF<EFFECT_CLASH>,EffectPulseF<EFFECT_LOCKUP_BEGIN>>,Int<32000>>,Int<6000>,Int<32768>>>,StaticFire<RgbArg<BASE_COLOR_ARG,Yellow>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Yellow>>,0,2,0,2000,3>,StaticFire<RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>,Mix<Int<10280>,Black,RgbArg<ALT_COLOR_ARG,Rgb<180,130,0>>>,0,5>>,
      AlphaL<AudioFlickerL<RgbArg<SWING_COLOR_ARG,Rgb<118,0,194>>>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,
      TransitionEffectL<TrConcat<TrJoin<TrDelayX<IgnitionTime<0>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<118,0,194>>,TrFade<1000>>,EFFECT_IGNITION>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Cyan>,TrFade<300>>,EFFECT_BLAST>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,TrFade<300>>,EFFECT_CLASH>,
      LockupTrL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,TrFade<400>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>>,SaberBase::LOCKUP_NORMAL>,
      LockupTrL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrInstant,TrFade<1000>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,
      InOutTrL<TrWipeX<IgnitionTime<0>>,TrWipeX<RetractionTime<>>,TransitionLoop<Black,TrConcat<TrWipe<200>,RgbArg<OFF_COLOR_ARG,Yellow>,TrWipe<200>>>>>>(),
    // === Crystal Chamber: ===
/* copyright Fett263 (Accent LED / PCB) OS6 Style
https://fett263.com/fett263-proffieOS6-style-library.html#CustomBlade
OS6.7 v4.018
Single Style
On Behavior: Power Wave High
Off Behavior: Power Wave Slow

Base Color: Rgb<118,0,194> (0)

--Effects Included--
Ignition Effect: Instant
PowerUp Effect: Power Flash
Retraction Effect: Instant
Lockup Effect: Full Blade Lockup (Dissipate)
LightningBlock Effect: Full Blade Lightning
Blast Effect: Full Blade Blast Fade
Clash Effect: Flash on Clash (Full Blade)
*/
    //StylePtr<Layers<Stripes<12000,-1800,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<6000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>>,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<14000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>>>,TransitionEffectL<TrConcat<TrJoin<TrDelayX<IgnitionTime<0>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,0>>,TrFade<1000>>,EFFECT_IGNITION>,TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Rgb<0,255,255>>,TrFade<300>>,EFFECT_BLAST>,TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,TrFade<300>>,EFFECT_CLASH>,LockupTrL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,TrFade<400>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>>,SaberBase::LOCKUP_NORMAL>,LockupTrL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrInstant,TrFade<1000>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,InOutTrL<TrInstant,TrInstant,Stripes<16000,-400,RgbArg<OFF_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<8000>,Black,RgbArg<OFF_COLOR_ARG,Rgb<118,0,194>>>,Mix<Int<16000>,Black,RgbArg<OFF_COLOR_ARG,Rgb<118,0,194>>>>>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      Stripes<12000,-1800,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<6000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>>,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<14000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<118,0,194>>>>,
      TransitionEffectL<TrConcat<TrJoin<TrDelayX<IgnitionTime<0>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Yellow>,TrFade<1000>>,EFFECT_IGNITION>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Cyan>,TrFade<300>>,EFFECT_BLAST>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<CLASH_COLOR_ARG,Rgb<118,0,194>>,TrFade<300>>,EFFECT_CLASH>,
      LockupTrL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,TrFade<400>>,TrConcat<TrInstant,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<0>>,TrWaveX<RgbArg<LOCKUP_COLOR_ARG,Rgb<118,0,194>>,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>>,SaberBase::LOCKUP_NORMAL>,
      LockupTrL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrInstant,TrFade<1000>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,
      InOutTrL<TrInstant,TrInstant,Stripes<16000,-400,RgbArg<OFF_COLOR_ARG,Rgb<118,0,194>>,Mix<Int<8000>,Black,RgbArg<OFF_COLOR_ARG,Rgb<118,0,194>>>,Mix<Int<16000>,Black,RgbArg<OFF_COLOR_ARG,Rgb<118,0,194>>>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "centennial"},

   { "cere-junda;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Shadow_Hunter"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Orange,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Orange>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "cere\njunda"},

   { "coda;common", "common/tracks/mandotheme.wav",
/*
    StylePtr<Layers< // Argified from olicomplex1.6.00.h
      // BASE BLADE
    AudioFlicker<Stripes<23000,-1300,RgbArg<BASE_COLOR_ARG,Green>,RgbArg<BASE_COLOR_ARG,Green>,Mix<Int<11284>,Black,RgbArg<BASE_COLOR_ARG,Green>>,RgbArg<BASE_COLOR_ARG,Green>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Green>>>,RgbArg<BASE_COLOR_ARG,Green>>,
      // USER 3 RAIN EFFECT??
    ColorSelect<EffectIncrementF<EFFECT_USER3,Int<2>>,TrFade<100>,AlphaL<White,Int<0>>,AlphaL<RgbArg<RETRACTION_COLOR_ARG,White>,SparkleF<300,800>>>,
      // USER 3 SOUND EFFECT
    TransitionEffectL<TrDoEffect<TrInstant,EFFECT_SOUND_LOOP,0>,EFFECT_USER3>,
      // STAB EFFECT
    TransitionEffectL<TrConcat<TrJoin<TrDelayX<WavLen<>>,TrWipeIn<200>>,AlphaL<AudioFlickerL<RgbArg<STAB_COLOR_ARG,DarkOrange>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<2000>>>,TrWipe<200>>,EFFECT_STAB>,
      // BLAST EFFECT
    TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Moccasin>,Scale<WavLen<>,Int<100>,Int<400>>,Int<100>,Scale<WavLen<>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<BLAST_COLOR_ARG,Moccasin>,TrFade<300>>>,EFFECT_BLAST>,
      // CLASH EFFECT
    Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Moccasin>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Moccasin>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      // LOCKUP EFFECT
    LockupTrL<
      // LOCKUP BEGIN
    TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Moccasin>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Moccasin>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Moccasin>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Moccasin>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Moccasin>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Moccasin>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,
      // LOCKUP
    TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Moccasin>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Moccasin>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,
      // LOCKUP END
    TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Moccasin>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      // LIGHTNING BLOCK
    ResponsiveLightningBlockL<
      // LB
    Strobe<RgbArg<LB_COLOR_ARG,LightCyan>,AudioFlicker<RgbArg<LB_COLOR_ARG,LightCyan>,Blue>,50,1>,
      // LB START
    TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,LightCyan>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,
      // LB END
    TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,LightCyan>,TrFade<400>>>,
      // DRAG EFFECT
    LockupTrL<AlphaL<
      // DRAG BEGIN
    TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,
      // DRAG SIZE
    SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,
      // DRAG IN/OUT EFFECT
    TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      // STAB EFFECT
    LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,
      // STAB SIZE
    SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,
      // STAB START EFFECT
    TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      // BLAST EFFECT
    TransitionEffectL<TrConcat<TrDelayX<Mult<WavLen<EFFECT_INTERACTIVE_BLAST>,Int<24576>>>,AlphaL<White,Int<0>>,TrDoEffect<TrInstant,EFFECT_GAME_CHOICE>>,EFFECT_INTERACTIVE_BLAST>,
      // EFFECT GAME CHOICE: Choose to deflect blast.
    TransitionEffectL<TrConcat<TrExtendX<IntArg<STYLE_OPTION2_ARG,1000>,TrInstant>,TransitionPulseL<TrDoEffect<TrInstant,EFFECT_BLAST>,ThresholdPulseF<SwingSpeed<300>,Int<16000>>>,TrInstant>,EFFECT_GAME_CHOICE>,
      // IN / OUT EFFECT
    InOutTrL<
      // IGNITION EFFECT
    TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>>,
      // RETRACTION EFFECT
    TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>>,
      // EFFECT USER 7
    TransitionEffectL<TrConcat<TrDelayX<Int<1000>>,AlphaL<White,Int<0>>,TrConcat<TrInstant,AudioFlickerL<RgbArg<IGNITION_COLOR_ARG,White>>,TrFade<1200>>>,EFFECT_USER7>,TransitionEffectL<TrConcat<TrDelayX<Int<1000>>,Black,TrWipeX<Int<300>>>,EFFECT_USER7>,TransitionEffectL<TrConcat<TrDoEffectAlwaysX<TrDelayX<Int<1000>>,EFFECT_QUOTE,Int<1>,Int<-1>>,AlphaL<White,Int<0>>,TrDoEffectAlwaysX<TrInstant,EFFECT_FAST_ON,Int<-1>,Int<-1>>>,EFFECT_USER7>,
      // BATTERY LEVEL
    TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>(),
*/
    StylePtr<Layers< // Argified from olicomplex1.6.00.h
      //Rotates 3 times per minute between DodgerBlue and DeepSkyBlue with subtle Cyan/Blue audioflicker
    Mix<Sin<Int<3>>,AudioFlicker<RgbArg<BASE_COLOR_ARG,DodgerBlue>,Stripes<5000,-90,Rgb<0,55,165>,Rgb<0,85,115>,Rgb<0,65,145>,Rgb<0,30,185>>>,AudioFlicker<Rgb<0,155,255>,Stripes<5000,-90,Rgb<0,95,170>,Rgb<0,115,215>,Rgb<0,145,145>,Rgb<0,65,225>>>>,
      //Alternate DeepSkyBlue with very subtle audioflicker
      //AudioFlicker<RotateColorsX<Variation,DeepSkyBlue>,RotateColorsX<Variation,Rgb<0,95,195>>>,
      //Unstable swing
    AlphaL<BrownNoiseFlicker<DeepSkyBlue,Black,300>,SwingSpeed<400>>,
      //Unstable ripple swing
    AlphaL<Stripes<1000,-2000,RandomPerLEDFlicker<DeepSkyBlue,Black>,Black,DeepSkyBlue,Black>,SwingSpeed<525>>,
      //Responsive lockup effect with screeching humpflicker endlock
    LockupTrL<Layers<
      AlphaL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,240,80>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
      AlphaL<LemonChiffon,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<875>>,SaberBase::LOCKUP_NORMAL>,
      //Alternate standard lockup
    //LockupTrL<Layers<
      //AlphaL<AudioFlickerL<White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
      //AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<4000>>,SaberBase::LOCKUP_NORMAL>,
      //Responsive lightning block
    ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      //Responsive stab
    ResponsiveStabL<RgbArg<STAB_COLOR_ARG,Yellow>>,
      //Alternate responsive stab, no color change
      //ResponsiveStabL<Red>,
      //Multi-blast, blaster reflect cycles through different responsive effects
    EffectSequence<EFFECT_BLAST,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,LocalizedClashL<RgbArg<BLAST_COLOR_ARG,White>,80,30,EFFECT_BLAST>,ResponsiveBlastWaveL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<500>,Int<200>>,Scale<SwingSpeed<400>,Int<100>,Int<400>>>,BlastL<RgbArg<BLAST_COLOR_ARG,White>,200,200>,ResponsiveBlastFadeL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<6000>,Int<12000>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<400>,Int<100>>,Scale<SwingSpeed<400>,Int<200>,Int<100>>,Scale<SwingSpeed<400>,Int<400>,Int<200>>>>,
      //Alternate responsive blast
      //ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      //Responsive Clash
    ResponsiveClashL<TransitionEffect<RgbArg<CLASH_COLOR_ARG,Rgb<255,240,80>>,LemonChiffon,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Alternate white clash
      //ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Stabilize ignition effect
    TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,Rgb<90,180,255>,RandomPerLEDFlicker<Rgb<0,26,60>,Black>,BrownNoiseFlicker<Rgb<90,180,255>,Rgb<0,26,60>,200>,RandomPerLEDFlicker<Rgb<0,42,128>,Rgb<0,26,60>>>,TrFade<400>,Stripes<3000,-3500,Rgb<90,180,255>,RandomPerLEDFlicker<Rgb<0,26,60>,Black>,BrownNoiseFlicker<Rgb<90,180,255>,Rgb<0,26,60>,200>,RandomPerLEDFlicker<Rgb<0,42,128>,Rgb<0,26,60>>>,TrFade<450>,HumpFlickerL<AlphaL<Rgb<90,180,255>,Int<19000>>,25>,TrFade<2875>>,EFFECT_IGNITION>,
      //Slow brighten retraction effect
    TransitionEffectL<TrConcat<TrFade<1125>,Rgb<90,180,255>,TrInstant>,EFFECT_RETRACTION>,
      //Drag
    LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      //Responsive melt
    LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      //Power save, if using fett263's prop file hold Aux and click PWR while ON (pointing up) to dim blade in 25% increments.
    EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<8192>>,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<24576>>,AlphaL<Black,Int<0>>>,
      //Standard ignition/retraction
    InOutTrL<TrWipe<500>,TrWipeIn<1125>>,
      //Optional/alternate passive battery monitor, on boot (1st line) or font change (2nd line) you will get a visual indicator at the emitter of your current battery level. This also works without a blade if you have a lit emitter or blade plug. Green is Full, Red is Low (the color will blend from Green to Red as the battery is depleted), the indicator will fade out after 3000 ms and not display again until powered down and back up or fonts change.
      //TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_BOOT>,
      //TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_NEWFONT>,
      //On demand battery level, if using fett263's prop file Hold AUX and click PWR while OFF, the battery level is represented by the location on the blade; tip = full, hilt = low and color; green = full, yellow = half, red = low
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<1000>>,EFFECT_BATTERY_LEVEL>,
      //Overload preon effect
    TransitionEffectL<TrConcat<TrFade<450>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,DeepSkyBlue>,10>,Bump<Int<0>,Int<3000>>>,TrFade<350>,AlphaL<HumpFlickerL<DeepSkyBlue,15>,Bump<Int<0>,Int<5000>>>,TrFade<450>,AlphaL<HumpFlickerL<DeepSkyBlue,20>,Bump<Int<0>,Int<7000>>>,TrFade<500>,AlphaL<HumpFlickerL<DeepSkyBlue,25>,Bump<Int<0>,Int<10500>>>,TrBoing<150,1>>,EFFECT_PREON>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "coda"},

   { "crucible;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Emperor"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Orange,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Orange>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,1>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "crucible"},

   { "DaganGeraLight;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Tragedy"
    //StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Orange,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,White,RandomPerLEDFlicker<Rgb<60,60,60>,Black>,BrownNoiseFlicker<White,Rgb<30,30,30>,200>,RandomPerLEDFlicker<Rgb<80,80,80>,Rgb<30,30,30>>>,TrFade<800>>,EFFECT_IGNITION>,TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<White,40>,TrFade<1000>>,EFFECT_RETRACTION>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,
      LockupTrL<Layers<
        AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
        AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<NavajoWhite,Rgb<255,225,0>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      ResponsiveStabL<Orange>,
      ResponsiveBlastL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,
      ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,White,RandomPerLEDFlicker<Rgb<60,60,60>,Black>,BrownNoiseFlicker<White,Rgb<30,30,30>,200>,RandomPerLEDFlicker<Rgb<80,80,80>,Rgb<30,30,30>>>,TrFade<800>>,EFFECT_IGNITION>,
      TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<White,40>,TrFade<1000>>,EFFECT_RETRACTION>,
      LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>>>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "dagangera\nlight"},

   { "DaganGeraDark;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
/* copyright Fett263 CustomBlade (Primary Blade) OS7 Style
https://fett263library.com/early-access/#CustomBlade
OS7.8 v1.012
Single Style
Yin and Yang Blade (Blade Angle)
Default: Static
1: Static

Base Color: BaseColorArg (0)
Yin and Yang Blade: Blade changes based on pointing up or down, creates yin and yang effect when spun.

--Effects Included--
Ignition Effect: Standard Ignition [Color: IgnitionColorArg]
Retraction Effect: Standard Retraction [Color: RetractionColorArg]
Lockup Effect: Intensity Lockup V1, Begin Effect: Real Clash, End Effect: Full Blade Absorb [Color: LockupColorArg]
LightningBlock Effect: Responsive Lightning Block [Color: LBColorArg]
Drag Effect: Intensity Sparking Drag [Color: DragColorArg]
Melt Effect: Intensity Melt [Color: StabColorArg]
Blast Effect: Blast Wave (Random) [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
*/
    StylePtr<Layers<Mix<IsLessThan<BladeAngle<>,Int<16384>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<0,255,0>>>,TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<200>>,StripesX<Int<1800>,Scale<NoisySoundLevel,Int<-3500>,Int<-5000>>,Mix<Int<6425>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12850>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<10280>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Int<13000>>>,TrJoin<TrDelay<8000>,TrInstant>,TrFade<3000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrConcat<TrJoin<TrDelay<4000>,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<TwistAngle<>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>,RotateColorsX<Int<3000>,Mix<Int<12000>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrJoin<TrDelay<4000>,TrFade<300>>,AlphaL<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeX<BendTimePowX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<BendTimePowX<RetractionTime<0>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,Black>>>(),
    // === NPXL inner ring: ===
    StylePtr<YingYang>(),
    // === NPXL outer ring: ===
    StylePtr<YingYang>(),
    // === Crystal Chamber: ===
    StylePtr<YingYang>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "dagangera\ndark"},

   { "DarthVader;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Prodigal_Son"
    StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Green>,RotateColorsX<Variation,Rgb<0,128,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<Rgb<255,240,80>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<LemonChiffon,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,TransitionEffect<Rgb<255,180,50>,LemonChiffon,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,TrConcat<TrInstant,TransitionEffect<LemonChiffon,Rgb<255,180,50>,TrInstant,TrFade<200>,EFFECT_LOCKUP_BEGIN>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Red,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<TransitionEffect<Rgb<255,240,80>,LemonChiffon,TrInstant,TrFade<50>,EFFECT_BLAST>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,ResponsiveClashL<TransitionEffect<Rgb<255,240,80>,LemonChiffon,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTip<White,300>,TrWipeIn<500>,Black>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "darth\nvader"},

   { "DarkSouls;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers<Mix<BladeAngle<>,StyleFire<Gradient<RotateColorsX<Variation,Orange>,RotateColorsX<Variation,DarkOrange>,RotateColorsX<Variation,OrangeRed>,RotateColorsX<Variation,Red>>,RotateColorsX<Variation,Rgb<60,0,0>>,0,3,FireConfig<0,2000,5>,FireConfig<0,2000,5>,FireConfig<0,2000,5>,FireConfig<0,2000,5>>,StyleFire<Gradient<RotateColorsX<Variation,Orange>,RotateColorsX<Variation,DarkOrange>,RotateColorsX<Variation,OrangeRed>,RotateColorsX<Variation,Red>>,RotateColorsX<Variation,Rgb<60,0,0>>,0,6,FireConfig<0,2000,5>,FireConfig<0,2000,5>,FireConfig<0,2000,5>,FireConfig<0,2000,5>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Orange,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<Yellow,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>,Int<400>>,ResponsiveClashL<TransitionEffect<Rgb<255,240,80>,LemonChiffon,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<10000>,Int<30000>>,Int<10000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipe<350>,TrWipeIn<1500>,Black>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "dark\nsouls"},

   { "ronin;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
      //Theory2Button.h "Ronin"
    StylePtr<Layers<AudioFlicker<RotateColorsX<Variation,Red>,RotateColorsX<Variation,Rgb<128,0,0>>>,LockupTrL<Layers<AlphaL<AudioFlickerL<White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,White,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,ResponsiveStabL<Orange,TrWipeIn<600>,TrWipe<600>>,ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>,Int<400>>,ResponsiveClashL<White,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Mix<TwistAngle<>,Rgb<255,200,0>,DarkOrange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipe<1000>,TrWipeIn<1400>,Black>>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "ronin"},

   { "deadlink;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers<
    HumpFlicker<RotateColorsX<Variation,Magenta>,RotateColorsX<Variation,Rgb<150,0,150>>,50>,
      //Magenta HumpFlicker
    AlphaL<Stripes<2500,-3000,RotateColorsX<Variation,DeepPink>,RotateColorsX<Variation,Rgb16<12482,0,18076>>,Pulsing<RotateColorsX<Variation,Rgb<10,0,26>>,Black,800>>,SwingSpeed<350>>,
      //Ripple swing effect
      //AlphaL<AudioFlickerL<Stripes<2500,-3000,RotateColorsX<Variation,DeepPink>,RotateColorsX<Variation,Rgb16<12482,0,18076>>,Pulsing<RotateColorsX<Variation,Rgb<10,0,26>>,Black,800>>>,SwingSpeed<300>>,
      //Alternate audioflicker ripple swing effect
    LockupTrL<Layers<
      AlphaL<AudioFlickerL<Rgb<255,225,0>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
      AlphaL<NavajoWhite,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrWaveX<White,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>>>,SaberBase::LOCKUP_NORMAL>,
      //Responsive lockup
    ResponsiveLightningBlockL<Strobe<White,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      //Responsive lightning block
    AlphaL<RotateColorsX<Variation,Rgb16<33124,0,65535>>,SmoothStep<Scale<SlowNoise<Int<2500>>,Int<1000>,Int<3500>>,Int<-4000>>>,
      //Sparking emitter flare
    ResponsiveStabL<Red>,
      //Responsive stab
    EffectSequence<EFFECT_BLAST,ResponsiveBlastL<White,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>,Int<400>>,LocalizedClashL<White,80,30,EFFECT_BLAST>,ResponsiveBlastWaveL<White,Scale<SwingSpeed<400>,Int<500>,Int<200>>,Scale<SwingSpeed<400>,Int<100>,Int<400>>>,BlastL<White,200,200>,ResponsiveBlastFadeL<White,Scale<SwingSpeed<400>,Int<6000>,Int<12000>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>>,ResponsiveBlastL<White,Scale<SwingSpeed<400>,Int<400>,Int<100>>,Scale<SwingSpeed<400>,Int<200>,Int<100>>,Scale<SwingSpeed<400>,Int<400>,Int<200>>>>,
      //Multi-blast, blaster reflect cycles through different responsive effects
    ResponsiveClashL<TransitionEffect<Rgb<255,225,0>,NavajoWhite,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Responsive clash
    TransitionEffectL<TrConcat<TrDelay<875>,Stripes<3000,-3500,RotateColorsX<Variation,Rgb<160,0,255>>,RandomPerLEDFlicker<RotateColorsX<Variation,Rgb<21,0,52>>,Black>,BrownNoiseFlicker<RotateColorsX<Variation,Rgb<160,0,255>>,RotateColorsX<Variation,Rgb<21,0,52>>,200>,RandomPerLEDFlicker<RotateColorsX<Variation,Rgb<42,0,105>>,RotateColorsX<Variation,Rgb<21,0,52>>>>,TrFade<250>,HumpFlicker<RotateColorsX<Variation,Magenta>,RotateColorsX<Variation,Rgb<80,0,80>>,50>,TrFade<400>,Stripes<3000,-3500,RotateColorsX<Variation,DeepPink>,RandomPerLEDFlicker<RotateColorsX<Variation,Rgb<21,0,52>>,Black>,BrownNoiseFlicker<RotateColorsX<Variation,Rgb<160,0,255>>,RotateColorsX<Variation,Rgb<21,0,52>>,200>,RandomPerLEDFlicker<RotateColorsX<Variation,Rgb<42,0,105>>,RotateColorsX<Variation,Rgb<21,0,52>>>>,TrFade<1500>>,EFFECT_IGNITION>,
      //Unstable ignition effect
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<RotateColorsX<Variation,Rgb<180,60,255>>,40>,TrFade<2000>>,EFFECT_RETRACTION>,
      //Bright HumpFlicker retraction effect
    LockupTrL<AlphaL<BrownNoiseFlickerL<White,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      //Drag
    LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      //Responsive melt
    EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<8192>>,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<24576>>,AlphaL<Black,Int<0>>>,
      //Power save, if using fett263's prop file hold Aux and click PWR while ON (pointing up) to dim blade in 25% increments.
    InOutTrL<TrConcat<TrWipeIn<200>,RandomBlink<30000,RotateColorsX<Variation,Rgb16<65535,13655,65535>>>,TrWipeIn<200>,Mix<SmoothStep<Scale<SlowNoise<Int<30000>>,Int<1000>,Int<6000>>,Int<-3000>>,Black,RotateColorsX<Variation,Rgb16<65535,13655,65535>>>,TrDelay<400>,Mix<SmoothStep<Int<4000>,Int<-3000>>,Black,White>,TrWipe<175>>,TrColorCycle<2000>>,
      //Lightning strike ignition, cycle down retraction
      //TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_BOOT>,
      //TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_NEWFONT>,
      //Optional/alternate passive battery monitor, on boot (1st line) or font change (2nd line) you will get a visual indicator at the emitter of your current battery level. This also works without a blade if you have a lit emitter or blade plug. Green is Full, Red is Low (the color will blend from Green to Red as the battery is depleted), the indicator will fade out after 3000 ms and not display again until powered down and back up or fonts change.
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<1000>>,EFFECT_BATTERY_LEVEL>
      //On demand battery level, if using fett263's prop file Hold AUX and click PWR while OFF, the battery level is represented by the location on the blade; tip = full, hilt = low and color; green = full, yellow = half, red = low
    >>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "deadlink"},

    {"decay;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
/* copyright Fett263 Greyscale (Primary Blade) OS7 Style
https://fett263.com/fett263-proffieOS7-style-library.html#Greyscale
OS7.12 v1.017-p
Single Style
Base Style: Decay (Inverted Rolling Pulse with Ripple Swing)

Base Color: BaseColorArg (0)

--Effects Included--
Preon Effect: Force Drain (Decay Version) [Color: BaseColorArg]
Ignition Effect: SparkTip Ignition [Color: IgnitionColorArg]
PowerUp Effect: Unstable Power Up Forward [Color: BaseColorArg]
Retraction Effect: SparkTip Retraction [Color: RetractionColorArg]
CoolDown Effect: Unstable Cool Down Reverse [Color: BaseColorArg]
Lockup Effect: Responsive Lockup, Begin Effect: Real Clash, End Effect: Full Blade Absorb [Color: LockupColorArg]
LightningBlock Effect: Responsive Lightning Block [Color: LBColorArg]
Drag Effect: Intensity Sparking Drag [Color: DragColorArg]
Melt Effect: Intensity Melt [Color: StabColorArg]
Stab Effect: AudioFlicker Stab [Color: StabColorArg]
Blast Effect: Blast Wave (Random) [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Emitter Effect: Emitter Flare [Color: EmitterColorArg]
Battery Level: % Blade (Green to Red)
*/
    StylePtr<Layers<Layers<StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<25746>,Black,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>>,Mix<Sin<Int<23>>,Mix<Int<19504>,Black,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>,RotateColorsX<Int<32000>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>>,Mix<Sin<Int<16>>,Mix<Int<3167>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>,Mix<Sin<Int<18>>,Mix<Int<11000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>,Mix<Int<22000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>>>,AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Black>,Black,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Black>,SwingSpeed<500>>>,AlphaL<RgbArg<EMITTER_COLOR_ARG,Rgb<255,255,255>>,SmoothStep<IntArg<EMITTER_SIZE_ARG,598>,Int<-6000>>>,TransitionEffectL<TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,Stripes<3000,-3500,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<8487>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<8487>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>,Black>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<8487>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>,Mix<Int<3855>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<8487>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<8487>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>,Mix<Int<3855>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<8487>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>>>,TrFade<800>>,EFFECT_IGNITION>,TransitionEffectL<TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,3500,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<14273>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<14273>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>,Black>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<14273>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>,Mix<Int<3855>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<14273>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<14273>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>,Mix<Int<3855>,Black,Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Mix<Int<14273>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>>>>>,TrFade<800>>,EFFECT_RETRACTION>,TransitionEffectL<TrConcat<TrJoin<TrDelayX<WavLen<>>,TrWipeIn<200>>,AlphaL<AudioFlickerL<RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<2000>>>,TrWipe<200>>,EFFECT_STAB>,TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Int<200>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Int<16000>>,30>,TrSmoothFade<600>>>,LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrConcat<TrJoin<TrDelay<4000>,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<TwistAngle<>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>,RotateColorsX<Int<3000>,Mix<Int<12000>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrJoin<TrDelay<4000>,TrFade<300>>,AlphaL<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,InOutTrL<TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Black>,TransitionEffectL<TrConcat<TrExtendX<Mult<WavLen<EFFECT_PREON>,Int<21845>>,TrInstant>,AlphaL<AlphaL<Mix<Trigger<EFFECT_PREON,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>>,BrownNoiseFlickerL<RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Int<300>>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>,HumpFlickerFX<Scale<Trigger<EFFECT_PREON,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>>,Int<30>,Int<70>>>>,SmoothStep<Scale<NoisySoundLevel,IntArg<PREON_SIZE_ARG,2000>,Scale<Trigger<EFFECT_PREON,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<6000>>,Int<40000>>>,Int<-3000>>>,TrExtend<4000,TrBoingX<Mult<WavLen<EFFECT_PREON>,Int<5324>>,9>>,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>,Mult<WavLen<EFFECT_PREON>,Int<8176>>>,BrownNoiseFlicker<Black,RotateColorsX<Int<1000>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>,100>,RandomPerLEDFlicker<RotateColorsX<Int<2000>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>,Mix<Int<16384>,Black,RotateColorsX<Int<2000>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,RotateColorsX<Int<2750>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>>>,Mix<Int<8172>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,White>,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-5000>,Int<42500>>,Int<-4000>>>,TrFadeX<Mult<WavLen<EFFECT_PREON>,Int<8176>>>>,EFFECT_PREON>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>("~ ~ ~ ~ 275"),
/* == Old style GreyScale Decay Style ==
    StylePtr<Layers< // Argified from olicomplex1.6.00.h
    StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RgbArg<BASE_COLOR_ARG,Rgb<95,0,210>>,Rgb<75,0,165>>,Mix<Sin<Int<23>>,Rgb<52,0,125>,Rgb<84,0,210>>,Mix<Sin<Int<16>>,Rgb<115,15,220>,Rgb<95,0,210>>,Mix<Sin<Int<18>>,Rgb<26,0,42>,Rgb<60,0,132>>>,
      //Bright purple absorb effect
      //StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,RotateColorsX<Variation,Rgb<95,0,210>>,RotateColorsX<Variation,Rgb<75,0,165>>>,Mix<Sin<Int<23>>,RotateColorsX<Variation,Rgb<52,0,125>>,RotateColorsX<Variation,Rgb<84,0,210>>>,Mix<Sin<Int<16>>,RotateColorsX<Variation,Rgb<115,0,220>>,RotateColorsX<Variation,Rgb<95,0,210>>>,Mix<Sin<Int<18>>,RotateColorsX<Variation,Rgb<26,0,42>>,RotateColorsX<Variation,Rgb<60,0,132>>>>,
      //Alternate darker purple absorb effect, color change friendly (for reds and such)
    AlphaL<BrownNoiseFlicker<Rgb<95,0,210>,Black,300>,SwingSpeed<400>>,
      //Unstable swing
    AlphaL<Stripes<1000,2000,RandomPerLEDFlicker<Rgb<95,0,210>,Black>,Black,Rgb<95,0,210>,Black>,SwingSpeed<500>>,
      //Ripple down swing
    AlphaL<Rgb<160,60,255>,Scale<IsLessThan<SwingSpeed<600>,Int<13600>>,Scale<SwingSpeed<600>,Int<-19300>,Int<32768>>,Int<0>>>,
      //Bright hard swings
    LockupTrL<Layers<
    AlphaL<AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Azure>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,
    AlphaL<White,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>,Int<10000>>>>,TrConcat<TrInstant,White,TrFade<400>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrWaveX<White,Int<300>,Int<100>,Int<400>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>>>,StripesX<Int<3500>,Int<1200>,Mix<Sin<Int<20>>,Rgb<95,0,210>,Rgb<75,0,165>>,Mix<Sin<Int<23>>,Rgb<52,0,125>,Rgb<84,0,210>>,Mix<Sin<Int<16>>,Rgb<115,15,220>,Rgb<95,0,210>>,Mix<Sin<Int<18>>,Rgb<26,0,42>,Rgb<60,0,132>>>,TrFade<1225>,Rgb<160,60,255>,TrFade<250>>,SaberBase::LOCKUP_NORMAL>,
      //Responsive lockup with reset
    ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<White,Blue>,50,1>,TrConcat<TrInstant,AlphaL<White,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<White,Int<16000>>,30>,TrSmoothFade<600>>>,
      //Responsive lightning block
    AlphaL<Rgb<160,60,255>,SmoothStep<Scale<SlowNoise<Int<1750>>,Int<1500>,Int<4500>>,Int<-6000>>>,
      //Sparking emitter flare
      //AlphaL<RotateColorsX<Variation,Rgb16<45025,0,65535>>,SmoothStep<Scale<SlowNoise<Int<1750>>,Int<1500>,Int<4500>>,Int<-6000>>>,
      //Alternate darker sparking emitter flare, color change friendly (for reds and such)
    ResponsiveStabL<RgbArg<STAB_COLOR_ARG,Red>>,
      //Responsive stab
    EffectSequence<EFFECT_BLAST,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Int<400>,Scale<SwingSpeed<200>,Int<100>,Int<400>>>,LocalizedClashL<RgbArg<BLAST_COLOR_ARG,White>,80,30,EFFECT_BLAST>,ResponsiveBlastWaveL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<500>,Int<200>>,Scale<SwingSpeed<400>,Int<100>,Int<400>>>,BlastL<RgbArg<BLAST_COLOR_ARG,White>,200,200>,ResponsiveBlastFadeL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<6000>,Int<12000>>,Scale<SwingSpeed<400>,Int<400>,Int<100>>>,ResponsiveBlastL<RgbArg<BLAST_COLOR_ARG,White>,Scale<SwingSpeed<400>,Int<400>,Int<100>>,Scale<SwingSpeed<400>,Int<200>,Int<100>>,Scale<SwingSpeed<400>,Int<400>,Int<200>>>>,
      //Multi-blast, blaster reflect cycles through different responsive effects
    ResponsiveClashL<TransitionEffect<RgbArg<CLASH_COLOR_ARG,Azure>,White,TrInstant,TrFade<100>,EFFECT_CLASH>,TrInstant,TrFade<400>,Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,Int<6000>,Int<20000>>,
      //Responsive clash
    TransitionEffectL<TrConcat<TrInstant,Stripes<3000,-3500,Rgb<160,60,255>,RandomPerLEDFlicker<Rgb<60,60,60>,Black>,BrownNoiseFlicker<Rgb<175,80,255>,Rgb<30,30,30>,200>,RandomPerLEDFlicker<Rgb<80,80,80>,Rgb<30,30,30>>>,TrFade<1650>>,EFFECT_IGNITION>,
      //Bright unstable ignition
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<Rgb<160,60,255>,40>,TrFade<1200>>,EFFECT_RETRACTION>,
      //Bright HumpFlicker retraction
    LockupTrL<AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,SmoothStep<Int<30000>,Int<5000>>>,TrWipeIn<400>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
      //Drag
    LockupTrL<AlphaL<Mix<TwistAngle<>,Red,Orange>,SmoothStep<Int<28000>,Int<5000>>>,TrWipeIn<600>,TrFade<300>,SaberBase::LOCKUP_MELT>,
      //Responsive melt
    EffectSequence<EFFECT_POWERSAVE,AlphaL<Black,Int<8192>>,AlphaL<Black,Int<16384>>,AlphaL<Black,Int<24576>>,AlphaL<Black,Int<0>>>,
      //Power save, if using fett263's prop file hold Aux and click PWR while ON (pointing up) to dim blade in 25% increments.
    InOutTrL<TrWipeInSparkTip<White,200>,TrWipeInSparkTip<White,1125>>,
      //Spark tip in ignition and retraction
      //InOutTrL<TrWipeSparkTip<White,200>,TrWipeInSparkTip<White,1125>>,
      //Alternate spark tip ignition and retraction
      //TransitionEffectL<TrConcat<TrDelay<1500>,Black,TrFade<1000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_BOOT>,
      //TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Int<0>,Int<6000>>>,TrFade<3000>>,EFFECT_NEWFONT>,
      //Optional/alternate passive battery monitor, on boot (1st line) or font change (2nd line) you will get a visual indicator at the emitter of your current battery level. This also works without a blade if you have a lit emitter or blade plug. Green is Full, Red is Low (the color will blend from Green to Red as the battery is depleted), the indicator will fade out after 3000 ms and not display again until powered down and back up or fonts change.
    TransitionEffectL<TrConcat<TrInstant,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrDelay<2000>,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<BatteryLevel,Int<10000>>>,TrFade<1000>>,EFFECT_BATTERY_LEVEL>,
      //On demand battery level, if using fett263's prop file Hold AUX and click PWR while OFF, the battery level is represented by the location on the blade; tip = full, hilt = low and color; green = full, yellow = half, red = low
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<95,0,210>>,40>,TrWipe<150>,HumpFlickerL<Rgb<95,0,210>,40>,TrBoing<350,2>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<95,0,210>>,30>,Bump<Int<0>,Int<5000>>>,TrFade<625>,AlphaL<HumpFlickerL<Rgb<95,0,210>,30>,Bump<Int<0>,Int<9000>>>,TrFade<625>,AlphaL<HumpFlickerL<Rgb<95,0,210>,40>,Bump<Int<0>,Int<13000>>>,TrFade<500>,AlphaL<HumpFlickerL<Rgb<95,0,210>,69>,Bump<Int<0>,Int<18000>>>,TrFade<750>,AlphaL<HumpFlickerL<Rgb<95,0,210>,69>,Bump<Int<0>,Int<23000>>>,TrFade<750>,AlphaL<HumpFlickerL<Rgb<95,0,210>,30>,Bump<Int<0>,Int<7000>>>,TrFade<500>,AlphaL<HumpFlickerL<Rgb<95,0,210>,50>,Bump<Int<0>,Int<19000>>>,TrBoing<1950,9>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrInstant,TransitionLoopL<TrWaveX<RgbArg<PREON_COLOR_ARG,Rgb<95,0,210>>,Int<500>,Int<200>,Int<400>,Int<32768>>>,TrDelay<1250>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<300>,Int<300>,Int<300>,Int<32768>>>,TrDelay<1000>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<300>,Int<315>,Int<215>,Int<32768>>>,TrDelay<1000>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<115>,Int<375>,Int<115>,Int<32768>>>,TrDelay<1250>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<300>,Int<300>,Int<300>,Int<32768>>>,TrDelay<1500>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<100>,Int<400>,Int<100>,Int<32768>>>,TrDelay<1750>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<300>,Int<300>,Int<300>,Int<32768>>>,TrDelay<2000>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<300>,Int<315>,Int<215>,Int<32768>>>,TrDelay<450>,TransitionLoopL<TrWaveX<Rgb<95,0,210>,Int<100>,Int<400>,Int<100>,Int<32768>>>,TrDelay<1450>,AlphaL<Rgb<95,0,210>,Bump<Int<0>,Int<6000>>>,TrBoing<1450,5>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrDelay<4350>,AlphaL<Mix<Trigger<EFFECT_PREON,Int<3000>,Int<5000>,Int<3000>>,BrownNoiseFlicker<RgbArg<PREON_COLOR_ARG,Black>,Rgb16<33124,0,65535>,100>,RandomPerLEDFlicker<Rgb16<56306,0,65535>,Rgb16<21919,0,22863>>,BrownNoiseFlicker<Mix<NoisySoundLevel,Rgb16<33124,0,65535>,RotateColorsX<Int<2750>,Rgb16<33124,0,65535>>>,Rgb16<42753,14386,65535>,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-5000>,Int<42500>>,Int<-4000>>>,TrDelay<7750>>,EFFECT_PREON>
      //Life force drain preon
    >>(),
   == End of old style GreyScale Decay Style ==
*/
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "decay"},

   { "decimate;common", "common/tracks/mercury.wav",
    // === Main Blade: ===
/* copyright Fett263 KyloRen (Primary Blade) OS7 Style
https://www.fett263.com/fett263-proffieOS7-style-library.html#KyloRen
OS7.14 v2.52p
This Style Contains 17280 Unique Combinations
Style Options:
Default (0): Kylo Ren Unstable (Film Based)
1: Kylo Ren Unstable (Supreme Leader)

Base Color: BaseColorArg (0)

--Effects Included--
Preon Effect Options: Default (0): Disable, 1: Faulty Ignition, 2: Faulty Ignition (Volatile Version), 3: Rey TROS Spark, 4: Sparking, 5: Overload [Color: PreonColorArg]
Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Down = Fast) [Color: IgnitionColorArg]
PowerUp Effect Options: Default (0): Disable, 1: Power Flash, 2: Rey TROS Spark, 3: Unstable Power Up Forward, 4: Power Surge (Unstable V1) [Color: IgnitionColorArg]
Retraction Effect Options: Default (0): Standard Retraction, 1: SparkTip Retraction, 2: Dual Mode Wipe In (Up = Fast), 3: Glitch Off [Color: RetractionColorArg]
CoolDown Effect Options: Default (0): Disable, 1: Power Flash, 2: Power Burst Reverse, 3: Unstable Cool Down Reverse [Color: RetractionColorArg]
PostOff Effect Options: Default (0): Disable, 1: Battery Glow (Preon Size), 2: Emitter Glow (Preon Size) [Color: PostOffColorArg]
Lockup Effect:
0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
[Color: LockupColorArg]
Lightning Block Effect:
0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
[Color: LBColorArg]
Drag Effect:
0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
Blast Effect (Randomly Selected): Blast Wave (Random), Responsive Blast Wave (Random), Responsive Blast Fade (Random), Responsive Blast Ripple Fade [Color: BlastColorArg]
Clash Effect: Real Clash V1 [Color: ClashColorArg]
Swing Effect Options: Default (0): Disable, 1: Interactive Power Build-up (Force Toggle) [Color: SwingColorArg]
Battery Level: % Blade (Green to Red)
Display Volume: % Blade [Color: BaseColorArg]
Battery Monitor: Passive Battery Monitor (Retraction)
Swing Option 1- Interactive Power Build-up (Force Toggle) Interactive Control: Force to enable/disable, hold steady to charge up Power and swing to throw.
*/
    //StylePtr<Layers<ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,RandomPerLEDFlicker<Mix<Int<3213>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,300>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,0,6,10,1000,2>,StripesX<Int<1500>,Scale<SlowNoise<Int<2500>>,Int<-3000>,Int<-5000>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,Mix<Int<2570>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>,RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>>>,ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,ColorSelect<IncrementF<EffectPulseF<EFFECT_FORCE>,Int<32000>,Int<2>>,TrSmoothFade<500>,AlphaL<RgbArg<BASE_COLOR_ARG,Red>,Int<0>>,AlphaL<StripesX<Int<10000>,Scale<IncrementWithReset<ThresholdPulseF<Ifon<StrobeF<Int<12>,Int<1>>,Int<0>>,Int<32000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>>,Int<0>>,Int<24000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<10000>,Int<100>>,Int<-50>,Int<-8000>>,Mix<Int<10000>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<20000>,Black,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,RgbArg<SWING_COLOR_ARG,Rgb<255,255,255>>>,SmoothStep<Sum<IncrementWithReset<ThresholdPulseF<StrobeF<Int<10>,Int<1>>,Int<30000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<4000>,Int<100>>,IncrementWithReset<ThresholdPulseF<Ifon<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>,Int<0>>,Int<16000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<38000>,Int<38000>>>,Int<-2000>>>>>,TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelay<200>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<300>,AlphaL<Blue,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<500>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<300>>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<1200>>>,EFFECT_IGNITION>,TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<0>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>>>,TrFade<800>>>,EFFECT_RETRACTION>,TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaMixL<Bump<Scale<BladeAngle<>,Int<28000>,Int<8000>>,Scale<EffectPosition<EFFECT_BLAST>,Int<9000>,Int<13000>>>,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>,TrFade<300>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,IgnitionTime<300>,Mult<IgnitionTime<300>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<0>,Int<16384>>,RetractionTime<0>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<0>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,Rgb<255,255,255>>>,TrWipeInX<Mult<RetractionTime<0>,Int<16384>>>>>,Black>,TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<Black,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Rgb<50,50,50>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,RotateColorsX<Int<4000>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>>,White,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<Black,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<6425>,Black,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>>,RotateColorsX<Int<21845>,RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>>,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrJoin<TrDelayX<Int<100>>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrWipe<100>,AlphaL<DodgerBlue,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrDelayX<Sum<WavLen<EFFECT_PREON>,Int<200>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,-200>,Sum<IntArg<PREON_SIZE_ARG,-200>,Int<4000>>>,Int<-2000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,10>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,15>,SmoothStep<Sum<Int<2000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,Rgb<255,255,255>>,20>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrBoingX<Mult<Int<8192>,WavLen<EFFECT_PREON>>,3>>>,EFFECT_PREON>,TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelayX<Int<500>>,TrFade<500>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFade<500>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,Rgb<255,255,255>>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>>,EFFECT_POSTOFF>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>,TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    StylePtr<Layers< //Same as above but "Layerized"
      ColorSelect<IntArg<STYLE_OPTION_ARG,0>,TrInstant,StaticFire<BrownNoiseFlicker<RgbArg<BASE_COLOR_ARG,Red>,RandomPerLEDFlicker<Mix<Int<3213>,Black,RgbArg<BASE_COLOR_ARG,Red>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Red>>>,300>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Red>>,0,6,10,1000,2>,StripesX<Int<1500>,Scale<SlowNoise<Int<2500>>,Int<-3000>,Int<-5000>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<10280>,Black,RgbArg<BASE_COLOR_ARG,Red>>,RgbArg<BASE_COLOR_ARG,Red>,Mix<Int<2570>,Black,RgbArg<BASE_COLOR_ARG,Red>>,RgbArg<BASE_COLOR_ARG,Red>>>,
      ColorSelect<IntArg<SWING_OPTION_ARG,0>,TrInstant,AlphaL<Black,Int<0>>,ColorSelect<IncrementF<EffectPulseF<EFFECT_FORCE>,Int<32000>,Int<2>>,TrSmoothFade<500>,AlphaL<RgbArg<BASE_COLOR_ARG,Red>,Int<0>>,AlphaL<StripesX<Int<10000>,Scale<IncrementWithReset<ThresholdPulseF<Ifon<StrobeF<Int<12>,Int<1>>,Int<0>>,Int<32000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>>,Int<0>>,Int<24000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<10000>,Int<100>>,Int<-50>,Int<-8000>>,Mix<Int<10000>,Black,RgbArg<SWING_COLOR_ARG,White>>,Mix<Int<20000>,Black,RgbArg<SWING_COLOR_ARG,White>>,RgbArg<SWING_COLOR_ARG,White>>,SmoothStep<Sum<IncrementWithReset<ThresholdPulseF<StrobeF<Int<10>,Int<1>>,Int<30000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<4000>,Int<100>>,IncrementWithReset<ThresholdPulseF<Ifon<HoldPeakF<SwingSpeed<400>,Int<100>,Int<33000>>,Int<0>>,Int<16000>>,ThresholdPulseF<Sum<ThresholdPulseF<Ifon<InvertF<HoldPeakF<SwingSpeed<400>,Int<100>,Int<38000>>>,Int<0>>,Int<16000>>,EffectPulseF<EFFECT_FORCE>>,Int<32000>>,Int<38000>,Int<38000>>>,Int<-2000>>>>>,
      TransitionEffectL<TrSelect<IntArg<IGNITION_POWER_UP_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,RgbArg<IGNITION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelay<200>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<300>,AlphaL<Blue,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-500>>>,TrFade<500>>,TrConcat<TrJoin<TrDelayX<IgnitionTime<>>,TrInstant>,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<800>>,TrConcat<TrInstant,Stripes<3000,-3500,RgbArg<IGNITION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<IGNITION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<16384>,Black,RgbArg<IGNITION_COLOR_ARG,White>>,Mix<Int<7710>,Black,RgbArg<IGNITION_COLOR_ARG,White>>>>,TrFade<1200>>>,EFFECT_IGNITION>,
      TransitionEffectL<TrSelect<IntArg<RETRACTION_COOL_DOWN_ARG,0>,TrInstant,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,RgbArg<RETRACTION_COLOR_ARG,White>,TrFade<1000>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<5000,2500,RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>,TrFade<800>>,TrConcat<TrJoin<TrDelayX<RetractionTime<>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,White>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,White>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>>>,TrFade<800>>>,EFFECT_RETRACTION>,
      TransitionEffectL<TrRandom<TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,TrWaveX<RgbArg<BLAST_COLOR_ARG,White>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<BladeAngle<>,Int<28000>,Int<8000>>>,TrConcat<TrInstant,AlphaMixL<Bump<Scale<BladeAngle<>,Int<28000>,Int<8000>>,Scale<EffectPosition<EFFECT_BLAST>,Int<9000>,Int<13000>>>,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<16384>,Black,RgbArg<BLAST_COLOR_ARG,White>>>,TrFade<300>>,TrConcat<TrInstant,AlphaL<RgbArg<BLAST_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<6000>>>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>,AlphaL<Remap<CenterDistF<Scale<BladeAngle<>,Int<3000>,Int<29000>>>,Stripes<1600,-2000,RgbArg<BLAST_COLOR_ARG,White>,Mix<Int<2096>,Black,RgbArg<BLAST_COLOR_ARG,White>>>>,Bump<Scale<BladeAngle<>,Int<3000>,Int<29000>>,Int<32000>>>,TrJoin<TrSmoothFade<50>,TrCenterWipeX<Int<100>,Scale<BladeAngle<>,Int<3000>,Int<29000>>>>>>,EFFECT_BLAST>,
      Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,White>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,
      LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,White>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,White>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,White>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
      ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,White>,AudioFlicker<RgbArg<LB_COLOR_ARG,White>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,White>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,White>,TrFade<400>>>,
      LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,White>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,White>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG>,
      LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,DarkOrange>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,DarkOrange>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
      InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,BendTimePowInvX<IgnitionTime<>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,IgnitionTime<>,Mult<IgnitionTime<>,Int<16384>>>>>,TrSelect<IntArg<RETRACTION_OPTION_ARG,0>,TrWipeInX<BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,White>,BendTimePowInvX<RetractionTime<>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeInX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,Mult<RetractionTime<>,Int<16384>>,RetractionTime<>>>,TrConcat<TrJoin<TrDelayX<Mult<RetractionTime<>,Int<16384>>>,TrWipeInX<Mult<RetractionTime<>,Int<16384>>>>,Mix<SmoothStep<NoisySoundLevel,Int<-1>>,Black,RgbArg<RETRACTION_COLOR_ARG,White>>,TrWipeInX<Mult<RetractionTime<>,Int<16384>>>>>>,
      TransitionEffectL<TrSelect<IntArg<PREON_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<RgbArg<PREON_COLOR_ARG,Black>,RgbArg<PREON_COLOR_ARG,White>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,White>,Rgb<50,50,50>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,White>,RotateColorsX<Int<4000>,RgbArg<PREON_COLOR_ARG,White>>>,White,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrInstant,AlphaL<Mix<Trigger<EFFECT_PREON,Mult<Int<16384>,WavLen<EFFECT_PREON>>,Mult<Int<10922>,WavLen<EFFECT_PREON>>,Mult<Int<5462>,WavLen<EFFECT_PREON>>>,BrownNoiseFlicker<Black,RgbArg<PREON_COLOR_ARG,White>,100>,RandomPerLEDFlicker<RgbArg<PREON_COLOR_ARG,White>,Mix<Int<6425>,Black,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,White>>>>,BrownNoiseFlicker<Mix<NoisySoundLevel,RgbArg<PREON_COLOR_ARG,White>,RotateColorsX<Int<26000>,RgbArg<PREON_COLOR_ARG,White>>>,RotateColorsX<Int<21845>,RgbArg<PREON_COLOR_ARG,White>>,50>>,SmoothStep<Scale<NoisySoundLevel,Int<-10000>,Int<36000>>,Int<-4000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrJoin<TrDelayX<Int<100>>,TrInstant>,AlphaL<Green,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrWipe<100>,AlphaL<DodgerBlue,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrDelayX<Sum<WavLen<EFFECT_PREON>,Int<200>>>>,TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,-200>,Sum<IntArg<PREON_SIZE_ARG,-200>,Int<4000>>>,Int<-2000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,TrConcat<TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,White>,10>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,White>,15>,SmoothStep<Sum<Int<2000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrFadeX<Mult<Int<8192>,WavLen<EFFECT_PREON>>>,AlphaL<HumpFlickerL<RgbArg<PREON_COLOR_ARG,White>,20>,SmoothStep<Sum<Int<4000>,IntArg<PREON_SIZE_ARG,-200>>,Int<-4000>>>,TrBoingX<Mult<Int<8192>,WavLen<EFFECT_PREON>>,3>>>,EFFECT_PREON>,
      TransitionEffectL<TrSelect<IntArg<OFF_OPTION_ARG,0>,TrInstant,TrConcat<TrInstant,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,Int<0>>,TrJoin<TrDelayX<Int<500>>,TrFade<500>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-4000>>>,TrFade<500>>,TrConcat<TrInstant,AlphaL<White,Int<0>>,TrJoin<TrDelay<1000>,TrInstant>,AlphaL<RgbArg<POSTOFF_COLOR_ARG,White>,SmoothStep<IntArg<PREON_SIZE_ARG,-200>,Int<-2000>>>,TrSmoothFadeX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>>,EFFECT_POSTOFF>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>,
      TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrInstant>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-4000>>>,TrFade<300>>,EFFECT_RETRACTION>,
      TransitionEffectL<TrConcat<TrExtend<2000,TrWipe<100>>,AlphaL<RgbArg<BASE_COLOR_ARG,Red>,SmoothStep<VolumeLevel,Int<-1>>>,TrFade<300>>,EFFECT_VOLUME_LEVEL>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "decimate\nwhite"},

   { "dragon_witch;common", "common/tracks/venus.wav",
    // === Main Blade: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<EASYBLADE(Purple,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "dragon_witch\nmagenta"},

   { "EngineGrip;common", "common/tracks/mercury.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleRainbowPtr<300,800>(),
      Rainbow,
      LockupL<AudioFlicker<Rainbow,RgbArg<LOCKUP_COLOR_ARG,White>>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleRainbowPtr<300,800>(),
      Rainbow,
      LockupL<AudioFlicker<Rainbow,RgbArg<LOCKUP_COLOR_ARG,White>>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleRainbowPtr<300,800>(),
      Rainbow,
      LockupL<AudioFlicker<Rainbow,RgbArg<LOCKUP_COLOR_ARG,White>>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleRainbowPtr<300,800>(),
      Rainbow,
      LockupL<AudioFlicker<Rainbow,RgbArg<LOCKUP_COLOR_ARG,White>>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "enginegrip\nrainbow"},

   { "exalted;common", "common/tracks/uranus.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleStrobePtr<WHITE,Rainbow,15,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Black>,
        StrobeL<White,Int<15>,Int<1>>>,
      LockupL<Strobe<RgbArg<LOCKUP_COLOR_ARG,Black>,White,45,1>>,
      SimpleClashL<Rainbow>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleStrobePtr<WHITE,Rainbow,15,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Black>,
        StrobeL<White,Int<15>,Int<1>>>,
      LockupL<Strobe<RgbArg<LOCKUP_COLOR_ARG,Black>,White,45,1>>,
      SimpleClashL<Rainbow>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleStrobePtr<WHITE,Rainbow,15,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Black>,
        StrobeL<White,Int<15>,Int<1>>>,
      LockupL<Strobe<RgbArg<LOCKUP_COLOR_ARG,Black>,White,45,1>>,
      SimpleClashL<Rainbow>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleStrobePtr<WHITE,Rainbow,15,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Black>,
        StrobeL<White,Int<15>,Int<1>>>,
      LockupL<Strobe<RgbArg<LOCKUP_COLOR_ARG,Black>,White,45,1>>,
      SimpleClashL<Rainbow>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "exalted\nstrobe"},

   {"LiquidStatic;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    /* copyright Fett263 SmokeBlade (Primary Blade) OS6 Style
https://fett263.com/fett263-proffieOS6-style-library.html#SmokeBlade
OS6.7 v4.014
Single Style
Base Style: Smoke Blade

Base Color: Rgb<0,0,255> (0)

--Effects Included--
Preon Effect: Sparking (PreOn Color)
Ignition Effect: SparkTip Ignition
PowerUp Effect: Power Surge (Unstable V2)
Retraction Effect: SparkTip Retraction
CoolDown Effect: Unstable Cool Down Reverse
PostOff Effect: Emitter Spark (Preon Size)
Lockup Effect: Responsive Lockup, Begin Effect: Real Clash, End Effect: Full Blade Absorb
LightningBlock Effect: Original Lightning Block
Drag Effect: Intensity Sparking Drag
Melt Effect: Intensity Melt
Stab Effect: Normal Stab
Blast Effect: Blast Wave (Random)
Clash Effect: Flash on Clash (Full Blade)
Emitter Effect: Emitter Flare
*/
    StylePtr<Layers<
    StripesX<Sin<Int<12>,Int<3000>,Int<7000>>,Scale<SwingSpeed<100>,Int<75>,Int<125>>,StripesX<Sin<Int<10>,Int<1000>,Int<3000>>,Scale<SwingSpeed<100>,Int<75>,Int<100>>,Pulsing<RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<2570>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,1200>,Mix<SwingSpeed<200>,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,Black>>,Mix<Int<7710>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,Pulsing<Mix<Int<6425>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,StripesX<Sin<Int<10>,Int<2000>,Int<3000>>,Sin<Int<10>,Int<75>,Int<100>>,RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<12000>,Black,RgbArg<BASE_COLOR_ARG,Blue>>>,2000>,Pulsing<Mix<Int<16448>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,Mix<Int<642>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,3000>>,
    AlphaL<StaticFire<RgbArg<BASE_COLOR_ARG,Blue>,Mix<Int<256>,Black,RgbArg<BASE_COLOR_ARG,Blue>>,0,1,10,2000,2>,Int<10000>>,
    AlphaL<RgbArg<EMITTER_COLOR_ARG,LightCyan>,SmoothStep<IntArg<EMITTER_SIZE_ARG,2000>,Int<-6000>>>,
    TransitionEffectL<TrConcat<TrInstant,HumpFlickerL<RgbArg<IGNITION_COLOR_ARG,White>,40>,TrFade<1200>>,EFFECT_IGNITION>,
    TransitionEffectL<TrConcat<TrJoin<TrDelayX<RetractionTime<2100>>,TrInstant>,Stripes<3000,3500,RgbArg<RETRACTION_COLOR_ARG,Blue>,RandomPerLEDFlicker<Mix<Int<7710>,Black,RgbArg<RETRACTION_COLOR_ARG,Blue>>,Black>,BrownNoiseFlicker<RgbArg<RETRACTION_COLOR_ARG,Blue>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Blue>>,200>,RandomPerLEDFlicker<Mix<Int<3137>,Black,RgbArg<RETRACTION_COLOR_ARG,Blue>>,Mix<Int<3855>,Black,RgbArg<RETRACTION_COLOR_ARG,Blue>>>>,TrFade<800>>,EFFECT_RETRACTION>,
    TransitionEffectL<TrConcat<TrJoin<TrDelayX<WavLen<>>,TrWipeIn<200>>,AlphaL<RgbArg<STAB_COLOR_ARG,Red>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<2000>>>,TrWipe<200>>,EFFECT_STAB>,
    MultiTransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Aquamarine>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,
    TransitionEffectL<TrConcat<TrJoin<TrDelay<30>,TrInstant>,RgbArg<CLASH_COLOR_ARG,DeepSkyBlue>,TrFade<300>>,EFFECT_CLASH>,
    LockupTrL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,AudioFlickerL<RgbArg<LOCKUP_COLOR_ARG,Cyan>>,BrownNoiseFlickerL<RgbArg<LOCKUP_COLOR_ARG,Cyan>,Int<200>>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Cyan>,TrFade<400>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Cyan>,TrFade<400>>,SaberBase::LOCKUP_NORMAL>,
    LockupTrL<AlphaL<RgbArg<LB_COLOR_ARG,Cyan>,LayerFunctions<Bump<Scale<SlowNoise<Int<2000>>,Int<3000>,Int<16000>>,Scale<BrownNoiseF<Int<10>>,Int<14000>,Int<8000>>>,Bump<Scale<SlowNoise<Int<2300>>,Int<26000>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<10000>>>,Bump<Scale<SlowNoise<Int<2300>>,Int<20000>,Int<30000>>,Scale<IsLessThan<SlowNoise<Int<1500>>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<0>>,Int<0>>>>>,TrConcat<TrInstant,AlphaL<RgbArg<LB_COLOR_ARG,Cyan>,Bump<Int<12000>,Int<18000>>>,TrFade<200>>,TrConcat<TrInstant,HumpFlickerL<AlphaL<RgbArg<LB_COLOR_ARG,Cyan>,Int<16000>>,30>,TrSmoothFade<600>>,SaberBase::LOCKUP_LIGHTNING_BLOCK>,
    LockupTrL<AlphaL<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,LightCyan>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrConcat<TrJoin<TrDelay<4000>,TrWipeIn<200>>,AlphaL<BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,LightCyan>,Int<300>>,SmoothStep<IntArg<DRAG_SIZE_ARG,28000>,Int<3000>>>,TrFade<4000>>,TrFade<300>,SaberBase::LOCKUP_DRAG>,
    LockupTrL<AlphaL<Remap<Scale<RampF,Int<65536>,Int<0>>,StaticFire<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>,Mix<TwistAngle<>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>,RotateColorsX<Int<3000>,Mix<Int<12000>,Black,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>>,0,3,5,3000,10>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrConcat<TrWipeIn<100>,AlphaL<RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrJoin<TrDelay<4000>,TrFade<300>>,AlphaL<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,24,0>>>>,SmoothStep<IntArg<MELT_SIZE_ARG,28000>,Int<4000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT>,
    InOutTrL<TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,White>,IgnitionTime<>>,TrWipeInSparkTipX<RgbArg<RETRACTION_COLOR_ARG,Blue>,RetractionTime<>>>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<PREON_COLOR_ARG,White>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<4000>>>,Int<-2000>>>,TrDelayX<WavLen<EFFECT_PREON>>>,EFFECT_PREON>,
    TransitionEffectL<TrConcat<TrInstant,AlphaL<BrownNoiseFlickerL<RgbArg<POSTOFF_COLOR_ARG,White>,Int<30>>,SmoothStep<Scale<SlowNoise<Int<2000>>,IntArg<PREON_SIZE_ARG,2000>,Sum<IntArg<PREON_SIZE_ARG,2000>,Int<3000>>>,Int<-4000>>>,TrDelayX<Scale<IsLessThan<WavLen<EFFECT_POSTOFF>,Int<200>>,WavLen<EFFECT_POSTOFF>,Int<4000>>>>,EFFECT_POSTOFF>>>(),
    // === NPXL inner ring: ===
    StylePtr<ColorCycle<BLACK,1,0,BLUE,99,1200,4000>>(), //"Tron"
    // === NPXL outer ring: ===
    StylePtr<ColorCycle<OrangeRed,1,1,EasyBlade<Rainbow,WHITE>,100,1200,10000>>(), //"Rainbow Tron"
    // === Crystal Chamber: ===
    StylePtr<Layers<Layers<Green,RandomL<Black>>,InOutHelperL<InOutFuncX<Int<300>,Int<800>>,Pulsing<Blue,Black,2000>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "liquid\nstatic"},

/* === SIMPLEST TEST STYLES ===
    { "common", "",
    // === Main Blade: ===
    StylePtr<Green>(),
    // === NPXL inner ring: ===
    StylePtr<Blue>(),
    // === NPXL outer ring: ===
    StylePtr<Purple>(),
    // === Crystal Chamber: ===
    StylePtr<Red>(),
    // === Bluetooth: ===
    StylePtr<Black>(),
    "simple\n test"},
*/

/* === EMPTY TEMPLATE ===
   {";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    ,
    // === NPXL inner ring: ===
    ,
    // === NPXL outer ring: ===
    ,
    // === Crystal Chamber: ===
    ,
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},
*/
/* === EXAMPLE EMPTY TEMPLATE ===
   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<CYAN,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Cyan>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Cyan>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<CYAN,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Cyan>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Cyan>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<CYAN,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Cyan>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Cyan>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<CYAN,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Cyan>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Cyan>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Blue>,White>,AudioFlicker<Blue,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(BLUE,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RED,YELLOW,3>>(), // was StyleFirePtr<RED,YELLOW,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<RED,WHITE,300,800>(),
      RgbArg<BASE_COLOR_ARG,Red>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Red>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,0>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,1>(),
    // === NPXL outer ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutHelper<EASYBLADE(OnSpark<GREEN>,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<AudioFlicker<YELLOW,WHITE>,BLUE,300,800>(),
      Layers<
        RgbArg<BASE_COLOR_ARG,Yellow>,
        AudioFlickerL<White>>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Yellow>,White>,White>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Blue>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === NPXL outer ring: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Crystal Chamber: ===
    StylePtr<InOutSparkTip<SimpleClash<Lockup<Blast<RgbArg<BASE_COLOR_ARG,Magenta>,White>,AudioFlicker<Magenta,White>>,White>,300,800>>(), // was StylePtr<InOutSparkTip<EASYBLADE(MAGENTA,WHITE),300,800>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},

   { ";common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    ""},
*/

/*
   { "TeensySF;common", "common/tracks/venus.wav",       //
    // ============        Persistance Of Vision         //
    // === POV: ===        =====================         //
    // ============ https://pod.hubbe.net/tools/pov.html //
    // === Main Blade: ===                               //
    &style_pov,                                          //
    // === NPXL inner ring: ===                          //
    StylePtr<Black>(),                                   // Disable this to save memory
    // === NPXL outer ring: ===                          //
    StylePtr<Black>(),                                   //
    // === Crystal Chamber: ===                          //
    StylePtr<Black>(),                                   //
    // === Bluetooth: === (fake blade)                   //
    StylePtr<Blue>(),                                    //
    "pov"},                                              //
*/

/*
   { "common", ""
    // =================================
    // === Main Blade Length Finder: === Change the blade config to 144 LEDs: WS281XBladePtr<144,bladePin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>(),
    // =================================
    // https://pod.hubbe.net/howto/how-many-leds-does-my-blade-have.html
    StylePtr<LengthFinder<>>(),
    // === NPXL inner ring: ===
    StylePtr<Black>(),
    // === NPXL outer ring: ===
    StylePtr<Black>(),
    // === Crystal Chamber: ===
    StylePtr<Black>(),
    // === Bluetooth: === (fake blade)
    StylePtr<Blue>(),
    "length\nfinder"},
*/

// === Battery Level / Keep as last preset ===
   { "Zombie_Chainsaw;common", "",
    // === Main Blade: ===
    StylePtr<BatteryLevelStyle>(),
    // === NPXL inner ring: ===
    StylePtr<BatteryLevelStyle>(),
    // === NPXL outer ring: ===
    &style_charging,                                    // Charging style but I have no charge port
    // === Crystal Chamber: ===
/* copyright 2022 NoSloppy (BRIAN CONNER) OS6 Style https://crucible.hubbe.net/t/first-time-with-subblades-does-my-blade-define-look-right/1301/26
OS6.5 v1.017
This Style Contains 1 Unique Combination
Style Options:
Default (0): Boring ass battery meter blade (Original TeensySaberV3)
--Effects Included--
Ignition Effect Options: Default:  Boring battery bump
Retraction Effect: Standard Retraction
Lockup Effect: n/a
LightningBlock Effect: n/a
Drag Effect: n/a
Melt Effect: n/a
Stab Effect: n/a
Blast Effect: n/a
Clash Effect: n/a
*/
    StylePtr<Layers<Black,AlphaL<Mix<BatteryLevel,Red,Green>,Bump<Scale<BatteryLevel,Int<0>,Int<35000>>,Int<10000>>>>>(), //"charging2.0"
    //StylePtr<Black>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    " battery\nlevel 2.0"},

};

/*********************************\
|*                               *|
|* == Blaster sound font list == *|
|*                               *|
\*********************************/

/*
   { "_BlasterMode/DC-17;_common-Blaster;common", "common/tracks/mandotheme.wav",
    "dc-17"},

   { "_BlasterMode/E-11;_common-Blaster;common", "common/tracks/mandotheme.wav",
    "e-11"},

   { "_BlasterMode/DL-44;_common-Blaster;common", "common/tracks/mandotheme.wav",
    "dl-44-ghostbusters"},

   { "_BlasterMode/Bowcaster;_common-Blaster;common", "common/tracks/mandotheme.wav",
    "bowcaster"},

   { "_BlasterMode/Droideka;_common-Blaster;common", "common/tracks/mandotheme.wav",
    "droideka"},
*/

/******************************\
|*                            *|
|* == Blaster presets list == *|
|*                            *|
\******************************/

Preset presets_blaster[] = {
   { "_BlasterMode/DC-17;_common-Blaster;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "dc-17"},

   { "_BlasterMode/E-11;_common-Blaster;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "e-11"},

   { "_BlasterMode/DL-44;_common-Blaster;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    /* copyright Fett263 Ghostbusters (Primary Blade) OS7 Style
     * https://www.fett263.com/fett263-proffieOS7-style-library.html#Ghostbusters
     * OS7.14 v2.52p
     * This Style Contains 4 Unique Combinations
     * Base Style: Ghostbusters Particle Beam (Original)
     *
     * Base Color: BaseColorArg (0)
     *
     * --Effects Included--
     * Ignition Effect Options: Default (0): Standard Ignition, 1: SparkTip Ignition, 2: Dual Mode Wipe (Down = Fast), 3: Multi-Point Out [Color: IgnitionColorArg]
     * Retraction Effect: Standard Retraction [Color: RetractionColorArg]
     * Lockup Effect:
     * 0: mainLockMulti0Shape - Begin: Real Clash - Style: Intensity AudioFlicker - End: Full Blade Absorb
     * [Color: LockupColorArg]
     * Lightning Block Effect:
     * 0: mainLBMulti0Shape - Begin: Responsive Impact - Style: Strobing AudioFlicker - End: Full Blade Absorb
     * [Color: LBColorArg]
     * Drag Effect:
     * 0: mainDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
     * [Color: DragColorArg]
     * Melt Effect:
     * 0: mainMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
     * [Color: StabColorArg]
     * Blast Effect: Blast Wave (Random) [Color: BlastColorArg]
     * Clash Effect: Real Clash V1 [Color: ClashColorArg]
     * Battery Level: % Blade (Green to Red)
     */
    StylePtr<Layers<StaticFire<Mix<SmoothStep<Int<2000>,Int<-2000>>,Stripes<16000,-3900,RgbArg<BASE_COLOR_ARG,Rgb<255,14,0>>,Mix<Int<8172>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,14,0>>>,Mix<Int<16384>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,14,0>>>,Stripes<2500,-3500,RgbArg<BASE_COLOR_ARG,Rgb<255,14,0>>,RgbArg<BASE_COLOR_ARG,Rgb<255,14,0>>,Blue,Mix<Int<16000>,Black,RgbArg<BASE_COLOR_ARG,Rgb<255,14,0>>>,DodgerBlue>>,White>,Blue,0,6,1,2000,3>,TransitionEffectL<TrWaveX<RgbArg<BLAST_COLOR_ARG,Rgb<255,255,255>>,Scale<EffectRandomF<EFFECT_BLAST>,Int<100>,Int<400>>,Int<100>,Scale<EffectPosition<EFFECT_BLAST>,Int<100>,Int<400>>,Scale<EffectPosition<EFFECT_BLAST>,Int<28000>,Int<8000>>>,EFFECT_BLAST>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,TransitionEffectL<TrConcat<TrInstant,AlphaL<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<12000>,Int<60000>>>>,TrFadeX<Scale<ClashImpactF<>,Int<200>,Int<400>>>>,EFFECT_CLASH>,TransitionEffectL<TrWaveX<RgbArg<CLASH_COLOR_ARG,Rgb<255,255,255>>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Int<100>,Scale<ClashImpactF<>,Int<100>,Int<400>>,Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>>,EFFECT_CLASH>>,LockupTrL<TransitionEffect<AlphaL<AlphaMixL<Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>,AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,BrownNoiseFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<12000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>,300>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<22000>>>>,AlphaL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Mix<Int<20000>,Black,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<SwingSpeed<100>,Int<14000>,Int<18000>>>>,TrExtend<5000,TrInstant>,TrFade<5000>,EFFECT_LOCKUP_BEGIN>,TrConcat<TrJoin<TrDelay<50>,TrInstant>,Mix<IsLessThan<ClashImpactF<>,Int<26000>>,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,AlphaL<RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Scale<BladeAngle<0,16000>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-12000>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<10000>>>,Sum<IntArg<LOCKUP_POSITION_ARG,16000>,Int<-10000>>>,Scale<ClashImpactF<>,Int<20000>,Int<60000>>>>>,TrFade<300>>,TrConcat<TrInstant,RgbArg<LOCKUP_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,SaberBase::LOCKUP_NORMAL,Int<1>>,ResponsiveLightningBlockL<Strobe<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,AudioFlicker<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Blue>,50,1>,TrConcat<TrExtend<200,TrInstant>,AlphaL<RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,Bump<Scale<BladeAngle<>,Int<10000>,Int<21000>>,Int<10000>>>,TrFade<200>>,TrConcat<TrInstant,RgbArg<LB_COLOR_ARG,Rgb<255,255,255>>,TrFade<400>>,Int<1>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,28000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrSelect<IntArg<IGNITION_OPTION_ARG,0>,TrWipeX<BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeSparkTipX<RgbArg<IGNITION_COLOR_ARG,Rgb<255,255,255>>,BendTimePowInvX<IgnitionTime<300>,Mult<IntArg<IGNITION_OPTION2_ARG,10992>,Int<98304>>>>,TrWipeX<Scale<IsLessThan<BladeAngle<>,Int<16000>>,IgnitionTime<300>,Mult<IgnitionTime<300>,Int<16384>>>>,TrConcat<TrInstant,AlphaL<Black,LayerFunctions<LinearSectionF<Int<6144>,Scale<Trigger<EFFECT_IGNITION,IgnitionTime<300>,IgnitionTime<300>,Int<0>>,Int<12288>,Int<0>>>,LinearSectionF<Int<16384>,Scale<Trigger<EFFECT_IGNITION,IgnitionTime<300>,IgnitionTime<300>,Int<0>>,Int<12288>,Int<0>>>,LinearSectionF<Int<26624>,Scale<Trigger<EFFECT_IGNITION,IgnitionTime<300>,IgnitionTime<300>,Int<0>>,Int<12288>,Int<0>>>>>,TrDelayX<IgnitionTime<300>>>>,TrWipeInX<BendTimePowInvX<RetractionTime<0>,Mult<IntArg<RETRACTION_OPTION2_ARG,10992>,Int<98304>>>>,Black>,TransitionEffectL<TrConcat<TrJoin<TrDelay<2000>,TrWipe<1000>>,AlphaL<Mix<BatteryLevel,Red,Green>,SmoothStep<BatteryLevel,Int<-10>>>,TrWipeIn<1000>>,EFFECT_BATTERY_LEVEL>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "dl-44-ghostbusters"},

   { "_BlasterMode/Bowcaster;_common-Blaster;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "bowcaster"},

   { "_BlasterMode/Droideka;_common-Blaster;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<Gradient<RED,BLUE>,Gradient<CYAN,YELLOW>,300,800>(),
      Gradient<RgbArg<BASE_COLOR_ARG,Red>,Blue>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<Gradient<RgbArg<LOCKUP_COLOR_ARG,Red>,Blue>,White>>,
      SimpleClashL<Gradient<RgbArg<CLASH_COLOR_ARG,Cyan>,Yellow>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "droideka"},
};

/***********************************\
|*                                 *|
|* == Detonator sound font list == *|
|*                                 *|
\***********************************/

/*
   { "_detonator/td;common", "common/tracks/mandotheme.wav",
    "1 thermal\ndetonator"}, // styles copied from "config/td_proffieboard_config.h"

   { "_detonator/tmod;common", "common/tracks/the_book_of_boba_fett.wav",
    "2 thermal\ndetonator"},

   { "_Detonator/td_extra;common", "common/tracks/mandotheme.wav",
    "3 thermal\ndetonator"}, // styles copied from "config/td_proffieboard_config.h"

   { "_detonator/tmod_extra;common", "common/tracks/the_book_of_boba_fett.wav",
    "4 thermal\ndetonator"},
*/

/********************************\
|*                              *|
|* == Detonator presets list == *|
|*                              *|
\********************************/

Preset presets_detonator[] = {
   { "_Detonator/td;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,0>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleRainbowPtr<300,800>(),
      Rainbow,
      LockupL<AudioFlicker<Rainbow,RgbArg<LOCKUP_COLOR_ARG,White>>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,White>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Green>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "1 thermal\ndetonator"}, // styles copied from "config/td_proffieboard_config.h"

   { "_Detonator/tmod;common", "common/tracks/the_book_of_boba_fett.wav",
    // === Main Blade: ===
    StylePtr<InOutHelperTD<SimpleClash<ColorSequence<2000,
                         Rgb<255,0,0>,Rgb<255,255,0>,Rgb<255,0,255>,
                         Rgb<0,255,255>,Rgb<0,255,0>,Rgb<0,0,255>,
                         Rgb<255,255,0>,Rgb<0,255,255> >, RandomFlicker<WHITE, BLACK>, 6000, EFFECT_BLAST>, 100, 100, 6000, BLACK>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutHelperTD<Lockup<SimpleClash<RED,RandomFlicker<RED,BLACK>, 6000, EFFECT_BLAST>, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100, 6000>>(),
    // === NPXL outer ring: ===
    StyleRainbowPtr<100, 100>(),
    // === Crystal Chamber: ===
    StylePtr<InOutHelper<Lockup<RED, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "2 thermal\ndetonator"}, // styles copied from "config/td_proffieboard_config.h"

   { "_Detonator/td_extra;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === NPXL inner ring: ===
    StylePtr<InOutHelperTD<Lockup<SimpleClash<RED,RandomFlicker<RED,BLACK>, 6000, EFFECT_BLAST>, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100, 6000>>(),
    // === NPXL outer ring: ===
    StyleRainbowPtr<100, 100>(),
    // === Crystal Chamber: ===
    StylePtr<InOutHelper<Lockup<RED, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "3 thermal\ndetonator"}, // styles copied from "config/td_proffieboard_config.h"

   { "_Detonator/tmod_extra;common", "common/tracks/the_book_of_boba_fett.wav",
    // === Main Blade: ===
    StylePtr<InOutHelperTD<SimpleClash<ColorSequence<2000,
                         Rgb<255,0,0>,Rgb<255,255,0>,Rgb<255,0,255>,
                         Rgb<0,255,255>,Rgb<0,255,0>,Rgb<0,0,255>,
                         Rgb<255,255,0>,Rgb<0,255,255> >, RandomFlicker<WHITE, BLACK>, 6000, EFFECT_BLAST>, 100, 100, 6000, BLACK>>(),
    // === NPXL inner ring: ===
    StylePtr<InOutHelperTD<Lockup<SimpleClash<RED,RandomFlicker<RED,BLACK>, 6000, EFFECT_BLAST>, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100, 6000>>(),
    // === NPXL outer ring: ===
    StyleRainbowPtr<100, 100>(),
    // === Crystal Chamber: ===
    StylePtr<InOutHelper<Lockup<RED, Pulsing<RED, Rgb<50,0,0>, 300>>, 100, 100>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "4 thermal\ndetonator"}, // styles copied from "config/td_proffieboard_config.h"

};

/*********************************\
|*                               *|
|* == Jetpack sound font list == *|
|*                               *|
\*********************************/

/*
   { "_Jetpack/jp_1;common", "common/tracks/mandotheme.wav",
    "jetpack\n1 red"},

   { "_Jetpack/jp_2;common", "common/tracks/the_book_of_boba_fett.wav",
    "jetpack\n2 blue"},

   { "_Jetpack/jp_3;common", "common/tracks/mandotheme.wav",
    "jetpack\n3 green"},
*/

/******************************\
|*                            *|
|* == Jetpack presets list == *|
|*                            *|
\******************************/

Preset presets_jetpack[] = {
   { "_Jetpack/jp_1;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,0>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === NPXL outer ring: === Dim blade, brightness at 50% ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Red>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<RED,YELLOW,2>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "jetpack\n1 red"},

   { "_Jetpack/jp_2;common", "common/tracks/the_book_of_boba_fett.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,0>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,1>(),
    // === NPXL outer ring: === Dim blade, brightness at 50% ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Blue>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "jetpack\n2 blue"},

   { "_Jetpack/jp_3;common", "common/tracks/mandotheme.wav",
    // === Main Blade: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Green>,Yellow,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(),
    // === NPXL inner ring: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Green>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,1>(),
    // === NPXL outer ring: === Dim blade, brightness at 50% ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Green>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,2>(),
    // === Crystal Chamber: ===
    StylePtr<StyleFire<RgbArg<BASE_COLOR_ARG,Green>,Cyan,0,2,FireConfig<0,2000,5>,FireConfig<3000,0,0>,FireConfig<0,5000,10>,FireConfig<0,0,10>>>(), // was StyleFirePtr<BLUE,CYAN,3>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "jetpack\n3 green"},
};

/***********************************\
|*                                 *|
|* == MorseCode sound font list == *|
|*                                 *|
\***********************************/

/*
//need to create a style that can do red (digits), blue (letters), green (special characters)
   { "_MorseCode;common", "",
    "morse code"},
*/

/********************************\
|*                              *|
|* == MorseCode presets list == *|
|*                              *|
\********************************/

/*
Preset presets_morsecode[] = {  //need to create a style that can do red (digits), blue (letters), green (special characters)
   { "_MorseCode;common", "",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "morse code"},
};
*/

/*******************************\
|*                             *|
|* == Droid sound font list == *|
|*                             *|
\*******************************/

/*
   { "_DroidMode;common", "",
    "droid"},
*/

/****************************\
|*                          *|
|* == Droid presets list == *|
|*                          *|
\****************************/

/*
Preset presets_droid[] = {
   { "_DroidMode;common", "",
    // === Main Blade: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL inner ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === NPXL outer ring: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Crystal Chamber: ===
    StylePtr<Layers< // was StyleNormalPtr<WHITE,RED,300,800,RED>(),
      RgbArg<BASE_COLOR_ARG,White>,
      BlastL<RgbArg<BLAST_COLOR_ARG,White>>,
      LockupL<AudioFlicker<RgbArg<LOCKUP_COLOR_ARG,White>,Red>>,
      SimpleClashL<RgbArg<CLASH_COLOR_ARG,Red>>,
      InOutHelperL<InOutFuncX<IntArg<IGNITION_TIME_ARG,300>,IntArg<RETRACTION_TIME_ARG,800>>>>>(),
    // === Bluetooth: ===
    StylePtr<Blue>(),
    "droid"},
};
*/

/***************************\
|*                         *|
|* == Blades Array list == *|
|*                         *|
\***************************/

    // "One array to bring them all, and in the darkness bind them !" "@profezzorn"
BladeConfig blades[] = {
  { 0, //SABER
    // === Main Blade: ===
    WS281XBladePtr<144,bladePin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>(),
    // === NPXL Connector (1, 3, 5, 7, 9, 11, 13, 15 are "inner ring") ===
    SubBladeWithStride(0,15,2,WS281XBladePtr<16,blade2Pin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>()),
    // === NPXL Connector (2, 4, 6, 8, 10, 12, 14, 16 are "outer ring") ===
    SubBladeWithStride(1,15,2,NULL),
    // === Crystal Chamber: ===
    WS281XBladePtr<1,blade3Pin,Color8::GRB,PowerPINS<bladePowerPin1>>(),
    // === Bluetooth: === (fake blade)
    SimpleBladePtr<CH3LED,NoLED,NoLED,NoLED,bladePowerPin6,-1,-1,-1>(),
    // & add "StylePtr<Blue>()," in every presets.
    CONFIGARRAY(presets_saber), "__saber_save" },
    // === AV switch(s) ===
    //SimpleBladePtr<CreeXPE2WhiteTemplate<10000>, NoLED, NoLED, NoLED, bladePowerPin1, -1, -1, -1>()
    // & add "StylePtr<InOutTr<White,TrColorCycle<3000>,TrFade<500>,Pulsing<White,Black,10000>>>()" in every presets.

  { 1, //BLASTER
    // === Main Blade: ===
    WS281XBladePtr<144,bladePin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>(),
    // === NPXL Connector (1, 3, 5, 7, 9, 11, 13, 15 are "inner ring") ===
    SubBladeWithStride(0,15,2,WS281XBladePtr<16,blade2Pin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>()),
    // === NPXL Connector (2, 4, 6, 8, 10, 12, 14, 16 are "outer ring") ===
    SubBladeWithStride(1,15,2,NULL),
    // === Crystal Chamber: ===
    WS281XBladePtr<1,blade3Pin,Color8::GRB,PowerPINS<bladePowerPin1>>(),
    // === Bluetooth: === (fake blade)
    SimpleBladePtr<CH3LED,NoLED,NoLED,NoLED,bladePowerPin6,-1,-1,-1>(),
    // & add "StylePtr<Blue>()," in every presets.
    CONFIGARRAY(presets_blaster), "__blaster_save" },

  { 2, //DETONATOR
    // === Main Blade: ===
    WS281XBladePtr<144,bladePin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>(),
    // === NPXL Connector (1, 3, 5, 7, 9, 11, 13, 15 are "inner ring") ===
    SubBladeWithStride(0,15,2,WS281XBladePtr<16,blade2Pin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>()),
    // === NPXL Connector (2, 4, 6, 8, 10, 12, 14, 16 are "outer ring") ===
    SubBladeWithStride(1,15,2,NULL),
    // === Crystal Chamber: ===
    WS281XBladePtr<1,blade3Pin,Color8::GRB,PowerPINS<bladePowerPin1>>(),
    // === Bluetooth: === (fake blade)
    SimpleBladePtr<CH3LED,NoLED,NoLED,NoLED,bladePowerPin6,-1,-1,-1>(),
    // & add "StylePtr<Blue>()," in every presets.
    CONFIGARRAY(presets_detonator), "__detonator_save" },

  { 3, //JETPACK
    // === Main Blade: ===
    WS281XBladePtr<144,bladePin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>(),
    // === NPXL Connector (1, 3, 5, 7, 9, 11, 13, 15 are "inner ring") ===
    SubBladeWithStride(0,15,2,WS281XBladePtr<16,blade2Pin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>()),
    // === Dim blade, brigness at 50% === NPXL Connector (2, 4, 6, 8, 10, 12, 14, 16 are "outer ring") ===
    DimBlade(50.0, SubBladeWithStride(1,15,2,NULL)),
    // === Crystal Chamber: ===
    WS281XBladePtr<1,blade3Pin,Color8::GRB,PowerPINS<bladePowerPin1>>(),
    // === Bluetooth: === (fake blade)
    SimpleBladePtr<CH3LED,NoLED,NoLED,NoLED,bladePowerPin6,-1,-1,-1>(),
    // & add "StylePtr<Blue>()," in every presets.
    CONFIGARRAY(presets_jetpack), "__jetpack_save" },

/*
  { 4, //MORSE_CODE
    // === Main Blade: ===
    WS281XBladePtr<144,bladePin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>(),
    // === NPXL Connector (1, 3, 5, 7, 9, 11, 13, 15 are "inner ring") ===
    SubBladeWithStride(0,15,2,WS281XBladePtr<16,blade2Pin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>()),
    // === NPXL Connector (2, 4, 6, 8, 10, 12, 14, 16 are "outer ring") ===
    SubBladeWithStride(1,15,2,NULL),
    // === Crystal Chamber: ===
    WS281XBladePtr<1,blade3Pin,Color8::GRB,PowerPINS<bladePowerPin1>>(),
    // === Bluetooth: === (fake blade)
    SimpleBladePtr<CH3LED,NoLED,NoLED,NoLED,bladePowerPin6,-1,-1,-1>(),
    // & add "StylePtr<Blue>()," in every presets.
    CONFIGARRAY(presets_morsecode), "__morsecode_save" },

  { 5, //DROID
    // === Main Blade: ===
    WS281XBladePtr<144,bladePin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>(),
    // === NPXL Connector (1, 3, 5, 7, 9, 11, 13, 15 are "inner ring") ===
    SubBladeWithStride(0,15,2,WS281XBladePtr<16,blade2Pin,Color8::GRB,PowerPINS<bladePowerPin2,bladePowerPin3>>()),
    // === NPXL Connector (2, 4, 6, 8, 10, 12, 14, 16 are "outer ring") ===
    SubBladeWithStride(1,15,2,NULL),
    // === Crystal Chamber: ===
    WS281XBladePtr<1,blade3Pin,Color8::GRB,PowerPINS<bladePowerPin1>>(),
    // === Bluetooth: === (fake blade)
    SimpleBladePtr<CH3LED,NoLED,NoLED,NoLED,bladePowerPin6,-1,-1,-1>(),
    // & add "StylePtr<Blue>()," in every presets.
    CONFIGARRAY(presets_droid), "__droid_save" },
*/
};

/* === Standard blade, brightness at 100% ===   <--- FOR REFERENCE ONLY! DO NOT UNCOMMENT "AS-IS"!
BladeConfig blades[] = {
{ 0, WS281XBladePtr<144, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >(),
CONFIGARRAY(presets) },  };
// === Dim blade, brightness at 50% ===         <--- FOR REFERENCE ONLY! DO NOT UNCOMMENT "AS-IS"!
BladeConfig blades[] = {                        <--- BUT THIS OPTION IS COOL TO KNOW !!!
{ 0, DimBlade(50.0, WS281XBladePtr<144, bladePin, Color8::GRB, PowerPINS<bladePowerPin2, bladePowerPin3> >()),
CONFIGARRAY(presets) },  };  */

/*******************\
|*                 *|
|* == RFID list == *|
|*                 *|
\*******************/

//RFID_Command RFID_Commands[] = {
//  { ???????????????, "command to turn ON, like on a CFX board with the ring" }, // Could this even be possible on Proffie ???
//  { 0x0000000C04ULL, "change_preset", "0" },    // https://crucible.hubbe.net/t/magnetometers/2442
//  { 0x09003A8CDCULL, "change_preset", "1" },
//  { 0x??????????ULL, "change_preset", "2" },
//  { 0x??????????ULL, "change_preset", "3" },
//  { 0x??????????ULL, "change_preset", "4" },
//  { 0x??????????ULL, "change_preset", "5" },
//  { 0x??????????ULL, "change_preset", "6" },
//};

#endif // CONFIG_PRESETS



#ifdef CONFIG_BUTTONS
//============================================================================================================================================

                                             // There are currently three available button classes:
                                             // Button (standard momentary button)
                                             // TouchButton (similar to momentary button, but reacts to touch).
                                             // LatchingButton (on/off button, always controls ignition)
Button PowerButton(BUTTON_POWER,powerButtonPin,"pow");
Button AuxButton(BUTTON_AUX,auxPin,"aux");
Button Aux2Button(BUTTON_AUX2,aux2Pin,"aux2");
                //BUTTON_AUX2                // https://pod.hubbe.net/config/the-config_buttons-section.html
                //BUTTON_UP                  //
                //BUTTON_DOWN                //
                //BUTTON_LEFT                //UP, DOWN, LEFT, RIGHT & SELECT could be defined if wanting to use a "Potentiometer"/"Joystick"
                //BUTTON_RIGHT               //
                //BUTTON_SELECT              //
                //BUTTON_BLADE_DETECT ??? // this is not a button
//Button FireButton(BUTTON_FIRE,powerButtonPin,"fire");        //For Blaster
//Button ModeButton(BUTTON_MODE_SELECT,auxPin,"modeselect");   //For Blaster
//Button ReloadButton(BUTTON_RELOAD,aux2Pin,"reload");         //For Blaster
                //BUTTON_CLIP_DETECT ??? // this is not a button
//PullDownButton PowerButton(BUTTON_POWER, powerButtonPin, "pow");
//TouchButton PowerButton(BUTTON_POWER, powerButtonPin, 1700, "pow");
//InvertedLatchingButton PowerButton(BUTTON_POWER, powerButtonPin, "pow"); //for Dettonator

// === Rotary Encoder (see rotary.h) ===

//ChangePresetRotaryReceiver rotary_receiver;// The receiver decides what the rotary events do.
//Rotary<8, 9> rotary(&rotary_receiver);     // Uncomment the 2 lines on the left for rotary encoder.
/*
The receiver decides what the rotary events do.
There are currently receivers for changing presets,
changing variation and one that just prints things out.
More receivers can be added as needed.
(I should probably create one for changing the volume.)
*/

// === Proffie IR implementation ===
// Sender
// In CONFIG_BUTTONS
// BlasterIRSender blaster_ir_sender;
//
// Receiver
// in CONFIG_BUTTONS
// IRReceiver<blade3Pin> ir_receiver;
// BlasterDecoder blaster_decoder;
// PrintDecoder print_decoder;
/* Then hook up an IR receiver module to data3. Any pin can be used for this, just change the configuration code
   accordingly. The IR receiver module will also need power and GND. In most cases, you probably want to hook up
   the power to the 3.3v pad. I don’t think the IR module will draw enough power to matter, but I’m not sure. */

#endif // CONFIG_BUTTONS



#ifdef CONFIG_STYLES                         // https://pod.hubbe.net/config/the-config_styles-section.html
//============================================================================================================================================

        // Use in presets: StylePtr<BatteryLevelStyle>(),
using BatteryLevelStyle = InOutHelperX<Gradient<Red,Orange,Yellow,Green,Green,Green,Green>,BatteryLevel>;
        // Marble style from Profezzorn | Use in presets: StylePtr<MarbleStyle>(),
using MarbleStyle = Mix<CircularSectionF<MarbleF<Int<-2000>,Int<40000>,Ifon<Int<827680>,Int<0>>,Int<1276800>>,ChangeSlowly<Ifon<Int<32768>,Int<3276>>,Int<2048>>>,Black,Mix<InOutFuncX<Int<2000>,Int<2000>>,Rgb<0,0,20>,Rgb<0,255,255>>>;
//more stuff can come here
/* copyright Fett263 CustomBlade (Accent LED / PCB) OS7 Style
https://fett263library.com/early-access/#CustomBlade
OS7.8 v1.012
Single Style
Yin and Yang Blade (Blade Angle)
Default: Static
1: Static

Off Behavior: Off [Color: OffColorArg]

Base Color: BaseColorArg (0)
Yin and Yang Blade: Blade changes based on pointing up or down, creates yin and yang effect when spun.

--Effects Included--
Ignition Effect: Instant [Color: IgnitionColorArg]
Retraction Effect: Instant [Color: RetractionColorArg]
*/      // YingYang style from LyleStyle | Use in presets: StylePtr<YingYang>(),
using YingYang = Layers<Mix<IsLessThan<BladeAngle<>,Int<16384>>,RgbArg<BASE_COLOR_ARG,Rgb<0,0,255>>,RgbArg<ALT_COLOR_ARG,Rgb<255,0,0>>>,InOutTrL<TrInstant,TrInstant,Black>>;

/* copyright Fett263 Simple (Accent LED / PCB) OS7 Style
https://fett263library.com/early-access/#Simple
OS7.15 v4.11
Single Style
Base Style: Static

Off Behavior: Slow Run Forward [Color: OffColorArg]

Base Color: BaseColorArg (0)

--Effects Included--
Ignition Effect: Instant [Color: IgnitionColorArg]
Retraction Effect: Instant [Color: RetractionColorArg]
Lockup Effect: NoneLightning Block Effect: NoneDrag Effect:
0: accentDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: accentMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
*/      //left to right style from Aaron_Hilditch | Use in presets: StylePtr<LeftToRight>(),
using LeftToRight = Layers<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,31000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,26000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,26000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrInstant,TrInstant,TransitionLoop<Black,TrConcat<TrWipe<1000>,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,TrWipe<1000>>>>>;

/* copyright Fett263 Simple (Accent LED / PCB) OS7 Style
https://fett263library.com/early-access/#Simple
OS7.15 v4.11
Single Style
Base Style: Static

Off Behavior: Slow Run Reverse [Color: OffColorArg]

Base Color: BaseColorArg (0)

--Effects Included--
Ignition Effect: Instant [Color: IgnitionColorArg]
Retraction Effect: Instant [Color: RetractionColorArg]
Lockup Effect: NoneLightning Block Effect: NoneDrag Effect:
0: accentDragMulti0Shape - Begin: Wipe In - Style: Intensity Sparking Drag - End: Wipe Out
[Color: DragColorArg]
Melt Effect:
0: accentMeltMulti0Shape - Begin: Wipe In - Style: Intensity Melt - End: Wipe Out
[Color: StabColorArg]
*/      //right to left style from Aaron_Hilditch | Use in presets: StylePtr<RightToLeft>(),
using RightToLeft = Layers<RgbArg<BASE_COLOR_ARG,Rgb<255,0,0>>,LockupTrL<AlphaL<TransitionEffect<RandomPerLEDFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>>,BrownNoiseFlickerL<RgbArg<DRAG_COLOR_ARG,Rgb<255,255,255>>,Int<300>>,TrExtend<4000,TrInstant>,TrFade<4000>,EFFECT_DRAG_BEGIN>,SmoothStep<Scale<TwistAngle<>,IntArg<DRAG_SIZE_ARG,31000>,Int<30000>>,Int<3000>>>,TrWipeIn<200>,TrWipe<200>,SaberBase::LOCKUP_DRAG,Int<1>>,LockupTrL<AlphaL<Stripes<2000,4000,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,Mix<Sin<Int<50>>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,Mix<Int<4096>,Black,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,26000>,Int<30000>>,Int<3000>>>,TrConcat<TrExtend<4000,TrWipeIn<200>>,AlphaL<HumpFlicker<Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>,RotateColorsX<Int<3000>,Mix<TwistAngle<>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>,RotateColorsX<Int<3000>,RgbArg<STAB_COLOR_ARG,Rgb<255,68,0>>>>>,100>,SmoothStep<Scale<TwistAngle<>,IntArg<MELT_SIZE_ARG,26000>,Int<30000>>,Int<3000>>>,TrFade<4000>>,TrWipe<200>,SaberBase::LOCKUP_MELT,Int<1>>,InOutTrL<TrInstant,TrInstant,TransitionLoop<Black,TrConcat<TrWipeIn<1000>,RgbArg<OFF_COLOR_ARG,Rgb<255,0,0>>,TrWipeIn<1000>>>>>;

/*
// https://crucible.hubbe.net/t/blade-plug-charging-with-switch-common-positive/6890/4
class BlackPower {
public:
  LayerRunResult run(BladeBase* base) {
    return LayerRunResult::UNKNOWN;
  }
  SimpleColor getColor(int led) {
    return SimpleColor(Color16(0,0,0));
  }
};
*/

#endif // CONFIG_STYLES



#ifdef CONFIG_BOTTOM
//============================================================================================================================================

    DisplayHelper<128,uint32_t,              //OLED additional features: "Bullet Counts" - Code needed to activate the display.
    BaseLayerOp<StandardDisplayController>,  // https://pod.hubbe.net/display/bullet-counts.html
    //IfImageOp<OP>,                         //
    ClearRectangleOp<10,80,8,24>,            //
    WriteBulletCountOp<10,20,5> >            // https://pod.hubbe.net/display/images-and-animations.html
    display_controller;
    SSD1306Template<128,uint32_t> display(&display_controller);

/*
//to make saber "self turn off when wall mounted" https://crucible.hubbe.net/t/power-on-directly-after-boot/3904/2
struct Script : Looper, StateMachine {
  const char* name() override { return "script"; }
  void Loop() override {
     STATE_MACHINE_BEGIN();
     CommandParser::DoParse("on", nullptr);
     SLEEP(2 * 60 * 1000); // 2 minutes
     CommandParser::DoParse("off", nullptr);
     STATE_MACHINE_END();
  }
};
Script script;
*/

/***********************\
|*                     *|
|* == Color Display == *| https://pod.hubbe.net/display/color_displays.html
|*                     *|
\***********************/

/*
//In CONFIG_TOP, add: #define ENABLE_SPIDISPLAY

//In CONFIG_BOTTOM, add ONE of these (depending on display size):
SPIDisplay_DFRobot096<3> display;
StandarColorDisplayController<160, 80> display_controller(&display);

SPIDisplay_AdaFruit358<3> display;
StandarColorDisplayController<160, 128> display_controller(&display);

SPIDisplay_AdaFruit5206<3> display;
StandarColorDisplayController<280, 240> display_controller(&display);

SPIDisplay_AdaFruit4311<3> display;
StandarColorDisplayController<320, 240> display_controller(&display);
                                             // https://crucible.hubbe.net/t/color-display-progress/4429/84
                                             // https://github.com/profezzorn/ProffieColorImages
                                             // https://github.com/Cairon/ProffieColorImages
                                             // https://www.dfrobot.com/product-2445.html
                                             // https://github.com/Cairon/ProffieColorImages/tree/main/virtual_crystal_chamber/scene
                                             // https://www.youtube.com/watch?v=wcSiNcCtR2I
                                             // https://github.com/Cairon/ProffieColorImages/tree/main/virtual_crystal_chamber/images/effects
                                             //
                                             //For this to work, I need to have in Arduino IDE: Files, Preferences, Additional Boards Manager URLs:
                                             // https://profezzorn.github.io/arduino-proffieboard/package_proffieboard_index.json,https://profezzorn.github.io/arduino-proffieboard/package_proffieboard_experimental_index.json
                                             //The first one will let you install in Tools, Board: "your board", Board Manager...,
                                             //Proffieboard V3.6.0 (at the time of writing)
                                             //The second one will let you install in Tools, Board: "your board", Board Manager...,
                                             //Proffieboard Experimental V4.1.0 (at the time of writing)
                                             //using Arduino IDE 1.8.19 (at the time of writing)
                                             //
                                             //@profezzorn "I suggest at least upgrading to 1.8.19 at the very least.
                                             //Soon, new versions of the arduino-proffieboard plugin will require the 2.x versions
                                             //of arduino to download though, so it might be better to start using that now.
                                             //(2.x was pretty bad in the beginning, but nowadays it tends to work quite well,
                                             //even if the UI is still a bit confusing.)
                                             //
                                             //You can also use 2.x to install the arduino-proffieboard plugin and then use 1.8.19
                                             //to do the actual compilation. I think you can have both 1.x and 2.x installed at the
                                             //same time on all platforms."
*/
#endif // CONFIG_BOTTOM

/*
Generated with Fett263 Config Helper v3.10d https://fett263.com/fett263-os7-config-helper.html
&
Build with ProffieConfig 1.6.9, created by Ryryog25.
The tool can be found here: https://github.com/ryryog25/ProffieConfig/wiki/ProffieConfig
ProffieConfig is an All-In-One utility for managing your Proffieboard.
                    //I can't wait for ProffieConfig 2.x, it seems it will be amazing !!!
*/

// ¯\_(ツ)_/¯
