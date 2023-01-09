#ifndef TRANSITIONS_DOEFFECT_H
#define TRANSITIONS_DOEFFECT_H

#include "base.h"
#include "../sound/hybrid_font.h"

// Usage: TrDoEffectX<TRANSITION, EFFECT, WAVNUM, LOCATION>
// or: TrDoEffect<TRANSITION, EFFECT, WAVNUM, LOCATION>
// TRANSITION: TRANSITION
// EFFECT: effect type
// WAVNUM, LOCATION: a number
// return value: TRANSITION
// Runs the specified TRANSITION and triggers EFFECT
// Can specify WAV file to use for EFFECT with WAVNUM
// NOTE: 0 is first wav file, -1 is random wav
// LOCATION = -1 is random 

template<class TRANSITION, BladeEffectType EFFECT, class WAVNUM = Int<-1>, int LOCATION = -1>
class TrDoEffectX : public TRANSITION {
public:
  void begin() {
    TRANSITION::begin();
    begin_ = true;
  }
  void run(BladeBase* blade) {
    TRANSITION::run(blade);
    wavnum_.run(blade);
    float location = LOCATION;
    if (location == -1) location = random(32768) / 32768.0;
    if (begin_) {
      int wavnum = wavnum_.calculate(blade);
      SaberBase::DoEffect(EFFECT, location, wavnum);
      begin_ = false;
    }
  }

 private:
  PONUA SVFWrapper<WAVNUM> wavnum_;
  bool begin_;
};

template<class TRANSITION, BladeEffectType EFFECT, int WAVNUM = -1, int LOCATION = -1> using TrDoEffect = TrDoEffectX<TRANSITION, EFFECT, Int<WAVNUM>, LOCATION>;

#endif
