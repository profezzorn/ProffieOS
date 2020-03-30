#ifndef COMMON_SCOPED_CYCLE_COUNTER_H
#define COMMON_SCOPED_CYCLE_COUNTER_H

class ScopedCycleCounter {
public:
  ScopedCycleCounter(uint64_t& dest) :
    dest_(dest) {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS      
#ifdef TEENSYDUINO
    cycles_ = ARM_DWT_CYCCNT;
#else
    cycles_ = DWT->CYCCNT;
#endif
#endif    
  }
  ~ScopedCycleCounter() {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    uint32_t cycles;
#ifdef TEENSYDUINO
    cycles = ARM_DWT_CYCCNT - cycles_;
    ARM_DWT_CYCCNT = cycles_;
#else
    cycles = DWT->CYCCNT - cycles_;
    DWT->CYCCNT = cycles_;
#endif
    dest_ += cycles;
#endif    
  }
private:
  uint32_t cycles_;
  uint64_t& dest_;
};


#endif
