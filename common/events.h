#ifndef COMMON_EVENTS_H
#define COMMON_EVENTS_H

// Max 17!
enum BUTTON : uint32_t {
  BUTTON_NONE =            0,   // used for gestures and the like
  BUTTON_POWER =           1,
  BUTTON_AUX =             2,
  BUTTON_AUX2 =            4,
  BUTTON_UP =              8,
  BUTTON_DOWN =           16,
  BUTTON_LEFT =           32,
  BUTTON_RIGHT =          64,
  BUTTON_SELECT =        128,

  // D-pad and FIRE//MODE_SELECT/CLIP_DETECT/RELOAD and RANGE have the same numbers,
  // so you can't have a D-pad in a blaster.
  BUTTON_FIRE =            8,
  BUTTON_MODE_SELECT =    16,
  BUTTON_CLIP_DETECT =    32,
  BUTTON_RELOAD =         64,
  BUTTON_RANGE =         128,

  BUTTON_BLADE_DETECT =  256,
/* For multiple blade detect part 1/5                                        // added by Oli
(Need to add part 1/5 to events.h 2/5 to config, part 3A/5 & 3B/5 to props,  // added by Oli
 part 4/5 to to ProffieOS.ino,part 5A/5 & 5B/5 to prop_base.h) */            // added by Oli
  BUTTON_BLADE_DETECT2 = 2048,                                               // added by Oli
  BUTTON_BLADE_DETECT3 = 4096,                                               // added by Oli
  MODE_ANY_BUTTON =        12,

  BUTTON_TRIGGER_ONE =      1,
  BUTTON_TRIGGER_TWO =      2,
  BUTTON_TRIGGER_THREE =    4,
  BUTTON_TRIGGER_FOUR =     8,
  BUTTON_TRIGGER_FIVE =    16,
  BUTTON_TRIGGER_SIX =     32,
  BUTTON_TRIGGER_SEVEN =   64,
  BUTTON_TRIGGER_EIGHT =  128,

  MODE_ON =              1024,
  MODE_OFF =                0,
};

// Returns value in range 0-31
constexpr uint32_t compress_button(uint32_t button) {
  return button ? 31 - __builtin_clz(button) : 0;
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
  EVENT_TWIST_RIGHT,
  EVENT_TWIST_LEFT,
  EVENT_CLASH,
  EVENT_THRUST,
  EVENT_PUSH,

  // For backwards compatibilty.
  EVENT_DOUBLE_CLICK = EVENT_SECOND_PRESSED,

  // added by Oli
  // Reseved for multi_prop or any other prop that would be used to
  // switch between multiple props with an extra-long button push.
  MULTI_PROP_EVENT_HELD_XTRA_LONG,
};

#define NO_BLADE 1000000000 /* 1 billion */

#define EVENTID(BUTTON, EVENT, MODIFIERS) (((EVENT) << 24) | (compress_button(BUTTON) << 17) | ((MODIFIERS) & ~(BUTTON)))

uint32_t current_modifiers = BUTTON_NONE;

#endif
