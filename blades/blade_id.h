#ifndef BLADES_BLADE_ID_H
#define BLADES_BLADE_ID_H

#include "../common/analog_read.h"

template<int PIN, int PULLUP_OHMS>
struct ExternalPullupBladeID {
  float id() {
    int blade_id = LSAnalogRead(PIN, INPUT);
    float volts = blade_id * 3.3f / 1024.0f;  // Volts at bladeIdentifyPin
    float amps = (3.3f - volts) / PULLUP_OHMS;
#if 0
   STDOUT << "BLADE ID: " << blade_id << "\n";
   STDOUT << "VOLTS: " << volts << "\n";
   STDOUT << "AMPS: " << amps << "\n";
#endif
    float resistor = volts / amps;
    return resistor;
  }
};

#ifdef TEENSYDUINO
template<int PIN>
struct InternalPullupBladeID {
  float id() {
    int blade_id = LSAnalogRead(PIN, INPUT_PULLUP);
    float volts = blade_id * 3.3f / 1024.0f;  // Volts at bladeIdentifyPin
    float amps = (3.3f - volts) / 33000; // Internal pullups are ~33k
    float resistor = volts / amps;
    return resistor;
  }
};
#endif

#if PROFFIEBOARD_VERSION - 0 > 0
template<int PIN>
struct SnapshotBladeID {
  float id() {
    pinMode(PIN, INPUT_PULLUP);
    delay(1); // let everything settle
    return LSAnalogRead(PIN, INPUT_PULLUP); // 0-1024.0
  }
};
#endif
  
template<int PIN, int PULLUP_PIN>
struct BridgedPullupBladeID {
  float id() {
    pinMode(PULLUP_PIN, INPUT_PULLUP);
#ifdef TEENSYDUINO
    ExternalPullupBladeID<PIN, 33000> ID;
#else    
    ExternalPullupBladeID<PIN, 37000> ID;
#endif    
    float ret = ID.id();
#if PROFFIEBOARD_VERSION == 2
    ret += 470;
#endif    
    pinMode(PULLUP_PIN, INPUT_ANALOG);
    return ret;
  }
};

template<int N, class BLADE_ID>
struct AverageBladeIDNTimes {
  float id() {
    float id = 0;
    BLADE_ID blade_id;
    for (int i = 0; i < N; i++) id += blade_id.id();
    return id / N;
  }
};

template<class POWER_PINS, class BLADE_ID>
struct EnablePowerBladeID {
  float id() {
    int delay_time = 10;
    POWER_PINS power_pins;
#if defined(SHARED_POWER_PINS) && defined(BLADE_ID_SCAN_MILLIS)
    if (power_pins.isOn()) delay_time = 0;
#endif    
    power_pins.Init();
    power_pins.Power(true);
    delay(delay_time);
    BLADE_ID blade_id;
    float ret = blade_id.id();
    power_pins.Power(false);
    power_pins.DeInit();
    return ret;
  }
};

template<int MIN, int MAX, class BLADE_ID>
struct NO_BLADE_RangeID {
  float id() {
    static_assert(MIN >= 0, "NO_BLADE_ID_RANGE min value cannot be less than zero.");
    static_assert(MIN < MAX, "NO_BLADE_ID_RANGE min value must be less than max value.");
    static_assert(MAX < NO_BLADE, "NO_BLADE_ID_RANGE max value must be less than 1000000000.");
    BLADE_ID blade_id;
    float ret = blade_id.id();
    if (ret >= MIN && ret <= MAX) ret += NO_BLADE;
    return ret;
  }
};

struct NoBladeID {
  float id() { return 0.0; }
};

// Define the default blade ID class.

#ifndef BLADE_ID_CLASS

#ifdef TEENSYDUINO
#define BLADE_ID_CLASS InternalPullupBladeID<bladeIdentifyPin>
#elif PROFFIEBOARD_VERSION - 0 >= 3
#define BLADE_ID_CLASS BridgedPullupBladeID<bladeIdentifyPin, bladePin>
#elif defined(PROFFIEBOARD_VERSION)
#define BLADE_ID_CLASS SnapshotBladeID<bladeIdentifyPin>
#else
#define BLADE_ID_CLASS NoBladeID
#endif

#endif  // BLADE_ID_CLASS

#ifdef BLADE_ID_TIMES
#define BLADE_ID_CLASS2 AverageBladeIDNTimes<BLADE_ID_TIMES, BLADE_ID_CLASS>
#else
#define BLADE_ID_CLASS2 BLADE_ID_CLASS
#endif

// Used like this:
// #define NO_BLADE_ID_RANGE 500,550
// where 500 is the lowest value and 550 is the higest value that should return NO_BLADE
#ifdef NO_BLADE_ID_RANGE
#define BLADE_ID_CLASS3 NO_BLADE_RangeID<NO_BLADE_ID_RANGE, BLADE_ID_CLASS2>
#else
#define BLADE_ID_CLASS3 BLADE_ID_CLASS2
#endif

#ifdef ENABLE_POWER_FOR_ID
#define BLADE_ID_CLASS_INTERNAL EnablePowerBladeID<ENABLE_POWER_FOR_ID, BLADE_ID_CLASS3>
#else
#define BLADE_ID_CLASS_INTERNAL BLADE_ID_CLASS3
#endif

#endif  // BLADES_BLADE_ID_H
