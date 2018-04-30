#ifndef COMMON_BITFIELD_H
#define COMMON_BITFIELD_H

// This class describes a bitfield, for examples of how to use it
// see ../buttons/stm32l4_touchbutton.h

#ifdef STM32L4
// TODO: replace atomics with inline code.
#include <armv7_atomic.h>
#endif

template<typename T, class RegisterClass> 
struct BitField {
  template<int POS = 0, int BITS = 0>
  struct Field {
    Field(T v, T m = MASK) : value(v), mask(m) {}
    static const T MASK = (~(~(T)0) << BITS) << POS;
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
  T set(const Field<POS, BITS>& v) volatile {
#ifdef STM32L4
    armv7_atomic_modify(&reg->value, v.mask, v.value);
#else
    value = (value & v.mask) | v.value;
#endif
  }
  T value;
};

#endif
