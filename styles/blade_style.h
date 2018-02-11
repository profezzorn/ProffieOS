#ifndef STYLES_BLADE_STYLE_H
#define STYLES_BLADE_STYLE_H

class BladeBase;

// Base class for blade styles.
// Blade styles are responsible for the colors and patterns
// of the colors of the blade. Each time run() is called, the
// BladeStyle shouldl call blade->set() to update the color
// of all the LEDs in the blade.
class BladeStyle {
public:
  virtual ~BladeStyle() {}
  virtual void activate() {}
  virtual void deactivate() {}
  virtual void run(BladeBase* blade) = 0;
  virtual bool NoOnOff() { return false; }
};

#endif
