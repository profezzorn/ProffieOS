#ifndef BUTTONS_TOUCHBUTTON_H
#define BUTTONS_TOUCHBUTTON_H

#include "button_base.h"

// What follows is a copy of the touch.c code from the TensyDuino core library.
// That code originally implements the touchRead() function, I have modified it
// to become a class instead. That way reading the touch sensor can be
// initiated and polled without waiting around for it.

/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows 
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if defined(__MK20DX128__) || defined(__MK20DX256__)
// These settings give approx 0.02 pF sensitivity and 1200 pF range
// Lower current, higher number of scans, and higher prescaler
// increase sensitivity, but the trade-off is longer measurement
// time and decreased range.
#define CURRENT   2 // 0 to 15 - current to use, value is 2*(current+1)
#define NSCAN     9 // number of times to scan, 0 to 31, value is nscan+1
#define PRESCALE  2 // prescaler, 0 to 7 - value is 2^(prescaler+1)
#define TOUCH_BUTTON_SUPPORTED
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
255, 255,  11,   5
};

#elif defined(__MK66FX1M0__)
#define CURRENT   2
#define NSCAN     9
#define PRESCALE  2
#define TOUCH_BUTTON_SUPPORTED
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255, 255, 255,  11,
 12, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

#elif defined(__MKL26Z64__)
#define NSCAN     9
#define PRESCALE  2
#define TOUCH_BUTTON_SUPPORTED
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255
};

#endif

#ifdef TOUCH_BUTTON_SUPPORTED
class TouchButton : public ButtonBase {
public:
  TouchButton(BUTTON button, int pin, int threshold, const char* name)
    : ButtonBase(name, button),
      pin_(pin),
      threshold_(threshold) {
    pinMode(pin, INPUT_PULLUP);
#ifdef ENABLE_SNOOZE
    snooze_touch.pinMode(pin, threshold);
#endif
#if defined(__MK64FX512__)
    STDOUT.println("Touch sensor not supported!\n");
#endif
    if (pin >= NUM_DIGITAL_PINS) {
      STDOUT.println("touch pin out of range");
      return;
    }
    if (pin2tsi[pin_] == 255) {
      STDOUT.println("Not a touch-capable pin!");
    } 
  }

protected:
  bool Read() override {
    return is_pushed_;
  }

  virtual uint32_t timeout() {
    return 50;
  }

  void Update(int value) {
    if (print_next_) {
      STDOUT.print("Touch ");
      STDOUT.print(name_);
      STDOUT.print(" = ");
      STDOUT.print(value);
      STDOUT.print(" (");
      STDOUT.print(min_);
      STDOUT.print(" - ");
      STDOUT.print(max_);
      STDOUT.println(")");

#ifdef SPEAK_TOUCH_VALUES
      if (talkie.Empty()) {
	talkie.Say2Digits(min_);
	talkie.Say(spTO);
	talkie.Say2Digits(max_);
      }
#endif

      print_next_ = false;
      min_ = 10000000;
      max_ = 0;
    } else {
      min_ = min(value, min_);
      max_ = max(value, max_);
    }
    is_pushed_ = value > threshold_;
  }

  void Loop() override {
    ButtonBase::Loop();
    if (monitor.ShouldPrintMultiple(Monitoring::MonitorTouch)) {
      print_next_ = true;
    }
    STATE_MACHINE_BEGIN();
    while (true) {
      // Wait until it's our turn.
      while (current_button) YIELD();
      current_button = this;

      // Initiate touch read.
      {
         int32_t ch = pin2tsi[pin_];
         *portConfigRegister(pin_) = PORT_PCR_MUX(0);
         SIM_SCGC5 |= SIM_SCGC5_TSI;

#if defined(KINETISK) && !defined(HAS_KINETIS_TSI_LITE)
         TSI0_GENCS = 0;
         TSI0_PEN = (1 << ch);
         TSI0_SCANC = TSI_SCANC_REFCHRG(3) | TSI_SCANC_EXTCHRG(CURRENT);
         TSI0_GENCS = TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_PS(PRESCALE) | TSI_GENCS_TSIEN | TSI_GENCS_SWTS;
#elif defined(KINETISL) || defined(HAS_KINETIS_TSI_LITE)
         TSI0_GENCS = TSI_GENCS_REFCHRG(4) | TSI_GENCS_EXTCHRG(3) | TSI_GENCS_PS(PRESCALE)
           | TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_TSIEN | TSI_GENCS_EOSF;
         TSI0_DATA = TSI_DATA_TSICH(ch) | TSI_DATA_SWTS;
#endif
      }
      // Wait for result to be available.
      SLEEP_MICROS(10);
      while (TSI0_GENCS & TSI_GENCS_SCNIP) YIELD();
      SLEEP_MICROS(1);

      // Read resuilt.
#if defined(KINETISK) && !defined(HAS_KINETIS_TSI_LITE)
      Update(*((volatile uint16_t *)(&TSI0_CNTR1) + (pin2tsi[pin_])));
#elif defined(KINETISL) || defined(HAS_KINETIS_TSI_LITE)
      Update(TSI0_DATA & 0xFFFF);
#endif
      // Let someone else have a turn.
      current_button = NULL;
      YIELD();
    }
    STATE_MACHINE_END();
  }

  static TouchButton *current_button;
  bool print_next_ = false;
  uint8_t pin_;
  int threshold_;
  int min_ = 100000000;
  int max_ = 0;
  bool is_pushed_ = false;

  StateMachineState state_machine_;
};

TouchButton* TouchButton::current_button = NULL;

#endif

#endif // BUTTONS_TOUCHBUTTON_H
