#ifndef BLADES_LED_INTERFACE_H
#define BLADES_LED_INTERFACE_H

// LED interface.
class LEDInterface {
public:
  // Given a color, return a the right PWM level (0-65535).
  virtual int PWM(Color16 c) = 0;

  // Same as PWM(), but ignores battery voltage.
  virtual int PWM_overdrive(Color16 c) = 0;
};

#endif
