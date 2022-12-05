#ifndef COMMON_SCOPED_CYCLE_COUNTER_H
#define COMMON_SCOPED_CYCLE_COUNTER_H

class ScopedCycleCounter {
public:
  static inline uint32_t getCycles() {
#if defined(TEENSYDUINO)
    return ARM_DWT_CYCCNT - counted_cycles_;
#elif defined(ARDUINO_ARCH_STM32L4)
    return DWT->CYCCNT - counted_cycles_;
#elif defined(ESP32)
    return cpu_hal_get_cycle_count();
#else    
    return 0;
#endif    
  }
  ScopedCycleCounter(uint64_t& dest) :
    dest_(dest) {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    cycles_ = getCycles();
#endif    
  }
  ~ScopedCycleCounter() {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    uint32_t cycles;
    cycles = getCycles() - cycles_;
    noInterrupts();
    counted_cycles_ += cycles;
    interrupts();
    dest_ += cycles;
#endif
  }
private:
  static uint32_t counted_cycles_;
  uint32_t cycles_;
  uint64_t& dest_;
};

uint32_t ScopedCycleCounter::counted_cycles_ = 0;

#endif
