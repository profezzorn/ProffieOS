#ifndef STYLES_LOCKUP_H
#define STYLES_LOCKUP_H

// Usage: Lockup<BASE, LOCKUP>
// BASE, LOCKUP: COLOR
// return value: COLOR
// Shows LOCKUP if the lockup state is true, otherwise BASE.
// Also handles "Drag" effect.
template<class BASE, class LOCKUP>
class Lockup {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    lockup_.run(blade);
    int num_leds = blade->num_leds();
    if (num_leds > 6) {
      drag_cutoff_ = num_leds * 98 / 100;
    } else {
      drag_cutoff_ = 0;
    }
  }
  OverDriveColor getColor(int led) {
    // Good luck desciphering this one...
    switch (SaberBase::Lockup()) {
      case SaberBase::LOCKUP_DRAG:
        if (led >= drag_cutoff_) {
          case SaberBase::LOCKUP_NORMAL:
            return lockup_.getColor(led);
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
};

#endif
