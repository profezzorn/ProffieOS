#ifndef MODES_STYLE_OPTION_MODES_H
#define MODES_STYLE_OPTION_MODES_H

namespace mode {

template<class SPEC>
class SelectArgSmoothMode : public SPEC::SmoothMode {
public:
  int get() override { return GetIntArg(menu_current_blade, menu_current_arg); }
  void set(int x) {
    value_ = x;
    if (!getSL<SPEC>()->busy()) {
      getSL<SPEC>()->SayWhole(x * 100 / 32768);
      getSL<SPEC>()->SayPercent();
    }
  }

  void select() override {
    SPEC::SmoothMode::select();
    SetIntArg(menu_current_blade, menu_current_arg, value_);
    popMode();
  }

private:  
  int value_ = 0;
};

template<class SPEC>
class SelectArgTime : public SPEC::SmoothMode {
public:
  float t(int x) { return powf(x / 32767.0f, 2.0) * 30.0; }
  int v(float seconds) {
    if (seconds > 30.0) seconds = 30.0;
    if (seconds < 0.0) seconds = 0.0;
    return powf(seconds / 30.0f, 1.0/2.0) * 32767.0;
  }

  int get() override {
    int millis = GetIntArg(menu_current_blade, menu_current_arg);
    value_ = v(millis / 1000.0f);
    return value_;
  }

  void set(int x) {
    value_ = x;
    if (!getSL<SPEC>()->busy()) {
      getSL<SPEC>()->SayNumber(t(x), SAY_DECIMAL);
      getSL<SPEC>()->SaySeconds();
    }
  }

  void select() override {
    getSL<SPEC>()->SaySelect();
    SetIntArg(menu_current_blade, menu_current_arg, (int)(t(value_) * 1000));
    SPEC::SmoothMode::select();
  }

  void exit() override {
    getSL<SPEC>()->SayCancel();
    SPEC::SmoothMode::exit();
  }

private:  
  int value_ = 0;
};

template<class SPEC>
class SelectArgNumber : public SPEC::MenuBase {
public:
  void mode_activate(bool onreturn) override {
    SPEC::MenuBase::mode_activate(onreturn);
    if (!onreturn) {
      int max = GetMaxStyleArg();
      if (max < 0) max = 32768;
      max_ = max;
      this->pos_ = GetIntArg(menu_current_blade, menu_current_arg);
      // TODO: What if pos_ > max_ ?
    }
  }
  void say() override {
    getSL<SPEC>()->SayWhole(this->pos_);
  }
  uint16_t size() override { return max_; }

  void select() override {
    SetIntArg(menu_current_blade, menu_current_arg, this->pos_);
    SPEC::MenuBase::select();
  }
private:
  uint16_t max_;
};

template<class SPEC>
class SelectArgMode : public SPEC::MenuBase {
public:
  void mode_activate(bool onreturn) override {
    // TODO: Set pos_ to something reasonable?
    arginfo_ = style_parser.GetArgInfo(GetStyle(menu_current_blade));

    // Shift away "builtin X Y"
    arginfo_ >>= arginfo_.offset_;
    arginfo_.used_.clear(0);
    
    menu_current_style_offset  = arginfo_.offset_; // almost always 2

    if (arginfo_.used() == 0) {
      getSL<SPEC>()->SayThisStyleHasNoSettings();
      popMode();
      return;
    }

    PVLOG_DEBUG << " offset = " << arginfo_.offset_ << "\n";
    for (int i = 0; i < 128; i++) {
      if (arginfo_.used(i)) {
	PVLOG_DEBUG << "Used args = " << i << " color = " << arginfo_.iscolor(i) << "\n";
      }
    }
    
    SPEC::MenuBase::mode_activate(onreturn);
  }
  uint16_t size() override { return arginfo_.used(); }
  void fadeout(float len) {
    getSL<SPEC>()->fadeout(len);
  }
  void say() override {
    getSL<SPEC>()->SayArgument(getCurrentArgument());
  }

  void select() override {
    menu_current_arg = getCurrentArgument();
    if (arginfo_.iscolor(menu_current_arg)) {
      pushMode<typename SPEC::SelectArgColor>();
    } else {
      int max_arg = GetMaxStyleArg();
      if (max_arg == 32768 || max_arg == 32767) {
	pushMode<typename SPEC::SelectArgSmooth>();
      } else if (max_arg <= 0 && isTimeArg(menu_current_arg)) {
	pushMode<typename SPEC::SelectArgTime>();
      } else {
	pushMode<typename SPEC::SelectArgNumber>();
      }
    }
  }

protected:
  int getCurrentArgument() { return arginfo_.nth(this->pos_); }
  ArgInfo arginfo_;
};


int menu_selected_blade;

// Select this style for copying.
template<class SPEC>
struct SelectStyleEntry : public  MenuEntry {
  void say(int entry) {
    getSL<SPEC>()->SaySelectStyle();
  }
  void select(int entry) {
    menu_selected_preset = prop_GetPresetPosition();
    menu_selected_blade = menu_current_blade;
    getSL<SPEC>()->SaySelect();
  }
};

template<class SPEC>
struct ApplyColorsFromSelectedStyleEntry : public  MenuEntry {
  void say(int entry) {
    getSL<SPEC>()->SayApplyColorsFromSelectedStyle();
  }
  void select(int entry) {
    if (menu_selected_preset == -1) {
      getSL<SPEC>()->SayNoStyleSelected();
      return;
    }
    CurrentPreset preset;
    preset.Load(menu_selected_preset);
    const char* FROM = preset.GetStyle(menu_selected_blade);
    const char* TO = GetStyle(menu_selected_blade);
    SetStyle(menu_selected_blade, style_parser.CopyColorArguments(FROM, TO));
    getSL<SPEC>()->SaySelect();
  }
};

template<class SPEC>
struct ApplyColorsToAllBladesEntry : public MenuEntry {
  void say(int entry) {
    getSL<SPEC>()->SayApplyColorsToAllBlades();
  }
  void select(int entry) {
    getSL<SPEC>()->SaySelect();
    const char* FROM = GetStyle(menu_selected_blade);
    for (int b = 1; b <= NUM_BLADES; b++) {
      if (b == menu_selected_blade) continue;
      SetStyle(b, style_parser.CopyColorArguments(FROM, GetStyle(b)));
    }
  }
};

template<class SPEC>
struct ApplyStyleArumentsFromSelectedStyleEntry : public  MenuEntry {
  void say(int entry) {
    getSL<SPEC>()->SayApplyStyleSettingsFromSelectedStyle();
  }
  void select(int entry) {
    if (menu_selected_preset == -1) {
      getSL<SPEC>()->SayNoStyleSelected();
      return;
    }
    CurrentPreset preset;
    preset.Load(menu_selected_preset);
    const char* FROM = preset.GetStyle(menu_selected_blade);
    const char* TO = GetStyle(menu_selected_blade);
    SetStyle(menu_selected_blade, style_parser.CopyArguments(FROM, TO));
    getSL<SPEC>()->SaySelect();
  }
};


template<class SPEC>
struct ResetColorsEntry : public MenuEntry {
  void say(int entry) {
    getSL<SPEC>()->SayResetColors();
  }
  void select(int entry) {
    if (menu_selected_preset == -1) {
      getSL<SPEC>()->SayNoStyleSelected();
      return;
    }
    CurrentPreset preset;
    preset.Load(menu_selected_preset);
    const char* TO = GetStyle(menu_selected_blade);
    SetStyle(menu_selected_blade, style_parser.CopyColorArguments("builtin 0 0", TO));
    getSL<SPEC>()->SaySelect();
  }
};

template<class SPEC>
struct ResetStyleArgumentsEntry : public MenuEntry {
  void say(int entry) {
    getSL<SPEC>()->SayResetStyleSettings();
  }
  void select(int entry) {
    CurrentPreset preset;
    preset.Load(menu_selected_preset);
    const char* TO = GetStyle(menu_selected_blade);
    SetStyle(menu_selected_blade, style_parser.CopyArguments("builtin 0 0", TO));
    getSL<SPEC>()->SaySelect();
  }
};

template<class SPEC>
struct SelectStyleMenu : public SPEC::MenuBase {
  uint16_t size() override {
    return NUM_BLADES * current_config->num_presets;
  }
  int blade() { return this->pos_ / NUM_BLADES; }
  int preset() { return this->pos_ % NUM_BLADES; }
  void mode_activate(bool onreturn) override {
    SPEC::MenuBase::mode_activate(onreturn);
    int preset;
    int style;
    style_parser.GetBuiltinPos(GetStyle(menu_current_blade), &preset, &style);
    this->pos_ = preset * NUM_BLADES + style;
  }
  
  void say() override {
    getSL<SPEC>()->SayBlade();
    getSL<SPEC>()->SayWhole(blade());
    getSL<SPEC>()->SayPreset();
    getSL<SPEC>()->SayWhole(preset());
  }

  void select() override {
    LSPtr<char> builtin(CurrentPreset::mk_builtin_str(preset(), blade()));
    SetStyle(menu_selected_blade, style_parser.CopyArguments(builtin.get(), GetStyle(menu_selected_blade)));
    SPEC::MenuBase::select();
  }
};

template<class SPEC>
struct EditStyleMenu : public MenuEntryMenu<SPEC,
  SubMenuEntry<typename SPEC::EditStyleOptions, typename SPEC::SoundLibrary::tEditStyleSettings>,
  typename SPEC::SelectStyleEntry,
  typename SPEC::ApplyColorsFromSelectedStyleEntry,
  typename SPEC::ApplyStyleArumentsFromSelectedStyleEntry,
  typename SPEC::ApplyColorsToAllBladesEntry,
  typename SPEC::ResetColorsEntry,
  typename SPEC::ResetStyleArgumensEntry,
  SubMenuEntry<typename SPEC::SelectStyleMenu, typename SPEC::SoundLibrary::tEditStyle>
> {};

}   // namespace mode

#endif
