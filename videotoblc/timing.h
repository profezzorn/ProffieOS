#ifndef TIMING_H
#define TIMING_H
#include <sys/time.h>
#include <math.h>

static inline struct timeval Time() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t;
}

static inline double operator-(struct timeval a, struct timeval b) {
  return (a.tv_sec - b.tv_sec) + (a.tv_usec - b.tv_usec)/1000000.0;
}

static inline void Sleep(double t) {
  if (t > 0.0) {
    struct timeval tv;
    tv.tv_sec = (int)floor(t);
    tv.tv_usec = (int)floor((t - tv.tv_sec) * 1000000);
    select(0, NULL, NULL, NULL, &tv);
  }
}

#endif
