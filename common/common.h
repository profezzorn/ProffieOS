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

#endif

