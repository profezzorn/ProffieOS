#ifndef BLADES_BLADE_BASE_H
#define BLADES_BLADE_BASE_H

#include "../common/saber_base.h"

using BladeEffectType = EffectType;

  enum HandledFeature {
    HANDLED_FEATURE_NONE = 0,
    HANDLED_FEATURE_CHANGE = 1 << 0,
    HANDLED_FEATURE_CHANGE_TICKED = 1 << 1,
    HANDLED_FEATURE_STAB = 1 << 2,
    HANDLED_FEATURE_DRAG = 1 << 3,
    HANDLED_FEATURE_MELT = 1 << 4,
    HANDLED_FEATURE_LIGHTNING_BLOCK = 1 << 5,
  };

#include "../styles/blade_style.h"

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
  virtual bool IsPrimary() = 0;

  virtual void Activate() = 0;
  virtual void Deactivate() = 0;

  virtual BladeStyle* UnSetStyle() = 0;
  virtual void SetStyle(BladeStyle* style) = 0;
  virtual BladeStyle* current_style() const = 0;

  // Let the blade know that this style handles "effect".
  static void HandleFeature(HandledFeature feature) {
    handled_features_ = (HandledFeature) ((int)handled_features_ | (int)feature);
  }

  static HandledFeature GetHandledTypes() {
    return handled_features_;
  }
  static void ResetHandledTypes() {
    handled_features_ = HANDLED_FEATURE_NONE;
  }

  
 protected:
  static HandledFeature handled_features_;
};

HandledFeature BladeBase::handled_features_ = HANDLED_FEATURE_NONE;

template<BladeEffectType effect>
class OneshotEffectDetector {
public:
  OneshotEffectDetector() {
    switch (effect) {
      case EFFECT_STAB:
	BladeBase::HandleFeature(HANDLED_FEATURE_STAB);
	break;
      default:
	break;
    }
  }
  BladeEffect* Detect(BladeBase* blade) {
    BladeEffect* effects;
    size_t n = blade->GetEffects(&effects);
    // If no other thing is handling stab, treat it like a clash.
    // But only for the primary blade...
    bool match_stab = effect == EFFECT_CLASH &&
      !blade->current_style()->IsHandled(HANDLED_FEATURE_STAB) &&
      blade->IsPrimary();
    for (size_t i = 0; i < n; i++) {
      if (effect == effects[i].type ||
	  (match_stab && effects[i].type == EFFECT_STAB)) {
	if (effects[i].start_micros == last_detected_)
	  return nullptr;
	last_detected_ = effects[i].start_micros;
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
