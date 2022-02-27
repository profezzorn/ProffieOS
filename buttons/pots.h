#ifndef BUTTONS_POTS_H
#define BUTTONS_POTS_H

class AnalogReceiver {
public:
  // Value is 0.0 - 1.0, possibly with wraparound.
  virtual void Set(float value) = 0;
};

class ChangeVariationAnalogReceiver : public AnalogReceiver {
public:
  void Set(float value) override {
    SaberBase::SetVariation(value * 32767);
  }
};

#ifdef ENABLE_AUDIO
class ChangeVolumeAnalogReceiver : public AnalogReceiver {
public:
  void Set(float value) override {
    dynamic_mixer.set_volume(VOLUME * value);
  }
};
#endif

class BasePotClass : public Looper, StateMachine {
public:
  const char* name() override { return "BasePotClass"; }
  BasePotClass(int pin) : reader_(pin, INPUT)  {}
  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (micros() - last_read_time_ < 1000) YIELD();
      while (!reader_.Start()) YIELD();
      while (!reader_.Done()) YIELD();
      uint32_t now = micros();
      float mul = expf(logf(0.1) * (now - last_read_time_) / 1000000.0);
      last_read_time_ = now;
      reading_ = reading_ * mul + reader_.Value() * (1.0/1024.0) * (1.0 - mul);
      HaveResult();
    }
    STATE_MACHINE_END();
  }
  virtual void HaveResult() {}

  float reading_ = 0.0;
  uint32_t last_read_time_ = 0;
  AnalogReader reader_;
};

class PotClass : public BasePotClass {
public:
  const char* name() override { return "BasePotClass"; }
  PotClass(int pin, AnalogReceiver* receiver) : BasePotClass(pin), receiver_(receiver) {}

  void HaveResult() override {
    receiver_->Set(reading_);
  }
  
  AnalogReceiver* receiver_;
};

// For 360 degree dual analog rotary pot.
class DualPotClass : public Looper  {
public:
  const char* name() override { return "DualPotClass"; }
  DualPotClass(int pin1, int pin2, AnalogReceiver* receiver) : a_(pin1), b_(pin2), receiver_(receiver) {}
  void Loop() override {
    while (millis() == last_read_) return;
    last_read_ = millis();
    float angle = atan2f(sinf((a_.reading_ - 0.5) * M_PI),
			 sinf((b_.reading_ - 0.5) * M_PI));
    angle /= M_PI * 2.0;
    if (angle < 0) angle += 1.0;
    receiver_->Set(angle);
  }
private:
  uint32_t last_read_ = millis();
  BasePotClass a_, b_;
  AnalogReceiver* receiver_;
};

#endif
