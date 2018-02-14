#ifndef COMMON_MONITOR_HELPER
#define COMMON_MONITOR_HELPER

#include "looper.h"
#include "command_parser.h"

// Debug printout helper class
class MonitorHelper : Looper, CommandParser {
public:
  MonitorHelper() : Looper(), CommandParser() {}
  const char* name() override { return "MonitorHelper"; }
  
protected:
  void Loop() { monitor.Loop(); }
  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "monitor") || !strcmp(cmd, "mon")) {
      if (!strcmp(arg, "swings")) {
        monitor.Toggle(Monitoring::MonitorSwings);
        return true;
      }
      if (!strcmp(arg, "gyro")) {
        monitor.Toggle(Monitoring::MonitorGyro);
        return true;
      }
      if (!strcmp(arg, "samples")) {
        monitor.Toggle(Monitoring::MonitorSamples);
        return true;
      }
      if (!strcmp(arg, "touch")) {
        monitor.Toggle(Monitoring::MonitorTouch);
        return true;
      }
      if (!strcmp(arg, "battery")) {
        monitor.Toggle(Monitoring::MonitorBattery);
        return true;
      }
      if (!strcmp(arg, "pwm")) {
        monitor.Toggle(Monitoring::MonitorPWM);
        return true;
      }
      if (!strcmp(arg, "clash")) {
        monitor.Toggle(Monitoring::MonitorClash);
        return true;
      }
      if (!strcmp(arg, "temp")) {
        monitor.Toggle(Monitoring::MonitorTemp);
        return true;
      }
      if (!strcmp(arg, "strokes")) {
        monitor.Toggle(Monitoring::MonitorStrokes);
        return true;
      }
      if (!strcmp(arg, "serial")) {
        monitor.Toggle(Monitoring::MonitorSerial);
        return true;
      }
    }
    return false;
  }
  void Help() {
    STDOUT.println(" mon[itor] swings/samples/touch/battery/pwm/clash/temp/serial - toggle monitoring");
  }
};

#endif
