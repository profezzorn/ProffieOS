#ifndef COMMON_BITFIELD_H
#define COMMON_BITFIELD_H

// This class describes a bitfield, for examples of how to use it
// see ../buttons/stm32l4_touchbutton.h

#ifdef __STM32L4_CMSIS_VERSION
// TODO: replace atomics with inline code.
#include <armv7m_atomic.h>
#define USE_ATOMICS
#endif

template<typename T, class RegisterClass> 
struct BitField {
  template<int POS = 0, int BITS = 0>
  struct Field {
    static const T MASK = (~((~(T)0) << BITS)) << POS;
    Field(T v) : value(v << POS), mask(MASK) {}
    Field(T v, T m) : value(v), mask(m) {}
    static T get(const BitField& reg) {
      return (reg.value & MASK) >> POS;
    }
    static T get(const volatile BitField& reg) {
      return (reg.value & MASK) >> POS;
    }
    static T get(volatile BitField& reg) {
      return (reg.value & MASK) >> POS;
    }
    static T get(BitField& reg) {
      return (reg.value & MASK) >> POS;
    }
    template<int OTHER_POS, int OTHER_BITS>
    Field operator|(const Field<OTHER_POS, OTHER_BITS>& other) const {
      return Field(value | other.value, mask | other.mask);
    }
    T value;
    T mask;
  };

  template<class FIELD> T get() volatile { return FIELD::get(*this); }

  template<int POS, int BITS>
  void set(const Field<POS, BITS>& v) volatile {
#ifdef USE_ATOMICS
    armv7m_atomic_modify(&value, v.mask, v.value);
#else
    value = (value & ~v.mask) | v.value;
#endif
  }
  T value;
};

#endif
