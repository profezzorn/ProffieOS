#ifndef STYLES_INOUT_HELPER_H
#define STYLES_INOUT_HELPER_H

// InOutHelperX<BASE, EXTENSION, OFF_COLOR>
// BASE, OFF_COLOR: COLOR
// EXTENSION: FUNCTION
// return value: COLOR
// This class does a basic extend/retract. Basically it fades between
// BASE and OFF_COLOR (which defaults to black). The amount of extension
// is determined by EXTENSION. If EXTENSION returns 32768, the blade is fully
// extended. If it returns zero, it is not extended.

template<class T, class EXTENSION, class OFF_COLOR=Rgb<0,0,0>, bool ALLOW_DISABLE=1 >
class InOutHelperX {
public:
  bool run(BladeBase* blade) __attribute__((warn_unused_result)) {
    base_.run(blade);
    off_color_.run(blade);
    extension_.run(blade);
    thres = (extension_.getInteger(0) * blade->num_leds()) >> 7;

    if (ALLOW_DISABLE && is_same_type<OFF_COLOR, Rgb<0,0,0> >::value && thres == 0)
      return false;
    return true;
  }
  OverDriveColor getColor(int led) {
    int black_mix = clampi32(thres - led * 256, 0, 256);
    OverDriveColor ret = base_.getColor(led);
    OverDriveColor off_color  = off_color_.getColor(led);
    ret.c = off_color.c.mix(ret.c, black_mix);
    return ret;
  }
private:
  T base_;
  OFF_COLOR off_color_;
  EXTENSION extension_;
  int thres = 0;
};

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

#include "../functions/ifon.h"
  
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
template<class ON, class OutTr, class InTr, class OFF=Rgb<0,0,0>, bool ALLOW_DISABLE=1 >
class InOutTr {
public:
  bool run(BladeBase* blade) __attribute__((warn_unused_result)) {
    on_color_.run(blade);
    off_color_.run(blade);

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
    if (ALLOW_DISABLE && is_same_type<OFF, Rgb<0,0,0> >::value &&
	!on_ && !out_active_ && !in_active_)
      return false;
    return true;
  }


  OverDriveColor runIn(OverDriveColor a, OverDriveColor b, int led) {
    if (in_active_) {
      return in_tr_.getColor(a, b, led);
    } else {
      return b;
    }
  }
  OverDriveColor runOut(OverDriveColor a, OverDriveColor b, int led) {
    if (out_active_) {
      return out_tr_.getColor(a, b, led);
    } else {
      return b;
    }
  }

  OverDriveColor getColor(int led) {
    if (!out_active_ && !in_active_) {
      if (on_) {
	return on_color_.getColor(led);
      } else {
	return off_color_.getColor(led);
      }
    } else {
      OverDriveColor on_color = on_color_.getColor(led);
      OverDriveColor off_color = off_color_.getColor(led);
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
  ON on_color_;
  OFF off_color_;
  OutTr out_tr_;
  InTr in_tr_;
};

#endif
