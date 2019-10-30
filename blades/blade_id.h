#ifndef BLADES_BLADE_ID_H
#define BLADES_BLADE_ID_H

#include "../common/analog_read.h"

template<int PIN, int PULLUP>
struct ExternalPullupBladeID {
  float id() {
    int blade_id = LSAnalogRead(PIN, INPUT);
    float volts = blade_id * 3.3f / 1024.0f;  // Volts at bladeIdentifyPin
    float amps = (3.3f - volts) / PULLUP;
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
    delay(100); // let everything settle
    return LSAnalogRead(PIN, INPUT_PULLUP); // 0-1024.0
  }
};
#endif
  
template<int PIN, int PULLUP_PIN>
struct BridgedPullupBladeID {
  float id() {
    pinMode(PIN, INPUT_PULLUP);
#ifdef TEENSYDUINO
    ExternalPullupBladeID<PIN, 33000> ID;
#else    
    ExternalPullupBladeID<PIN, 40000> ID;
#endif    
    float ret = ID.id();
#if PROFFIEBOARD_VERSION == 2
    ret += 470;
#endif    
    pinMode(PIN, INPUT_ANALOG);
    return ret;
  }
};

template<class POWER_PINS, class BLADE_ID>
struct EnablePowerBladeID {
  float id() {
    POWER_PINS power_pins;
    power_pins.Init();
    power_pins.Power(true);
    delay(10);
    BLADE_ID blade_id;
    float ret = blade_id.id();
    power_pins.Power(false);
    return ret;
  }
};

// Define the default blade ID class.

#ifndef BLADE_ID_CLASS

#ifdef TEENSYDUINO
#define BASIC_BLADE_ID_CLASS InternalPullupBladeID<bladeIdentifyPin>
#else
#define BASIC_BLADE_ID_CLASS SnapshotBladeID<bladeIdentifyPin>
#endif

#ifdef ENABLE_POWER_FOR_ID
#define BLADE_ID_CLASS EnablePowerBladeID<ENABLE_POWER_FOR_ID, BASIC_BLADE_ID_CLASS>
#else
#define BLADE_ID_CLASS BASIC_BLADE_ID_CLASS
#endif

#endif  // BLADE_ID_CLASS

#endif  // BLADES_BLADE_ID_H
