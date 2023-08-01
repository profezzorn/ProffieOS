#ifndef COMMON_ATOMIC_H
#define COMMON_ATOMIC_H

#ifdef ESP32
#define PROFFIEOS_USE_ATOMICS
#endif

#ifdef PROFFIEOS_USE_ATOMICS

#include <atomic>

#ifndef ATOMIC_BOOL_LOCK_FREE
#error Atomic bool is not lock free
#endif

#ifndef ATOMIC_INT_LOCK_FREE
#error Atomic int is not lock free
#endif

#ifndef ATOMIC_POINTER_LOCK_FREE
#error Atomic pointer is not lock free
#endif


template<class T> class POAtomic {
public:
  POAtomic() : value_() {}
  POAtomic(T value) : value_(value) {}
  
  T get() {
    return value_.load(std::memory_order_acquire);
  }
  T get() const {
    return value_.load(std::memory_order_acquire);
  }
  void set(T value) {
    value_.store(value, std::memory_order_release);
  }
  T exchange(T value) {
    return value_.exchange(value, std::memory_order_release);
  }
  void operator+=(T value) { value_ += value; }
  void operator-=(T value) { value_ -= value; }
  
private:
  std::atomic<T> value_;
};

#else

template<class T> class POAtomic {
public:
  POAtomic() : value_() {}
  POAtomic(T value) : value_(value) {}
  T get() { return value_; }
  T get() const { return value_; }
  void set(T value) {
    value_ = value;
  }
  void operator+=(T value) {
#ifdef ARDUINO_ARCH_STM32L4
    __atomic_fetch_add(&value_, value, __ATOMIC_RELAXED);
#else
    value_ += value;
#endif    
  }
  void operator-=(T value) {
#ifdef ARDUINO_ARCH_STM32L4
    __atomic_fetch_sub(&value_, value, __ATOMIC_RELAXED);
#else
    value_ -= value;
#endif    
  }
  
private:
  volatile T value_;
#ifndef PROFFIE_TEST  
  static_assert(sizeof(T) <= 4, "Atomic must be 32 bits or less.");
#endif
};

#endif // PROFFIEOS_USE_ATOMICS

#endif // COMMON_ATOMIC_H
