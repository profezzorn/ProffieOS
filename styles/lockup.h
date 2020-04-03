#ifndef STYLES_LOCKUP_H
#define STYLES_LOCKUP_H

#include "../functions/smoothstep.h"
#include "../functions/islessthan.h"
#include "../functions/layer_functions.h"
#include "../functions/scale.h"
#include "../functions/brown_noise.h"


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

// Usage: Lockup<BASE, LOCKUP, DRAG_COLOR, LOCKUP_SHAPE, DRAG_SHAPE, LB_SHAPE>
// Or: LockupL<LOCKUP, DRAG_COLOR, LOCKUP_SHAPE, DRAG_SHAPE, LB_SHAPE>
// BASE, LOCKUP: COLOR
// DRAG_COLOR: COLOR (defaults to the LOCKUP color)
// LOCKUP_SHAPE: FUNCTION (defaults to Int<32768>)
// DRAG_SHAPE: FUNCTION (defaults to SmoothStep<Int<28671>, Int<4096>>)
// LB_SHAPE: FUNCTION (defaults to a suitable function)
// return value: COLOR
// Shows LOCKUP if the lockup state is true, otherwise BASE.
// Also handles "Drag" effect.
template<
  class LOCKUP, class DRAG_COLOR = LOCKUP,
  class LOCKUP_SHAPE = Int<32768>,
  class DRAG_SHAPE = SmoothStep<Int<28671>, Int<4096>>,
  class LB_SHAPE = LayerFunctions<Bump<Scale<SlowNoise<Int<2000>>,Int<3000>,Int<16000>>,
				Scale<BrownNoiseF<Int<10>>,Int<14000>,Int<8000>>>,
			Bump<Scale<SlowNoise<Int<2300>>,Int<26000>,Int<8000>>,
				Scale<NoisySoundLevel,Int<5000>,Int<10000>>>,
			Bump<Scale<SlowNoise<Int<2300>>,Int<20000>,Int<30000>>,
				Scale<IsLessThan<SlowNoise<Int<1500>>,Int<8000>>,Scale<NoisySoundLevel,Int<5000>,Int<0>>,Int<0>>>>  >
class LockupL {
public:
  void run(BladeBase* blade) {
    single_pixel_ = blade->num_leds() == 1;
    lockup_.run(blade);
    if (!is_same_type<DRAG_COLOR, LOCKUP>::value)
      drag_.run(blade);
    lockup_shape_.run(blade);
    drag_shape_.run(blade);
    lb_shape_.run(blade);
    handled_ = blade->current_style()->IsHandled(FeatureForLockupType(SaberBase::Lockup()));
  }
private:
  bool handled_;
  bool single_pixel_;
  LOCKUP lockup_;
  DRAG_COLOR drag_;
  LOCKUP_SHAPE lockup_shape_;
  DRAG_SHAPE drag_shape_;
  LB_SHAPE lb_shape_;
public:
  auto getColor(int led) -> decltype(lockup_.getColor(led) * 1)  {
    // transparent
    int blend = 0;
    if (handled_) return RGBA_um::Transparent();
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_MELT:
	// TODO: Better default for MELT?
      case SaberBase::LOCKUP_DRAG: {
	blend = single_pixel_ ? 32768 : drag_shape_.getInteger(led);
        if (!is_same_type<DRAG_COLOR, LOCKUP>::value) {
	  return drag_.getColor(led) * blend;
	}
	break;
      }
      case SaberBase::LOCKUP_LIGHTNING_BLOCK:
	blend = lb_shape_.getInteger(led);
	break;
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


// Usage: LockupTr<BASE, COLOR, BeginTr, EndTr, LOCKUP_TYPE>
// Or: LockupTrL<COLOR, BeginTr, EndTr, LOCKUP_TYPE>
// COLOR; COLOR or LAYER
// BeginTr, EndTr: TRANSITION
// LOCKUP_TYPE: a SaberBase::LockupType
// Return type: LAYER
// This layer creates a complete lockup effect.
// When lockup is initiated, BeginTr is used to transition from transparent
// to COLOR. When lockup ends, EndTr is used to transition from COLOR to
// transparent again. If you wish to for your lockup to have a shape, you
// can have COLOR be partially transparent to make the base layer show through.
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
      begin_tr_.run(blade);
      if (begin_tr_.done()) begin_active_ = false;
    }
    if (end_active_) {
      end_tr_.run(blade);
      if (end_tr_.done()) end_active_ = false;
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
    SCOPED_PROFILER();
    RGBA off_color = RGBA_um::Transparent();
    if (!begin_active_ && !end_active_) {
      if (active_) {
	return color_.getColor(led);
      } else {
	return off_color;
      }
    } else {
      RGBA on_color = color_.getColor(led);
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
  EndTr end_tr_;
};
    
template<
  class BASE,
  class COLOR,
  class BeginTr,
  class EndTr,
  SaberBase::LockupType LOCKUP_TYPE>
  using LockupTr = Layers<BASE, LockupTrL<COLOR, BeginTr, EndTr, LOCKUP_TYPE>>;
  

#endif
