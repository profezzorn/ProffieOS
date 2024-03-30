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
  int get() override { return GetIntArg(menu_current_blade, menu_current_arg); }
  float t(int x) { return powf(x / 32768.0f, 2.0) * 30.0; }
  void set(int x) {
    value_ = x;
    if (!getSL<SPEC>()->busy()) {
      getSL<SPEC>()->SayNumber(t(x), SAY_DECIMAL);
      getSL<SPEC>()->SaySeconds();
    }
  }

  void select() override {
    SPEC::SmoothMode::select();
    SetIntArg(menu_current_blade, menu_current_arg, (int)(t(value_) * 1000));
    popMode();
  }

private:  
  int value_ = 0;
};

template<class SPEC>
class SelectArgNumber : public SPEC::MenuBase {
public:
  void activate(bool onreturn) override {
    SPEC::MenuBase::activate(onreturn);
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
    SPEC::SmoothMode::select();
    SetIntArg(menu_current_blade, menu_current_arg, this->pos_);
    popMode();
  }
private:
  uint16_t max_;
};

template<class SPEC>
class SelectArgMode : public SPEC::MenuBase {
public:
  void activate(bool onreturn) override {
    // TODO: Set pos_ to something reasonable?
    arginfo_ = style_parser.GetArgInfo(GetStyle(menu_current_blade));
    SPEC::MenuBase::activate(onreturn);
  }
  int size() override { return arginfo_.used(); }
  void say() override {
    getSL<SPEC>()->SayArgument(getCurrentArgument());
  }

  void select() override {
    menu_current_arg = getCurrentArgument();
    if (arginfo_.iscolor(menu_current_arg)) {
      pushMode<SPEC::SelectArgColor>();
    } else {
      int max_arg = GetMaxStyleArg();
      if (max_arg == 32768 || max_arg == 32767) {
	pushMode<SPEC::SelectArgSmooth>();
      } else if (max_arg <= 0 && isTimeArg(menu_current_arg)) {
	pushMode<SPEC::SelectArgTime>();
      } else {
	pushMode<SPEC::SelectArgNumber>();
      }
    }
  }

protected:
  int getCurrentArgument() { return arginfo_.nth(this->pos_); }
  ArgInfo arginfo_;
};

}   // namespace mode

#endif
