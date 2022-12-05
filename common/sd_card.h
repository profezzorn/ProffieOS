#ifndef COMMON_SDCARD_H
#define COMMON_SDCARD_H

#include "lsfs.h"

#if defined(ENABLE_SD) && defined(ARDUINO_ARCH_STM32L4)

// Unmount sdcard when we don't need it anymore.
class SDCard : Looper {
public:
  SDCard() : Looper() {}
  const char* name() override { return "SDCard"; }

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

  void Mount() {
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
