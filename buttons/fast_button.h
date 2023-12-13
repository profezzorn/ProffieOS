#ifndef BUTTONS_FAST_BUTTON_H
#define BUTTONS_FAST_BUTTON_H

// Like a button, but can respond to very short events.
// No de-bouncing, only generates EVENT_CLICK_SHORT
template<int PIN>
class FastButton : public Looper {
public:
  USE_PIN_INPUT(PIN, FastButton<PIN>);

  FastButton(const char* name, enum BUTTON button) : name_(name), button_(button) {
    pinMode(PIN, INPUT_PULLUP);
    attachInterrupt(PIN, &FastButton::irq, FALLING);
  }

  const char* name() override { return name_; }

  void Loop() override {
    if (pressed) {
      pressed = false;
      prop.Event(button_, EVENT_CLICK_SHORT);
    }
  }

private:
  static void irq() { pressed = true; }
  
  static volatile bool pressed;
  const char* name_;
  enum BUTTON button_;
};

template<int PIN> volatile bool FastButton<PIN>::pressed = false;

#endif
