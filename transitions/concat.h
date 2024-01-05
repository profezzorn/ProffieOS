#ifndef TRANSITIONS_CONCAT_H
#define TRANSITIONS_CONCAT_H

// Usage: TrConcat<TRANSITION, INTERMEDIATE, TRANSITION, ...>
// OR:  TrConcat<TRANSITION, TRANSITION, ...>
// TRANSITION: TRANSITION
// INTERMEDIATE: COLOR
// return value: TRANSITION
// Concatenates any number of transitions.
// If an intermediate color is provided, we first transition to that color, then
// we transition away from it in the next transition.
// If no intermediate color is provided, the first and second transition will both
// transition from the same input colors. If for instance both the first and second
// transitions are TrFades, then there will be a jump in the middle as the transition
// will go back and start from the beginning. Using TimeReverseX on the second transition
// will avoid this, as the second transition will then run backwards.

#ifdef OPTIMIZE_TRCONCAT

// this TRCONCAT can save some RAM, but it's not compatible with
// IntArg/RgbArg

template<class A, class INTERMEDIATE, class B>
class TrConcat3<A, INTERMEDIATE, B...> {
public:
  TrConcat3() {
    new (&b_) B;
  }
  ~TrConcat() {
    if (run_a_) {
      a_.~A();
    } else {
      b_.~B();
    }
  }
  void begin() {
    if (!run_a_) {
      b_.~B();
      new (&a_) A;
      run_a_ = true;
    }
    a_.begin();
  }
  bool done() { return !run_a_ && b_.done(); }
  void run(BladeBase* blade) {
    intermediate_.run(blade);
    if (run_a_) {
      a_.run(blade);
      if (!a_.done()) return;
      a_.~A();
      new (&b_) B;
      run_a_ = false;
      b_.begin();
    }
    b_.run(blade);
  }
  
private:
  bool run_a_ = false;
  union {
    A a_;
    B b_;
  };
  PONUA INTERMEDIATE intermediate_;
public:  
  template<class X, class Y>
    auto getColor(const X& a, const Y& b, int led) -> decltype(MixColors(a_.getColor(a, intermediate_.getColor(led), led),
								 	 b_.getColor(intermediate_.getColor(led), b, led), 1,1)) {
    if (done()) return b;
    auto intermediate = intermediate_.getColor(led);
    if (run_a_) {
      return a_.getColor(a, intermediate, led);
    } else {
      return b_.getColor(intermediate, b, led);
    }
  }
};

#else // OPTIMIZE_TRCONCAT


template<class A, class INTERMEDIATE, class B>
class TrConcat3 {
public:
  void begin() { a_.begin(); run_a_ = true; }
  bool done() { return !run_a_ && b_.done(); }
  void run(BladeBase* blade) {
    intermediate_.run(blade);
    if (run_a_) {
      a_.run(blade);
      if (!a_.done()) return;
      run_a_ = false;
      b_.begin();
    }
    b_.run(blade);
  }
  
private:
  bool run_a_ = false;
  PONUA A a_;
  PONUA B b_;
  PONUA INTERMEDIATE intermediate_;
public:  
  template<class X, class Y>
  auto getColor(const X& a, const Y& b, int led) -> decltype(
    MixColors(b, MixColors(a_.getColor(a, intermediate_.getColor(led), led),
			   b_.getColor(intermediate_.getColor(led), b, led), 1,1), 1, 1)) {
    if (done()) return b;
    auto intermediate = intermediate_.getColor(led);
    if (run_a_) {
      return a_.getColor(a, intermediate, led);
    } else {
      return b_.getColor(intermediate, b, led);
    }
  }
};

#endif

template<class A, class B>
class TrConcat2 {
public:
  void begin() { a_.begin(); run_a_ = true; }
  bool done() { return !run_a_ && b_.done(); }
  void run(BladeBase* blade) {
    if (run_a_) {
      a_.run(blade);
      if (!a_.done()) return;
      run_a_ = false;
      b_.begin();
    }
    b_.run(blade);
  }
  
private:
  bool run_a_ = false;
  PONUA A a_;
  PONUA B b_;
public:  
  template<class X, class Y>
  auto getColor(const X& a, const Y& b, int led) -> decltype(MixColors(a_.getColor(a, b, 1), b_.getColor(a, b, 1), 1, 1)) {
    if (run_a_) {
      return a_.getColor(a, b, led);
    } else {
      return b_.getColor(a, b, led);
    }
  }
};

template <typename R, bool result = std::is_same<decltype(((R*)nullptr)->getColor(0).c.r), uint16_t>::value>
constexpr bool hasGetColorHelper(int) { 
    return result;
}
template <typename R> constexpr bool hasGetColorHelper(...) { return false; }
template <typename R> constexpr bool hasGetColor() { return hasGetColorHelper<R>(0); }

static_assert(std::is_same<decltype(((Black*)nullptr)->getColor(0).c.r), uint16_t>::value, "getcolor has changed...");
static_assert(hasGetColor<Black>(), "hasGetColor is not working");
static_assert(!hasGetColor<Int<1>>(), "hasGetColor is not working");

template<class ... REST> struct TrConcatSelector {};
template<class A, class B, typename Enable, class ... REST> struct TrConcat3Selector {
private:
  typedef void type;
};
template<class ... REST> using TrConcat = typename TrConcatSelector<REST...>::type;

template<class A, class B, class ... REST>
struct TrConcat3Selector<A, B, typename std::enable_if<hasGetColor<B>()>::type, REST...> {
  typedef TrConcat3<A, B, TrConcat<REST...>> type;
};

template<class A, class B, class ... REST>
struct TrConcat3Selector<A, B, typename std::enable_if<!hasGetColor<B>()>::type, REST...> {
  typedef TrConcat2<A, TrConcat<B, REST...>> type;
};


template<class A, class B, class ... REST>
struct TrConcatSelector<A, B, REST...> {
  typedef typename TrConcat3Selector<A, B, void, REST...>::type type;
};
template<class A, class B> 
struct TrConcatSelector<A, B> {
  typedef TrConcat2<A, B> type;
};
template<class A> 
struct TrConcatSelector<A> {
  typedef A type;
};


#endif
