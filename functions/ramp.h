#ifndef FUNCTIONS_RAMP_H
#define FUNCTIONS_RAMP_H

//Returns led as value between 0 ~ 32768

class Ramp {
public:
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
  }
  int getInteger(int led) { return led * 32768 / num_leds_; }

private:
  int num_leds_;
};

#endif
