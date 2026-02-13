#ifndef STYLES_STYLE_PTR_H
#define STYLES_STYLE_PTR_H

#include "blade_style.h"

// Usage: StylePtr<BLADE>
// BLADE: COLOR
// return value: suitable for preset array
// Most blade styls are created by taking a blade style template and wrapping it
// in this class, which implements the BladeStyle interface. We do this so that
// the getColor calls will be inlined in this loop for speed.

struct HandledTypeResetter {
  HandledTypeResetter() { BladeBase::ResetHandledTypes(); }
};

struct HandledTypeSaver {
  HandledTypeSaver() { handled_features_ = BladeBase::GetHandledTypes(); }
  bool IsHandled(HandledFeature feature) {
    return (handled_features_ & feature) != 0;
  }
  HandledFeature handled_features_;
};

class StyleBase : public BladeStyle {
protected:
  void activate() override { }
  HandledTypeResetter handled_type_resetter_;
};

template<class RetType>
class StyleHelper : public StyleBase {
public:
  virtual RetType getColor2(int i) = 0;
  OverDriveColor getColor(int i) override { return getColor2(i); }

  template<bool ROTATE>
  void runloop2(BladeBase* blade) {
    int num_leds = blade->num_leds();
    int rotation = (SaberBase::GetCurrentVariation() & 0x7fff) * 3;
    for (int i = 0; i < num_leds; i++) {
      RetType c = getColor2(i);
      if (ROTATE) c.c = c.c.rotate(rotation);
      if (c.getOverdrive()) {
         blade->set_overdrive(i, c.c);
      } else {
#ifdef DYNAMIC_BLADE_DIMMING
	c.c.r = clampi32((c.c.r * SaberBase::GetCurrentDimming()) >> 14, 0, 65535);
	c.c.g = clampi32((c.c.g * SaberBase::GetCurrentDimming()) >> 14, 0, 65535);
	c.c.b = clampi32((c.c.b * SaberBase::GetCurrentDimming()) >> 14, 0, 65535);
#endif
	blade->set(i, c.c);
      }
      if (!(i & 0xf)) Looper::DoHFLoop();
    }
  }

  void runloop(BladeBase* blade) {
    bool rotate = !IsHandled(HANDLED_FEATURE_CHANGE) &&
      blade->get_byteorder() != Color8::NONE &&
      (SaberBase::GetCurrentVariation() & 0x7fff) != 0;
    if (rotate) {
      runloop2<true>(blade);
    } else {
      runloop2<false>(blade);
    }
  }
};

#include "get_arg_max.h"

template<class T>
class Style : public StyleHelper<decltype(T().getColor(0))> {
public:
  bool IsHandled(HandledFeature effect) override {
    return handled_type_saver_.IsHandled(effect);
  }

  virtual auto getColor2(int i) -> decltype(T().getColor(0)) override {
    return base_.getColor(i);
  }

  void run(BladeBase* blade) override {
    if (!RunStyle(&base_, blade))
      blade->allow_disable();
    this->runloop(blade);
  }

  int get_max_arg(int argument) override {
#define GET_ARG_MAX_HELPER(ARG) if (GetArgMax<T, ARG>::value != -1) if (argument == ARG) return GetArgMax<T, ARG>::value
#define GET_ARG_MAX_HELPER2(ARG)		\
    GET_ARG_MAX_HELPER(ARG);			\
    GET_ARG_MAX_HELPER(ARG+1);			\
    GET_ARG_MAX_HELPER(ARG+2);			\
    GET_ARG_MAX_HELPER(ARG+3);			\
    GET_ARG_MAX_HELPER(ARG+4);			\
    GET_ARG_MAX_HELPER(ARG+5);			\
    GET_ARG_MAX_HELPER(ARG+6);			\
    GET_ARG_MAX_HELPER(ARG+7);			\
    GET_ARG_MAX_HELPER(ARG+8);			\
    GET_ARG_MAX_HELPER(ARG+9);

    GET_ARG_MAX_HELPER2(0);
    GET_ARG_MAX_HELPER2(10);
    GET_ARG_MAX_HELPER2(20);
    GET_ARG_MAX_HELPER2(30);
    GET_ARG_MAX_HELPER2(40);
    GET_ARG_MAX_HELPER2(50);
    GET_ARG_MAX_HELPER2(60);
    GET_ARG_MAX_HELPER2(70);
    GET_ARG_MAX_HELPER2(80);
    GET_ARG_MAX_HELPER2(90);

    return -1;
  }

protected:
  T base_;
  HandledTypeSaver handled_type_saver_;
};

template<class T>
class ChargingStyle : public Style<T> {
public:
  void run(BladeBase* blade) override {
    RunStyle(&this->base_, blade);
    this->runloop(blade);
  }
  bool NoOnOff() override { return true; }
  bool Charging() override { return true; }
};

// Get a pointer to class.
template<class STYLE>
StyleAllocator StylePtr() {
  static StyleFactoryImpl<Style<STYLE> > factory;
  return &factory;
};

class StyleFactoryWithDefault : public StyleFactory {
public:
  StyleFactoryWithDefault(StyleFactory* allocator,
			  const char* default_arguments) :
    allocator_(allocator), default_arguments_(default_arguments) {
  }
  BladeStyle* make() override {
    DefaultArgumentParserWrapper dapw(CurrentArgParser, default_arguments_);
    CurrentArgParser = &dapw;
    BladeStyle* ret = allocator_->make();
    CurrentArgParser = dapw.argParser_;
    return ret;
  }
  
  StyleFactory* allocator_;
  const char* default_arguments_;
};

template<class STYLE>
StyleAllocator StylePtr(const char* default_arguments) {
  return new StyleFactoryWithDefault(StylePtr<STYLE>(), default_arguments);
}

// Same as StylePtr, but makes the style a "charging" style, which means
// that you can't turn it on/off, and the battery low warning is disabled.
template<class STYLE>
StyleAllocator ChargingStylePtr() {
  static StyleFactoryImpl<ChargingStyle<STYLE> > factory;
  return &factory;
}
#endif  // STYLES_STYLE_PTR_H
