#ifndef TRANSITIONS_RANDOM_H
#define TRANSITIONS_RANDOM_H

// Usage: TrRandom<TR1, TR2, ...>
// TR1, TR2: TRANSITION
// return value: TRANSITION
// Each time a new transition is started, a random
// transition is picked from the specified list of
// transitions.

template<class T> class TrHelper2 {};

template<class TR>
class TrHelper2<TypeList<TR>> : public TR {
private:
  TR tr_;
public:
  size_t size() { return 1; }
  bool done(int N) { return tr_.done(); }
  void begin(int N) { tr_.begin(); }
  void run(int N, BladeBase* blade) { tr_.run(blade); }
  template<class A, class B>
  auto getColor(int N, A a, B b, int led) -> decltype(tr_.getColor(a, b, led)) {
    return TR::getColor(a, b, led);
  }
};

template<class T1, class T2, class... REST>
class TrHelper2<TypeList<T1, T2, REST...>> {
private:
  PONUA TrHelper2<typename SplitTypeList<TypeList<T1, T2, REST...>>::first_half> a_;
  PONUA TrHelper2<typename SplitTypeList<TypeList<T1, T2, REST...>>::second_half> b_;
public:
  size_t size() { return sizeof...(REST) + 2; }

  bool done(int N) {
    if (N < a_.size()) return a_.done(N);
    return b_.done(N - a_.size());
  }
  void begin(int N) {
    if (N < a_.size()) return a_.begin(N);
    return b_.begin(N - a_.size());
  }
  void run(int N, BladeBase* blade) {
    if (N < a_.size()) return a_.run(N, blade);
    return b_.run(N - a_.size(), blade);
  }
  template<class A, class B>
  auto getColor(int N, A a, B b, int led) -> decltype(MixColors(a_.getColor(0, a, b, led), b_.getColor(0, a, b, led), 1, 1)) {
    if (N < a_.size()) return a_.getColor(N, a, b, led);
    return b_.getColor(N - a_.size(), a, b, led);
  }
};

template<class... TR>
class TrHelper3 {
protected:
  TrHelper2<TypeList<TR...>> tr_;
  uint16_t selected_;
public:
  bool done() { return tr_.done(selected_); }
  void begin() { tr_.begin(selected_); }
  void run(BladeBase* blade) { tr_.run(selected_, blade); }

  template<class A, class B>
  auto getColor(A a, B b, int led) -> decltype(tr_.getColor(0, a, b, led)) {
    return tr_.getColor(selected_, a, b, led);
  }
};


template<class... TRANSITION>
class TrRandom : public TrHelper3<TRANSITION...> {
public:
  void begin() {
    this->selected_ = random(sizeof...(TRANSITION));
    TrHelper3<TRANSITION...>::begin();
  }
};

#endif
