#ifndef COMMON_STATUS_LED_H
#define COMMON_STATUS_LED_H

#ifndef NO_STATUS_LED
#if PROFFIEBOARD_VERSION - 0 >= 3

#include "looper.h"
#include "../blades/pwm_pin.h"

class StatusLED : public Looper, public CommandParser {
public:
  const char* name() override { return "StatusLED"; }
  StatusLED() {}
  void Setup() {
    pinMode(chargeDetectPin, INPUT_PULLUP);
  }

  // information:
  // CHARGING????
  // loop speed
  // on/off
  // motion on/off
  // usb connected?
  // errors
  // button presses
  
  // speed
  // intensity
  // waveform

  enum BlinkErrors {
    LOW_BATTERY = 1,
    SD_CARD_NOT_FOUND = 2,
    ERROR_IN_BLADE_ARRAY = 3,
  };

  void Loop() override {
    uint32_t now_millis = millis();
    if (now_millis - last_millis_ < 5) return;
    last_millis_ = now_millis;

    bool ignited = SaberBase::IsOn();
    bool usb_on = USBD_Connected();
    bool motion = SaberBase::MotionRequested();
    bool charging = !digitalRead(chargeDetectPin);

    bool should_be_active = ignited || usb_on || motion || blinks_ || charging;
    if (!should_be_active) {
      if (active_) {
	pin_.Deactivate();
	active_ = false;
      }
      return;
    }

    if (!active_) {
      pin_.Activate();
      active_ = true;
    }

    if (blinks_) {
      uint32_t t = now_millis - blink_start_;
      int b = t / 500;
      if (b < blinks_) {
	pin_.set( (t % 500 > 250) ? 32768 : 0);
	return;
      }
      if (b < blinks_ + 3) {
	pin_.set(0);
	return;
      }
      if (repeat_) {
	blink_start_ = millis();
	return;
      }
      blinks_ = 0;
      pos_ = 0.0;
    }
      
    uint32_t now_micros = micros();
    uint32_t delta_micros = now_micros - last_micros_;
    last_micros_ = now_micros;

    float loops_per_second = hf_loop_counter.LoopsPerSecond() + 1.0;
    float speed = 500000 / loops_per_second;
    // STDOUT << "LPS:" << loops_per_second << " speed=" << speed << "\n";
    float min_speed = 50;   // 10000 loops per second
    float max_speed = 250;  // 1000 loops per second
    if (speed < min_speed) speed = min_speed;
    if (speed > max_speed) speed = max_speed;

    pos_ = fract(pos_ + speed * delta_micros * 0.000001 / 60.0);

    float v;
    if (usb_on) {
      v = pos_;
    } else {
      v = pos_ * 2.0;
      if (pos_ > 0.5) v = 2.0 - v;
    }
    v *= ignited ? 32767 : 4096;

    if (charging && (now_millis & 2047) < 100) {
      v = 32767;
    }

    pin_.set((int)v);
  }

  void Blink(int N, bool repeat) {
    blinks_ = N;
    repeat_ = repeat;
    blink_start_ = millis();
  }

  virtual bool Parse(const char* cmd, const char* arg) {
    if (!strcmp(cmd, "blink") && arg) {
      Blink(atoi(arg), false);
      return true;
    }
    return false;
  }

private:
  uint32_t blink_start_;
  int blinks_ = 0;
  bool repeat_ = false;
  uint32_t last_micros_;
  uint32_t last_millis_;
  float pos_ = 0;
  bool active_ = false;
  SimplePWMPin<statusLEDPin> pin_;
};

StatusLED status_led;

#endif // PROFFIEBOARD_VERSION >= 3
#endif // NO_STATUS_LED
#endif
