#ifndef FUNCTIONS_RAMP_H
#define FUNCTIONS_RAMP_H

// Usage: RampF  
// Returns 0 at base and 32768 at tip.  
// Example: Mix<RampF, COLOR1, COLOR2, COLOR3>  
// This would do the same thing as Gradient<COLOR1, COLOR2, COLOR3>  

class RampF {
public:
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
  }
  int getInteger(int led) { return led * 32768 / num_leds_; }

private:
  int num_leds_;
};

#endif
