#ifndef COMMON_LOOP_COUNTER_H
#define COMMON_LOOP_COUNTER_H

class LoopCounter {
public:
  float LoopsPerSecond() {
    if (!millis_sum_) return 0.0;
    return updates_ * 1000.0 / millis_sum_;
  }
  void Print() {
    if (millis_sum_)
      STDOUT.print(updates_ * 1000.0 / millis_sum_);
  }
  void Reset() {
    updates_ = 0;
    millis_sum_ = 0;
    last_millis_ = 0;
  }
  void Update() {
    uint32_t m = millis();
    if (last_millis_) {
      millis_sum_ += m - last_millis_;
      updates_++;
      if (updates_ > 1000) {
         updates_ /= 2;
         millis_sum_ /= 2;
      }
    }
    last_millis_ = m;
  }
private:
  int updates_ = 0;
  int millis_sum_ = 0;
  uint32_t last_millis_ = 0;
};

#endif
