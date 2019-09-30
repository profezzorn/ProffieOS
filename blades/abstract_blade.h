#ifndef BLADES_ABSTRACT_BLADE_H
#define BLADES_ABSTRACT_BLADE_H
#include "blade_base.h"

class AbstractBlade : public BladeBase, public SaberBase {
public:
  AbstractBlade() : SaberBase(NOLINK) {}
  void Activate() override {
    SaberBase::Link(this);
    addEffect(EFFECT_BOOT, (200 + random(700)) / 1000.0f);
  }
  void Deactivate() override {
    SaberBase::Unlink(this);
  }

  size_t GetEffects(BladeEffect** blade_effects) override {
    *blade_effects = effects_;
    while (num_effects_ &&
           micros() - effects_[num_effects_-1].start_micros > 5000000) {
      num_effects_--;
    }
    return num_effects_;
  }

  virtual void SetStyle(BladeStyle* style) {
    BladeBase::SetStyle(style);
  }

  void addEffect(BladeEffectType type, float location) {
    for (size_t i = NELEM(effects_) - 1; i; i--) {
      effects_[i] = effects_[i-1];
    }
    effects_[0].type = type;
    effects_[0].start_micros = micros();
    effects_[0].location = location;
    num_effects_ = std::min(num_effects_ + 1, NELEM(effects_));
  }

  void SB_Clash() override {
    addEffect(EFFECT_CLASH, (200 + random(700)) / 1000.0f);
  }
  void SB_Stab() override {
    addEffect(EFFECT_STAB, 1.0f);
  }
  void SB_Blast() override {
    addEffect(EFFECT_BLAST, (200 + random(700)) / 1000.0f);
  }
  void SB_Force() override {
    addEffect(EFFECT_FORCE, 1.0f);
  }

  void SB_On() override {
    addEffect(EFFECT_IGNITION, 0);
  }

  void SB_Off(OffType off_type) override {
    switch (off_type) {
      case OFF_BLAST:
        addEffect(EFFECT_BLAST, (200 + random(700)) / 1000.0f);
      case OFF_NORMAL:
        addEffect(EFFECT_RETRACTION, 0);
        break;
    }
  }

  void SB_BeginLockup() override {
    if (SaberBase::Lockup() == LOCKUP_NORMAL) {
      addEffect(EFFECT_LOCKUP_BEGIN, (200 + random(700)) / 1000.0f);
    }
    if (SaberBase::Lockup() == LOCKUP_DRAG) {
      addEffect(EFFECT_DRAG_BEGIN, (200 + random(700)) / 1000.0f);
    }
  }

  void SB_EndLockup() override {
    // TODO: use same location as begin
    if (SaberBase::Lockup() == LOCKUP_NORMAL) {
      addEffect(EFFECT_LOCKUP_END, (200 + random(700)) / 1000.0f);
    }
    if (SaberBase::Lockup() == LOCKUP_DRAG) {
      addEffect(EFFECT_DRAG_END, (200 + random(700)) / 1000.0f);
    }
  }

private:
  size_t num_effects_ = 0;
  BladeEffect effects_[4];
};

#endif
