#ifndef MODE_SETTINGS_MENUES_H
#define MODE_SETTINGS_MENUES_H

#include "../common/color.h"
#include "menu_list.h"

int prop_GetCurrentClashThreshold();
void prop_SetClashThreshold(int clash_threshold);
int prop_GetBladeLength(int blade);
int prop_GetMaxBladeLength(int blade);
void prop_SetBladeLength(int blade, int len);

namespace mode {

#ifndef VOLUME_MENU_GAMMA
#define VOLUME_MENU_GAMMA 2.2
#endif

template<class SPEC>
struct SmoothVolumeMode : public SPEC::SmoothMode {
  // x = 0-32767
  virtual int get() override {
    float ret = dynamic_mixer.get_volume() / VOLUME;
    ret = powf(ret, VOLUME_MENU_GAMMA);
    return ret * 32768.0;
  }
    
  virtual void set(int x) {
    float ret = x / 32768.0f;
    if (!getSL<SPEC>()->busy()) {
      getSL<SPEC>()->SayWhole(ret * 100);
      getSL<SPEC>()->SayPercent();
    }
    ret = powf(ret, 1.0 / VOLUME_MENU_GAMMA);
    dynamic_mixer.set_volume(VOLUME * ret);
  }
};

#ifdef DYNAMIC_BLADE_LENGTH

template<class SPEC>
class ShowLengthStyle {
  void run(BladeBase* blade) {
    len_ = getPtr<SPEC::ChangeBladeLength>()->getLength();
  }
  OverDriveColor getColor(int led) {
    if (led == len_ - 1) return SimpleColor(Color16(65535,65535,65535));
    return SimpleColor(Color16(0,0,0));
  }
  int len_;
};

template<class SPEC>
struct ChangeBladeLengthBlade1 : public SPEC::MenuBase {
  virtual int blade() { return 1; }
  void activate(bool onreturn) {
    SPEC::MenuBase::activate(onreturn);
    this->pos_ = saved_len_ = prop_GetBladeLength(1);
    SetBladeLength(blade(), prop_GetMaxBladeLength(1));
    showlen_.Start(blade());
  }
  void say() override {
    getSL<SPEC>()->SayWhole(this->pos_);
  }
  uint16_t size() override {
    return prop_GetMaxBladeLength(blade());
  }
  void select() override {
    // SPEC::MenuBase::select(); = 0
    prop_SetBladeLength(blade(), this->pos_);
    getSL<SPEC>()->SaySelect();
    showlen_.Stop(blade());
  }
  void exit() override {
    SPEC::MenuBase::exit();
    prop_SetBladeLength(blade(), saved_len_);
    getSL<SPEC>()->SayCancel();
    showlen_.Stop(blade());
    // TODO: SAVE!
  }

  int getLength() { return this->pos_; }

  ShowColorSingleBladeTemplate<typename SPEC::ShowLengthStyle> showlen_;
  int saved_len_;
};
#endif

#ifdef DYNAMIC_CLASH_THRESHOLD

#ifndef CLASH_THRESHOLD_GAMMA
#define CLASH_THRESHOLD_GAMMA 2.0f
#endif

#define MAX_CLASH_THRESHOLD_G 16.0f

template<class SPEC>
struct SmoothClashThresholdMode : public SPEC::SmoothMode {
  void activate(bool onreturn) {
    SPEC::SmoothMode::activate(onreturn);
  }
  // x = 0-32767
  virtual int get() override {
    float ret = prop_GetCurrentClashThreshold() / MAX_CLASH_THRESHOLD_G;
    ret = powf(ret, CLASH_THRESHOLD_GAMMA);
    return ret * 32768.0;
  }
    
  virtual void set(int x) {
    value_ = x;
  }
  void exit() override {
    SPEC::SmoothMode::exit();
    getSL<SPEC>()->SayCancel();
  }
  void select() override {
    SPEC::SmoothMode::select();
    
    float ret = value_ / 32768.0f;
    ret = powf(ret, 1.0 / VOLUME_MENU_GAMMA);
    ret *= MAX_CLASH_THRESHOLD_G;
    if (!getSL<SPEC>()->busy()) {
//      getSL<SPEC>()->SayClashThreshold(ret * 100);
      getSL<SPEC>()->SayNumber(ret, SayType::SAY_DECIMAL);
    }
    prop_SetClashThreshold(ret);
    getSL<SPEC>()->SaySelect();
    popMode();
    // TODO: SAVE!
  }

  int value_;
};
#endif


#ifdef DYNAMIC_BLADE_DIMMING

template<class SPEC>
struct SmoothChangeBladeDimmingMode : public SPEC::SmoothMode {
  void activate(bool onreturn) {
    SPEC::SmoothMode::activate(onreturn);
    saved_value_ = SaberBase::GetCurrentDimming();
  }
  // x = 0-32767
  virtual int get() override {
    float ret =  SaberBase::GetCurrentDimming() / 32768.0;
    ret = powf(ret, COLOR_MENU_GAMMA);
    return ret * 32768.0;
  }
    
  virtual void set(int x) {
    float ret = x / 32768.;
    ret = powf(ret, 1.0/COLOR_MENU_GAMMA);
    SaberBase::SetDimming(ret * 32768.0f);
  }
  void exit() override {
    SPEC::SmoothMode::exit();
    SaberBase::SetDimming(saved_value_);
    getSL<SPEC>()->SayCancel();
  }
  void select() override {
    // TODO: SAVE!
    SPEC::SmoothMode::select();
    getSL<SPEC>()->SaySelect();
    popMode();
  }

  int saved_value_;
};

#endif


template<class SPEC>
struct SettingsMenuMode : public MenuEntryMenu<SPEC,
  SubMenuEntry<typename SPEC::ChangeVolumeMode, typename SPEC::SoundLibrary::tEditVolume>
#ifdef DYNAMIC_BLADE_LENGTH
  ,SubMenuEntry<typename SPEC::ChangeBladeLengthMode, typename SPEC::SoundLibrary::tEditBladeLength>
#endif
#ifdef DYNAMIC_CLASH_THRESHOLD
  ,SubMenuEntry<typename SPEC::ChangeClashThresholdMode, typename SPEC::SoundLibrary::tEditClashThreshold>
#endif  
#ifdef DYNAMIC_BLADE_DIMMING
  ,SubMenuEntry<typename SPEC::ChangeBladeDimmingMode, typename SPEC::SoundLibrary::tEditBrightness>
#endif
> {};

template<class SPEC>
class BasicTopMenu : public MenuEntryMenu<SPEC,
  SubMenuEntry<typename SPEC::EditPresetsMenu, typename SPEC::SoundLibrary::tEditPresets>,
  SubMenuEntry<typename SPEC::SettingsMenu, typename SPEC::SoundLibrary::tEditSettings>
> {};

}  // namespace mode

#endif
