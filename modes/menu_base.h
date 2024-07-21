#ifndef MODE_MENU_BASE_H
#define MODE_MENU_BASE_H

namespace mode {

// Base class for menues with a known size.
template<class SPEC>
struct MenuBase : public SPEC::SteppedMode {
  virtual void say() = 0;
  virtual uint16_t size() = 0;

  void exit() override {
    getSL<SPEC>()->SayCancel();
    SPEC::SteppedMode::exit();
  }
  
  void select() override {
    getSL<SPEC>()->SaySelect();
    SPEC::SteppedMode::select();
  }
  
  void mode_activate(bool onreturn) override {
    SPEC::SteppedMode::mode_activate(onreturn);
    say();
  }
  
  void fadeout(float len) {
    getSL<SPEC>()->fadeout(len);
  }
  
  void next() override {
    pos_ = MOD(pos_ + 1, size());
  }
  void prev() override {
    pos_ = MOD(pos_ - 1, size());
  }

  uint16_t pos_;
};

}  // namespace mode

#endif
