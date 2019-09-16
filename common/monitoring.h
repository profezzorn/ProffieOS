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
  };

  bool ShouldPrint(MonitorBit bit) {
    if (bit & monitor_soon_) {
      monitor_soon_ &= ~bit;
      return true;
    }
    return false;
  }

  bool ShouldPrintMultiple(MonitorBit bit) {
    if (bit & monitor_soon_) {
      monitored_ |= bit;
      return true;
    }
    return false;
  }
  
  bool IsMonitoring(MonitorBit bit) const {
    return (active_monitors_ & bit) != 0;
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

#endif
