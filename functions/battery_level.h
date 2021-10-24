#ifndef FUNCTIONS_BATTERY_LEVEL_H
#define FUNCTIONS_BATTERY_LEVEL_H

#include "svf.h"

// Usage: BatteryLevel
// Returns 0-32768 based on battery level.
// returned value: INTEGER

class BatteryLevelSVF {
 public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    return clampi32(battery_monitor.battery_percent() * 32768 / 100, 0, 32768);
  }
};

using BatteryLevel = SingleValueAdapter<BatteryLevelSVF>;

#endif
