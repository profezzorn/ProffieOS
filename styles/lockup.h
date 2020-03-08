#ifndef STYLES_LOCKUP_H
#define STYLES_LOCKUP_H

#include "../functions/smoothstep.h"

HandledFeature FeatureForLockupType(SaberBase::LockupType t) {
  switch (t) {
    default:
      return HANDLED_FEATURE_NONE;
    case SaberBase::LOCKUP_DRAG:
      return HANDLED_FEATURE_DRAG;
    case SaberBase::LOCKUP_LIGHTNING_BLOCK:
      return HANDLED_FEATURE_LIGHTNING_BLOCK;
    case SaberBase::LOCKUP_MELT:
      return HANDLED_FEATURE_MELT;
  }
}

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
    handled_ = blade->current_style()->IsHandled(FeatureForLockupType(SaberBase::Lockup()));
  }
  OverDriveColor getColor(int led) {
    OverDriveColor ret = base_.getColor(led);
    if (!handled_)
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_MELT:
	// TODO: Better default for MELT
      case SaberBase::LOCKUP_DRAG: {
	int blend = single_pixel_ ? 32768 : drag_shape_.getInteger(led) >> 1;
        if (!is_same_type<DRAG_COLOR, LOCKUP>::value) {
	  ret.c = ret.c.mix2(drag_.getColor(led).c, blend);
	} else {
	  ret.c = ret.c.mix2(lockup_.getColor(led).c, blend);
	}
	break;
      }
      case SaberBase::LOCKUP_LIGHTNING_BLOCK:
	// TODO: Better default for LB
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
  bool handled_;
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
    handled_ = blade->current_style()->IsHandled(FeatureForLockupType(SaberBase::Lockup()));
  }
private:
  bool handled_;
  bool single_pixel_;
  LOCKUP lockup_;
  DRAG_COLOR drag_;
  LOCKUP_SHAPE lockup_shape_;
  DRAG_SHAPE drag_shape_;
public:
  auto getColor(int led) -> decltype(lockup_.getColor(led) * 1)  {
    // transparent
    int blend = 0;
    if (handled_) return RGBA_um::Transparent();
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_MELT:
	// TODO: Better default for MELT
      case SaberBase::LOCKUP_DRAG: {
	blend = single_pixel_ ? 32768 : drag_shape_.getInteger(led);
        if (!is_same_type<DRAG_COLOR, LOCKUP>::value) {
	  return drag_.getColor(led) * blend;
	}
	break;
      }
      case SaberBase::LOCKUP_LIGHTNING_BLOCK:
	// TODO: Better default for LB
      case SaberBase::LOCKUP_NORMAL:
      case SaberBase::LOCKUP_ARMED:
      case SaberBase::LOCKUP_AUTOFIRE:
	blend = lockup_shape_.getInteger(led);
	break;
      case SaberBase::LOCKUP_NONE:
	return RGBA_um::Transparent();
    }
    return lockup_.getColor(led) * blend;
  }
};

template<class BASE,
  class LOCKUP, class DRAG_COLOR = LOCKUP,
  class LOCKUP_SHAPE = Int<32768>, class DRAG_SHAPE = SmoothStep<Int<28671>, Int<4096>> >
  using Lockup = Layers<BASE, LockupL<LOCKUP, DRAG_COLOR, LOCKUP_SHAPE, DRAG_SHAPE>>;

#endif


template<class COLOR,
  class BeginTr,
  class EndTr,
  SaberBase::LockupType LOCKUP_TYPE>
class LockupTrL {
public:
  LockupTrL() {
    BladeBase::HandleFeature(FeatureForLockupType(LOCKUP_TYPE));
  }
  void run(BladeBase* blade) {
    color_.run(blade);
    if (active_ != (SaberBase::Lockup() == LOCKUP_TYPE)) {
      if ((active_ = (SaberBase::Lockup() == LOCKUP_TYPE))) {
	begin_tr_.begin();
	begin_active_ = true;
      } else {
	end_tr_.begin();
	end_active_ = true;
      }
    }

    if (begin_active_) {
      if (begin_tr_.done()) {
	begin_active_ = false;
      } else {
	begin_tr_.run(blade);
      }
    }
    if (end_active_) {
      if (end_tr_.done()) {
	end_active_ = false;
      } else {
	end_tr_.run(blade);
      }
    }
  }

  RGBA runBegin(RGBA a, RGBA b, int led) {
    if (begin_active_) {
      return begin_tr_.getColor(a, b, led);
    } else {
      return b;
    }
  }
  RGBA runEnd(RGBA a, RGBA b, int led) {
    if (end_active_) {
      return end_tr_.getColor(a, b, led);
    } else {
      return b;
    }
  }
  RGBA getColor(int led) {
    RGBA on_color = color_.getColor(led);
    RGBA off_color = RGBA_um::Transparent();
    if (!begin_active_ && !end_active_) {
      if (active_) {
	return on_color;
      } else {
	return off_color;
      }
    } else {
      if (active_) {
	return runBegin(runEnd(on_color, off_color, led), on_color, led);
      } else {
	return runEnd(runBegin(off_color, on_color, led), off_color, led);
      }
    }
  }
private:
  bool active_;
  COLOR color_;
  bool begin_active_;
  bool end_active_;
  BeginTr begin_tr_;
  BeginTr end_tr_;
};
    
template<
  class BASE,
  class COLOR,
  class BeginTr,
  class EndTr,
  SaberBase::LockupType LOCKUP_TYPE>
  using LockupTr = Layers<BASE, LockupTrL<COLOR, BeginTr, EndTr, LOCKUP_TYPE>>;
  

#endif
