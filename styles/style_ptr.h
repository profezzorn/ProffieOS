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

template<class T>
class Style : public StyleBase {
public:
  bool IsHandled(HandledFeature effect) override {
    return handled_type_saver_.IsHandled(effect);
  }

  void run(BladeBase* blade) override {
    if (!RunStyle(&base_, blade))
      blade->allow_disable();
    int num_leds = blade->num_leds();
    bool rotate = !IsHandled(HANDLED_FEATURE_CHANGE) && blade->get_byteorder() != Color8::NONE;

    for (int i = 0; i < num_leds; i++) {
      OverDriveColor c = base_.getColor(i);
      if (rotate)
	c.c = c.c.rotate((SaberBase::GetCurrentVariation() & 0x7fff) * 3);
      if (c.overdrive) {
         blade->set_overdrive(i, c.c);
      } else {
         blade->set(i, c.c);
      }
      if (!(i & 0xf)) Looper::DoHFLoop();
    }
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
