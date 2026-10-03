#ifndef DELAY_TIMER_H
#define DELAY_TIMER_H

class DelayTimer {
 public:
  void Start(uint32_t duration_ms) {
    uint32_t now = millis();
    deadline_ms_ = now + duration_ms;
    PVLOG_DEBUG << "*** TIMER Start: " << duration_ms
                << " ms, now=" << now
                << " deadline=" << deadline_ms_ << "\n";
  }

  void Append(uint32_t duration_ms) {
    uint32_t now = millis();
    uint32_t old_deadline = deadline_ms_;
    if (deadline_ms_ < now) deadline_ms_ = now;
    deadline_ms_ += duration_ms;
    PVLOG_DEBUG << "*** TIMER Append: " << duration_ms
                << " ms, now=" << now
                << " old=" << old_deadline
                << " new=" << deadline_ms_ << "\n";
  }

  void ExtendTo(uint32_t deadline_ms) {
    if (deadline_ms > deadline_ms_) {
      PVLOG_DEBUG << "*** TIMER ExtendTo: old=" << deadline_ms_
                  << " new=" << deadline_ms << "\n";
      deadline_ms_ = deadline_ms;
    }
  }

  bool Active() const {
    return millis() < deadline_ms_;
  }

  // Milliseconds left until the deadline, 0 if it has passed.
  uint32_t remaining() const {
    uint32_t now = millis();
    return (now < deadline_ms_) ? (deadline_ms_ - now) : 0;
  }

  bool Expired() {
    uint32_t now = millis();
    if (deadline_ms_ == 0 || now < deadline_ms_) return false;
    PVLOG_DEBUG << "*** TIMER EXPIRED: now=" << now
                << " deadline=" << deadline_ms_ << "\n";
    deadline_ms_ = 0;
    return true;
  }

  void Stop() {
    deadline_ms_ = 0;
  }

  uint32_t deadline() const {
    return deadline_ms_;
  }

 private:
  uint32_t deadline_ms_ = 0;
};

inline DelayTimer& delay_timer() {
  static DelayTimer timer;
  return timer;
}

#endif  // DELAY_TIMER_H