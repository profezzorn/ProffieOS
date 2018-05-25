#ifndef BLADES_POWER_PIN_H
#define BLADES_POWER_PIN_H

class PowerPinInterface {
public:
  virtual void Init() = 0;
  virtual void Power(bool on) = 0;
};

template<int...>
class PowerPINS {};

template<>
class PowerPINS<> : public PowerPinInterface {
public:
  void Init() override {}
  void Power(bool power) override {
    battery_monitor.SetLoad(power);
  }
};

template<int PIN, int... PINS>
class PowerPINS<PIN, PINS...> : public PowerPinInterface {
public:
  void Init() override {
    pinMode(PIN, OUTPUT);
    rest_.Init();
  }
  void Power(bool power) override {
    digitalWrite(PIN, power);
    rest_.Power(power);
  }
private:
  PowerPINS<PINS...> rest_;
};

#endif
