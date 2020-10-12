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
  
  void SB_Effect(BladeEffectType type, float location) {
    switch (type) {
      default: break;
      case EFFECT_LOCKUP_BEGIN:
	switch (SaberBase::Lockup()) {
	  case LOCKUP_DRAG:
	    type = EFFECT_DRAG_BEGIN;
	  case LOCKUP_NORMAL:
	    break;
	  default: return;
	}
	break;
      case EFFECT_LOCKUP_END:
	switch (SaberBase::Lockup()) {
	  case LOCKUP_DRAG:
	    type = EFFECT_DRAG_END;
	  case LOCKUP_NORMAL:
	    break;
	  default: return;
	}
    }
    for (size_t i = NELEM(effects_) - 1; i; i--) {
      effects_[i] = effects_[i-1];
    }
    effects_[0].type = type;
    effects_[0].start_micros = micros();
    effects_[0].location = location;
    num_effects_ = std::min(num_effects_ + 1, NELEM(effects_));
  }
  

  bool IsPrimary() override {
    return GetPrimaryBlade() == this;
  }

  // Shared Effects
  void SB_PreOn(float* delay) override {
    SB_Effect(EFFECT_PREON, 0);
  }

  void SB_On() override {
    SB_Effect(EFFECT_IGNITION, 0);
  }

  void SB_Off(OffType off_type) override {
    switch (off_type) {
      case OFF_BLAST:
	SB_Effect(EFFECT_BLAST, (200 + random(700)) / 1000.0f);
	break;
      case OFF_NORMAL:
	SB_Effect(EFFECT_RETRACTION, 0);
        break;
      case OFF_IDLE:
	// do nothing
	break;
    }
  }
  
protected:
  BladeStyle *current_style_ = nullptr;
private:
  size_t num_effects_ = 0;
  BladeEffect effects_[4];
};

#endif
