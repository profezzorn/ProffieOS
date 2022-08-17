#ifndef TRANSITIONS_BASE_H
#define TRANSITIONS_BASE_H

#include <type_traits>

#define AUTO_RETURN(X) -> typename std::remove_const<typename std::remove_reference<decltype(X)>::type>::type { return (X); }

// These are just placeholders, all the logic is in AddBend
template<class MILLIS, class BEND_FUNCTION> struct BendTimePowX {};
template<class MILLIS, class BEND_FUNCTION> struct BendTimePowInvX {};
template<int MILLIS, int BEND_VALUE> using BendTimePow = BendTimePowX<Int<MILLIS>, Int<BEND_VALUE>>;
template<int MILLIS, int BEND_VALUE> using BendTimePowInv = BendTimePowInvX<Int<MILLIS>, Int<BEND_VALUE>>;

template<class MILLIS>
struct AddBend {
  void run(BladeBase* blade) {
    millis_.run(blade);
  }
  int calculate(BladeBase* blade) {
    return millis_.calculate(blade);
  }
  PONUA SVFWrapper<MILLIS> millis_;
  template<class T>
  uint32_t bend(uint32_t t, uint32_t len, T scale) { return scale * t / len; }
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

  PONUA SVFWrapper<MILLIS> millis_;
  PONUA SVFWrapper<POW> bend_value_;
  float exponent_;
  template<class T>
  T bend(uint32_t t, uint32_t len, T scale) {
    if (t > len) return t;
    float frac = t / (float)len;
    frac = powf(frac, exponent_);
    return scale * frac;
  }
};

template<class MILLIS, class POW>
struct AddBend<class BendTimePowInvX<MILLIS, POW>> {
  void run(BladeBase* blade) {
    millis_.run(blade);
    bend_value_.run(blade);
  }

  int calculate(BladeBase* blade) {
    exponent_ = bend_value_.calculate(blade) / 32768.0;
    return millis_.calculate(blade);
  }

  PONUA SVFWrapper<MILLIS> millis_;
  PONUA SVFWrapper<POW> bend_value_;
  float exponent_;
  template<class T>
  T bend(uint32_t t, uint32_t len, T scale) {
    if (t > len) return t;
    float frac = 1.0 - t / (float)len;
    frac = 1.0 - powf(frac, exponent_);
    return scale * frac;
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
