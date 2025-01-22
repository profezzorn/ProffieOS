#ifndef MODES_SELECT_CANCEL_MODE_H
#define MODES_SELECT_CANCEL_MODE_H

#include "mode.h"

namespace mode {

struct SelectCancelMode : public ModeInterface {
  virtual void select() { popMode(); }
  virtual void exit() { popMode(); }

  bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, 0)) {
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, 0):
	select();
	return true;

      case EVENTID(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT, 0):
      case EVENTID(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT, 0):
	exit();
	return true;
    }
    return false;
  }

  bool mode_Parse(const char *cmd, const char* arg) override {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "select") || !strcmp(cmd, "sel")) {
      select();
      return true;
    }
    if (!strcmp(cmd, "cancel") || !strcmp(cmd, "can")) {
      exit();
      return true;
    }
#endif
    return false;
  }
};

} // namespace mode

#endif
