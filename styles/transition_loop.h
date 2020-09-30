#ifndef STYLES_TRANSITION_LOOP_H
#define STYLES_TRANSITION_LOOP_H

// Usage: TransitionLoop<COLOR, TRANSITION>
// Or: TransitionLoopL<TRANSITION>
// COLOR: COLOR
// TRANSITION : TRANSITION
// return value: COLOR
//
// Continuously transitions COLOR to COLOR
// Makes more sense if TRANSITION is a TrConcat, as this will
// transition to/from the intermediate steps in a loop.

template<class TRANSITION>
class TransitionLoopL {
public:
  void run(BladeBase* blade) {
    if (transition_.done()) transition_.begin();
    transition_.run(blade);
  }
private:
  TRANSITION transition_;
public:  
  auto getColor(int led) -> decltype(transition_.getColor(RGBA_um_nod::Transparent(),
							  RGBA_um_nod::Transparent(),
							  led)) {
    return transition_.getColor(RGBA_um_nod::Transparent(),
				RGBA_um_nod::Transparent(),
				led);
  }
};

template<class T, class TRANSITION>
  using TransitionLoop = Layers<T, TransitionLoopL<TRANSITION>>;

#endif
