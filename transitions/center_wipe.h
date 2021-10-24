#ifndef TRANSITIONS_CENTER_WIPE_H
#define TRANSITIONS_CENTER_WIPE_H

#include "wave.h"
#include "../functions/mult.h"
#include "../functions/sum.h"

// Usage: TrCenterWipeX<POSITION_FUNCTION, MILLIS_FUNCTION>
// or: TrCenterWipe<POSITION, MILLIS>
// POSITION_FUNCTION & MILLIS_FUNCTION: FUNCTION
// POSITION: Int
// MILLIS: a number
// return value: TRANSITION
// In the beginning entire blade is color A, then color B 
// starts at the POSTION and extends up and down the blade
// in the specified number of milliseconds.
template<class MILLIS, class POSITION = Int<16384>>
class TrCenterWipeX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    pos_.run(blade);
    TransitionBaseX<MILLIS>::run(blade);
    uint32_t center = (pos_.calculate(blade) * blade->num_leds()) >> 7;
    uint32_t fade_top = this->update(256 * blade->num_leds() - center);
    uint32_t fade_bottom = this->update(center);
    uint32_t top = clampi32(center + fade_top, center, 256 * blade->num_leds());
    uint32_t bottom = clampi32(center - fade_bottom, 0, center);
    range_ = Range(bottom, top);
  }

  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) -> decltype(MixColors(a,b,1,1)) {
        int mix = (range_ & Range(led << 8, (led << 8) + 256)).size();
    return MixColors(a, b, mix, 8);
  }
private:
  PONUA SVFWrapper<POSITION> pos_;
  Range range_;
};

template<int MILLIS, int POSITION = 16384> using TrCenterWipe = TrCenterWipeX<Int<MILLIS>, Int<POSITION>>;
template<class COLOR, class MILLIS, class POSITION = Int<16384>> using TrCenterWipeSparkX = TrJoin<TrCenterWipeX<MILLIS, POSITION>,TrWaveX<COLOR, Sum<MILLIS, MILLIS, MILLIS, MILLIS>, Int<200>, Sum<MILLIS, MILLIS>, POSITION>>;
template<class COLOR, int MILLIS, int POSITION = 16384> using TrCenterWipeSpark = TrJoin<TrCenterWipeX<Int<MILLIS>, Int<POSITION>>,TrWaveX<COLOR, Sum<Int<MILLIS>, Int<MILLIS>, Int<MILLIS>, Int<MILLIS>>, Int<200>, Sum<Int<MILLIS>, Int<MILLIS>>, Int<POSITION>>>;

// Usage: TrCenterWipeInX<POSITION_FUNCTION, MILLIS_FUNCTION>
// or: TrCenterWipeIn<POSITION, MILLIS>
// POSITION_FUNCTION & MILLIS_FUNCTION: FUNCTION
// POSITION: Int
// MILLIS: a number
// return value: TRANSITION
// In the beginning entire blade is color A, then color B 
// starts at the ends and moves toward POSITION
// in the specified number of milliseconds.
template<class MILLIS, class POSITION = Int<16384>>
class TrCenterWipeInX : public TransitionBaseX<MILLIS> {
public:
  void run(BladeBase* blade) {
    pos_.run(blade);
    TransitionBaseX<MILLIS>::run(blade);
    uint32_t center = (pos_.calculate(blade) * blade->num_leds()) >> 7;
    uint32_t fade_top = this->update(256 * blade->num_leds() - center);
    uint32_t fade_bottom = this->update(center);
    uint32_t top = clampi32((256 * blade->num_leds()) - fade_top, center, 256 * blade->num_leds());
    uint32_t bottom = clampi32(0 + fade_bottom, 0, center);
    range_ = Range(bottom, top);
  }

  template<class A, class B>
  auto getColor(const A& a, const B& b, int led) -> decltype(MixColors(a,b,1,1)) {
        int mix = (range_ & Range(led << 8, (led << 8) + 256)).size();
    return MixColors(b, a, mix, 8);
  }
private:
  PONUA SVFWrapper<POSITION> pos_;
  Range range_;
};

template<int MILLIS, int POSITION = 16384> using TrCenterWipeIn = TrCenterWipeInX<Int<MILLIS>, Int<POSITION>>;
template<class COLOR, class MILLIS, class POSITION = Int<16384>> using TrCenterWipeInSparkX = TrJoin<TrCenterWipeInX<MILLIS, POSITION>,TrJoin<TrWaveX<COLOR, MILLIS, Int<200>, Sum<MILLIS, MILLIS>, Int<0>>,TrWaveX<COLOR, MILLIS, Int<200>, Sum<MILLIS, MILLIS>, Int<32768>>>>;
template<class COLOR, int MILLIS, int POSITION = 16384> using TrCenterWipeInSpark = TrJoin<TrCenterWipeInX<Int<MILLIS>, Int<POSITION>>,TrJoin<TrWaveX<COLOR, Int<MILLIS>, Int<200>, Sum<Int<MILLIS>, Int<MILLIS>>, Int<0>>,TrWaveX<COLOR, Int<MILLIS>, Int<200>, Sum<Int<MILLIS>, Int<MILLIS>>, Int<32768>>>>;

#endif
