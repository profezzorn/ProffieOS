#ifndef TRANSITIONS_BASE_H
#define TRANSITIONS_BASE_H

#include <type_traits>

#define AUTO_RETURN(X) -> typename std::remove_const<typename std::remove_reference<decltype(X)>::type>::type { return (X); }

template<class MILLIS>
class TransitionBaseX {
public:
  void run(BladeBase* blade) {
    millis_.run(blade);
    if (restart_ ) {
      start_millis_ = millis();
      len_ = millis_.getInteger(0);
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
    return scale * t / len_;
  }
  bool restart() const { return restart_; }
  uint32_t start_millis() { return start_millis_; }
private:
  bool restart_ = false;
  MILLIS millis_;
  uint32_t start_millis_;
  int len_ = 0;
};

#endif
