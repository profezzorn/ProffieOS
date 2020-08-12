#ifndef BLADES_ABSTRACT_BLADE_H
#define BLADES_ABSTRACT_BLADE_H
#include "blade_base.h"

BladeBase* GetPrimaryBlade();

class AbstractBlade : public BladeBase, public SaberBase {
public:
  AbstractBlade() : SaberBase(NOLINK) {}
  void Activate() override {
    SaberBase::Link(this);
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

  void SetStyle(BladeStyle* style) override {
    // current_style should be nullptr;
    current_style_ = style;
    if (current_style_) {
      current_style_->activate();
    }
  }

  BladeStyle* UnSetStyle() override {
    BladeStyle *ret = current_style_;
    if (ret) {
      ret->deactivate();
      num_effects_ = 0;
    }
    current_style_ = nullptr;
    return ret;
  }
  
  BladeStyle* current_style() const override {
    return current_style_;
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

  // Saber Effects
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

  // Shared Effects
  void SB_PreOn(float* delay) override {
    addEffect(EFFECT_PREON, 0);
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
      case OFF_IDLE:
	// do nothing
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

  bool IsPrimary() override {
    return GetPrimaryBlade() == this;
  }

  // Blaster Effects
  void SB_Stun() override {
    addEffect(EFFECT_STUN, 0);
  }
  void SB_Fire() override {
    addEffect(EFFECT_FIRE, 0);
  }
  void SB_ClipIn() override {
    addEffect(EFFECT_CLIP_IN, 0);
  }
  void SB_ClipOut() override {
    addEffect(EFFECT_CLIP_OUT, 0);
  }
  void SB_Reload() override {
    addEffect(EFFECT_RELOAD, 0);
  }
  void SB_Mode() override {
    addEffect(EFFECT_MODE, 0);
  }
  void SB_Range() override {
    addEffect(EFFECT_RANGE, 0);
  }
  void SB_Empty() override {
    addEffect(EFFECT_EMPTY, 0);
  }
  void SB_Full() override {
    addEffect(EFFECT_FULL, 0);
  }
  void SB_Jam() override {
    addEffect(EFFECT_JAM, 0);
  }
  void SB_UnJam() override {
    addEffect(EFFECT_UNJAM, 0);
  }
  void SB_PLIOn() override {
    addEffect(EFFECT_PLI_ON, 0);
  }
  void SB_PLIOff() override {
    addEffect(EFFECT_PLI_OFF, 0);
  }
  void SB_NewFont() override {
    addEffect(EFFECT_NEWFONT, (200 + random(700)) / 1000.0f);
  }
  void SB_Boot() override {
    addEffect(EFFECT_BOOT, (200 + random(700)) / 1000.0f);
  }

protected:
  BladeStyle *current_style_ = nullptr;
private:
  size_t num_effects_ = 0;
  BladeEffect effects_[4];
};

#endif
