#ifndef COMMON_EVENTS_H
#define COMMON_EVENTS_H

enum BUTTON : uint32_t {
  BUTTON_NONE = 0,   // used for gestures and the like
  BUTTON_POWER = 1,
  BUTTON_AUX = 2,
  BUTTON_AUX2 = 4,
  BUTTON_UP = 8,
  BUTTON_DOWN = 16,
  BUTTON_LEFT = 32,
  BUTTON_RIGHT = 64,

  // D-pad and FIRE/RELOAD/CLIP_DETECT have the same numbers,
  // so you can't have a D-pad in a blaster.
  BUTTON_FIRE = 8,
  BUTTON_RELOAD = 16,
  BUTTON_CLIP_DETECT = 32,
    
  BUTTON_SELECT = 128,
  BUTTON_BLADE_DETECT = 256,
  MODE_ANY_BUTTON = 512,
  MODE_ON = 1024,
  MODE_OFF = 0,
};

enum EVENT : uint32_t {
  EVENT_NONE = 0,
  EVENT_PRESSED,
  EVENT_RELEASED,
  EVENT_HELD,
  EVENT_HELD_MEDIUM,
  EVENT_HELD_LONG,
  EVENT_CLICK_SHORT,
  EVENT_CLICK_LONG,
  EVENT_DOUBLE_CLICK, // Note, will also generate a short click
  EVENT_LATCH_ON,
  EVENT_LATCH_OFF,
  EVENT_STAB,
  EVENT_SWING,
  EVENT_SHAKE,
  EVENT_TWIST,
  EVENT_CLASH,
};

#define NO_BLADE 1000000000 /* 1 billion */

#define EVENTID(BUTTON, EVENT, MODIFIERS) (((EVENT) << 24) | ((BUTTON) << 12) | ((MODIFIERS) & ~(BUTTON)))

uint32_t current_modifiers = BUTTON_NONE;

#endif
