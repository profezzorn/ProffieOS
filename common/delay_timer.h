#ifndef DELAY_TIMER_H
#define DELAY_TIMER_H

inline uint32_t& delay_until_ms() {
  static uint32_t ts = 0;
  return ts;
}

inline void StartDelayTimer(uint32_t duration_ms) {
  delay_until_ms() = millis() + duration_ms;
}

inline bool DelayTimerActive() {
  return millis() < delay_until_ms();
}

#endif  // DELAY_TIMER_H
