#ifndef BLADES_DIM_BLADE_WRAPPER_H
#define BLADES_DIM_BLADE_WRAPPER_H

class DimBladeWrapper : public BladeWrapper {
public:
  DimBladeWrapper(BladeBase* blade, int fraction) {
    blade_ = blade;
    fraction_ = fraction;
  }
  void set(int led, Color16 c) override {
    return blade_->set(led, c.mix2(Color16(), fraction_));
  }
private:
  int fraction_;
};

// Reduces the brightness of the blade.
// percentage = 0 -> dark
// percentage = 100 -> no change
class BladeBase* DimBlade(float percentage, BladeBase* blade) {
  return new DimBladeWrapper(blade, (int)(percentage * 32768 / 100.0));
}

#endif
