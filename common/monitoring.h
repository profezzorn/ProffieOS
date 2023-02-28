#ifndef COMMON_MONITORING_H
#define COMMON_MONITORING_H

// Debug printout helper class
class Monitoring  {
public:
  enum MonitorBit {
    MonitorSwings = 1,
    MonitorSamples = 2,
    MonitorTouch = 4,
    MonitorBattery = 8,
    MonitorPWM = 16,
    MonitorClash = 32,
    MonitorTemp = 64,
    MonitorGyro = 128,
    MonitorStrokes = 256,
    MonitorSerial = 512,
    MonitorFusion = 1024,
    MonitorVariation = 2048,
  };

  bool ShouldPrint(MonitorBit bit) {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (bit & monitor_soon_) {
      monitor_soon_ &= ~bit;
      return true;
    }
#endif
    return false;
  }

  bool ShouldPrintMultiple(MonitorBit bit) {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (bit & monitor_soon_) {
      monitored_ |= bit;
      return true;
    }
#endif
    return false;
  }

  bool IsMonitoring(MonitorBit bit) const {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    return (active_monitors_ & bit) != 0;
#else
    return false;
#endif
  }
  
  void Loop() {
    monitor_soon_ &= ~monitored_;
    monitored_ = 0;
    if (millis() - last_monitor_loop_ > monitor_frequency_ms_) {
      monitor_soon_ = active_monitors_;
      last_monitor_loop_ = millis();
    }
  }

  void Toggle(MonitorBit bit) {
    active_monitors_ ^= bit;
  }
private:
  uint32_t monitor_frequency_ms_ = 200;
  int last_monitor_loop_ = 0;
  uint32_t monitor_soon_ = 0;
  uint32_t monitored_ = 0;
  uint32_t active_monitors_ = 0;
};

extern Monitoring monitor;

#define STRINGIFY(...) #__VA_ARGS__
#define TOSTRING(...) STRINGIFY(__VA_ARGS__)

#define TRACE_CATEGORY_BLADE 0x1
#define TRACE_CATEGORY_MOTION 0x2
#define TRACE_CATEGORY_IR 0x4
#define TRACE_CATEGORY_PROP 0x8
#define TRACE_CATEGORY_I2C 0x10

#define TRACE_EXPAND_AGAIN(CAT) (CAT)
#define TRACE_CATEGORY(CAT) TRACE_EXPAND_AGAIN(TRACE_CATEGORY_##CAT)

#ifdef ENABLE_TRACING
#if (ENABLE_TRACING - 0) == 0
#define TRACING_CATEGORIES -1
#else
#define TRACING_CATEGORIES ENABLE_TRACING
#endif

// TODO: Move this somewhere more global
volatile const char* trace[128];
volatile int trace_pos;

void DoTrace(const char* str) {
  noInterrupts();
  trace[trace_pos++ & 127] = str;
  interrupts();
}

#define TRACE(CAT, X) do {				\
  if (TRACE_CATEGORY(CAT) & (TRACING_CATEGORIES))       \
    DoTrace(__FILE__ ":" TOSTRING(__LINE__) ": " X);	\
} while(0)
#else
#define TRACING_CATEGORIES 0
#define TRACE(CAT, X) do {				\
  if (TRACE_CATEGORY(CAT) & (TRACING_CATEGORIES))       \
    do { } while(0);					\
} while(0)
#endif // ENABLE_TRACING


#endif
