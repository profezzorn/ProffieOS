#ifndef TRANSITIONS_BASE_H
#define TRANSITIONS_BASE_H

#include <type_traits>

#define AUTO_RETURN(X) -> typename std::remove_const<typename std::remove_reference<decltype(X)>::type>::type { return (X); }

// When specifying the time for a transition, you can use these functions to make the transition speed change as it goes.
// Note that none of these change the total time, they just make it fast in the beginning and slow at the end, or vice versa.
// Here are a few examples:
// TrFadeX<BendTimePowX<Int<1000>, Int<65536>>>  : x^2 function, will start slow (speed=0) and speed up, speed will be 2 at end
// TrFadeX<BendTimePowX<Int<1000>, Int<16384>>   : x^0.5 function, starts at infinite speed and slows down to 0.5 at end.
// TrFadeX<BendTimePowInvX<Int<1000>, Int<65536>>: 1-(1-x)^2 starts at speed 2 and ends at speed 0
// TrFadeX<BendTimePowInvX<Int<1000>, Int<16384>>: 1-(1-x)^0.5 starts at speed 0.5 and ends at infinite speed
// TrFadeX<ReverseTimeX<Int<1000>>: 1-x does the transition backwards
//
// Basically, BendTimePowX<X, Int<Y>> is equal to X^(Y/32768) and BendTimePowInvX<X, Int<Y>> is equal to 1-(1-X)^(Y/32768)

// These are just placeholders, all the logic is in AddBend
template<class MILLIS, class BEND_FUNCTION> struct BendTimePowX {};
template<class MILLIS> struct ReverseTimeX {};
template<class MILLIS, class BEND_FUNCTION> using BendTimePowInvX = ReverseTimeX<BendTimePowX<ReverseTimeX<MILLIS>, BEND_FUNCTION>>;
template<int MILLIS, int BEND_VALUE> using BendTimePow = BendTimePowX<Int<MILLIS>, Int<BEND_VALUE>>;
template<int MILLIS, int BEND_VALUE> using BendTimePowInv = BendTimePowInvX<Int<MILLIS>, Int<BEND_VALUE>>;
template<int MILLIS> using ReverseTime = ReverseTimeX<Int<MILLIS>>;

template<class MILLIS>
struct AddBend : public SVFWrapper<MILLIS> {
  template<class T>
  T bend(uint32_t t, uint32_t len, T scale) { return scale * t / len; }
};

template<class MILLIS, class POW>
struct AddBend<class BendTimePowX<MILLIS, POW>> {
  void run(BladeBase* blade) {
    millis_.run(blade);
    bend_value_.run(blade);
  }

  int calculate(BladeBase* blade) {
    exponent_ = bend_value_.calculate(blade) / 32768.0;
    return millis_.calculate(blade);
  }

  PONUA AddBend<MILLIS> millis_;
  PONUA SVFWrapper<POW> bend_value_;
  float exponent_;
  template<class T>
  T bend(uint32_t t, uint32_t len, T scale) {
    return scale * powf(millis_.bend(t, len, 1.0f), exponent_);
  }
};

template<class MILLIS, int POW>
struct AddBend<class BendTimePowX<MILLIS, Int<POW>>> : public AddBend<MILLIS> {
  template<class T>
  T bend(uint32_t t, uint32_t len, T scale) {
    return scale * powf(AddBend<MILLIS>::bend(t, len, 1.0f), POW / 32768.0);
  }
};

template<class MILLIS>
struct AddBend<class ReverseTimeX<MILLIS>> : public AddBend<MILLIS> {
  template<class T>
  T bend(uint32_t t, uint32_t len, T scale) {
    return scale - AddBend<MILLIS>::bend(t, len, scale);
  }
};

template<class MILLIS>
class TransitionBaseX {
public:
  void run(BladeBase* blade) {
    millis_.run(blade);
    if (restart_ ) {
      start_millis_ = millis();
      len_ = millis_.calculate(blade);
      restart_ = false;
    }
  }
  bool done() { return len_ == 0; }
  void begin() { restart_ = true; }

  template<typename T>
  T update(T scale) {
    if (done()) return scale;
    uint32_t t = millis() - start_millis_;
    if (t > len_) {
      len_ = 0;
      return scale;
    }
    return millis_.bend(t, len_, scale);
  }
  bool restart() const { return restart_; }
  uint32_t start_millis() { return start_millis_; }
private:
  bool restart_ = false;
  PONUA AddBend<MILLIS> millis_;
  uint32_t start_millis_;
  uint32_t len_ = 0;
};


template<class T>
class TransitionHelper {
public:
  void begin() {
    tr_.begin();
    active_ = true;
  }
  void run(BladeBase * blade) {
    if (active_) {
      tr_.run(blade);
      if (tr_.done()) active_ = false;
    }
  }

  T tr_;
  bool active_ = false;

  operator bool() const { return active_; }

  template<class A, class B>
  auto getColor(A a, B b, int led) -> decltype(tr_.getColor(a, b, led)) {
    if (active_)
      return tr_.getColor(a, b, led);
    else
      return b;
  }
};



#endif
