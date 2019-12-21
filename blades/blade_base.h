#ifndef BLADES_BLADE_BASE_H
#define BLADES_BLADE_BASE_H

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

  // Blaster Effect Types
  EFFECT_STUN = 1 << 12, // used to pass STUN to allow alternate colors for stun type blasts
  EFFECT_FIRE = 1 << 13,
  EFFECT_CLIP_IN = 1 << 14,
  EFFECT_CLIP_OUT = 1 << 15,
  EFFECT_RELOAD = 1 << 16,
  EFFECT_MODE = 1 << 17,
  EFFECT_RANGE = 1 << 18,
  EFFECT_EMPTY = 1 << 19,
  EFFECT_FULL = 1 << 20,
  EFFECT_JAM = 1 << 21,
  EFFECT_UNJAM = 1 << 22,
  EFFECT_PLI_ON = 1 << 23,
  EFFECT_PLI_OFF = 1 << 24,
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
  static void HandleEffectType(BladeEffectType effect) {
    handled_types_ = (BladeEffectType) ((int)handled_types_ | (int)effect);
  }

  // Returns true if the current style handles a particular effect type.
  static bool IsHandled(BladeEffectType effect) {
    return (handled_types_ & effect) != 0;
  }

  static BladeEffectType GetHandledTypes() {
    return handled_types_;
  }
  static void ResetHandledTypes() {
    handled_types_ = EFFECT_NONE;
  }

  
 protected:
  static BladeEffectType handled_types_;
};

BladeEffectType BladeBase::handled_types_ = EFFECT_NONE;

template<BladeEffectType effect>
class OneshotEffectDetector {
public:
  OneshotEffectDetector() {
    BladeBase::HandleEffectType(effect);
  }
  BladeEffect* Detect(BladeBase* blade) {
    BladeEffect* effects;
    size_t n = blade->GetEffects(&effects);
    BladeEffectType mask = effect;
    // If no other thing is handling stab, treat it like a clash.
    // But only for the primary blade...
    if ((effect & EFFECT_CLASH) && !blade->current_style()->IsHandled(EFFECT_STAB) &&
	blade->IsPrimary()) {
      mask = (BladeEffectType)(((int)mask) | ((int)EFFECT_STAB));
    }
    for (size_t i = 0; i < n; i++) {
      if (mask & effects[i].type) {
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
