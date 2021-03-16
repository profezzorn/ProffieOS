#ifndef STYLES_STYLE_PTR_H
#define STYLES_STYLE_PTR_H

#include "blade_style.h"

// Usage: StylePtr<BLADE>
// BLADE: COLOR
// return value: suitable for preset array
// Most blade styls are created by taking a blade style template and wrapping it
// this class, which implements the BladeStyle interface. We do this so that the
// getColor calls will be inlined in this loop for speed.

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
private:
  T base_;
  HandledTypeSaver handled_type_saver_;
};

// Get a pointer to class.
template<class STYLE>
StyleAllocator StylePtr() {
  static StyleFactoryImpl<Style<STYLE> > factory;
  return &factory;
};


#endif
