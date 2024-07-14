#ifndef MODE_TOP_MENU_WRAPPER_H
#define MODE_TOP_MENU_WRAPPER_H

namespace mode {

// Just makes it say "exit" when you exit the menu system.
template<class SPEC, class MENU>
class TopMenuWrapper: public MENU {
  void mode_activate(bool onreturn) override {
    if (!onreturn) {
      volHum_ = font_config.volHum;
      volEff_ = font_config.volEff;
      font_config.volHum = 1;
      font_config.volEff = 1;
    }
    MENU::mode_activate(onreturn);
  }
  void exit() override {
    MENU::exit();
    getSL<SPEC>()->SayExit();
    font_config.volHum = volHum_;
    font_config.volEff = volEff_;
    
  }
private:
  int volHum_;
  int volEff_;
  
};

}

#endif // MODE_TOP_MENU_WRAPPER_H
