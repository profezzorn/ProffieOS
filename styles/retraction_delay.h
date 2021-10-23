#ifndef STYLES_RETRACTION_DELAY_H
#define STYLES_RETRACTION_DELAY_H

// Usage: RetractionDelay<DELAY_MILLIS, BASE>
// DELAY_MILLIS: a number
// BASE: COLOR
// return value: COLOR
// This class renders BASE as normal, but delays retraction by
// the specified number of milliseconds.
class BladeBase;
template<class MILLIS>
class RetractionDelayBase : public BladeWrapper {
public:
  void run(BladeBase* base) {
    millis_.run(base);
    blade_ = base;
    if (!base->is_on()) {
      if (!waiting_) {
        waiting_ = true;
        wait_start_time_ = millis();
      }
      uint32_t waited = millis() - wait_start_time_;
      int delay_millis = millis_.calculate(base);
      if (waited > delay_millis) {
        is_on_ = false;
        wait_start_time_ = millis() - delay_millis - 1;
      }
    } else {
      waiting_ = false;
      is_on_ = true;
    }
  }
  bool is_on() const override { return is_on_; }
private:
  bool is_on_ = false;
  bool waiting_ = false;
  uint32_t wait_start_time_;
  PONUA SVFWrapper<MILLIS> millis_;
};

template<class MILLIS, class BASE>
class RetractionDelayX : public RetractionDelayBase<MILLIS> {
public:
  bool run(BladeBase* base) __attribute__((warn_unused_result)) {
    RetractionDelayBase<MILLIS>::run(base);
    return RunStyle(&base_, this);
  }
private:
  BASE base_;
public:
  auto getColor(int led) ->decltype(base_.getColor(led)) { return base_.getColor(led); }
};

template<int millis, class BASE>
  using RetractionDelay = RetractionDelayX<Int<millis>, BASE>;

#endif
