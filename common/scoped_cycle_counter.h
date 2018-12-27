#ifndef COMMON_SCOPED_CYCLE_COUNTER_H
#define COMMON_SCOPED_CYCLE_COUNTER_H

class ScopedCycleCounter {
public:
  ScopedCycleCounter(uint64_t& dest) :
    dest_(dest) {
#ifdef TEENSYDUINO
    cycles_ = ARM_DWT_CYCCNT;
#else
    cycles_ = DWT->CYCCNT;
#endif
  }
  ~ScopedCycleCounter() {
#ifdef TEENSYDUINO
    cycles_ = ARM_DWT_CYCCNT - cycles_;
#else
    cycles_ = DWT->CYCCNT - cycles_;
#endif
    dest_ += cycles_;
  }
private:
  uint32_t cycles_;
  uint64_t& dest_;
};

#endif
