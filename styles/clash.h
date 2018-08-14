#ifndef STYLES_SIMPLE_CLASH_H
#define STYLES_SIMPLE_CLASH_H

// Usage: SimpleClash<BASE, CLASH_COLOR, CLASH_MILLIS>
// BASE: COLOR
// CLASH_COLOR: COLOR (defaults to white)
// CLASH_MILLIS: a number (defaults to 40)
// return value: COLOR
// Turns the blade to CLASH_COLOR for CLASH_MILLIS millseconds
// when a clash occurs.

template<class T, class CLASH_COLOR = Rgb<255,255,255>, int CLASH_MILLIS = 40>
class SimpleClash {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    clash_color_.run(blade);
    uint32_t m = millis();
    if (blade->clash()) clash_millis_ = m;
    clash_ = m - clash_millis_ < CLASH_MILLIS;
  }
  OverDriveColor getColor(int led) {
    if (clash_) {
      return clash_color_.getColor(led);
    } else {
      return base_.getColor(led);
    }
  }
private:
  bool clash_;
  T base_;
  CLASH_COLOR clash_color_;
  uint32_t clash_millis_;
};


// Usage: LocalizedClash<BASE, CLASH_COLOR, CLASH_MILLIS, CLASH_WIDTH_PERCENT=50>
// BASE: COLOR
// CLASH_COLOR: COLOR (defaults to white)
// CLASH_MILLIS: a number (defaults to 40)
// return value: COLOR
// Similar to SimpleClash, but lights up a portion of the blade.
// The fraction of the blade is defined by CLASH_WIDTH_PERCENT
// The location of the clash is random within the middle half of the blade.

// Based on exp(-x*x)
// TODO: Put in shared location as it is identical to the one in blast.h
static uint8_t clash_hump[32] = {
  255,255,252,247,240,232,222,211,
  199,186,173,159,145,132,119,106,
  94,82,72,62,53,45,38,32,
  26,22,18,14,11,9,7,5
};

template<class T,
         class CLASH_COLOR = Rgb<255,255,255>,
         int CLASH_MILLIS = 40,
         int CLASH_WIDTH_PERCENT = 50>
class LocalizedClashClash {
public:
  void run(BladeBase* blade) {
    base_.run(blade);
    clash_color_.run(blade);
    uint32_t m = millis();
    if (blade->clash()) {
      clash_millis_ = m;
      mult_ = NELEM(clash_hump) * 2 * 102400 / CLASH_WIDTH_PERCENT / blade->num_leds();
      clash_location_ = random(blade->num_leds() * mult_ / 2) +
	blade->num_leds() * mult_ / 4;
    }
    clash_ = m - clash_millis_ < CLASH_MILLIS;
  }
  OverDriveColor getColor(int led) {
    if (clash_) {
      OverDriveColor ret = base_.getColor(led);
      int dist = abs(led * mult_ - clash_location_) / 1024;
      if (dist < NELEM(clash_hump)) {
	OverDriveColor clash =  clash_color_.getColor(led);
	ret.c = ret.c.mix(clash.c, clash_hump[dist]);
      }
      return ret;
    } else {
      return base_.getColor(led);
    }
  }
private:
  bool clash_;
  T base_;
  int mult_;
  int clash_location_;
  CLASH_COLOR clash_color_;
  uint32_t clash_millis_;
};

#endif
