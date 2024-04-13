#ifndef SELECT_CANCEL_MODE_H
#define SELECT_CANCEL_MODE_H

#include "mode.h"

namespace mode {

struct SelectCancelMode : public ModeInterface {
  virtual void select() = 0;
  virtual void exit() = 0;

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
};

} // namespace mode

#endif
