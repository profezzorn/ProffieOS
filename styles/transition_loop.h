#ifndef STYLES_TRANSITION_LOOP_H
#define STYLES_TRANSITION_LOOP_H

// Usage: TransitionLoop<COLOR, TRANSITION>
// COLOR: COLOR
// TRANSITION : TRANSITION
// return value: COLOR
//
// Continuously transitions COLOR to COLOR
// Makes more sense if TRANSITION is a TrConcat, as this will
// transition to/from the intermediate steps in a loop.

template<class T, class TRANSITION>
class TransitionLoop {
public:
  void run(BladeBase* blade) {
    color_.run(blade);
    transition_.run(blade);
    if (transition_.done()) transition_.begin();
  }
  OverDriveColor getColor(int led) {
    OverDriveColor ret = color_.getColor(led);
    return transition_.getColor(ret, ret, led);
  }
private:
  T color_;
  TRANSITION transition_;
};

#endif
