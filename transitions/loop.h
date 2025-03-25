#ifndef TRANSITIONS_LOOP_H
#define TRANSITIONS_LOOP_H

// Usage: TrLoop<TRANSITION>
// TRANSITION: TRANSITION
// Return Value: TRANSITION
// Runs the specified transition in a loop forever.

template<class TR>
class TrLoop : public TR {
public:
  void run(BladeBase* blade) {
    if (TR::done()) TR::begin();
    TR::run(blade);
  }
  bool done() { return false; }
};

// Usage: TrLoopNX<N_FUNCTION, TRANSITION>
// or: TrLoopN<N, TRANSITION>
// N_FUNCTION: FUNCTION (number of Loops)
// N: a number (Loops)
// TRANSITION: TRANSITION
// Return Value: TRANSITION
// Runs the specified transition N times.

template<class N, class TRANSITION>
class TrLoopNX : public TRANSITION {
public:
  void run(BladeBase* blade) {
    n_.run(blade);
    if (loops_ < 0) loops_ = n_.calculate(blade) + 1;
    if (loops_ > 0 && TRANSITION::done()) {
      if (loops_ > 1) TRANSITION::begin();
      loops_--;
    }
    TRANSITION::run(blade);
  }
  void begin() {
    TRANSITION::begin();
    loops_ = -1;
  }
  bool done() { return loops_ == 0; }

private:
  int loops_ = 0;
  PONUA SVFWrapper<N> n_;
};

template<int N, class TRANSITION> using TrLoopN = TrLoopNX<Int<N>, TRANSITION>;

// Usage: TrLoopUntil<PULSE, TRANSITION, OUT>
// TRANSITION, OUT: TRANSITION
// PULSE: FUNCTION (pulse)
// Return Value: TRANSITION
// Runs the specified transition until the pulse occurs.
// When the pulse occurs, the loop continues, but OUT is used to
// transition away from it, and when OUT is done, the transition is done.

template<class PULSE, class TR, class OUT>
class TrLoopUntil {
public:
  void begin() {
    transition_.begin();
    pulsed_ = false;
  }
  bool done() {
    return pulsed_ && out_.done();
  }
  void run(BladeBase* blade) {
    pulse_.run(blade);
    if (transition_.done()) {
      transition_.begin();
    }
    transition_.run(blade);
    if (!pulsed_) {
      if (pulse_.calculate(blade)) {
	out_.begin();
	pulsed_ = true;
      }
    }
    if (pulsed_) {
      out_.run(blade);
    }
  }

private:
  bool pulsed_ = false;
  PONUA SVFWrapper<PULSE> pulse_;
  PONUA TR transition_;
  PONUA OUT out_;
public:
  template<class X, class Y>
  auto getColor(const X& a, const Y& b, int led) ->
    decltype(MixColors(transition_.getColor(a, a, 1),
		       out_.getColor(transition_.getColor(a, a, 1), b, 1), 1,1))
  {
    if (pulsed_) {
      return out_.getColor(transition_.getColor(a, a, led), b, led);
    } else {
      return transition_.getColor(a, a, led);
    }
  }
};

// Usage: TransitionLoopWhileL<LOOP_TRANSITION, END_TRANSITION, CONDITION>
// LOOP_TRANSITION : TRANSITION
// END_TRANSITION : TRANSITION
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
  if (!run_ || loop_tr_.done()) {
    run_ = false;
    if (condition_.calculate(blade)) {
     run_ = true;
     end_ = false;
     loop_tr_.begin();
    }
  }
  if (run_) {
    if (!condition_.calculate(blade)) {
      run_ = false;
      end_ = true;
      end_tr_.begin();
    } else {
      loop_tr_.run(blade);
    }
  }
  if (end_) {
    if (end_tr_.done()) {
      end_ = false;
    }
    end_tr_.run(blade);
    loop_tr_.run(blade);
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
    if (run_) {
      return loop_tr_.getColor(RGBA_um_nod::Transparent(),
          RGBA_um_nod::Transparent(), led);
    } else if (end_) {
      return end_tr_.getColor(RGBA_um_nod::Transparent(),
          RGBA_um_nod::Transparent(), led);
    } else {
      return RGBA_um_nod::Transparent();
    }
  }
};

#endif
