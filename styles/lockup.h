#ifndef STYLES_LOCKUP_H
#define STYLES_LOCKUP_H

#include "../functions/smoothstep.h"

// Usage: Lockup<BASE, LOCKUP, DRAG_COLOR>
// BASE, LOCKUP: COLOR
// DRAG_COLOR: COLOR (defaults to the LOCKUP color)
// return value: COLOR
// Shows LOCKUP if the lockup state is true, otherwise BASE.
// Also handles "Drag" effect.
template<class BASE,
  class LOCKUP, class DRAG_COLOR = LOCKUP,
  class LOCKUP_SHAPE = Int<32768>, class DRAG_SHAPE = SmoothStep<Int<32000>, Int<8000>> >
class Lockup {
public:
  void run(BladeBase* blade) {
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
        if (!is_same_type<DRAG_COLOR, LOCKUP>::value) {
	  ret.c = ret.c.mix2(drag_.getColor(led).c, drag_shape_.getInteger(led)>>1);
	} else {
	  ret.c = ret.c.mix2(lockup_.getColor(led).c, drag_shape_.getInteger(led)>>1);
	}
	break;
      case SaberBase::LOCKUP_NORMAL:
      case SaberBase::LOCKUP_ARMED:
	if (is_same_type<LOCKUP_SHAPE, Int<32768>>::value) {
	  return lockup_.getColor(led);
	} else {
	  ret.c = ret.c.mix2(lockup_.getColor(led).c, lockup_shape_.getInteger(led)>>1);
	}
      case SaberBase::LOCKUP_NONE:
	break;
      }
    }
    return ret;
  }
private:
  BASE base_;
  LOCKUP lockup_;
  DRAG_COLOR drag_;
  LOCKUP_SHAPE lockup_shape_;
  DRAG_SHAPE drag_shape_;
};

#endif
