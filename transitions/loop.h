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

// Usage: TrLoopN<N, TRANSITION>
// N: FUNCTION (number of Loops)
// TRANSITION: TRANSITION
// Return Value: TRANSITION
// Runs the specified transition N times.

template<class N, class TRANSITION>
class TrLoopN : public TRANSITION {
public:
  void run(BladeBase* blade) {
    n_.run(blade);
    num_loops_ = n_.calculate(blade);
    if (loops_ < num_loops_ && TRANSITION::done()) {
      TRANSITION::begin();
      loops_ += 1;
    }
    TRANSITION::run(blade);
  }
  void begin() {
    TRANSITION::begin();
    loops_ = 1;
  }
  bool done() {
    return loops_ == num_loops_ && TRANSITION::done(); 
  }

private:
  int loops_;
  int num_loops_;
  PONUA SVFWrapper<N> n_;
};

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
  PONUA TR out_;
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

#endif
