#ifndef COMMON_BLADE_CONFIG_H
#define COMMON_BLADE_CONFIG_H

struct BladeConfig {
  // Blade identifier resistor.
  int ohm;

  // Blade driver.
#define DEFINE_BLADES(N) BladeBase* blade##N;
  ONCEPERBLADE(DEFINE_BLADES);

  // Blade presets
  Preset* presets;
  size_t num_presets;
};

extern BladeConfig* current_config;

#endif
