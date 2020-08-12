#ifndef COMMON_BATTERY_MONITOR_H
#define COMMON_BATTERY_MONITOR_H

#ifndef NO_BATTERY_MONITOR

#include "analog_read.h"

class BatteryMonitor : Looper, CommandParser, StateMachine {
public:
BatteryMonitor() : reader_(batteryLevelPin,
			     INPUT
#if VERSION_MAJOR == 5
                             , 10e-6
#endif
   ) {}
  const char* name() override { return "BatteryMonitor"; }
  float battery() const {
    return last_voltage_;
  }
  void SetLoad(bool on) {
    loaded_ = on;
  }
  bool low() const { return low_count_ > 1000; }
  float battery_percent() {
    // Energy is roughly proportional to voltage squared.
    float v = battery();
    float min_v = 3.0;
    float max_v = 4.2;
    return 100.0 * (v * v - min_v * min_v) / (max_v * max_v - min_v * min_v);
//    return 100.0 * (v - min_v) / (max_v - min_v);
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
    last_voltage_ = battery_now();
    SetPinHigh(false);
  }
  void Loop() override {
    if (monitor.ShouldPrint(Monitoring::MonitorBattery) ||
        millis() - last_print_millis_ > 20000) {
      STDOUT.print("Battery voltage: ");
      STDOUT.println(battery());
      last_print_millis_ = millis();
    }

    STATE_MACHINE_BEGIN();
    last_voltage_read_time_ = micros();
    while (true) {
      while (micros() - last_voltage_read_time_ < 1000) YIELD();
      while (!reader_.Start()) YIELD();
      while (!reader_.Done()) YIELD();
      float v = battery_now();
      uint32_t now = micros();
      float mul = powf(0.05, (now - last_voltage_read_time_) / 1000000.0);
      last_voltage_read_time_ = now;
      last_voltage_ = last_voltage_ * mul + v * (1 - mul);
      if (IsLow()) {
	low_count_++;
      } else {
	low_count_ = 0;
      }
    }
    STATE_MACHINE_END();
  }

  bool IsLow() {
#if VERSION_MAJOR >= 4
    if (USBD_Connected()) return false;
#endif
    // Battery isn't low if it's not connected at all.
    if (battery() < 0.5) return false;
    
    return battery() < (loaded_ ? 2.6 : 3.0);
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
      talkie.SayDigit((int)floorf(v));
      talkie.Say(spPOINT);
      talkie.SayDigit(((int)floorf(v * 10)) % 10);
      talkie.SayDigit(((int)floorf(v * 100)) % 10);
      talkie.Say(spVOLTS);
#endif
      return true;
    }
#if 0
    if (!strcmp(cmd, "bstate")) {
      STDOUT.print("Next state: ");
      STDOUT.println(reader_.state_machine_.next_state_);
      STDOUT.print("ADC SMP: ");
      STDOUT.println(reader_.adc_smp_);
      STDOUT.print("ADC state: ");
      STDOUT.println(stm32l4_adc.state);
      return true;
    }
#endif
    return false;
  }
  void Help() override {
    STDOUT.println(" batt[ery[_voltage]] - show battery voltage");
  }
private:
  float battery_now() {
    // This is the volts on the battery monitor pin.
    float volts = 3.3 * reader_.Value() / 1024.0;
#if VERSION_MAJOR == 5
    return volts * 2.0;
#else
#ifdef V2
    float pulldown = 220000;  // External pulldown
    float pullup = 2000000;   // External pullup
#else
    float pulldown = 33000;  // Internal pulldown is 33kOhm
    float pullup = BATTERY_PULLUP_OHMS;  // External pullup
#endif
    return volts * (1.0 + pullup / pulldown);
#endif
  }

  bool loaded_ = false;
  float last_voltage_ = 0.0;
  uint32_t last_voltage_read_time_ = 0;
  uint32_t last_print_millis_;
  uint32_t low_count_ = 0;
  AnalogReader reader_;
};

#else  // NO_BATTERY_MONITOR

class BatteryMonitor {
public:
  float battery() const { return 3.7f; }
  void SetLoad(bool on) {}
  bool low() const { return false; }
  float battery_percent() { return 100.0f;  }
  void SetPinHigh(bool go_high) {}
};

#endif  // NO_BATTERY_MONITOR

BatteryMonitor battery_monitor;

#endif
