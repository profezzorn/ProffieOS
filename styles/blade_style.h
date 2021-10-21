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

  virtual bool Charging() { return false; }

  virtual bool IsHandled(HandledFeature feature) = 0;
  
  virtual OverDriveColor getColor(int i) { return OverDriveColor(); }
};

class StyleFactory {
public:
  virtual BladeStyle* make() = 0;
};

template<class STYLE>
class StyleFactoryImpl : public StyleFactory {
  BladeStyle* make() override {
    STDERR << "Style RAM = " << sizeof(STYLE) << "\n";
    return new STYLE();
  }
};

enum class LayerRunResult {
  UNKNOWN,
  OPAQUE_BLACK_UNTIL_IGNITION,
  TRANSPARENT_UNTIL_IGNITION,
};

enum class FunctionRunResult {
  UNKNOWN,
  ZERO_UNTIL_IGNITION,
  ONE_UNTIL_IGNITION
};

template<class T, typename X> struct RunStyleHelper {
  static bool run(T* style, BladeBase* blade) {
    return style->run(blade);
  }
};

template<class T> struct RunStyleHelper<T, void> {
  static bool run(T* style, BladeBase* blade) {
    style->run(blade);
    return true;
  }
};

template<class T> struct RunStyleHelper<T, LayerRunResult> {
  static bool run(T* style, BladeBase* blade) {
    return style->run(blade) != LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION;
  }
};

// Helper function for running the run() function in a style and
// returning a bool.
// Since some run() functions return void, we need some template
// magic to detect which way to run the function.
template<class T>
inline bool RunStyle(T* style, BladeBase* blade) {
  return RunStyleHelper<T, decltype(style->run(blade))>::run(style, blade);
}

template<class T, typename X> struct RunLayerHelper {
  static LayerRunResult run(T* style, BladeBase* blade) {
    style->run(blade);
    return LayerRunResult::UNKNOWN;
  }
};

template<class T> struct RunLayerHelper<T, LayerRunResult> {
  static LayerRunResult run(T* style, BladeBase* blade) {
    return style->run(blade);
  }
};
  
template<class T> struct RunLayerHelper<T, bool> {
  static LayerRunResult run(T* style, BladeBase* blade) {
    return style->run(blade) ? LayerRunResult::UNKNOWN : LayerRunResult::OPAQUE_BLACK_UNTIL_IGNITION;
  }
};
  
template<class T>
inline LayerRunResult RunLayer(T* style, BladeBase* blade) {
  return RunLayerHelper<T, decltype(style->run(blade))>::run(style, blade);
};


template<class T, typename X> struct RunFunctionHelper {
  static FunctionRunResult run(T* style, BladeBase* blade) {
    return style->ThisIsAnError();
  }
};

template<class T> struct RunFunctionHelper<T, FunctionRunResult> {
  static FunctionRunResult run(T* style, BladeBase* blade) {
    return style->run(blade);
  }
};
  
template<class T> struct RunFunctionHelper<T, void> {
  static FunctionRunResult run(T* style, BladeBase* blade) {
    style->run(blade);
    return FunctionRunResult::UNKNOWN;
  }
};
  
template<class T> struct RunFunctionHelper<T, bool> {
  static FunctionRunResult run(T* style, BladeBase* blade) {
    return style->ThisIsAnError();
  }
};
  
template<class T>
inline FunctionRunResult RunFunction(T* style, BladeBase* blade) {
  return RunFunctionHelper<T, decltype(style->run(blade))>::run(style, blade);
};

#endif
