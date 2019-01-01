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
  // TODO: activate/deactivate aren't required anymore since
  // styles are now created with new, so constructors/destructors
  // should be used instead.
  virtual void activate() {}
  virtual void deactivate() {}

  // Called to update the blade.
  // Blade is expected to call blade->set, blade->set_overdrive and
  // blade->allow_disable to do set all the LEDs to the right value.
  virtual void run(BladeBase* blade) = 0;

  // If this returns true, this blade style has no on/off states, so
  // we disabllow the saber from turning on. Mostly used for charging
  // styles.
  virtual bool NoOnOff() { return false; }
};

class StyleFactory {
public:
  virtual BladeStyle* make() = 0;
};

template<class STYLE>
class StyleFactoryImpl : public StyleFactory {
  BladeStyle* make() override {
    return new STYLE();
  }
};

#endif
