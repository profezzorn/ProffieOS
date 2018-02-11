#ifndef STYLES_IFON_H
#define STYLES_IFON_H

// Usage: Ifon<A, B>
// Returns A if saber is on, B otherwise.
// A, B: INTEGER
// return value: INTEGER

// TODO: optimize when the A & B is the same for all LEDs.

class BladeBase;

template<class IFON, class IFOFF>
class Ifon {
public:
  void run(BladeBase* blade) {
    ifon_.run(blade);
    ifoff_.run(blade);
    on_ = blade->is_on();
  }
  int getInteger(int led) {
    return on_ ? ifon_.getInteger(led) : ifoff_.getInteger(led);
  }

private:
  IFON ifon_;
  IFOFF ifoff_;
  bool on_;
};

#endif
