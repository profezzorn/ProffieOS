#ifndef BLADES_BLADE_BASE_H
#define BLADES_BLADE_BASE_H

#include "../styles/blade_style.h"

// Bitfield
enum BladeEffectType {
  EFFECT_NONE =  0x0,
  EFFECT_CLASH = 1 << 0,
  EFFECT_BLAST = 1 << 1,
  EFFECT_FORCE = 1 << 2,
  EFFECT_STAB =  1 << 3,
  EFFECT_BOOT =  1 << 4,
  EFFECT_LOCKUP_BEGIN = 1 << 5,
  EFFECT_LOCKUP_END = 1 << 6,
  EFFECT_DRAG_BEGIN = 1 << 7,
  EFFECT_DRAG_END = 1 << 8,
  EFFECT_IGNITION = 1 << 9,
  EFFECT_RETRACTION = 1 << 10,
  EFFECT_CHANGE = 1 << 11, // used for click to change
};

struct BladeEffect {
  BladeEffectType type;
  uint32_t start_micros;
  float location; // 0 = base, 1 = tip
};

class BladeBase {
public:
  // Returns number of LEDs in this blade.
  virtual int num_leds() const = 0;

  // Returns the byte order of this blade.
  virtual Color8::Byteorder get_byteorder() const = 0;

  // Returns true if the blade is supposed to be on.
  // false while "turning off".
  virtual bool is_on() const = 0;

  // Return how many effects are in effect.
  virtual size_t GetEffects(BladeEffect** blade_effects) = 0;

  // Let the blade know that this style handles "effect".
  virtual void HandleEffectType(BladeEffectType effect) = 0;
  virtual bool IsHandled(BladeEffectType effect) = 0;

  // Set led 'led' to color 'c'.
  virtual void set(int led, Color16 c) = 0;

  // Bypasses battery voltage based PWM, intended to be used for
  // brief flashes only.
  virtual void set_overdrive(int led, Color16 c) { set(led, c); }

  // Clear blade colors.
  virtual void clear() {
    for (int i = 0; i < num_leds(); i++) set(i, Color16());
  }

  // Called to let the blade know that it's ok to
  // disable power now. (Usually called after is_on()
  // has returned false for some period of time.)
  virtual void allow_disable() = 0;

  virtual void Activate() = 0;

  // TODO: Move to abstract_blade ?
  virtual BladeStyle* UnSetStyle() {
    BladeStyle *ret = current_style_;
    if (ret) {
      ret->deactivate();
    }
    current_style_ = nullptr;
    return ret;
  }
  virtual void SetStyle(BladeStyle* style) {
    // current_style should be nullptr;
    current_style_ = style;
    if (current_style_) {
      current_style_->activate();
    }
  }

  BladeStyle* current_style() const {
    return current_style_;
  }

 protected:
  BladeStyle *current_style_ = nullptr;
};

template<BladeEffectType effect>
class OneshotEffectDetector {
public:
  BladeEffect* Detect(BladeBase* blade) {
    blade->HandleEffectType(effect);
    BladeEffect* effects;
    size_t n = blade->GetEffects(&effects);
    for (size_t i = 0; i < n; i++) {
      if (effect & effects[i].type) {
	if (effects[i].start_micros == last_detected_)
	  return nullptr;
	last_detected_ = effects[i].start_micros;
	return effects + i;
      }
    }
    return nullptr;
  }
  BladeEffect* getDetected(BladeBase* blade) {
    BladeEffect* effects;
    size_t n = blade->GetEffects(&effects);
    for (size_t i = 0; i < n; i++) {
      if (effect & effects[i].type) {
	if (effects[i].start_micros == last_detected_)
	  return effects + i;
      }
    }
    return nullptr;
  }
  uint32_t last_detected_micros() { return last_detected_; }
private:
  uint32_t last_detected_;
};

#endif
