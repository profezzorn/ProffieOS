#ifndef COMMON_LINKED_LIST_H
#define COMMON_LINKED_LIST_H

#include "malloc_helper.h"

// Magic type used to prevent linked-list types from automatically linking.
enum NoLink { NOLINK = 17 };

// Magic type for HF linking
enum HFLink { HFLINK = 18 };

#ifdef ENABLE_DEBUG

#if 0
#define CHECK_LL(T, START, NEXT) do {                                   \
  int len = 0;                                                          \
  for (T* i = START; i; i = i->NEXT) {                                  \
    if (abs(((long)i) - (long)&START) > RamSize()) {			\
      STDOUT.print("Linked list " #START " has invalid pointer @ ");    \
      STDOUT.print(__LINE__);                                           \
      STDOUT.print(" pointer: ");                                       \
      STDOUT.println((long)i, 16);                                      \
      START = NULL;                                                     \
      break;                                                            \
    }                                                                   \
    if (++len > 1000) {                                                 \
      STDOUT.print("Linked list " #START " has become infinite @ ");    \
      STDOUT.println(__LINE__);                                         \
      i->NEXT = NULL;                                                   \
      break;                                                            \
    }                                                                   \
  }                                                                     \
} while(0)

#else

// If we have a debugger, this is better.

#define CHECK_LL(T, START, NEXT) do {                                   \
  int len = 0;                                                          \
  for (T* i = START; i; i = i->NEXT) {                                  \
    ++len;                                                              \
    PROFFIEOS_ASSERT(abs(((long)i) - (long)&START) < RamSize());	\
    PROFFIEOS_ASSERT(len <= 1000); 					\
  }                                                                     \
} while(0)

#endif

#else
#define CHECK_LL(T, START, NEXT)
#endif

#endif
