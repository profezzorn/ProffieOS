#ifndef TRANSITIONS_WIPE_H
#define TRANSITIONS_WIPE_H

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

template<class MILLIS>
class TrWipeInX : public TransitionBaseX<MILLIS> {
public:
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
    int mix = (Range(0, fade_) & Range(led << 8, (led << 8) + 256)).size();
    OverDriveColor ret = a;
    ret.c = a.c.mix(b.c, mix);
    return ret;
  }
private:
  Range fade_;
};

template<int MILLIS> using TrWipeIn = TrWipeInX<Int<MILLIS>>;

#endif
