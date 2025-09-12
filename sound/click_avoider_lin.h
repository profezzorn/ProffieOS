#ifndef SOUND_CLICK_AVOIDER_LIN_H
#define SOUND_CLICK_AVOIDER_LIN_H

#include <algorithm>

// This class is used to control volumes without causing
// clicking noises. Everytime you call advance(), the value
// goes closer towards the target at a preset speed.
class ClickAvoiderLin {
public:
  ClickAvoiderLin() : speed_(0) { }
  ClickAvoiderLin(uint32_t speed) : speed_(speed) { }
  void set_target(uint32_t target) { target_ = target; }
  void set_speed(uint32_t speed) { speed_ = speed; }
  void set(uint16_t v) { current_ = v; }
  void set_dodge(bool do_dodge) {
    dodge_i14_ = do_dodge ? (1 << 12) : (1 << 14);
  }
  uint32_t value() const { return current_; }
  uint32_t get_target() const {
    return (target_ * dodge_i14_) >> 14;
  }
  uint32_t get_speed() const {
    if (dodge_i14_ != (1 << 14)) {
      return 50 * (1 << 14) / AUDIO_RATE; // FIXME: This should use kMaxVolume
    } else {
      return speed_;
    }
  }
  void advance() {
    uint32_t target = get_target();
    uint32_t speed = get_speed();
    if (current_ > target) {
      current_ -= std::min(speed, current_ - target);
      return;
    }
    if (current_ < target) {
      current_ += std::min(speed, target - current_);
      return;
    }
  }
  bool isConstant() const {
    return current_ == get_target();
  }

  uint32_t speed_;
  uint32_t current_;
  uint32_t target_;
  uint32_t dodge_i14_ = 1 << 14;
};

#endif
