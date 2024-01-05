#ifndef STYLES_RAINBOW_H
#define STYLES_RAINBOW_H

// Usage: Rainbow
// return value: COLOR
// Basic RGB rainbow.

class Rainbow {
public:
  void run(BladeBase* base) {
    m = millis();
  }
  SimpleColor getColor(int led) {
    Color16 c(std::max(0, (sin_table[((m * 3 + led * 50)) & 0x3ff] << 2)),
              std::max(0, (sin_table[((m * 3 + led * 50 + 1024 / 3)) & 0x3ff] << 2)),
              std::max(0, (sin_table[((m * 3 + led * 50 + 1024 * 2 / 3)) & 0x3ff] << 2)));
    return SimpleColor(c);
  }
private:
  uint32_t m;
};

#endif
