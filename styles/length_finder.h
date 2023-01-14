#ifndef STYLES_LENGTH_FINDER_H
#define STYLES_LENGTH_FINDER_H

// Usage: LengthFinder<BASE, LIGHTUP>
// or: LengthFinder<>
// BASE, LIGHTUP: COLOR
// Return value: COLOR
// Lights up exactly one led, based on the current color change
// variable. When changed, says what the current color change is
// so that you know which led is lit up.
// Use this when you don't know how many LEDs are in your blade.
// Set the blade length to 144, enter color change mode and
// change it until no LED turns on, go back one and there you go!
template<class BASE = BLACK, class LIGHTUP = WHITE>
class LengthFinder {
public:
  LengthFinder() {
    BladeBase::HandleFeature(HANDLED_FEATURE_CHANGE);
    BladeBase::HandleFeature(HANDLED_FEATURE_CHANGE_TICKED);
  }
  void run(BladeBase* blade) {
    base_.run(blade);
    lightup_.run(blade);
    if (SaberBase::GetCurrentVariation() < 0) {
      SaberBase::SetVariation(0);
    }
    if (SaberBase::GetCurrentVariation() >= blade->num_leds()) {
      SaberBase::SetVariation(blade->num_leds() - 1);
    }
    if (SaberBase::GetCurrentVariation() != led_) {
      led_ = SaberBase::GetCurrentVariation();
      say_it_ = true;
    }
    if (say_it_ && millis() - last_speak_ > 3000) {
      last_speak_ = millis();
      say_it_ = false;
#ifdef ENABLE_AUDIO
      talkie.SayNumber(led_ + 1);
#endif
      STDOUT << "LEN=" << (led_ + 1) << "\n";
    }
  }

  OverDriveColor getColor(int led) {
    if (led == led_) return lightup_.getColor(led);
    return base_.getColor(led);
  }
private:
  bool say_it_ = true;
  int led_= -1;
  uint32_t last_speak_ = 0;
  BASE base_;
  LIGHTUP lightup_;
};

#endif
