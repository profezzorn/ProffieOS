#ifndef COMMON_BATTERY_MONITOR_H
#define COMMON_BATTERY_MONITOR_H

class BatteryMonitor : Looper, CommandParser {
public:
  const char* name() override { return "BatteryMonitor"; }
  float battery() const {
    return last_voltage_;
  }
  void SetLoad(bool on) {
    loaded_ = on;
  }
  bool low() const {
    // Battery isn't low if it's not connected at all.
    return battery() < (loaded_ ? 2.6 : 3.0) && battery() > 0.5;
  }
  float battery_percent() {
    // Energy is roughly proportional to voltage squared.
    float v = battery();
    float min_v = 3.0;
    float max_v = 4.2;
    return 100.0 * (v * v - min_v * min_v) / (max_v * max_v - min_v * min_v);
  }
  void SetPinHigh(bool go_high) {
    if (go_high) {
      pinMode(batteryLevelPin, OUTPUT);
      digitalWrite(batteryLevelPin, HIGH);
    } else {
#if VERSION_MAJOR >= 2
      pinMode(batteryLevelPin, INPUT);
#else
      pinMode(batteryLevelPin, INPUT_PULLDOWN);
#endif
      // delay reading voltage for a bit to let it settle.
      last_voltage_read_time_ = micros();
    }
  }
protected:
  void Setup() override {
    really_old_voltage_ = old_voltage_ = last_voltage_ = battery_now();
    SetPinHigh(false);
  }
  void Loop() override {
    uint32_t now = micros();
    if (now - last_voltage_read_time_ >= 1000) {
      float v = battery_now();
      last_voltage_ = last_voltage_ * 0.999 + v * 0.001;
      last_voltage_read_time_ = now;
    }
    if (monitor.ShouldPrint(Monitoring::MonitorBattery) ||
        millis() - last_print_millis_ > 20000) {
      STDOUT.print("Battery voltage: ");
      STDOUT.println(battery());
      last_print_millis_ = millis();
    }
  }

  bool Parse(const char* cmd, const char* arg) override {
    if (!strcmp(cmd, "battery_voltage")) {
      STDOUT.println(battery());
      return true;
    }
    if (!strcmp(cmd, "batt") || !strcmp(cmd, "battery")) {
      STDOUT.print("Battery voltage: ");
      float v = battery();
      STDOUT.println(v);
#ifdef ENABLE_AUDIO
      talkie.SayDigit((int)floor(v));
      talkie.Say(spPOINT);
      talkie.SayDigit(((int)floor(v * 10)) % 10);
      talkie.SayDigit(((int)floor(v * 100)) % 10);
      talkie.Say(spVOLTS);
#endif
      return true;
    }
    return false;
  }
  void Help() override {
    STDOUT.println(" batt[ery[_voltage]] - show battery voltage");
  }
private:
  float battery_now() {
    // This is the volts on the battery monitor pin.
    // TODO: analogRead can be very slow, make an async one and/or read it less often.
    float volts = 3.3 * analogRead(batteryLevelPin) / 1024.0;
#ifdef V2
    float pulldown = 220000;  // External pulldown
    float pullup = 2000000;  // External pullup
#else
    float pulldown = 33000;  // Internal pulldown is 33kOhm
    float pullup = BATTERY_PULLUP_OHMS;  // External pullup
#endif
    return volts * (1.0 + pullup / pulldown);
  }

  bool loaded_ = false;
  float last_voltage_ = 0.0;
  uint32_t last_voltage_read_time_ = 0;
  float old_voltage_ = 0.0;
  float really_old_voltage_ = 0.0;
  uint32_t last_print_millis_;
  uint32_t last_beep_ = 0;
};

BatteryMonitor battery_monitor;

#endif
