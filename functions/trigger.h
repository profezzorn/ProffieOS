#ifndef FUNCTIONS_TRIGGER_H
#define FUNCTIONS_TRIGGER_H

// Usage: Trigger<EFFECT, FADE_IN_MILLIS, SUSTAIN_MILLIS, FADE_OUT_MILLIS, DELAY>
// Normally returns 0, but when EFFECT occurs, it ramps up to 32768,
// stays there for SUSTAIN_MILLIS, then fades down to zero again.
// If delay is specified, the whole thing is delayed that much before it starts.
// EFFECT: BladeEffectType
// FADE_IN_MILLIS: INTEGER
// SUSTAIN_MILLIS: INTEGER
// FADE_OUT_MILLIS: INTEGER
// DELAY_MILLIS: INTEGER (defaults to Int<0>)
// return value: INTEGER
template<
  BladeEffectType EFFECT,
  class FADE_IN_MILLIS,
  class SUSTAIN_MILLIS,
  class FADE_OUT_MILLIS,
  class DELAY_MILLIS = Int<0>>
class Trigger {
  enum TriggerState {
    TRIGGER_DELAY = 0,
    TRIGGER_ATTACK = 1,
    TRIGGER_SUSTAIN = 2,
    TRIGGER_RELEASE = 3,
    TRIGGER_OFF = 4
  };
 public:
  void run(BladeBase* blade) {
    delay_millis_.run(blade);
    fade_in_millis_.run(blade);
    sustain_millis_.run(blade);
    fade_out_millis_.run(blade);

    if (effect_.Detect(blade)) {
      start_time_ = micros();
      trigger_state_ = TRIGGER_DELAY;
    }
    if (trigger_state_ == TRIGGER_OFF) {
      value_ = 0;
      return;
    }
    uint32_t t = micros() - start_time_;

    while (true) {
      uint32_t micros_for_state = get_millis_for_state(blade) * 1000;
      if (t < micros_for_state) {
	switch (trigger_state_) {
	case TRIGGER_DELAY:
	  value_ = 0;
	  return;
	case TRIGGER_ATTACK:
	  value_ = t * 32768.0 / micros_for_state;
	  return;
	case TRIGGER_SUSTAIN:
	  value_ = 32768;
	  return;
	case TRIGGER_RELEASE:
	  value_ = 32768 - t * 32768.0 / micros_for_state;
	  return;
	case TRIGGER_OFF:
	  value_ = 0;
	  return;
	}
      }
      trigger_state_++;
      t -= micros_for_state;
      start_time_ += micros_for_state;
    }
  }
  uint32_t get_millis_for_state(BladeBase* base) {
    switch (trigger_state_) {
    case TRIGGER_DELAY: return delay_millis_.calculate(base);
    case TRIGGER_ATTACK: return fade_in_millis_.calculate(base);
    case TRIGGER_SUSTAIN: return sustain_millis_.calculate(base);
    case TRIGGER_RELEASE: return fade_out_millis_.calculate(base);
    case TRIGGER_OFF:
      break;
    }
    return 1000000;
  }
  int getInteger(int led) const { return value_; }
 private:
  PONUA SVFWrapper<DELAY_MILLIS> delay_millis_;
  PONUA SVFWrapper<FADE_IN_MILLIS> fade_in_millis_;
  PONUA SVFWrapper<SUSTAIN_MILLIS> sustain_millis_;
  PONUA SVFWrapper<FADE_OUT_MILLIS> fade_out_millis_;
  int value_;
  uint8_t trigger_state_ = TRIGGER_OFF;
  uint32_t start_time_;
  OneshotEffectDetector<EFFECT> effect_;
};

#endif
