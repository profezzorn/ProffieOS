#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

// For std::min, and other things.
#include <algorithm>

// For static_assert, and other things.
#include <type_traits>

// No unique address saves some RAM.
// but it's not available in older compilers.
#if __has_cpp_attribute(no_unique_address)
#define PONUA [[no_unique_address]]
#else
#define PONUA
#endif

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))
const char install_time[] = __DATE__ " " __TIME__
#ifdef INSTALL_TIME_EXTRA
  INSTALL_TIME_EXTRA
#endif  
  ;

#ifdef DEBUG
#define PROFFIEOS_ASSERT(X) do {					\
  if (!(X)) {								\
    interrupts();							\
    if (!(X)) STDERR << "ASSERT " << #X << " FAILED @ " << __FILE__ << ":" << __LINE__ << "\n"; \
    while(true);							\
  }									\
} while(0)
#else
#define PROFFIEOS_ASSERT(X) do {} while(0)
#endif

#endif

