#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

// No unique address saves some RAM.
// but it's not available in older compilers.
#if __has_cpp_attribute(no_unique_address)
#define PONUA [[no_unique_address]]
#else
#define PONUA
#endif

#endif

