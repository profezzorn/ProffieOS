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
  TransitionLoopL() { transition_.begin(); }
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

// Usage: TransitionLoopWhileL<LOOP_TRANSITION, END_TRANSITION, CONDITION>
// LOOP_TRANSITION : TRANSITION
// END_TRANSITION : TRANSITION
// CONDITION: FUNCTION
// return value: COLOR
//
// Continuously runs LOOP_TRANSITION while CONDITION > 0
// Makes more sense if TRANSITION is a TrConcat, as this will
// transition to/from the intermediate steps in a loop.
// If CONDITION <= 0, runs END_TRANSITION and stops.

template<class LoopTr, class EndTr, class CONDITION>
class TransitionLoopWhileL {
public:
  void run(BladeBase* blade) {
    condition_.run(blade);
    bool cond = condition_.calculate(blade) > 0;
    if (!run_ && cond) {
      run_ = true;
      loop_tr_.begin();
    }
    if (run_ && !cond && !end_) {
      end_ = true;
      end_tr_.begin();
    }
    if (run_) {
      loop_tr_.run(blade);
      if (loop_tr_.done()) loop_tr_.begin();
      if (end_) {
	end_tr_.run(blade);
	if (end_tr_.done()) {
	  end_ = false;
	  run_ = false;
	}
      }
    }
  }

private:
  bool run_ = false;
  bool end_ = false;
  LoopTr loop_tr_;
  EndTr end_tr_;
  PONUA SVFWrapper<CONDITION> condition_;

public:
auto getColor(int led) -> decltype(
    MixColors(end_tr_.getColor(loop_tr_.getColor(RGBA_um_nod::Transparent(), RGBA_um_nod::Transparent(), led), RGBA_um_nod::Transparent(), led),
        loop_tr_.getColor(loop_tr_.getColor(RGBA_um_nod::Transparent(), RGBA_um_nod::Transparent(), led), RGBA_um_nod::Transparent(), led), 1, 1)) {
    decltype(MixColors(end_tr_.getColor(loop_tr_.getColor(RGBA_um_nod::Transparent(), RGBA_um_nod::Transparent(), led), RGBA_um_nod::Transparent(), led),
        loop_tr_.getColor(loop_tr_.getColor(RGBA_um_nod::Transparent(), RGBA_um_nod::Transparent(), led), RGBA_um_nod::Transparent(), led), 1, 1)) ret = RGBA_um_nod::Transparent();
    if (run_) {
      ret = loop_tr_.getColor(RGBA_um_nod::Transparent(), RGBA_um_nod::Transparent(), led);
      if (end_) ret = end_tr_.getColor(ret, RGBA_um_nod::Transparent(), led);
    }
    return ret;
  }
};

#endif  // STYLES_TRANSITION_LOOP_H
