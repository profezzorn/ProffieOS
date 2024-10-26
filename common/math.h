#ifndef COMMON_MATH_H
#define COMMON_MATH_H

// Returns the decimals of a number, ie 12.2134 -> 0.2134
float fract(float x) {
  return x - floorf(x);
}

// clamp(x, a, b) makes sure that x is between a and b.
float clamp(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
float Fmod(float a, float b) {
  return a - floorf(a / b) * b;
}

int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
int16_t clamptoi16(int32_t x) {
  return clampi32(x, -32768, 32767);
}
int32_t clamptoi24(int32_t x) {
  return clampi32(x, -8388608, 8388607);
}

static inline int32_t MOD(int32_t x, int32_t m) {
  if (x >= 0) return x % m;
  return m + ~(~x % m);
}

#endif  // COMMON_MATH_H
