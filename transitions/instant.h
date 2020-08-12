#ifndef TRANSITIONS_INSTANT_H
#define TRANSITIONS_INSTANT_H

// Usage: TrInstant
// return value: TRANSITION
// Instant transition.
class TrInstant {
public:
  bool done() { return true; }
  void begin() {}
  void run(BladeBase* blade) {}
  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) AUTO_RETURN(b)
};

#endif
