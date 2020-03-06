#ifndef STYLES_LOCKUP_H
#define STYLES_LOCKUP_H

#include "../functions/smoothstep.h"

#if 0
// Usage: Lockup<BASE, LOCKUP, DRAG_COLOR>
// BASE, LOCKUP: COLOR
// DRAG_COLOR: COLOR (defaults to the LOCKUP color)
// return value: COLOR
// Shows LOCKUP if the lockup state is true, otherwise BASE.
// Also handles "Drag" effect.
template<class BASE,
  class LOCKUP, class DRAG_COLOR = LOCKUP,
  class LOCKUP_SHAPE = Int<32768>, class DRAG_SHAPE = SmoothStep<Int<28671>, Int<4096>> >
class Lockup {
public:
  void run(BladeBase* blade) {
    single_pixel_ = blade->num_leds() == 1;
    base_.run(blade);
    lockup_.run(blade);
    if (!is_same_type<DRAG_COLOR, LOCKUP>::value)
      drag_.run(blade);
    lockup_shape_.run(blade);
    drag_shape_.run(blade);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor ret = base_.getColor(led);
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_DRAG: {
	int blend = single_pixel_ ? 32768 : drag_shape_.getInteger(led) >> 1;
        if (!is_same_type<DRAG_COLOR, LOCKUP>::value) {
	  ret.c = ret.c.mix2(drag_.getColor(led).c, blend);
	} else {
	  ret.c = ret.c.mix2(lockup_.getColor(led).c, blend);
	}
	break;
      }
      case SaberBase::LOCKUP_NORMAL:
      case SaberBase::LOCKUP_ARMED:
      case SaberBase::LOCKUP_AUTOFIRE:
	if (is_same_type<LOCKUP_SHAPE, Int<32768>>::value) {
	  return lockup_.getColor(led);
	} else {
	  ret.c = ret.c.mix2(lockup_.getColor(led).c, lockup_shape_.getInteger(led)>>1);
	}
      case SaberBase::LOCKUP_NONE:
	break;
    }
    return ret;
  }
private:
  bool single_pixel_;
  BASE base_;
  LOCKUP lockup_;
  DRAG_COLOR drag_;
  LOCKUP_SHAPE lockup_shape_;
  DRAG_SHAPE drag_shape_;
};
#else
// Usage: LockupL<BASE, LOCKUP, DRAG_COLOR>
// BASE, LOCKUP: COLOR
// DRAG_COLOR: COLOR (defaults to the LOCKUP color)
// return value: COLOR
// Shows LOCKUP if the lockup state is true, otherwise BASE.
// Also handles "Drag" effect.
template<
  class LOCKUP, class DRAG_COLOR = LOCKUP,
  class LOCKUP_SHAPE = Int<32768>, class DRAG_SHAPE = SmoothStep<Int<28671>, Int<4096>> >
class LockupL {
public:
  void run(BladeBase* blade) {
    single_pixel_ = blade->num_leds() == 1;
    lockup_.run(blade);
    if (!is_same_type<DRAG_COLOR, LOCKUP>::value)
      drag_.run(blade);
    lockup_shape_.run(blade);
    drag_shape_.run(blade);
  }
private:
  bool single_pixel_;
  LOCKUP lockup_;
  DRAG_COLOR drag_;
  LOCKUP_SHAPE lockup_shape_;
  DRAG_SHAPE drag_shape_;
public:
  auto getColor(int led) -> decltype(lockup_.getColor(led) * 1)  {
    // transparent
    int blend = 0;
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_DRAG: {
	blend = single_pixel_ ? 32768 : drag_shape_.getInteger(led);
        if (!is_same_type<DRAG_COLOR, LOCKUP>::value) {
	  return drag_.getColor(led) * blend;
	}
	break;
      }
      case SaberBase::LOCKUP_NORMAL:
      case SaberBase::LOCKUP_ARMED:
      case SaberBase::LOCKUP_AUTOFIRE:
	blend = lockup_shape_.getInteger(led);
	break;
      case SaberBase::LOCKUP_NONE: {
	decltype(lockup_.getColor(led) * 1) ret(Color16(), false, 0);
	return ret;
      }
    }
    return lockup_.getColor(led) * blend;
  }
};

template<class BASE,
  class LOCKUP, class DRAG_COLOR = LOCKUP,
  class LOCKUP_SHAPE = Int<32768>, class DRAG_SHAPE = SmoothStep<Int<28671>, Int<4096>> >
  using Lockup = Layers<BASE, LockupL<LOCKUP, DRAG_COLOR, LOCKUP_SHAPE, DRAG_SHAPE>>;

#endif

#endif
