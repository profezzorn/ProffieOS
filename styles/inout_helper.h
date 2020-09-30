#ifndef STYLES_INOUT_HELPER_H
#define STYLES_INOUT_HELPER_H

#include "alpha.h"
#include "layers.h"
#include "../functions/ifon.h"
  

// InOutHelperX<BASE, EXTENSION, OFF_COLOR>
// BASE, OFF_COLOR: COLOR
// EXTENSION: FUNCTION
// return value: COLOR
// This class does a basic extend/retract. Basically it fades between
// BASE and OFF_COLOR (which defaults to black). The amount of extension
// is determined by EXTENSION. If EXTENSION returns 32768, the blade is fully
// extended. If it returns zero, it is not extended.

template<class EXTENSION, class OFF_COLOR=Rgb<0,0,0>, bool ALLOW_DISABLE=1 >
  using InOutHelperL = AlphaL<OFF_COLOR, InOutHelperF<EXTENSION, ALLOW_DISABLE>>;

template<class T, class EXTENSION, class OFF_COLOR=Rgb<0,0,0>, bool ALLOW_DISABLE=1 >
  using InOutHelperX = Layers<T, InOutHelperL<EXTENSION, OFF_COLOR, ALLOW_DISABLE>>;

// Usage: InOutHelper<BASE, OUT_MILLIS, IN_MILLIS>
// or: InOutHelper<BASE, OUT_MILLIS, IN_MILLIS, OFF_COLOR>
// BASE, OFF_COLOR: COLOR
// OUT_MILLIS, IN_MILLIS: a number
// return value: COLOR
// This class does a basic extend/retract. Basically it fades between
// BASE and OFF_COLOR (which defaults to black). It starts by just
// displaying OFF_COLOR, and when you turn the saber on it starts mixing
// in BASE at the base of the saber. After OUT_MILLIS milliseconds, it
// will be displaying the BASE color on the entire blade.

template<class T, int OUT_MILLIS, int IN_MILLIS, class OFF_COLOR=Rgb<0,0,0> >
  using InOutHelper = InOutHelperX<T, InOutFunc<OUT_MILLIS, IN_MILLIS>, OFF_COLOR>;

template<class T, int OUT_MILLIS, int IN_MILLIS, int EXPLODE_MILLIS, class OFF_COLOR=Rgb<0,0,0> >
  using InOutHelperTD = InOutHelperX<T, InOutFuncTD<OUT_MILLIS, IN_MILLIS, EXPLODE_MILLIS>, OFF_COLOR>;


// InOutTr<BASE, OUT_TRANSITION, IN_TRANSITION, OFF_COLOR>
// BASE, OFF_COLOR: COLOR
// OUT_TRANSITION, IN_TRANSITION: TRANSITION
// return value: COLOR
// Similar to InOutHelper<>, but uses configuratble transitions
// to go to and from the BASE to the OFF_COLOR.
template<class OutTr, class InTr, class OFF=Rgb<0,0,0>, bool ALLOW_DISABLE=1 >
class InOutTrL {
public:
  LayerRunResult run(BladeBase* blade) __attribute__((warn_unused_result)) {
    LayerRunResult can_turn_off = RunLayer(&off_color_, blade);

    if (on_ != blade->is_on()) {
      if ((on_ = blade->is_on())) {
	out_tr_.begin();
	out_active_ = true;
      } else {
	in_tr_.begin();
	in_active_ = true;
      }
    }

    if (out_active_) {
      out_tr_.run(blade);
      if (out_tr_.done()) out_active_ = false;
    }
    if (in_active_) {
      in_tr_.run(blade);
      if (in_tr_.done()) in_active_ = false;
    }
    if (ALLOW_DISABLE && !on_ && !out_active_ && !in_active_)
      return can_turn_off;
    return LayerRunResult::UNKNOWN;
  }


  RGBA runIn(RGBA a, RGBA b, int led) {
    if (in_active_) {
      return in_tr_.getColor(a, b, led);
    } else {
      return b;
    }
  }
  RGBA runOut(RGBA a, RGBA b, int led) {
    if (out_active_) {
      return out_tr_.getColor(a, b, led);
    } else {
      return b;
    }
  }

  RGBA getColor(int led) {
    if (!out_active_ && !in_active_) {
      if (on_) {
	return RGBA_um_nod::Transparent();
      } else {
	return off_color_.getColor(led);
      }
    } else {
      RGBA on_color = RGBA_um_nod::Transparent();
      RGBA off_color = off_color_.getColor(led);
      if (on_) {
	return runOut(runIn(on_color, off_color, led), on_color, led);
      } else {
	return runIn(runOut(off_color, on_color, led), off_color, led);
      }
    }
  }
private:
  bool on_ = false;
  bool out_active_ = false;
  bool in_active_ = false;
  OFF off_color_;
  OutTr out_tr_;
  InTr in_tr_;
};

template<class ON, class OutTr, class InTr, class OFF=Rgb<0,0,0>, bool ALLOW_DISABLE=1 >
  using InOutTr = Layers<ON, InOutTrL<OutTr, InTr, OFF, ALLOW_DISABLE>>;


#endif
