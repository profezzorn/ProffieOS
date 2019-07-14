#ifndef BLADES_POWER_PIN_H
#define BLADES_POWER_PIN_H

class PowerPinInterface {
public:
  virtual void Init() = 0;
  virtual void Power(bool on) = 0;
};


#ifndef SHARED_POWER_PINS

template<int PIN>
class PowerPinWrapper : public PowerPinInterface {
public:
  void Init() override {
    pinMode(PIN, OUTPUT);
  }
  void Power(bool power) override {
    digitalWrite(PIN, power);
  }
};

#else

template<int PIN>
class PowerPinSingleton {
public:
  static void Init() {
    if (refs_ == 255) {
      pinMode(PIN, OUTPUT);
      digitalWrite(PIN, 0);
      refs_ = 0;
    }
  }
  static void Power(bool on) {
    refs_ += on ? 1 : -1;
    digitalWrite(PIN, refs_ != 0);
  }
private:
  static uint8_t refs_;
};
template<int PIN>
uint8_t PowerPinSingleton<PIN>::refs_ = 0;

template<int PIN>
class PowerPinWrapper : public PowerPinInterface {
public:
  void Init() override {
    PowerPinSingleton<PIN>::Init();
  }
  void Power(bool power) override {
    if (power == on_) return;
    on_ = power;
    PowerPinSingleton<PIN>::Power(power);
  }
private:
  bool on_ = false;
};
#endif

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
    pin_.Init();
    rest_.Init();
  }
  void Power(bool power) override {
    pin_.Power(power);
    rest_.Power(power);
  }
private:
  PowerPinWrapper<PIN> pin_;
  PowerPINS<PINS...> rest_;
};

#endif
