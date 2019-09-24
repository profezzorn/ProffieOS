#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

template<int PIN>
class IRReceiver : public Looper {
public:
  IRReceiver() {
    pinMode(PIN, INPUT_PULLUP);
//    pinMode(blade4Pin, OUTPUT);
    attachInterrupt(PIN, &IRReceiver::sample, CHANGE);
  }
  ~IRReceiver() {
    detachInterrupt(PIN);
  }
  const char* name() override { return "IRReceiver"; }

private:
  struct IREvent {
    bool high;
    uint32_t duration;
  };

  // Called from interrupt, or with noInterrupts()
  static void generateEvent(bool state, uint32_t now) {
    uint32_t duration = now - current_state_micros;
    volatile IREvent* buf = &buffer[(head + 1) & 0xf];
    buf->high = !current_state;
    buf->duration = duration;
    head++;
    current_state = state;
    current_state_micros = now;
  }

  // Interrupt handler.
  static void sample() {
    bool state = digitalRead(PIN);
    if (state != current_state) {
      generateEvent(state, micros());
//      digitalWrite(blade4Pin, state);
    }
  }

  void Loop() override {
    // If it's been a while since we generated an event,
    // generate one. This helps decoders that need the last
    // "low" event in order to finish decoding.
    if (micros() - current_state_micros > 20000) {
      noInterrupts();
      uint32_t now = micros();
      if (now - current_state_micros > 20000)
	generateEvent(digitalRead(PIN), now);
      interrupts();
    }
    while (tail != head) {
      bool high = buffer[tail & 0xf].high;
      uint32_t duration = buffer[tail & 0xf].duration;
      tail++;
      IRDecoder::DoSignal(high, duration);
    }
  }

  static volatile uint8_t head;
  static volatile uint8_t tail;
  static volatile IREvent buffer[16];
  static volatile bool current_state;
  static volatile uint32_t current_state_micros;
};

template<int PIN> volatile uint8_t IRReceiver<PIN>::head = 0;
template<int PIN> volatile uint8_t IRReceiver<PIN>::tail = 0;
template<int PIN> volatile typename IRReceiver<PIN>::IREvent IRReceiver<PIN>::buffer[16];
template<int PIN> volatile bool IRReceiver<PIN>::current_state = false;
template<int PIN> volatile uint32_t IRReceiver<PIN>::current_state_micros = 0;

#endif
