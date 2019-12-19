#ifndef TRANSITIONS_WIPE_H
#define TRANSITIONS_WIPE_H

// Usage: TrWipeX<MILLIS_FUNCTION>
// or: TrWipe<MILLIS>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// return value: TRANSITION
// Similar to saber ignition. In the beginning
// entire blade is color A, then color B starts at the base
// and extends up to the tip of the blade in the specified
// number of milliseconds.
template<class MILLIS>
class TrWipeX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    if (this->done()) {
      fade_ = blade->num_leds() * 256;
    } else {
      fade_ = (millis() - this->start_millis_) * 256 * blade->num_leds() / this->len_;
    }
  }
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    int mix = (Range(0, fade_) & Range(led << 8, (led << 8) + 256)).size();
    OverDriveColor ret = a;
    ret.c = a.c.mix(b.c, mix);
    return ret;
  }
private:
  uint32_t fade_;
};

template<int MILLIS> using TrWipe = TrWipeX<Int<MILLIS>>;

// Usage: TrWipeInX<MILLIS_FUNCTION>
// or: TrWipeIn<MILLIS>
// MILLIS_FUNCTION: FUNCTION
// MILLIS: a number
// return value: TRANSITION
// Like TrWipe, but from tip to base.
template<class MILLIS>
class TrWipeInX : public TransitionBaseX<MILLIS> {
public:
  TrWipeInX() : TransitionBaseX<MILLIS>(), fade_(0, 0) {}

  void run(BladeBase* blade) {
    if (this->done()) {
      fade_ = Range(0, blade->num_leds() * 256);
    } else {
      fade_ = Range(256 * blade->num_leds() -
		    (millis() - this->start_millis_) * 256 * blade->num_leds() / this->len_,
		    blade->num_leds() * 256);
    }
  }
  OverDriveColor getColor(const OverDriveColor& a,
			  const OverDriveColor& b,
			  int led) {
    int mix = (fade_ & Range(led << 8, (led << 8) + 256)).size();
    OverDriveColor ret = a;
    ret.c = a.c.mix(b.c, mix);
    return ret;
  }
private:
  Range fade_;
};

template<int MILLIS> using TrWipeIn = TrWipeInX<Int<MILLIS>>;

#endif
