#ifndef BLADES_BLADE_BASE_H
#define BLADES_BLADE_BASE_H

#include "../styles/blade_style.h"

class BladeBase {
public:
  // Returns number of LEDs in this blade.
  virtual int num_leds() const = 0;

  // Returns true if the blade is supposed to be on.
  // false while "turning off".
  virtual bool is_on() const = 0;

  // Set led 'led' to color 'c'.
  virtual void set(int led, Color16 c) = 0;

  // Bypasses battery voltage based PWM, intended to be used for
  // brief flashes only.
  virtual void set_overdrive(int led, Color16 c) { set(led, c); }

  // Returns true when a clash occurs.
  // Returns true only once per clash.
  virtual bool clash() = 0;

  // Clear blade colors.
  virtual void clear() {
    for (int i = 0; i < num_leds(); i++) set(i, Color16());
  }

  // Called to let the blade know that it's ok to
  // disable power now. (Usually called after is_on()
  // has returned false for some period of time.)
  virtual void allow_disable() = 0;

  virtual void Activate() = 0;

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

#endif
