#ifndef STYLES_LOCKUP_H
#define STYLES_LOCKUP_H

// Usage: Lockup<BASE, LOCKUP, DRAG_COLOR>
// BASE, LOCKUP: COLOR
// DRAG_COLOR: COLOR (defaults to the LOCKUP color)
// return value: COLOR
// Shows LOCKUP if the lockup state is true, otherwise BASE.
// Also handles "Drag" effect.
template<class BASE, class LOCKUP, class DRAG_COLOR = LOCKUP>
class Lockup {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    lockup_.run(blade);
    if (!is_same_type<DRAG_COLOR, LOCKUP>::value)
      drag_.run(blade);
    int num_leds = blade->num_leds();
    if (num_leds > 6) {
      drag_cutoff_ = num_leds * 98 / 100;
    } else {
      drag_cutoff_ = 0;
    }
  }
  OverDriveColor getColor(int led) {
    switch (SaberBase::Lockup()) {
      // Good luck desciphering this one..
      case SaberBase::LOCKUP_DRAG:
      if (led >= drag_cutoff_) {
	if (is_same_type<DRAG_COLOR, LOCKUP>::value) {
	  case SaberBase::LOCKUP_NORMAL:
	  case SaberBase::LOCKUP_ARMED:  // TODO: should this be different?
	  return lockup_.getColor(led);
	} else {
	  return drag_.getColor(led);
	}
      } else {
	case SaberBase::LOCKUP_NONE:
	break;
      }
    }
    return base_.getColor(led);
  }
private:
  int drag_cutoff_;
  BASE base_;
  LOCKUP lockup_;
  DRAG_COLOR drag_;
};

#endif
