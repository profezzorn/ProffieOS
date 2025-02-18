#ifndef BUTTONS_FAST_BUTTON_H
#define BUTTONS_FAST_BUTTON_H

template<int PIN>
class FastButtonBase : public Looper {
public:
  USE_PIN_INPUT(PIN, FastButtonBase<PIN>);
  FastButtonBase(const char* name, enum BUTTON button) : name_(name), button_(button) {}
  const char* name() override { return name_; }

  void Loop() override {
    if (pressed) {
      pressed = false;
      prop.Event(button_, EVENT_CLICK_SHORT);
    }
  }
  
protected:
  static volatile bool pressed;
  static void irq() { pressed = true; }
  
  const char* name_;
  enum BUTTON button_;
};

template<int PIN> volatile bool FastButtonBase<PIN>::pressed = false;


// Like a button, but can respond to very short events.
// No de-bouncing, only generates EVENT_CLICK_SHORT
template<int PIN>
class FastButton : public FastButtonBase<PIN> {
public:
  FastButton(const char* name, enum BUTTON button) : FastButtonBase<PIN>(name, button) {
    pinMode(PIN, INPUT_PULLUP);
    attachInterrupt(PIN, &FastButtonBase<PIN>::irq, FALLING);
  }
};

// Like a FastButton, but reacts on rising edge.
template<int PIN>
class FastPullDownButton : public FastButtonBase<PIN> {
public:
  FastPullDownButton(const char* name, enum BUTTON button) : FastButtonBase<PIN>(name, button) {
    pinMode(PIN, INPUT_PULLDOWN);
    attachInterrupt(PIN, &FastButtonBase<PIN>::irq, RISING);
  }
};

#endif
