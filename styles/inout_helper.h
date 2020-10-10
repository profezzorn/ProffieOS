#ifndef STYLES_INOUT_HELPER_H
#define STYLES_INOUT_HELPER_H

#include "alpha.h"
#include "layers.h"
#include "../functions/ifon.h"
#include "../transitions/base.h"
  

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
      } else {
	in_tr_.begin();
      }
    }

    out_tr_.run(blade);
    in_tr_.run(blade);
    if (ALLOW_DISABLE && !on_ && !out_tr_ && !in_tr_)
      return can_turn_off;
    return LayerRunResult::UNKNOWN;
  }

private:
  bool on_ = false;
  OFF off_color_;
  TransitionHelper<OutTr> out_tr_;
  TransitionHelper<InTr> in_tr_;
public:
  auto getColor(int led) -> decltype(
    MixColors(out_tr_.getColor(in_tr_.getColor(RGBA_um_nod::Transparent(), off_color_.getColor(led), led), RGBA_um_nod::Transparent(), led),
	      in_tr_.getColor(out_tr_.getColor(off_color_.getColor(led), RGBA_um_nod::Transparent(), led), off_color_.getColor(led), led), 1, 1)) {
    if (!out_tr_ && !in_tr_) {
      if (on_) {
	return RGBA_um_nod::Transparent();
      } else {
	return off_color_.getColor(led);
      }
    } else {
      RGBA_um_nod on_color = RGBA_um_nod::Transparent();
      auto off_color = off_color_.getColor(led);
      if (on_) {
	return out_tr_.getColor(in_tr_.getColor(on_color, off_color, led), on_color, led);
      } else {
	return in_tr_.getColor(out_tr_.getColor(off_color, on_color, led), off_color, led);
      }
    }
  }
};

template<class ON, class OutTr, class InTr, class OFF=Rgb<0,0,0>, bool ALLOW_DISABLE=1 >
  using InOutTr = Layers<ON, InOutTrL<OutTr, InTr, OFF, ALLOW_DISABLE>>;


#endif
