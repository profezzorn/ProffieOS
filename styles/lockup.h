#ifndef STYLES_LOCKUP_H
#define STYLES_LOCKUP_H

#include "../functions/smoothstep.h"
#include "../functions/islessthan.h"
#include "../functions/layer_functions.h"
#include "../functions/scale.h"
#include "../functions/brown_noise.h"
#include "../transitions/base.h"


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

// Usage: Lockup<BASE, LOCKUP, DRAG_COLOR, LOCKUP_SHAPE, DRAG_SHAPE>
// Or: LockupL<LOCKUP, DRAG_COLOR, LOCKUP_SHAPE, DRAG_SHAPE, LB_SHAPE>
// BASE, LOCKUP: COLOR
// DRAG_COLOR: COLOR (defaults to the LOCKUP color)
// LOCKUP_SHAPE: FUNCTION (defaults to Int<32768>)
// DRAG_SHAPE: FUNCTION (defaults to SmoothStep<Int<28671>, Int<4096>>)
// LB_SHAPE: FUNCTION (defaults to a suitable function)
// return value: COLOR
// Shows LOCKUP if the lockup state is true, otherwise BASE.
// Also handles Drag, Melt and Lightning Block lockup types unless those
// are handled elsewhere in the same style.
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
  PONUA LOCKUP lockup_;
  PONUA DRAG_COLOR drag_;
  PONUA LOCKUP_SHAPE lockup_shape_;
  PONUA DRAG_SHAPE drag_shape_;
  PONUA LB_SHAPE lb_shape_;
public:
  auto getColor(int led) -> decltype(lockup_.getColor(led) * 1)  {
    // transparent
    int blend = 0;
    if (handled_) return RGBA_um_nod::Transparent();
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
	return RGBA_um_nod::Transparent();
    }
    return lockup_.getColor(led) * blend;
  }
};

template<class BASE,
  class LOCKUP, class DRAG_COLOR = LOCKUP,
  class LOCKUP_SHAPE = Int<32768>, class DRAG_SHAPE = SmoothStep<Int<28671>, Int<4096>> >
  using Lockup = Layers<BASE, LockupL<LOCKUP, DRAG_COLOR, LOCKUP_SHAPE, DRAG_SHAPE>>;

// Usage: LockupTr<BASE, COLOR, BeginTr, EndTr, LOCKUP_TYPE, CONDITION>
// Or: LockupTrL<COLOR, BeginTr, EndTr, LOCKUP_TYPE, CONDITION>
// COLOR; COLOR or LAYER
// BeginTr, EndTr: TRANSITION
// LOCKUP_TYPE: a SaberBase::LockupType
// Return type: LAYER
// This layer creates a complete lockup effect.
// When lockup is initiated, BeginTr is used to transition from transparent
// to COLOR. When lockup ends, EndTr is used to transition from COLOR to
// transparent again. If you wish to for your lockup to have a shape, you
// can have COLOR be partially transparent to make the base layer show through.
// If CONDITION equals 0, Lockup effect ignored
enum class LockupTrState {
  INACTIVE,
  ACTIVE,
  SKIPPED
};

template<class COLOR,
  class BeginTr,
  class EndTr,
  SaberBase::LockupType LOCKUP_TYPE,
  class CONDITION = Int<1>>
class LockupTrL {
public:
  LockupTrL() {
    BladeBase::HandleFeature(FeatureForLockupType(LOCKUP_TYPE));
  }
  void run(BladeBase* blade) {
      color_.run(blade);
      condition_.run(blade);
      switch (active_) {
        case LockupTrState::INACTIVE:
          if (SaberBase::Lockup() == LOCKUP_TYPE) {
             if (condition_.calculate(blade)) {
               active_ = LockupTrState::ACTIVE;
               begin_tr_.begin();
             } else {
               active_ = LockupTrState::SKIPPED;
             }
          }
          break;
        case LockupTrState::ACTIVE:
          if (SaberBase::Lockup() != LOCKUP_TYPE) {
            end_tr_.begin();
            active_ = LockupTrState::INACTIVE;
          }
          break;
       case LockupTrState::SKIPPED:
          if (SaberBase::Lockup() != LOCKUP_TYPE) {
            active_ = LockupTrState::INACTIVE;
          }
         break;
       }
       begin_tr_.run(blade);
       end_tr_.run(blade);
    }

private:
  LockupTrState active_ = LockupTrState::INACTIVE;
  PONUA SVFWrapper<CONDITION> condition_;
  PONUA COLOR color_;
  TransitionHelper<BeginTr> begin_tr_;
  TransitionHelper<EndTr> end_tr_;

public:
  auto getColor(int led) -> decltype(
    MixColors(end_tr_.getColor(begin_tr_.getColor(RGBA_um_nod::Transparent(), color_.getColor(led), led), RGBA_um_nod::Transparent(), led),
        begin_tr_.getColor(end_tr_.getColor(color_.getColor(0), RGBA_um_nod::Transparent(), led), color_.getColor(0), led), 1, 1)) {
    SCOPED_PROFILER();
    RGBA_um_nod off_color = RGBA_um_nod::Transparent();
    if (!begin_tr_ && !end_tr_) {
      if (active_ == LockupTrState::ACTIVE) {
        return color_.getColor(led);
      } else {
        return off_color;
      }
    } else {
      auto on_color = color_.getColor(led);
      if (active_ == LockupTrState::ACTIVE) {
        return begin_tr_.getColor(end_tr_.getColor(on_color, off_color, led), on_color, led);
      } else {
        return end_tr_.getColor(begin_tr_.getColor(off_color, on_color, led), off_color, led);
      }
    }
  }
};
    
template<
  class BASE,
  class COLOR,
  class BeginTr,
  class EndTr,
  SaberBase::LockupType LOCKUP_TYPE,
  class CONDITION = Int<1>>
  using LockupTr = Layers<BASE, LockupTrL<COLOR, BeginTr, EndTr, LOCKUP_TYPE, CONDITION>>;
  

#endif
