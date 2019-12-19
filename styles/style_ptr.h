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
  HandledTypeSaver() { handled_types_ = BladeBase::GetHandledTypes(); }
  bool IsHandled(BladeEffectType effect) {
    return (handled_types_ & effect) != 0;
  }
  BladeEffectType handled_types_;
};

template<class T>
class Style : public BladeStyle {
public:
  void activate() override { }

  bool HandlesColorChange() override {
    return handled_type_saver_.IsHandled(EFFECT_CHANGE);
  }

  bool IsHandled(BladeEffectType effect) override {
    return handled_type_saver_.IsHandled(effect);
  }

  void run(BladeBase* blade) override {
    if (!RunStyle(&base_, blade))
      blade->allow_disable();
    int num_leds = blade->num_leds();
    bool rotate = !HandlesColorChange() && blade->get_byteorder() != Color8::NONE;

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
  HandledTypeResetter handled_type_resetter_;
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
