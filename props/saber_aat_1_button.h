// Saber Art and Tech Buttons props file, includes the following changes
// Also include prop_base.h, for it contains the volume menu by rotating the Hilt
//
// Button configs:
//
// 1 Button:
// Activate - short click while OFF
// Activate Muted - fast double click while OFF
// Turn off blade - hold and wait till blade is off while ON
// Lockup - hold + hit clash while ON pointing the blade tip up
// Drag - hold + hit clash while ON pointing the blade tip down
// Blaster Blocks - short click while ON
// Force Effects - Twist the hilt while ON
// Play/Stop Music - hold and release while OFF
// Next Preset - Hold and clash  while OFF
// Prev Preset - Hold and twist while OFF
// Enter/Exit menu   - hold and wait while off/in menu mode
// Next menu/confirm - short click while in menu mode

//TWIST;
// Force effect while on
// revert to first (default) preset while OFF
// Color Change mode - Twist hilt while in Colorchange mode to change blade color
// Volume Change mode - Twist hilt while in Volumechange mode to increase or decrease volume 
// Birghtess Change mode - Twist hilt while in Brightness change mode to increase or decrease the brigtness of the blade 

#ifndef PROPS_SABER_AAT_BUTTONS_H
#define PROPS_SABER_AAT_BUTTONS_H

#include "prop_base.h"

#undef PROP_TYPE
#define PROP_TYPE SaberAatButtons

// The Saber class implements the basic states and actions
// for the saber.
class SaberAatButtons : public PropBase {
public:
  SaberAatButtons() : PropBase() {}
  const char* name() override { return "SaberAatButtons"; }

  enum MenuType {
    MENU_NONE,  
    MENU_COLOR,
    //MENU_VOLUME,
    MENU_DIM,
    NUM_MENUS
  };
  
  bool CfgMenu(){
      
      //Go to next menu (increment enum) 
      menu_ = static_cast<MenuType>((menu_ + 1) % NUM_MENUS); 
      
      //do Menu item
      switch(menu_){
          case MENU_COLOR:
//             ToggleMenuChangeMode(); // we are entering menu mode
             if (!SaberBase::IsOn()){
                 On();
                 delay(250);//wait for saber to activate
             }
            //no need to play sound, is handled by Colorchange itself
            ToggleColorChangeMode();
            return true;
            break;
          // case MENU_VOLUME:
            // if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
                // ToggleColorChangeMode();//Turn Colorchange control off
            // }
            // ToggleVolumeChangeMode();
            // STDOUT.println("Enter Volume Menu");
            // return true;
            // break;
          case MENU_DIM:
            if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) { // this should match previous menu
                ToggleColorChangeMode();//Turn Volume control off
                SaveVolumeIfNeeded();
            }
            ToggleBrightnessChangeMode();
            STDOUT.println("Enter Brightness Menu");
            return true;
            break;
          case MENU_NONE:
            ExitMenu();
            return false;
            break;
          case NUM_MENUS:
            return false;//just to eliminate compiler warning
            break;
      }
    return false  ;
  }
 
  void ExitMenu(){
       bool exiting_ = false;
       menu_ = MENU_NONE;
       
        if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
          // Just exit color change mode.
          // Don't turn saber off.
          ToggleColorChangeMode();
          exiting_ = true;
        }
        
        // if (VolumeChangeMode()){
          // ToggleVolumeChangeMode();
          // SaveVolumeIfNeeded();
          // STDOUT.println("Exit Volume Menu");
          // exiting_ = true;
        // }

        if (SaberBase::GetDimChangeMode() != SaberBase::DIM_CHANGE_MODE_NONE) { 
          ToggleBrightnessChangeMode();
          STDOUT.println("Exit Brightness Menu");
          exiting_ = true;
        }
        
//        if (exiting_){
//            if (SaberBase::GetMenuChangeMode() != SaberBase::MENU_CHANGE_MODE_NONE) {
//              ToggleMenuChangeMode();
//              STDOUT.println("Exit Menu-mode");
//              exiting_ = true;
//            }
//        }
  }
  

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {


    // pointing up/down
    if (accel_.x < -0.15) {
      pointing_down_ = true;
    } else {
      pointing_down_ = false;   
    }
    //for up down with hysteresis, uncomment following
    //if (accel_.x > 0.15) {
    //  pointing_down_ = false;
    //}
    // if (accel_.x < -0.15) {
    //   pointing_down_ = true;
    // }
    
    
    if(menu_ == MENU_NONE){     
            switch (EVENTID(button, event, modifiers)) {
                case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
                  //pointing up/down detection used to be here
                  return true;

                case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
                      aux_on_ = false;
                      On();
                    return true;

                    // Handle double-click with preon
                case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_OFF):
                    if (on_pending_) {
                      if (SetMute(true)) {
                        unmute_on_deactivation_ = true;
                      }
                      return true;
                    }
                    return false;

                case EVENTID(BUTTON_POWER, EVENT_DOUBLE_CLICK, MODE_ON):
                    if (millis() - activated_ < 500) {
                      if (SetMute(true)) {
                        unmute_on_deactivation_ = true;
                      }
                    }
                    return true;

                case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
                      SaberBase::DoBlast();
                    return true;

                case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON):
                      SaberBase::DoForce();
                    return true;

                case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_ON):
                    if (!SaberBase::Lockup()) {
                      Off();
                      return true;
                    }
                    break;

                case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_OFF):
                      CfgMenu();//enter config menu
                    return true;

                case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_ON | BUTTON_POWER):
                    if (!SaberBase::Lockup()) {
                      if (pointing_down_) {
                        SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
                      } else {
                        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
                      }
                      SaberBase::DoBeginLockup();
                      return false;//to keep track off button released
                    }
                    break;

                case EVENTID(BUTTON_NONE, EVENT_STAB, MODE_ON | BUTTON_POWER):
                    if (!SaberBase::Lockup()) {
                        SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
                        SaberBase::DoBeginLockup();
                    return false; //break??
                    }
                    break;

                case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_ON | BUTTON_POWER):
                    if (!SaberBase::Lockup()) {
                        SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
                        SaberBase::DoBeginLockup();
                    return false; //break??
                    }
                    break;

                // Off functions
                case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_OFF):
                //case EVENTID(BUTTON_POWER, EVENT_CLICK_LONG, MODE_ON): // this is annoying and you will trigger it by accident too much
                    if (!SaberBase::Lockup()) {
                       StartOrStopTrack();
                    }
                    return true;
                    
                case EVENTID(BUTTON_NONE, EVENT_CLASH, MODE_OFF | BUTTON_POWER):
                    next_preset();
                    return true;

                case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF | BUTTON_POWER):
                    previous_preset();
                    return true;
                
                case EVENTID(BUTTON_NONE, EVENT_TWIST, MODE_OFF):
                    Parse("set_preset", "0");//select 1st preset
                #ifdef SAVE_PRESET
                    SaveState(0);//remember we went to preset 0 next time we start up
                #endif
                    return true; //break??

                case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_OFF):
                    SaberBase::RequestMotion();
                    return true;

              // Events that needs to be handled regardless of what other buttons
              // are pressed.
                case EVENTID(BUTTON_POWER, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
                    if (SaberBase::Lockup()) {
                      SaberBase::DoEndLockup();
                      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
                      return true;
                    }
                
            }
    }
    else // we are in menu mode
    {
       //MENU MODE
           switch(EVENTID(button, event, modifiers)) {

              case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_ON):
              case EVENTID(BUTTON_POWER, EVENT_CLICK_SHORT, MODE_OFF):
                //go to next menu item
                CfgMenu();
                return true;

              case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_OFF):
              case EVENTID(BUTTON_POWER, EVENT_HELD_LONG, MODE_ON):
                ExitMenu();         
                return true;

            }
        
    }
    return false;
  }

private:
  RefPtr<BufferedWavPlayer> player;
  MenuType menu_ = MenuType(MENU_NONE);
  bool aux_on_ = true;
  bool pointing_down_ = false;
  bool mode_menu_ = false;
  bool mode_volume_ = false;
  bool mode_dim_ = false;
  bool mode_color_ = false;
};

#endif
