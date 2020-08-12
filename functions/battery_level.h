#ifndef FUNCTIONS_BATTERY_LEVEL_H
#define FUNCTIONS_BATTERY_LEVEL_H

// Usage: BatteryLevel
// Returns 0-32768 based on battery level.
// returned value: INTEGER

class BatteryLevel {
 public:
  void run(BladeBase* blade) {
    value_ = clampi32(battery_monitor.battery_percent() * 32768 / 100, 0, 32768);
  }
  int getInteger(int led) { return value_; }
  int value_;
};

#endif
