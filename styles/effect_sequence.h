#ifndef STYLES_EFFECT_SEQUENCE_H
#define STYLES_EFFECT_SEQUENCE_H

//Usage EffectSequence<EFFECT, COLORS...>
//Create a sequence of Colors based on number of times the effect is used.

template<BladeEffectType EFFECT, class... COLORS> 
class EffectSequence {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    if (effect_.Detect(blade)) {
	n_ = (n_ + 1) % sizeof...(COLORS);
    }
  }
private:
  OneshotEffectDetector<EFFECT> effect_;
  int n_ = -1;
  MixHelper<COLORS...> colors_;

public:  
  auto getColor(int led) -> decltype(colors_.getColor(n_, led)) {
    return colors_.getColor(n_, led);
  }

};

#endif
