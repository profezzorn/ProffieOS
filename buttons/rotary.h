#ifndef BUTTONS_ROTARY_H
#define BUTTONS_ROTARY_H

class RotaryReceiver {
public:
  virtual void Update(int delta) = 0;
};

class ChangePresetRotaryReceiver : public RotaryReceiver {
public:
  void Update(int delta) override {
    if (delta == 1) prop.next_preset();
    if (delta == -1) prop.previous_preset();
  }
};

template<int howmuch=32768 / 20>
class SmoothChangeVariationRotaryReceiver : public RotaryReceiver {
public:
  void Update(int delta) override {
    SaberBase::SetVariation(0x7fff & (SaberBase::GetCurrentVariation() + howmuch * delta));
  }
};

class TickedChangeVariationRotaryReceiver : public RotaryReceiver {
public:
  void Update(int delta) override {
    SaberBase::UpdateVariation(delta);
  }
};
class PrintRotaryReceiver : public RotaryReceiver {
public:
  void Update(int delta) override {
    STDOUT << "ROTARY: " << delta << "\n";
  }
};

void do_nothing(void* context) {}

// This is generally meant for an EC11-type twist knob which
// has four steps between each detent.
template<int pin1, int pin2, int steps_per_detent>
class RotaryClass : public Looper {
public:
  RotaryClass(RotaryReceiver* receiver) : receiver_(receiver) {}

  void Setup() override {
    pinMode(pin1, INPUT_PULLUP);
    pinMode(pin2, INPUT_PULLUP);
    Setup1();
  }

  const char* name() override { return "Rotary"; }

  static void detach_interrupt(int pin) {
#ifdef PROFFIEBOARD_VERSION
    // detachInterrupt doesn't work right in arduino-proffieboard <= 2.2
    stm32l4_exti_notify(&stm32l4_exti, g_APinDescription[pin].pin, EXTI_CONTROL_DISABLE, &do_nothing, NULL);
#else
    detachInterrupt(pin);
#endif
  }

  static void adjustPhaseAndAdd(int p, int d) {
    int delta = delta_;
    switch ((delta - p) & 3) {
      case 0: break;
      case 1: delta -= 1; break;
      case 2:
	if (delta > 0) {
	delta -= 2;
      } else {
	delta += 2;
      }
      break;
      case 3: delta += 1; break;
    }
    delta += d;
    // STDOUT << "P: " << p << " D: " << d << " delta_:" << delta_ << " delta:" << delta << "\n";
    delta_ = delta;
  }
  static int Setup1() {
    detach_interrupt(pin2);
    if (digitalRead(pin1)) {
      attachInterrupt(pin1, &RotaryClass::LO1, FALLING);
      return 1;
    } else {
      attachInterrupt(pin1, &RotaryClass::HI1, RISING);
      return -1;
    }
  }
  static int Setup2() {
    detach_interrupt(pin1);
    if (digitalRead(pin2)) {
      attachInterrupt(pin2, &RotaryClass::LO2, FALLING);
      return 1;
    } else {
      attachInterrupt(pin2, &RotaryClass::HI2, RISING);
      return -1;
    }
  }

  static void LO1() { adjustPhaseAndAdd(0, Setup2()); }
  static void LO2() { adjustPhaseAndAdd(1, -Setup1()); }
  static void HI1() { adjustPhaseAndAdd(2, -Setup2()); }
  static void HI2() { adjustPhaseAndAdd(3, Setup1()); }

  void Loop() override {
    int delta = delta_;
    while (delta - pos_ >= steps_per_detent) {
      receiver_->Update(1);
      pos_ += steps_per_detent;
      // STDOUT << "POS = " << pos_ << "\n";
    }
    while (delta - pos_ <= -steps_per_detent + 1) {
      receiver_->Update(-1);
      pos_ -= steps_per_detent;
      // STDOUT << "POS = " << pos_ << "\n";
    }
  }
private:
  static volatile int8_t delta_;
  int8_t pos_ = 0;
  RotaryReceiver* receiver_;
};

template<int pin1, int pin2, int steps_per_detent> volatile int8_t RotaryClass<pin1, pin2, steps_per_detent>::delta_ = 0;
template<int pin1, int pin2, int steps_per_detent = 4> using Rotary = RotaryClass<pin1, pin2, steps_per_detent>;

#endif
