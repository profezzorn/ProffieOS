#ifndef NEWFONT_WAIT_H
#define NEWFONT_WAIT_H

inline uint32_t& newfont_wait_until_ms() {
  static uint32_t ts = 0;
  return ts;
}

inline void StartNewFontWait(uint32_t duration_ms) {
  newfont_wait_until_ms() = millis() + duration_ms;
}

inline bool NewFontWaitActive() {
  return millis() < newfont_wait_until_ms();
}

#endif  // NEWFONT_WAIT_H