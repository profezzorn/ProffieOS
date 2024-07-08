#ifndef MODE_TOP_MENU_WRAPPER_H
#define MODE_TOP_MENU_WRAPPER_H

namespace mode {

// Just makes it say "exit" when you exit the menu system.
template<class SPEC, class MENU>
class TopMenuWrapper: public MENU {
  void exit() override {
    MENU::exit();
    getSL<SPEC>()->SayExit();
  }
};

}

#endif // MODE_TOP_MENU_WRAPPER_H
