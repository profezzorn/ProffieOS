#ifndef COMMON_SDCARD_H
#define COMMON_SDCARD_H

#include "lsfs.h"

#if defined(ENABLE_SD) && defined(ARDUINO_ARCH_STM32L4)

// Unmount sdcard when we don't need it anymore.
#if defined(XPOWERMAN) 
class SDCard : Looper , xPowerSubscriber {
#else // nULTRA_PROFFIE
class SDCard : Looper {
#endif // ULTRA_PROFFIE
public:
  #if defined(XPOWERMAN)
    SDCard() : Looper(), xPowerSubscriber(pwr4_SD) {}
  #else
    SDCard() : Looper()  {}
  #endif
  const char* name() override { return "SDCard"; }

  #if defined(XPOWERMAN) 
    bool Active() {
      if (SoundActive() || AudioStreamWork::sd_is_locked() || AudioStreamWork::SDActive()) return true;
      if (SaberBase::IsOn()) return true;
      #ifdef USB_CLASS_MSC
        //if (USBD_Configured()) return false;
        // if (USBD_Connected()) return false;
        if(stm32l4_gpio_pin_read(GPIO_PIN_PB2)) return false;  // more reliable that usbd_connected
      #endif
      return false;
    }
        void PwrOn_Callback() override { 
          #ifdef DIAGNOSE_POWER
            STDOUT.println(" sd+ "); 
          #endif
          }         
        void PwrOff_Callback() override {
            if(LSFS::IsMounted()) { 
            AudioStreamWork::LockSD_nomount(true);
            AudioStreamWork::CloseAllOpenFiles();
            //#ifdef DIAGNOSE_STORAGE
            STDOUT.println("Unmounting SD");
            // #endif          
            LSFS::End();
            AudioStreamWork::LockSD_nomount(false);
            }
            #ifdef DIAGNOSE_POWER
              STDOUT.println(" sd- "); 
            #endif
        }
    #ifdef USB_CLASS_MSC
    bool HoldPower() override {
        // if (USBD_Connected()) return true;
        if(stm32l4_gpio_pin_read(GPIO_PIN_PB2)) return true;
        return false;
    }
    #endif         
  #else // nPOWERMAN
    bool Active() {
    #ifdef ENABLE_AUDIO    
        if (amplifier.Active() || AudioStreamWork::sd_is_locked() || AudioStreamWork::SDActive()) {
          last_enabled_ = millis();
          return true;
        }
    #endif    
        if (SaberBase::IsOn()) {
          last_enabled_ = millis();
          return true;
        }
    #ifdef USB_CLASS_MSC
        if (USBD_Configured()) return false;
    #endif
        uint32_t t = millis() - last_enabled_;
        if (t < 1000) return true;
        return false;
      }  
  #endif // POWERMAN

  void Mount() {
    #if defined(XPOWERMAN)
      uint32_t mountTimeout = PWRMAN_SDMOUNTTIMEOUT;
      RequestPower(&mountTimeout);   // allow longer time for pre-loop initializations
    #endif    

    last_enabled_ = millis();
    if (LSFS::IsMounted()) return;
    // Wait for card to become available, up to 1000ms
    uint32_t start = millis();
    while (!LSFS::CanMount() && millis() - start < 1000)
      armv7m_core_yield();
    if (!LSFS::CanMount()) {
      char tmp[128];
      LSFS::WhyBusy(tmp);
      STDOUT.print("SD card is busy, flags= ");
      STDOUT.println(tmp);
      return;
    }
    
    if (!LSFS::Begin()) {
      STDOUT.println("Failed to mount SD card.");
      return;
    }
  }

protected:
  void Setup() override {
    last_enabled_ = millis();
  }

#if defined(XPOWERMAN) 
  void Loop() override {
      if (Active()) 
          RequestPower();   // for all subscribed domains        
      if(!LSFS::IsMounted()) {   // attempt to mount once every secondm if it should be active
        if (Active() && millis() - last_mount_try_ > 1000) {
          last_mount_try_ = millis();
          AudioStreamWork::LockSD_nomount(true);
          if (LSFS::CanMount()) Mount();
          AudioStreamWork::LockSD_nomount(false);
        }
      }
      #ifdef USB_CLASS_MSC
      else {
        if (!Active()) {
            PwrOff_Callback();
        }
      }
      #endif   
  }
#else // nXPOWERMAN

  void Loop() override {
    if (LSFS::IsMounted()) {
      if (!Active()) {
	AudioStreamWork::LockSD_nomount(true);
	AudioStreamWork::CloseAllOpenFiles();
	STDOUT.println("Unmounting SD Card.");
	LSFS::End();
	AudioStreamWork::LockSD_nomount(false);
      }
    } else {
      if (Active() && millis() - last_mount_try_ > 1000) {
	last_mount_try_ = millis();
	AudioStreamWork::LockSD_nomount(true);
	if (LSFS::CanMount()) Mount();
	AudioStreamWork::LockSD_nomount(false);
      }
    }
  }
#endif // XPOWERMAN

private:
  uint32_t last_enabled_;
  uint32_t last_mount_try_;
};

SDCard sdcard;
inline void MountSDCard() { sdcard.Mount(); }
#else
inline void MountSDCard() {  }
#endif // v4 && enable_sd

#endif
