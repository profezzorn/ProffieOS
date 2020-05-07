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
  uint32_t value() const {return current_; }
  void advance() {
    uint32_t target = target_;
    if (current_ > target) {
      current_ -= std::min(speed_, current_ - target);
      return;
    }
    if (current_ < target) {
      current_ += std::min(speed_, target - current_);
      return;
    }
  }
  bool isConstant() const {
    return current_ == target_;
  }

  uint32_t speed_;
  uint32_t current_;
  uint32_t target_;
};

#endif
