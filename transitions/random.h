#ifndef TRANSITIONS_RANDOM_H
#define TRANSITIONS_RANDOM_H

// Usage: TrRandom<TR1, TR2, ...>
// TR1, TR2: TRANSITION
// return value: TRANSITION
// Each time a new transition is started, a random
// transition is picked from the specified list of
// transitions.

class TransitionInterface {
public:
  virtual bool done() = 0;
  virtual void begin() = 0;
  virtual void run(BladeBase* blade) = 0;
  virtual RGBA getColor(const RGBA& a, const RGBA& b, int led) = 0;
};

template<class TR>
class TrWrapper : public TransitionInterface {
public:
  bool done() override { return tr_.done(); }
  void begin() override { tr_.begin(); }
  void run(BladeBase* blade) override { tr_.run(blade); }
  RGBA getColor(const RGBA& a,
		const RGBA& b,
		int led) override {
    return tr_.getColor(a, b, led);
  }
public:
  TR tr_;
};

// This could be made generic for any interface.
template<class... TR> class TrHelper {};
template<class TR>
class TrHelper<TR> {
public:
  TransitionInterface* get(int n) { return &tr_; }
private:
  TrWrapper<TR> tr_;
};

template<class TR, class... REST>
class TrHelper<TR, REST...> {
public:
  TransitionInterface* get(int n) {
    if (!n) return &tr_;
    return rest_.get(n-1);
  }
private:
  TrWrapper<TR> tr_;
  TrHelper<REST...> rest_;
};


template<class... TRANSITION>
class TrRandom {
public:
  void begin() {
    selected_ = transitions_.get(random(sizeof...(TRANSITION)));
    selected_->begin();
  }
  bool done() { return selected_->done(); }
  void run(BladeBase* blade) { selected_->run(blade); }
  RGBA getColor(const RGBA& a,
		const RGBA& b,
		int led) {
    return selected_->getColor(a, b, led);
  }
private:
  TrHelper<TRANSITION...> transitions_;
  TransitionInterface* selected_;
};

#endif
