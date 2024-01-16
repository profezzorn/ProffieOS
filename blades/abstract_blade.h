#ifndef BLADES_ABSTRACT_BLADE_H
#define BLADES_ABSTRACT_BLADE_H
#include "blade_base.h"

BladeBase* GetPrimaryBlade();
int GetBladeNumber(BladeBase *blade);

class AbstractBlade : public BladeBase, public SaberBase {
public:
  AbstractBlade() : SaberBase(NOLINK) {}
  void Activate(int blade_number) override {
    blade_number_ = blade_number;
    SaberBase::Link(this);
  }
  void Deactivate() override {
    SaberBase::Unlink(this);
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
    }
    current_style_ = nullptr;
    return ret;
  }
  
  BladeStyle* current_style() const override {
    return current_style_;
  }

  int GetBladeNumber() const override {
    return blade_number_;
  }

  bool CheckBlade(EffectLocation location) override {
    extern bool PRINT_CHECK_BLADE;
    if (PRINT_CHECK_BLADE) 
      STDERR << "CheckBlade " << GetBladeNumber() << " @ " << location << " = " << location.on_blade(GetBladeNumber()) << "\n";
    return location.on_blade(GetBladeNumber());
  }

protected:
  BladeStyle *current_style_ = nullptr;
private:
  int blade_number_;
};

#ifdef BLADE_ID_SCAN_MILLIS
bool ScanBladeIdNow();
#endif  

#endif
