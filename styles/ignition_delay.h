#ifndef STYLES_IGNITION_DELAY_H
#define STYLES_IGNITION_DELAY_H

// Usage: IgnitionDelay<DELAY_MILLIS, BASE>
// DELAY_MILLIS: a number
// BASE: COLOR
// return value: COLOR
// This class renders BASE as normal, but delays ignition by
// the specified number of milliseconds. Intended for kylo-style
// quillions.
class BladeBase;
template<int delay_millis, class BASE>
class IgnitionDelay : public BladeWrapper {
public:
  bool run(BladeBase* base)  __attribute__((warn_unused_result)) {
    blade_ = base;
    if (base->is_on()) {
      if (!waiting_) {
        waiting_ = true;
        wait_start_time_ = millis();
      }
      uint32_t waited = millis() - wait_start_time_;
      if (waited > delay_millis) {
        is_on_ = true;
        wait_start_time_ = millis() - delay_millis - 1;
      }
    } else {
      waiting_ = false;
      is_on_ = false;
    }
    return RunStyle(&base_, this);
  }
  OverDriveColor getColor(int led) { return base_.getColor(led); }
  bool is_on() const override { return is_on_; }
private:
  bool is_on_ = false;
  bool waiting_ = false;
  uint32_t wait_start_time_;
  BASE base_;
};

#endif
