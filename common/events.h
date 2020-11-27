#ifndef COMMON_EVENTS_H
#define COMMON_EVENTS_H

// Max 32, but some values can share if not used together.
enum BUTTON : uint32_t {
  BUTTON_NONE = 0,   // used for gestures and the like
  MODE_ON = 1,

  BUTTON_POWER = 1U << 1,
  BUTTON_AUX = 1U << 2,
  BUTTON_AUX2 = 1U << 3,

  // DPAD
  BUTTON_UP = 1U << 4,
  BUTTON_DOWN = 1U << 5,
  BUTTON_LEFT = 1U << 6,
  BUTTON_RIGHT = 1U << 7,
  BUTTON_SELECT = 1U << 8,

  // D-pad and FIRE//MODE_SELECT/CLIP_DETECT/RELOAD and RANGE have the same numbers,
  // so you can't have a D-pad in a blaster.
  BUTTON_FIRE = 1U << 4,
  BUTTON_MODE_SELECT = 1U << 5,
  BUTTON_CLIP_DETECT = 1U << 6,
  BUTTON_RELOAD = 1U << 7,
  BUTTON_RANGE = 1U << 8,

  BUTTON_BLADE_DETECT = 1U << 9,
  MODE_ANY_BUTTON = 1U << 10,

  // MICOM buttons
  BUTTON_TRIGGER_ONE = 1U << 1,
  BUTTON_TRIGGER_TWO = 1U << 2,
  BUTTON_TRIGGER_THREE = 1U << 3,
  BUTTON_TRIGGER_FOUR = 1U << 4,
  BUTTON_TRIGGER_FIVE = 1U << 5,
  BUTTON_TRIGGER_SIX = 1U << 6,
  BUTTON_TRIGGER_SEVEN = 1U << 7,
  BUTTON_TRIGGER_EIGHT = 1U << 8,
    
  MODE_OFF = 0,

  // REMOTE buttons
  BUTTON_FF = 1U << 11,
  BUTTON_PLAY = 1U << 13,
  BUTTON_REW = 1U << 12,

  BUTTON_REPEAT = 1U << 14,
  BUTTON_SKIP = 1U << 15,

  BUTTON_NEXT = 1U << 16,
  BUTTON_PAUSE = 1U << 9,  // same as blade detect
  BUTTON_PREV = 1U << 17,

  BUTTON_CHUP = 1U << 18,
  BUTTON_CHDN = 1U << 19,

  BUTTON_VOLUP = 1U << 20,
  BUTTON_VOLDN = 1U << 21,

  BUTTON_A = 1U << 22,
  BUTTON_B = 1U << 23,
  BUTTON_C = 1U << 24,
  BUTTON_D = 1U << 25,

  BUTTON_OPTIONS = 1U << 26,
  BUTTON_MENU = 1U << 27,
  BUTTON_EXIT = 1U << 28,

  BUTTON_TOP1 = 1U << 29,
  BUTTON_TOP2 = 1U << 30,
  BUTTON_TOP3 = 1U << 31,

  // Same as aux / aux2
  BUTTON_SCREEN_LEFT = 1U << 2,
  BUTTON_SCREEN_RIGHT = 1U << 3,
};

// Returns value in range 0-31
constexpr uint32_t compress_button(uint32_t button) {
  return button ? 31 - __builtin_clz(button) : 0;
}

// Returns unique values for 0-2 buttons.
// Result must fit in 17 bits.
constexpr uint32_t compress_button_mask(uint32_t buttons) {
  return buttons % 36238U;
}

enum EVENT : uint32_t {
  EVENT_NONE = 0,

  // These five sections must be identical!
  EVENT_PRESSED,
  EVENT_RELEASED,
  EVENT_HELD,
  EVENT_HELD_MEDIUM,
  EVENT_HELD_LONG,
  EVENT_CLICK_SHORT,
  EVENT_SAVED_CLICK_SHORT,
  EVENT_CLICK_LONG,

  EVENT_FIRST_PRESSED,
  EVENT_FIRST_RELEASED,
  EVENT_FIRST_HELD,
  EVENT_FIRST_HELD_MEDIUM,
  EVENT_FIRST_HELD_LONG,
  EVENT_FIRST_CLICK_SHORT,
  EVENT_FIRST_SAVED_CLICK_SHORT,
  EVENT_FIRST_CLICK_LONG,
    
  EVENT_SECOND_PRESSED,
  EVENT_SECOND_RELEASED,
  EVENT_SECOND_HELD,
  EVENT_SECOND_HELD_MEDIUM,
  EVENT_SECOND_HELD_LONG,
  EVENT_SECOND_CLICK_SHORT,
  EVENT_SECOND_SAVED_CLICK_SHORT,
  EVENT_SECOND_CLICK_LONG,

  EVENT_THIRD_PRESSED,
  EVENT_THIRD_RELEASED,
  EVENT_THIRD_HELD,
  EVENT_THIRD_HELD_MEDIUM,
  EVENT_THIRD_HELD_LONG,
  EVENT_THIRD_CLICK_SHORT,
  EVENT_THIRD_SAVED_CLICK_SHORT,
  EVENT_THIRD_CLICK_LONG,

  EVENT_FOURTH_PRESSED,
  EVENT_FOURTH_RELEASED,
  EVENT_FOURTH_HELD,
  EVENT_FOURTH_HELD_MEDIUM,
  EVENT_FOURTH_HELD_LONG,
  EVENT_FOURTH_CLICK_SHORT,
  EVENT_FOURTH_SAVED_CLICK_SHORT,
  EVENT_FOURTH_CLICK_LONG,

  EVENT_LATCH_ON,
  EVENT_LATCH_OFF,
  EVENT_STAB,
  EVENT_SWING,
  EVENT_SHAKE,
  EVENT_TWIST,
  EVENT_CLASH,
  EVENT_THRUST,
  EVENT_PUSH,

    // For backwards compatibilty.
  EVENT_DOUBLE_CLICK = EVENT_SECOND_PRESSED,
};

#define NO_BLADE 1000000000 /* 1 billion */

#define EVENTID(BUTTON, EVENT, MODIFIERS) (((EVENT) << 24) | (compress_button(BUTTON) << 17) | compress_button_mask((MODIFIERS) & ~(BUTTON)))

uint32_t current_modifiers = BUTTON_NONE;

class EventHandler {
public:
  virtual bool Event(enum BUTTON button, EVENT event, uint32_t modifiers) = 0;
};

#endif
